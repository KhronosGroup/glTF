# MSFT\_CollisionPrimitives

## Status

Draft

## Dependencies

Written against glTF 2.0 spec.

## Overview

This extension adds the ability to specify collision primitives inside a glTF asset. This extension does not mandate any particular behaviour for those objects aside from their collision geometry. These types may be used in combination with additional extensions or application-specific business logic. The MSFT\_RigidBodies extension uses primitives defined in this spec to provide geometric primitives for collision detection in a rigid body simulation.

## glTF Schema Updates

The `MSFT_CollisionPrimitives` extension can be added to any `node` to define one or more of the following properties:

| |Type|Description|
|-|-|-|
|**collider**|`object`|Create a collision shape in the physics engine for this node.|

### Colliders

This extension provides a set of document-level objects, which can be referenced by nodes in the scene. The precise usage of these collider primitives should be specified by the extenions which utilizes the colliders. In general, these colliders are used to specify geometry which can be used for collision detection. Typically, the geometry specified by the collider will be simpler than any meshes used by the node or it's children, enabling real-time applications to perform queries such as intersection tests.

Implementations of this extension should ensure that collider transforms are always kept in sync with node transforms - for example animated node transforms should be applied to the applications' internal representation of the collision geometry.

To describe the shape that represents this node, should use, colliders must define exactly one of the following properties:

| |Type|Description|
|-|-|-|
|**sphere**|`object`|A sphere centered at the origin in local space.|
|**box**|`object`|An axis-aligned box centered at the origin in local space.|
|**capsule**|`object`|A capsule centered at the origin and aligned along the Y axis in local space.|
|**cylinder**|`object`|A cylinder centered at the origin and aligned along the Y axis in local space.|
|**convex**|`object`|A convex hull wrapping a `mesh` object.|
|**trimesh**|`object`|A triangulated representation of a `mesh` object.|

The sphere, box, capsule, cylinder and convex types should all produce _solid_ colliders by default - a rigid body anywhere inside the shape should be pushed out of it.
However the trimesh type always represents an infinitely thin shell or sheet - for example a trimesh collider created from a `mesh` object in the shape of a box will be represented as a hollow box.

If you want your collider to have an offset from the local space (for example a sphere _not_ centered at local origin, or a rotated box), you should add an extra node to the hierarchy and apply your transform and your collider properties to that.

**Collision Filtering**

By default each `collider` can generate collisions with every other `collider`, provided they are sufficiently close. If you want certain objects in your scene to ignore collisions with others, you can provide the following optional collider properties:

| |Type|Description|
|-|-|-|
|**collisionSystems**|`[string]`|An array of arbitrary strings indicating the `system` a node is in.|
|**notCollideWithSystems**|`[string]`|An array of strings representing the systems which this node will _not_ collide with|
|**collideWithSystems**|`[string]`|An array of strings representing the systems which this node can collide with|

Both `collideWithSystems` and `notCollideWithSystems` are provided so that users can override the default collision behaviour with minimal configuration -- only one of these should be specified per-collider. Note, given knowledge of all the systems in a scene and one of the vvalues `notCollideWithSystems`/`collideWithSystems` the unspecified field can be calculated: `collideWithSystems = notCollideWithSystems'`

`notCollideWithSystems` is useful for an object which should collide with everything except those listed in `notCollideWithSystems` (i.e., used to opt-out of collisions) while `collideWithSystems` is the inverse -- the collider should _not_ collide with any other collider excpet those listed in `collideWithSystems`

A node `A` will collide with node `B` if `A.collisionSystem ⊆ B.collideWithSystems && A.collisionSystem ⊄ B.notCollideWithSystems`

Note, that this can generate asymmetric states - `A` might determine that it _does_ collide with `B`, but `B` may determine that it _does not_ collide with `A`. As the default behaviour is that collision should be enabled, both `doesCollide(A, B)` and `doesCollide(B, A)` tests should be performed and collision should not occur if either returns false.

### JSON Schema

* **JSON schema**: [glTF.MSFT_CollisionPrimitives.schema.json](schema/glTF.MSFT_CollisionPrimitives.schema.json)

## Known Implementations

[Blender exporter](https://github.com/eoineoineoin/glTF_Physics_Blender_Exporter) (work in progress)

[Godot importer](https://github.com/eoineoineoin/glTF_Physics_Godot_Importer) (work in progress)

## Validator

To do
