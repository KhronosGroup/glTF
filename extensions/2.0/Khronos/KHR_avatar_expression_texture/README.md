# KHR_avatar_expression_texture

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Individual Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Individual Contributor

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.  
Dependent on: `KHR_avatar`,`KHR_animation_pointer` 
Can be used alongside: `KHR_avatar_expression_mapping`

## Overview

The `KHR_avatar_expression_texture` extension enables expression-level control using texture swaps or UV transformations. This approach is beneficial for avatars that represent expressions visually via changes in texture, such as cartoon or anime-style characters.

- Expression timing, blending, and control must use glTF `animations` channels.
- Animations targeting expression-driven texture transforms must adhere strictly to glTF animation standards and khr_animation_pointer semantics.

## Reference Expression Vocabulary

Expressions may be categorized as:

- **Emotions** (e.g., `happy`, `angry`, `surprised`)
- **Visemes** (e.g., `aa`, `oo`, `th`)
- **Modifiers** (e.g., `left`, `right`, `upper`, `lower`)
- **Gestures and Actions** (e.g., `blink`, `smile`, `jawOpen`)

Optionally, these may be aligned with industry standards, such as [Facial Action Coding System (FACS)](https://en.wikipedia.org/wiki/Facial_Action_Coding_System). 

## Extension Schema

```json
{
  "extensions": {
    "KHR_avatar_expression_texture": {
      "expressions": [
        {
          "material": 2,
          "texture": 5,
          "expression": "happy",
          "animation": 0,
          "channels": [0]
        },
        {
          "material": 2,
          "texture": 5,
          "expression": "angry",
          "animation": 1,
          "channels": [0,1]
        }
      ]
    }
  }
}
```

### Properties

| Property            | Type           | Description                                                                   |
|---------------------|----------------|-------------------------------------------------------------------------------|
| `expressions`          | array          | List of texture-expression bindings.                                          |
| `material`          | integer        | Index into the glTF `materials` array.                                        |
| `texture`           | integer        | (Optional) Index into the glTF `textures` array, defining a replacement texture. |
| `textureTransform`  | object         | (Optional) UV transformations for texture-based expressions.                  |
| `expression`        | string         | Expression name associated with this texture or UV change.                    |

### textureTransform properties

| Property | Type      | Description                          |
|----------|-----------|--------------------------------------|
| `offset` | float[2]  | UV offset for texture placement.     |
| `scale`  | float[2]  | UV scale for texture transformation. |


### Recommended Interpolation for Binary Expressions

For expressions that represent binary or toggle states (such as eye blinks, mouth open/close states, or other on/off expressions), the use of glTF animation channels with `"interpolation": "STEP"` is strongly recommended.

Using STEP interpolation ensures that the expression toggles cleanly between fully off (0) and fully on (1) states, providing crisp transitions and avoiding unintended interpolation artifacts.


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
    "KHR_avatar_expression_texture": {
      "expressions": [
        {
          "material": 2,
          "texture": 5,
          "expression": "happy",
          "animation": 0,
          "channels": [0, 1]
        },
        {
          "material": 2,
          "texture": 5,
          "expression": "angry",
          "animation": 1,
          "channels": [0,1]
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
- The avatar system uses these animations to blend or toggle texture visuals in accordance with semantic expressions.

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
