# KHR_node_lookat_target

## Contributors

- Ken Jakubzak, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Conventions

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **NOT RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in [BCP 14](https://www.rfc-editor.org/info/bcp14) when, and only when, they appear in all capitals, as shown here. Sections labeled (Informative), their subsections, and all examples are informative; all other sections are normative.

## Dependencies

Written against the glTF 2.0 specification.

This extension has no extension dependencies.

## Overview (Informative)

`KHR_node_lookat_target` identifies a glTF node as a passive positional marker. A host can discover the marker and supply its evaluated global-space point to a separately defined downstream system, such as a camera, constraint, animation, interaction, or application system.

The marker has no side effects. It does not select a downstream system, establish a link, orient a node, move eyes or a head, run inverse kinematics (IK), activate or control a camera, follow another object, or define priority, timing, smoothing, constraints, tours, signage, networking, or shared-attention behavior.

A node name or an `extras` entry can identify a target for one application, but it conveys nothing to a consumer that does not recognize that convention; core glTF uses names for application-specific purposes such as display and does not require them to be unique, and it defines `extras` only as application-specific data. This extension instead gives look-at targets a standard, discoverable structure. A consumer that supports it recognizes each marker and makes the marker and its target point available to host logic, regardless of the node's name or `extras`; support does not require anything to respond to the marker. When an asset lists the extension in `extensionsRequired`, a conforming consumer either exposes its markers or treats the asset as unsupported. A marker node can also be the `targetNode` of a `KHR_node_camera_hint`, and neither extension requires the other.

## Extension usage

The extension object **MUST** be attached to a glTF node. An asset using this extension **MUST** list `KHR_node_lookat_target` in `extensionsUsed`.

An asset **MAY** list `KHR_node_lookat_target` in `extensionsRequired`. A consumer claiming support for this extension **MUST** recognize each marker, interpret the evaluated node instance's global translation as its target point, and make the marker and point available to host logic. Support does not require the consumer to select the marker or cause another object to respond to it. A consumer that cannot provide this minimum behavior **MUST** treat an asset listing the extension in `extensionsRequired` as unsupported according to the glTF 2.0 extension rules.

The annotated node remains an ordinary node. Its mesh, skin, camera, children, animation, and other extensions retain their normal meanings. In particular, this annotation does not hide or replace ordinary content attached to the node.

### Extension object

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `hint` | string | No | — | Nonempty advisory label suggesting an intended use. This extension defines no standard values; see [Advisory hint](#advisory-hint). |

The extension object **MAY** be empty. Multiple nodes **MAY** carry the extension, and multiple markers **MAY** have the same `hint` value. Array order and node traversal order do not define priority or selection among markers; marker selection is host-defined, as described in [Host-defined behavior](#host-defined-behavior).

## Target-point semantics

For an evaluated instance `i` of an annotated node in a host-provided evaluation state `S`, let `globalTransform(i, S)` be the node instance's global transform after applying ordinary glTF scene-graph rules and the animation state in `S`. The marker's target point is the translation component of that transform:

```text
targetPoint(i, S) = translation(globalTransform(i, S))
```

Equivalently, the target point is the global-space position of the annotated node's local origin. Parent transforms therefore affect the point. The annotated node's own rotation and scale do not define a direction, volume, radius, or orientation for this extension.

The result is evaluated independently for each node instance. An annotated node with no evaluated instance provides no target point. The extension does not select a scene, instance, animation, animation-local time, weight, or composition policy; the host supplies the instance `i` and the evaluation state `S`. Animation of the annotated node or any ancestor retains ordinary core glTF meaning, and this extension adds no animation-mixing rules.

## Advisory hint

`hint` is an advisory label suggesting an intended use. This extension defines no standard `hint` values. Values are exact and case-sensitive; for example, `"ACME_eye_target"` and `"acme_eye_target"` are different values. Values **SHOULD** be vendor-qualified, for example `"ACME_eye_target"`, `"ACME_camera_target"`, or `"ACME_aim_target"`, to reduce collisions. This extension does not define what any particular value classifies; that meaning comes only from a separate specification or application profile that defines the value. A value can help a host present or filter markers, but this extension assigns no behavior to any value.

A consumer **MUST NOT** reject a marker because it does not recognize the `hint` value. An unrecognized hint and an omitted hint both leave the marker's target-point semantics unchanged. Any portable selection, linkage, or response behavior requires a separate specification.

## Host-defined behavior

The host decides:

- which scene, node instance, animations, local times, weights, and composition policy to use;
- whether to select, filter, or use a discovered marker;
- which downstream system, if any, uses a selected point;
- how a downstream system establishes links or resolves multiple markers;
- orientation, axes, roll, limits, weights, priorities, activation, and constraints;
- animation drivers, gaze, IK, camera, following, and interaction behavior; and
- downstream update cadence, additional interpolation, smoothing, transitions, and networking.

These choices are not conformance behavior for this extension.

## Complete example (Informative)

The child marker below has local translation `[0.0, 1.5, 2.0]`. Its parent contributes `[1.0, 0.0, 0.0]`, so its global target point in the static scene is `[1.0, 1.5, 2.0]`.

```json
{
  "asset": { "version": "2.0" },
  "scene": 0,
  "scenes": [
    { "nodes": [0] }
  ],
  "extensionsUsed": [
    "KHR_node_lookat_target"
  ],
  "nodes": [
    {
      "name": "ObjectRoot",
      "translation": [1.0, 0.0, 0.0],
      "children": [1]
    },
    {
      "name": "FocusPoint",
      "translation": [0.0, 1.5, 2.0],
      "extensions": {
        "KHR_node_lookat_target": {
          "hint": "ACME_eye_target"
        }
      }
    }
  ]
}
```

The `ACME_eye_target` string does not cause eye movement. It remains advisory input that a host may use when selecting among markers.

## Ordinary node content example (Informative)

The following partial fragment omits the `asset` object and the referenced mesh data. It demonstrates that a marker may be attached to a node that also has ordinary visual content.

```json
{
  "extensionsUsed": ["KHR_node_lookat_target"],
  "nodes": [
    {
      "mesh": 0,
      "extensions": {
        "KHR_node_lookat_target": {}
      }
    }
  ]
}
```

## Interaction with other systems (Informative)

Another extension or application may refer to the same node or discover it by convention. The presence of this marker does not itself establish that relationship. In particular, a `KHR_node_camera_hint.targetNode` index has its own meaning and does not require the referenced node to carry `KHR_node_lookat_target`.

## Validation boundary

The JSON Schema enforces only the extension object's local structure: `hint` is a nonempty string when present, and an empty extension object is valid.

A validator that claims to validate this extension **MUST** verify placement on a node and the ordinary `extensionsUsed` and `extensionsRequired` declarations. It does not select downstream systems, evaluate host-chosen animation state, or validate gaze, IK, camera, constraint, or application behavior.

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and marker demo.

## Known limitations (Informative)

- The extension provides no portable link from a marker to a downstream system.
- `hint` values have no portable behavior.
- Marker selection, priority, and response behavior are host-defined.

## Schema

- [node.KHR_node_lookat_target.schema.json](./schema/node.KHR_node_lookat_target.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
