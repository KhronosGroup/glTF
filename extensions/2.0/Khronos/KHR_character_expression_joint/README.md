# KHR_character_expression_joint

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
Typically used in conjunction with: `KHR_character_expression_mapping`

Assets using `KHR_character_expression_joint` MUST list `KHR_character_expression_joint`, `KHR_character_expression`, `KHR_character`, and the transitive `KHR_xmp_json_ld` dependency in `extensionsUsed`. They MUST contain the top-level `KHR_character` and `KHR_character_expression` extension objects. The `KHR_character_expression_joint` object MUST be attached to an expression entry in that top-level expression object.

## Overview

The `KHR_character_expression_joint` extension provides a semantic mapping between facial expression names and joint-based animations. It enables tools and runtimes to associate expressions like `blink`, `smile`, or `jawOpen` with specific nodes whose transforms are animated using standard glTF animation channels.

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

The following is a partial extension fragment. Dependency declarations, the character objects, and the referenced animations are omitted.

```json
{
  "extensions": {
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "smile",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_joint": {
              "channels": [0, 1, 2]
            }
          }
        },
        {
          "expression": "frown",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_joint": {
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

| Property   | Type  | Description                                                                                                                        |
| ---------- | ----- | ---------------------------------------------------------------------------------------------------------------------------------- |
| `channels` | array | Nonempty array of unique indices into the `channels` array of the animation selected by the containing expression entry. Each selected channel MUST target `"rotation"`, `"translation"`, or `"scale"`. |

## Animation Integration

- Expression timing, blending, and control must use glTF `animations` channels.
- Animations targeting expression-driven `rotation`, `translation`, or `scale` must conform to glTF 2.0's animation model.
- This ensures consistency, ease of implementation, and interoperability across runtimes.

Each animation channel used to drive an expression should operate within a **normalized 0-to-1 range**, where:

- `0.0` indicates the expression is fully inactive.
- `1.0` indicates the expression is fully active.

The transformation values themselves (e.g., degree of rotation or distance of translation) should scale proportionally with the normalized input range.

This approach simplifies character implementation by centralizing expression playback in the glTF animation system and unifying runtime logic for blending and prioritization.

### Recommended Interpolation for Binary Expressions

For expressions that represent binary or toggle states (such as `blinkLeft`, `blinkRight`, or `jawOpen`), the use of glTF animation channels with `"interpolation": "STEP"` is strongly recommended.

STEP interpolation ensures that an expression toggles cleanly between fully off (`0.0`) and fully on (`1.0`) states, providing crisp visual transitions and avoiding interpolation artifacts that could occur with `LINEAR` interpolation in binary scenarios.

## Implementation Notes

- Multiple joints may be assigned to the same expression.
- Expression states should be normalized to the [0.0–1.0] range for consistent runtime interpretation.
- This extension does not conflict with standard rigging or skinning systems.

### Blending Behavior

When blending joint transforms from multiple sources (e.g., layered animations or runtime overrides), implementations **SHOULD** use the following approaches:

**For translation and scale values**, use traditional linear interpolation (lerp):

```text
result = lerp(base_value, blend_value, blend_weight)
       = base_value + blend_weight * (blend_value - base_value)
```

**For rotation values** (quaternions), use the logarithmic (log/exp) approach to ensure smooth, geodesic interpolation on the rotation manifold:

```text
result = exp(lerp(log(base_rotation), log(blend_rotation), blend_weight))
```

This approach:

- Provides the shortest-path interpolation between rotations
- Avoids issues with gimbal lock present in Euler angle representations
- Produces more natural blending for skeletal animations, especially for large rotation differences

## Known Implementations

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## License

This extension is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
