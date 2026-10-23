# KHR_character_expression_joint

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta
- Sarah Cooney, Microsoft XGTG
- Aaron Franke, Independent Contributor

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Conventions

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **NOT RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in [BCP 14](https://www.rfc-editor.org/info/bcp14) when, and only when, they appear in all capitals, as shown here. Sections labeled (Informative), their subsections, and all examples are informative; all other sections are normative.

## Dependencies

Written against the glTF 2.0 specification.

Requires the extensions: `KHR_character` and `KHR_character_expression`.

Assets using `KHR_character_expression_joint` **MUST** list `KHR_character_expression_joint`, `KHR_character_expression`, and `KHR_character` in `extensionsUsed`. They **MUST** contain the top-level `KHR_character` and `KHR_character_expression` extension objects. A `KHR_character_expression_joint` object **MUST** be attached to an expression entry in that top-level `KHR_character_expression` object.

A listed channel **MAY** use `KHR_animation_pointer`. Assets using this form **MUST** list `KHR_animation_pointer` in `extensionsUsed`.

An asset **MAY** list `KHR_character_expression_joint` in `extensionsRequired`. A consumer claiming support for this extension **MUST** recognize its node-TRS channel classifications, **MUST** produce response records for every channel listed here that it supports under `KHR_character_expression` when the entry's effective driver is positive, and **MUST** implement additive translation aggregation as defined below. Under `KHR_character_expression`, channels listed here produce no response records on a consumer that does not support this extension. This support does not change base sampling. Support for `KHR_animation_pointer` remains independently necessary to emit a response for the pointer form.

## Overview (Informative)

This extension classifies listed channels of an expression animation as node-TRS channels and adds validation metadata. It does not change their samples or require the targeted nodes to be skin joints. Translation responses aggregate additively. Rotation and scale composition and the final mix with ordinary animation and other runtime systems are host-defined.

Channels listed here produce responses only on a consumer that supports this extension. A channel that no derived expression extension on the entry lists produces no response. Every channel in the selected animation still contributes to the base extension's duration.

## Extension structure (Normative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, nodes, accessors, buffers, and the referenced animation.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "smile",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_joint": {
              "channels": [0, 1, 2]
            }
          }
        }
      ]
    }
  }
}
```

### Properties

| Property | Type | Required | Description |
| --- | --- | --- | --- |
| `channels` | array | Yes | Nonempty array of unique channel indices in the animation selected by the containing expression entry. |

## Channel classification and validity (Normative)

Each value in `channels` **MUST** be a valid index into the `channels` array of the animation selected by the containing expression entry's `animation` property.

Each listed channel **MUST** target node `translation`, `rotation`, or `scale` using one of these forms:

- An ordinary core target with a defined, valid `target.node` and `target.path` equal to `"translation"`, `"rotation"`, or `"scale"`.
- A valid `KHR_animation_pointer` target resolving to `/nodes/{node}/translation`, `/nodes/{node}/rotation`, or `/nodes/{node}/scale`.

For either form, the target node cannot define `matrix` (see `KHR_character_expression`, Channel targets).

The target node is not required to occur in any `skin.joints` array and is not required to influence a skinned mesh. The word “joint” in this extension's name is a classification label, not an additional skin-membership constraint.

Each listed channel and its sampler **MUST** satisfy all core animation rules, any `KHR_animation_pointer` rules, and all `KHR_character_expression` rules. In particular:

- the output accessor of a channel with an ordinary core target has an accessor type and component type that core glTF allows for the channel's `target.path`;
- the output accessor of a channel with a `KHR_animation_pointer` target has an accessor type and component type that `KHR_animation_pointer` allows for the targeted property, and its values are converted as that extension specifies; and
- in both forms, rotation output values are quaternions subject to core glTF's quaternion validity rules.

## Response handling and runtime composition (Normative)

Each listed channel produces the ordinary absolute node-TRS sample defined by `KHR_character_expression`. Listing a channel here does not reinterpret its response as a transform delta or blend weight, or change its driver-selected sample time.

Multiple expression entries **MAY** target the same node's `translation`, `rotation`, or `scale`. Node translation uses the additive formula independently for its three components, and its aggregate is subject to the no-clamp rule (see `KHR_character_expression`, Additive aggregation). This extension does not prescribe quaternion blending, scale multiplication, override priority, a reference pose, or another method for combining rotation or scale responses; these use Other composition in the base extension. The final mix with ordinary animation or other non-expression property writers remains host-defined (see `KHR_character_expression`, Host integration boundary). The within-animation duplicate-target prohibition and all target-property validity rules continue to apply.

## Authoring guidance (Informative)

Several expression entries can translate, rotate, or scale the same node. A rig can use `jawOpen` and `jawLeft` on one jaw joint or several gaze directions on one eye joint. Helper-node chains are optional: authors can use them when a particular fixed parent-to-child transform order is desired, with each helper's origin at the joint's pivot and each expression targeting its own helper. Host mixing on one node can differ from that hierarchy-based result or from another format's yaw/pitch algorithm.

For example, separate `lookUp` and `lookRight` expression animations can each contain one rotation channel targeting the same `leftEye` node. A zero driver produces no rotation response. A positive driver produces that animation's ordinary absolute sampled rotation. When both drivers are positive, both animations are sampled independently at their driver-selected times; the host decides how their responses affect the eye. The two channels remain in separate animations, where overlap is permitted; the core prohibition on duplicate targets within one animation still applies.

For hosts that blend scale responses, linear interpolation is a useful choice, and normalized linear interpolation (nlerp) can be useful for rotations. Quaternion blending needs care with hemisphere/sign handling because `q` and `-q` represent the same rotation. Reference values, weights, ordering, neutral-response behavior, and the chosen method can affect the result; these are host choices for rotation and scale. They do not change the required additive aggregation of translation responses. The driver already selects the response-curve sample time; subsequent host blending is a separate operation.

An exporter that bakes translation, rotation, and scale channels for every animated node can list only the channels that actually move. A listed channel that holds its node at its authored value still produces a response when the effective driver is positive. Listing such channels on several entries is valid. A constant translation channel contributes zero additive change from its time-zero sample; the effect of constant rotation or scale responses depends on the host's mixing policy. Left unlisted, a channel produces no response record and still contributes to the base extension's duration.

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## Schema

- [expression.KHR_character_expression_joint.schema.json](./schema/expression.KHR_character_expression_joint.schema.json)

A `KHR_character_expression_joint` object is placed in the `extensions` property of an expression entry, keyed by the extension name, rather than on a core glTF object. A validator matches the object to the schema above by that key.

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
