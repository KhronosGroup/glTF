# KHR_avatar_expression_mapping

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
Can be used alongside: `KHR_avatar_expression_morphtargets` or other expression sources

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

## Extension Schema

```json
{
  "extensions": {
    "KHR_avatar_expression_mapping": {
      "expressionSetMappings": {
        "example_1": {
          "smileLeft": [
            { "target": "Smile", "weight": 0.8 },
            { "target": "LeftCheekRaise", "weight": 0.2 }
          ],
          "jawOpen": [{ "target": "MouthOpen", "weight": 1.0 }]
        },
        "example_2": {
          "smileLeft": [
            { "target": "smile_small", "weight": 0.2 },
            { "target": "left_dimple", "weight": 0.8 }
          ],
          "jawOpen": [{ "target": "mouth_open", "weight": 1.0 }]
        }
      }
    }
  }
}
```

### Properties

| Property   | Type   | Description                                                        |
| ---------- | ------ | ------------------------------------------------------------------ |
| `mappings` | object | Dictionary mapping expression names to reference vocabulary terms. |
| `target`   | string | Name of the expression in the target vocabulary.                   |
| `weight`   | number | Influence of this target. Must sum to 1.0 per expression key.      |

### Mapping Types

This extension supports both one-to-one and one-to-many mappings:

- **One-to-one**: An expression maps directly to a single reference vocabulary term with weight 1.0.
- **One-to-many**: An expression is composed from multiple reference terms, blended using assigned weights.

This allows developers to bridge between custom expression sets and shared vocabularies.

## Implementation Notes

- This extension is typically used at the top level of the glTF file.
- Expression names should match those used in `KHR_avatar_expression_morphtargets`, animation tracks, or tracking pipelines.
- Tools can interpret this mapping to apply automatic translation between expression sets.

## Example

```json
{
  "extensionsUsed": ["KHR_avatar_expression_mapping"],
  "extensions": {
    "KHR_avatar_expression_mapping": {
      "mappings": {
        "smileLeft": [
          { "target": "Smile", "weight": 0.8 },
          { "target": "LeftCheekRaise", "weight": 0.2 }
        ]
      }
    }
  }
}
```

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
