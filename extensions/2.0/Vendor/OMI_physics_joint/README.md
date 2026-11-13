# OMI_physics_joint

## Contributors

- Aaron Franke, Godot Engine.

## Status

Open Metaverse Interoperability Group Stage 1 Proposal

## Dependencies

Written against the glTF 2.0 spec.

Depends on the `OMI_physics_body` spec, which depends on the `OMI_physics_shape` spec.

## Overview

This extension allows defining a glTF node as a physics joint for constraining the relative motion of dynamic bodies, or a dynamic body to a non-dynamic body.

Each physics joint node is a glTF node as a descendant of one physics body that references another glTF node, joining them together. The joint node must have a dynamic body ancestor node (`OMI_physics_body` motion type "dynamic") for the joint to function. A joint must be on its own glTF node, it should not be on the same node as a mesh, camera, light, physics body, physics shape, etc. The joint node is connected to another glTF node, `"connectedNode"`, which must be a descendant of some kind of physics body node (`OMI_physics_body` motion of any type). The transforms of the joint node and its connected node relative to their respective physics bodies determine the joint's connection point and orientation.

Each physics joint node must reference joint settings defined in the document-level joint `"physicsJoints"` array. Each joint settings object contains an array of joint limits and an array of joint drives, which define the behavior of the joint. The parameters of all joint limits define how the joint constrains the relative motion of the connected nodes. The parameters of all joint drives define how the joint applies forces to the connected nodes to drive them to a desired relative target, which includes forces that make the drive act as a motor or spring.

## glTF Schema Updates

This extension consists of five new data structures for defining physics joints. A glTF file with physics joints should have a document-level `"OMI_physics_joint"` object added to the document `"extensions"` which contains an array of physics joint settings. Each joint settings object has an array of joint limits, and an array of joint drives. Then, the key `"OMI_physics_joint"` can be added to the node-level `"extensions"` of a glTF node to define that node is a physics joint, as long as the node is a descendant of a physics body node. Each physics joint node references a settings object the document-level constraints array by index, and each joint node also references another glTF node to which it is connected.

The extension must also be added to the glTF's `extensionsUsed` array and because it is optional, it does not need to be added to the `extensionsRequired` array.

## Joint Node

If a node has the `"OMI_physics_joint"` extension defined, it is a physics joint that constrains the relative motion of two physics bodies at their attachment point glTF nodes. The joint settings and the connected node must be defined.

|                     | Type      | Description                                                           | Default value        |
| ------------------- | --------- | --------------------------------------------------------------------- | -------------------- |
| **joint**           | `integer` | The index of the joint settings in the top level physicsJoints array. | Required, no default |
| **connectedNode**   | `integer` | The index of the node to which this is connected.                     | Required, no default |
| **enableCollision** | `boolean` | If true, allow the connected objects to collide.                      | false                |

### Joint

The `"joint"` property is an integer index that references a joint in the document-level physicsJoints array in the `OMI_physics_body` extension. This property is required and has no default value.

The document-level joint object holds parameters that define the behavior of the joint, including constraints, limits, and drives/motors. This allows the limits and drives utilized by the joint to be defined in a shareable manner.

### Connected Node

The `"connectedNode"` property is an integer index that references another glTF node in the scene. This property is required and has no default value.

The connected node must be a descendant of some kind of physics body node (`OMI_physics_body` motion of any type). The transforms of the joint node and its connected node relative to their respective physics bodies determine the joint's connection point and orientation, known as the attachment frame. Changing the attachment frame at runtime is not expected to be supported. When calculating physics interactions, the joint should attempt to keep the joint and its connected node at the same global position and orientation, within the range allowed by the joint's limits.

### Enable Collision

The `"enableCollision"` property is a boolean value. If true, allow the connected objects to collide. Connected objects do not collide with each other by default.

## Joint Settings

The top-level extension object contains an array of joint settings objects, `"physicsJoints"`, which are referenced by the joint nodes. Each object contains an array of joint limits and an array of joint drives, which define the behavior of the joint.

|            | Type    | Description                                                                         | Default value |
| ---------- | ------- | ----------------------------------------------------------------------------------- | ------------- |
| **limits** | `array` | The set of limits which constrain relative motion between a pair of nodes.          | []            |
| **drives** | `array` | The set of drives applying forces to this joint. Drives include motors and springs. | []            |

Joint settings objects may have zero or more joint limits and zero or more joint drives. At least one array should have contents, or the joint settings object is useless within this extension. See "Joint Limits" and "Joint Drives" below for more information about the properties of the objects in each array.

## Joint Limits

Joint limits, also known as joint constraints, allow restricting the relative motion of two glTF nodes connected by a joint. Limits can be set on the joint's position, rotation, or both. At least one of the linear or angular axes must be set, or the limit is useless within this extension. At least one of the `"min"` or `"max"` must be set, or the limit is useless within this extension.

|                 | Type        | Description                                                                           | Default value |
| --------------- | ----------- | ------------------------------------------------------------------------------------- | ------------- |
| **linearAxes**  | `integer[]` | The indices of the linear axes which are limited by this constraint.                  | []            |
| **angularAxes** | `integer[]` | The indices of the angular axes which are limited by this constraint.                 | []            |
| **min**         | `number`    | The minimum of the allowed range of relative distance in meters, or angle in radians. | -Infinity     |
| **max**         | `number`    | The maximum of the allowed range of relative distance in meters, or angle in radians. | Infinity      |
| **stiffness**   | `number`    | The stiffness strength used when the joint is extended beyond the limit.              | Infinity      |
| **damping**     | `number`    | Damping applied to the velocity when the joint is extended beyond the limit.          | 0.0           |

This approach of building joints from a set of individual limit is flexible enough to allow for many types of bilateral joints. For example, a hinged door can be constructed by locating the attachment frames at the point where the physical hinge would be on each body, adding a 3D linear limit with `"min"` and `"max"` set to zero, a 1D angular limit on the vertical axis with `"min"` and `"max"` describing the swing angle of the door, and a 2D angular limit on the horizontal axes with `"min"` and `"max"` set to zero.

### Linear Axes

The `"linearAxes"` property is an array of integers that represent the indices of the linear axes which are limited by this constraint. Linear axes constrain the linear motion in 1, 2, or 3 dimensions. Can only contain 3 possible values: 0 (X), 1 (Y), or 2 (Z). If not specified or empty, the constraint does not limit linear motion.

- A 1D linear limit should restrict the translation between the attachment frames along one axis.
- A 2D linear limit should restrict the translation between the attachment frames on two axes.
- A 3D linear limit should restrict the translation between the attachment frames on all three axes.

To restrict linear movement on a line, use a 2D linear limit with min and max equal, and to restrict movement to a line segment, add a 1D linear limit with a range the size of the line segment. See [slider_ball.gltf](examples/slider_ball.gltf) or [swing_and_slide.gltf](examples/swing_and_slide.gltf) for examples.

At least one of `"linearAxes"` or `"angularAxes"` must be set to a valid non-empty value. If both are empty or not present, the joint limit is invalid.

### Angular Axes

The `"angularAxes"` property is an array of integers that represent the indices of the angular axes which are limited by this constraint. Angular axes constrain the angular motion in 1, 2, or 3 dimensions. Can only contain 3 possible values: 0 (X), 1 (Y), or 2 (Z). If not specified or empty, the constraint does not limit angular motion.

- A 1D angular limit should restrict the angle between the attachment frames around one axis.
- A 2D angular limit should restrict the angle between the attachment frames around two axes.
- A 3D angular limit should restrict the angle between the attachment frames around all three axes.

To restrict angular movement on a circle, like a hinge, use a 2D angular limit with min and max equal, and to restrict movement to a range of angles, add a 1D angular limit with a range the size of the angle range. See [pendulum_balls.gltf](examples/pendulum_balls.gltf) or [swing_and_slide.gltf](examples/swing_and_slide.gltf) for examples.

At least one of `"linearAxes"` or `"angularAxes"` must be set to a valid non-empty value. If both are empty or not present, the joint limit is invalid.

### Min

The `"min"` property is a number that represents the minimum of the allowed range of relative distance in meters, or angle in radians. If not specified, the default value is `-Infinity`, which means the minimum is unbounded.

When the relative offset between the connected nodes is between the min and max, the limit is considered non-violating and no corrective forces are applied. Outside of this range, the behavior depends on the stiffness and damping properties. If both `"min"` and `"max"` are set to the same value, the joint is fixed at that position or angle. If everything is fixed, this is also known as a "weld" joint.

### Max

The `"max"` property is a number that represents the maximum of the allowed range of relative distance in meters, or angle in radians. If not specified, the default value is `Infinity`, which means the maximum is unbounded.

When the relative offset between the connected nodes is between the min and max, the limit is considered non-violating and no corrective forces are applied. Outside of this range, the behavior depends on the stiffness and damping properties. If both `"min"` and `"max"` are set to the same value, the joint is fixed at that position or angle. If everything is fixed, this is also known as a "weld" joint.

### Stiffness

The `"stiffness"` property is a number that represents the stiffness strength used to calculate a restorative force when the joint is extended beyond the limit. If not specified or negative, the limit should be infinitely stiff.

The stiffness value is also known as the spring constant. Specifying a finite stiffness will cause the limit to become soft at the limits, and then the stiffness should be used as the spring constant in the Hooke's Law spring equation.

Stiffness is measured in different units depending on the type of stiffness. For linear stiffness, it is measured in Newtons per meter, or kg/s² in SI base units. For angular stiffness, it is measured in Newton-meters per radian, or kg⋅m²/s²/rad in SI base units. This difference is because linear stiffness involves force divided by distance (N / m), while angular stiffness involves torque divided by angle (N⋅m / rad).

### Damping

The `"damping"` property is a number that represents damping applied to the velocity when the joint is extended beyond the limit. If not specified, the default value is 0.0.

Damping is measured in different units depending on the type of damping. For linear damping, it is measured in Newton-seconds per meter, or kg/s in SI base units. For angular damping, it is measured in Newton-meter-seconds per radian, or kg⋅m²/s/rad in SI base units. This difference is because linear damping involves impulse divided by distance (N⋅s / m), while angular damping involves torque-impulse divided by angle (N⋅m⋅s / rad).

### Example: Fixed/Weld Joint

The following example defines a fixed joint composed of a single limit, which has a min/max of `0.0` and applies to all linear and angular axes. This means that no movement is allowed between the bodies connected by this joint, which effectively welds the two nodes together:

```json
{
    "extensions": {
        "OMI_physics_joint": {
            "physicsJoints": [
                {
                    "limits": [
                        {
                            "angularAxes": [0, 1, 2],
                            "linearAxes": [0, 1, 2],
                            "max": 0.0,
                            "min": 0.0
                        }
                    ]
                }
            ]
        }
    }
}
```

The damping and stiffness are not specified in the above example, which means the joint limit has no damping and is infinitely stiff. With this example weld joint, the physics engine should ensure that their global transforms are equal to each other, and prevent the connected nodes from moving relative to each other in global space.

## Joint Drives

Joint drives allow a joint to apply forces to the connected nodes to drive them to a desired relative target. Each joint drive describes only a single degree of freedom in the joint's local space, specified with a combination of the required `"type"` and `"axis"` parameters. Drives can have a target linear velocity or angular velocity, in which case they are known as **motors**. Drives can also have a target position or angle, in which case they are known as **springs**.

|                    | Type      | Description                                                                            | Default value        |
| ------------------ | --------- | -------------------------------------------------------------------------------------- | -------------------- |
| **type**           | `string`  | Determines the type of degree of freedom which this drive controls: linear or angular. | Required, no default |
| **mode**           | `string`  | Specifies the force calculation mode: force or acceleration.                           | Required, no default |
| **axis**           | `integer` | The index of the axis which this drive applies forces on.                              | Required, no default |
| **maxForce**       | `number`  | The maximum force (or torque, for angular drives) the drive can apply.                 | Infinity             |
| **positionTarget** | `number`  | The target translation/angle along the axis that this drive attempts to achieve.       | None                 |
| **velocityTarget** | `number`  | The target velocity along/about the axis that this drive attempts to achieve.          | None                 |
| **stiffness**      | `number`  | The stiffness of the drive, scaling the force based on the position target.            | 0.0                  |
| **damping**        | `number`  | The damping of the drive, scaling the force based on the velocity target.              | 0.0                  |

### Type

The `"type"` property is a string that determines the type of degree of freedom which this drive controls. The value must be either `"linear"`, `"angular"`, or a value specified by another extension. This property is required and has no default value.

### Mode

The `"mode"` property is a string that specifies the force calculation mode. The value must be either `"force"`, `"acceleration"`, or a value specified by another extension. This property is required and has no default value.

The acceleration mode is useful for tuning the drive parameters to scale the force by the effective inertia of the driven degree of freedom. This mode allows more easily achieving the desired behavior in scenarios where the masses or mass distributions of the connected nodes are not known in advance. However, not all physics engines support this mode.

### Axis

The `"axis"` property is an integer that represents the index of the axis which this drive applies forces on. The value must be 0 (X), 1 (Y), or 2 (Z). This property is required and has no default value.

### Max Force

The `"maxForce"` property is a number that represents the maximum force (or torque, for angular drives) the drive can apply. If not specified or negative, the drive is not force-limited.

### Position Target

The `"positionTarget"` property is a number that represents the target position or angle along the axis that this drive attempts to achieve. If not specified, the drive is not a spring that attempts to reach a target position or angle.

When a position target is specified, the drive force is proportional to `stiffness * (positionTarget - positionCurrent) + damping * (velocityTarget - velocityCurrent)` where `positionCurrent` and `velocityCurrent` are the signed values of the position and velocity of the connected glTF node relative to the joint glTF node.

### Velocity Target

The `"velocityTarget"` property is a number that represents the target linear velocity or angular velocity the axis that this drive attempts to achieve. If not specified, the drive is not a motor that attempts to reach a target velocity.

When a velocity target is specified, but not a position target, the drive force is proportional to `damping * (velocityTarget - velocityCurrent)` where `velocityCurrent` is the signed value of the velocity of the connected glTF node relative to the joint glTF node.

### Stiffness

The `"stiffness"` property is a number that represents the stiffness of the drive, scaling the force based on the position target. If not specified, the default value is 0.0, which means the drive does not have any spring force.

The stiffness value is also known as the spring constant. In order to make the drive act as a spring, this must be set to a positive value, along with setting a position target, in which case the stiffness should be used as the spring constant in the Hooke's Law spring equation.

Stiffness is measured in different units depending on the type of stiffness. For linear stiffness, it is measured in Newtons per meter, or kg/s² in SI base units. For angular stiffness, it is measured in Newton-meters per radian, or kg⋅m²/s²/rad in SI base units. This difference is because linear stiffness involves force divided by distance (N / m), while angular stiffness involves torque divided by angle (N⋅m / rad).

### Damping

The `"damping"` property is a number that represents the damping of the drive, scaling the force based on the velocity target. If not specified, the default value is 0.0, which means the drive does not have any damping force.

Damping is measured in different units depending on the type of damping. For linear damping, it is measured in Newton-seconds per meter, or kg/s in SI base units. For angular damping, it is measured in Newton-meter-seconds per radian, or kg⋅m²/s/rad in SI base units. This difference is because linear damping involves impulse divided by distance (N⋅s / m), while angular damping involves torque-impulse divided by angle (N⋅m⋅s / rad).

## Special Cases

While a generic joint type is very useful and extensible, it's also worth defining when a generic joint is equivalent to a special-purpose joint. These mappings can be used to convert to more optimized joint types if supported in a physics engine, or as simply a friendly name to display to a user.

In this chart, "fixed" means "limited with min and max set to zero", and "free" means "not limited".

| Special case name  | Description                                                                                                              |
| ------------------ | ------------------------------------------------------------------------------------------------------------------------ |
| Fixed / Weld Joint | All axes fixed.                                                                                                          |
| Pin Joint          | All linear axes fixed, all angular axes free.                                                                            |
| Hinge Joint        | One angular axis limited with non-equal min and max limits, the rest are fixed.                                          |
| Slider Joint       | One linear axis and optionally the angular axis around it limited with non-equal min and max limits, the rest are fixed. |

## glTF Object Model

The following JSON pointers are defined representing mutable properties defined by this extension, for use with the glTF Object Model including extensions such as `KHR_animation_pointer` and `KHR_interactivity`.

| Pointer                                                                   | Type      |
| ------------------------------------------------------------------------- | --------- |
| `/extensions/OMI_physics_joint/physicsJoints/{}/limits/{}/min`            | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/limits/{}/max`            | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/limits/{}/stiffness`      | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/limits/{}/damping`        | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/drives/{}/maxForce`       | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/drives/{}/positionTarget` | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/drives/{}/velocityTarget` | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/drives/{}/stiffness`      | `float`   |
| `/extensions/OMI_physics_joint/physicsJoints/{}/drives/{}/damping`        | `float`   |
| `/nodes/{}/extensions/OMI_physics_joint/joint/enableCollision`            | `boolean` |

Additionally, the following JSON pointers are defined for read-only properties:

| Pointer                                              | Type  |
| ---------------------------------------------------- | ----- |
| `/extensions/OMI_physics_joint/physicsJoints.length` | `int` |

## JSON Schema

See the following schema files:

- Per-node joint properties: [node.OMI_physics_joint.schema.json](schema/node.OMI_physics_joint.schema.json)
- Document-level array of joint settings: [glTF.OMI_physics_joint.schema.json](schema/glTF.OMI_physics_joint.schema.json)
  - Joint settings: [glTF.OMI_physics_joint.joint.schema.json](schema/glTF.OMI_physics_joint.joint.schema.json)
    - Joint limit properties: [glTF.OMI_physics_joint.joint.limit.schema.json](schema/glTF.OMI_physics_joint.joint.limit.schema.json)
    - Joint drive properties: [glTF.OMI_physics_joint.joint.drive.schema.json](schema/glTF.OMI_physics_joint.joint.drive.schema.json)

## Known Implementations

- Godot Engine add-on: https://github.com/omigroup/omi-godot/tree/main/addons/omi_extensions/physics_joint

## Resources:

- Godot Joint3D https://docs.godotengine.org/en/latest/classes/class_generic6dofjoint3d.html
- Unity Joints https://docs.unity3d.com/Manual/Joints.html
- Wikipedia Stiffness https://en.wikipedia.org/wiki/Stiffness
