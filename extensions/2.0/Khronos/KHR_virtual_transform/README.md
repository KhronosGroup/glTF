<<<<<<<< Updated upstream:extensions/2.0/Khronos/KHR_character_virtual_joints/README.md
# KHR_character_virtual_joints
========
# KHR_virtual_transforms
>>>>>>>> Stashed changes:extensions/2.0/Khronos/KHR_virtual_transforms/README.md

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

<<<<<<<< Updated upstream:extensions/2.0/Khronos/KHR_character_virtual_joints/README.md
The `KHR_character_virtual_joints` extension introduces _virtual joints_—custom transform nodes that exist relative to the character’s skeletal hierarchy but are **not part of the skinned joint structure**. These virtual transforms serve as semantic attachment or control points for systems like look-at targeting, item equipping, IK hints, and seating positions.
========
The `KHR_virtual_transforms` extension introduces _virtual transforms_; metadata-informed virtual 'nodes' that exist relative to the a model's skeletal/node hierarchy, but are not nodes themselves and have settings whether to respect the parent nodes position/rotation at runtime. These virtual transforms serve as semantic attachment or control points for applications/systems to utilize; without it needing to be tied to a literal node hierarchy.

In the context of characters, these virtual transforms serve as semantic attachment or control points for systems like look-at targeting, item equipping, IK hints, and seating positions. For other types of models, they can be leveraged for UI attach points, etc. 
>>>>>>>> Stashed changes:extensions/2.0/Khronos/KHR_virtual_transforms/README.md

Virtual joints are defined via an offset transform relative to a single parent joint, and do **not** participate in skinning. They are evaluated at runtime for behavior logic and procedural animation.

This extension is inspired in part by constructs like `lookAt` in VRM and aims to unify such functionality into a generic system usable across multiple glTF-based runtimes.

<<<<<<<< Updated upstream:extensions/2.0/Khronos/KHR_character_virtual_joints/README.md
========
**TODO**: Need to work on proposed logic for interpolation when multiple parent nodes; this likely needs to be some sort of field with established lerp logic types. We could make the stance that it's a pure 50/50 split; but I imagine that might cause some eyebrows to be raised. In general this likely needs a few revs, even past moving it out of the character set of extensions.

>>>>>>>> Stashed changes:extensions/2.0/Khronos/KHR_virtual_transforms/README.md
## Use Cases

- **Look-at targets** (for head/eye tracking)
- **Attachment points** (e.g., a weapon or tool socket)
- **Sitting or standing targets**
- **Hint points** for IK solvers
- **Camera or gaze anchors**
- **UI Attach points** - UI attachments that potentially need to not respect the rotation of the parent joints (e.g. a wrist UI in an AR/VR experience that needs to always be above the ) 

## Schema

```json
{
  "extensions": {
<<<<<<<< Updated upstream:extensions/2.0/Khronos/KHR_character_virtual_joints/README.md
    "KHR_character_virtual_joints": {
      "virtualJoints": [
========
    "KHR_virtual_transforms": {
      "virtualTransforms": [
>>>>>>>> Stashed changes:extensions/2.0/Khronos/KHR_virtual_transforms/README.md
        {
          "name": "arm_socket",
          "parentJoint": 18,
          "translation": [0.0, 0.1, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true
        },
        {
          "name": "lookAt_target",
          "parentJoint": 8,
          "translation": [0.0, 0.0, 0.35],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true
        },
        {
          "name": "sitting_point",
          "parentJoint": 0,
          "translation": [0.0, 0.0, -0.2],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": true
        },
        {
          "name": "wrist_ui",
          "parentJoint": 19,
          "translation": [0.0, 0.25, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "respectParentPosition": true,
          "respectParentRotation": false
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
| `parentJoint` | integer  | Index into the glTF `nodes[]` array representing the base joint           |
| `translation` | float[3] | Initialized local offset (X, Y, Z) relative to the parent joint                        |
| `rotation`    | float[4] | Local orientation as quaternion (X, Y, Z, W) relative to the parent joint |
| `respectParentPosition`    | bool | bool to determine whether this should respect the parent joint position post-initialization |
| `respectParentRotation`    | bool | bool to determine whether this should respect the parent joint rotation post-initialization |

## Examples

### Arm Attachment Virtual Joint

- **Name**: `"arm_socket"`
- **Parent**: Between elbow and wrist (e.g., joint index 18)
- **Translation**: `(0.0, 0.1, 0.0)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **Usage**: Anchor for attaching objects, which want to respect the positions/rotation of the parent joints

### Look At Virtual Joint

- **Name**: `"lookAt_target"`
- **Parent**: `"head"` joint (e.g., joint index 8)
- **Translation**: `(0.0, 0.0, 0.35)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
- **Usage**: Target for runtime look-at behavior (eyes/head alignment).

### Sitting Point Virtual Joint

- **Name**: `"sitting_point"`
- **Parent**: `"pelvis"` joint (e.g., joint index 0)
- **Translation**: `(0.0, 0.0, -0.2)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `true`
<<<<<<<< Updated upstream:extensions/2.0/Khronos/KHR_character_virtual_joints/README.md
- **Usage**: Anchor point for aligning seated positions.
========
- **tags**: Array of strings used to denote semantic tags.
>>>>>>>> Stashed changes:extensions/2.0/Khronos/KHR_virtual_transforms/README.md


### Wrist UI Virtual Joint

- **Name**: `"wrist_ui_socket"`
- **Parent**: `"wrist"` joint (e.g., joint index 19)
- **Offset**: `(0.0, .25, 0.0)`
- **Rotation**: `(0.0, 0.0, 0.0, 1.0)`
- **respectParentPosition**: `true`
- **respectParentRotation**: `false`
- **Usage**: Anchor point for wrist UI, which may wish to respect the parent joint's position, but not the rotation. 

## Implementation Notes

- Virtual joints should exist only within this extension and **must not overlap with joints used in skinning**.
- Their transformation is computed at runtime by applying the offset to the parent joint’s world transform.
- Tools and runtimes may expose these as attachable sockets or semantic retargeting anchors.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
