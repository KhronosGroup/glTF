# KHR_character_skeleton_bindpose

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
Requires the extension(s):  `KHR_character`

## Overview

The `KHR_character_skeleton_bindpose` extension defines one or more canonical bind poses for the character's skeleton. These poses serve as neutral references that support animation retargeting, mesh deformation validation, and consistency across toolchains. Multiple bind poses can be defined per-skin (each with a unique pose type), using the same joint ordering as the skin's joints array for direct mapping and maximum simplicity.

## Extension Schema

This extension is applied to individual skins in the glTF file, providing bind pose data that corresponds directly to the skin's joint array. Multiple bind poses can be defined, each with a unique `poseType`.

```json
{
  "skins": [
    {
      "joints": [2, 5, 8, 12],
      "extensions": {
        "KHR_character_skeleton_bindpose": {
          "poses": [
            {
              "poseType": "TPose",
              "bindPoses": [
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.0, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.5, 0, 1] }
              ]
            },
            {
              "poseType": "IPose",
              "bindPoses": [
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.0, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.5, 0, 1] }
              ]
            }
          ]
        }
      }
    }
  ]
}
```

### Properties

| Property | Type | Description |
| -------- | ---- | ----------- |
| `poses` | array | Array of pose definitions. Each pose has a unique `poseType` and corresponding `bindPoses` array. |

#### poses Array

Each entry in the `poses` array contains:

| Property | Type | Description |
| -------- | ---- | ----------- |
| `poseType` | string | Enum: `TPose`, `APose`, `IPose`, `Custom`. Declares the pose classification for this bind pose set. |
| `bindPoses` | array | List of bind pose matrices corresponding directly to the skin's joints array. Each entry at index i corresponds to the joint at joints[i]. |

#### bindPoses Array

Each entry in the `bindPoses` array contains:

| Property | Type | Description |
| -------- | ---- | ----------- |
| `matrix` | float[16] | Column-major 4x4 matrix defining the joint's bind pose in model space. |

## Per-Skin Joint Indexing

The `bindPoses` array uses per-skin joint indexing for maximum simplicity and glTF alignment:

- **Direct Mapping**: Each entry in the bind pose array corresponds directly to the joint at the same index in the skin's joints array.
- **Simplicity**: No need for extra mapping or referencing—just match the order.
- **Consistency**: This matches how glTF handles skinning weights and inverse bind matrices, making it intuitive for tool and engine implementers.
- **Efficiency**: No redundant data; everything is scoped to the skin and its joints.

### Example Mapping

Given a skin with `"joints": [2, 5, 8, 12]`:

- `bindPoses[0]` corresponds to joint node 2
- `bindPoses[1]` corresponds to joint node 5
- `bindPoses[2]` corresponds to joint node 8
- `bindPoses[3]` corresponds to joint node 12

## Relationship to glTF 2.0 Skinning

In glTF 2.0, `skins[].inverseBindMatrices` define the transformation from joint space to mesh space. However, those matrices may not capture the canonical rest pose or may be preprocessed for runtime purposes.

This extension supplements or overrides that mechanism by defining joint rest positions explicitly in model space, allowing authoring tools and runtimes to perform consistent deformation or retargeting operations.

> Note: This extension does not require that these bind pose matrices be used for skinning directly—they are semantic references to a "T-pose" or other neutral position.

### poseType Enum

| Value    | Description                                                                          |
| -------- | ------------------------------------------------------------------------------------ |
| `TPose`  | Arms extended horizontally. Common neutral rest pose for retargeting.                |
| `APose`  | Arms angled downward (~45°). Used by some character authoring tools.                 |
| `IPose`  | Arms resting at sides, parallel to the body. Natural standing pose.                  |
| `Custom` | Any pose that does not fit T-Pose, A-Pose, or I-Pose definitions.                    |

## Implementation Notes

- Each `poseType` value must be unique within the `poses` array. A skin cannot have two poses with the same `poseType`.
- The number of bind poses in each `bindPoses` array must match the number of joints in the skin's joints array.
- The bind pose matrix must be specified in model space (i.e., world-relative).
- This extension can be used in tandem with `skins[].inverseBindMatrices`, but is intended to expose clearer semantic meaning.
- Validation should ensure that each bindPoses array length equals joints array length.

## Example

```json
{
  "asset": {
    "version": "2.0"
  },
  "extensionsUsed": [
    "KHR_character",
    "KHR_character_skeleton_biped",
    "KHR_character_skeleton_bindpose"
  ],
  "skins": [
    {
      "joints": [0, 1, 2, 3],
      "extensions": {
        "KHR_character_skeleton_bindpose": {
          "poses": [
            {
              "poseType": "TPose",
              "bindPoses": [
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.0, 0, 1] },
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.5, 0, 1] }
              ]
            },
            {
              "poseType": "IPose",
              "bindPoses": [
                { "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1] },
                { "matrix": [0.707, 0, -0.707, 0, 0, 1, 0, 0, 0.707, 0, 0.707, 0, 0, 0.5, 0, 1] },
                { "matrix": [0.707, 0, -0.707, 0, 0, 1, 0, 0, 0.707, 0, 0.707, 0, 0, 1.0, 0, 1] },
                { "matrix": [0.707, 0, -0.707, 0, 0, 1, 0, 0, 0.707, 0, 0.707, 0, 0, 1.5, 0, 1] }
              ]
            }
          ]
        }
      }
    }
  ]
}
```

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
