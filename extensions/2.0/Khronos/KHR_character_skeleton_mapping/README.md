# KHR_character_skeleton_mapping

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
Typically used in conjunction with: `KHR_character_skeleton_biped`

## Overview

The `KHR_character_skeleton_mapping` extension provides a mechanism to map a skeleton rig to a reference rig, enabling retargeting and compatibility across different skeleton topologies. This extension is particularly useful for normalizing diverse rig structures across platforms and authoring tools. It provides one-to-one and one-to-many mapping; with the expectation that distributed weights add up to 1.0.

## Mapping to Known Standard Rigs

In many real-world scenarios, developers must remap an character's native joint structure to a **known, standardized rig**—such as a runtime's internal character model or a predefined specification like VRM's Humanoid rig.

This extension supports such cases by allowing one-to-one or one-to-many weighted mappings between a model’s joints and those of a **target standard rig**.

These standard rigs are typically defined by the consuming platform, runtime, or service provider. Each standard rig:

- Defines a fixed joint name vocabulary and hierarchy.
- Is assumed to be known at runtime and used for animation playback, retargeting, or IK purposes.

### Extension Schema/Example: Mapping to VRM Humanoid

The [VRM 1.0 Humanoid specification](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/humanoid.md) defines a standardized set of joints used across VRM-compatible platforms.

Here’s an example mapping from a custom rig into VRM Humanoid:

```json
{
  "extensionsUsed": ["KHR_character_skeleton_mapping"],
  "extensions": {
    "KHR_character_skeleton_mapping": {
      "skeletalRigMappings": {
        "vrmHumanoid": {
          "myRig_hips": [{ "target": "hips", "weight": 1.0 }],
          "myRig_head": [{ "target": "head", "weight": 1.0 }],
          "myRig_leftFoot": [
            { "target": "leftFoot", "weight": 0.8 },
            { "target": "leftToeBase", "weight": 0.2 }
          ]
        },
        "example_rig": {
          "myRig_hips": [{ "target": "hip_bone", "weight": 1.0 }],
          "myRig_head": [{ "target": "head_bone", "weight": 1.0 }],
          "myRig_leftFoot": [
            { "target": "l_foot_bone", "weight": 0.8 },
            { "target": "l_toe_bone", "weight": 0.2 }
          ]
        }
      }
    }
  }
}
```

In this example:

- `"target"` is a joint name defined by the target standard rig (here, VRM Humanoid, or a hypotheical example_rig system).
- The system using this extension may understand what `"vrmHumanoid"` or `"example_rig"` means (i.e., the joint vocabulary and structure must be pre-declared by the consuming runtime); and if so can leverage the provided joint mappings

### Breakdown and Lower-Level Properties

The structure of the data contained in the extension can be described as a dictionary of dictionaries:

Target Skeleton/Rig Name : Joint Mapping Dictionary (Joint Name : List/Array of objects containing the below properties)

| Property | Type   | Description                                              |
| -------- | ------ | -------------------------------------------------------- |
| `target` | string | Name of the joint in the target vocabulary.              |
| `weight` | number | Influence of this target. Must sum to 1.0 per joint key. |

### Mapping Types

This extension supports both one-to-one and one-to-many mappings (as well as multiple instances of those mapping sets):

- **One-to-one**: A source joint maps directly to a single reference joint with a weight of 1.0.
- **One-to-many**: A source joint influences multiple reference joints, each with a fractional weight (all weights must sum to 1.0).

This provides flexibility for approximating intermediate or compound joints across rig topologies.

## Mapping Registry and Target Namespaces

While this extension does not mandate a central registry, developers are encouraged to:

- Document the name and structure of their standard rigs
- Reuse identifiers like `"vrmHumanoid"`, `"unityHumanoid"`, or `"metaRig"` consistently
- Provide a public schema or joint list for validation and interoperability

## Implementation Notes

- Joint names are assumed to refer to nodes in the `nodes` array.
- The reference rig vocabulary may be shared across engines or projects.
- This extension does not modify skinning behavior, but informs tooling and runtime animation retargeting.

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
