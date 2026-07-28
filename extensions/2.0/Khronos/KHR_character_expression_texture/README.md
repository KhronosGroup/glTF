# KHR_character_expression_texture

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
Can be used alongside: `KHR_character_expression_mapping`

## Overview

The `KHR_character_expression_texture` extension enables expression-level control using UV transformations. This approach is beneficial for characters that represent expressions visually using texture atlases, such as cartoon or anime-style characters.

- Expression timing, blending, and control must use glTF `animations` channels.
- Animations targeting expression-driven texture transforms must adhere strictly to glTF animation standards and `KHR_animation_pointer` semantics.

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

| `channels` | array | Array representing the target channels that are texture transform-based. |

### textureTransform properties

| Property | Type     | Description                          |
| -------- | -------- | ------------------------------------ |
| `offset` | float[2] | UV offset for texture placement.     |
| `scale`  | float[2] | UV scale for texture transformation. |
| `rotation` | float | UV rotation in radians. |

### Integration with KHR_animation_pointer

Texture transformation animations use the [`KHR_animation_pointer`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_animation_pointer) extension. This provides a standardized mechanism for animating `KHR_texture_transform` properties via JSON pointers.

This method ensures consistent and interoperable animation targeting for texture-based expressions across glTF runtimes.

## UV Transformations

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
            "path": "pointer",
            "extensions": {
              "KHR_animation_pointer": {
                "pointer": "/materials/2/pbrMetallicRoughness/baseColorTexture/extensions/KHR_texture_transform/scale"
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

This associates the listed texture-transform animation channels with the `"happy"` and `"angry"` expressions.

### Step 2: Animate Texture Properties Using KHR_animation_pointer

Animation of texture-transform properties must be implemented using `KHR_animation_pointer`.

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
                "pointer": "/materials/2/pbrMetallicRoughness/baseColorTexture/extensions/KHR_texture_transform/scale"
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
- Expression-driven UV transformations are typically applied to materials on facial regions, such as eyes or mouth.
- This extension does not define animation sequences, only the semantic binding between expressions and texture transforms.
- The `STEP` interpolation may be used for binary changes between regions of a texture atlas.
- The `LINEAR` interpolation may be used for UV offset transitions or subtle animations.

### Blending Behavior

When blending UV transform values (offset, scale) from multiple sources (e.g., layered animations or runtime overrides), implementations **SHOULD** use traditional linear interpolation (lerp):

```text
result = lerp(base_value, blend_value, blend_weight)
       = base_value + blend_weight * (blend_value - base_value)
```

### Runtime Behavior

- Expression weights should animate between `0.0` (off) and `1.0` (fully active).
- The character system uses these animations to transform texture coordinates in accordance with semantic expressions.

## Known Implementations

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
