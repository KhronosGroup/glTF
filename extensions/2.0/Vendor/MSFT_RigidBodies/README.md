# MSFT\_RigidBodies

## Contributors

* Rory Mullane, Microsoft, <mailto:romul@microsoft.com>
* Eoin McLoughlin, Microsoft, <mailto:eomcl@microsoft.com>
* George Tian, Microsoft, <mailto:geotian@microsoft.com>

## Status

Draft

## Dependencies

Written against glTF 2.0 spec.

## Overview

This extension adds the ability to specify physics properties to a glTF asset, suitable for a rigid body simulation.
An implementation of this extension can use these properties to animate node transforms by simulating them using a physics engine.
Objects within the asset can collide with each other and be constrained together to produce physically plausible interactions.

Note, since all physics engines behave differently to each other, deterministic simulation should never be assumed. Even the same engine is likely to behave different on different platforms.

## glTF Schema Updates

The `MSFT_RigidBodies` extension can be added to any `node` to define one or more of the following properties:

| |Type|Description|
|-|-|-|
|**rigidBody**|`object`|Allow the physics engine to move this node and its children.|
|**joint**|`object`|Constrain the motion of this node relative to another.|

### Units

Units used in this specification are the same as those in the [glTF specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units); some additional units are used by this extension:

| Property | Units|
|-|-|
|`rigidBody.mass`|Kilograms (kg)|
|`rigidBody.inertiaTensor`|Kilogram meter squared (kg·m<sup>2</sup>)|
|`rigidBody.linearVelocity`|Meters per second (m·s<sup>-1</sup>)|
|`rigidBody.angularVelocity`|Radians per second (rad·s<sup>-1</sup>)|
|`joint.constraint.springConstant`|Newton per meter (N·m<sup>-1</sup>)|

### Rigid Bodies

If a `node` has `rigidBody` properties, that implies that its transform should driven by the physics engine.
The physics engine should update the node's local transform after every simulation step.

All descendant nodes should move with that node - i.e. the physics engine should treat them as part of a single rigid body.
However if a descendant node has its own `rigidBody` properties, that should be treated as an independent rigid body during simulation - there is no implicit requirement that it follows its 'parent' rigid body.

If a rigid body node's transform is animated by animations in the file, those animations should take priority over the physics simulation. Rigid bodies should follow the transforms provided by the animations.

Rigid bodies have the following properties:

| |Type|Description|
|-|-|-|
|**isKinematic**|`boolean`|Treat the rigid body as having infinite mass. Its velocity will be constant during simulation.|
|**mass**|`number`|The mass of the rigid body. Larger values imply the rigid body is harder to move.|
|**inertiaTensor**|`number[9]`|Mass distribution of the rigid body in local space. Larger values imply the rigid body is harder to rotate.|
|**centerOfMass**|`number[3]`|Center of mass of the rigid body in local space.|
|**linearVelocity**|`number[3]`|Initial linear velocity of the rigid body in local space.|
|**angularVelocity**|`number[3]`|Initial angular velocity of the rigid body in local space.|

### Colliders

To specify the geometry used to detect overlaps, we use the MSFT\_CollisionPrimitives extension. By default, a node with a collision primitive should not participate in the physics simulation, as that geometry may be used for application-specific use-cases. In order to indicate that a `collider` should be used in the physics simulation, that node should have a `physicsMaterial` property, which indicates that it generates some physical response from interactions with other colliders. A collider attached to a node with `MSFT_RigidBodies` should be interpreted as an object which cannot generate a collision response (i.e. no impulses are applied as a result of collision detection), but instead, should be used as a "trigger", which the application may utilize to generate callbacks for implementation of application-specific logic.
If the node is part of a rigid body (i.e. itself or an ascendent has `rigidBody` properties) then the collider belongs to that rigid body and should move with it during simulation.
Otherwise the collider exists as a static object in the physics simulation which can be collided with but can not be moved.

Implementations of this extension should ensure that collider transforms are always kept in sync with node transforms - for example animated node transforms should be applied to the physics engine (even for static colliders).

Note that, depending on the simulation engine in use, `convex` and `trimesh` colliders may impose a large computational cost when converting to native types if the source mesh contains many vertices. In addition, real-time engines generally prefer to avoid collisions between two `trimesh` objects. For best performance and behaviour, consult the manual for the physics simulation engine you are using.

**Collision Response**

You can control how objects should respond during collisions by tweaking their friction and restitution values. This is done by providing the following collider property:

| |Type|Description|
|-|-|-|
|**physicsMaterial**|`integer`|The index of a top level `physicsMaterial`.|
|**collider**|`integer`|The index of a top level `Collider`.|


The top level array of `physicsMaterial` objects is provided by adding the `MSFT_RigidBodies` extension to any root `glTF` object, while the colliers array is provided by the `MSFT_CollisionPrimitives` extension. If a collider has no physics material assigned, that collider should not generate impulses when overlapping with other colliders - implementations may use this behaviour to trigger callbacks, which can implement application-specific logic; such objects are typically called "triggers", "phantoms", "sensors", or "overlap volumes" in physics simulation engines.

Physics materials offer the following properties:

| |Type|Description|
|-|-|-|
|**staticFriction**|`number`|The friction used when an object is laying still on a surface.<br/>Typical range from 0 to 1.|
|**dynamicFriction**|`number`|The friction used when already moving.<br/>Typical range from 0 to 1.|
|**restitution**|`number`|The bouncyness of the surface.<br/>Typical range from 0 to 1.|
|**frictionCombine**|`string`|How to combine two friction values.<br/>"AVERAGE", "MINIMUM", "MAXIMUM", or "MULTIPLY".|
|**restitutionCombine**|`string`|How to combine two restitution values.<br/>"AVERAGE", "MINIMUM", "MAXIMUM", or "MULTIPLY".|

When a pair of colliders collide during physics simulation, the applied friction and restitution values are based on their "combine" policies:
* If either uses "AVERAGE" : The two values should be averaged.
* Else if either uses "MINIMUM" : The smallest of the two values should be used.
* Else if either uses "MAXIMUM" : The largest of the two values should be used.
* Else if either uses "MULTIPLY" : The two values should be multiplied with each other.

### Joints

If a `node` has `joint` properties, that implies it should be constrained to another object during physics simulation.
Joints require a `connectedNode` property, defining the other end of the joint, in addition to a `jointLimits` property, which indexes into the top level array of `physicsJointLimits` and determines how the range of motion is restricted.
In order for the joint to have any effect on the simulation, at least one of the connected nodes or its ancestors should have `rigidBody` properties (otherwise the nodes cannot be moved by the physics engine).

The transform of the node (or the `connectedNode`) from the first parent `rigidBody` defines the constraint space - for example if a joint were to eliminate all degrees of freedom, the physics simulation should attempt to move the rigidBody nodes such that the transforms of the constrained child nodes become aligned with each other.

The top level array of `physicsJointLimits` objects is provided by adding the `MSFT_RigidBodies` extension to any root `glTF` object and contains an array of joints. Joints must contain one of more `constraint` objects.
Each of these constraints removes some of the relative movement permitted between the two connected nodes.
Each constraint should be one of the following:

| |Type|Description|
|-|-|-|
|**linearAxes**|`integer[1..3]`|The linear axes to constrain (0=X, 1=Y, 2=Z).|
|**angularAxes**|`integer[1..3]`|The angular axes to constrain (0=X, 1=Y, 2=Z).|
|**min**|`number`|The minimum allowed relative distance/angle.|
|**max**|`number`|The maximum allowed relative distance/angle.|
|**springConstant**|`number`|Optional softness of the limits when beyond the limits.|
|**springDamping**|`number`|Optional spring damping applied when beyond the limits.|

Each constraint must provide an array of axes which are restricted.
These axes refer to the columns of the basis defined by the transform of the connected nodes and as such, should be in the range 0 to 2.
The number of axes provided determines whether is should be a 1, 2 or 3 dimensional constraint as follows:
* For linear constraints, a 1D constraint will keep that axis within the `min` and `max` distance from the infinite plane defined by the other two axes.
A 2D constraint will keep the node translations within a certain distance from an infinite line (i.e., within an infinite cylinder) and a 3D constraint will keep the nodes within a certain distance from a point (i.e. within a sphere).
* For angular constraints, a 1D constraint restricts angular movement about one axis, as in a universal joint.
A 2D constraint restricts angular movement about two - keeping the pivots within a cone.

Each constraint contains a `min` and `max` parameter, describing the range of allowed difference between the two node transforms - within this range, the constraint is considered non-violating and no corrective forces are applied.
These values represent a _distance_ for linear constraints, or an _angle_ in radians for angular constraints.

Additionally, each constraint has an optional `springConstant` and `springDamping` which specify the proportion of the recovery applied to the constraint.
By default, an infinite spring constant is assumed, implying hard limits. Specifying these spring values will cause constraints to become soft at the limits.

This approach of building joints from a set of individual constraints is flexible enough to allow for many types of bilateral joints.
For example, to define a hinged door you would place connected nodes at the point where the physical hinge would be and add: a 3D linear constraint with zero maximum distance;
a 1D angular constraint describing the swing of the door around it's vertical axis; a 2D angular constraint with zero limits about the remaining two axes.

Note however that some types of constraint are currently not possible to describe.
For example, a pulley, which needs a third transform in order to calculate a distance, cannot be described. Similarly, this does not have a mechanism to link two axes by some factor, such as a screw, whose translation is affected by the amount of rotation about some axis.

### JSON Schema

* **JSON schema**: [glTF.MSFT_RigidBodies.schema.json](schema/glTF.MSFT_RigidBodies.schema.json)

## Known Implementations

[Blender exporter](https://github.com/eoineoineoin/glTF_Physics_Blender_Exporter) (work in progress)

[Godot importer](https://github.com/eoineoineoin/glTF_Physics_Godot_Importer) (work in progress)

## Validator

To do
