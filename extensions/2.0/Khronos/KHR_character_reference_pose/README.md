# KHR_character_reference_pose

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / <https://github.com/fire>
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

Requires the extension(s): `KHR_character`

## Overview

The `KHR_character_reference_pose` extension defines canonical reference poses for character skeletons by tagging standard glTF animations with semantic pose type information. These poses serve as neutral references that support animation retargeting, mesh deformation validation, and consistency across toolchains.

Reference poses are stored as single-frame (or single-keyframe) animations using the standard glTF animation infrastructure, providing:

- **TRS format**: Local-space translation, rotation, and scale — the format required by retargeting systems
- **Binary storage**: Efficient accessor-based storage via existing animation buffers
- **Compression support**: Normalized quaternions and other animation compression techniques
- **Universal tooling**: 100% compatibility with existing glTF viewers and tools

## Extension Schema

This extension is applied to individual animations in the glTF file, marking them as reference poses with a semantic `poseType`.

```json
{
  "animations": [
    {
      "name": "TPose",
      "extensions": {
        "KHR_character_reference_pose": {
          "poseType": "TPose"
        }
      },
      "channels": [
        { "sampler": 0, "target": { "node": 0, "path": "rotation" } },
        { "sampler": 1, "target": { "node": 0, "path": "translation" } },
        { "sampler": 2, "target": { "node": 1, "path": "rotation" } },
        { "sampler": 3, "target": { "node": 1, "path": "translation" } }
      ],
      "samplers": [
        { "input": 0, "output": 1, "interpolation": "STEP" },
        { "input": 0, "output": 2, "interpolation": "STEP" },
        { "input": 0, "output": 3, "interpolation": "STEP" },
        { "input": 0, "output": 4, "interpolation": "STEP" }
      ]
    }
  ]
}
```

### Properties

| Property   | Type   | Required | Default  | Description |
|------------|--------|----------|----------|-------------|
| `poseType` | string | No       | `TPose`  | The semantic classification of this reference pose. |

### poseType Values

The `poseType` property supports predefined values with specific semantics, but also accepts any custom string for extensibility:

| Value    | Description |
|----------|-------------|
| `TPose`  | Arms extended horizontally. Common neutral rest pose for retargeting. |
| `APose`  | Arms angled downward (~45°). Used by some character authoring tools. |
| `IPose`  | Arms resting at sides, parallel to the body. Natural standing pose. |
| *custom* | Any string for poses that do not fit the predefined types (e.g., `"RelaxedPose"`, `"CombatStance"`). |

## Animation Structure

Reference poses use the standard glTF animation structure with specific conventions:

### Channels

Each joint that participates in the reference pose should have channels for the transforms that define its pose:

- **`rotation`**: VEC4 quaternion (required for most joints)
- **`translation`**: VEC3 position (required for root, optional for others)
- **`scale`**: VEC3 scale (optional, typically omitted when uniform)

### Samplers

- **`interpolation`**: Should be `STEP` since poses are static snapshots
- **`input`**: Time accessor with a single value (typically `0.0`)
- **`output`**: Transform data accessor

### Accessors

| Channel Path   | Accessor Type | Component Type(s) |
|----------------|---------------|-------------------|
| `translation`  | `VEC3`        | float |
| `rotation`     | `VEC4`        | float, or normalized byte/short |
| `scale`        | `VEC3`        | float |

## Relationship to glTF 2.0 Skinning

This extension defines **reference poses for retargeting**, which are distinct from the skinning **bind pose**:

| Concept | Definition | Location in glTF |
|---------|------------|------------------|
| **Bind Pose** | The pose used when skinning weights were painted | `inverse(skin.inverseBindMatrices)` |
| **Reference Pose** | A canonical pose for retargeting (T-Pose, A-Pose) | This extension |

These may or may not be the same pose. For example:

- An artist might paint skinning weights in an A-Pose
- But the animation retargeting system requires a T-Pose reference
- Both can coexist: the bind pose in `inverseBindMatrices`, the T-Pose in this extension

> **Note**: Reference poses defined by this extension are NOT used for skinning calculations. They provide semantic information for retargeting and other toolchain operations.

## Implementation Notes

- **Single keyframe**: Reference poses typically have one keyframe at `t=0`. The animation represents a static pose, not a motion.
- **Local space**: All transforms are in local space relative to the parent node, matching standard glTF animation behavior.
- **Joint coverage**: The animation should include channels for all joints relevant to the pose. Joints without channels retain their default node transforms.
- **Multiple poses**: Multiple reference poses can exist in a single asset — simply tag multiple animations with this extension using different `poseType` values.
- **Discovery**: To find reference poses, iterate through the `animations` array and check for the presence of this extension.
- **Playback**: Reference pose animations can be "played" like any animation, which will set the skeleton to the reference pose — useful for visualization and debugging.

## Example

Complete example showing a T-Pose reference for a simple 3-joint skeleton:

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_character", "KHR_character_reference_pose"],
  "scene": 0,
  "scenes": [{ "nodes": [0] }],
  "nodes": [
    { "name": "Hips", "children": [1], "translation": [0, 1, 0] },
    { "name": "Spine", "children": [2], "translation": [0, 0.3, 0] },
    { "name": "Head", "translation": [0, 0.4, 0] }
  ],
  "buffers": [
    { "byteLength": 76, "uri": "pose_data.bin" }
  ],
  "bufferViews": [
    { "buffer": 0, "byteOffset": 0, "byteLength": 4 },
    { "buffer": 0, "byteOffset": 4, "byteLength": 48 },
    { "buffer": 0, "byteOffset": 52, "byteLength": 36 }
  ],
  "accessors": [
    {
      "bufferView": 0,
      "componentType": 5126,
      "count": 1,
      "type": "SCALAR",
      "min": [0],
      "max": [0]
    },
    {
      "bufferView": 1,
      "componentType": 5126,
      "count": 3,
      "type": "VEC4"
    },
    {
      "bufferView": 2,
      "componentType": 5126,
      "count": 3,
      "type": "VEC3"
    }
  ],
  "animations": [
    {
      "name": "TPose",
      "extensions": {
        "KHR_character_reference_pose": {
          "poseType": "TPose"
        }
      },
      "channels": [
        { "sampler": 0, "target": { "node": 0, "path": "rotation" } },
        { "sampler": 0, "target": { "node": 1, "path": "rotation" } },
        { "sampler": 0, "target": { "node": 2, "path": "rotation" } },
        { "sampler": 1, "target": { "node": 0, "path": "translation" } },
        { "sampler": 1, "target": { "node": 1, "path": "translation" } },
        { "sampler": 1, "target": { "node": 2, "path": "translation" } }
      ],
      "samplers": [
        { "input": 0, "output": 1, "interpolation": "STEP" },
        { "input": 0, "output": 2, "interpolation": "STEP" }
      ]
    }
  ],
  "skins": [
    { "joints": [0, 1, 2] }
  ],
  "extensions": {
    "KHR_character": {
      "rootNode": 0
    }
  }
}
```

## Migration from KHR_character_skeleton_bindpose

If you have assets using the previous `KHR_character_skeleton_bindpose` format:

1. **Extract pose data**: Convert model-space matrices to local-space TRS
2. **Create animation**: Build a single-frame animation with the TRS values
3. **Add extension**: Tag the animation with `KHR_character_reference_pose`
4. **Remove old extension**: Delete `KHR_character_skeleton_bindpose` from skins

### Conversion Formula

For each joint `i` with global matrix `M_i` from the old format:

1. Compute parent's global matrix `M_parent`
2. Local matrix: `M_local = inverse(M_parent) * M_i`
3. Decompose `M_local` into TRS components
4. Store in animation channel outputs

## Known Implementations

(To be added as implementations become available)

## License

This extension specification is licensed under the Khronos Group Extension License.
See: <https://www.khronos.org/registry/gltf/license.html>
