# EXT\_mesh\_gpu\_instancing

## Contributors

* John Cooke, OTOY, <mailto:john.cooke@otoy.com>
* Don McCurdy, Google, [@donmccurdy](https://github.com/donmccurdy/)
* Arseny Kapoulkine, [@zeuxcg](https://twitter.com/zeuxcg)

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is specfically designed to enable GPU instancing, rendering many copies of a single mesh at once using a small number of draw calls.  It's useful for things like trees, grass, road signs, etc.  The `TRANSLATION`, `ROTATION`, and `SCALE` attributes allow the mesh to be displayed at many different locations, with different rotations and scales. As with vertex attributes, custom instance attributes may be prefixed with an underscore (e.g. `_ID`, `_COLOR`, etc.) and used for application-specific effects.

> **Implementation Note:** While this extension stores mesh data optimized for GPU instancing, it is important to note that (1) GPU instancing and other optimizations are possible — and encouraged — even without this extension, and (2) other common meanings of the term "instancing" exist, distinct from this extension. See [Appendix: Motivation and Purpose](#appendix-motivation-and-purpose).

## Extending Nodes With Instance Attributes

Instancing is defined by adding the `EXT_mesh_gpu_instancing` extension to any glTF node that has a mesh. Instancing only applies to mesh nodes, there is no defined behavior for a node with this extension that doesn't also have a mesh. Applying to nodes rather than meshes allows the same mesh to be used by several nodes, instanced or otherwise. The attributes section contains accessor ids for the `TRANSLATION`, `ROTATION`, and `SCALE` attribute buffers, all of which are optional. The attributes specify an object space transform that should be multipled by the node's world transform in the shader to produce the final world transform for the instance. For example, the following defines some instancing attributes to a node with mesh.

```json
{
    "nodes": [
        {
            "mesh": 0,
            "name": "teapot",
            "extensions": {
                "EXT_mesh_gpu_instancing": {
                    "attributes": {
                        "TRANSLATION": 0,
                        "ROTATION": 1,
                        "SCALE": 2,
                        "_ID" : 3
                    },
                }
            }
        }
    ]
}
```

Valid accessor type and component type for each attribute semantic property are defined below.

|Name|Accessor Type|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`"TRANSLATION"`|`"VEC3"`|`5126`&nbsp;(FLOAT)|XYZ translation vector|
|`"ROTATION"`|`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized|XYZW rotation quaternion|
|`"SCALE"`|`"VEC3"`|`5126`&nbsp;(FLOAT)|XYZ scale vector|

All attribute accessors in a given node **must** have the same `count`.

> **Implementation Note:** When instancing is used on the node, the non-instanced version of the mesh should not be rendered.

Because the extension does not provide a way to specify a non-instanced fallback, files that use the extension **should** specify it in `extensionsRequired` array.

## Transformation Order

When using instancing, the instance transform matrix is constructed by multiplying translation (if present), rotation (if present), and scale in the same order as they are for nodes:

    InstanceXform = Translation * Rotation * Scale

The node's world transform matrix is constructed by multiplying the node's local transform matrix from the root of the tree up until (and including) the node itself:

    NodeWorldXform = RootXform * ... * ParentLocalXform * NodeLocalXform

The resulting transforms are applied to mesh positions/normals/etc., with instance transformation applied first:

    WorldPosition = NodeWorldXform * InstanceXform * VertexPosition

> Note: the construction above assumes column vectors; for row vectors, the matrix multiplication order should be reversed

## Appendix: Motivation and Purpose

*This section is non-normative.*

### Definitions

The term "instancing" has come to mean different things within different parts of the graphics ecosystem. In authoring tools and data formats, it may refer to reuse of any resource for efficiency, whether that resource represents a GPU draw call or not. We will refer to this technique as *data instancing*, and it is unrelated to the `EXT_mesh_gpu_instancing` extension described by this document. On its own, the glTF 2.0 specification provides flexible ways to use data instancing and avoid duplication of meshes, textures, and other low-level resources.

In graphics APIs, the term more commonly refers to drawing a single mesh many times with a single draw call, and a single shader program. Differing vector or scalar parameters (color, position) may be used as inputs to the shader program at each instance.  We will refer to this technique as *GPU instancing*, and it is the focus of the `EXT_mesh_gpu_instancing` extension.

### Motivation

GPU instancing is a common optimization technique, and may be enabled by client implementations with or without this extension — the technique is compatible with, and independent of, the glTF 2.0 specification. What glTF 2.0 does not provide, on its own, is an efficient means to transmit very large multiples of mesh data intended for instancing, in a format that indicates that the data has been processed for this purpose. That additional structure is useful for more advanced effects such as fur, grass, dense trees, and buildings.

One tradeoff that any engine must make when considering instancing is in regard to frustum culling. Instances are by definition drawn as a batch, and may (in the worst case) perform worse as a result than if they had been drawn individually, where only a few instances are actually within the visible area. For this reason, the best use of instancing is often an application-specific optimization — for a simple viewer, as single large batch may be appropriate. For a large game world, instances should be grouped into colocated cells (each represented as a node extended with `EXT_mesh_gpu_instancing`) and culled by node bounds. This extension aims to provide the flexibility to optimize data for these different use cases, without prescribing a single solution.

The technique is useful to improve runtime performance, transmission size, and loading time, but it may reduce portability across tools and lose some information (like node hierarchies) from the original asset. For archival purposes, the *data instancing* techniques enabled by the core glTF 2.0 specification should be used instead.

### Current Limitations

The reader may notice that several desirable features are missing from the extension, such as specifying different materials for different instances within the same batch, or animation of individual instances. These are omitted, at present, for lack of a clear mechanism to support them in a sufficient number of realtime engines. Such features can be implemented for application-specific use with custom instance attributes (such as `_ID`, `_VELOCITY`, `_END_POSITION`).  They may also be enabled by future extensions or other formats wrapping or referencing glTF 2.0 data. This limitation may be revisited if future developments provide opportunities to do so.
