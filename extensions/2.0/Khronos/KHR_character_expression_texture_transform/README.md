# KHR_character_expression_texture_transform

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

Written against the glTF 2.0 specification and the [glTF 2.0 Asset Object Model](../../../../specification/2.0/ObjectModel.adoc) (AOM).

Requires the extensions: `KHR_character`, `KHR_character_expression`, `KHR_animation_pointer`, and `KHR_texture_transform`.

Assets using `KHR_character_expression_texture_transform` **MUST** list `KHR_character_expression_texture_transform`, `KHR_character_expression`, `KHR_character`, `KHR_animation_pointer`, and `KHR_texture_transform` in `extensionsUsed`. They **MUST** contain the top-level `KHR_character` and `KHR_character_expression` extension objects. A `KHR_character_expression_texture_transform` object **MUST** be attached to an expression entry in that top-level `KHR_character_expression` object.

Every listed channel uses `KHR_animation_pointer` to target a property of an authored `KHR_texture_transform` object as described below.

An asset **MAY** list `KHR_character_expression_texture_transform` in `extensionsRequired`. An asset that lists it there **SHOULD** also list `KHR_animation_pointer` and `KHR_texture_transform` in `extensionsRequired`. A consumer claiming support for this extension **MUST** recognize its texture-transform channel classifications, **MUST** produce response records for every channel listed here that it supports under `KHR_character_expression` when the entry's effective driver is positive, and **MUST** implement the additive expression-composition rules below. Under `KHR_character_expression`, channels listed here produce no response records on a consumer that does not support this extension. Emitting response records for those channels also requires support for each channel's target extensions (see Expression composition), which always include `KHR_animation_pointer` and `KHR_texture_transform`.

## Overview (Informative)

This extension classifies listed channels of an expression animation as texture-transform channels. It adds validation metadata and authoring rules for the channels it classifies, and defines additive offset and rotation composition. It does not bind an expression to a texture or texture index, or change the samples of the channels it lists. Scale composition and mixing expression results with ordinary animation and other runtime systems are host-defined.

The concrete target is supplied entirely by the listed channel's `KHR_animation_pointer` pointer. Channels listed here produce responses only on a consumer that supports this extension. A channel that no derived expression extension on the entry lists produces no response. Every channel in the selected animation still contributes to the base extension's duration.

## Extension structure (Normative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, materials, texture-transform objects, animations, accessors, and buffers.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "name": "happy",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_texture_transform": {
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

Each listed channel **MUST** have:

- `target.path` equal to `"pointer"`;
- no `target.node`; and
- a valid `KHR_animation_pointer` target whose pointer resolves to the `offset`, `scale`, or `rotation` property of a `KHR_texture_transform` extension object actually authored in the asset.

It is not sufficient for the pointed-to property to have the same shape or name elsewhere. The owning object **MUST** be an actual `KHR_texture_transform` object attached at a location allowed by that extension.

Each listed channel and its sampler **MUST** satisfy all core animation, `KHR_animation_pointer`, `KHR_texture_transform`, and `KHR_character_expression` rules. In particular:

| Target property | AOM value | Output accessor type |
| --- | --- | --- |
| `offset` | two floating-point components | `VEC2` |
| `scale` | two floating-point components | `VEC2` |
| `rotation` | one floating-point component in radians | `SCALAR` |

The output accessor's component type and values **MUST** satisfy `KHR_animation_pointer` and the target property's constraints.

The authored initial value of each listed channel's target is resolved as defined by `KHR_character_expression` (see Authored initial state). Within an authored `KHR_texture_transform` object, omitted `offset`, `scale`, and `rotation` resolve to `[0, 0]`, `[1, 1]`, and `0`, respectively, as defined by `KHR_texture_transform`.

## Expression composition (Normative)

`rotation` is a rotation angle in radians and is sampled under the radian-angle sampling rule in [`KHR_character_expression` Sampling](../KHR_character_expression/README.md#sampling), so a key sequence describes the full signed rotation path, including turns of more than one revolution.

Each response record holds its ordinary absolute sampled `offset`, `scale`, or `rotation` value. Listing a channel here does not reinterpret its sample as a delta or blend weight, or change its driver-selected sample time.

`rotation` uses the additive formula (see `KHR_character_expression`, Additive aggregation) on raw angles: `n_r` and `s_r` are the raw samples produced by sampling. Neither sample is canonicalized, wrapped, or otherwise rewritten.

The contribution `s_r - n_r` **MUST** be computed without reduction modulo `2 * pi`, and the aggregate result **MUST NOT** be wrapped. All `rotation` contributions, including a single one, use the additive aggregation defined by `KHR_character_expression`. This preserves authored winding and rotations of more than one revolution.

For example, with an authored `rotation` of `2 * pi`, a decoded neutral sample of `2 * pi` (one full turn) and current sample of `6 * pi` (three full turns) contribute `4 * pi` — two full turns — rather than zero.

`offset` uses the additive rule independently for its two components, and its aggregate is subject to the no-clamp rule (see `KHR_character_expression`, Additive aggregation).

Multiple expression entries **MAY** target the same texture transform's `offset`, `scale`, or `rotation`. Texture-transform `scale` is nonadditive: its per-entry response remains the ordinary absolute sampled vector, and composition of scale responses is host-defined (see `KHR_character_expression`, Other composition). The within-animation duplicate-target prohibition and all target-property validity rules continue to apply. Mixing the resulting expression responses with ordinary animation and other property writers remains host-defined (see `KHR_character_expression`, Host integration boundary).

The concrete target is the resolved property of one specific authored `KHR_texture_transform` object, not merely a terminal property name such as `offset` or a shared texture index. The target extensions of a listed channel, as defined by `KHR_character_expression`, are `KHR_animation_pointer`, `KHR_texture_transform`, and any extension that defines an object on the pointer's path, such as a material extension that defines the containing `textureInfo`. A validator that lacks support for any of them follows the incomplete-validation rules of `KHR_character_expression` (see [Validation boundary](../KHR_character_expression/README.md#validation-boundary)).

Listing a channel here makes it eligible to produce a response record but does not bind or otherwise modify that record. The rule against listing one channel in more than one derived expression extension on the same expression entry is defined by `KHR_character_expression` (Derived expression extensions).

## Atlas cells and discrete texture regions (Informative)

A texture atlas packs several images into one texture. An expression can select one cell of an atlas by animating `offset`, usually against a fixed `scale`. This extension defines no atlas layout, no cell grid, no cell index, and no required padding allowance. A cell is an authoring convention carried entirely by the authored `offset` and `scale` values.

The expression sample time defined by `KHR_character_expression` varies continuously with the driver, so a sampler using `LINEAR` or `CUBICSPLINE` interpolation produces values between its authored keys. Between two keys that select adjacent cells, those values address a region that straddles both cells. `CUBICSPLINE` can also overshoot past the authored keys and address a region that no key selects.

A listed channel authored to address discrete regions should declare `STEP` interpolation on its sampler. `STEP` is not the core default, so the sampler declares `interpolation` explicitly.

Under `STEP`, a key's value applies once the expression sample time reaches that key's time, and the sample time reaches its maximum `T` only at an effective driver of `1`. A channel whose switching key is at `T` therefore selects its final cell only at full driver, which a suppression factor or a mapping weight below `1` can make unreachable. Placing the switching key at the driver level where the change is intended, and keeping a final key at `T` that holds the selected cell, avoids this (see [`KHR_character_expression` Authoring guidance](../KHR_character_expression/README.md#authoring-guidance-informative)).

Offsets from expression entries that are active at the same time add up under the additive rule, so two atlas-selecting entries can together address another cell or a region outside `[0, 1]`. These contributions are each channel's change from its own time-zero sample, added to the authored initial offset. Entries whose cells exclude each other need to avoid contributing nonzero offset changes at the same time. One conservative way is to keep their effective drivers from being positive together, for example through a `block` suppression of [`KHR_character_expression_suppression`](../KHR_character_expression_suppression/README.md) from one entry to the other with an `amount` of `1` and a `threshold` of `0` (the defaults), which makes the target's effective driver `0` whenever the source's driver is positive.

### Atlas authoring guidance (Informative)

`STEP` holds one authored value across the interval between two keys, but it does not stop texture filtering from reading texels of a neighboring cell at a cell border. Authors can separate cells by a gutter of duplicated edge texels, wide enough for the filter and the mip levels the texture is sampled with. No single gutter width is correct for every texture.

Keeping transformed texture coordinates within `[0, 1]` avoids making cell selection depend on the texture sampler's `wrapS` and `wrapT` properties. This extension assumes no `wrap` mode and does not constrain those properties. `KHR_texture_transform` states the condition under which values outside that range still produce a usable result.

On a listed channel that does not address discrete regions, including continuous texture-coordinate animation such as scrolling, `LINEAR` and `CUBICSPLINE` are appropriate.

## Partial animation-channel example (Informative)

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, the top-level `KHR_character_expression` object and its expression entries, the `KHR_character_expression_texture_transform` object whose `channels` array lists the channel, the containing animation and its sampler, the material and the `KHR_texture_transform` object at the pointed-to location, accessors, buffers, and matching authored initial values.

```json
{
  "sampler": 0,
  "target": {
    "path": "pointer",
    "extensions": {
      "KHR_animation_pointer": {
        "pointer": "/materials/2/pbrMetallicRoughness/baseColorTexture/extensions/KHR_texture_transform/scale"
      }
    }
  }
}
```

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## Schema

- [expression.KHR_character_expression_texture_transform.schema.json](./schema/expression.KHR_character_expression_texture_transform.schema.json)

A `KHR_character_expression_texture_transform` object is placed in the `extensions` property of an expression entry, keyed by the extension name, rather than on a core glTF object. A validator matches the object to the schema above by that key.

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
