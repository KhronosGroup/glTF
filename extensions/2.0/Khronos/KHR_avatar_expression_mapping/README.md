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

## Reference Expression Categories/Vocabularies

Expression names may be grouped into categories including:

- **Emotions** (e.g. `happy`, `angry`, `surprised`)
- **Visemes** (e.g. `aa`, `oo`, `th`)
- **Modifiers** (e.g. `left`, `right`, `upper`, `lower`)
- **Gestures and Actions** (e.g. `blink`, `smile`, `jawOpen`)

The `KHR_avatar_expression_mapping` extension provides a general-purpose mechanism for mapping expression names used in an avatar to a known expression vocabulary. This allows different authoring pipelines or runtimes to translate between heterogeneous expression sets.

## Extension Schema

```json
{
  "extensions": {
    "KHR_avatar_expression_mapping": {
      "mappings": {
        "smileLeft": [
          { "target": "Smile", "weight": 0.8 },
          { "target": "LeftCheekRaise", "weight": 0.2 }
        ],
        "jawOpen": [{ "target": "MouthOpen", "weight": 1.0 }]
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
