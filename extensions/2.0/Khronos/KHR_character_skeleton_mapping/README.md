# KHR_character_skeleton_mapping

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
          "hips": "myRig_hips",
          "head": "myRig_head",
          "leftFoot": "myRig_leftFoot",
          "rightFoot": "myRig_rightFoot",
          "leftHand": "myRig_leftHand",
          "rightHand": "myRig_rightHand"
        },
        "example_rig": {
          "hip_bone": "myRig_hips",
          "head_bone": "myRig_head",
          "l_foot_bone": "myRig_leftFoot",
          "r_foot_bone": "myRig_rightFoot",
          "l_hand_bone": "myRig_leftHand",
          "r_hand_bone": "myRig_rightHand"
        }
      }
    }
  }
}
```

In this example:

- The key is the target joint name defined by the target standard rig (e.g., `"hips"` for VRM Humanoid)
- The value is the source joint name from the model's native rig (e.g., `"myRig_hips"`)
- The system using this extension may understand what `"vrmHumanoid"` or `"example_rig"` means (i.e., the joint vocabulary and structure must be pre-declared by the consuming runtime)

### Breakdown and Lower-Level Properties

The structure of the data contained in the extension can be described as a dictionary of dictionaries:

**Target Skeleton/Rig Name** : **Joint Mapping Dictionary** (Target Joint Name : Source Joint Name)

Each mapping entry is simply:

| Key (Target Joint) | Value (Source Joint) | Description                                              |
| ------------------ | -------------------- | -------------------------------------------------------- |
| string             | string               | Direct mapping from target joint name in the target vocabulary to source joint name in the model's native rig |

### Mapping Types

This extension supports one-to-one mappings:

- **One-to-one**: A target joint maps directly to a single source joint via a simple string value.

This approach ensures maximum simplicity, compatibility across all engines and tools, and follows glTF's design philosophy of keeping core extensions simple and stable.

## Mapping Registry and Target Namespaces

While this extension does not mandate a central registry, developers are encouraged to:

- Document the name and structure of their standard rigs
- Reuse identifiers like `"vrmHumanoid"`, `"unityHumanoid"`, or `"metaRig"` consistently
- Provide a public schema or joint list for validation and interoperability

## Implementation Notes

- Target joint names (keys) are defined by the target rig's vocabulary.
- Source joint names (values) are assumed to refer to nodes in the glTF `nodes` array.
- The reference rig vocabulary may be shared across engines or projects.
- This extension does not modify skinning behavior, but informs tooling and runtime animation retargeting.
- For validation, ensure that all source joint names (values) correspond to valid glTF nodes.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
