# EXT\_mesh\_manifold

## Contributors

* Emmett Lalish, [@elalish](https://github.com/elalish/)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

A mesh is typically considered manifold when it topologically represents a solid object with well-defined volume, sometimes known as watertight. For the purpose of this extension, a mesh is considered manifold when it is an oriented 2-manifold triangle mesh, meaning for each halfedge (each of the three ordered vertex index pairs around a triangle) there is exactly one other halfedge with swapped vertices, and no other halfedge with the same vertices.

>NOTE: This is the same definition as used in the 3D Manufacturing Format (3MF) for maximum interoperability.

Manifoldness is critical for reliability of geometric algorithms that operate on solid objects, and so is important in CAD, FEA (Finite Element Analysis), CFD (Computational Fluid Dynamics), and more. However, GPU-friendly formats like glTF with indexed triangles lose manifoldness data because anywhere vertex properties change at an edge (e.g. change of materials, UV swatch, or normals at a crease) the entire vertex must be duplicated and referenced separately. This means the only way to determine linked edges is by merging vertices with identical positions, but this geometric check cannot losslessly recreate the topology, as some separate vertices may happen to coincide in position.

This extension allows authors to denote that the contained mesh data is in fact manifold, and adds a small amount of extra data to losslessly recover this manifold mesh by leveraging sparse accessors. 

## Extending Meshes With Manifold Data

A glTF mesh is denoted as a manifold by adding an `EXT_mesh_manifold` extension object to it and obeying the following restrictions:

- all primitives **MUST** use the `TRIANGLES` topology type;
- same attributes of different mesh primitives **MUST** reference the same accessor;
- the `indices` property **MUST** be defined for all primitives and the referenced accessors with index data **MUST** reference the same buffer view.

The required `manifoldPrimitive` property added in the extension is an alternative `primitive` with additional restrictions:
- it **MUST** use the `TRIANGLES` topology type;
- it **MUST** have only the `POSITION` `attribute`, which **MUST** reference the same position `accessor` as the other primitives use; 
- it **MUST NOT** contain a material;
- it **MUST NOT** contain morph targets, though the other primitives can;
- its `indices` property references an `accessor`, which **SHOULD** in turn reference the same `bufferView` as the indices accessors of other primitives of the same mesh;
- when its `indices` differ from the primitives' `indices`, its `indices` accessor **SHOULD** include a `sparse` property, to represent the changes compactly;
- the resulting `indices` **MUST** form an oriented 2-manifold, meaning every halfedge of every triangle matches exactly one other halfedge that has the same indices in the opposite order.

If the manifold `indices` differ from the combined primitives' `indices`, then `mergeIndices` and `mergeValues` **MUST** be included and reference accessors which are equivalent to the sparse indices and values needed to convert the primitives' `indices` into the `indices` of the `manifoldPrimitive`.

- Every index merged **MUST** have identical position values for the before and after vertices referenced, so that the geometry is unchanged.
- Implementations **MUST** rely on `mergeIndices` and `mergeValues` accessors (when present) to relate the vertices of the separate primitives that are merged to form the unified manifold.
- The manifold's `indices` accessor **SHOULD** be sparse and it **SHOULD** use the same buffer views, offsets, component type, and count for its sparse properties as the `mergeIndices` and `mergeValues` accessors to avoid binary data duplication.
- If the mesh contains only primitives that are already disjoint and independently manifold, then no merge accessors or sparsity are needed.

An example is given below, representing a portion of the included sample's JSON. This object has two materials/primitives with a single shared vertex attribute array. The triangle indices are also a single shared array, partitioned into separate primitive accessors. 

```json
"meshes": [
    {
      "primitives": [
        {
          "attributes": {
            "POSITION": 3,
            "TEXCOORD_0": 4
          },
          "mode": 4,
          "material": 0,
          "indices": 5
        },
        {
          "attributes": {
            "POSITION": 3,
            "TEXCOORD_0": 4
          },
          "mode": 4,
          "material": 1,
          "indices": 6
        }
      ],
      "extensions": {
        "EXT_mesh_manifold": {
          "manifoldPrimitive": {
            "indices": 2,
            "mode": 4,
            "attributes": {
              "POSITION": 3
            }
          },
          "mergeIndices": 0,
          "mergeValues": 1
        }
      }
    }
  ],
"accessors": [
    {
      "name": "sparse indices",
      "type": "SCALAR",
      "componentType": 5125,
      "count": 1728,
      "bufferView": 2,
      "byteOffset": 0
    },
    {
      "name": "sparse values",
      "type": "SCALAR",
      "componentType": 5125,
      "count": 1728,
      "bufferView": 2,
      "byteOffset": 6912
    },
    {
      "name": "manifold indices",
      "type": "SCALAR",
      "componentType": 5125,
      "count": 10320,
      "bufferView": 3,
      "byteOffset": 0,
      "sparse": {
        "count": 1728,
        "indices": {
          "bufferView": 2,
          "byteOffset": 0,
          "componentType": 5125
        },
        "values": {
          "bufferView": 2,
          "byteOffset": 6912
        }
      }
    },
    {
      "name": "position",
      "type": "VEC3",
      "componentType": 5126,
      "count": 2292,
      "max": [
        50,
        50,
        50
      ],
      "min": [
        -50,
        -50,
        -50
      ],
      "bufferView": 4,
      "byteOffset": 0
    },
    {
      "name": "tex-coord",
      "type": "VEC2",
      "componentType": 5126,
      "count": 2292,
      "bufferView": 4,
      "byteOffset": 12
    },
    {
      "name": "first primitive indices",
      "type": "SCALAR",
      "componentType": 5125,
      "count": 1728,
      "bufferView": 3,
      "byteOffset": 0
    },
    {
      "name": "second primitive indices",
      "type": "SCALAR",
      "componentType": 5125,
      "count": 8592,
      "bufferView": 3,
      "byteOffset": 6912
    }
  ],
```

## Usage

While this extension places additional restrictions on how a mesh is stored, it is still a valid glTF 2.0 and thus will render properly even on software that does not implement support for this extension. Software that needs only manifold geometry rather than a rendering mesh **SHOULD** ignore the original mesh primitives and use the `manifoldPrimitive` instead.

In practice, the amount of extra data to represent manifoldness is very small since the sparse accessors only need to be defined along the boundaries between discontinuous vertex properties. The properties themselves are untouched.

## References

- Open source TypeScript I/O library available [here](https://github.com/elalish/manifold/blob/master/bindings/wasm/examples/manifold-gltf.ts).
- Add this extension automatically to a mostly-manifold glTF [here](https://manifoldcad.org/make-manifold).
- Build manifold models and download them as GLB with this extension at [ManifoldCAD.org](https://manifoldcad.org/).