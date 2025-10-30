# KHR\_node\_visibility

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

This extension allows to control visibility of node hierarchies. It is intended for use in conjunction with `KHR_animation_pointer` and/or interactivity extensions but can also be used on its own.

## Extending Nodes

The `KHR_node_visibility` extension object is added to the objects within the `nodes` array. The extension object contains a single boolean `visible` property. This value is mutable through JSON pointers as defined in the glTF 2.0 Asset Object Model and controls visibility of the node that contains it and all its children nodes recursively. A value of `false` causes all nodes below in the hierarchy to be omitted from display, even any nodes below that have a value of `true`.

When a node is not visible, all its visual features including but not limited to meshes, light sources (e.g., attached with `KHR_lights_punctual`), point clouds, particles, billboards, volumetric effects, etc., are not rendered. Visibility affects neither cameras, nor node's interactivity features such as selectability or hoverability.

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

## Appendix: Full Khronos Copyright Statement

Copyright 2025 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified
Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent,
trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this Specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
https://www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those
specifically identified sections define normative functionality. The Khronos Intellectual
Property Rights Policy excludes external references to materials and associated enabling
technology not created by Khronos from the Scope of this Specification, and any licenses
that may be required to implement such referenced materials and associated technologies
must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All
other product names, trademarks, and/or company names are used solely for identification
and belong to their respective owners.