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

The `KHR_character_skeleton_mapping` extension provides a mechanism to map a skeleton rig to a reference rig, enabling retargeting and compatibility across different skeleton topologies. This extension is particularly useful for normalizing diverse rig structures across platforms and authoring tools. It provides one-to-one mapping for maximum simplicity and compatibility.

## Mapping to Known Standard Rigs

In many real-world scenarios, developers must remap an character's native joint structure to a **known, standardized rig**—such as a runtime's internal character model or a predefined specification like VRM's Humanoid rig.

This extension supports such cases by allowing one-to-one mappings between a model's joints and those of a **target standard rig**.

These standard rigs are typically defined by the consuming platform, runtime, or service provider. Each standard rig:

- Defines a fixed joint name vocabulary and hierarchy.
- Is assumed to be known at runtime and used for animation playback, retargeting, or IK purposes.

### Extension Schema/Example: Mapping to VRM Humanoid

The [VRM 1.0 Humanoid specification](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/humanoid.md) defines a standardized set of joints used across VRM-compatible platforms.

Here's an example mapping from a custom rig into VRM Humanoid:

```json
{
  "extensionsUsed": ["KHR_character_skeleton_mapping"],
  "extensions": {
    "KHR_character_skeleton_mapping": {
      "skeletalRigMappings": {
        "vrmHumanoid": {
          "myRig_hips": "hips",
          "myRig_head": "head",
          "myRig_leftFoot": "leftFoot",
          "myRig_rightFoot": "rightFoot",
          "myRig_leftHand": "leftHand",
          "myRig_rightHand": "rightHand"
        },
        "example_rig": {
          "myRig_hips": "hip_bone",
          "myRig_head": "head_bone",
          "myRig_leftFoot": "l_foot_bone",
          "myRig_rightFoot": "r_foot_bone",
          "myRig_leftHand": "l_hand_bone",
          "myRig_rightHand": "r_hand_bone"
        }
      }
    }
  }
}
```

In this example:

- The key is the source joint name from the model's native rig (e.g., `"myRig_hips"`)
- The value is the target joint name defined by the target standard rig (e.g., `"hips"` for VRM Humanoid)
- The system using this extension may understand what `"vrmHumanoid"` or `"example_rig"` means (i.e., the joint vocabulary and structure must be pre-declared by the consuming runtime)

### Breakdown and Lower-Level Properties

The structure of the data contained in the extension can be described as a dictionary of dictionaries:

**Target Skeleton/Rig Name** : **Joint Mapping Dictionary** (Source Joint Name : Target Joint Name)

Each mapping entry is simply:

| Key (Source Joint) | Value (Target Joint) | Description                                              |
| ------------------ | -------------------- | -------------------------------------------------------- |
| string             | string               | Direct mapping from source joint name to target joint name in the target vocabulary |

### Mapping Types

This extension supports one-to-one mappings:

- **One-to-one**: A source joint maps directly to a single target joint via a simple string value.

This approach ensures maximum simplicity, compatibility across all engines and tools, and follows glTF's design philosophy of keeping core extensions simple and stable.

## Mapping Registry and Target Namespaces

While this extension does not mandate a central registry, developers are encouraged to:

- Document the name and structure of their standard rigs
- Reuse identifiers like `"vrmHumanoid"`, `"unityHumanoid"`, or `"metaRig"` consistently
- Provide a public schema or joint list for validation and interoperability

## Implementation Notes

- Source joint names (keys) are assumed to refer to nodes in the glTF `nodes` array.
- Target joint names (values) are defined by the target rig's vocabulary.
- The reference rig vocabulary may be shared across engines or projects.
- This extension does not modify skinning behavior, but informs tooling and runtime animation retargeting.
- For validation, ensure that all source joint names correspond to valid glTF nodes.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
