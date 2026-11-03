# KHR_node_visibility_hint

## Contributors

- Ken Jakubzak, Meta
- 0b5vr / pixiv Inc.
- Aaron Franke, Independent Contributor
- Hideaki Eguchi / VirtualCast, Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Conventions

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **NOT RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in [BCP 14](https://www.rfc-editor.org/info/bcp14) when, and only when, they appear in all capitals, as shown here. Sections labeled (Informative), their subsections, and all examples are informative; all other sections are normative.

## Dependencies

Written against the glTF 2.0 specification.

This extension has no extension dependencies. It composes with [`KHR_node_visibility`](../KHR_node_visibility/README.md) when both extensions are supported, but does not require it.

## Overview (Informative)

The *subject* is the entity the asset represents, such as a character, a vehicle, or a robot. Some of its content is meant to be visible only some of the time: a character's head can block a camera placed at its eyes, and a vehicle's detailed cockpit interior may be needed only while someone sits inside.

This extension lets an author give a node one or more visibility roles, which say when the node's content, and that of its descendants, is meant to be visible. The application's logic decides when each role applies. Where an application has no logic for these roles, they hide nothing, so viewers such as asset inspectors and thumbnails show all content.

## Extension usage

The extension object **MUST** be attached to a glTF node. An asset using this extension **MUST** list `KHR_node_visibility_hint` in `extensionsUsed` and **MUST NOT** list it in `extensionsRequired`. Except as recommended in [Recommended rendering](#recommended-rendering) and [Label](#label), a host **MAY** use, adapt, or ignore hints. An implementation that does not support this extension ignores hints, so they suppress no visual content there.

### Extension object

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `roles` | string[] | Yes | One or more distinct nonempty visibility roles. Their order has no meaning. |
| `label` | string | No | Nonempty human-readable label, preferred over `node.name` and the roles when a host or tool presents the hint to a user. It has no effect on visibility. |

## Roles

A role names when the author intends content to be visible. Role strings are exact and case-sensitive.

A render view is a rendered viewpoint whose image is presented, such as a camera view, one eye of a stereo pair, or a mirror view. Shadow maps and lighting or reflection probes are not render views. For each render view and instance of the asset, the application's logic determines which roles apply. This extension does not define that logic, which can depend on cameras, application state, or anything else, and several roles can apply in one view at once. Roles apply to an asset instance as a whole: this extension does not identify subjects within an asset, and representation of multiple subjects within a single asset is outside its scope.

The standard roles are `"always"`, `"first_person"`, and `"third_person"`. `"always"` applies in every render view. `"first_person"` and `"third_person"` apply where the application's logic determines. Where the application has no logic for a standard role, that role applies in every render view, so an application with no logic hides nothing.

> Implementation Note (Informative): `"first_person"` typically marks content intended for a view from the subject's own position, such as first-person arms or a cockpit interior, and `"third_person"` content that would obstruct such a view, such as a head or helmet. When an application applies `"first_person"` in its first-person camera views and `"third_person"` in all other views, they behave like `firstPersonOnly` and `thirdPersonOnly` in the [VRM 1.0 `firstPerson` specification](https://github.com/vrm-c/vrm-specification/blob/70ae16e93abd6da727fdf641b67aa41010c6d933/specification/VRMC_vrm-1.0/firstPerson.md).

Any other nonempty string, including a differently cased standard value such as `"First_Person"`, is a custom role. Custom roles **SHOULD** be vendor-qualified, for example `"ACME_thermal_only"`. A custom role applies where the application or the application profile that defines it says. A host ignores custom roles it does not recognize.

A hint allows a render view when at least one of its recognized roles applies there. A hint whose roles include `"always"`, or that has no recognized role, imposes no restriction.

Using a [`KHR_node_camera_hint`](../KHR_node_camera_hint/README.md), including one whose role is `"first_person"`, does not by itself make a role apply; the application's logic can take such hints into account.

## Combining hints

For a node instance `n` and a render view, let `T` be the set of roles that apply in that view for that instance. `ownHintVisible(n, T)` is `true` when `n` has no hint, and otherwise whether the hint on `n` allows that view. The effective hint predicate is the logical AND of `ownHintVisible` for `n` and every ancestor in the same node-instance hierarchy. With the value for the parent of a root defined as `true`:

```text
hintVisible(n, T) = ownHintVisible(n, T) AND hintVisible(parent(n), T)
```

Roles in one hint are alternatives; hints on a node and its ancestors all apply. A descendant hint can add a restriction. A descendant hint **MUST NOT** restore visual content hidden by an ancestor's hint for the same render view and instance. An explicit `"always"` role imposes no additional restriction and cannot cancel an ancestor's restriction.

When `KHR_node_visibility` is present and supported, `coreVisible(n)` is the logical AND of the current `KHR_node_visibility.visible` values on `n` and all its ancestors, using that extension's default of `true` where the property or extension is absent; otherwise it is `true`. A `visible: false` at any level hides that node and its subtree in every view. A `visible: true` on a descendant does not change this outcome, because `coreVisible(n)` is false whenever any ancestor of `n` has `visible: false`. This restates `KHR_node_visibility` and does not modify it.

```text
renderNodeVisualContent(n, T) = hintVisible(n, T) AND coreVisible(n)
```

[`KHR_mesh_primitive_visibility_hint`](../KHR_mesh_primitive_visibility_hint/README.md) further restricts individual primitives: a primitive instance is drawn only when both its containing node's predicate and its own primitive predicate allow it.

## Recommended rendering

An application that applies `"first_person"` **SHOULD** apply `"third_person"` wherever `"first_person"` does not apply.

A host **SHOULD NOT** directly draw a node's visual content in a render view where `renderNodeVisualContent` is `false` for that instance, and **SHOULD NOT** omit it because of a hint where that predicate is `true`. This predicate determines whether the node's own visual content is drawn directly in that view by a host that follows this section. A host **MAY** depart from this section on an explicit user choice, such as an editing or diagnostic view that shows all content.

Rendering techniques and effects on shadows, reflections, and lighting are host-defined. The predicate is evaluated independently for each render view and node instance. Applying it does not change glTF-authored or animated node state, transforms, animation, or cameras, and one view's result does not affect another view or instance.

> Implementation Note (Informative): Visibility can differ between views or instances, including instances that share a mesh. Shared resources and temporary rendering state need to accommodate those differences.

## Label

When a host or tool presents a hint to a user, it **SHOULD** identify the hint by `label` when present, otherwise by `node.name` when present and nonempty, otherwise by its roles.

## Examples (Informative)

An aircraft can show its detailed cockpit interior only while the application applies `"first_person"`, for example from the pilot's seat, and its canopy exterior otherwise:

```text
Aircraft (no hint)
  -> Airframe (no hint)
  -> CanopyExterior (roles: [third_person])
  -> CockpitInterior (roles: [first_person])
```

The following partial fragment omits mesh, accessor, and buffer definitions. A VR application applies `"first_person"` in the user's own eye views of the user's avatar, and a custom role from its profile, `"ACME_mirror_only"`, in its mirror views. The status display is meant for the user's own eyes and mirrors, but not for spectators.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_visibility_hint"],
  "nodes": [
    { "name": "CharacterRoot", "children": [1, 2, 3, 4] },
    { "name": "Torso", "mesh": 0 },
    { "name": "Head", "mesh": 1,
      "extensions": { "KHR_node_visibility_hint": { "roles": ["third_person"] } } },
    { "name": "FirstPersonArms", "mesh": 2,
      "extensions": { "KHR_node_visibility_hint": { "roles": ["first_person"] } } },
    { "name": "SelfStatus", "mesh": 3,
      "extensions": { "KHR_node_visibility_hint": { "roles": ["first_person", "ACME_mirror_only"], "label": "Self status" } } }
  ]
}
```

| Node | User's eye view (`first_person` applies) | Mirror view (`third_person` and `ACME_mirror_only` apply) | Spectator view (`third_person` applies) | Host with no logic, such as an inspector (every standard role applies) |
|------|------|------|------|------|
| `Torso` | Drawn | Drawn | Drawn | Drawn |
| `Head` | Not drawn | Drawn | Drawn | Drawn |
| `FirstPersonArms` | Drawn | Not drawn | Not drawn | Drawn |
| `SelfStatus` | Drawn | Drawn | Not drawn | Drawn |

A host that has logic for `"first_person"` but does not recognize `"ACME_mirror_only"` ignores that role, so `SelfStatus` is drawn only where `"first_person"` applies. A shared skeleton can still deform all four meshes, because hiding content does not disable transform or animation evaluation.

### Visibility cases

The following cases assume no other ancestor hints and a `coreVisible` result of `true`:

| Parent roles | Child roles | Roles that apply | Child drawn |
|--------------|-------------|------------------|-------------|
| None | `third_person` | Every standard role (no logic) | Yes |
| None | `first_person` | Every standard role (no logic) | Yes |
| None | `first_person` | `third_person` | No |
| `third_person` | `always` | `first_person` | No |
| `third_person` | `always` | `third_person` | Yes |
| `third_person` | `first_person` | `first_person` | No |
| `third_person` | `first_person` | `first_person`, `third_person` | Yes |
| None | `first_person`, `ACME_mirror_only` (unrecognized) | `third_person` | No |
| None | `ACME_mirror_only` (unrecognized) | Any | Yes |

To give parts of a subtree different visibility, authors put the roles on separate branches below an unannotated root, as in the examples above.

## Validation boundary

The JSON Schema enforces only the extension object's local structure: `roles` is a required nonempty array of distinct nonempty strings, and `label` is nonempty when present.

A validator **MUST** verify placement on a glTF node, the `extensionsUsed` declaration, and absence from `extensionsRequired`. Custom roles are valid. A validator **MAY** report, without treating the asset as invalid, `roles` that list `"always"` with other roles. Validators do not check host rendering behavior.

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and first-/third-person visibility demo.

## Known limitations (Informative)

- The application decides which roles apply in each render view, and when that changes.
- The predicate is binary; fades and transitions are host-defined.
- Restructuring a node hierarchy changes which ancestor restrictions apply, so authoring tools should preserve or deliberately relocate annotations when reparenting nodes. A child hint cannot reopen a subtree hidden by an ancestor hint.

## Schema

- [node.KHR_node_visibility_hint.schema.json](./schema/node.KHR_node_visibility_hint.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
