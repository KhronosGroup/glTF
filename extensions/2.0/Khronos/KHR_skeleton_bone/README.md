<!--
Copyright 2025-2026 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# KHR_skeleton_bone

## Contributors

- Aaron Franke, Godot Engine

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines length, connectivity, and strict requirements for glTF nodes used as bones in a skeleton.

The `KHR_skeleton_bone` extension may be added to joint and non-joint nodes to explicitly define its length and/or whether it is connected to its parent. Alternatively, an empty `KHR_skeleton_bone` extension can be added to joint and non-joint nodes to explicitly mark them as bones in the skeleton hierarchy even when the bone does not have any additional properties defined.

When a node has the `KHR_skeleton_bone` extension defined, it is a bone in a skeleton hierarchy, even if it is an empty object, and even if it is not used as a glTF skin joint. When a node is used as a glTF skin joint, it is also a bone in the skeleton hierarchy, even if it does not have the `KHR_skeleton_bone` extension defined on it. Therefore, the requirements for bones imposed by this extension apply even to nodes without the `KHR_skeleton_bone` extension on them, if they are used as glTF skin joints.

When a glTF file has `KHR_skeleton_bone` included in the `"extensionsUsed"` array, all bones in the glTF file **MUST** follow the requirements defined by this extension, including nodes used as glTF skin joints which do not have the `KHR_skeleton_bone` extension defined on them.

All bones **MUST** form a contiguous hierarchy descendant from the skeleton root node, meaning that each and every bone **MUST** have a contiguous chain of bone parenting leading up to a skeleton root node; the parent node of a bone **MUST** either be another bone or the skeleton root node itself. The `"skeleton"` property of all glTF skins is required when `KHR_skeleton_bone` is used in the file.

All bone nodes **MUST NOT** have a mesh, camera, or light attached to the same node, and **SHOULD NOT** have similar extension-defined "non-bone components" attached to the same node, such that the glTF node serves only as a bone, and any non-bone components are be attached to child glTF nodes. This is a structural choice which enables the creation of an optimized dedicated bone hierarchy, important for applications where such a concept is separate from the standard node hierarchy, and other non-bone objects can then be attached as child nodes of the bone nodes.

All bones **MUST** belong to a single skeleton, which **SHOULD** correspond to one glTF skin, but **MAY** correspond to multiple glTF skins. If a bone is used as a joint in multiple skins, those skins **MUST** all have the same skeleton node. Multiple skins **MUST NOT** share any joints without also sharing the skeleton. A skeleton's scope of bone hierarchy ownership is defined as the overlap between all joints used by all skins that reference the same skeleton root node, and all `KHR_skeleton_bone` nodes that form a contiguous hierarchy descendant from the skeleton root node. This ensures that applications can always build a single skeleton hierarchy from a given skeleton root node, even if meshes use different skins with different subsets of nodes used as joints.

All skinned mesh nodes **MUST** be a direct child of the skeleton root node. Meaning, the skinned mesh node's `skin` property **MUST** refer to a glTF skin whose `"skeleton"` property refers to the same node as the skinned mesh node's parent.

## glTF Schema Updates

The `"KHR_skeleton_bone"` extension name may be added to the `"extensionsUsed"` array of the glTF file. Its existence alone has semantic meaning, indicating that all skins, skinned meshes, and joints in the glTF file **MUST** follow the requirements defined by this extension. The `"KHR_skeleton_bone"` extension name **SHOULD NOT** be added to the `"extensionsRequired"` array.

The `KHR_skeleton_bone` extension may be added to nodes in the glTF file's `"nodes"` array to explicitly mark it as a bone in a skeleton. Its existence alone has semantic meaning, and it may have these properties defined:

| Property      | Type      | Description                                                             | Default            |
| ------------- | --------- | ----------------------------------------------------------------------- | ------------------ |
| **connected** | `boolean` | Whether the bone is connected to its parent bone.                       | `false`            |
| **length**    | `number`  | The length of the bone in meters in the local +Y direction, if defined. | No defined length. |

### Connected

The `"connected"` property is a boolean that defines whether the bone is connected to its parent bone. This property is optional, and if not defined, the bone is not considered connected to its parent bone.

A bone that is connected to its parent bone has its head positioned at the tail of its parent bone. This means that the child bone is not allowed to be translated away from the end of the parent bone, and **MUST** have its local origin's Y component equal to the length of the parent bone, with the X and Z components equal to zero. This is usually done through the glTF node's `"translation"` property, or alternatively it may be done through the node's `"matrix"` property.

During runtime, a connected bone **SHOULD NOT** be allowed to be moved away from the end of its parent bone, such that its local translation remains the same as the location of the parent bone's tail. As such, the local translation of a connected bone **SHOULD NOT** be altered by animations, interactivity graphs, or anything else, unless the `"connected"` property is first set to false.

Applications **MAY** enforce this restriction in any way they see fit, including ignoring the `"connected"` property entirely. If an application wants to force a bone to move in global space while keeping it connected, it may perform an inverse kinematics algorithm, it may stretch the parent bone to reach the child bone, it may choose to "pull along" the parent bone to bring the tail into alignment with the head of the connected child bone, or it may use any other technique.

The `"connected"` property has no semantic meaning for bones that do not have another bone as a parent, meaning bones whose parent is the skeleton root node.

### Length

The `"length"` property is a number that defines the length of the bone in meters. This property is optional, and if not defined, the bone does not have a defined length.

If a bone has the `"length"` property defined, it **MUST NOT** be negative. The length of the bone is used to determine how far the bone extends along the local Y axis of the bone node. The head of the bone is defined as the node's position, and the tail of the bone is offset from this position by `"length"` in the local +Y direction. The final perceived bone length includes scale, so if the bone node has a `"length"` property set to 0.1 and has a global scale of 3.0 along the local Y axis, the effective global bone length is 0.3 meters.

### glTF Object Model

The following JSON pointers are defined representing mutable properties defined by this extension, for use with the glTF Object Model including extensions such as `KHR_animation_pointer` and `KHR_interactivity`:

| JSON Pointer                                       | Object Model Type |
| -------------------------------------------------- | ----------------- |
| `/nodes/{}/extensions/KHR_skeleton_bone/connected` | `boolean`         |
| `/nodes/{}/extensions/KHR_skeleton_bone/length`    | `float`           |

### JSON Schema

See [node.KHR_skeleton_bone.schema.json](schema/node.KHR_skeleton_bone.schema.json) for the bone schema.

## Known Implementations

- None

## Resources

- GitHub discussion on "Stricter Skinning Requirements": https://github.com/KhronosGroup/glTF/issues/1665
- Previously proposed `KHR_skin_strict` extension: https://github.com/KhronosGroup/glTF/pull/1747
- Blender bone length: https://docs.blender.org/api/current/bpy.types.Bone.html#bpy.types.Bone.length
- VRChat PhysBones: https://creators.vrchat.com/common-components/physbones/
- VRMC_springBone: https://github.com/vrm-c/vrm-specification/tree/master/specification/VRMC_springBone-1.0
