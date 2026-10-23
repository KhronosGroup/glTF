# KHR_character_skeleton_mapping

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

Requires the extension: `KHR_character`.

Assets using `KHR_character_skeleton_mapping` **MUST** list `KHR_character_skeleton_mapping` and `KHR_character` in `extensionsUsed`. They **MUST** contain top-level `KHR_character_skeleton_mapping` and `KHR_character` extension objects.

## Overview (Informative)

The `KHR_character_skeleton_mapping` extension associates roles from one or more externally defined skeletal vocabularies with nodes in a glTF asset. A mapping can help tools discover which node an author associates with a role such as `hips`, `head`, or `leftHand`.

The extension stores associations only. It does not define a skeleton topology, require a skin, identify skin joints, define inverse kinematics (IK), or provide an animation-retargeting algorithm.

## Extension usage

The `KHR_character_skeleton_mapping` object **MUST** be attached to the top-level glTF object. An asset **MAY** list the extension in `extensionsRequired`.

A consumer claiming support for this extension **MUST** resolve each `node` index and make every mapping set, and the association between each vocabulary role and its referenced node, available to host logic. Support for this extension does not imply that the consumer understands a particular external vocabulary or can perform retargeting or IK. A consumer that cannot resolve the associations and make them available to host logic **MUST** treat an asset listing `KHR_character_skeleton_mapping` in `extensionsRequired` as unsupported, according to the glTF 2.0 extension rules.

Requiring this extension guarantees only the generic association contract defined here. It does not require support for any particular vocabulary identified by a mapping-set identifier.

Mapping-set selection, and the application of a mapping set to a particular skin, skeleton, or other part of the asset, are host-defined.

Because `skeletalRigMappings` is keyed by mapping-set identifier and each role identifier is associated with one node, a role is associated with at most one node per mapping-set identifier, even when the asset contains several skeletons.

## Mapping-set and role semantics

The extension object **MUST** contain `skeletalRigMappings`, a nonempty object. Each property name in that object is a mapping-set identifier, and its value is a nonempty mapping-set object.

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `skeletalRigMappings` | object | Yes | Nonempty dictionary of mapping sets, keyed by mapping-set identifier. |

Each mapping-set identifier names the external skeletal vocabulary of its mapping set. Every mapping-set identifier **MUST** be nonempty; this extension places no other constraint on its syntax. Mapping-set identifiers are exact and case-sensitive: consumers **MUST** compare them by Unicode code point, without Unicode normalization. Consumers **MUST NOT** depend on dereferencing a mapping-set identifier, or on any redirection from it, while loading the asset. Exporters **SHOULD** use an identifier that denotes one fixed version of the vocabulary. An identifier need not be a URI; for example, `vrm1_humanoid` identifies the VRM 1.0 Humanoid vocabulary listed below.

Each property name in a mapping-set object **MUST** be a nonempty role identifier. Role identifiers are exact and case-sensitive: consumers **MUST** compare them by Unicode code point, without Unicode normalization. An exporter **MUST** write only role identifiers that the mapping set's vocabulary defines. Validators are not required to check vocabulary membership: a role identifier that the vocabulary does not define does not make the mapping set or the asset invalid, and how a consumer handles it is host-defined. Each role identifier's value is a role association, which associates that role with one glTF node:

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `node` | integer | Yes | Index of the associated node in the top-level `nodes` array. |
| `name` | string | No | Optional name of this association. |

The association's `name` is independent of the referenced node's `name`. The `node` index is authoritative. Consumers **MUST NOT** resolve an association by `name`. Multiple associations, in the same mapping set or in different mapping sets, **MAY** reference the same node.

Associated nodes are not required to appear in a `skin.joints` array. This extension does not assert skin ownership, node hierarchy, role completeness, anatomical correctness, transform compatibility, or a one-to-one correspondence between roles and distinct nodes. Requirements defined by an external vocabulary remain outside this extension.

### Identifiers for known vocabularies (Informative)

Consumers compare mapping-set identifiers exactly, so two different identifiers for the same vocabulary do not match. The identifier below denotes one fixed version of a published vocabulary; tools that write or look for that version can use it to agree on one string. The list is not exhaustive, and identifiers not listed here are equally valid.

| Vocabulary | Mapping-set identifier |
|------------|------------------------|
| [VRM 1.0 Humanoid](https://github.com/vrm-c/vrm-specification/blob/70ae16e93abd6da727fdf641b67aa41010c6d933/specification/VRMC_vrm-1.0/humanoid.md) | `vrm1_humanoid` |

## Complete example (Informative)

The mapping-set identifier below is `vrm1_humanoid`, which denotes the [VRM 1.0 Humanoid vocabulary](https://github.com/vrm-c/vrm-specification/blob/70ae16e93abd6da727fdf641b67aa41010c6d933/specification/VRMC_vrm-1.0/humanoid.md) at the linked revision. The URL is a documentation reference, not the mapping-set identifier. The example contains no `skin`; the associations remain valid generic role-to-node metadata. The mapping is deliberately partial: it associates only six of the vocabulary's roles. The node tree is also simplified: `Head`, the feet and the hands are direct children of `Hips`, without the spine, arm and leg nodes that the vocabulary places between them. Any completeness and hierarchy requirements of that vocabulary remain outside this extension.

```json
{
  "asset": { "version": "2.0" },
  "scene": 0,
  "scenes": [
    { "nodes": [0] }
  ],
  "extensionsUsed": [
    "KHR_character",
    "KHR_character_skeleton_mapping"
  ],
  "nodes": [
    { "name": "CharacterRoot", "children": [1] },
    { "name": "Hips", "children": [2, 3, 4, 5, 6] },
    { "name": "Head" },
    { "name": "LeftFoot" },
    { "name": "RightFoot" },
    { "name": "LeftHand" },
    { "name": "RightHand" }
  ],
  "extensions": {
    "KHR_character": {},
    "KHR_character_skeleton_mapping": {
      "skeletalRigMappings": {
        "vrm1_humanoid": {
          "hips": { "node": 1, "name": "Hips" },
          "head": { "node": 2, "name": "head_for_tracking" },
          "leftFoot": { "node": 3, "name": "LeftFoot" },
          "rightFoot": { "node": 4, "name": "RightFoot" },
          "leftHand": { "node": 5, "name": "LeftHand" },
          "rightHand": { "node": 6, "name": "RightHand" }
        }
      }
    }
  }
}
```

## Validation boundary

The JSON Schemas enforce the presence of a nonempty `skeletalRigMappings` object, nonempty mapping sets, nonempty mapping-set and role identifiers, association shape, and non-negative `node` indices. A validator that claims to validate this extension **MUST** additionally resolve every `node` index against the top-level `nodes` array and verify the required extension declarations and top-level dependency objects. Such a validator **MUST NOT** report an error or warning solely because an association's `name` differs from the referenced node's `name`, including when that node has no `name`.

Validators are not required to dereference a mapping-set identifier, understand external role semantics, verify role completeness, identify skin joints, or run a retargeter. A validator that knows a mapping set's vocabulary **MUST NOT** report an error solely because that vocabulary does not define one of the mapping set's role identifiers; it **SHOULD** report a warning.

## Implementation notes (Informative)

- Mapping-set and role strings are identifiers, not display labels.
- A consumer can expose associations for inspection without implementing vocabulary-specific animation behavior.
- Vocabulary-specific adapters may interpret recognized roles, but their retargeting, transform, scale, and IK behavior is outside this extension.
- Renaming a node or an association does not change the reference, which uses the `node` index.

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader, retargeting helpers, and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and rig-switching demo.

## Schema

- [glTF.KHR_character_skeleton_mapping.schema.json](./schema/glTF.KHR_character_skeleton_mapping.schema.json)
- [jointAssociation.KHR_character_skeleton_mapping.schema.json](./schema/jointAssociation.KHR_character_skeleton_mapping.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
