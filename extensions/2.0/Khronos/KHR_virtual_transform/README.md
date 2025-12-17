# KHR_virtual_transform

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.

## Overview

The `KHR_virtual_transform` extension introduces _virtual transforms_; metadata-informed virtual 'nodes' that exist relative to a model's skeletal/node hierarchy, but are not nodes themselves and have settings whether to respect the parent node's position/rotation/scale at runtime. These virtual transforms serve as semantic attachment or control points for applications/systems to utilize; without it needing to be tied to a literal node hierarchy.

In the context of characters, these virtual transforms serve as semantic attachment or control points for systems like look-at targeting, item equipping, IK hints, and seating positions. For other types of models, they can be leveraged for UI attach points, etc.

Virtual transforms are defined via an offset transform relative to a single parent node, forming a tree structure that matches glTF's core specification. They do **not** participate in skinning and are evaluated at runtime for behavior logic and procedural animation.

This extension is inspired in part by constructs like `lookAt` in VRM and aims to unify such functionality into a generic system usable across multiple glTF-based runtimes.

## Use Cases

- **Look-at targets** (for head/eye tracking)
- **Attachment points** (e.g., a weapon or tool socket)
- **Sitting or standing targets**
- **Camera or gaze anchors**
- **UI Attach points** - UI attachments that potentially need to maintain consistent size regardless of parent scaling
- **Reference markers** - Debug or measurement points that should be scale-independent

## Schema

```json
{
  "extensions": {
    "KHR_virtual_transform": {
      "virtualTransforms": [
        {
          "name": "right_hand_attach",
          "parent": 18,
          "translation": [0.0, 0.1, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "scale": [1.0, 1.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true,
          "respectParentScale": true,
          "tags": ["weapon_socket"]
        },
        {
          "name": "lookAt_target",
          "parent": 8,
          "translation": [0.0, 0.0, 0.35],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "scale": [1.0, 1.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true,
          "respectParentScale": true,
          "tags": ["look_at"]
        },
        {
          "name": "sitting_point",
          "parent": 0,
          "translation": [0.0, 0.0, -0.2],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "scale": [1.0, 1.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true,
          "respectParentScale": true,
          "tags": ["seating"]
        },
        {
          "name": "wrist_ui",
          "parent": 19,
          "translation": [0.0, 0.25, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "scale": [1.0, 1.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": false,
          "respectParentScale": false,
          "tags": ["ui"]
        },
        {
          "name": "debug_marker",
          "parent": 5,
          "translation": [0.0, 0.0, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "scale": [0.1, 0.1, 0.1],
          "respectParentPosition": true,
          "respectParentRotation": false,
          "respectParentScale": false,
          "tags": ["debug", "marker"]
        }
      ]
    }
  }
}
```

### Properties

| Property                | Type     | Description                                                                                 |
| ----------------------- | -------- | ------------------------------------------------------------------------------------------- |
| `name`                  | string   | Semantic identifier of the virtual transform                                                    |
| `parent`                | integer  | Index of the parent node in the glTF `nodes[]` array                                       |
| `translation`           | float[3] | Local offset (X, Y, Z) relative to the parent node. Default: [0.0, 0.0, 0.0]               |
| `rotation`              | float[4] | Local orientation as quaternion (X, Y, Z, W) relative to the parent node. Default: [0.0, 0.0, 0.0, 1.0] |
| `scale`                 | float[3] | Local scale (X, Y, Z) relative to the parent node. Default: [1.0, 1.0, 1.0]                |
| `respectParentPosition` | boolean  | Whether this should respect the parent node position post-initialization. Default: true    |
| `respectParentRotation` | boolean  | Whether this should respect the parent node rotation post-initialization. Default: true    |
| `respectParentScale`    | boolean  | Whether this should respect the parent node scale post-initialization. Default: true       |
| `tags`                  | string[] | Array of strings used to denote tags that can be attributed to the virtual transform        |

## Hierarchy Structure

Virtual transforms follow glTF's standard tree hierarchy model:

- Each virtual transform has exactly one parent node (or none, if it's a root)
- This forms a **tree** structure (no cycles, no nodes with multiple parents)
- The world transform of a virtual transform is computed by recursively multiplying its local transform by its parent's world transform
- This matches the glTF core specification and how most 3D engines and DCC tools operate

## Transform Calculation

The world transform of a virtual transform is calculated as:

1. Start with the virtual transform's local transform (translation, rotation, scale)
2. If `respectParentPosition` is true, apply the parent node's world position
3. If `respectParentRotation` is true, apply the parent node's world rotation
4. If `respectParentScale` is true, apply the parent node's world scale

This provides complete control over which aspects of the parent's transform affect the virtual transform, enabling use cases such as UI elements that follow position but maintain independent size and orientation.

## Examples

### Right Hand Attachment Virtual Transform

Example - Anchor for attaching objects, which want to respect all aspects of the parent joint transform

- **Name**: `"right_hand_attach"`
- **Parent**: Right hand node (e.g., node index 18)
- **Translation**: `[0.0, 0.1, 0.0]`
- **Rotation**: `[0.0, 0.0, 0.0, 1.0]`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **respectParentScale**: `true`
- **tags**: `["weapon_socket"]`

### Look At Virtual Transform

Example - Target for runtime look-at behavior (eyes/head alignment)

- **Name**: `"lookAt_target"`
- **Parent**: Head joint (e.g., node index 8)
- **Translation**: `[0.0, 0.0, 0.35]`
- **Rotation**: `[0.0, 0.0, 0.0, 1.0]`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **respectParentScale**: `true`
- **tags**: `["look_at"]`

### Sitting Point Virtual Transform

Example - Anchor point for aligning seated positions.

- **Name**: `"sitting_point"`
- **Parent**: Pelvis joint (e.g., node index 0)
- **Translation**: `[0.0, 0.0, -0.2]`
- **Rotation**: `[0.0, 0.0, 0.0, 1.0]`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **respectParentScale**: `true`
- **tags**: `["seating"]`

### Wrist UI Virtual Transform

Example - Anchor point for wrist UI, which follows position but maintains independent rotation and size.

- **Name**: `"wrist_ui"`
- **Parent**: Wrist joint (e.g., node index 19)
- **Translation**: `[0.0, 0.25, 0.0]`
- **Rotation**: `[0.0, 0.0, 0.0, 1.0]`
- **respectParentPosition**: `true`
- **respectParentRotation**: `false`
- **respectParentScale**: `false`
- **tags**: `["ui"]`

### Debug Marker Virtual Transform

Example - A debug marker that follows the parent's position but maintains independent orientation and consistent size.

- **Name**: `"debug_marker"`
- **Parent**: Spine joint (e.g., node index 5)
- **Translation**: `[0.0, 0.0, 0.0]`
- **Rotation**: `[0.0, 0.0, 0.0, 1.0]`
- **Scale**: `[0.1, 0.1, 0.1]` (small marker)
- **respectParentPosition**: `true`
- **respectParentRotation**: `false`
- **respectParentScale**: `false` (maintains consistent size)
- **tags**: `["debug", "marker"]`

## Implementation Notes

- Virtual transforms should exist only within this extension and **must not overlap with joints used in skinning**.
- Their transformation is computed at runtime by applying the offset to the parent node's world transform.
- Tools and runtimes may expose these as attachable sockets or semantic retargeting anchors.
- The single-parent hierarchy ensures compatibility with all glTF-compliant tools and engines.
- For advanced constraint-based relationships (multiple influences), consider using a dedicated constraint system rather than multiple parents in the scene graph.
- The three `respectParent*` flags provide fine-grained control over transform inheritance, enabling use cases from fully-inherited transforms to completely independent positioning.

## License

This extension is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
