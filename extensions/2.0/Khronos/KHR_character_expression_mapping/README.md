# KHR_character_expression_mapping

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
Can be used alongside: `KHR_character_expression_morphtarget`, `KHR_character_expression_joints`, `KHR_character_expression_texture` or other expression sources

## Overview

This extension is intended to enable encoding multiple expression set mappings, mapping expression animations currently present in the model to expressions that a runtime/endpoint understands and can handle.

## Reference Expression Categories/Vocabularies

Expressions in this context describe face-localized animations used to drive small and/or larger movements across the face and/or down-chain meshes needed for reasonable conveyance of emotion/intent.

For examples of relevant types of expressions, you can reference concepts such as:

- **Emotions** (Emotion-derived facial movements such as what [VRM defines as presets](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/expressions.md), e.g. `happy`, `angry`, `surprised`)
- **Visemes** (A visual representations of mouth movements for parts of speech, e.g. `aa`, `oo`, `th`)
- **FACS** ([Facial Action Coding System (FACS)](https://en.wikipedia.org/wiki/Facial_Action_Coding_System) which is a system intended to describe visually distinguishable facial movements (and is often split further based on left/right), e.g. `brow lowerer`, `chin raiser`, `lid droop`)
- **Gestures and Actions** (Larger descriptors that describe general facial actionse (but not emotion), e.g. `blink`, `smile`, `jawOpen`)

Optionally, these expressions may be aligned with industry standards (or an endpoint/experiences expected expressions set).

## Extension Schema/Example

```json
{
  "extensionsUsed": ["KHR_character_expression_mapping"],
  "extensions": {
    "KHR_character_expression_mapping": {
      "expressionSetMappings": {
        "example_set_1": {
          "Smile": [
            { "source": "smileLeft", "weight": 0.8 },
            { "source": "smileRight", "weight": 0.8 },
          ],
          "LeftCheekRaise": [
            { "source": "smileLeft", "weight": 0.2 }
          ],
          "MouthOpen": [{ "source": "jawOpen", "weight": 1.0 }]
        },
        "example_set_2": {
          "smile_small": [
            { "source": "smileLeft", "weight": 0.2 }
            { "source": "smileRight", "weight": 0.2 }
          ],
          "left_dimple": [
            { "source": "smileLeft", "weight": 0.8 }
          ],
          "mouth_open": [{ "source": "jawOpen", "weight": 1.0 }]
        }
      }
    }
  }
}
```

### Breakdown and Lower-Level Properties

The structure of the data contained in the extension can be described as a dictionary of dictionaries:
Target Expression Set Name : Expression Mapping Dictionary (Expression Name : List/Array of objects containing the below properties)

| Property | Type   | Description                                                   |
| -------- | ------ | ------------------------------------------------------------- |
| `source` | string | Name of the source expression in the model.                   |
| `weight` | number | Relative influence of this source expression on the target. Weights represent the proportional contribution needed to achieve the desired target result and do not need to sum to any specific value. |

### Mapping Types

This extension supports both one-to-one and one-to-many mappings (as well as multiple instances of those mapping sets):

- **One-to-one**: An expression maps directly to a single reference vocabulary term with a proportional weight.
- **One-to-many**: An expression is composed from multiple reference terms, blended using assigned weights.

This allows developers to bridge between custom expression sets and shared vocabularies.

## Implementation Notes

- This extension is typically used at the top level of the glTF file.
- Expression names should match those used in `KHR_character_expression_morphtarget`, `KHR_character_expression_texture`, or `KHR_character_expression_joint`;
- Tools can interpret this mapping to apply automatic translation between expression sets.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
