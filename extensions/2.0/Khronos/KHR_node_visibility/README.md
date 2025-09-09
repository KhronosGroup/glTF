# KHR\_node\_visibility

## Contributors

- Dwight Rodgers, Adobe
- Peter Martin, Adobe
- Emmett Lalish, Google
- Alexey Knyazev, Independent

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF animations and interactivity to control visibility of node hierarchies. It is intended for use in conjunction with `KHR_animation_pointer` and/or `KHR_interactivity` extensions.

## Extending Nodes

The `KHR_node_visibility` extension object is added to the objects within the `nodes` array. The extension object contains a single boolean `visible` property. This value is mutable through JSON pointers as defined in the glTF 2.0 Asset Object Model and controls visibility of the node that contains it and all its children nodes recursively. A value of `false` causes all nodes below in the hierarchy to be omitted from display, even any nodes below that have a value of `true`.

| Property    | Type      | Description                            | Required            |
|-------------|-----------|----------------------------------------|---------------------|
| **visible** | `boolean` | Specifies whether the node is visible. | No, default: `true` |

In other words, a node is visible if and only if its own `visible` property is `true` and all its parents are visible. This allows a single change of a `visible` property at a high level of the hierarchy to hide or show complex (multi-node) objects.

In the following example, both nodes (and therefore their meshes) are initially hidden.

```json
{
    "nodes": [
        {
            "children": [1],
            "mesh": 0,
            "extensions": {
                "KHR_node_visibility": {
                    "visible": false
                }
            },
        },
        {
            "mesh": 1
        }
    ]
}
```

## Extending glTF 2.0 Asset Object Model

The following pointer template represents the mutable property defined by this extension.

| Pointer                                            |  Type  |
|----------------------------------------------------|--------|
| `/nodes/{}/extensions/KHR_node_visibility/visible` | `bool` |

## JSON Schema

- [node.KHR_node_visibility.schema.json](schema/node.KHR_node_visibility.schema.json)
