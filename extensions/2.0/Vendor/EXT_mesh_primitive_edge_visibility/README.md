<!--
Copyright 2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# EXT_mesh_primitive_edge_visibility

## Contributors

* Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Depends on `BENTLEY_primitive_restart` if primitive restart is used.

## Overview

3D modeling and computer-aided drafting environments like SketchUp, MicroStation, and Revit provide non-photorealistic visualizations that render 3D objects with their edges visible. The edges can improve the readability of complex models and convey semantics of the underlying topology. The `EXT_mesh_primitive_edge_visibility` extension augments a triangle mesh primitive with sufficient information to enable engines to produce such visualizations. The information is encoded in a compact form to minimize the impact on the size of the glTF asset.

Figure 1 illustrates a typical rendering of a cylinder with its edges. The width of the edges has been exaggerated for emphasis.

<figure>
<img src="./figures/visible-edges.png"/>
<figcaption><em><b>Figure 1</b> Cylinder mesh with visible edges</em></figcaption>
</figure>

This image shows both of the two types of edges described by `EXT_mesh_primitive_edge_visibility`.
- A [silhouette edge](https://en.wikipedia.org/wiki/Silhouette_edge) is any edge separating a front-facing triangle from a back-facing triangle. In Figure 1, one silhouette is visible along each of the curved left and right sides of the cylinder. Silhouette edges are *conditionally visible* - their visibility is determined at display time based on the orientations of the adjacent triangles relative to the camera. In Figure 2, each of the vertical edges encircling the cylinder represents a potential silhouette edge.
- A hard edge is any edge attached to only a single triangle, or any edge between two logical faces of the 3D object. In Figure 1, hard edges are visible around the perimeters of the cylinder's circular end caps. Hard edges are *always visible* regardless of the camera direction.

## Shortcomings of Existing Techniques

Various techniques can be applied to a glTF asset to approximate the rendering in Figure 1. The approach illustrated in Figure 2 uses a wiremesh shader to render every edge of every triangle. While the interior edges can be useful for visualizing the structure of the triangle mesh, they obscure the underlying cylinder topology.

<figure>
<img src="./figures/wiremesh.png"/>
<figcaption><em><b>Figure 2</b> Triangle edges drawn using a wiremesh technique</em></figcaption>
</figure>

Screen-space techniques (e.g., "toon" or "outline" shaders) can be applied to add edges during image post-processing. Because they operate on pixels rather than geometry, such techniques can only approximate the actual edges. The technique used in Figure 3, for example, fails to reconstruct the edges along the upper end cap of the cylinder where they fall inside the cylinder's volume projected onto the image plane.

<figure>
<img src="./figures/outline.png"/>
<figcaption><em><b>Figure 3</b> Edges drawn using a screen-space technique</em></figcaption>
</figure>

The hard edges could be encoded explicitly into the glTF asset as additional primitives to be drawn together with the triangle mesh. However, this will generally produce a "stippling" effect where the edges and triangles collide in the depth buffer, as shown in Figure 4.

<figure>
<img src="./figures/depth-fighting.png"/>
<figcaption><em><b>Figure 4</b> Edges drawn as separate primitives</em></figcaption>
</figure>

The [CESIUM_primitive_outline](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/CESIUM_primitive_outline) extension provides an additional index buffer describing each hard edge as a line segment (a pair of indices into the triangle mesh's list of vertices). The extension leaves the details of how to render the edges up to the engine, requiring only that they render without depth-fighting. This approach satisfies the use case for which it was intended - displaying the edges of boxy, low-resolution buildings - but suffers some limitations:
- The surface geometry must be represented as indexed triangles.
- It only supports hard edges, not silhouettes.
- The representation of the edges as pairs of vertex indices can significantly increase the size of the glTF asset.
- The edges cannot specify their own materials.

<figure>
<img src="./figures/hard-edges.png"/>
<figcaption><em><b>Figure 5</b> Hard edges encoded using CESIUM_primitive_outline</em></figcaption>
</figure>

## glTF Schema Updates

The `EXT_mesh_primitive_edge_visibility` extension is applied to a mesh primitive of topology type 4 (triangles), 5 (triangle strip), or 6 (triangle fan) that uses indexed or non-indexed geometry. It provides the information required for engines to produce a rendering of the primitive's edges. Engines are not required to *always* render the edges - they may, for example, permit the user to toggle edge display on and off. When rendering the edges, engines may use any technique that meets the requirements specified below.

### Edge Visibility

The visibility of a single edge is specified using 2 bits as one of the following visibility values:
- 0: Hidden edge - the edge should never be drawn.
- 1: Silhouette edge - the edge should be drawn only in silhouette (i.e., when separating a front-facing triangle from a back-facing triangle).
- 2: Hard edge - the edge should always be drawn.
- 3: Hard edge encoded in `primitives` - a representation of the edge is included in the extension's `primitives` property. The edge should always be drawn, either by drawing the `primitives` array or by drawing it like an edge with visibility value `2` (never both).

The extension's `visibility` property specifies the index of an accessor of `SCALAR` type and component type 5121 (unsigned byte) that encodes as a bitfield the visibility of every edge of every triangle in the mesh. The ordering of triangles and vertices is as described by [Section 3.7.2.1](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#meshes-overview) of the glTF 2.0 specification. For each triangle `(v0, v1, v2)`, the bitfield encodes three visibility values for the edges `(v0:v1, v1:v2, v2:v0)` in that order. Therefore, the accessor's `count` **MUST** be `6 * N / 8` rounded up to the nearest whole number, where `N` is the number of triangles in the primitive. Any unused bits in the last byte **MUST** be set to zero.

The visibility of a given edge shared by a given pair of triangles **MUST** be encoded as a non-zero value no more than once. All other occurrences of the same edge in the bitfield **MUST** be encoded as zero, to prevent engines from either producing redundant geometry for shared edges or having to manually detect and account for such redundancies.

Engines **MUST** render all edges according to their specified visibility values. The edges **MUST** draw in front of their corresponding triangles with no depth-fighting. Engines may choose a material with which to draw the edges - e.g., they could apply the primitive's material to the edges, or apply some uniform material to the edges of all primitives in the scene, or some other consistent option. If engines choose to render the edges specified by the `primitives` property, then they **MUST** render those primitives according to the glTF 2.0 specification (e.g., using the specified material) and they **MUST NOT** also produce their own rendering of the edges with visibility value `3`.

#### Example

Consider the simple example of a pair of adjacent triangles described by the index list `[0,1,2, 0,2,3]`:
```
  0______3
  | ⟍    |
  |   ⟍  |
  |_____⟍|
  1      2
```

With 2 bits per edge, encoding the visibility of two triangles requires 12 bits. So the visibility buffer must be 2 bytes long, with 4 bits of the second byte going unused. The diagram below shows which edge's visibility will be encoded into each pair of bits.

```
Byte    0                 1
       ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐   
       │0:2│2:0│1:2│0:1│ │   │   │3:0│2:3│   
       └───┴───┴───┴───┘ └───┴───┴───┴───┘   
Bit        6   4   2   0     14  12  10  8
```

Assume that the vertical edges `0:1` and `2:3` are hard edges, the horizontal edges `3:0` and `1:2` are hidden edges, and the shared diagonal edge `0:2` is a silhouette edge. Then the corresponding visibility values would be `[2,0,1, 1,2,0]`. However, we must ensure that the visibility of the shared edge is encoded only once, so after replacing one of the redundant `1`s the visibility values are `[2,0,1, 0,2,0]`. Encoding the edge visibility produces the sequence of bytes `[18, 2]`, as illustrated below.

```
Byte    0                 1
       ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐   
Binary │ 00│ 01│ 00│ 10│ │ 00│ 00│ 00│ 10│
       └───┴───┴───┴───┘ └───┴───┴───┴───┘   
Decimal              18                 2
```

### Edge Primitives

The extension's `primitives` property optionally provides an array of primitives representing all of the hard edges encoded with visibility value `3` in the `visibility` property. Engines should render these primitives directly, in which case they **MUST** do so without depth-fighting and **MUST NOT** also produce their own rendering of these edges. The primary use case is for edges that should be drawn using a different material than the corresponding surface - for example, a red outline drawn around a filled green shape.

The `primitives` property **MUST** be defined *if and only if* at least one edge is encoded with visibility value `3` in `visibility`.

### Silhouette Normals

The extension's `silhouetteNormals` property specifies the index of an accessor of `SCALAR` type and component type 5123 (unsigned short) providing normal vectors used to determine the visibility of silhouette edges at display time. For each edge encoded as a silhouette (visibility value `1`) in `visibility`, the silhouette normals buffer provides the two outward-facing normal vectors of the pair of triangles sharing the edge. Each normal vector is compressed into 16 bits using the "oct16" encoding described [here](https://jcgt.org/published/0003/02/01/). The ordering of the normal vector pairs corresponds to the ordering of the edges in `visibility`; that is, the first pair of normals corresponds to the first edge encoded with visibility `1`, the second pair to the second occurrence of visibility `1`; and so on. The accessor's `count` **MUST** be twice the number of edges encoded with visibility value `1`.

Engines should render a silhouette edge unless both adjacent triangles are front-facing or both are back-facing, as determined by their normal vectors.

The `silhouetteNormals` property **MUST** be defined *if and only if* at least one edge is encoded with visibility value `1` in `visibility`.

### Extensions

The `BENTLEY_primitive_restart` extension *may* be applied to the `EXT_mesh_primitive_edge_visibility` extension object, in which case it applies to the `primitives` property in the same way that it would apply to `mesh.primitives`.

## JSON Schema

[./schema/primitive.EXT_mesh_primitive_edge_visibility.schema.json](primitive.EXT_mesh_primitive_edge_visibility.schema.json)

## Implementation Notes

Example routines for encoding and decoding of oct16 normal vectors:

- [GLSL](https://jcgt.org/published/0003/02/01/) (Section 3.1)
- [JavaScript](https://github.com/CesiumGS/cesium/blob/dd9c25b652460e8cd77ecd29fbbf7cc44d8b62a2/packages/engine/Source/Core/AttributeCompression.js#L77)
- [C++](https://github.com/iTwin/imodel-native/blob/4f0a5008dba61124b490a7506a6cd82ce1b8f148/iModelCore/iModelPlatform/PublicAPI/DgnPlatform/Render.h#L2012)

The [pull request](https://github.com/iTwin/itwinjs-core/pull/5581) that informed the design of this extension provides [an iterator](https://github.com/iTwin/itwinjs-core/blob/03b760e1e91bde5221aa7370ea45c52f966e3368/core/frontend/src/common/imdl/CompactEdges.ts#L42) over the `visibility` and `silhouetteNormals` buffers.

iTwin.js [implements](https://github.com/iTwin/itwinjs-core/blob/03b760e1e91bde5221aa7370ea45c52f966e3368/core/frontend/src/internal/render/webgl/glsl/Edge.ts#L107) conditional display of silhouette edges. It draws edges in a separate pass from surfaces to [ensure](https://github.com/iTwin/itwinjs-core/blob/03b760e1e91bde5221aa7370ea45c52f966e3368/core/frontend/src/internal/render/webgl/glsl/FeatureSymbology.ts#L426) the mitigate z-fighting.
