# KHR_character_expression_suppression

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

Assets using `KHR_character_expression_suppression` **MUST** list `KHR_character_expression_suppression`, `KHR_character_expression`, and `KHR_character` in `extensionsUsed`. They **MUST** contain the top-level `KHR_character` and `KHR_character_expression` extension objects. A `KHR_character_expression_suppression` object **MUST** be attached to an expression entry in the top-level `KHR_character_expression.expressions` array.

## Overview (Informative)

`KHR_character_expression_suppression` lets one native expression attenuate the driver of another expression, or its own, before response-animation evaluation. Standard `blend` and `block` suppressions are deterministic scalar factors evaluated from one immutable native-driver vector.

This extension does not acquire drivers, select an expression mapping set, define final animation mixing, or prescribe how sampled property values combine with other animation systems.

## Extension usage (Normative)

A suppression is a directed rule that multiplies a target expression driver by a factor in `[0, 1]` computed from the immutable input vector `u`; standard factors read only the containing source expression's value.

The extension object contains a nonempty `suppressions` array. Each suppression's implicit source is the array index of the expression entry containing that extension object. Its required `target` is an index into the same top-level expressions array. A native expression is an entry of that array, identified by its index.

An asset **MAY** list `KHR_character_expression_suppression` in `extensionsRequired`. A consumer claiming support for this extension **MUST** evaluate every standard suppression and apply the custom-type rules below. A consumer that cannot provide that behavior **MUST** treat the asset as unsupported when the extension is listed in `extensionsRequired`.

### Extension object

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `suppressions` | array | Yes | Nonempty array of suppression objects originating from the containing expression. |

### Suppression object

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `target` | integer | Yes | — | Index of the native expression attenuated by this suppression. |
| `name` | string | No | — | Optional name of this suppression. |
| `type` | string | No | `"blend"` | Standard `"blend"` or `"block"`, or a vendor-extension-name-shaped custom token. |
| `amount` | number | No | `1` | Attenuation amount in `[0, 1]`. |
| `threshold` | number | No | `0` | Strict activation threshold in `[0, 1]` for `"block"`. |

The `target` index identifies the suppressed expression. Consumers **MUST** resolve the suppressed expression by `target`, not by `name`. A suppression's `name` names the suppression itself and is independent of the target expression entry's `name`; no correspondence between those names is implied or required.

## Immutable input and output vectors

The host supplies one final native-driver vector after any host-defined driver acquisition, mapping-set selection, and arbitration between direct-native and mapped input surfaces. Each supplied native-driver value **MUST** be finite. Consumers **MUST** clamp each supplied native-driver value to `[0, 1]` before evaluating suppressions. Let `u[i]` be the resulting value for native expression index `i`. A native expression for which the host supplies no driver has `u[i] = 0`. This `u` vector is the immutable pre-suppression input snapshot for one suppression evaluation.

The suppression operation produces an effective driver vector `v` of the same length. Every suppression factor **MUST** use the pre-suppression value in `u` for its source expression. A suppression factor **MUST NOT** read any suppression output or any value from `v` as an input. The output vector `v` is produced only after all factors have been evaluated from `u`.

For a suppression `s`, define:

```text
source(s) = index of the expression entry containing s
target(s) = s.target
amount(s) = s.amount when present; otherwise 1
threshold(s) = s.threshold when present; otherwise 0
```

## Standard suppression factors

For a standard `"blend"` suppression:

```text
factor(s, u) = 1 - amount(s) * u[source(s)]
```

For a standard `"block"` suppression:

```text
factor(s, u) =
    1 - amount(s)    when u[source(s)] > threshold(s)
    1                otherwise
```

The `"block"` factor is threshold-triggered attenuation, not necessarily a complete block: when `u[source(s)] > threshold(s)`, it multiplies the target by `1 - amount(s)` and fully blocks it only when `amount(s) == 1`; otherwise, its factor is `1`. The comparison is strict. The standard `"blend"` factor does not read `threshold`: an exporter **MUST NOT** write `threshold` on a suppression whose `type` is `"blend"` or omitted, and an importer **MUST** ignore `threshold` on such a suppression.

Given all suppressions in all expression entries, each effective target driver is:

```text
v[t] = u[t] * product(
    factor(s, u)
    for every suppression s where target(s) == t
)
```

An empty product is `1`, so a driver with no incoming suppressions is unchanged. Because `u`, `amount`, `threshold`, and every supported factor are in `[0, 1]`, every value in `v` is also in `[0, 1]`.

Multiple suppressions **MAY** target the same expression, including multiple suppressions from the same source expression. All such suppressions participate in the product, so their combined influence is multiplicative; duplicate suppressions contribute duplicate factors. A self-suppression reads the source's unsuppressed value from `u` and contributes normally. Mutual and longer cycles are valid because every suppression factor reads the same immutable `u`. The resulting `v` is mathematically independent of JSON object order, expression-array traversal order, and suppression-array order.

An expression's outgoing suppressions always read its value from `u`, even when incoming suppressions give that expression a different value in `v`.

## Custom suppression types

A custom suppression type is any value other than `"blend"` or `"block"`. It **MUST** be shaped like a glTF vendor extension name: an uppercase alphanumeric prefix, an underscore, and a lowercase snake-case suffix, for example `"ACME_expression_suppression_curve"`.

A custom token **MAY** appear without a companion extension object. Unless a supported companion defines its factor, as described below, a custom suppression **MUST** be ignored at runtime by this extension and **MUST** contribute a factor of `1`, so it does not alter the target driver. This covers a bare custom token and a custom token whose optional companion is not supported:

```text
factor(s, u) = 1
```

Portable custom behavior requires an extension object on the same suppression object whose key exactly equals the suppression's `type` value. That companion specification **MUST** define exactly one finite factor in `[0, 1]` for the suppression. The factor **MUST** be a deterministic function only of the immutable vector `u`, the suppression's source index `source(s)`, and authored data on the suppression and companion objects, including authored asset data that those objects reference, such as accessors and their buffer data. It **MUST NOT** read any value from `v`, prior evaluations, time, or any host input other than `u`; mutate a driver; directly replace `v[target(s)]`; or otherwise bypass the product equation. A supported companion's factor participates in the same product as every standard factor.

`amount` and `threshold` carry no standard meaning on a custom suppression type. A companion specification **MAY** define how its factor reads either value, subject to the requirements above. A supported companion's factor reads an omitted `amount` as `1` and an omitted `threshold` as `0`, as `amount(s)` and `threshold(s)` define for every suppression, so omitting either value gives the same factor as authoring its default explicitly. A consumer that ignores a custom type at runtime ignores both values with it.

The companion extension **MUST** be listed in `extensionsUsed`. If correct presentation depends on its custom factor, both `KHR_character_expression_suppression` and the companion extension **MUST** be listed in `extensionsRequired`. For an unsupported optional companion, the factor-`1` rule above applies; an unsupported required companion makes the asset unsupported under the ordinary glTF extension rules.

An unrelated or differently named extension object on the same suppression does not define the custom type. It follows its own specification, while the unmatched custom token falls under the factor-`1` rule above.

An implementation claiming read-write or round-trip support for this extension that retains a custom suppression when re-emitting an asset **MUST** preserve its unrecognized custom `type` token. It **MUST** also preserve any `amount` and `threshold` values authored on that suppression that it does not interpret. Preserving an unrecognized token or an ignored value does not make it affect runtime evaluation. If it retains a same-object companion extension that it does not interpret, it **MUST** preserve that extension's payload. These preservation requirements do not apply to data that the user deliberately edits or removes. Deliberately removing a custom suppression or companion extension is an authoring operation and requires the ordinary corresponding updates to `extensionsUsed` and `extensionsRequired`. Render-only consumers are not required to provide serialization.

A companion payload can contain indices, such as expression indices or the indices of accessors whose data its factor reads. A tool that removes, inserts, or reorders elements of the indexed arrays without interpreting the payload cannot update those indices, so a preserved payload can go stale and refer to the wrong or missing data; such a tool can warn the user or remove the companion as an authoring operation.

## Response-evaluator boundary

The effective vector `v` is the suppression operation's output. A character-expression response evaluator consumes `v[i]` as the current driver for expression entry `i`.

`KHR_character_expression` defines the aggregation of every additive property that has at least one contributing response record. Composition of other property types, and the final mix of expression results with ordinary animation and other non-expression property writers, remain host-defined.

The [evaluation pipeline overview](../KHR_character_expression/README.md#evaluation-pipeline) in `KHR_character_expression` illustrates how these stages fit together.

## Standard-suppression example (Informative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, the referenced animations, and the derived expression extensions that list each entry's channels. In this fragment, `aa` is expression index 0, `happy` is index 1, and `angry` is index 2.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "aa",
          "animation": 0
        },
        {
          "name": "happy",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_suppression": {
              "suppressions": [
                {
                  "target": 0,
                  "name": "attenuate_viseme_for_happy",
                  "type": "blend",
                  "amount": 0.5
                }
              ]
            }
          }
        },
        {
          "name": "angry",
          "animation": 2,
          "extensions": {
            "KHR_character_expression_suppression": {
              "suppressions": [
                {
                  "target": 0,
                  "type": "block",
                  "amount": 1.0,
                  "threshold": 0.2
                }
              ]
            }
          }
        }
      ]
    }
  }
}
```

For `u = [1, 0.5, 0.1]`, the blend factor is `0.75` and the block factor is `1` because the strict threshold comparison `0.1 > 0.2` is false. Therefore `v[0] = 0.75`. If `u[2]` is instead `0.6`, the comparison `0.6 > 0.2` is true, so the block factor becomes `0` and `v[0]` becomes `0`.

For a driver intended to equal a threshold, or within rounding error of it, implementations that hold the driver and the threshold at different precisions can resolve the strict comparison differently, so authors should avoid thresholds that coincide with expected positive driver values.

## Custom-type examples (Informative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, the top-level `KHR_character_expression` object and its expression entries, the source expression entry's `KHR_character_expression_suppression` object whose `suppressions` array contains the suppression, and the referenced animations. This bare custom suppression is valid and is ignored at runtime by this extension, so it does not alter the target driver:

```json
{
  "target": 0,
  "type": "ACME_expression_suppression_curve"
}
```

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, the top-level `KHR_character_expression` object and its expression entries, the source expression entry's `KHR_character_expression_suppression` object whose `suppressions` array contains the suppression, and the referenced animations. It demonstrates where a portable companion extension would be attached. The payload is illustrative; only the `ACME_expression_suppression_curve` specification can define its factor.

```json
{
  "target": 0,
  "type": "ACME_expression_suppression_curve",
  "extensions": {
    "ACME_expression_suppression_curve": {
      "controlPoints": [0.0, 0.25, 1.0]
    }
  }
}
```

An asset containing the companion object must list `ACME_expression_suppression_curve` in `extensionsUsed`. When the custom factor is essential, it must list both `KHR_character_expression_suppression` and `ACME_expression_suppression_curve` in `extensionsRequired`.

## Validation boundary

The JSON Schemas enforce nonempty suppression arrays, required non-negative target indices, string-valued optional names, standard or vendor-extension-name-shaped type strings, and `amount` and `threshold` values in `[0, 1]`. A validator **MUST NOT** report an error solely because `threshold` is present on a suppression whose `type` is `"blend"` or omitted; it **SHOULD** report a warning.

A validator that claims to validate this extension **MUST** additionally:

- verify placement on an expression entry;
- resolve every `target` against the top-level expressions array;
- verify the required extension declarations and top-level dependency objects;
- for a custom type with a same-named companion object, verify ordinary `extensionsUsed` and `extensionsRequired` declarations; and
- apply any additional structural rules defined by a supported companion specification.

A validator **MUST NOT** report an error or warning solely because a suppression's `name` differs from the target expression entry's `name`, including when that expression entry has no `name`.

Bare custom types are valid; at runtime, a suppression with a bare custom type contributes a factor of `1`, as stated in [Custom suppression types](#custom-suppression-types). A validator does not execute suppression equations or validate host-defined final animation composition.

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## Schema

- [expression.KHR_character_expression_suppression.schema.json](./schema/expression.KHR_character_expression_suppression.schema.json)
- [suppression.KHR_character_expression_suppression.schema.json](./schema/suppression.KHR_character_expression_suppression.schema.json)

A `KHR_character_expression_suppression` object is placed in the `extensions` property of an expression entry, keyed by the extension name, rather than on a core glTF object. A validator matches the object to the first schema above by that key; that schema refers to the second for each suppression object.

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
