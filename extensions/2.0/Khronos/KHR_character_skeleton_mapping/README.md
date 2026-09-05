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

Assets using `KHR_character_skeleton_mapping` MUST list `KHR_character_skeleton_mapping`, `KHR_character`, and the transitive `KHR_xmp_json_ld` dependency in `extensionsUsed`. They MUST contain top-level `KHR_character_skeleton_mapping` and `KHR_character` extension objects. The `KHR_xmp_json_ld` declaration does not require an XMP extension object or packet unless metadata is provided.

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
  "asset": { "version": "2.0" },
  "extensionsUsed": [
    "KHR_character",
    "KHR_character_skeleton_mapping",
    "KHR_xmp_json_ld"
  ],
  "nodes": [
    { "name": "CharacterRoot", "children": [1] },
    { "name": "Hips", "children": [2] },
    { "name": "Head" },
    { "name": "LeftFoot" },
    { "name": "RightFoot" },
    { "name": "LeftHand" },
    { "name": "RightHand" }
  ],
  "extensions": {
    "KHR_character": {
      "rootNode": 0
    },
    "KHR_character_skeleton_mapping": {
      "skeletalRigMappings": {
        "vrmHumanoid": {
          "hips": { "node": 1, "name": "Hips" },
          "head": { "node": 2, "name": "Head" },
          "leftFoot": { "node": 3, "name": "LeftFoot" },
          "rightFoot": { "node": 4, "name": "RightFoot" },
          "leftHand": { "node": 5, "name": "LeftHand" },
          "rightHand": { "node": 6, "name": "RightHand" }
        },
        "example_rig": {
          "hip_bone": { "node": 1, "name": "Hips" },
          "head_bone": { "node": 2, "name": "Head" },
          "l_foot_bone": { "node": 3, "name": "LeftFoot" },
          "r_foot_bone": { "node": 4, "name": "RightFoot" },
          "l_hand_bone": { "node": 5, "name": "LeftHand" },
          "r_hand_bone": { "node": 6, "name": "RightHand" }
        }
      }
    }
  }
}
```

In this example:

- The key is the target joint name defined by the target standard rig (e.g., `"hips"` for VRM Humanoid)
- The value is a joint association object whose required `node` is the index of the source joint's glTF node in the model's native rig — a 0-based index into the document's top-level `nodes` array (e.g., `1`)
- The optional `name` is a human-readable label. When present, it MUST exactly match the referenced node's `name` property.
- Because the value is a node index, two different target rigs that map to the same source joint reference the **same** node index (e.g., `vrmHumanoid.hips` and `example_rig.hip_bone` both resolve to node `1`)
- The system using this extension may understand what `"vrmHumanoid"` or `"example_rig"` means (i.e., the joint vocabulary and structure must be pre-declared by the consuming runtime)

### Breakdown and Lower-Level Properties

The structure of the data contained in the extension can be described as a dictionary of dictionaries:

**Target Skeleton/Rig Name** : **Joint Mapping Dictionary** (Target Joint Name : Joint Association)

Each mapping entry is simply:

| Key (Target Joint) | Value (Joint Association) | Description                                              |
| ------------------ | ------------------------- | -------------------------------------------------------- |
| string             | object                    | Direct mapping from a target joint name in the target vocabulary to a required source `node` index and optional matching `name` |

### Mapping Types

This extension supports one-to-one mappings:

- **One-to-one**: A target joint maps directly to a single source joint through a joint association object containing a `node` index (an integer `glTFid`).

This approach ensures maximum simplicity, compatibility across all engines and tools, and follows glTF's design philosophy of keeping core extensions simple and stable.

## Mapping Registry and Target Namespaces

While this extension does not mandate a central registry, developers are encouraged to:

- Document the name and structure of their standard rigs
- Reuse identifiers like `"vrmHumanoid"`, `"unityHumanoid"`, or `"metaRig"` consistently
- Provide a public schema or joint list for validation and interoperability

## Implementation Notes

- Target joint names (keys) are defined by the target rig's vocabulary.
- Each source joint association has a required `node`, a 0-based index into the glTF `nodes` array (a `glTFid`), identifying the node in the model's native rig.
- An association `name`, when present, MUST exactly and case-sensitively match the referenced node's `name` property. Resolution is always performed using `node`.
- The reference rig vocabulary may be shared across engines or projects.
- This extension does not modify skinning behavior, but informs tooling and runtime animation retargeting.
- For validation, ensure that every value is a valid, non-negative index into the top-level `nodes` array.

## Known Implementations

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader, retargeting helpers, and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and rig-switching demo.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
