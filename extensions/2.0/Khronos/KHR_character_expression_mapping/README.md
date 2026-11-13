# KHR_character_expression_mapping

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / <https://github.com/fire>
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

Assets using `KHR_character_expression_mapping` **MUST** list `KHR_character_expression_mapping`, `KHR_character_expression`, and `KHR_character` in `extensionsUsed`. They **MUST** contain top-level objects for all three extensions.

## Overview (Informative)

`KHR_character_expression_mapping` maps inputs from an external expression vocabulary to the drivers of expression entries in `KHR_character_expression`. For example, an application can supply the VRM preset input `happy` and use the asset's mapping to drive its native smile expressions without knowing their names or indices.

An asset can provide mappings for several vocabularies. The host selects a mapping set and supplies values for its input keys. Each input contributes to one or more native expression drivers through authored weights. Mapping native drivers back to an external vocabulary is outside this extension's scope.

## Extension usage

The extension object **MUST** be attached to the top-level glTF object and **MUST** contain a nonempty `expressionSetMappings` object.

An asset **MAY** list `KHR_character_expression_mapping` in `extensionsRequired`. A consumer claiming support for this extension **MUST** make every mapping set available to host logic and, when the host selects a mapping set and supplies input values, compute the mapped drivers as defined below. A consumer that cannot provide this behavior **MUST** treat an asset listing this extension in `extensionsRequired` as unsupported according to the glTF 2.0 extension rules.

Support for this extension does not imply that the consumer understands a particular external vocabulary. Requiring this extension guarantees only the generic mapping contract defined here. It does not require support for any particular vocabulary identified by a mapping-set identifier.

## Mapping-set identifiers

Each property name directly inside `expressionSetMappings` is a mapping-set identifier, which names the mapping set's external expression vocabulary. Every mapping-set identifier **MUST** be nonempty; this extension places no other constraint on its syntax. Mapping-set identifiers are exact and case-sensitive: consumers **MUST** compare them by Unicode code point, without Unicode normalization. Consumers **MUST NOT** depend on dereferencing a mapping-set identifier, or on any redirection from it, while loading the asset. Exporters **SHOULD** use an identifier that denotes one fixed version of the vocabulary. An identifier need not be a URI; for example, `vrm1_expressions` identifies the VRM 1.0 preset expression vocabulary listed below.

Each property name directly inside a mapping-set object is an input key, which names an expression in the mapping set's external vocabulary. A mapping-set object **MUST** contain at least one input key, and every input key **MUST** be nonempty. Input keys are exact and case-sensitive: consumers **MUST** compare them by Unicode code point, without Unicode normalization. The meaning and completeness requirements of input keys are defined by the external vocabulary, not by this extension.

### Identifiers for known vocabularies (Informative)

Consumers compare mapping-set identifiers exactly, so two different identifiers for the same vocabulary do not match. The identifier below denotes one fixed version of a published vocabulary; tools that write or look for that version can use it to agree on one string. The list is not exhaustive, and identifiers not listed here are equally valid.

| Vocabulary | Mapping-set identifier |
|------------|------------------------|
| [VRM 1.0 preset expressions](https://github.com/vrm-c/vrm-specification/blob/70ae16e93abd6da727fdf641b67aa41010c6d933/specification/VRMC_vrm-1.0/expressions.md) | `vrm1_expressions` |

## Mapping entries

Each input key in a mapping set **MUST** contain a nonempty array of entries:

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `source` | integer | Yes | Index of the native expression to drive in `KHR_character_expression.expressions`. |
| `name` | string | No | Optional name of this mapping entry. |
| `weight` | number | Yes | Contribution coefficient in `[0, 1]`. |

Every `source` **MUST** be a valid index into the `expressions` array of the top-level `KHR_character_expression` extension object. Consumers **MUST** resolve entries by `source`, not by `name`. A mapping entry's `name` is independent of the referenced expression entry's `name`.

An input key **MAY** drive multiple native expressions, and multiple input keys **MAY** contribute to the same native expression. Repeated entries contribute independently.

## Input evaluation

Each supplied input value **MUST** be finite. Consumers **MUST** clamp each input value to `[0, 1]` before evaluating the mapping. A missing input value is `0`; input keys not present in the selected mapping set make no contribution. Every serialized `weight` **MUST** be in `[0, 1]`.

Let `c[q]` be the clamped input value for key `q`, and let `M[q]` be that key's entry array in the selected mapping set. For every native expression index `i`, the mapped driver is:

```text
clamp01(x) = min(1, max(0, x))

mappedNative[i] = clamp01(
    sum(
        entry.weight * c[q]
        for each q and each entry in M[q]
        where entry.source == i
    )
)
```

All contributions are summed before the final clamp. JSON object member order and entry-array order do not affect the mathematical result. A native expression with no matching entry receives a mapped driver of `0`.

## Host integration

The host chooses the mapping set, acquires its input values, and decides how to select or combine mapped drivers with other driver sources. This extension does not select a vocabulary or define that arbitration.

After arbitration, the host supplies the final native-driver vector to the expression system. When `KHR_character_expression_suppression` is supported and applied, that vector supplies its [pre-suppression input](../KHR_character_expression_suppression/README.md#immutable-input-and-output-vectors) `u`. Mapping does not apply suppression or evaluate expression animations. The [evaluation pipeline overview](../KHR_character_expression/README.md#evaluation-pipeline) in `KHR_character_expression` illustrates how the subsequent stages fit together.

## Complete example (Informative)

The mapping-set identifier `vrm1_expressions` denotes the VRM 1.0 preset expression vocabulary at the revision linked above. The URL is a documentation reference, not the mapping-set identifier. This example maps the VRM 1.0 inputs `happy`, `relaxed`, and `aa` to three native expression drivers. `happy` drives two expressions, and both `happy` and `relaxed` contribute to expression 0. The shared translation animation is a minimal illustrative response; a character would provide its own expression animations.

```json
{
  "asset": { "version": "2.0" },
  "scene": 0,
  "scenes": [
    { "nodes": [0] }
  ],
  "extensionsUsed": [
    "KHR_character",
    "KHR_character_expression",
    "KHR_character_expression_joint",
    "KHR_character_expression_mapping"
  ],
  "nodes": [
    { "name": "CharacterRoot" }
  ],
  "buffers": [
    {
      "byteLength": 32,
      "uri": "data:application/octet-stream;base64,AAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAA="
    }
  ],
  "bufferViews": [
    { "buffer": 0, "byteOffset": 0, "byteLength": 8 },
    { "buffer": 0, "byteOffset": 8, "byteLength": 24 }
  ],
  "accessors": [
    {
      "bufferView": 0,
      "componentType": 5126,
      "count": 2,
      "type": "SCALAR",
      "min": [0],
      "max": [1]
    },
    {
      "bufferView": 1,
      "componentType": 5126,
      "count": 2,
      "type": "VEC3"
    }
  ],
  "animations": [
    {
      "channels": [
        { "sampler": 0, "target": { "node": 0, "path": "translation" } }
      ],
      "samplers": [
        { "input": 0, "output": 1, "interpolation": "LINEAR" }
      ]
    }
  ],
  "extensions": {
    "KHR_character": {},
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "smileLeft",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_joint": { "channels": [0] }
          }
        },
        {
          "name": "smileRight",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_joint": { "channels": [0] }
          }
        },
        {
          "name": "jawOpen",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_joint": { "channels": [0] }
          }
        }
      ]
    },
    "KHR_character_expression_mapping": {
      "expressionSetMappings": {
        "vrm1_expressions": {
          "happy": [
            { "source": 0, "name": "happy_for_14", "weight": 0.8 },
            { "source": 1, "weight": 0.8 }
          ],
          "relaxed": [
            { "source": 0, "weight": 0.2 }
          ],
          "aa": [
            { "source": 2, "weight": 1.0 }
          ]
        }
      }
    }
  }
}
```

For inputs `happy=0.5`, `relaxed=0.25`, and `aa=0.25`, the mapped drivers are `smileLeft=0.45`, `smileRight=0.4`, and `jawOpen=0.25`.

## Validation boundary

The JSON Schema enforces the required `expressionSetMappings` property, nonempty mapping dictionaries, identifiers, input keys, and entry arrays, required non-negative expression indices, and weights in `[0, 1]`.

A validator that claims to validate this extension **MUST** additionally:

- verify placement on the top-level glTF object and the required extension declarations and dependency objects; and
- resolve every `source` against `KHR_character_expression.expressions`.

Such a validator **MUST NOT** report an error or warning solely because a mapping entry's `name` differs from the referenced expression entry's `name`. The referenced expression entry remains subject to the name validity requirements of `KHR_character_expression`.

Validation does not select a mapping set or require support for a particular vocabulary.

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - TypeScript schema types and conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, and sample assets.

## Schema

- [glTF.KHR_character_expression_mapping.schema.json](./schema/glTF.KHR_character_expression_mapping.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
