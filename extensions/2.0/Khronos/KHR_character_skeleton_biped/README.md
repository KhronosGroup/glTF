# KHR_character_skeleton_biped

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
Depends on: `KHR_character`

## Overview

**THIS EXTENSION IS VERY MUCH WORK IN PROGRESS**

The `KHR_character_skeleton_biped` extension enables bipedal characters. Like with other character extensions, its presence represents the data contract that the model itself is a bipedal character model. This extension enables clarifying what joints are for retargeting and compatibility across tools and platforms by specifying a canonical set of joints and their hierarchical structure.

This extension does not modify the glTF skinning or animation system, but instead annotates the character’s node hierarchy to indicate which nodes represent canonical joints in a bipedal skeleton.

## Extension Schema Example

```json
{
  "extensions": {
    "KHR_character_skeleton_biped": {
      "joints": {
        "hips": 0,
        "spine": 1,
        "chest": 2,
        "neck": 3,
        "head": 4,
        "leftUpperLeg": 5,
        "leftLowerLeg": 6,
        "leftFoot": 7,
        "rightUpperLeg": 8,
        "rightLowerLeg": 9,
        "rightFoot": 10,
        "leftUpperArm": 11,
        "leftLowerArm": 12,
        "leftHand": 13,
        "rightUpperArm": 14,
        "rightLowerArm": 15,
        "rightHand": 16
      }
    }
  }
}
```

### Properties

| Property | Type   | Description                                                      |
| -------- | ------ | ---------------------------------------------------------------- |
| `joints` | object | Mapping from biped joint names to node indices in the glTF file. |

Each joint name corresponds to a node present in the glTF node hierarchy. The hierarchy as it exists then informs the skeletal hierarchy for parsing purposes.

## Example

```json
{
  "asset": {
    "version": "2.0"
  },
  "nodes": [
    { "name": "Hips" },
    { "name": "Spine", "parent": 0 },
    { "name": "Chest", "parent": 1 },
    { "name": "Neck", "parent": 2 },
    { "name": "Head", "parent": 3 },
    ...
  ],
  "extensionsUsed": [
    "KHR_character",
    "KHR_character_skeleton_biped"
  ],
  "extensions": {
    "KHR_character_skeleton_biped": {
      "joints": {
        "hips": 0,
        "spine": 1,
        "chest": 2,
        "neck": 3,
        "head": 4
      }
    }
  }
}
```

## Relationship to glTF 2.0 Skinning

The `KHR_character_skeleton_biped` extension builds on top of the core glTF 2.0 skinning system. It does **not** introduce new skinning mechanics, but instead annotates which nodes in the existing `skins[j].joints[]` structure correspond to meaningful joints in a developer-defined skeletal rig.

The joint indices specified in the `joints` dictionary refer directly to elements within the glTF `nodes` array, and should match those used in a valid `skin` structure:

- The character mesh should be associated with a `skin`, per [glTF 2.0 Skinning](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#skins).
- The joints listed in this extension must be a subset of the joints defined in `skins[j].joints`.

This enables runtime engines to extract semantic meaning from joint hierarchies already used for mesh deformation, supporting animation retargeting and character interoperability.

## Skeletal Mapping Support

Rather than define mapping logic directly in this extension, developers should use the [`KHR_character_skeleton_mapping`](./KHR_character_skeleton_mapping_README.md) extension in conjunction with this one for any one-to-one or one-to-many joint remapping between different rig topologies.

That extension supports weighted joint translation and enables cleaner separation between semantic labeling (this extension) and rig interoperability (mapping).

## Skeleton Standardization (In Progress)

This extension currently avoids defining any required or default biped joint hierarchy.

> **Why?**  
> Standardizing on a single joint hierarchy would inadvertently lock the majority of developers into a single rig structure, limiting flexibility for studios and tools that use different conventions.

Instead, this extension enables **declarative labeling** of a bipedal skeleton using your rig's native hierarchy, and encourages semantic interoperability through mapping via the `KHR_character_skeleton_mapping` extension.

As the ecosystem matures, guidance or profiles may emerge for common structures—but this extension deliberately avoids enforcing that at this time.

## Implementation Notes

- This extension is non-destructive and overlays semantic information atop the existing glTF node hierarchy.
- Tools can use this extension to retarget animation between characters, align expressions, or validate rigging consistency.
- The specified joint indices must form a valid hierarchy rooted at a common ancestor (usually `hips`).

## Known Implementations

None, TBD

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
