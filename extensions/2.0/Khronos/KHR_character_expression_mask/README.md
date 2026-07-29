# KHR_character_expression_mask

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

## Dependencies

Written against the glTF 2.0 specification.
Requires the extension(s): `KHR_character`, `KHR_character_expression`
Works alongside: `KHR_character_expression_mapping`, `KHR_character_expression_joint`, `KHR_character_expression_texture`, `KHR_character_expression_morphtarget`.

Assets using `KHR_character_expression_mask` MUST list `KHR_character_expression_mask`, `KHR_character_expression`, `KHR_character`, and the transitive `KHR_xmp_json_ld` dependency in `extensionsUsed`. They MUST contain the top-level `KHR_character` and `KHR_character_expression` extension objects. The `KHR_character_expression_mask` object MUST be attached to an expression entry in that top-level expression object.

## Overview

The `KHR_character_expression_mask` extension allows artists to define how one expression masks or influences another expression. This prevents unwanted interactions between different expressions, such as:

- Emotional expressions (e.g., "happy", "angry") conflicting with lip-sync phonemes
- Multiple expressions targeting the same facial region
- Procedural animations overriding or blending with authored animations

By defining masks on expressions, this extension enables runtime systems to correctly resolve conflicts between expressions without application-specific logic.

### Motivation

When applying emotional expressions such as "happy" or "angry" while lip-sync is active, the avatar's expression may collapse depending on the implementation—for example, "opening the mouth twice." However, certain avatars may want to express both expressions simultaneously with controlled blending. This behavior is not deterministic from the application side and is better defined by the asset.

This extension provides VRM-compatible expression override behavior, allowing assets to define how expressions interact with each other.

## Use Cases

- Define how emotional expressions reduce lip-sync phoneme influence
- Prevent conflicting expressions from combining incorrectly
- Allow artists to control expression priority and blending behavior
- Enable consistent cross-platform expression behavior

## Behavior

An expression with masks defines how it affects other target expressions. It is allowed to have multiple masks targeting the same expression from different sources, and the total influence is the product of all mask influences.

### "blend" Masks

A "blend" mask reduces the target expression proportionally to the source expression's value.

Given two expressions "aa" (lip-sync phoneme) and "happy" (emotion), where "happy" has a "blend" mask targeting "aa" with an amount of `x`:

```text
aa_out = aa_in * (1 - x * happy_in)
happy_out = happy_in
```

When "happy" is fully active (1.0) with amount 0.5, the "aa" expression is reduced to 50% of its input value.

### "block" Masks

A "block" mask fully reduces the target expression when the source expression exceeds a threshold.

Given two expressions "aa" and "angry", where "angry" has a "block" mask targeting "aa" with an amount of `x` and a threshold of `y`:

```text
aa_out = aa_in * (1 - (angry_in > y ? x : 0))
angry_out = angry_in
```

When the threshold is 0, the target is blocked unless the source expression is completely inactive.

### Execution Order

The execution order of expressions does not affect the result because:

- The influence only evaluates the input values of expressions
- The total influence is the product of all masks targeting the same expression, which is commutative

## Schema

The following is a partial extension fragment. Dependency declarations, the character objects, and animations are omitted.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "aa",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_morphtarget": {
              "channels": [0]
            }
          }
        },
        {
          "expression": "happy",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_morphtarget": {
              "channels": [0]
            },
            "KHR_character_expression_mask": {
              "masks": [
                {
                  "target": 0,
                  "type": "blend",
                  "amount": 0.5
                }
              ]
            }
          }
        },
        {
          "expression": "angry",
          "animation": 2,
          "extensions": {
            "KHR_character_expression_morphtarget": {
              "channels": [0]
            },
            "KHR_character_expression_mask": {
              "masks": [
                {
                  "target": 0,
                  "type": "block",
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

## Properties

### KHR_character_expression_mask

| Property | Type  | Description                                                           | Required |
| -------- | ----- | --------------------------------------------------------------------- | -------- |
| `masks`  | array | An array of mask objects that define how this expression affects others | Yes      |

### Mask Object

| Property    | Type   | Description                                                                                      | Required |
| ----------- | ------ | ------------------------------------------------------------------------------------------------ | -------- |
| `target`    | integer | Index of the target expression in the top-level `KHR_character_expression.expressions[]` array. | Yes      |
| `type`      | string | The mask type. `"blend"` and `"block"` are standardized; custom nonempty strings are permitted. Default: `"blend"` | No       |
| `amount`    | number | The amount of influence (0.0–1.0). Default: `1.0`                                                | No       |
| `threshold` | number | For `"block"` type: the threshold above which the target is blocked (0.0–1.0). Default: `0.0`   | No       |

Each `target` MUST resolve to an entry in the top-level `KHR_character_expression.expressions[]` array. Expression indices are independent of expression labels and remain unambiguous when labels are duplicated or changed.

### Mask Type Enum

The following values have standardized behavior. Other nonempty strings are application-defined mask types and MUST be preserved by readers even when their behavior is not understood.

| Value   | Description                                                                            |
| ------- | -------------------------------------------------------------------------------------- |
| `blend` | Reduces the target proportionally to this expression's value multiplied by the amount  |
| `block` | Fully reduces the target by the amount when this expression exceeds the threshold      |

## Implementation Notes

- Multiple masks from different expressions may target the same expression; the combined effect is multiplicative
- The `threshold` property is only meaningful for `"block"` type masks
- Implementations should evaluate all mask influences before applying expression outputs
- Expression values should be normalized to the [0.0–1.0] range

## Blending Behavior Recommendations

When blending expression values from multiple sources, implementations **SHOULD** use traditional linear interpolation (lerp):

```text
result = lerp(base_value, blend_value, blend_weight)
       = base_value + blend_weight * (blend_value - base_value)
```

## Known Implementations

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## License

This extension is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
