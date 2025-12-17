# KHR_character_expression_texture

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.
Requires the extension(s): `KHR_character`, `KHR_character_expression`, `KHR_animation_pointer`
Can be used alongside: `KHR_character_expression_mapping`

## Overview

The `KHR_character_expression_texture` extension enables expression-level control using texture swaps or UV transformations. This approach is beneficial for characters that represent expressions visually via changes in texture, such as cartoon or anime-style characters.

- Expression timing, blending, and control must use glTF `animations` channels.
- Animations targeting expression-driven texture transforms must adhere strictly to glTF animation standards and khr_animation_pointer semantics.

## Reference Expression Vocabulary

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
          "expression": "happy",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_texture": {
              "channels": [0]
            }
          }
        },
        {
          "expression": "angry",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_texture": {
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

| Property | Type | Description |
| -------- | ---- | ----------- |

| `channel` | array | Array representing the target channels that are texture transform-based |

### textureTransform properties

| Property | Type     | Description                          |
| -------- | -------- | ------------------------------------ |
| `offset` | float[2] | UV offset for texture placement.     |
| `scale`  | float[2] | UV scale for texture transformation. |

### Integration with KHR_animation_pointer

For animations that involve texture swaps or UV transformations, implementations should rely on the [`KHR_animation_pointer`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_animation_pointer) extension. This provides a standardized mechanism for animating texture and UV transform properties via JSON pointers.

This method ensures consistent and interoperable animation targeting for texture-based expressions across glTF runtimes.

## Clarifications on Texture Manipulations

### Texture Swaps

Dynamic texture swaps are not explicitly defined within the core glTF 2.0 specification. To implement runtime texture swapping:

- Predefine all textures in the glTF `textures` array.
- Use the [`KHR_animation_pointer`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_animation_pointer) extension to animate pointers that reference texture indices within material definitions.

### UV Transformations

UV manipulations (offset, scale, rotation) require the widely adopted [`KHR_texture_transform`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_transform) extension.

- Animate properties within `KHR_texture_transform` using `KHR_animation_pointer`.
- Ensure these transforms are included explicitly in the glTF material definitions to enable animations.

### Example Animation Setup using KHR_animation_pointer

```json
{
  "animations": [
    {
      "channels": [
        {
          "sampler": 0,
          "target": {
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/materials/2/pbrMetallicRoughness/baseColorTexture/index"
              }
            }
          }
        },
        {
          "sampler": 1,
          "target": {
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/materials/3/pbrMetallicRoughness/baseColorTexture/extensions/KHR_texture_transform/offset"
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
          "interpolation": "LINEAR"
        }
      ]
    }
  ]
}
```

This clearly separates semantic bindings (the domain of this extension) from runtime animation states (handled by standard glTF animation mechanisms).

### Recommended Interpolation for Binary Expressions

For expressions that represent binary or toggle states (such as eye blinks, mouth open/close states, or other on/off expressions), the use of glTF animation channels with `"interpolation": "STEP"` is strongly recommended.

Using STEP interpolation ensures that the expression toggles cleanly between fully off (0) and fully on (1) states, providing crisp transitions and avoiding unintended interpolation artifacts.

## Example: Expression with glTF Animation

### Step 1: Bind Expressions to Materials

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "happy",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_texture": {
              "channels": [0, 1]
            }
          }
        },
        {
          "expression": "angry",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_texture": {
              "channels": [0, 1]
            }
          }
        }
      ]
    }
  }
}
```

This binds material index 2 to the `"happy"` expression and material index 3 to `"angry"`.

### Step 2: Animate Texture Properties Using KHR_animation_pointer

Animation of either the texture index or texture transform must be implemented using `KHR_animation_pointer`.

```json
{
  "animations": [
    {
      "channels": [
        {
          "sampler": 0,
          "target": {
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/materials/2/pbrMetallicRoughness/baseColorTexture/index"
              }
            }
          }
        },
        {
          "sampler": 1,
          "target": {
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/materials/3/pbrMetallicRoughness/baseColorTexture/extensions/KHR_texture_transform/offset"
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
          "interpolation": "LINEAR"
        }
      ]
    }
  ]
}
```

## Implementation Notes

- Use this extension when morph targets or joint animations alone are insufficient or stylistically undesirable for expressions.
- Expression-driven texture swaps or UV transformations are typically applied to materials on facial regions, such as eyes or mouth.
- This extension does not define animation sequences, only the semantic binding between expressions and textures or UV transforms.
- The `STEP` interpolation is ideal for switching texture indices (e.g., on/off or binary swaps).
- The `LINEAR` interpolation may be used for UV offset transitions or subtle animations.

### Runtime Behavior

- Expression weights should animate between `0.0` (off) and `1.0` (fully active).
- The character system uses these animations to blend or toggle texture visuals in accordance with semantic expressions.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
