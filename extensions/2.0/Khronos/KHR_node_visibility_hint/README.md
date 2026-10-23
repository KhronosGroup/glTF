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

Many applications present the same asset in more than one kind of view. For example, character head geometry may be useful in a third-person view but obstruct a first-person camera. This extension associates a node with a visibility role whose restrictions also apply to its descendants. Render views carry view tags that the host assigns, such as `"first_person"` for a first-person view. A node's content appears only when its own hint and every ancestor hint allow it in that view.

The role defines a pure predicate for each node instance and render view. Evaluating it does not alter glTF-authored or animated node state, tag views, create or activate a camera, or define transitions when a view's tags change.

## Extension usage

The extension object **MUST** be attached to a glTF node. An asset using this extension **MUST** list `KHR_node_visibility_hint` in `extensionsUsed`.

An asset **MAY** list `KHR_node_visibility_hint` in `extensionsRequired`. A consumer claiming support for this extension **MUST** evaluate standard roles and inheritance as specified. When the consumer also supports `KHR_node_visibility`, it **MUST** apply the interaction defined below. A consumer that cannot provide the required hint behavior **MUST** treat an asset listing `KHR_node_visibility_hint` in `extensionsRequired` as unsupported according to the glTF 2.0 extension rules.

If the extension appears only in `extensionsUsed`, an implementation that does not support it ignores the hint and renders the asset as core glTF and the other extensions it supports define, so the hint suppresses no visual content.

### Extension object

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `role` | string | Yes | — | Nonempty visibility role. Standard values are defined below. |
| `label` | string | No | — | Nonempty human-readable label, preferred over `node.name` and `role` when a host or tool presents the hint to a user. It has no effect on visibility. |

When a host or tool presents a hint to a user, it **SHOULD** identify the hint by `label` when present, otherwise by `node.name` when present and nonempty, otherwise by its `role`.

## View tags

A render view is a rendered viewpoint whose image is presented, such as a camera view, one eye of a stereo pair, or a mirror view. Shadow maps and lighting or reflection probes are not render views. The host gives each render view zero, one, or several view tags. The standard view tags are `"first_person"`, for a view presented from the character's own viewpoint, and `"third_person"`, for a third-person view of the character, such as a spectator or mirror view. A host **MAY** also give a render view custom view tags defined by a separate specification or application profile; this extension defines none. View tag strings are exact and case-sensitive. Custom view tags **SHOULD** be vendor-qualified, for example `"ACME_mirror"`, to reduce collisions. Their meaning comes only from the specification or application profile that defines them. View tags are not stored in the asset.

How a host tags its views is host-defined, including when tags change and whether one render view has different tags for different instances of the asset, for example `"first_person"` for the instance that represents the user and `"third_person"` for other instances of the same asset. Where this extension refers to the view tags of a render view, it means the tags that apply to the instance being evaluated.

`KHR_character` designates a glTF asset as containing a character; representation of multiple characters within a single asset is outside its scope. This extension does not define character ownership.

Using a `KHR_node_camera_hint`, including one whose role is `"first_person"`, does not by itself tag a view; the host tags its views and can take such hints into account.

## Standard role predicate

The standard role strings are exact and case-sensitive. For a render view with view tags `T`, the standard role predicate is:

| `role` | Visible when |
|--------|--------------|
| `"always"` | Always, including when `T` is empty. |
| `"first_person"` | `T` includes `"first_person"`. |
| `"third_person"` | `T` includes `"third_person"`. |

Custom view tags in `T` do not affect these results. For an individual standard role predicate in a render view without view tags, only `"always"` returns `true`. A supported custom role returns `true` there only where the specification defining that role says. Ancestor hints and `KHR_node_visibility` can further restrict the final visibility.

Every consumer that supports this extension supports the three standard roles. A consumer supports a custom role only as described under [Custom roles](#custom-roles). A consumer that supports this extension **SHOULD NOT** render visual content when a hint on that node or an ancestor has a role it does not support (the role-support rule); whether to render such content anyway remains the application's decision.

A node with neither its own hint nor an annotated ancestor is equivalent to `role: "always"`. An explicit `role: "always"` imposes no additional restriction and cannot cancel an ancestor's restriction. A differently cased token such as `"First_Person"` is not a standard role but a custom role.

### Authoring guidance (Informative)

Because a render view can have no view tags, and a standard role other than `"always"` allows content only in views with the matching tag, authors are responsible for assigning roles so that content appears where intended. Content intended for every view uses `"always"` or no hint on a branch whose ancestor hints also allow every view. To give descendant content a different set of views, authors can move the ancestor's restriction to the specific renderable nodes it is intended to affect.

## Inheritance and composition

For a node instance `n` and the view tags `T` of its render view, let `ownHintVisible(n, T)` be `true` when `n` has no hint. When `n` has a hint with a standard role, it is that role's standard predicate. For a supported custom role, it is the visibility that the separate specification defining the role gives for that render view and instance. For a role the consumer does not support, it is `false` unless the application decides to render the content anyway, as described under [Standard role predicate](#standard-role-predicate).

The effective hint predicate `hintVisible(n, T)` **MUST** be the logical AND of `ownHintVisible` for `n` and every ancestor in the same evaluated node-instance hierarchy. Equivalently, with `parent(n)` denoting the parent node instance and the parent predicate of a root defined as `true`:

```text
hintVisible(n, T) = ownHintVisible(n, T) AND hintVisible(parent(n), T)
```

An unannotated node inherits its parent's effective hint visibility. A descendant hint can impose an additional restriction. A descendant hint **MUST NOT** restore visual content hidden by an ancestor's hint for the same render view and instance, including when the descendant's role is `"always"`.

When `KHR_node_visibility` is present and supported, let `coreVisible(n)` be the logical AND of the current `KHR_node_visibility.visible` values on `n` and all its ancestors, using that extension's default of `true` where the property or extension is absent. Otherwise, `coreVisible(n)` is `true`. If `KHR_node_visibility` is separately listed in `extensionsRequired` and the consumer does not support it, the asset is unsupported under ordinary glTF extension rules.

Because `coreVisible(n)` is a logical AND over a node and all its ancestors, an invisible node higher in the hierarchy always takes priority: a `visible: false` at any level hides that node and its entire subtree, and no `visible: true` on a descendant can restore it. The following cases summarize this interaction for an ancestor–descendant pair, independent of any hint role:

| Ancestor `visible` | Descendant `visible` | Result |
|--------------------|----------------------|--------|
| `true`  | `false` | Descendant hidden. |
| `false` | `true`  | Ancestor and descendant hidden. |
| `false` | `false` | Ancestor and descendant hidden. |
| `true`  | `true`  | Both visible, subject to the hint predicate. |

This mirrors the normative semantics of `KHR_node_visibility`; it is restated here only to make the composition explicit and does not modify that extension.

For each node instance `n` and render view, the visual-content predicate uses that instance's effective hint visibility and view tags `T`:

```text
renderNodeVisualContent(n, T) = hintVisible(n, T) AND coreVisible(n)
```

This predicate determines whether the node's own visual content is drawn directly in that view. Rendering techniques and effects on shadows, reflections, and lighting are host-defined. `KHR_node_visibility` retains its own requirements. Hints do not change camera behavior or non-visual node state.

> Implementation Note (Informative): Visibility can differ between views or instances, including instances that share a mesh. Shared resources and temporary rendering state need to accommodate those differences.

Effective hint visibility **MUST** be evaluated independently for each render view and node instance. Evaluation **MUST NOT** observably change glTF-authored or animated node state or cause one view's result to affect another view or instance. Hiding visual content through this predicate does not change camera behavior or transform and animation evaluation. `KHR_node_visibility.visible: false` on an ancestor continues to hide descendant visual content according to that extension's normative semantics.

When both extensions are used, the asset **MUST** list both in `extensionsUsed`. Combined use does not change the rule for listing `KHR_node_visibility_hint` in `extensionsRequired` given under [Extension usage](#extension-usage); whether `KHR_node_visibility` is listed there follows that extension's specification and the glTF 2.0 extension rules.

## Custom roles

A custom role is any nonempty role string other than the three standard values. A consumer supports a custom role only when it implements a separate specification or application profile that defines that role; every other custom role is unsupported by that consumer, and content with that role falls under the role-support rule in [Standard role predicate](#standard-role-predicate).

Custom roles **SHOULD** be vendor-qualified, for example `"ACME_mirror_only"`, to reduce collisions. Their meaning comes from the separate specification or application profile that defines them and is not portable under this extension alone.

Custom-role semantics come only from a separate specification, such as a vendor extension or application profile. Content with a supported custom role is visible where that specification says, typically in render views with a custom view tag that it defines. That specification defines its own association with role tokens, data placement, behavior, and support contract; this extension does not prescribe a companion object or naming relationship. Any glTF extension used for that purpose **MUST** follow the ordinary `extensionsUsed` and `extensionsRequired` rules. Without separately supported semantics, the role is unsupported and the role-support rule applies.

### Custom-role authoring guidance (Informative)

Because a consumer that supports this extension is recommended not to render content restricted by a role it does not support, authors can reserve custom roles for content whose absence is acceptable in applications that do not support the role. A custom role on a descendant adds its restriction to the ancestor hint results. It cannot restore content hidden by an ancestor's role.

For example, a VR application can tag its mirror views `"third_person"` and `"ACME_mirror"`, a custom view tag defined by its application profile. If that profile also defines a custom role `"ACME_mirror_only"` that is visible only in views tagged `"ACME_mirror"`, an avatar can give the node of a mirror-only detail mesh that role. The detail then appears in the application's mirrors but not in its other third-person views, and an application without that profile does not support the role and is recommended not to render the detail.

## Interaction with primitive visibility

[`KHR_mesh_primitive_visibility_hint`](../KHR_mesh_primitive_visibility_hint/README.md) defines the corresponding predicate at mesh-primitive granularity. For a primitive instance, the containing node predicate and the primitive predicate are combined with logical AND. A primitive hint cannot make visual content visible when its containing node is hidden.

## Examples (Informative)

The following partial fragment omits mesh, accessor, and buffer definitions. It demonstrates inherited restrictions and an additional descendant restriction.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_visibility_hint"],
  "nodes": [
    {
      "name": "CharacterRoot",
      "mesh": 0,
      "children": [1],
      "extensions": {
        "KHR_node_visibility_hint": { "role": "third_person" }
      }
    },
    {
      "name": "FirstPersonHands",
      "mesh": 1,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "first_person" }
      }
    }
  ]
}
```

In a render view tagged only `"first_person"`, both nodes' visual content is hint-hidden because `CharacterRoot` requires `"third_person"`. In a view tagged only `"third_person"`, `CharacterRoot` is hint-visible while `FirstPersonHands` is hint-hidden because its own role requires `"first_person"`. In a view carrying both tags, both nodes are hint-visible. In a view without tags, neither node's visual content is hint-visible.

To show first-person hands independently of third-person body content, authors can leave their shared transform root unannotated and put the roles on separate mesh nodes:

```text
CharacterRoot (no hint)
  -> Body (role: third_person)
  -> FirstPersonHands (role: first_person)
```

`Body` and `FirstPersonHands` are siblings in this arrangement. A skeleton can still provide their deformation; visibility filtering does not disable transform or animation evaluation.

The following partial fragment omits mesh, accessor, and buffer definitions. It demonstrates composition with `KHR_node_visibility`.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": [
    "KHR_node_visibility_hint",
    "KHR_node_visibility"
  ],
  "extensionsRequired": [
    "KHR_node_visibility_hint",
    "KHR_node_visibility"
  ],
  "nodes": [
    {
      "mesh": 0,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "third_person" },
        "KHR_node_visibility": { "visible": false }
      }
    }
  ]
}
```

The node is hidden in every render view because the two predicates are conjoined and the `KHR_node_visibility` predicate is false.

The following partial fragment omits mesh, accessor, and buffer definitions. It demonstrates that an explicit `role: "always"` cannot cancel an ancestor hint restriction. It also includes an ancestor whose `KHR_node_visibility.visible: false` hides its subtree in every view.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": [
    "KHR_node_visibility_hint",
    "KHR_node_visibility"
  ],
  "extensionsRequired": [
    "KHR_node_visibility_hint",
    "KHR_node_visibility"
  ],
  "nodes": [
    {
      "name": "HintHiddenParent",
      "mesh": 0,
      "children": [1],
      "extensions": {
        "KHR_node_visibility_hint": { "role": "third_person" }
      }
    },
    {
      "name": "AlwaysChild",
      "mesh": 1,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "always" }
      }
    },
    {
      "name": "HiddenParent",
      "mesh": 2,
      "children": [3],
      "extensions": {
        "KHR_node_visibility_hint": { "role": "third_person" },
        "KHR_node_visibility": { "visible": false }
      }
    },
    {
      "name": "AlwaysChildOfHiddenParent",
      "mesh": 3,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "always" },
        "KHR_node_visibility": { "visible": true }
      }
    }
  ]
}
```

In a render view tagged only `"first_person"`, visual content attached to `HintHiddenParent` and `AlwaysChild` is hint-hidden because the parent's `"third_person"` predicate is false. The child's `"always"` role adds no restriction and cannot restore that content. In a view tagged `"third_person"`, the first pair is hint-visible.

`HiddenParent` and `AlwaysChildOfHiddenParent` remain hidden in every view because `visible: false` on `HiddenParent` hides the entire subtree under `KHR_node_visibility` semantics. The child's explicit `visible: true` does not change this outcome, because `coreVisible(n)` is false whenever any ancestor of `n` has `visible: false`.

### Visibility cases

The following cases assume no other ancestor hints and a `coreVisible` result of `true`:

| Parent hint | Child hint | View tags | Child hint-visible |
| --- | --- | --- | --- |
| None | None | None | Yes |
| `always` | `always` | None | Yes |
| `third_person` | None | `first_person` | No |
| `third_person` | `always` | `first_person` | No |
| `third_person` | `always` | `third_person` | Yes |
| `always` | `third_person` | `first_person` | No |
| `third_person` | `first_person` | `first_person` | No |
| `third_person` | `first_person` | `third_person` | No |
| `third_person` | `first_person` | `first_person`, `third_person` | Yes |

These annotations remain valid even when a particular view's tags make their combined predicate false.

## Validation boundary

The JSON Schema enforces only the extension object's local structure: `role` is required and nonempty, and `label` is nonempty when present.

A validator that claims to validate this extension **MUST** verify placement on a node and the ordinary `extensionsUsed` and `extensionsRequired` declarations. Custom roles remain structurally valid; validation of separately specified custom semantics is outside this extension.

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and first-/third-person visibility demo.

## Known limitations (Informative)

- The host chooses which view tags each render view has and when they change.
- The standard predicate is binary; fades and transitions are host-defined.
- Custom-role behavior is portable only through a separately supported specification.
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
