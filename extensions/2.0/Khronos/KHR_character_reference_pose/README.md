# KHR_character_reference_pose

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / <https://github.com/fire>
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta
- Sarah Cooney, Microsoft XGTG
- Aaron Franke, Independent Contributor

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Conventions

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **NOT RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in [BCP 14](https://www.rfc-editor.org/info/bcp14) when, and only when, they appear in all capitals, as shown here. Sections labeled (Informative), their subsections, and all examples are informative; all other sections are normative.

## Dependencies

Written against the glTF 2.0 specification.

Requires the extension: `KHR_character`.

Assets using `KHR_character_reference_pose` **MUST** list `KHR_character_reference_pose` and `KHR_character` in `extensionsUsed`. They **MUST** contain a top-level `KHR_character` extension object.

## Overview (Informative)

The `KHR_character_reference_pose` extension identifies a standard glTF animation that defines one static local-space reference pose. The animation's ordinary node translation, rotation, and scale channels supply the values they target; every node and TRS property they do not target uses its authored initial value from the asset. The required `poseType` label records the author's classification of that pose.

The extension does not define a retargeting algorithm, joint vocabulary, skin bind pose, playback policy, animation-mixing behavior, or anatomical requirements other than the [up and facing convention](#up-and-facing-convention). Applications may use the reference pose as input to systems that define those behaviors separately.

## Extension usage

The extension object **MUST** be attached to an animation object. The tagged animation **MUST** satisfy the validity requirements below.

An asset **MAY** list `KHR_character_reference_pose` in `extensionsRequired`. A consumer claiming support for this extension **MUST** identify every tagged animation and make each tagged animation, its `poseType`, and its resolved reference-pose values available to host logic, and, whenever it uses a node's local or global reference pose for a tagged animation, **MUST** use that node's resolved reference-pose value or global reference-pose transform, respectively, for that animation as specified below. A consumer that cannot do so **MUST** treat an asset listing the extension in `extensionsRequired` as unsupported, according to the glTF 2.0 extension rules.

### Extension object

The following partial fragment omits the `asset` object, dependency declarations, the top-level `KHR_character` object, nodes, accessors, and buffers. It shows the extension on a tagged animation.

```json
{
  "animations": [
    {
      "name": "ReferencePose",
      "extensions": {
        "KHR_character_reference_pose": {
          "poseType": "t_pose"
        }
      },
      "channels": [
        { "sampler": 0, "target": { "node": 0, "path": "rotation" } },
        { "sampler": 1, "target": { "node": 0, "path": "translation" } },
        { "sampler": 2, "target": { "node": 1, "path": "rotation" } },
        { "sampler": 3, "target": { "node": 1, "path": "translation" } }
      ],
      "samplers": [
        { "input": 0, "output": 1, "interpolation": "STEP" },
        { "input": 0, "output": 2, "interpolation": "STEP" },
        { "input": 0, "output": 3, "interpolation": "STEP" },
        { "input": 0, "output": 4, "interpolation": "STEP" }
      ]
    }
  ]
}
```

### Properties

The extension object **MUST** contain `poseType`, a nonempty string.

| Property   | Type   | Required | Description |
|------------|--------|----------|-------------|
| `poseType` | string | Yes      | Nonempty classification label for this reference pose. |

### Pose labels (Informative)

The labels below are common authoring conventions. They are author assertions, not machine-verifiable anatomy. The resolved reference-pose values defined in [Reference-pose validity](#reference-pose-validity) are authoritative when a label and the data appear inconsistent.

| Value    | Description |
|----------|-------------|
| `t_pose` | Commonly describes a pose with arms extended horizontally. |
| `a_pose` | Commonly describes a pose with arms angled downward. |
| `i_pose` | Commonly describes a pose with arms resting beside the body. |
| *custom* | Any other nonempty label, such as `"ACME_relaxed_pose"`. See [Custom pose labels](#custom-pose-labels). |

Multiple tagged animations may use the same label; animation array indices remain authoritative.

### Custom pose labels

The standard labels `t_pose`, `a_pose`, and `i_pose` are exact and case-sensitive; any other nonempty string, including a differently cased standard label such as `"T_POSE"`, is a custom label. Custom labels **SHOULD** be vendor-qualified, for example `"ACME_relaxed_pose"`, to reduce collisions. This extension does not define what a custom label classifies; that meaning comes only from a separate specification or application profile that defines the label. A custom label does not by itself make a tagged animation invalid; its resolved reference-pose values are defined exactly as for a standard label.

## Reference-pose validity

A tagged animation **MUST** conform to the glTF 2.0 animation requirements and to all of the following additional requirements:

1. The animation **MUST** contain at least one channel.
2. Every channel **MUST** target an ordinary node `translation`, `rotation`, or `scale` property. The channel target **MUST** contain `node`; `weights`, `KHR_animation_pointer`, and other target domains **MUST NOT** be used.
3. The input accessor of every sampler referenced by a channel **MUST** have `count` equal to `1`.
4. The effective interpolation of every referenced sampler **MUST** be `LINEAR` or `STEP`. An omitted `interpolation` property has the core default `LINEAR`. `CUBICSPLINE` and any other interpolation mode **MUST NOT** be used.
5. Each referenced output accessor **MUST** have the type, component type, element count, and value validity required by core glTF for its channel target and interpolation mode.

The single timestamp is not required to be `0`. `STEP` interpolation is not required. With one keyframe, core endpoint behavior yields the same static sample at every requested time for `LINEAR` and `STEP` interpolation.

For each tagged animation, every node has a resolved reference-pose value, which is a local transform. For a node targeted by one or more channels of that animation, the value is a local TRS transform: each TRS property targeted by such a channel uses the referenced sampler's single output value, and each other TRS property uses the node's authored initial value. For any other node, including a node targeted only by channels of other animations, the value is the node's authored initial local transform: its `matrix` when that property is present, otherwise its TRS properties. For both kinds of node, a TRS property taken from the node but omitted from it uses its core glTF default. Authored initial values are the node property values resolved from the asset before ordinary animation or any other runtime writer is applied; when determining a reference-pose value, a consumer **MUST NOT** substitute its current live node transform. A node's global reference-pose transform is computed as core glTF computes a node's global transformation matrix, using each node's resolved reference-pose value for the same tagged animation as its local transform.

A tagged animation's channels need not target every node, every joint, or every TRS property of a targeted node; untargeted nodes and properties resolve as defined above. The extension does not require a skin, require targeted nodes to be skin joints, require a unique pose label, or validate the anatomical meaning of a label. Samplers not referenced by a channel do not contribute to the reference pose.

Tagging an animation changes neither its ordinary core glTF playback behavior nor any node property; this extension only defines the resolved reference-pose values above.

### Validation boundary

The JSON Schema enforces only the extension object's local structure: `poseType` is required and is a nonempty string. Core glTF schemas enforce the ordinary animation object's local structure.

A glTF Validator or authoring tool that claims to validate this extension **MUST** perform the cross-object checks that JSON Schema cannot express:

- verify extension placement on an animation;
- verify the required extension declarations and the top-level `KHR_character` object;
- resolve each channel, sampler, node, and accessor index;
- verify that every channel target is ordinary node TRS;
- verify that every channel-referenced input accessor has exactly one element;
- verify that every channel-referenced sampler's effective interpolation is `LINEAR` or `STEP`;
- apply the core output accessor cardinality and format checks; and
- decode every channel-referenced output accessor, including buffer data and any sparse values, and verify the value validity of the decoded values.

## Up and facing convention

For each tagged animation, the posed result, with every node placed by its global reference-pose transform, **SHOULD** be upright along +Y with its front facing +Z, as core glTF defines up and front for an asset ([Coordinate System and Units](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units)).

## Relationship to skinning and retargeting (Informative)

The reference pose is independent of skinning state. It does not modify `skin.inverseBindMatrices`, determine a bind pose, or participate in skin deformation calculations. A tagged animation may target nodes that are not skin joints, and an asset may use this extension without a skin.

Choosing a tagged animation for a particular skin or `KHR_character_skeleton_mapping` mapping set is host-defined. The nodes that a tagged animation targets need not be associated with roles by `KHR_character_skeleton_mapping` or belong to the character.

A retargeter may consume the reference pose, but this extension does not define source/target correspondence, transform conversion, scale handling, constraints, or a solver. Therefore it does not by itself guarantee lossless, universal, or interoperable retargeting.

## Example (Informative)

Complete example showing a reference pose, labeled `t_pose`, for a simple three-node hierarchy. Arm nodes are omitted for brevity, so the example does not show the arm placement that the label describes:

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_character", "KHR_character_reference_pose"],
  "scene": 0,
  "scenes": [{ "nodes": [0] }],
  "nodes": [
    { "name": "Hips", "children": [1], "translation": [0, 1, 0] },
    { "name": "Spine", "children": [2], "translation": [0, 0.3, 0] },
    { "name": "Head", "translation": [0, 0.4, 0] }
  ],
  "buffers": [
    {
      "byteLength": 88,
      "uri": "data:application/octet-stream;base64,AAAAAAAAAAAAAAAAAAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAACAPwAAAAAAAIA/AAAAAAAAAACamZk+AAAAAAAAAADNzMw+AAAAAA=="
    }
  ],
  "bufferViews": [
    { "buffer": 0, "byteOffset": 0, "byteLength": 4 },
    { "buffer": 0, "byteOffset": 4, "byteLength": 16 },
    { "buffer": 0, "byteOffset": 20, "byteLength": 16 },
    { "buffer": 0, "byteOffset": 36, "byteLength": 16 },
    { "buffer": 0, "byteOffset": 52, "byteLength": 12 },
    { "buffer": 0, "byteOffset": 64, "byteLength": 12 },
    { "buffer": 0, "byteOffset": 76, "byteLength": 12 }
  ],
  "accessors": [
    {
      "bufferView": 0,
      "componentType": 5126,
      "count": 1,
      "type": "SCALAR",
      "min": [0],
      "max": [0]
    },
    {
      "bufferView": 1,
      "componentType": 5126,
      "count": 1,
      "type": "VEC4"
    },
    {
      "bufferView": 2,
      "componentType": 5126,
      "count": 1,
      "type": "VEC4"
    },
    {
      "bufferView": 3,
      "componentType": 5126,
      "count": 1,
      "type": "VEC4"
    },
    {
      "bufferView": 4,
      "componentType": 5126,
      "count": 1,
      "type": "VEC3"
    },
    {
      "bufferView": 5,
      "componentType": 5126,
      "count": 1,
      "type": "VEC3"
    },
    {
      "bufferView": 6,
      "componentType": 5126,
      "count": 1,
      "type": "VEC3"
    }
  ],
  "animations": [
    {
      "name": "ReferencePose",
      "extensions": {
        "KHR_character_reference_pose": {
          "poseType": "t_pose"
        }
      },
      "channels": [
        { "sampler": 0, "target": { "node": 0, "path": "rotation" } },
        { "sampler": 1, "target": { "node": 1, "path": "rotation" } },
        { "sampler": 2, "target": { "node": 2, "path": "rotation" } },
        { "sampler": 3, "target": { "node": 0, "path": "translation" } },
        { "sampler": 4, "target": { "node": 1, "path": "translation" } },
        { "sampler": 5, "target": { "node": 2, "path": "translation" } }
      ],
      "samplers": [
        { "input": 0, "output": 1, "interpolation": "STEP" },
        { "input": 0, "output": 2, "interpolation": "STEP" },
        { "input": 0, "output": 3, "interpolation": "STEP" },
        { "input": 0, "output": 4, "interpolation": "STEP" },
        { "input": 0, "output": 5, "interpolation": "STEP" },
        { "input": 0, "output": 6, "interpolation": "STEP" }
      ]
    }
  ],
  "extensions": {
    "KHR_character": {}
  }
}
```

## Implementation notes (Informative)

- Tagged animations are identified by animation array index. `poseType` classifies the pose; it does not identify an animation.
- The complete example uses `STEP` and a timestamp of `0` as convenient authoring choices, not conformance requirements.
- Authoring tools may preview a tagged animation through ordinary core animation playback, but this extension does not prescribe how that preview is mixed with other animation sources. Core playback leaves every node property the tagged animation does not target at its current value, including untargeted TRS properties of targeted nodes, so a preview matches the reference pose only when each untargeted TRS property, or `matrix`, holds its authored initial value.
- Conversion from any previous bind-pose or matrix representation is format-specific and may involve non-TRS transforms or other information that this extension cannot represent. No general lossless conversion is defined here.
- An asset converted from a convention in which characters face -Z, such as VRM 0.x, needs a 180-degree rotation about +Y, for example at a root node, to follow the [up and facing convention](#up-and-facing-convention).

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity reference-pose demo.

## Schema

- [animation.KHR_character_reference_pose.schema.json](./schema/animation.KHR_character_reference_pose.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
