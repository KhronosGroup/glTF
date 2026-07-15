# KHR\_node\_selectability

## Contributors

- Dwight Rodgers, Adobe
- Peter Martin, Adobe
- Emmett Lalish, Google
- Alexey Knyazev, Independent

## Status

Release Candidate

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF animations and interactivity to control selectability of glTF node hierarchies. It is intended for use in conjunction with `KHR_animation_pointer` and/or `KHR_interactivity` extensions, although it may be used independently.

## Concepts

### Selecting / Select action

The concept of “selecting” in this extension refers to a user interaction with an object in the scene. Typical examples of a “selection” action would be: a user clicking a mouse button while the mouse is somewhere over the 2D render of the scene, a user tapping with a finger on a point in the 2D render of the scene, a user using a six degree of freedom controller to point in a 3D scene and then pressing a trigger or other action button to indicate their intent to select, or a user using finger and hand gestures.

### Selection Ray

A “selection” indicates that the user wishes to interact with a glTF node in the scene that is along a specific ray, for example along the ray from the camera through the pixel and out into the scene, or along the ray from the controller out to the scene.

This ray will be called the “selection ray”.

Implementations that do not use a ray-based approach but have a concept similar to selection **MAY** issue this event as deemed appropriate.

### Selectability

In this section of this specification, the term “object” is used to refer to the geometry of a specific glTF node (not including its subtree), whether defined by a mesh on that glTF node or by any future extensions.

It is usually assumed that the user wishes to interact with the first object that intersects the selection ray. However, this may not always be the case. For example, a mostly transparent object representing an atmospheric effect like mist may be intended only for appearance, and a selection ray ought to skip this object and terminate at the first object thereafter.

The concept of “selectability” defines whether the selection ray should terminate at a given object. A selection action should be interpreted as applying to the first “selectable” object along the ray, with all earlier “unselectable” objects being skipped, regardless of the opacity, visibility, or appearance of the objects.

## Extending Nodes

The `KHR_node_selectability` extension object is added to the objects within the `nodes` array. The extension object contains a single boolean `selectable` property. This value is mutable through JSON pointers as defined in the glTF 2.0 Asset Object Model and controls “selectability” of the glTF node that contains it and all its children nodes recursively. A value of `false` causes all nodes below in the hierarchy to be omitted from “selection”, even any nodes below that have a value of `true`.

|                | Type      | Description                               | Required            |
|----------------|-----------|-------------------------------------------|---------------------|
| **selectable** | `boolean` | Specifies whether the node is selectable. | No, default: `true` |

In other words, a glTF node is “selectable” if and only if there exists no glTF node including or above it in the hierarchy where such node has a `selectable` property with value `false`. This allows a single change of a `selectable` property at a high level of the hierarchy to enable or disable the selection of complex (multi-node) objects.

In the following example, both glTF nodes (and therefore their meshes) are initially unselectable.

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

When used in conjunction with `KHR_interactivity`, a new interactivity operation is defined as follows:

|                         |                             | |
|-------------------------|-----------------------------|-|
| **Operation**           | `event/onSelect`            | Select event |
| **Configuration**       | `int nodeIndex`             | Index of the glTF node that has this event handler |
| **Output values**       | `ref selectedNode`          | Reference to the glTF node that was selected by the user |
|                         | `int controllerIndex`       | Index of the controller that generated the event |
|                         | `float3 selectionPoint`     | Position of intersection of the selection ray with the geometry of the selected glTF node in global space |
|                         | `float3 selectionRayOrigin` | Position of the origin of the selection ray in global space |
|                         | `ref event`                 | The event reference |
| **Output flow sockets** | `out` | The flow to be activated when the select event happens |

In the default configuration, the `nodeIndex` configuration value is undefined and the interactivity node is never activated.

If the `nodeIndex` configuration property is not provided by the behavior graph, if it is not a literal number, if its value is not exactly representable as a 32-bit signed integer, or if it is negative or greater than or equal to the number of glTF nodes in the asset, the default configuration **MUST** be used.

Interactivity nodes with this operation are activated when a “select” event occurs on a glTF node `nodeIndex` or on any glTF node in its subtree subject to the following propagation rule: the lowest glTF node in the tree receives the select event first, and the event bubbles up the tree until a glTF node with an associated `event/onSelect` interactivity node is found; the interactivity node is activated and the propagation continues.

The internal state of this interactivity node consists of the `selectionPoint` and `selectionRayOrigin` output values initialized to NaN vectors, the `selectedNode` and `controllerIndex` output values initialized to null and -1 respectively, and the `event` output value initialized to null.

In the case of multiple-controller systems, the `controllerIndex` output value **MUST** be set to the index of the controller that generated the event; in single-controller systems, this output value **MUST** be set to zero.

If the implementation provides selection ray information, the `selectionPoint` and `selectionRayOrigin` output values **MUST** be updated with it; otherwise, they **MUST** be set to NaN vectors.

The output value sockets **MUST** be updated before activating the `out` output flow.

If multiple interactivity nodes with this operation and the same configuration exist in the graph, they **MUST** be activated sequentially in the order they appear in JSON and they **MUST** have the same output values within the same event occurrence.

## Interaction with KHR\_node\_hoverability

If an implementation supports both `KHR\_node\_selectability` and `KHR\_node\_hoverability` extensions, it **MUST** use the same object testing conditions for all events defined in these extensions.

## JSON Schema

- [node.KHR_node_selectability.schema.json](schema/node.KHR_node_selectability.schema.json)
