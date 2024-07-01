# KHR\_node\_selectability

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

This extension allows glTF animations and interactivity to control selectability of node hierarchies. It is intended for use in conjunction with `KHR_animation_pointer` and/or `KHR_interactivity` extensions, although it may be used independently.

## Concepts

### Selecting / Select action

The concept of “selecting” in this extension refers to a user interaction with an object in the scene. Typical examples of a “selection” action would be: a user clicking a mouse button while the mouse is somewhere over the 2D render of the scene, a user tapping with a finger on a point in the 2D render of the scene, a user using a six degree of freedom controller to point in a 3D scene and then pressing a trigger or other action button to indicate their intent to select, or a user using finger and hand gestures.

### Selection Ray

In all cases, we assume that a “selection” indicates that the user wishes to interact with a node in the scene that is along a specific ray, for example along the ray from the camera through the pixel and out into the scene, or along the ray from the controller out to the scene.

This ray will be called the “selection ray”.

### Selectability

In this section of this specification we will use the term “object” to refer to the geometry of a specific node (not including its subtree), whether defined by a mesh on that node or by any future extensions.

It is usually assumed that the user wishes to interact with the first object that intersects the selection ray. However, this may not always be the case. For example, a mostly transparent object representing and atmospheric effect like mist may be intended only for appearance, and a selection ray ought to skip this object and terminate at the first object thereafter.

The concept of “selectability” defines whether the selection ray should terminate at a given object. A selection action should be interpreted as applying to the first “selectable” object along the ray, with all earlier “unselectable” objects being skipped, regardless of the opacity, visibility or appearance of the objects.

## Extending Nodes

The `KHR_node_selectability` extension object is added to the objects within the `nodes` array. The extension object contains a single boolean `selectable` property. This value is mutable through JSON pointers as defined in the glTF 2.0 Asset Object Model and controls “selectability” of the node that contains it and all its children nodes recursively. A value of `false` causes all nodes below in the hierarchy to be omitted from “selection”, even any nodes below that have a value of `true`.

|                | Type      | Description                               | Required            |
|----------------|-----------|-------------------------------------------|---------------------|
| **selectable** | `boolean` | Specifies whether the node is selectable. | No, default: `true` |

In other words, a node is “selectable” if and only if there exists no node including or above it in the hierarchy where such node has a `selectable` property with value `false`. This allows a single change of a `selectable` property at a high level of the hierarchy to enable or disable the selection of complex (multi-node) objects.

In the following example, both nodes (and therefore their meshes) are initially unselectable.

```json
{
    "nodes": [
        {
            "children": [1],
            "mesh": 0,
            "extensions": {
                "KHR_node_selectability": {
                    "selectable": false
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

| Pointer                                                  | Type   |
|----------------------------------------------------------|--------|
| `/nodes/{}/extensions/KHR_node_selectability/selectable` | `bool` |

## Interaction with KHR_interactivity

Unless used in conjunction with `KHR_interactivity`, the result of a “select” action on a given object is left up to the implementation.

When used in conjunction with `KHR_interactivity`, a new interactivity event is defined as follows:

|                         |                  | |
|-------------------------|------------------|-|
| **Type**                | `event/onSelect` | Select event |
| **Configuration**       | `int nodeIndex`        | Index of a node |
|                         | `bool stopPropagation` | Whether to allow parents of this node to also receive the event |
| **Output values**       | `int selectedNodeIndex`  | Index of the actual node that was selected |
|                         | `float3 selectionPoint`   | Position of intersection of the select ray with the geometry of the selected node in global space |
|                         | `float3 selectionRayOrigin` | Position of the origin of the select ray in global space |
| **Output flow sockets** | `out` | The flow to be activated when a select event happens on the given node |

This interactivity event node is activated when a “select” event occurs on a glTF node `nodeIndex` or on any node in its subtree subject to the following propagation rule: the lowest node in the tree receives the select event first, and the event bubbles up the tree until a glTF node with an associated `event/onSelect` behavior graph node with its `stopPropagation` configuration value set to `true` is found.

If the `nodeIndex` configuration value is negative or greater than or equal to the number of glTF nodes in the asset, the `event/onSelect` node is invalid. A behavior graph **MUST NOT** contain two or more `event/onSelect` nodes with the same `nodeIndex` configuration value.

The internal state of this node consists of the `selectionPoint` and `selectionRayOrigin` output values initialized to NaN vectors and the `selectedNodeIndex` output value initialized to -1.

The output value sockets **MUST** be updated before activating the `out` output flow.

## JSON Schema

- [node.KHR_node_selectability.schema.json](schema/node.KHR_node_selectability.schema.json)
