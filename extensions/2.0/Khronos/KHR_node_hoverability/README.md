# KHR\_node\_hoverability

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

This extension allows glTF animations and interactivity to control hoverability of glTF node hierarchies. It is intended for use in conjunction with `KHR_animation_pointer` and/or `KHR_interactivity` extensions, although it may be used independently.

## Concepts

### Hovering / Hover action

The concept of “hovering” in this extension refers to a user interaction with an object in the scene. Typical examples of a “hover” action would be: a user moving a mouse over the 2D render of the scene, a user holding a finger a few millimeters away from a touch screen above a point in the 2D render of the scene, a user using a six degree of freedom controller to point in a 3D scene, or a user using finger to point at an object.

A hover action has a “start” and a “stop” when the user begins and ends the process of hovering on a specific object.

A system with multiple controllers may have separate hover actions as each controller moves.

### Hover Ray

A “hover” action indicates that the user wishes to interact with a glTF node in the scene that is along a specific ray, for example along the ray from the camera through the pixel and out into the scene, or along the ray from the controller out to the scene.

This ray will be referred to as the “hover ray”.

Implementations that do not use a ray-based approach but have a concept similar to hover **MAY** issue these events as deemed appropriate.

### Hoverability

In this section of this specification, the term “object” is used to refer to the geometry of a specific glTF node (not including its subtree), whether defined by a mesh on that glTF node or by any future extensions.

It is usually assumed that the user wishes to interact with the first object that intersects the hover ray. However, this may not always be the case. For example, a mostly transparent object representing and atmospheric effect like mist may be intended only for appearance, and a hover ray ought to skip this object and terminate at the first object thereafter.

The concept of “hoverability” defines whether the hover ray should terminate at a given object. A hover action should be interpreted as applying to the first “hoverable” object along the ray, with all earlier “unhoverable” objects being skipped, regardless of the opacity, visibility, or appearance of the objects.

## Extending Nodes

The `KHR_node_hoverability` extension object is added to the objects within the `nodes` array. The extension object contains a single boolean `hoverable` property. This value is mutable through JSON pointers as defined in the glTF 2.0 Asset Object Model and controls “hoverability” of the glTF node that contains it and all its children nodes recursively. A value of `false` causes all nodes below in the hierarchy to be omitted from “hover” actions, even any nodes below that have a value of `true`.

|               | Type      | Description                              | Required            |
|---------------|-----------|------------------------------------------|---------------------|
| **hoverable** | `boolean` | Specifies whether the node is hoverable. | No, default: `true` |

In other words, a glTF node is “hoverable” if and only if there exists no glTF node including or above it in the hierarchy where such node has a `hoverable` property with value `false`. This allows a single change of a `hoverable` property at a high level of the hierarchy to enable or disable the “hover” actions of complex (multi-node) objects.

In the following example, both glTF nodes (and therefore their meshes) are initially unhoverable.

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

When used in conjunction with `KHR_interactivity`, two new interactivity operations, `event/onHoverIn` and `event/onHoverOut`, are defined as follows:

|                         |                       | |
|-------------------------|-----------------------|-|
| **Operation**           | `event/onHoverIn`     | Hover In event |
| **Configuration**       | `int nodeIndex`       | Index of the glTF node that has this event handler |
| **Output values**       | `ref hoveredNode`     | Reference to the glTF node on which “hover” action was started |
|                         | `int controllerIndex` | Index of the controller that generated the event |
|                         | `ref event`           | The event reference |
| **Output flow sockets** | `out`                 | The flow to be activated when the Hover In event happens |

In the default configuration, the `nodeIndex` configuration value is undefined and the interactivity node is never activated.

If the `nodeIndex` configuration property is not provided by the behavior graph, if it is not a literal number, if its value is not exactly representable as a 32-bit signed integer, or if it is negative or greater than or equal to the number of glTF nodes in the asset, the default configuration **MUST** be used.

Interactivity nodes with this operation are activated when a “hover” action is started on a glTF node `nodeIndex` or on any glTF node in its subtree subject to the following propagation rule: the lowest glTF node in the tree receives the “hover” event first, and the event bubbles up the tree until a glTF node with the associated `event/onHoverIn` interactivity node is found or until the currently processed glTF node's subtree includes both the prior and the current hover targets for the given controller, whichever occurs first; the interactivity node is activated and the propagation continues. In other words, a glTF node N only receives a new Hover In event when a “hover” action enters N’s subtree, not on each change of state within that subtree.

The internal state of this interactivity node consists of the `hoveredNode`, `controllerIndex`, and `event` output values initialized to null, -1, and null respectively.

In the case of multiple-controller systems, the `controllerIndex` output value **MUST** be set to the index of the controller that generated the event; in single-controller systems, this output value **MUST** be set to zero.

The output value sockets **MUST** be updated before activating the `out` output flow.

If multiple interactivity nodes with this operation and the same configuration exist in the graph, they **MUST** be activated sequentially in the order they appear in JSON and they **MUST** have the same output values within the same event occurrence.

|                         |                       | |
|-------------------------|-----------------------|-|
| **Operation**           | `event/onHoverOut`    | Hover Out event |
| **Configuration**       | `int nodeIndex`       | Index of the glTF node that has this event handler |
| **Output values**       | `ref hoveredNode`     | Reference to the glTF node on which “hover” action was initiated |
|                         | `int controllerIndex` | Index of the controller that generated the event |
|                         | `ref event`           | The event reference |
| **Output flow sockets** | `out`                 | The flow to be activated when the Hover Out event happens |

In the default configuration, the `nodeIndex` configuration value is undefined and the interactivity node is never activated.

If the `nodeIndex` configuration property is not provided by the behavior graph, if it is not a literal number, if its value is not exactly representable as a 32-bit signed integer, or if it is negative or greater than or equal to the number of glTF nodes in the asset, the default configuration **MUST** be used.

Interactivity nodes with this operation are activated when a “hover” action is stopped on a glTF node `nodeIndex` or on any glTF node in its subtree subject to the following propagation rule: the lowest glTF node in the tree receives the “hover” event first, and the event bubbles up the tree until a glTF node with the associated `event/onHoverOut` interactivity node is found or until the currently processed glTF node's subtree includes both the prior and the current hover targets for the given controller, whichever occurs first; the interactivity node is activated and the propagation continues. In other words, a glTF node N only receives a new Hover Out event when a “hover” action exits N’s entire subtree, not on each change of state within that subtree.

The internal state of this interactivity node consists of the `hoveredNode`, `controllerIndex`, and `event` output values initialized to null, -1, and null respectively.

In the case of multiple-controller systems, the `controllerIndex` output value **MUST** be set to the index of the controller that generated the event; in single-controller systems, this output value **MUST** be set to zero.

The output value sockets **MUST** be updated before activating the `out` output flow.

If multiple interactivity nodes with this operation and the same configuration exist in the graph, they **MUST** be activated sequentially in the order they appear in JSON and they **MUST** have the same output values within the same event occurrence.

### Invariant event triggering

The “hover” actions **MUST** occur regardless of the reason that caused the “hover ray” to intersect the “object”. Such reasons include, but are not limited to, user actions, object or camera movements, and toggling the `hoverable` property.

## Interaction with KHR\_node\_selectability

If an implementation supports both `KHR\_node\_hoverability` and `KHR\_node\_selectability` extensions, it **MUST** use the same object testing conditions for all events defined in these extensions.

## JSON Schema

- [node.KHR_node_hoverability.schema.json](schema/node.KHR_node_hoverability.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
