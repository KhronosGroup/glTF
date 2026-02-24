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

## Dependencies

Written against the glTF 2.0 specification.
Requires the extension(s): `KHR_character`, `KHR_character_expression`, `KHR_animation_pointer`
Used in conjunction with: `KHR_character_expression_mapping`

> **Note:** This extension explicitly requires `KHR_animation_pointer`. Implementations **MUST** support `KHR_animation_pointer` to use this extension. The `KHR_animation_pointer` extension enables animation of individual morph target weights via JSON pointers, which is essential for granular expression control.

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
            "KHR_character_expression_morphtarget": {
              "channels": [0]
            }
          }
        },
        {
          "expression": "frown",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_morphtarget": {
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
| `channel` | integer | Index representing the `"weights"` channel |

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

### Integration with KHR_animation_pointer

For advanced animation control of morph target weights, implementations should use the [`KHR_animation_pointer`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_animation_pointer) extension. This provides a standardized mechanism for animating weights via JSON pointers.

As defined in the [glTF Object Model](https://github.com/KhronosGroup/glTF/blob/main/specification/2.0/ObjectModel.adoc#core-pointers), the following pointer templates are supported for morph target weights:

| Pointer | Type | Description |
| ------- | ---- | ----------- |
| `/nodes/{}/weights` | `float[]` | Animate all morph target weights on a node simultaneously |
| `/nodes/{}/weights/{}` | `float` | Animate a specific morph target weight by index |

Where `{}` is replaced with the corresponding array element index (e.g., `/nodes/0/weights` or `/nodes/0/weights/2`).

### Example: Animating All Weights on a Node

This example animates all morph target weights on a node using the `/nodes/i/weights` pointer:

```json
{
  "animations": [
    {
      "channels": [
        {
          "sampler": 0,
          "target": {
            "path": "pointer",
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/nodes/0/weights"
              }
            }
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

### Example: Animating a Specific Weight Index

For more granular control, you can animate individual morph target weights using the `/nodes/i/weights/j` pointer. This is particularly useful when you want to animate a single expression without affecting other morph targets:

```json
{
  "animations": [
    {
      "channels": [
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

In this example, only the morph target at index 2 (e.g., `blinkLeft`) is animated, while other weights remain unchanged.

### Example: Animating Multiple Specific Weights

To animate multiple specific morph targets independently (e.g., left and right blinks with different timing):

```json
{
  "animations": [
    {
      "name": "blink",
      "channels": [
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
        },
        {
          "sampler": 1,
          "target": {
            "path": "pointer",
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/nodes/0/weights/3"
              }
            }
          }
        }
      ],
      "samplers": [
        {
          "input": 0,
          "output": 1,
          "interpolation": "STEP"
        },
        {
          "input": 2,
          "output": 3,
          "interpolation": "STEP"
        }
      ]
    }
  ]
}
```

This allows `blinkLeft` (weight index 2) and `blinkRight` (weight index 3) to be animated with different timing samplers.

This method ensures consistent and interoperable animation targeting for morph target-based expressions across glTF runtimes.

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
            "KHR_character_expression_morphtarget": {
              "channels": [0]
            }
          }
        },
        {
          "expression": "frown",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_morphtarget": {
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

### Blending Behavior

When blending morph target weights from multiple sources (e.g., layered animations or runtime overrides), implementations **SHOULD** use traditional linear interpolation (lerp):

```text
result = lerp(base_value, blend_value, blend_weight)
       = base_value + blend_weight * (blend_value - base_value)
```

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
