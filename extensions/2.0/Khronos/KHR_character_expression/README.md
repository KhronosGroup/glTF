# KHR_character_expression

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

Written against the glTF 2.0 specification and the [glTF 2.0 Asset Object Model](../../../../specification/2.0/ObjectModel.adoc) (AOM), which this extension uses for the identity, type, and mutability of animated properties.

Requires the extension: `KHR_character`.

Assets using `KHR_character_expression` **MUST** list `KHR_character_expression` and `KHR_character` in `extensionsUsed`. They **MUST** contain top-level `KHR_character_expression` and `KHR_character` extension objects.

Other extensions may define animation-channel targets or add metadata to expression entries. Their normal `extensionsUsed` and `extensionsRequired` rules continue to apply.

An asset **MAY** list `KHR_character_expression` in `extensionsRequired`. A consumer claiming support for this extension **MUST** implement the driver model, timeline, sampling, and response-record rules of the expression response evaluator for every channel it supports, and the additive aggregation rules for the response records it produces. Other property types compose as defined in Other composition. A derived expression extension assigns an expression type to the channels it lists on its expression entry, and which channels a consumer supports is defined in Implementation support and partial evaluation. The presence of a derived expression extension in `extensionsUsed` declares that the asset contains channels of that expression type. The rules below define what a consumer produces when it supports only some of an animation's channels.

## Overview (Informative)

This extension associates labeled character-expression drivers with glTF animations and defines a normalized, stateless way to sample those animations as expression responses. A driver may come from user data, another animation system, a procedural system, or any other host source.

A per-entry expression response is a set of absolute property samples. Multiple entries can target the same property. Additive responses form aggregate expression responses under the rules below; other property types use host-defined composition. This extension does not apply the responses to the asset state or define their final mix with ordinary animation or other runtime systems.

### Evaluation pipeline

The following pipeline summarizes how the evaluator and related extensions fit together:

```text
host driver sources
    -> optional external-input-to-native-driver mapping
    -> host direct/mapped arbitration
    -> immutable pre-suppression vector u
    -> optional KHR_character_expression_suppression evaluation
    -> effective driver snapshot v
    -> per-entry stateless sampling
    -> per-entry absolute response records
    -> additive aggregation
    -> additive aggregates and other per-entry responses
    -> host composition and application
```

In order: an optional [`KHR_character_expression_mapping`](../KHR_character_expression_mapping/README.md#input-evaluation) step converts external expression inputs to native drivers (the drivers of the asset's own expression entries), then host arbitration picks the final driver vector, which, with each value clamped to `[0, 1]`, is `u`. When the consumer supports and applies [`KHR_character_expression_suppression`](../KHR_character_expression_suppression/README.md#immutable-input-and-output-vectors), it turns `u` into the effective vector `v`.

The [response evaluator](#expression-response-evaluator-normative) then evaluates each entry with its driver from `v`, or from the final driver vector when suppression is not applied. [Simultaneous expression composition](#simultaneous-expression-composition-normative) aggregates additive responses. The host combines other property types and applies the results under the host integration boundary.

Driver sourcing, arbitration, evaluation cadence, and the final mix with non-expression writers remain host-defined, among the items defined in the [host integration boundary](#host-integration-boundary-normative).

### Relation to the hint extensions (Informative)

The response evaluator reads only the asset and the driver snapshot. It takes no render view, view tag, camera, or look-at target as input, so its response records are the same for every view.

[`KHR_node_visibility_hint`](../KHR_node_visibility_hint/README.md) and [`KHR_mesh_primitive_visibility_hint`](../KHR_mesh_primitive_visibility_hint/README.md) evaluate per-view predicates that do not alter glTF-authored or animated data, so they change neither the authored initial values nor the response records of an expression. [`KHR_node_camera_hint`](../KHR_node_camera_hint/README.md) and [`KHR_node_lookat_target`](../KHR_node_lookat_target/README.md) read node transforms in an evaluation state that the host selects, so whether that state includes expression results the host has applied is host-defined.

## Background (Informative)

A typical expression is a reusable animation unit that conveys emotion or intent through movement or other visual change, often centered on the face. Familiar vocabularies for describing such units include viseme sets (mouth shapes that correspond to speech sounds), the Facial Action Coding System (a taxonomy of visually distinguishable facial movements), and the [VRM 1.0 preset expressions](https://github.com/vrm-c/vrm-specification/blob/70ae16e93abd6da727fdf641b67aa41010c6d933/specification/VRMC_vrm-1.0/expressions.md) (a preset vocabulary covering emotions, visemes, blinks, gaze directions, and `neutral`).

Each expression entry pairs a descriptive `name` with a standard glTF animation. The entry's driver, taken from the effective vector `v` when the consumer supports and applies suppression, is clamped to `[0, 1]` to give `e`. An `e` of `0` produces no response, and a positive `e` selects the sample time `t = e * T` within that animation, where `T` is the [expression duration](#channel-set-and-duration), so an `e` of `1` samples the end of the animation. Like a morph-target weight, the driver runs from no contribution to full effect, but it selects a time rather than scaling the sampled values. The normative sections below define which channels a consumer animates and how simultaneous expressions combine.

## Extension structure (Normative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, the referenced animations, and the derived expression extensions that list each entry's channels. It shows only properties that this extension defines; as shown, neither entry produces a response record.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "smile",
          "animation": 0
        },
        {
          "name": "blinkLeft",
          "animation": 1
        }
      ]
    }
  }
}
```

### Properties

#### Extension object

| Property | Type | Required | Description |
| --- | --- | --- | --- |
| `expressions` | array | Yes | Nonempty array of expression entries. Array indices are the authoritative identities of the entries. |

#### Expression entry

| Property | Type | Required | Description |
| --- | --- | --- | --- |
| `name` | string | Yes | Nonempty descriptive name, unique among the entries in the containing `expressions` array. |
| `animation` | integer | Yes | Index of the glTF animation used as this entry's response curve. |

Each expression entry is a glTF property and may therefore contain `extensions` and `extras`.

Every expression entry **MUST** have a nonempty string `name`. Its `name` **MUST** be unique among all entries in the top-level `KHR_character_expression.expressions` array. Consumers **MUST** compare names by Unicode code point, without case folding, trimming, or Unicode normalization. Applications **MAY** provide name-based lookup of native expression entries using this comparison. Array indices remain the authoritative identities of expression entries. Extensions that refer to expression entries use their array indices unless they explicitly define another mechanism.

The `animation` value **MUST** be a valid index into the top-level `animations` array.

When two or more entries reference the same animation, each reference defines an independent evaluator input and result. Referencing an animation here neither changes nor prevents ordinary playback of that animation with its authored timeline.

## Expression response evaluator (Normative)

For an expression entry, let `A` be the animation selected by `animation` and let `x` be the current optional driver supplied to the evaluator. A supplied `x` **MUST** be finite. If no driver is supplied, `x` is `0`. The effective driver is:

```text
e = clamp(x, 0, 1)
```

If `e` is `0`, the evaluator **MUST** return an empty response and **MUST NOT** emit the authored initial values described below. Otherwise, it evaluates `A` as specified in the following sections.

Evaluation is stateless. A new evaluation replaces the previous response contribution from the same expression entry; it does not modify the glTF asset, the authored initial state, or the state used by a later evaluation. For the same asset and current driver value, the evaluator **MUST** return the same response regardless of prior driver values, evaluation cadence, or update history.

### Channel set and duration

Let:

- `C` be every channel in `A.channels`;
- `S` be the set of sampler indices referenced by the channels in `C`; and
- `last(s)` be the final decoded input key time of sampler `s`.

Every sampler in `S` **MUST** contain at least two input keys. Samplers in `A.samplers` that are not referenced by a channel in `C` are not members of `S` and do not affect expression evaluation.

Each referenced input accessor **MUST** satisfy the core animation-input requirements: `SCALAR` type, `FLOAT` component type, defined `min` and `max`, finite nonnegative values, and strictly increasing decoded key times. Its `count` **MUST** be at least two. These requirements **MUST** be checked against decoded resource data; accessor metadata alone cannot establish strict ordering or finiteness. Throughout this extension, decoded data are the accessor values obtained after any bufferView decompression and filtering, such as those defined by `KHR_meshopt_compression`, and after applying any sparse values.

The expression duration is:

```text
T = max(last(s)) for all s in S
```

Core glTF requires animation input keys to be nonnegative and strictly increasing. Consequently, the requirements above make `T` finite and greater than zero.

Every channel in `C`, including a channel that a particular implementation does not support, contributes its referenced sampler to `S` and therefore to `T`.

### Channel targets

Every channel in `C` **MUST** resolve to a concrete mutable Asset Object Model property under core glTF or the normative semantics of its declared target extensions (see Implementation support and partial evaluation). Merely placing an extension object on a target does not satisfy this requirement unless that extension defines the concrete mutable property target. A core channel with no `target.node` and no valid extension-defined property target is invalid for use by this extension.

All target, sampler, accessor, interpolation, value, and extension-specific validity requirements from core glTF and the channel's target extensions continue to apply. In particular, core glTF forbids `matrix` on a node referenced by an `animation.channel.target` ([Transformations](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#transformations)), and the AOM leaves the `rotation` and `scale` pointers of a node that defines `matrix` undefined, so such a pointer does not resolve to a concrete mutable AOM property. In addition, a channel in `C` **MUST NOT** target the `translation` of a node that defines `matrix`, including through a `KHR_animation_pointer` target, even though the AOM defines the `translation` pointer of such a node.

Within `A`, a concrete property **MUST NOT** be targeted by more than one channel. A channel targeting an entire array overlaps a channel targeting any element of that array. This prohibition applies across core and extension-defined target forms as well as within either form.

### Sampling

For `e > 0`, the expression sample time is:

```text
t = e * T
```

Each channel in `C` that the implementation supports (see Implementation support and partial evaluation below) is sampled at `t` using the conversion, interpolation, and endpoint-clamping rules defined by core glTF and by its target extensions. Expression evaluation does not wrap, repeat, reverse, rescale, or otherwise alter the animation timeline.

A property whose specification defines its value as a rotation angle in radians is sampled as an ordinary floating-point scalar, and its keys are interpolated numerically under its sampler's interpolation mode. The evaluator **MUST NOT** substitute shortest-arc interpolation, and **MUST NOT** wrap, unwrap, or otherwise reduce the sampled angle before placing it in a response record. A key sequence therefore describes the full signed rotation path, including a turn of more than half a revolution and a turn of more than one revolution.

Each supported channel produces a response record containing its concrete target property and the ordinary absolute glTF value sampled for that property. Samples are not deltas from the authored state, blend weights, or values to be scaled again by `e`.

Core glTF defines node `rotation` values, including animated values, as unit quaternions ([Transformations](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#transformations), [Spherical Linear Interpolation](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#interpolation-slerp)) and requires a quaternion produced by `CUBICSPLINE` interpolation to be normalized before it is applied to a node's rotation ([Cubic Spline Interpolation](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#interpolation-cubic)). A response record for node `rotation` therefore holds the sampled quaternion normalized to unit length, in every interpolation mode.

Each referenced output accessor **MUST** have the component type, accessor type, dimensions, and count required by its concrete target. It supplies one complete logical target value per input key for `LINEAR` and `STEP`, and the corresponding in-tangent, value, and out-tangent records for `CUBICSPLINE`. Scalar-packed array targets, including morph weights, **MUST** include every array element for every key and three times that number of elements for `CUBICSPLINE`. Target-specific extensions may further constrain these rules.

### Implementation support and partial evaluation

An implementation supports a channel when a derived expression extension on the expression entry lists the channel, the implementation supports that derived expression extension, and the implementation supports every extension that defines the channel's target form, its concrete target property, or an object on the path from the glTF root to that property; these extensions are the channel's target extensions. For this purpose, an implementation supports a target extension when it can resolve the channel's target through that extension to its concrete mutable Asset Object Model property, whether or not the implementation renders the effect of that property. An implementation that supports a derived expression extension **MUST NOT** treat a channel listed by that extension as unsupported for any reason other than lack of support for one of the channel's target extensions. The implementation **MUST** produce response records for every channel it supports when the entry's effective driver is positive, and **MUST NOT** produce a response record for any other channel, including a channel that no derived expression extension lists and a channel listed only by a derived expression extension the implementation does not support. Such a channel is not animated by the expression, but it still contributes to `T`. An unsupported channel does not by itself make the expression entry or the asset unsupported, and supporting `KHR_character_expression` does not make any target extension required; ordinary `extensionsRequired` handling still applies. A derived expression extension does not change how a supported channel is sampled; derived expression extensions are specified below.

## Authored initial state (Normative)

Authors **SHOULD** author each channel in `C` so that its sample at `t = 0` represents the target property's authored initial state. This helps expressions transition smoothly between the inactive state and a positive driver. A different time-zero sample does not invalidate the expression.

The authored initial value is resolved from the asset before ordinary animation, expression responses, or other runtime systems are applied:

1. Use the property's explicitly authored JSON value when present.
2. Otherwise, use the property's specification-defined default value.
3. For array-valued properties, a complete-array target uses the property's complete authored initial value and an element target uses its corresponding element. For node morph weights, that complete value is the node's instantiated morph-weight vector as defined by core glTF, whether or not a derived expression extension lists the channel.

For clarity, the core node TRS defaults are translation `[0, 0, 0]`, rotation `[0, 0, 0, 1]`, and scale `[1, 1, 1]`. Extension properties use the authored values and defaults defined by their own specifications. A target with neither an authored value nor a specification-defined default is invalid unless its target specification defines another initial-value rule.

The time-zero sample is the value obtained by sampling the channel at `t = 0` under ordinary glTF rules, including endpoint clamping when its first key time is greater than zero. For `CUBICSPLINE`, this is the sampled value, not an in-tangent or out-tangent.

### Validation boundary

A validator claiming support for this extension **MUST** verify that every expression entry has a nonempty string `name` and that all names in the containing `expressions` array are distinct under the comparison defined in Extension structure. Name uniqueness is a semantic constraint beyond the JSON Schema and can be checked from the JSON metadata independently of animation-resource decoding or support for derived expression extensions.

Validation of decoded input times and target values requires decoding the referenced resources, including any bufferView compression or filtering and sparse values. A validator that cannot decode those resources **MUST NOT** report the affected checks as complete.

Likewise, a validator lacking the schema or semantic support for a declared target extension of a channel, whether that extension is optional or required, cannot completely validate the affected target, output conversion, or initial value. It **MUST NOT** reject the asset solely because that target extension is unsupported, and it **MUST NOT** report the affected expression checks as complete.

## Simultaneous expression composition (Normative)

Multiple expression entries **MAY** target the same concrete mutable property, including node translation, rotation, and scale, morph weights, texture-transform properties, and Boolean or integer properties defined by target extensions. This includes complete-array and individual-element targets from different entries. Core animation targets and equivalent `KHR_animation_pointer` targets identify the same property.

A validator **MUST NOT** reject an asset solely because response records from different expression entries can overlap. This applies whether the entries reference the same animation, their drivers are mutually exclusive, suppression prevents simultaneous activation, or a listed channel holds its target at a constant value. The within-animation target-uniqueness rule in Channel targets and the ordinary validity requirements of core glTF and the target extensions continue to apply.

The expression response evaluator produces ordinary absolute samples independently for each evaluated entry. The composition stage **MUST** produce the aggregate response defined by Additive aggregation for every additive concrete property with at least one contributing current response record, whether or not records from distinct entries overlap on it. Other property types use Other composition. The extension does not apply these results to the asset state or prescribe their final mix with ordinary animations and other non-expression writers.

### Additive aggregation

One simultaneous-expression composition step **MUST** use one immutable snapshot containing a current driver for every expression entry; an absent driver is `0`. When the consumer supports and applies `KHR_character_expression_suppression`, its final effective vector `v` is that snapshot; otherwise, the snapshot is the host's final native-driver vector. Every entry is evaluated independently, including entries that reference the same animation. During additive aggregation, consumers **MUST NOT** select only one active entry, apply last-writer-wins behavior, or otherwise discard a contributing response record.

Every floating-point component of a concrete property aggregates under the formula below unless its specification defines quaternion semantics or it is core node `scale` or `KHR_texture_transform` `scale`. Boolean and integer properties are not additive. These other property types use Other composition; they are not prohibited from overlapping across expression entries. Other per-type rules, such as canonical forms, are defined by the concrete property's specification or derived expression extension; such rules cannot replace the authored initial value used as `b` below.

For one additive component of concrete property `p`, let:

- `b` be that component of `p`'s authored initial value;
- `s_r` be the current ordinary absolute sample in contributing response record `r`; and
- `n_r` be the value obtained by sampling the same channel and component at `t = 0`.

When one or more current response records contribute to that component, its aggregate value is:

```text
y = b + sum(s_r - n_r) for every contributing record r
```

The formula **MUST** be applied even when there is only one contributing record. The neutral value `n_r` comes from the same decoded channel as `s_r`; implementations **MUST NOT** substitute `b` for `n_r`. Each additive record therefore contributes its change from its own time-zero sample.

The composition stage produces one aggregate response record for each additive concrete property with at least one contributing current record. It produces no aggregate record for a property with no current contributors. Unless the concrete property's specification explicitly requires otherwise, the result **MUST NOT** be clamped, saturated, normalized, or otherwise bounded.

#### Accumulation example (Informative)

With `b = 0.5`, two records whose current samples are `0.75` and `0.625` and whose neutral samples are both `0.5` produce `0.5 + (0.75 - 0.5) + (0.625 - 0.5) = 0.875`.

### Other composition

Quaternion properties, core node `scale`, `KHR_texture_transform` `scale`, Boolean properties, and integer properties are not subject to the required component-wise additive formula above. Their response records remain ordinary absolute samples, and multiple records covering the same property are permitted.

The host selects a type-appropriate composition method for these records, including any weighting, reference-pose handling, ordering, override behavior, or treatment of inactive responses. For scale, that choice can include additive changes, multiplication, blending, or overrides. This extension does not prescribe an aggregate response for these property types. Values applied to a target **MUST** satisfy that property's validity requirements, including unit-quaternion requirements for node rotation. These host choices do not replace or change the required additive aggregation for additive properties.

## Derived expression extensions (Normative)

Derived expression extensions assign an expression type to the channels they list, add validity requirements for those channels, and define how they are handled. Listing determines which channels can produce response records: only a channel that the implementation supports, as defined in Implementation support and partial evaluation, produces response records, and every other channel produces no response record and is not animated by the expression. Every channel, listed or not, contributes to `T`. An asset declares the types it uses by listing these extensions in `extensionsUsed`, so a consumer can discover which types are present to be powered.

A validator recognizes an extension object on an expression entry when it knows from that extension's specification whether the extension lists channels. This recognition is necessary to validate channel-list ownership; it is not a cross-expression target-conflict check.

- A listed channel index **MUST** resolve against `channels` of the animation selected by the containing entry's `animation` property.
- The same channel index **MUST NOT** occur in more than one derived expression extension on the same expression entry. If that entry carries a derived expression extension the validator does not support, or its own `extensions` property contains an object the validator does not recognize, validation of this rule for that entry is incomplete; the validator **MUST NOT** report it as successfully completed or reject the asset solely because of that missing support.
- A channel not listed by any derived expression extension is still a member of `C` and contributes to `T`, but produces no response record.
- Support for a derived expression extension means recognizing the channels it lists and implementing its type-specific handling in addition to base sampling; each extension's Dependencies states its exact conformance claim. Ordinary `extensionsRequired` handling still applies.

## Host integration boundary (Normative)

This extension does not define an Asset Object Model property that stores `x`, nor does it define how a host obtains or updates a driver. Driver sources may include user input, tracked data, procedural logic, another animation system, or other host data.

The following remain host-defined:

- evaluation cadence and scheduling;
- driver smoothing, prediction, and arbitration;
- wiring from other animations or systems into expression drivers;
- prevention or handling of host-created feedback cycles; and
- composition of the property types listed in Other composition with each other, and the final mix of additive aggregates and other expression responses with ordinary animations and other non-expression property writers.

No behavior in this section changes the normative sampling, response-record, or additive aggregation rules above.

> Implementation Note: When an expression's driver returns to zero, its response disappears; the evaluator does not restore the target properties. The host determines their values when no expression response contributes to them.

## Authoring guidance (Informative)

Authors can use `STEP` interpolation when an expression is intended to behave as a discrete or binary control, such as an eye blink. This extension defines no binary-expression flag and no discrete-control semantics. `STEP` is an authoring choice rather than a recommendation of this extension. Other interpolation modes remain available wherever the concrete target's own rules permit them.

Under `STEP`, a key's value applies once the sample time `t = e * T` reaches that key's time. Because `t` reaches `T` only when `e` is `1`, a two-key `STEP` channel with keys at `0` and `T` changes to its final value only at full effective driver, which a suppression factor or a mapping weight below `1` can make unreachable. Placing the switching key at the driver level where the change is intended, and keeping a final key at `T` that holds the switched value, avoids this: for example, a channel with keys at `0`, `0.5 * T`, and `T`, the last two holding the same value, changes at `e = 0.5`. `T` is the latest final key time over the samplers that the animation's channels reference (see Channel set and duration), so moving a final key at `T` earlier also shortens `T` unless another channel also ends at `T`, and a longer channel elsewhere in the same animation changes the driver level at which a given key time is reached.

Runtimes using different precision for sample times and key times can resolve a driver at a switching boundary differently; authors should avoid relying on a driver landing exactly on that boundary.

An exporter can list only the channels that actually change their targets. A listed channel that holds its target at the authored initial value still produces a response record holding that value whenever the entry's effective driver is positive. Listing such channels on several entries is valid. A constant additive channel contributes zero change from its time-zero sample; the effect of other property types depends on the host's composition method. An unlisted channel still contributes to `T`.

For a property using Other composition, a time-zero sample that differs from the authored initial value can cause a jump when the host applies its response (see [Authored initial state](#authored-initial-state-normative)).

Implementers should document their composition choices, including how they handle overlapping targets, reference and neutral poses, weighting, ordering, discrete values, quaternion signs and normalization, and transitions between inactive and active responses. The driver already selects the sample time; additive aggregation does not apply it again as a blend weight. Composition of the other property types remains a separate host choice. Different runtimes can produce different results for those property types or for the final mix with non-expression writers.

### Viewers without expression support

A viewer that does not support this extension ignores it in an asset that does not require it, and treats each referenced animation as an ordinary animation with its authored timeline. Its time-zero sample may differ from the authored initial state, so applying that sample can change the target immediately. Core glTF lets such a viewer select an animation and play it automatically or pause it on its first frame. An application that supports this extension can avoid auto-playing the referenced animations or presenting them as ordinary clips.

Core glTF requires every extension needed to load or render an asset to be listed in `extensionsRequired`, so an asset whose correct presentation depends on expression evaluation lists `KHR_character_expression` there, together with each derived expression extension that lists a channel that presentation needs and each target extension of such a channel. A viewer without support then rejects the asset instead of showing its expression animations as clips. Requiring this extension alone does not ensure that any channel is animated, because a consumer animates only the channels it supports (see Implementation support and partial evaluation). When the authored state is an acceptable presentation by itself, leaving these extensions out of `extensionsRequired` lets such a viewer still load the asset.

## Extension example with metadata (Informative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, and the referenced animations. It uses placeholder names for three derived expression extensions.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "smile",
          "animation": 0,
          "extensions": {
            "ACME_expression_type_a": {
              "channels": [0, 1]
            },
            "ACME_expression_type_b": {
              "channels": [2]
            },
            "ACME_expression_type_c": {
              "channels": [3, 4]
            }
          }
        }
      ]
    }
  }
}
```

Any additional channel in animation `0` that none of these derived expression extensions lists contributes to `T` but produces no response record.

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## Schema

- [glTF.KHR_character_expression.schema.json](./schema/glTF.KHR_character_expression.schema.json)
- [expression.KHR_character_expression.schema.json](./schema/expression.KHR_character_expression.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
