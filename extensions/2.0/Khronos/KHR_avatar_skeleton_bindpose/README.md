# KHR_avatar_skeleton_bindpose

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
Dependent on: `KHR_avatar`

## Overview

**THIS EXTENSION IS VERY MUCH WORK IN PROGRESS**

The `KHR_avatar_skeleton_bindpose` extension defines a canonical bind pose for the avatar's skeleton. This pose serves as a neutral reference that supports animation retargeting, mesh deformation validation, and consistency across toolchains. It is particularly useful when the glTF `skins[].inverseBindMatrices` are insufficient or absent for expressing the true skeletal rest position.

## Extension Schema


```json
{
  "extensions": {
    "KHR_avatar_skeleton_bindpose": {
      "poseType": "TPose",
      "skeleton": 0,
      "jointBindPoses": [
        {
          "joint": 0,
          "matrix": [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
          ]
        }
      ]
    }
  }
}
```


### Properties

| Property          | Type      | Description                                                                 |
|------------------|-----------|-----------------------------------------------------------------------------|

| `poseType`       | string    | Enum: `TPose`, `APose`, `Custom`. Declares the avatar’s general pose classification. |
| `skeleton`       | integer   | Index into glTF `skins[]`; defines the joint set this bind pose applies to.        |

| `jointBindPoses` | array     | List of bind pose matrices, each associated with a joint node index.        |
| `joint`          | integer   | Index of the joint node in the glTF `nodes` array.                          |
| `matrix`         | float[16] | Column-major 4x4 matrix defining the joint’s bind pose in model space.      |

## Relationship to glTF 2.0 Skinning

In glTF 2.0, `skins[].inverseBindMatrices` define the transformation from joint space to mesh space. However, those matrices may not capture the canonical rest pose or may be preprocessed for runtime purposes.

This extension supplements or overrides that mechanism by defining joint rest positions explicitly in model space, allowing authoring tools and runtimes to perform consistent deformation or retargeting operations.

> Note: This extension does not require that these bind pose matrices be used for skinning directly—they are semantic references to a "T-pose" or other neutral position.


### poseType Enum

| Value     | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `TPose`   | Arms extended horizontally. Common neutral rest pose for retargeting.       |
| `APose`   | Arms angled downward. Used by some avatar authoring tools.                 |
| `Custom`  | Any pose that does not fit T-Pose or A-Pose definitions.                   |


## Implementation Notes

- A joint listed in `jointBindPoses` must exist in the `nodes` array.
- The bind pose matrix must be specified in model space (i.e., world-relative).
- This extension can be used in tandem with `skins[].inverseBindMatrices`, but is intended to expose clearer semantic meaning.

## Example

```json
{
  "asset": {
    "version": "2.0"
  },
  "extensionsUsed": [
    "KHR_avatar",
    "KHR_avatar_skeleton_biped",
    "KHR_avatar_skeleton_bindpose"
  ],
  "extensions": {
    "KHR_avatar_skeleton_bindpose": {
      "jointBindPoses": [
        {
          "joint": 0,
          "matrix": [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
          ]
        }
      ]
    }
  }
}
```

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
