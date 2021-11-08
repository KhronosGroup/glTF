# MAXAR_collision_geometry

## Contributors

* Erik Dahlström, Maxar, [@erikdahlstrom](https://github.com/erikdahlstrom)
* Sam Suhag, Cesium, [@sanjeetsuhag](https://github.com/sanjeetsuhag)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

In gaming and simulation engines, high resolution 3D models are often paired with decimated versions of the model that are optimized for use in rigid-body physics. These collision volumes are the union of a set of convex polyhedra. This extension to glTF enables designating meshes as collision volumes for other meshes.

## Adding Collision Meshes

The following example illustrates how a mesh can designate another mesh as its collision volume:

```json
"meshes": [
  {
    "name": "Detailed Car Mesh",
    "primitives": [
      {
        "attributes": {
          "POSITION": 0,
          "NORMAL": 1
        },
        "indices": 2
      }
    ],
    "extensions": {
      "MAXAR_collision_geometry": {
        "mesh": 1
      }
    }
  },
  {
    "name": "Box Collision Mesh",
    "primitives": [
      {
        "attributes": {
          "POSITION": 3,
        },
        "indices": 4
      }
    ]
  },
]
```

### Mesh Requirements

This extension enforces the following rules on the relationships between meshes:

1. Two or more meshes may use the same mesh as a collision mesh.
2. A mesh may not designate itself as its collision mesh.
3. A mesh with a collision mesh must not have morph targets.
4. A collision mesh must not have morph targets.
5. At runtime, the node transformation of a mesh must be applied to its collision mesh.

### Primitive Requirements

This extension enforces the following rules on the primitives inside a collision mesh:

1. Primitives in a collision mesh must have the POSITION attribute.
2. Primitives in a collision mesh must use the TRIANGLES primitive mode.
3. Each primitive inside a collision mesh must represent a convex polyhedron.

## Optional vs. Required

This extension is considered optional, meaning it should be placed in the glTF root's `extensionsUsed` list, but not in the `extensionsRequired` list.

## Schema Updates

- **glTF mesh extension JSON schema**: [mesh.MAXAR_collision_geometry.schema.json](schema/mesh.MAXAR_collision_geometry.schema.json)
