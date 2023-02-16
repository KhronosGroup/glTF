# OMI_collider

## Contributors

* Aaron Franke, The Mirror Megaverse Inc.
* Robert Long, The Matrix.org Foundation
* Mauve Signweaver, Mauve Software Inc.

## Status

Open Metaverse Interoperability Group Stage 2 Proposal

## Dependencies

Written against the glTF 2.0 spec.

Can be optionally used together with the `OMI_physics_body` spec.

## Overview

Colliders can be added to glTF nodes along with information about the "type" of collider it is representing.

This extension allows specifying shapes to be used in glTF scenes for collision or triggers. Shapes should be specified on their own glTF nodes.

This extension does not describe what colliders should do within a scene, aside from whether they are a trigger or not. For a more thorough physics body specification, you need to also use the `OMI_physics_body` extension, and add `OMI_collider` nodes as children of the physics body. Multiple `OMI_collider` siblings with the same `isTrigger` setting may be treated as a single "compound collider" in game engines that support them.

### Example:

This example defines a single box collider with a size of 1 meter in all dimensions:

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensions": {
        "OMI_collider": {
            "colliders": [
                {
                    "size": [1, 1, 1],
                    "type": "box"
                }
            ]
        }
    },
    "extensionsUsed": [
        "OMI_collider"
    ],
    "nodes": [
        {
            "extensions": {
                "OMI_collider": {
                    "collider": 0
                }
            },
            "name": "BoxCollider"
        }
    ],
    "scene": 0,
    "scenes": [
        {
            "nodes": [0]
        }
    ]
}
```

More example assets can be found in the [examples/](examples/) folder. All of these examples use `OMI_collider` and not `OMI_physics_body`.

## glTF Schema Updates

This extension consists of three new data structures for defining colliders on the root glTF document and referencing them on a glTF node. The main data structure defines a collider shape and is what most of this document describes. The second data structure uses the key `"OMI_collider"` in the document-level `"extensions"` which contains a list of the main collider shape data structures. The third data structure uses the key `"OMI_collider"` in the node-level `"extensions"` which contains an index of the collider shape to use from the list document-level collider list.

The extension must also be added to the glTF's `extensionsUsed` array and because it is optional, it does not need to be added to the `extensionsRequired` array.

### Property Summary

The rest of the document, including this summary, defines the properties for the main data structure.

|               | Type        | Description                                                                | Default value        | Valid on                  |
| --------------| ----------- | -------------------------------------------------------------------------- | -------------------- | ------------------------- |
| **type**      | `string`    | The type of the collider shape as a string.                                | Required, no default | Always valid              |
| **isTrigger** | `boolean`   | If true, the shape is a trigger and is not solid to other objects.         | false                | Always valid              |
| **size**      | `number[3]` | The size of the box shape in meters.                                       | [1.0, 1.0, 1.0]      | Box                       |
| **radius**    | `number`    | The radius of the shape in meters.                                         | 0.5                  | Sphere, capsule, cylinder |
| **height**    | `number`    | The height of the shape in meters.                                         | 2.0                  | Capsule, cylinder         |
| **mesh**      | `number`    | The index of the glTF mesh in the document to use as a trimesh shape.      | -1                   | Trimesh, hull             |

### Collider Types

The `"type"` property is a lowercase string that defines what type of shape this collider is.

The selection of shapes was carefully chosen with a balance of compatibility between major game engines and containing the most commonly used shapes for easy asset creation. Colliders inherit the transform of the glTF node they are attached to. This includes rotation and translation, however it is discouraged to scale collider nodes since this can cause problems in some physics engines.

Here is a table listing the mapping between the `OMI_collider` type and the equivalent types in major game engines.

| Shape    | Unity         | Unreal        | Godot                 | Blender     | Bullet (Ammo, Panda3D, etc) |
| -------- | ------------- | ------------- | --------------------- | ----------- | --------------------------- |
| Box      | Box           | Box           | BoxShape3D            | Box         | Box Shape                   |
| Sphere   | Sphere        | Sphere        | SphereShape3D         | Sphere      | Sphere Shape                |
| Capsule  | Capsule       | Capsule       | CapsuleShape3D        | Capsule     | Capsule Shape               |
| Cylinder | Approximation | Approximation | CylinderShape3D       | Cylinder    | Cylinder Shape              |
| Hull     | Mesh (Convex) | Convex        | ConvexPolygonShape3D  | Convex Hull | Convex Shape                |
| Trimesh  | Mesh          | Mesh          | ConcavePolygonShape3D | Mesh        | Mesh Shape                  |

#### Box

Box shapes describe a cube or cuboid shape. They have `size` property which is an array of 3 numbers that describes the width, height, and depth. If the `size` property is omitted, the default size is `[1, 1, 1]`, representing a cube with a volume of one cubic meter, edges/diameters one meter long, and extents/radius of half a meter. The position of the glTF node is the center of the box shape.

#### Sphere

Sphere shapes describe a uniform "ball" shape. They have a `radius` property which is a single number. If the `radius` property is omitted, the default radius is `0.5`, representing a sphere with a radius of half a meter, a diameter of one meter. The position of the glTF node is the center of the sphere shape.

#### Capsule

Capsule shapes describe a "pill" shape. They have a `radius` and `height` property. The height is aligned with the node's local vertical axis. If you wish to align it along a different axis, rotate the glTF node. If the `radius` property is omitted, the default radius is `0.5`, and if the `height` property is omitted, the default height is `2`. The height describes the total height from bottom to top. The height of the capsule must be at least twice as much as the radius. The "mid-height" between the centers of each spherical cap end can be found with `height - radius * 2`.

#### Cylinder

Cylinder shapes describe a "tall circle" shape. They are similar in structure to capsules, they have a `radius` and `height` property. The height is aligned with the node's local vertical axis. If you wish to align it along a different axis, rotate the glTF node. If the `radius` property is omitted, the default radius is `0.5`, and if the `height` property is omitted, the default height is `2`.

The use of cylinder is discouraged if another shape would work well in its place. Cylinders are harder to calculate than boxes, spheres, and capsules. Not all game engines support cylinder shapes. Engines that do not support cylinder shapes should use an approximation, such as a convex hull roughly shaped like a cylinder. Cylinders over twice as tall as they are wide can use another approximation: a convex hull combined with an embedded capsule (to allow for smooth rolling), by copying the cylinder's values into a new capsule shape.

#### Hull

Hull shapes represent a convex hull. Being "convex" means that the shape cannot have any holes or divots. Hulls are defined with a `mesh` property with an index of a mesh in the glTF `meshes` array. The glTF mesh in the array MUST be a `trimesh` to work, and should be made of only one glTF mesh primitive (one surface). Valid hulls must contain at least one triangle, which becomes three points on the convex hull. Hulls are recommended to have at least four points so that they have 3D volume. The final hull shape should have no more than 255 points in total.

Hulls can be used to represent complex convex shapes that are not easy to represent with other primitives. If a shape can be represented with a few primitives, prefer using those primitives instead of convex hulls. Hulls are much faster than trimesh shapes.

#### Trimesh

Trimesh shapes represent a concave triangle mesh. They are defined with a `mesh` property with an index of a mesh in the glTF `meshes` array. The glTF mesh in the array MUST be a `trimesh` to work, and should be made of only one glTF mesh primitive (one surface). Valid trimesh shapes must contain at least one triangle.

Avoid using a trimesh shape for most objects, they are the slowest shapes to calculate and have several limitations. Most physics engines do not support moving trimesh shapes or calculating collisions between multiple trimesh shapes. Trimesh shapes will not work reliably with `isTrigger` or with pushing objects out due to not having an "interior" space, they only have a surface. Trimesh shapes are typically used for complex level geometry (for example, things that you can go inside of). If your shape can be represented with a combination of simpler primitives, or a convex hull, or multiple convex hulls, prefer that instead.

### Trigger

The `"isTrigger"` property defines if this shape is a trigger or collider. It is a boolean, either `true` or `false`. If omitted, the default value is `false`.

Trigger shapes are not solid to other objects and will not collide with them. Triggers can generate events when another physics body "enters" them. For example, a "goal" area which triggers whenever a ball gets thrown into it. Which event gets triggered by a trigger shape is game-specific and not defined by `OMI_collider` (if nothing happens, that is also valid).

### JSON Schema

See [schema/collider.schema.json](schema/collider.schema.json) for the main collider schema, [schema/glTF.OMI_collider.schema.json](schema/glTF.OMI_collider.schema.json) for the document-level list of colliders, and [schema/node.OMI_collider.schema.json](schema/node.OMI_collider.schema.json) for the node-level collider selection.

## Known Implementations

* Godot Engine: https://github.com/godotengine/godot/pull/69266
* Third Room Unity Exporter: https://github.com/matrix-org/thirdroom-unity-exporter/blob/main/Runtime/Scripts/OMI_collider/OMI_ColliderExtension.cs
* Third Room glTF Transform: https://github.com/matrix-org/thirdroom/blob/main/src/asset-pipeline/extensions/OMIColliderExtension.ts
* Third Room glTF Loader: https://github.com/matrix-org/thirdroom/blob/main/src/engine/gltf/OMI_collider.ts

## Resources:

* Godot Shapes: https://docs.godotengine.org/en/latest/classes/class_shape3d.html
* Unity Colliders: https://docs.unity3d.com/Manual/CollidersOverview.html
* Unreal Engine Collision Shapes: https://docs.unrealengine.com/4.27/en-US/API/Runtime/PhysicsCore/FCollisionShape/
* Unreal Engine Mesh Collisions: https://docs.unrealengine.com/4.27/en-US/WorkingWithContent/Types/StaticMeshes/HowTo/SettingCollision/
* Blender Collisions: https://docs.blender.org/manual/en/latest/physics/rigid_body/properties/collisions.html
* Mozilla Hubs ammo-shape: https://github.com/MozillaReality/hubs-blender-exporter/blob/bb28096159e1049b6b80da00b1ae1534a6ca0855/default-config.json#L608
