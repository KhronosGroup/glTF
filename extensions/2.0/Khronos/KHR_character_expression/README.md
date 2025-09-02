# KHR_character_expression

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Individual Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Individual Contributor
- Nick Burkard, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.  
Requires the extension(s): `KHR_character`

## Overview

The `KHR_character_expression` extension provides a common interface for facial expression animations. It enables tools and runtimes to associate expressions like `blink`, `smile`, or `jawOpen` with specific animations in the glTF model's animations field.

When used in conjunction with the other expression extensions, enables a data contract with endpoints allowing them to understand just what kind of data is present and being powered.

This extension is purely descriptive: it does not define or store animation data itself.

## Reference Expression Categories/Vocabularies

Expressions in this context describe face-localized animations used to drive small and/or larger movements across the face and/or down-chain meshes needed for reasonable conveyance of emotion/intent.

For examples of relevant types of expressions, you can reference concepts such as:

- **Emotions** (Emotion-derived facial movements such as what [VRM defines as presets](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/expressions.md), e.g. `happy`, `angry`, `surprised`)
- **Visemes** (A visual representations of mouth movements for parts of speech, e.g. `aa`, `oo`, `th`)
- **FACS** ([Facial Action Coding System (FACS)](https://en.wikipedia.org/wiki/Facial_Action_Coding_System) which is a system intended to describe visually distinguishable facial movements (and is often split further based on left/right), e.g. `brow lowerer`, `chin raiser`, `lid droop`)
- **Gestures and Actions** (Larger descriptors that describe general facial actionse (but not emotion), e.g. `blink`, `smile`, `jawOpen`)

Optionally, these expressions may be aligned with industry standards (or an endpoint/experiences expected expressions set).

## Extension Schema

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "smile",
          "animation": 0
        },
        {
          "expression": "frown",
          "animation": 1
        }
      ]
    }
  }
}
```

### Properties

| Property      | Type    | Description                                                                    |
| ------------- | ------- | ------------------------------------------------------------------------------ |
| `expressions` | array   | Array of mappings between animation/channels and expression labels.            |
| `animation`   | integer | Index into the glTF `animations[]` array representing an expression animation. |
| `expression`  | string  | Expression name this joint contributes to.                                     |

## Animation Integration

- Expression timing, blending, and control must use glTF `animations` channels.
- This ensures consistency, ease of implementation, and interoperability across runtimes.

Each animation channel used to drive an expression should operate within a **normalized 0-to-1 range**, where:

- `0.0` indicates the expression is fully inactive.
- `1.0` indicates the expression is fully active.

The transformation values themselves (e.g., degree of rotation or distance of translation) should scale proportionally with the normalized input range.

This approach simplifies character implementation by centralizing expression playback in the glTF animation system and unifying runtime logic for blending and prioritization.

### Recommended Interpolation for Binary Expressions

For expressions that represent binary or toggle states (such as `blinkLeft`, `blinkRight`, or `jawOpen`), the use of glTF animation channels with `"interpolation": "STEP"` is strongly recommended.

STEP interpolation ensures that an expression toggles cleanly between fully off (`0.0`) and fully on (`1.0`) states, providing crisp visual transitions and avoiding interpolation artifacts that could occur with `LINEAR` interpolation in binary scenarios.

## Extension Example w/ typed extensions

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "smile",
          "animation": 0,
          "extensions": {
            "KHR_avatar_expression_joint": {
              "channels": [0, 1]
            },
            "KHR_avatar_expression_texture": {
              "channels": [2]
            },
            "KHR_avatar_expression_morphtarget": {
              "channels": [4, 5]
            }
          }
        },
        {
          "expression": "frown",
          "animation": 1,
          "extensions": {
            "KHR_avatar_expression_joint": {
              "channels": [0]
            },
            "KHR_avatar_expression_texture": {
              "channels": [1, 2]
            },
            "KHR_avatar_expression_morphtarget": {
              "channels": [3]
            }
          }
        }
      ]
    }
  }
}
```

## Implementation Notes

- Expression states should be normalized to the [0.0–1.0] range for consistent runtime interpretation.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
