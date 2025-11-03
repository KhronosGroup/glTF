# OMI_physics_body Trigger Property

A useful construct in a physics engine is a non-solid volume of space which does not generate impulses when overlapping with other volumes. These objects are typically called "triggers", "sensors", "phantoms", or "overlap volumes" in physics simulation engines. Triggers allow specifying such volumes either as a single shape or combination of nodes with shapes.

A trigger is added to a glTF node by specifying the `"trigger"` property inside of a node's `"OMI_physics_body"` extension.

A `"trigger"` may specify a `"shape"` property which references a geometric shape defined by the `OMI_physics_shape` extension. Alternatively, a `"trigger"` may have a `"nodes"` property, which is an array of glTF nodes which make up a compound trigger on this glTF node. The nodes in this array must be descendent nodes which must have `"trigger"` properties. At least one of `"shape"` or `"nodes"` must be set to a valid value.

As the name "trigger" suggests, implementations may use these shapes as sensors that generate overlap events, which can be used to trigger things. What behavior gets triggered is outside the scope of this extension, but may be defined in other glTF extensions or application-specific logic.

## Trigger Properties

|                     | Type        | Description                                                                                                         | Default value |
| ------------------- | ----------- | ------------------------------------------------------------------------------------------------------------------- | ------------- |
| **shape**           | `integer`   | The index of the shape to use as the trigger shape.                                                                 | -1            |
| **nodes**           | `integer[]` | For compound triggers, the set of descendant glTF nodes with a trigger property that make up this compound trigger. | []            |
| **collisionFilter** | `integer`   | The index of the filter information in the top level collisionFilters array.                                        | -1            |

### Shape

The `"shape"` property is an integer index that references a shape in the document-level shapes array as defined by the `OMI_physics_shape` extension. If not specified or -1, this node has no trigger shape, but may have `"nodes"` defined to create a compound trigger.

### Nodes

The `"nodes"` property is an array of integer indices that reference descendant glTF nodes with a trigger property, which make up a compound trigger on this glTF node. If not specified or empty, this node is not a compound trigger.

When this property is set and contains valid items, this indicates that this glTF node is a compound trigger. Each item is the index of a glTF node that must have its own OMI_physics_body trigger property, and must be a descendant of this node.

### Collision Filter

The `"collisionFilter"` property is an integer index that references a collision filter in the document-level collisionFilters array in the `OMI_physics_body` extension. If not specified or -1, the default collision filter is used.

Collision filters are used to determine which objects can collide with each other. A collision filter includes a set of collision systems (the "collision layer" the object is on), and a set of collision systems to collide with or to ignore (the "collision mask" of the object). See the below [Collision Filter Properties](#collision-filter-properties) section for more information on collision filters.

## Collision Filter Properties

The following properties may be defined in a collision filter in the top-level `collisionFilters` array:

|                           | Type       | Description                                                                         | Default value |
| ------------------------- | ---------- | ----------------------------------------------------------------------------------- | ------------- |
| **collisionSystems**      | `string[]` | An array of names of the collision systems of which this object is a member.        | `[]`          |
| **collideWithSystems**    | `string[]` | An array of names of the collision systems which this object can collide with.      | `[]`          |
| **notCollideWithSystems** | `string[]` | An array of names of the collision systems which this object does not collide with. | `[]`          |

### Collision Systems

The `"collisionSystems"` property is an array of strings representing the names of the collision systems or layers of which this object is a member. If not specified, the default value is an empty array, which means this filter is not a member of any collision systems, but may still collide with other objects.

### Collide With Systems

The `"collideWithSystems"` property is an array of strings representing the names of the collision systems or layers which this object can collide with. If not specified, the default value is an empty array.

If non-empty, objects using this filter should collide with only the collision systems in this array. This is also known as the "collision mask".

This property is mutually exclusive with the `"notCollideWithSystems"` property. If both are specified, the collision filter is invalid. If neither are specified, the object can collide with all systems.

### Not Collide With Systems

The `"notCollideWithSystems"` property is an array of strings representing the names of the collision systems or layers which this object does not collide with. If not specified, the default value is an empty array.

If non-empty, objects using this filter should collide with all collision systems except those in this array. This is the inverse of the `"collideWithSystems"` property, and represents an inverted "collision mask".

This property is mutually exclusive with the `"collideWithSystems"` property. If both are specified, the collision filter is invalid. If neither are specified, the object can collide with all systems.

## JSON Schema

See [schema/node.OMI_physics_body.trigger.schema.json](schema/node.OMI_physics_body.trigger.schema.json) for the trigger properties JSON schema.

See [schema/glTF.OMI_physics_body.collision_filter.schema.json](schema/glTF.OMI_physics_body.collision_filter.schema.json) for the collision filter properties JSON schema.
