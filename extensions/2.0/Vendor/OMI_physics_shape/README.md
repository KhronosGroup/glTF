# OMI_physics_shape

## Contributors

- Aaron Franke, Godot Engine.
- Robert Long, The Matrix.org Foundation
- Mauve Signweaver, Mauve Software Inc.

## Status

Open Metaverse Interoperability Group Stage 1 Proposal

## Dependencies

Written against the glTF 2.0 spec.

Does nothing on its own. Designed to be used together with the `OMI_physics_body` spec.

## Overview

This extension allows specifying physics shapes to be used in glTF scenes.

All types of physics shapes defined in this extension are intended to be used for physics simulations and to be portable between many engines. Without another extension using it, this extension does not mandate any particular behavior for the shapes aside from their geometric properties. The precise usage of these physics shape primitives SHOULD be specified by the extenions which utilize them. In general, these physics shapes are used to specify geometry which can be used for collision detection.

The `OMI_physics_shape` extension is intended to be used together with the `OMI_physics_body` extension, which allows attaching shapes to glTF nodes and specifying their behavior, including static, kinematic, dynamic, and non-solid triggers. The `OMI_physics_body` extension refers to a shape using an index of a shape in the `OMI_physics_shape` document-level shapes array.

### Example:

This example defines a single box shape with a size of 1 meter in all dimensions:

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensions": {
        "OMI_physics_shape": {
            "shapes": [
                {
                    "type": "box",
                    "box": {
                        "size": [1, 1, 1]
                    }
                }
            ]
        }
    },
    "extensionsUsed": [
        "OMI_physics_shape"
    ]
}
```

More example assets can be found in the [examples/](examples/) folder.

## glTF Schema Updates

This extension consists of many new data structures for defining physics shapes on the root glTF document and referencing them on a glTF node. The main data structure `glTF.OMI_physics_shape.shape.schema.json` uses several smaller data structures to define a physics shape. The `glTF.OMI_physics_shape.schema.json` data structure uses the key `"OMI_physics_shape"` in the document-level `"extensions"` which contains a list of the main physics shape data structures. The `node.OMI_physics_shape.schema.json` data structure uses the key `"OMI_physics_shape"` in the node-level `"extensions"` which contains an index of the physics shape to use from the list document-level physics shape list.

The extension must also be added to the glTF's `extensionsUsed` array and because it is optional, it does not need to be added to the `extensionsRequired` array.

### Property Summary

The main data structure `glTF.OMI_physics_shape.shape.schema.json` defines a type property.

|          | Type     | Description                                | Default value        |
| -------- | -------- | ------------------------------------------ | -------------------- |
| **type** | `string` | The type of the physics shape as a string. | Required, no default |

In addition to the type, a key with the same name as the type can be used to define a sub-JSON with the details of the shape. Which sub-properties are allowed depend on which shape type is being used. The possible properties are described in the following table.

|                  | Type        | Description                                                        | Default value   | Valid on          |
| ---------------- | ----------- | ------------------------------------------------------------------ | --------------- | ----------------- |
| **size**         | `number[3]` | The size of the box shape in meters.                               | [1.0, 1.0, 1.0] | Box               |
| **radius**       | `number`    | The radius of the sphere shape in meters.                          | 0.5             | Sphere            |
| **radiusBottom** | `number`    | The radius of the bottom disc or hemisphere in meters.             | 0.5             | Capsule, cylinder |
| **radiusTop**    | `number`    | The radius of the top disc or hemisphere in meters.                | 0.5             | Capsule, cylinder |
| **height**       | `number`    | The mid-height of the cylindrical part of the shape in meters.     | 1.0 or 2.0      | Capsule, cylinder |
| **mesh**         | `number`    | The index of the glTF mesh in the document to use as a mesh shape. | -1              | Trimesh, convex   |

If a key for the type is missing, or a sub-JSON key is missing, implementations should use the default value. A mesh index of -1 means invalid.

### Shape Types

The `"type"` property is a lowercase string that defines what type of shape this physics shape is.

The selection of shapes was carefully chosen with a balance of compatibility between major game engines and containing the most commonly used shapes for easy asset creation. Physics shapes inherit the transform of the glTF node they are attached to. This includes rotation and translation, however it is discouraged to scale physics shape nodes since this can cause problems in some physics engines.

Here is a table listing the mapping between the `OMI_physics_shape` type and the equivalent types in major game engines.

| Shape    | Unity         | Unreal        | Godot                 | Blender     | Bullet (Ammo, Panda3D, etc) |
| -------- | ------------- | ------------- | --------------------- | ----------- | --------------------------- |
| Box      | Box           | Box           | BoxShape3D            | Box         | Box Shape                   |
| Sphere   | Sphere        | Sphere        | SphereShape3D         | Sphere      | Sphere Shape                |
| Capsule  | Capsule       | Capsule       | CapsuleShape3D        | Capsule     | Capsule Shape               |
| Cylinder | Approximation | Approximation | CylinderShape3D       | Cylinder    | Cylinder Shape              |
| Convex   | Mesh (Convex) | Convex        | ConvexPolygonShape3D  | Convex Hull | Convex Shape                |
| Trimesh  | Mesh          | Mesh          | ConcavePolygonShape3D | Mesh        | Mesh Shape                  |

#### Box

Box shapes describe a cube or cuboid shape. They have `size` property which is an array of 3 numbers that describes the width, height, and depth. If the `size` property is omitted, the default size is `[1, 1, 1]`, representing a cube with a volume of one cubic meter, edges/diameters one meter long, and extents/radius of half a meter. The position of the glTF node is the center of the box shape.

#### Sphere

Sphere shapes describe a uniform "ball" shape. They have a `radius` property which is a single number. If the `radius` property is omitted, the default radius is `0.5`, representing a sphere with a radius of half a meter, a diameter of one meter. The position of the glTF node is the center of the sphere shape.

#### Capsule

Capsule shapes describe a "pill" shape. They have `height`, `radiusBottom`, and `radiusTop` properties. If either of the radius properties are omitted, the default radius is `0.5`, and if the `height` property is omitted, the default height is `1.0`.

The `height` property describes the "mid height", the distance between the centers of the two capping hemispheres. The full height from the bottom-most point to the top-most point can be found with `height + radiusBottom + radiusTop`, meaning that a capsule with default values has a full height of 2.0 meters. The height is aligned with the node's local vertical axis. If it's desired to align it along a different axis, rotate the glTF node.

The `radiusBottom` and `radiusTop` properties are recommended to be set to the same value. Having these be different values results in a shape that is not a regular capsule, but a "tapered capsule". Tapered capsules are not supported by all game engines, so using them may result in importers approximating the tapered capsule shape with a convex hull shape.

#### Cylinder

Cylinder shapes describe a "tall circle" shape. They are similar in structure to capsules, they have `height`, `radiusBottom`, and `radiusTop` properties. If the `radius` property is omitted, the default radius is `0.5`, and if the `height` property is omitted, the default height is `2.0`.

The `height` property of a cylinder describes the total height, the distance between the center of the bottom disc and the center of the top disc. The height is aligned with the node's local vertical axis. If it's desired to align it along a different axis, rotate the glTF node.

The `radiusBottom` and `radiusTop` properties are recommended to be set to the same value. Having these be different values results in a shape that is not a regular cylinder, but a "tapered cylinder". Tapered cylinders are not supported by all game engines, so using them may result in importers approximating the tapered cylinder shape with a convex hull shape.

The use of cylinder is discouraged if another shape would work well in its place. Cylinders are harder to calculate than boxes, spheres, and capsules. Not all game engines support cylinder shapes. Engines that do not support cylinder shapes should use an approximation, such as a convex hull roughly shaped like a cylinder. Cylinders over twice as tall as they are wide can use another approximation: a convex hull combined with an embedded capsule to allow for smooth rolling, by copying the cylinder's radius values, and setting the capsule's full height to the cylinder's height, meaning setting the capsule's mid height property to `height - radiusBottom - radiusTop`.

#### Convex

Convex shapes represent a convex hull. Being "convex" means that the shape cannot have any holes or divots, and that all line segments connecting points on the surface stay within the shape. Convex hulls are defined with a `mesh` property with an index of a mesh in the glTF `meshes` array. The glTF mesh in the array MUST be a `trimesh` to work, and should be made of only one glTF mesh primitive (one surface). Valid convex hulls must contain at least one triangle, which becomes three points on the convex hull. Convex hulls are recommended to have at least four points so that they have 3D volume.

Convex hulls can be used to represent complex convex shapes that are not easy to represent with other primitives. If a shape can be represented with a few primitives, prefer using those primitives instead of convex hulls. Convex hulls are much faster than trimesh shapes.

When convex hulls are used, asset creators should try to limit the number of vertices in the shape for improved performance and compatibility. Some game engines limit the amount of vertices that can be in a convex hull, which may caused the actual imported shape to be simplified if the shape in the glTF file is too complex.

#### Trimesh

Trimesh shapes represent a concave triangle mesh. They are defined with a `mesh` property with an index of a mesh in the glTF `meshes` array. The glTF mesh in the array MUST be a `trimesh` to work, and should be made of only one glTF mesh primitive (one surface). Valid trimesh shapes must contain at least one triangle.

Avoid using a trimesh shape for most objects, they are the slowest shapes to calculate and have several limitations. Most physics engines do not support moving trimesh shapes or calculating collisions between multiple trimesh shapes. Trimesh shapes will not work reliably with trigger bodies or with pushing objects out due to not having an "interior" space, they only have a surface. Trimesh shapes are typically used for complex level geometry (for example, things that objects can go inside of). If a shape can be represented with a combination of simpler primitives, or a convex hull, or multiple convex hulls, prefer that instead.

### glTF Object Model

The following JSON pointers are defined representing mutable properties defined by this extension, for use with the glTF Object Model including extensions such as `KHR_animation_pointer` and `KHR_interactivity`.

| JSON Pointer                                                    | Object Model Type |
| --------------------------------------------------------------- | ----------------- |
| `/extensions/OMI_physics_shape/shapes/{}/box/size`              | `float3`          |
| `/extensions/OMI_physics_shape/shapes/{}/sphere/radius`         | `float`           |
| `/extensions/OMI_physics_shape/shapes/{}/capsule/height`        | `float`           |
| `/extensions/OMI_physics_shape/shapes/{}/capsule/radiusBottom`  | `float`           |
| `/extensions/OMI_physics_shape/shapes/{}/capsule/radiusTop`     | `float`           |
| `/extensions/OMI_physics_shape/shapes/{}/cylinder/height`       | `float`           |
| `/extensions/OMI_physics_shape/shapes/{}/cylinder/radiusBottom` | `float`           |
| `/extensions/OMI_physics_shape/shapes/{}/cylinder/radiusTop`    | `float`           |

Additionally, the following JSON pointers are defined for read-only properties:

| JSON Pointer                                  | Object Model Type |
| --------------------------------------------- | ----------------- |
| `/extensions/OMI_physics_shape/shapes.length` | `int`             |

### JSON Schema

See [glTF.OMI_physics_shape.schema.json](schema/glTF.OMI_physics_shape.schema.json) for the document-level list of shapes, [glTF.OMI_physics_shape.shape.schema.json](schema/glTF.OMI_physics_shape.shape.schema.json) for the shape resource schema, and the `glTF.OMI_physics_shape.shape.*.schema.json` files for the individual shape types.

## Known Implementations

- Godot Engine: https://github.com/godotengine/godot/pull/78967

## Resources:

- Godot Shapes: https://docs.godotengine.org/en/latest/classes/class_shape3d.html
- Unity Colliders: https://docs.unity3d.com/Manual/CollidersOverview.html
- Unreal Engine Collision Shapes: https://docs.unrealengine.com/4.27/en-US/API/Runtime/PhysicsCore/FCollisionShape/
- Unreal Engine Mesh Collisions: https://docs.unrealengine.com/4.27/en-US/WorkingWithContent/Types/StaticMeshes/HowTo/SettingCollision/
- Blender Collisions: https://docs.blender.org/manual/en/latest/physics/rigid_body/properties/collisions.html
- Mozilla Hubs ammo-shape: https://github.com/MozillaReality/hubs-blender-exporter/blob/bb28096159e1049b6b80da00b1ae1534a6ca0855/default-config.json#L608
