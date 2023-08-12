# OMI_physics_body Collider Property

If a node has the `"collider"` property defined, it is a solid collider node that objects can collide with.

Collider nodes are recommended to be direct children of the "body" node (the one that defines motion), if any. This results in a very clear, simple, and portable document structure, which ensures each collider is transformed relative to the body via a single glTF node transform. However, collider nodes may also be defined as indrect descendants of the body node, and may be defined on the same node as the body node. If multiple ancestors are "body" nodes, only the nearest ancestor uses the collider. Implementations must support all of these cases in order to be compliant.

## Collider Properties

|           | Type      | Description                                           | Default value |
| --------- | --------- | ----------------------------------------------------- | ------------- |
| **shape** | `integer` | The index of the shape to use as the collision shape. | -1            |

### Shape

The `"shape"` property is an integer index that references a shape in the document-level shapes array as defined by the `OMI_physics_shape` extension. If not specified or -1, this node has no collider shape, but may be the parent of other nodes that do have collider shapes, and can combine those nodes into one collider (this may be a body or compound collider depending on the engine).

## JSON Schema

See [schema/node.OMI_physics_body.collider.schema.json](schema/node.OMI_physics_body.collider.schema.json) for the collider properties JSON schema.
