# KHR_character_virtual_transforms

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

## Overview

The `KHR_character_virtual_transforms` extension introduces _virtual transforms_—custom transform nodes that exist relative to the character’s skeletal/node hierarchy but are **not part of the skinned joint structure**. These virtual transforms serve as semantic attachment or control points for systems like look-at targeting, item equipping, IK hints, and seating positions.

Virtual transforms are defined via an offset transform relative to the parent nodes, and do **not** participate in skinning. If there are multiple parent nodes, the expected behavior is to have it be based on a translation directly between those nodes position-wise. They are evaluated at runtime for behavior logic and procedural animation.

This extension is inspired in part by constructs like `lookAt` in VRM and aims to unify such functionality into a generic system usable across multiple glTF-based runtimes.

**TODO**: Need to work on proposed logic for interpolation when multiple parent nodes; this likely needs to be some sort of field with established lerp logic types. We could make the stance that it's a pure 50/50 split; but I imagine that might cause some eyebrows to be raised. 

## Use Cases

- **Look-at targets** (for head/eye tracking)
- **Attachment points** (e.g., a weapon or tool socket)
- **Sitting or standing targets**
- **Camera or gaze anchors**
- **UI Attach points** - UI attachments that potentially need to not respect the rotation of the parent joints (e.g. a wrist UI in an AR/VR experience that needs to always be above the ) 

## Schema

```json
{
  "extensions": {
    "KHR_character_virtual_transforms": {
      "virtualTransforms": [
        {
          "name": "arm_socket",
          "parentNodes": [18, 19],
          "translation": [0.0, 0.1, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true,
          "tags": []
        },
        {
          "name": "lookAt_target",
          "parentNodes": [8],
          "translation": [0.0, 0.0, 0.35],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true,
          "tags": []
        },
        {
          "name": "sitting_point",
          "parentNodes": [0],
          "translation": [0.0, 0.0, -0.2],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true,
          "tags": []
        },
        {
          "name": "wrist_ui",
          "parentNodes": [19],
          "translation": [0.0, 0.25, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": false,
          "tags": []
        }
      ]
    }
  }
}
```

### Properties

| Property      | Type     | Description                                                               |
| ------------- | -------- | ------------------------------------------------------------------------- |
| `name`        | string   | Semantic identifier of the virtual joint                                  |
| `parentNodes` | Array  | Array of indices for nodes in the glTF `nodes[]` array
| `translation` | float[3] | Initialized local offset (X, Y, Z) relative to the parent joint                        |
| `rotation`    | float[4] | Local orientation as quaternion (X, Y, Z, W) relative to the parent joint |
| `respectParentPosition`    | bool | bool to determine whether this should respect the parent joint position post-initialization |
| `respectParentRotation`    | bool | bool to determine whether this should respect the parent joint rotation post-initialization |
| `tags`    | Array | Array of strings used to denote tags that can be attributed to the virtual transform |

## Examples

### Arm Attachment Virtual Joint
Example - Anchor for attaching objects, which want to respect the positions/rotation of the parent joints
- **Name**: `"arm_socket"`
- **Parent**: Between elbow and wrist (e.g., joint index 18)
- **Translation**: `(0.0, 0.1, 0.0)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **tags**: Array of strings used to denote semantic tags.

### Look At Virtual Joint
Example - Target for runtime look-at behavior (eyes/head alignment)
- **Name**: `"lookAt_target"`
- **Parent**: `"head"` joint (e.g., joint index 8)
- **Translation**: `(0.0, 0.0, 0.35)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **tags**: Array of strings used to denote semantic tags.

### Sitting Point Virtual Joint
Example - Anchor point for aligning seated positions.
- **Name**: `"sitting_point"`
- **Parent**: `"pelvis"` joint (e.g., joint index 0)
- **Translation**: `(0.0, 0.0, -0.2)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`


### Wrist UI Virtual Joint
Example - Anchor point for wrist UI, which may wish to respect the parent joint's position, but not the rotation. 
- **Name**: `"wrist_ui_socket"`
- **Parent**: `"wrist"` joint (e.g., joint index 19)
- **Offset**: `(0.0, .25, 0.0)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `false`
- **tags**: Array of strings used to denote semantic tags.

## Implementation Notes

- Virtual joints should exist only within this extension and **must not overlap with joints used in skinning**.
- Their transformation is computed at runtime by applying the offset to the parent joint’s world transform.
- Tools and runtimes may expose these as attachable sockets or semantic retargeting anchors.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
