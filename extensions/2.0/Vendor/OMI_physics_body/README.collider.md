# OMI_physics_body Collider Property

If a node has the `"collider"` property defined, it is a solid collider node that objects can collide with.

## Collider Properties

|                     | Type      | Description                                                                  | Default value |
| ------------------- | --------- | ---------------------------------------------------------------------------- | ------------- |
| **shape**           | `integer` | The index of the shape to use as the collision shape.                        | -1            |
| **physicsMaterial** | `integer` | The index of the physics material in the top level physicsMaterials array.   | -1            |
| **collisionFilter** | `integer` | The index of the filter information in the top level collisionFilters array. | -1            |

### Shape

The `"shape"` property is an integer index that references a shape in the document-level shapes array as defined by the `OMI_physics_shape` extension. If not specified or -1, this node has no collider shape, but may be the parent of other nodes that do have collider shapes, and can combine those nodes into one collider (this may be a body or compound collider depending on the engine).

### Physics Material

The `"physicsMaterial"` property is an integer index that references a physics material in the document-level physicsMaterials array in the `OMI_physics_body` extension. If not specified or -1, the default physics material is used.

Physics materials define the physical properties of a surface in a physics simulation, which affect how objects respond when they collide with it. See the below [Physics Material Properties](#physics-material-properties) section for more information.

### Collision Filter

The `"collisionFilter"` property is an integer index that references a collision filter in the document-level collisionFilters array in the `OMI_physics_body` extension. If not specified or -1, the default collision filter is used.

Collision filters are used to determine which objects can collide with each other. A collision filter includes a set of collision systems (the "collision layer" the object is on), and a set of collision systems to collide with or to ignore (the "collision mask" of the object). See the [README.trigger.md](README.trigger.md) file for more information on collision filters.

## Physics Material Properties

The following properties may be defined in a physics material in the top-level `physicsMaterials` array:

|                        | Type     | Description                                                              | Default value |
| ---------------------- | -------- | ------------------------------------------------------------------------ | ------------- |
| **staticFriction**     | `number` | The friction used when an object is laying still on a surface.           | 0.6           |
| **dynamicFriction**    | `number` | The friction used when already moving.                                   | 0.6           |
| **restitution**        | `number` | How bouncy is the surface?                                               | 0.0           |
| **frictionCombine**    | `string` | Determines how friction should be combined when two objects interact.    | `"average"`   |
| **restitutionCombine** | `string` | Determines how restitution should be combined when two objects interact. | `"average"`   |

### Static Friction

The `"staticFriction"` property is a number that represents the friction used when an object is laying still on a surface. Typically on a range of 0.0 to 1.0. If not specified, the default value is 0.6.

A value of 0.0 feels like ice, a value of 1.0 will make it very hard to get an object moving. Physics simulations which do not differentiate between static and dynamic friction should use the dynamic friction value.

### Dynamic Friction

The `"dynamicFriction"` property is a number that represents the friction used when already moving. Typically on a range of 0.0 to 1.0. If not specified, the default value is 0.6.

A value of 0.0 feels like ice, a value of 1.0 will make an object come to rest very quickly unless a lot of force or gravity pushes the object. Physics simulations which do not differentiate between static and dynamic friction should use the dynamic friction value.

### Restitution

The `"restitution"` property is a number that represents how bouncy the surface is. Typically on a range of 0.0 to 1.0. If not specified, the default value is 0.0, which means the surface is not bouncy at all.

### Friction Combine

The `"frictionCombine"` property is a string that determines how friction should be combined when two objects interact. If not specified, the default value is `"average"`.

### Restitution Combine

The `"restitutionCombine"` property is a string that determines how restitution should be combined when two objects interact. If not specified, the default value is `"average"`.

When a pair of physics materials interact during a simulation step, the applied friction and restitution values are based on their "combine" policies:

- If either uses `"average"`: The two values should be averaged.
- Else if either uses `"minimum"`: The smallest of the two values should be used.
- Else if either uses `"maximum"`: The largest of the two values should be used.
- Else if either uses `"multiply"`: The two values should be multiplied with each other.

## glTF Object Model

The following JSON pointers are defined representing mutable physics material properties defined by this extension, for use with the glTF Object Model including extensions such as `KHR_animation_pointer` and `KHR_interactivity`. See also the list of motion properties in the [README.motion.md](README.motion.md) file.

| JSON Pointer                                                       | Object Model Type |
| ------------------------------------------------------------------ | ----------------- |
| `/extensions/OMI_physics_body/physicsMaterials/{}/staticFriction`  | `float`           |
| `/extensions/OMI_physics_body/physicsMaterials/{}/dynamicFriction` | `float`           |
| `/extensions/OMI_physics_body/physicsMaterials/{}/restitution`     | `float`           |

Additionally, the following JSON pointers are defined for read-only properties:

| JSON Pointer                                           | Object Model Type |
| ------------------------------------------------------ | ----------------- |
| `/extensions/OMI_physics_body/collisionFilters.length` | `int`             |
| `/extensions/OMI_physics_body/physicsMaterials.length` | `int`             |

## JSON Schema

See [schema/node.OMI_physics_body.collider.schema.json](schema/node.OMI_physics_body.collider.schema.json) for the collider properties JSON schema.

See [schema/glTF.OMI_physics_body.material.schema.json](schema/glTF.OMI_physics_body.material.schema.json) for the physics material properties JSON schema.

See [schema/glTF.OMI_physics_body.collision_filter.schema.json](schema/glTF.OMI_physics_body.collision_filter.schema.json) for the collision filter properties JSON schema.
