# KHR_avatar_skeleton_mapping

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- Ernest Lee, Individual Contributor
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Individual Contributor

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.    
Dependent on: `KHR_avatar`
Typically used in conjunction with: `KHR_avatar_skeleton_biped`

## Overview

The `KHR_avatar_skeleton_mapping` extension provides a mechanism to map a skeleton rig to a reference rig, enabling retargeting and compatibility across different skeleton topologies. This extension is particularly useful for normalizing diverse rig structures across platforms and authoring tools. It provides one-to-one and one-to-many mapping; with the expectation that distributed weights add up to 1.0. 

## Extension Schema

```json
{
  "extensions": {
    "KHR_avatar_skeleton_mapping": {
      "mappings": {
        "Spine1": [
          { "target": "spine", "weight": 0.5 },
          { "target": "chest", "weight": 0.5 }
        ],
        "JawJoint": [
          { "target": "jaw", "weight": 1.0 }
        ]
      }
    }
  }
}
```

### Properties

| Property     | Type    | Description                                                                 |
|--------------|---------|-----------------------------------------------------------------------------|
| `mappings`   | object  | Dictionary mapping source joint names to reference rig joint names.         |
| `target`     | string  | Name of the reference joint.                                                |
| `weight`     | number  | Influence of this target in the mapping. Must sum to 1.0 per source joint.  |


### Mapping Types

This extension supports both one-to-one and one-to-many mappings:

- **One-to-one**: A source joint maps directly to a single reference joint with a weight of 1.0.
- **One-to-many**: A source joint influences multiple reference joints, each with a fractional weight (all weights must sum to 1.0).

This provides flexibility for approximating intermediate or compound joints across rig topologies.


## Mapping to Known Standard Rigs

In many real-world scenarios, developers must remap an avatar's native joint structure to a **known, standardized rig**—such as a runtime's internal avatar model or a predefined specification like VRM's Humanoid rig.

This extension supports such cases by allowing one-to-one or one-to-many weighted mappings between a model’s joints and those of a **target standard rig**.

These standard rigs are typically defined by the consuming platform, runtime, or service provider. Each standard rig:
- Defines a fixed joint name vocabulary and hierarchy.
- Is assumed to be known at runtime and used for animation playback, retargeting, IK, or expression control.

### Example: Mapping to VRM Humanoid

The [VRM 1.0 Humanoid specification](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/humanoid.md) defines a standardized set of joints used across VRM-compatible platforms.

Here’s an example mapping from a custom rig into VRM Humanoid:

```json
{
  "extensions": {
    "KHR_avatar_skeleton_mapping": {
      "source": "localRig",
      "target": "vrmHumanoid",
      "mappings": [
        {
          "source": "myRig_hips",
          "targets": [
            { "joint": "hips", "weight": 1.0 }
          ]
        },
        {
          "source": "myRig_head",
          "targets": [
            { "joint": "head", "weight": 1.0 }
          ]
        },
        {
          "source": "myRig_leftFoot",
          "targets": [
            { "joint": "leftFoot", "weight": 0.8 },
            { "joint": "leftToeBase", "weight": 0.2 }
          ]
        }
      ]
    }
  }
}
```

In this example:
- `"source"` is the custom rig's joint name.
- `"joint"` in the `"targets"` is a joint name defined by the target standard rig (here, VRM Humanoid).
- The system using this extension must understand what `"vrmHumanoid"` means (i.e., the joint vocabulary and structure must be pre-declared by the consuming runtime).

## Mapping Registry and Target Namespaces

While this extension does not mandate a central registry, developers are encouraged to:
- Document the name and structure of their standard rigs
- Reuse identifiers like `"vrmHumanoid"`, `"unityHumanoid"`, or `"metaRig"` consistently
- Provide a public schema or joint list for validation and interoperability


## Implementation Notes

- Joint names are assumed to refer to nodes in the `nodes` array.
- The reference rig vocabulary may be shared across engines or projects.
- This extension does not modify skinning behavior, but informs tooling and runtime animation retargeting.

## Example

```json
{
  "extensionsUsed": [
    "KHR_avatar_skeleton_mapping"
  ],
  "extensions": {
    "KHR_avatar_skeleton_mapping": {
      "mappings": {
        "Spine1": [
          { "target": "spine", "weight": 0.5 },
          { "target": "chest", "weight": 0.5 }
        ]
      }
    }
  }
}
```

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
