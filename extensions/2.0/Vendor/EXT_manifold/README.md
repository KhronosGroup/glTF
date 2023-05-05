# EXT\_manifold

## Contributors

* Emmett Lalish, [@elalish](https://github.com/elalish/)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

A mesh is considered manifold when it topologically represents a solid object with well-defined volume, sometimes known as water-tight. Manifoldness has many definitions, so we are specifically using the same definition as the 3MF spec (3D Manufacturing Format) for maximum interoperability: an oriented 2-manifold triangle mesh.

Manifoldness is critical for reliability of geometric algorithms that operate on solid objects, and so is important in CAD, FEA (Finite Element Analysis), CFD (Computational Fluid Dynamics), and more. However, GPU-friendly formats like glTF with indexed triangles lose manifoldness data because anywhere vertex properties change at an edge (e.g. change of materials, UV swatch, or normals at a crease) the entire vertex must be duplicated and referenced separately. This means the only way to determine linked edges is by merging vertices with identical positions, but this geometric check cannot losslessly recreate the topology, as some separate vertices may happen to coincide in position.

This extension allows authors to denote that the contained mesh data is in fact manifold, and adds a small amount of extra data to losslessly recover this manifold mesh by leveraging sparse accessors. 

## Extending Meshes With Manifold Data

Manifoldness is implied by adding the `EXT_manifold` extension to a glTF `mesh`. The `primitives` of the `mesh` **should** use the same set of attributes and each attribute sematic **must** consistently reference the same accessor. The `indices` **must** be present and the referenced `accessor` **must** in turn reference a single `bufferView` for the set of primitives. 

The `manifoldPrimitive` added in the extension is an alternative `primitive` which **must** have only the `POSITION` `attribute`, which **must** reference the same position `accessor` as the other primitives use. The `indices` reference an `accessor` which **must** in turn reference the same `bufferView` as the other primitives' `indices`. This `accessor` **may** include a `sparse` accessor that defines which vertices are to be merged. Every index merged **must** have identical position values for the before and after vertices referenced. 

If the `indices` `accessor` is sparse, then `mergeIndices` and `mergeValues` **must** be included and reference `accessors` which are equivalent to the sparse indices and values. This is a convenience as many glTF APIs do not expose the internals of the sparse representations.

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
        "EXT_manifold": {
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

While this extension places additional restrictions on how a mesh is stored, it is still a valid glTF 2.0 and thus will render properly even on software that does not implement support for this extension. For software that needs a manifold mesh rather than a rendering mesh, the simplest path is to simply replace the set of `primitives` with the `manifoldPrimitive`, which is still a valid glTF, but is simply missing all non-position `attributes`. 

As an oriented 2-manifold, it **must** be true that after applying the sparse accessor, every edge of every triangle has exactly one paired edge that has indices in the opposite order. In the case of a `mesh` consisting of just a single manifold `primitive`, the `manifoldPrimitive` **should** be identical aside from missing the non-position `attributes`. 

In practice, the amount of extra data to represent manifoldness is very small since the sparse accessors only need to be defined along the boundaries between discontinuous vertex properties. The properties themselves are untouched.