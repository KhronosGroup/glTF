# KHR\_node\_hoverability

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

This extension allows glTF animations and interactivity to control hoverability of node hierarchies. It is intended for use in conjunction with `KHR_animation_pointer` and/or `KHR_interactivity` extensions, although it may be used independently.

## Concepts

### Hovering / Hover action

The concept of “hovering” in this extension refers to a user interaction with an object in the scene. Typical examples of a “hover” action would be: a user moving a mouse over the 2D render of the scene, a user holding a finger a few millimeters away from a touch screen above a point in the 2D render of the scene, a user using a six degree of freedom controller to point in a 3D scene, or a user using finger to point at an object.

A hover action has a “start” and a “stop” when the user begins and ends the process of hovering on a specific object.

A system with multiple controllers may have separate hover actions as each controller moves.

### Hover Ray

In all cases, we assume that a “hover” action indicates that the user wishes to interact with a node in the scene that is along a specific ray, for example along the ray from the camera through the pixel and out into the scene, or along the ray from the controller out to the scene.

This ray will be referred to as the “hover ray”.

Implementations that do not use a ray-based approach but have a concept similar to hover **MAY** issue these events as deemed appropriate.

### Hoverability

In this section of this specification we will use the term “object” to refer to the geometry of a specific node (not including its subtree), whether defined by a mesh on that node or by any future extensions.

It is usually assumed that the user wishes to interact with the first object that intersects the hover ray. However, this may not always be the case. For example, a mostly transparent object representing and atmospheric effect like mist may be intended only for appearance, and a hover ray ought to skip this object and terminate at the first object thereafter.

The concept of “hoverability” defines whether the hover ray should terminate at a given object. A hover action should be interpreted as applying to the first “hoverable” object along the ray, with all earlier “unhoverable” objects being skipped, regardless of the opacity, visibility, or appearance of the objects.

## Extending Nodes

The `KHR_node_hoverability` extension object is added to the objects within the `nodes` array. The extension object contains a single boolean `hoverable` property. This value is mutable through JSON pointers as defined in the glTF 2.0 Asset Object Model and controls “hoverability” of the node that contains it and all its children nodes recursively. A value of `false` causes all nodes below in the hierarchy to be omitted from “hover” actions, even any nodes below that have a value of `true`.

|               | Type      | Description                              | Required            |
|---------------|-----------|------------------------------------------|---------------------|
| **hoverable** | `boolean` | Specifies whether the node is hoverable. | No, default: `true` |

In other words, a node is “hoverable” if and only if there exists no node including or above it in the hierarchy where such node has a `hoverable` property with value `false`. This allows a single change of a `hoverable` property at a high level of the hierarchy to enable or disable the “hover” actions of complex (multi-node) objects.

In the following example, both nodes (and therefore their meshes) are initially unhoverable.

```json
{
    "nodes": [
        {
            "children": [1],
            "mesh": 0,
            "extensions": {
                "KHR_node_hoverability": {
                    "hoverable": false
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

| Pointer                                                | Type   |
|--------------------------------------------------------|--------|
| `/nodes/{}/extensions/KHR_node_hoverability/hoverable` | `bool` |

## Interaction with KHR_interactivity

Unless used in conjunction with `KHR_interactivity`, the result of a “hover” action on a given object is left up to the implementation.

When used in conjunction with `KHR_interactivity`, two new interactivity events, `event/onHoverIn` and `event/onHoverOut`, are defined as follows:

|                         |                   | |
|-------------------------|-------------------|-|
| **Type**                | `event/onHoverIn` | Hover In event |
| **Configuration**       | `int nodeIndex`        | Index of a node that has this event handler |
|                         | `bool stopPropagation` | Whether to allow parents of this node to also receive the event |
| **Output values**       | `int hoverNodeIndex`  | Index of the actual node on which “hover” action was started |
|                         | `int controllerIndex` | Index of the controller that generated the event |
| **Output flow sockets** | `out` | The flow to be activated when a “hover” event happens on the given node |

This interactivity event node is activated when a “hover” action is started on a glTF node `nodeIndex` or on any node in its subtree subject to the following propagation rule: the lowest node in the tree receives the “hover” event first, and the event bubbles up the tree until a glTF node with the associated `event/onHoverIn` behavior graph node with its `stopPropagation` configuration value set to `true` is found or until the currently processed node's subtree includes both the prior and the current hover targets for the given controller, whichever occurs first. In other words, a glTF node N only receives a new Hover In event when a “hover” action enters N’s subtree, not on each change of state within that subtree.

If the `nodeIndex` configuration value is negative or greater than or equal to the number of glTF nodes in the asset, the `event/onHoverIn` node is invalid. A behavior graph **MUST NOT** contain two or more `event/onHoverIn` nodes with the same `nodeIndex` configuration value.

The internal state of this node consists of the `hoverNodeIndex` and `controllerIndex` output values initialized to -1.

In the case of multiple-controller systems, the `controllerIndex` output value **MUST** be set to the index of the controller that generated the event; in single-controller systems, this output value **MUST** be set to zero.

The output value sockets **MUST** be updated before activating the `out` output flow.

|                         |                    | |
|-------------------------|--------------------|-|
| **Type**                | `event/onHoverOut` | Hover Out event |
| **Configuration**       | `int nodeIndex`        | Index of a node that has this event handler |
|                         | `bool stopPropagation` | Whether to allow parents of this node to also receive the event |
| **Output values**       | `int hoverNodeIndex`  | Index of the actual node on which “hover” action was initiated |
|                         | `int controllerIndex` | Index of the controller that generated the event |
| **Output flow sockets** | `out` | The flow to be activated when a “hover” event happens on the given node |

This interactivity event node is activated when a “hover” action is stopped on a glTF node `nodeIndex` or on any node in its subtree subject to the following propagation rule: the lowest node in the tree receives the “hover” event first, and the event bubbles up the tree until a glTF node with the associated `event/onHoverOut` behavior graph node with its `stopPropagation` configuration value set to `true` is found or until the currently processed node's subtree includes both the prior and the current hover targets for the given controller, whichever occurs first. In other words, a glTF node N only receives a new Hover Out event when a “hover” action exits N’s entire subtree, not on each change of state within that subtree.

This node has the same validation and internal state rules as `event/onHoverIn`.

## JSON Schema

- [node.KHR_node_hoverability.schema.json](schema/node.KHR_node_hoverability.schema.json)
