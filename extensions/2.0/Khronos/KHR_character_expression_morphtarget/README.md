# KHR_character_expression_morphtarget

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
Requires the extension(s): `KHR_character`,`KHR_character_expression`
Used in conjunction with: `KHR_character_expression_mapping`

## Overview

The `KHR_character_expression_morphtarget` extension provides semantic bindings between character expressions and specific morph target indices in a glTF mesh. These mappings enable higher-level expression control systems to reference the correct blendshapes when driving facial animation using glTF animation channels.

This extension does **not** define animation data itself. Instead, it enables consistent reference of morph target indices across tools, runtimes, and expressions.

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
          "animation": 0,
          "extensions": {
            "KHR_character_expressions_morphtarget": {
              "channels": [0]
            }
          }
        },
        {
          "expression": "frown",
          "animation": 1,
          "extensions": {
            "KHR_character_expressions_morphtarget": {
              "channels": [0]
            }
          }
        }
      ]
    }
  }
}
```

### Properties

| Property  | Type    | Description                                |
| --------- | ------- | ------------------------------------------ |
| `channel` | integet | Index representing the `"weights"` channel |

## Animation Integration (Expressions Tab Recommendation)

This extension **does not animate morph targets directly**. It provides metadata only.

All morph target expressions should be driven using standard glTF animation channels, targeting the `weights` path on the corresponding node:

```json
{
  "animations": [
    {
      "channels": [
        {
          "sampler": 0,
          "target": {
            "node": 0,
            "path": "weights"
          }
        }
      ],
      "samplers": [
        {
          "input": 0,
          "output": 1,
          "interpolation": "LINEAR"
        }
      ]
    }
  ]
}
```

- Use `"interpolation": "STEP"` for binary/toggle expressions like `blink`.
- Each element of the `weights` array corresponds to one morph target on the associated mesh primitive.
- Expression systems should coordinate with these indices using the bindings declared by this extension.

## Example: Expression with glTF Animation

### Step 1: Bind Expression to Morph Target

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "smile",
          "animation": 0,
          "extensions": {
            "KHR_character_expressions_morphtarget": {
              "channels": [0]
            }
          }
        },
        {
          "expression": "frown",
          "animation": 1,
          "extensions": {
            "KHR_character_expressions_morphtarget": {
              "channels": [0]
            }
          }
        }
      ]
    }
  }
}
```

This binds morph target index 2 on mesh 0, primitive 0, to the expression `"blinkLeft"`.

### Step 2: Animate the Morph Target via Node Weights

```json
{
  "nodes": [
    {
      "mesh": 0,
      "weights": [0.0, 0.0, 0.0]
    }
  ],
  "animations": [
    {
      "channels": [
        {
          "sampler": 0,
          "target": {
            "node": 0,
            "path": "weights"
          }
        }
      ],
      "samplers": [
        {
          "input": 0,
          "output": 1,
          "interpolation": "STEP"
        }
      ]
    }
  ]
}
```

The animation should update the relevant morph target weight (in this case index 2) between `0.0` and `1.0` to control the `"blinkLeft"` expression. The usage of `STEP` interpolation ensures clean toggles between on/off states.

## Implementation Notes

- This extension provides a consistent mapping between expression names and mesh targets, allowing glTF animation data to be expressive-aware.
- Multiple morph targets can be bound to the same expression.
- Tools may use this metadata to validate expression sets, retarget character blendshape names, or drive runtime animation.

### Recommended Interpolation for Binary Expressions

For expressions that represent binary or toggle states (such as eye blinks, mouth open/close states, or other on/off expressions), the use of glTF animation channels with `"interpolation": "STEP"` is strongly recommended.

Using STEP interpolation ensures that the expression toggles cleanly between fully off (0) and fully on (1) states, providing crisp transitions and avoiding unintended interpolation artifacts.

All expression-driven changes defined by this extension should rely on standard glTF animation mechanisms as described in the Khronos [Expressions Tab](https://docs.google.com/document/d/1ALRCPbXqQuWZvA9Sm2TsBFuW75h-of_lfD6sN7QwFU0/edit?pli=1&tab=t.x433oui5434f).

- Expression timing, blending, and control must use glTF `animations` channels.
- Animations targeting expression-driven `weights` (for morphtargets)
- This ensures consistency, ease of implementation, and interoperability across runtimes.

This approach simplifies character implementation by centralizing expression playback in the glTF animation system, reducing custom handling and improving cross-platform compatibility.

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
