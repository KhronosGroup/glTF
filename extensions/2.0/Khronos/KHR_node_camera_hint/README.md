# KHR_node_camera_hint

## Contributors

- Ken Jakubzak, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Conventions

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **NOT RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in [BCP 14](https://www.rfc-editor.org/info/bcp14) when, and only when, they appear in all capitals, as shown here. Sections labeled (Informative), their subsections, and all examples are informative; all other sections are normative.

## Dependencies

Written against the glTF 2.0 specification.

This extension has no extension dependencies.

## Overview (Informative)

`KHR_node_camera_hint` lets an author describe candidate initial camera poses, projection data, and subject points with annotated glTF nodes. A host may use those descriptors when presenting an asset.

Camera hints are passive and advisory. This extension does not create or activate a camera, require a host to use or select a hint, define a camera controller, frame geometry, follow an animated node, or prescribe adaptation, transitions, smoothing, or interaction. Its only selection guidance is in [Default view selection](#default-view-selection) and [Custom roles](#custom-roles).

## Extension usage

The extension object **MUST** be attached to a glTF node. An asset using this extension **MUST** list `KHR_node_camera_hint` in `extensionsUsed`.

An asset **MUST NOT** list `KHR_node_camera_hint` in `extensionsRequired`. Camera hints have no mandatory presentation behavior. Except as recommended in [Default view selection](#default-view-selection), [Custom roles](#custom-roles), and [Label and node name](#label-and-node-name), consumers **MAY** use, adapt, or ignore any descriptor. Ignoring a camera hint does not suppress or otherwise alter ordinary glTF content.

The annotated node remains an ordinary node. Its mesh, skin, camera, children, animation, and other extensions retain their normal meanings. In particular, this annotation does not hide geometry attached to the node and does not instantiate the camera referenced by this extension's `camera` property.

### Extension object

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `role` | string | Yes | — | Nonempty semantic role for the camera hint. Standard values are defined below. |
| `camera` | integer | No | — | Advisory index of a camera in the top-level `cameras` array. |
| `targetNode` | integer | No | — | Index of a different node whose global translation is the hint's subject point. It does not change the initial camera pose. |
| `label` | string | No | — | Nonempty human-readable label for the viewpoint, for display in authoring tools, inspectors, and camera affordances. It has no camera behavior. |

## Authored descriptor

### Initial camera pose

The host supplies an evaluation state: the node transforms it is using, including any animated values it has applied, or the authored values when it applies none. The hint node's global transform is evaluated using ordinary core glTF scene-graph rules. Its translation component is the authored position.

For every role, the initial camera pose is interpreted as if a core glTF camera were instantiated on the hint node, following the [core View Matrix rules](https://github.com/KhronosGroup/glTF/blob/c18432787e6d545a1218c1926ccdcfaffd4c116b/specification/2.0/Specification.adoc#view-matrix). The camera looks along local −Z with local +Y up; orientation and scale handling follow those rules. This interpretation does not require a `node.camera` property or instantiate a camera.

When the core glTF camera view-matrix rules leave the view matrix undefined, for example because a scale component is zero, the hint defines no initial camera pose. A host **MAY** ignore that pose or derive a pose through host-defined adaptation. Such a transform does not invalidate the hint.

When defined, the initial camera pose is the authored starting pose, before host-defined adaptation, framing, orbiting, following, or interaction. A host **MAY** use, adapt, or ignore it. "Initial" does not select an animation time. No property of this extension, including `targetNode`, modifies that pose, and deriving it does not modify the node transform.

The initial camera pose and the subject point are expressed in the coordinate space of the asset's node hierarchy; any transform a host applies to place the whole asset is outside this definition.

### Projection reference

When `camera` is present, it **MUST** be a valid index into the top-level `cameras` array. The referenced camera definition is advisory projection data associated with the hint. Referencing it does not instantiate a core camera, override `node.camera`, or change the support contracts of extensions on that camera. A host **MAY** adapt or ignore the referenced projection data. When `camera` is absent, the hint carries no projection data; a `node.camera` on the hint node does not supply it.

The referenced camera contributes projection data only. The hint node's global transform, as interpreted in [Initial camera pose](#initial-camera-pose), is the sole source of the initial camera pose, including when the hint node itself carries the referenced camera; neither the referenced camera nor any other node that instantiates it via `node.camera` contributes to that pose. A camera pose that a host takes from another node is host-defined adaptation (see [Host-defined behavior](#host-defined-behavior)), not the hint's initial camera pose.

Multiple camera hints **MAY** reference the same camera, and a camera hint **MAY** reference the camera attached to its own node via `node.camera`.

### Subject point

When `targetNode` is present, it **MUST** be a valid index into the top-level `nodes` array and **MUST NOT** refer to the node containing the camera hint. A present `targetNode` is *usable* when it meets both requirements. When `targetNode` is usable, the subject point is its global translation, evaluated in the same evaluation state as the hint node, even when that translation equals the authored position.

When `targetNode` is absent or not usable, the hint defines no subject point, and a host **MAY** choose one.

The subject point identifies a point of interest for the hint, such as the subject to keep framed or the center to orbit about. The subject point does not orient the camera or change the initial camera pose. It is a single position with no bounds, margins, or extent. It lies at a definite distance from the authored position, which a host can use, for example as the starting orbit radius of an `orbit_view` hint.

`targetNode` **MAY** reference a node that carries `KHR_node_lookat_target`. This extension alone defines the subject point. Both extensions read that node's global translation, so when `targetNode` is usable, the subject point equals the marker's target point in the same evaluation state wherever `KHR_node_lookat_target` defines that target point. Neither extension requires the other. A standalone point of interest that is not tied to an initial camera pose can be expressed with `KHR_node_lookat_target` instead of a camera hint.

Animation of either node retains ordinary core glTF meaning. This extension does not select an evaluation state or require a camera to follow an animated hint or target.

## Standard roles

The standard role strings are exact and case-sensitive; any other string, including a differently cased standard value such as `"Default_View"`, is a custom role. Roles classify authored intent. Apart from the recommendations in [Default view selection](#default-view-selection) and [Custom roles](#custom-roles), a role does not select a hint or require a particular camera behavior.

For every role, standard or custom, the hint describes a candidate initial camera pose, as defined in [Initial camera pose](#initial-camera-pose); the role states what that pose is for. `camera`, `targetNode`, and `label` have the same meaning for every role, and any combination of them is valid with any role.

| `role` | Standard meaning |
|--------|------------------|
| `"default_view"` | Candidate initial camera pose for the view a host presents by default. |
| `"detail"` | Candidate initial camera pose for a detail view. |
| `"first_person"` | Candidate initial camera pose for a first-person view. |
| `"framing_view"` | Candidate initial camera pose for a framing view, intended to keep the subject point framed when the hint defines one. It does not define bounds, margins, aspect ratio, or a framing algorithm. |
| `"full_body"` | Candidate initial camera pose classified by the author as a full-body view. |
| `"orbit_view"` | Candidate initial camera pose for orbit interaction, centered on the subject point when the hint defines one. It does not define an orbit controller or orbit limits. |
| `"portrait"` | Candidate initial camera pose classified by the author as a portrait view. |
| `"third_person"` | Candidate initial camera pose for a third-person view. It does not define following or tracking. |

Hints whose role is `framing_view` or `orbit_view` **SHOULD** include `targetNode`.

Multiple hints **MAY** use the same role. Except as recommended in [Default view selection](#default-view-selection) and [Custom roles](#custom-roles), array order, node traversal order, and role do not define priority or selection.

Selecting a `"first_person"` or `"third_person"` hint does not by itself tag a view for `KHR_node_visibility_hint` or `KHR_mesh_primitive_visibility_hint`. A host can coordinate those features, but that coordination is outside the glTF asset contract.

### Default view selection

This recommendation applies when a host automatically chooses its default view from camera hints without first restricting the candidates to roles other than `"default_view"`. If at least one hint has role exactly `"default_view"`, the host **SHOULD** choose, among those hints, the one on the node with the lowest index in the top-level `nodes` array.

Tools that merge, prune, or reorder nodes can change node indices, and with them which hint this recommendation selects, even when they update every index reference correctly. An author who needs a particular default view can make it the only hint with role `"default_view"`.

A host that restricts its automatic choice to other roles is outside this recommendation; for example, a game host that starts in first person and chooses among `"first_person"` hints. This recommendation does not order hints that share any other role. A host **MAY** choose a different hint, or none, for an explicit user choice, an application profile, or accessibility needs.

## Custom roles

A custom role is any nonempty role string other than the standard values. Custom roles **SHOULD** be vendor-qualified, for example `"ACME_inspection_view"`, to reduce collisions. Custom roles are valid and have the same descriptor meaning as standard roles. This extension does not define what any particular custom role classifies; that meaning comes only from a separate specification or application profile that defines the role.

A host recognizes a custom role only when it implements a separate specification or application profile that defines that role; every other custom role is unrecognized by that host. A host **SHOULD NOT** use or surface hints with unrecognized custom roles in default camera affordances, meaning automatic uses and generic user-facing presentations of hints, such as an automatically chosen default view, camera menus, view tours, thumbnails, or automatic shot sequences. Such a role can mark an internal, experimental, or application-specific view, or be a role string from an earlier draft of this extension. A host **MAY** surface these hints in diagnostic, inspector, or editing views. This recommendation does not restrict importing, editing, preserving, or exporting such hints.

## Label and node name

`label` names the viewpoint this hint describes; `node.name` names the node as a whole, which can also carry a mesh, serve as a skin joint, or hold other content. Core glTF uses names for application-specific purposes such as display, refers to nodes by index rather than by name, and does not require names to be unique. A node named `"Empty_07"`, for example, can carry a hint labeled `"Entrance view"`. `label` has no camera behavior.

When a host or tool presents a hint to a user, including in camera menus and other camera affordances, it **SHOULD** identify the hint by `label` when present, otherwise by `node.name` when present and nonempty, otherwise by its `role`. For a standard role, or a custom role it recognizes, it **MAY** show a display name it provides, such as a localized name, in place of the raw role string. It **MAY** show additional context, such as the node index, alongside that text. This extension defines no localization mechanism for `label`.

## Host-defined behavior

The host decides:

- whether to use a hint and which hint to select, subject to [Default view selection](#default-view-selection) and [Custom roles](#custom-roles);
- which custom roles to surface in default camera affordances, subject to [Custom roles](#custom-roles);
- which evaluation state to use, including when to resolve animated global transforms;
- whether or how to adapt the initial camera pose or projection data;
- whether to choose a subject point for a hint that defines none, and whether and how to use the subject point, such as tracking it, framing it, or orbiting about it;
- camera creation, activation, following, constraints, and controllers;
- orbit, fitting, framing, margins, aspect-ratio handling, transitions, smoothing, and user interaction; and
- coordination with visibility hints or other presentation systems.

Apart from the recommendations in [Default view selection](#default-view-selection) and [Custom roles](#custom-roles), these choices are not conformance behavior for this extension. Validators do not check host behavior.

## Relationship to core glTF cameras (Informative)

A core glTF camera is an optical system. A `cameras` entry defines projection only; it carries no position or orientation. In core glTF, pose comes from the node a camera is attached to via `node.camera`.

A camera hint keeps that separation. The hint node's global transform supplies the initial camera pose under the core camera rules, `role` classifies what that pose is for, an optional `targetNode` supplies the subject point, and an optional `camera` reference supplies advisory projection.

| Source | Contributes |
|--------|-------------|
| The hint node's global transform | Initial camera position and orientation |
| The `targetNode` global translation, when present and usable | Subject point |
| The referenced `camera` | Projection type and parameters, and any camera extensions |

Adapting the initial camera pose, framing, bounds, margins, transitions, and how the subject point is used are host territory, as described in [Host-defined behavior](#host-defined-behavior).

Referencing a camera rather than redefining projection lets a hint carry any camera extension, such as depth of field or lens distortion, with no change to this extension.

A core camera node already pairs pose with projection through `node.camera`, so a separate advisory reference can look redundant. The difference is one of contract. Attaching a core camera instantiates a camera, which a renderer may select and use directly. A camera hint instantiates nothing; it records an authored viewpoint, what it is for, and which projection data is associated with it, and leaves every use decision to the host. That weaker contract lets hints annotate meshes, empties, skin joints, and other ordinary nodes without turning them into cameras, and lets several hints reference one projection definition without instantiating it. Annotating only core camera nodes would force authors to create a live camera for every candidate viewpoint and would conflate describing a viewpoint with instantiating a camera for it.

The hint property is named `camera` because it indexes the top-level `cameras` array. It does not carry the meaning of core `node.camera`:

| Aspect | Core `node.camera` | Hint `camera` |
|--------|-------------------|---------------|
| Instantiation | Instantiates the camera on the node. | Never instantiates a camera. |
| Selection | Identifies a camera a renderer may use directly. | Never selects or activates a camera or hint. |
| Pose source | The global transform of the node the camera is attached to. | The hint node's global transform; the referenced camera contributes projection data only. |
| Override | — | Does not override `node.camera` on any node. |
| Ignorability | Governed by core glTF and camera-extension rules. | A host may adapt or ignore the referenced projection data without affecting ordinary glTF content. |

## Complete example (Informative)

This example provides a default view with a separate point of interest and an orbit view with its own pivot. The referenced core camera supplies advisory projection data.

```json
{
  "asset": { "version": "2.0" },
  "scene": 0,
  "scenes": [
    { "nodes": [0, 1, 2, 3] }
  ],
  "extensionsUsed": [
    "KHR_node_camera_hint"
  ],
  "cameras": [
    {
      "type": "perspective",
      "perspective": {
        "yfov": 0.785398,
        "znear": 0.01,
        "zfar": 100.0
      }
    }
  ],
  "nodes": [
    {
      "name": "DefaultView",
      "translation": [0.0, 1.5, 3.0],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "default_view",
          "camera": 0,
          "targetNode": 1,
          "label": "Authored default view"
        }
      }
    },
    {
      "name": "PointOfInterest",
      "translation": [0.0, 1.0, 0.0]
    },
    {
      "name": "OrbitStart",
      "translation": [2.0, 0.75, 2.0],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "orbit_view",
          "targetNode": 3
        }
      }
    },
    {
      "name": "OrbitPivot",
      "translation": [0.0, 0.75, 0.0]
    }
  ]
}
```

The default-view hint's initial camera pose is the `DefaultView` node's authored position `[0.0, 1.5, 3.0]` with identity orientation; `targetNode` names `PointOfInterest` as the subject point without changing that pose. The orbit-view hint's initial camera pose is likewise the `OrbitStart` node's authored position with identity orientation, and `targetNode` names `OrbitPivot` as the orbit center; it does not create an orbit controller.

## Ordinary node content example (Informative)

The following partial fragment omits the `asset` object and the referenced mesh data. It demonstrates that a camera hint does not suppress ordinary content attached to its node.

```json
{
  "extensionsUsed": ["KHR_node_camera_hint"],
  "nodes": [
    {
      "mesh": 0,
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "detail"
        }
      }
    }
  ]
}
```

## Validation boundary

The JSON Schema enforces only the extension object's local structure: `role` is required and nonempty, `camera` and `targetNode` are non-negative indices, and `label` is nonempty when present.

A validator that claims to validate this extension **MUST** additionally:

- verify placement on a node;
- verify the `extensionsUsed` declaration and reject `KHR_node_camera_hint` in `extensionsRequired`;
- resolve `camera` against the top-level `cameras` array when present;
- resolve `targetNode` against the top-level `nodes` array when present; and
- reject a `targetNode` that refers to the node containing the extension.

Coincident hint and target positions are valid; the subject point then equals the authored position. Every scale value, including negative and zero scale, is valid. Validators do not select hints, compute framing or subject distances, choose default views on behalf of hosts, or validate host camera behavior.

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js loader, helper, and VRM conversion tooling.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity camera-hint demo.

## Known limitations (Informative)

- The extension does not define hint priority or selection, except for the recommended default-view choice and the recommendation against using or surfacing hints with unrecognized custom roles in default camera affordances.
- A subject point is a single position: it defines no framing bounds, margins, orbit limits, or controller parameters.
- Custom roles have no portable meaning beyond the generic descriptor.
- Advisory hints cannot guarantee presentation across hosts.

## Schema

- [node.KHR_node_camera_hint.schema.json](./schema/node.KHR_node_camera_hint.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
