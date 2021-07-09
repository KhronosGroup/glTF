# PANDA3D\_physics\_collision\_shapes

## Contributors

* Mitchell Stokes, Panda3D

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension extends nodes to include information necessary to construct collision shapes for a physics engine (e.g., [Bullet](https://bulletphysics.org/)).

## glTF Schema Updates

A `PANDA3D_physics_collision_shapes` object is added to the extensions list of any `node` for which collision shapes should be built.
The properties available are listed in the table below.

**PANDA3D\_physics\_collision\_shapes Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**shapes**|`array`|Collision shape information to represent the node|Yes|
|**groups**|`integer`|A 32-bit bit field representing the node's group membership|No, default: `1`|
|**masks**|`integer`|A 32-bit bit field representing what groups the node can collide with|No, default: `1`|
|**intangible**|`boolean`|Whether or not collision responses should be generated instead of just registered for this node|No, default: `false`|


**shape Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**type**|`enum`|The type of shape used (available options listed below)|No, default: `BOX`|
|**boundingBox**|`array`|The dimensions of the local (i.e., does not include the node's transform) bounding box of the collision shape centered on the origin|Yes|
|**primaryAxis**|`string`|The axis to use for the height of the collision shape (only applies to `CAPSULE`, `CYLINDER`, and `CONE`)|No, default: `Y`|
|**mesh**|integer|The ID of the mesh to use for `CONVEX_HULL` and `MESH` collision shapes|No, default: `node's mesh` if it exists, otherwise use `BOX` shape|
|**offsetMatrix**|`array`|A 4x4 transform matrix applied to the collision shape in addition to the node's transform|No, default: `[ 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 ]`|
|**offsetRotation**|`array`|A rotation offset (as a quaternion) applied to the physics shape in addition to the node's rotation|No, default: `[0.0, 0.0, 0.0, 1.0]`|
|**offsetScale**|`array`|A non-uniform scale offset applied to the collision shape in addition to the node's scale|No, default: `[1.0, 1.0, 1.0]`|
|**offsetTranslation**|`array`|A translation offset applied to the collision shape in addition to the node's translation|No, default: `[0.0, 0.0, 0.0]`|

**Collision Shapes**

Below are the allowed values for `collisionShape`.
When consuming this extension, an application should construct the `collisionShape` with parameters that best fill the `boundingBox`.
The shape offset should be applied to the shape prior to the node's transform.

* `BOX`
* `SPHERE`
* `CAPSULE`
* `CYLINDER`
* `CONE`
* `CONVEX_HULL`
* `MESH`

**Collision Filtering**

When two objects collide, their collisionGroups and collisionMasks properties should be used to determine if the collision should be filtered out.
If a bitwise AND operation between the collisionGroups of one object and the collisionMasks is not zero, then they will respond to collisions between each other.

**Intangible Objects**

There are times when it is desirable to register collisions, but not respond to them.
For example, collision volumes to detect if an object has entered an area.
In this case, the `intangible` property can be set to `true` to register the collision on the volume without responding (e.g., pushing an object back).
This is also known as "ghost" objects in some physics engines.

**Example**

Below are two example nodes with physics information defined.
The first has a `CAPSULE` shape, and the second has a `MESH` shape.

```javascript
{
    "extensions": {
        "PANDA3D_physics_collision_shapes": {
            "collisionShapes" [
                {
                    "boundingBox": [
                        2.0000009536743164,
                        2.0000009536743164,
                        4.323975563049316
                    ],
                    "shapeType": "CAPSULE",
                    "primaryAxis": "Z"
                }
            ]
        }
    },
    "mesh": 0,
    "name": "Cube",
    "rotation": [
        0.0,
        0.0,
        0.0,
        1.0
    ],
    "scale": [
        1.0,
        1.0,
        1.0
    ],
    "translation": [
        -3.725290298461914e-08,
        -2.9802322387695312e-08,
        1.1619879007339478
    ]
}
```

```javascript
{
    "extensions": {
        "PANDA3D_physics_collision_shapes": {
            "collisionShapes": [
                {
                    "boundingBox": [
                        2.0000009536743164,
                        2.0000009536743164,
                        2.0
                    ],
                    "shapeType": "MESH",
                    "primaryAxis": "Z",
                    "mesh": 0
                }
            ]
        }
    },
    "matrix": [
        1.0,
        0.0,
        0.0,
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        0.0,
        0.0,
        1.0
    ],
    "meshes": [
        0
    ],
    "name": "Cube"
}
```

### JSON Schema

* [node.PANDA3D_physics_collision_shapes.schema.json](schema/node.PANDA3D_physics_collision_shapes.schema.json)
* [node.PANDA3D_physics_collision_shapes.shape.schema.json](schema/node.PANDA3D_physics_collision_shapes.shape.schema.json)

## Other extensions

This extension is intended to replace the old BLENDER\_physics extension.
If both the PANDA3D\_physics\_collision\_shapes and the BLENDER\_physics extension are used, the collision information from PANDA3D\_physics\_collision\_shapes should be preferred over BLENDER\_physics.

## Known Implementations

* [blend2bam](https://github.com/Moguri/blend2bam/) adds this extension to glTF files exported from Blender
* [panda3d-gltf](https://github.com/Moguri/panda3d-gltf/) imports glTF objects into the Panda3D game engine and supports this extension
