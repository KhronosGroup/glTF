# OMI_physics_body Motion Property

If a node has the `"motion"` property defined, its transform is driven by the physics engine.

- Descendant nodes should move with that node. The physics engine should treat them as part of a single body.
- If a descendant node has its own motion property, that node should be treated as an independent body during simulation. There is no implicit requirement that it follows its "parent" rigid body.
- If a node's transform is animated by animations in the file, those animations should take priority over the physics simulation.

## Motion Properties

|                        | Type        | Description                                                          | Default value        |
| ---------------------- | ----------- | -------------------------------------------------------------------- | -------------------- |
| **type**               | `string`    | The type of the physics body as a string.                            | Required, no default |
| **mass**               | `number`    | The mass of the physics body in kilograms.                           | 1.0                  |
| **centerOfMass**       | `number[3]` | The center of mass offset from the origin in meters.                 | [0.0, 0.0, 0.0]      |
| **inertiaDiagonal**    | `number[3]` | The inertia around principle axes in kilogram meter squared (kg⋅m²). | [0.0, 0.0, 0.0]      |
| **inertiaOrientation** | `number[4]` | The inertia orientation as a Quaternion.                             | [0.0, 0.0, 0.0, 1.0] |
| **linearVelocity**     | `number[3]` | The initial linear velocity of the body in meters per second.        | [0.0, 0.0, 0.0]      |
| **angularVelocity**    | `number[3]` | The initial angular velocity of the body in radians per second.      | [0.0, 0.0, 0.0]      |
| **gravityFactor**      | `number`    | A multiplier applied to the acceleration due to gravity.             | 1.0                  |

### Motion Types

The `"type"` property is a lowercase string that defines what type of physics body this is. Different types of physics bodies have different interactions with physics systems and other bodies within a scene.

Here is a table listing the mapping between the `OMI_physics_body` type and the equivalent types in major game engines.

| Body Type | Unity                 | Godot 3       | Godot 4          | Unreal                                 |
| --------- | --------------------- | ------------- | ---------------- | -------------------------------------- |
| Static    | Collider              | StaticBody    | StaticBody3D     | WorldStatic, Simulate Physics = false  |
| Kinematic | Rigidbody.isKinematic | KinematicBody | AnimatableBody3D | WorldDynamic, Simulate Physics = false |
| Dynamic   | Rigidbody             | RigidBody     | RigidBody3D      | PhysicsBody, Simulate Physics = true   |

#### Static

Static bodies can be collided with, but do not have simulated movement. They are usually used for level geometry. Specifying a static body is optional, as nodes with collider properties are assumed to be static without itself or a parent node having the motion property.

#### Kinematic

Kinematic bodies can be collided with, and can be moved using scripts or animations. They can be used for moving platforms.

#### Dynamic

Dynamic bodies are bodies simulated with [rigid body dynamics](https://en.wikipedia.org/wiki/Rigid_body_dynamics). They collide with other bodies, and move around on their own in the physics simulation. They are affected by gravity. They can be used for props that move around in the world.

### Mass

The `"mass"` property is a number that defines how much mass this physics body has in kilograms. Not all body types can make use of mass, such as triggers or non-moving bodies, in which case the mass can be ignored. If not specified, the default value is 1 kilogram.

### Center of Mass

The `"centerOfMass"` property is an array of three numbers that defines the position offset in meters of the center of mass in the body's local space. If not specified, the default value is zero.

This property is useful when converting assets with a center of mass, but when creating new assets it is recommended to leave the center of mass at the body's origin. Some physics engines support the center of mass being offset from the origin, but not all of them do. Implementations without support for a center of mass offset would have to adjust the node positions to make this work, which may be undesired.

### Inertia Diagonal

The `"inertiaDiagonal"` property is an array of 3 numbers that defines the inertia around the principle axes of the body in kilogram meter squared (kg⋅m²). We specify "tensor" in the name because this defines inertia in multiple directions and is different from linear momentum inertia. Only the "dynamic" motion type can make use of inertia. If zero or not specified, the inertia should be automatically calculated by the physics engine.

### Inertia Orientation

The `"inertiaOrientation"` property is an array of 4 numbers that defines a Quaternion for orientation of the inertia's principle axes relative to the body's local space. If not specified or set to the default value of `[0.0, 0.0, 0.0, 1.0]`, no rotation is applied, the inertia's principle axes are aligned with the body's local space axes.

### Linear Velocity

The `"linearVelocity"` property is an array of three numbers that defines how much linear velocity this physics body starts with in meters per second. Not all body types can make use of linear velocity, such as non-moving bodies, in which case the linear velocity can be ignored. If not specified, the default value is zero.

### Angular Velocity

The `"angularVelocity"` property is an array of three numbers that defines how much angular velocity this physics body starts with in radians per second. Not all body types can make use of angular velocity, such as non-moving bodies, in which case the angular velocity can be ignored. If not specified, the default value is zero.

### Gravity Factor

The `"gravityFactor"` property is a number that defines a multiplier applied to the acceleration due to gravity. Values other than 1.0 are not realistic, but may be useful for artistic effects. If not specified, the default value is 1.0.

## glTF Object Model

The following JSON pointers are defined representing mutable motion properties defined by this extension, for use with the glTF Object Model including extensions such as `KHR_animation_pointer` and `KHR_interactivity`. See also the list of physics material properties in the [README.collider.md](README.collider.md) file.

| JSON Pointer                                                      | Object Model Type |
| ----------------------------------------------------------------- | ----------------- |
| `/nodes/{}/extensions/OMI_physics_body/motion/mass`               | `float`           |
| `/nodes/{}/extensions/OMI_physics_body/motion/centerOfMass`       | `float3`          |
| `/nodes/{}/extensions/OMI_physics_body/motion/inertiaDiagonal`    | `float3`          |
| `/nodes/{}/extensions/OMI_physics_body/motion/inertiaOrientation` | `float4`          |
| `/nodes/{}/extensions/OMI_physics_body/motion/linearVelocity`     | `float3`          |
| `/nodes/{}/extensions/OMI_physics_body/motion/angularVelocity`    | `float3`          |
| `/nodes/{}/extensions/OMI_physics_body/motion/gravityFactor`      | `float`           |

## JSON Schema

See [schema/node.OMI_physics_body.motion.schema.json](schema/node.OMI_physics_body.motion.schema.json) for the motion properties JSON schema.
