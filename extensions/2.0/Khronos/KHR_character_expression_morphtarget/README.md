# KHR_character_expression_morphtarget

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

Assets using `KHR_character_expression_morphtarget` **MUST** list `KHR_character_expression_morphtarget`, `KHR_character_expression`, and `KHR_character` in `extensionsUsed`. They **MUST** contain the top-level `KHR_character` and `KHR_character_expression` extension objects. A `KHR_character_expression_morphtarget` object **MUST** be attached to an expression entry in that top-level `KHR_character_expression` object.

A listed channel **MAY** use `KHR_animation_pointer` as described below. Assets using this form **MUST** list `KHR_animation_pointer` in `extensionsUsed`; it is not an unconditional dependency of this extension.

An asset **MAY** list `KHR_character_expression_morphtarget` in `extensionsRequired`. A consumer claiming support for this extension **MUST** recognize its morph-weight channel classifications, **MUST** produce response records for every channel listed here that it supports under `KHR_character_expression` when the entry's effective driver is positive, and **MUST** implement the additive expression-composition rules below. Under `KHR_character_expression`, channels listed here produce no response records on a consumer that does not support this extension. Support for an optional `KHR_animation_pointer` target remains an independent capability and does not change the expression duration `T` or the sample time `t` defined by `KHR_character_expression`.

## Overview (Informative)

This extension classifies listed channels of an expression animation as node morph-weight channels. It adds validation metadata and defines additive morph-weight composition. It does not create a separate morph-target binding or change the samples of the channels it lists. Mixing the aggregate expression response with ordinary animation and other runtime systems is host-defined.

Channels listed here produce responses only on a consumer that supports this extension. A channel that no derived expression extension on the entry lists produces no response. Every channel in the selected animation still contributes to the base extension's duration.

## Extension structure (Normative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, nodes, meshes, animations, accessors, and buffers.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "smile",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_morphtarget": {
              "channels": [0, 1]
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

Each listed channel **MUST** target morph weights through one of these forms:

1. An ordinary core target with a defined `target.node` and `target.path` equal to `"weights"`.
2. A valid `KHR_animation_pointer` target resolving to the complete `/nodes/{node}/weights` property or to one element `/nodes/{node}/weights/{weight}` of that property.

For either form, the target node index (`target.node` or `{node}`) **MUST** identify a node with a defined, valid `mesh` reference, and that mesh **MUST** contain one or more morph targets. For the element form, `{weight}` **MUST** be a valid morph-target index. Core glTF requires all primitives of the mesh to contain the same number `N` of morph targets in the same order.

The authored base vector `B` of a node's morph weights is their complete authored initial value under `KHR_character_expression` (Authored initial state): the node's instantiated morph-weight vector, which core glTF defines as `node.weights` when defined, otherwise `mesh.weights` when defined, otherwise `N` zeros. The recommendation to match the time-zero sample therefore uses `B` for a complete-array target and `B[k]` for an element target with index `k`.

Listing a channel here makes it eligible to produce a response record but does not weight, remap, or otherwise modify that record. Support for `KHR_animation_pointer` remains independently necessary to emit a response for the pointer form. The rule against listing one channel in more than one derived expression extension on the same expression entry is defined by `KHR_character_expression` (Derived expression extensions).

## Expression composition (Normative)

All complete-array and individual-element targets for one node's morph weights are canonicalized to the same `N`-element concrete property with authored base vector `B` (defined above).

A complete-array response record `r` contributes `s_r[j] - n_r[j]` to every element `j`. An individual-element response record for index `k` contributes only `s_r - n_r` to element `k`. Different individual-element targets therefore have disjoint coverage. Complete-array and individual-element response records from distinct expression entries **MAY** coexist, and all contributions covering an element participate in that element's additive sum. Within one animation, a complete-array channel and an element channel for the same node overlap and are prohibited by `KHR_character_expression` (Channel targets).

When at least one current morph-weight response record exists for a node, the composition stage **MUST** emit one complete `N`-element aggregate weight vector. Each element with one or more contributors is evaluated by the additive formula using `B[j]` as its base. Each element with no contributor **MUST** equal `B[j]`. The resulting weights are subject to the no-clamp rule of `KHR_character_expression` (Additive aggregation).

### Morph-weight composition example (Informative)

For `B = [0.25, 0.5, 0.75]`, a complete-array contribution of `[0.25, 0, -0.25]` and an element-1 contribution of `0.25` produce `[0.5, 0.75, 0.5]`. An element-only response would still produce a complete vector, retaining `B` for every untouched element.

## Partial animation examples (Informative)

The following partial fragments omit the `asset` object, dependency declarations, the top-level `KHR_character` object, the top-level `KHR_character_expression` object and its expression entries, the `KHR_character_expression_morphtarget` objects whose `channels` arrays list these channels, each fragment's containing animation and its sampler, the targeted node and its mesh, accessors, buffers, and matching authored initial values. The two fragments belong to different animations, because one animation cannot target both the complete weight vector of node `0` and one of its elements.

The ordinary core form identifies the complete weight vector:

```json
{
  "sampler": 0,
  "target": {
    "node": 0,
    "path": "weights"
  }
}
```

The pointer form may identify one weight:

```json
{
  "sampler": 0,
  "target": {
    "path": "pointer",
    "extensions": {
      "KHR_animation_pointer": {
        "pointer": "/nodes/0/weights/2"
      }
    }
  }
}
```

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## Schema

- [expression.KHR_character_expression_morphtarget.schema.json](./schema/expression.KHR_character_expression_morphtarget.schema.json)

A `KHR_character_expression_morphtarget` object is placed in the `extensions` property of an expression entry, keyed by the extension name, rather than on a core glTF object. A validator matches the object to the schema above by that key.

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
