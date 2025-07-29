# KHR_avatar_expression_joint

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
Dependent on: `KHR_avatar`
Typically used in conjunction with: `KHR_avatar_expression_mapping`

## Overview

The `KHR_avatar_expression_joint` extension provides a semantic mapping between facial expressions and joint transformations in the glTF node hierarchy. It enables tools and runtimes to associate expressions like `blink`, `smile`, or `jawOpen` with specific nodes whose transforms are animated using standard glTF animation channels.

This extension is purely descriptive: it does not define or store animation data itself.

## Reference Expression Categories/Vocabularies

Expression types include:

- **Emotions** (e.g. `happy`, `angry`, `surprised`)
- **Visemes** (e.g. `aa`, `oo`, `th`)
- **Modifiers** (e.g. `left`, `right`, `upper`, `lower`)
- **Gestures and Actions** (e.g. `blink`, `smile`, `jawOpen`)

Optionally, these may be aligned with industry standards, such as [Facial Action Coding System (FACS)](https://en.wikipedia.org/wiki/Facial_Action_Coding_System).

## Extension Schema

```json
{
  "extensions": {
    "KHR_avatar_expression_joint": {
      "expressions": [
        {
          "expression": "smile",
          "animation": 0,
          "channels": [0, 1, 2]
        },
        {
          "expression": "frown",
          "animation": 1,
          "channels": [0, 1]
        }
      ]
    }
  }
}
```

### Properties

| Property      | Type    | Description                                                                                                                        |
| ------------- | ------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| `expressions` | array   | Array of mappings between animation/channels and expression labels.                                                                |
| `animation`   | integer | Index into the glTF `animations[]` array representing an expression animation.                                                     |
| `expression`  | string  | Expression name this joint contributes to.                                                                                         |
| `channels`    | array   | array representing channels that must correspond to either `"rotation"`, `"translation"`, or `"scale"`; indicates transform types. |

## Animation Integration

- Expression timing, blending, and control must use glTF `animations` channels.
- Animations targeting expression-driven `rotation`, `translation`, or `scale` must conform to glTF 2.0's animation model.
- This ensures consistency, ease of implementation, and interoperability across runtimes.

Each animation channel used to drive an expression should operate within a **normalized 0-to-1 range**, where:

- `0.0` indicates the expression is fully inactive.
- `1.0` indicates the expression is fully active.

The transformation values themselves (e.g., degree of rotation or distance of translation) should scale proportionally with the normalized input range.

This approach simplifies avatar implementation by centralizing expression playback in the glTF animation system and unifying runtime logic for blending and prioritization.

### Recommended Interpolation for Binary Expressions

For expressions that represent binary or toggle states (such as `blinkLeft`, `blinkRight`, or `jawOpen`), the use of glTF animation channels with `"interpolation": "STEP"` is strongly recommended.

STEP interpolation ensures that an expression toggles cleanly between fully off (`0.0`) and fully on (`1.0`) states, providing crisp visual transitions and avoiding interpolation artifacts that could occur with `LINEAR` interpolation in binary scenarios.

## Implementation Notes

- Multiple joints may be assigned to the same expression.
- Expression states should be normalized to the [0.0–1.0] range for consistent runtime interpretation.
- This extension does not conflict with standard rigging or skinning systems.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
