# OMI_physics_body Trigger Property

If a node has the `"trigger"` property defined, it has a non-solid trigger shape that can detect when objects enter it.

Triggers are not solid and do not "collide" with other objects, but can generate events when another physics body "enters" them. For example, a "goal" area which triggers whenever a ball gets thrown into it.

Trigger nodes are recommended to not be defined on the same node that defines `"motion"` or `"collider"`. This results in a very clear, simple, and portable document structure. If a trigger is desired to have motion, define the trigger on a child node of the node with the motion property.

## Trigger Properties

|           | Type      | Description                                         | Default value |
| --------- | --------- | --------------------------------------------------- | ------------- |
| **shape** | `integer` | The index of the shape to use as the trigger shape. | -1            |

### Shape

The `"shape"` property is an integer index that references a shape in the document-level shapes array as defined by the `OMI_physics_shape` extension. If not specified or -1, this node has no trigger shape, but may be the parent of other nodes that do have trigger shapes, and should combine those nodes into one trigger (this may be a body or compound trigger depending on the engine).

## JSON Schema

See [schema/node.OMI_physics_body.trigger.schema.json](schema/node.OMI_physics_body.trigger.schema.json) for the trigger properties JSON schema.
