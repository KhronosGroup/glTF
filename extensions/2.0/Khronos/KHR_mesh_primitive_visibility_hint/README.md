# KHR_mesh_primitive_visibility_hint

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

This extension has no extension dependencies. It composes with [`KHR_node_visibility_hint`](../KHR_node_visibility_hint/README.md) and [`KHR_node_visibility`](../KHR_node_visibility/README.md) when the interacting extensions are supported, but does not require them.

## Overview (Informative)

Some assets combine geometry with different visibility roles into separate primitives of one mesh. This extension associates a mesh primitive with a visibility role so that its visual contribution can be evaluated independently for each render view, based on the view tags the host gives that view.

The role defines a pure per-view predicate. Evaluating it does not alter glTF-authored or animated material, primitive, mesh, or node data; tag views; create or activate a camera; or define transitions when a view's tags change.

## Extension usage

The extension object **MUST** be attached to an object in a mesh's `primitives` array. An asset using this extension **MUST** list `KHR_mesh_primitive_visibility_hint` in `extensionsUsed`.

An asset **MAY** list `KHR_mesh_primitive_visibility_hint` in `extensionsRequired`. A consumer claiming support for this extension **MUST** evaluate standard roles and omit hidden primitive instances from visual rendering as specified. A consumer that cannot provide that behavior **MUST** treat an asset listing `KHR_mesh_primitive_visibility_hint` in `extensionsRequired` as unsupported according to the glTF 2.0 extension rules.

If the extension appears only in `extensionsUsed`, an implementation that does not support it ignores the hint and renders the asset as core glTF and the other extensions it supports define, so the hint suppresses no primitive.

### Extension object

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `role` | string | Yes | — | Nonempty visibility role. Standard values are defined below. |
| `label` | string | No | — | Nonempty human-readable label, preferred over `role` when a host or tool presents the hint to a user. It has no effect on visibility. |

When a host or tool presents a hint to a user, it **SHOULD** identify the hint by `label` when present, otherwise by its `role`.

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

Custom view tags in `T` do not affect these results. For an individual standard role predicate in a render view without view tags, only `"always"` returns `true`. A supported custom role returns `true` there only where the specification defining that role says. The containing node's ancestor-inclusive visibility can further restrict the final result.

Every consumer that supports this extension supports the three standard roles. A consumer supports a custom role only as described under [Custom roles](#custom-roles). A consumer that supports this extension **SHOULD NOT** render a primitive whose role it does not support (the role-support rule); whether to render such a primitive anyway remains the application's decision.

An unannotated primitive is equivalent to `role: "always"`. A differently cased token such as `"First_Person"` is not a standard role but a custom role.

### Authoring guidance (Informative)

Because a render view can have no view tags, and a primitive with a standard role other than `"always"` is visible only in views with the matching tag, authors are responsible for assigning roles so that primitives appear where intended. For example, a primitive intended for every view uses `"always"`, which is also the role of an unannotated primitive.

## Primitive-instance composition

Let `primitiveHintVisible(p, T)` be the standard role predicate for the role of primitive `p` and the view tags `T` of the render view, when that role is a standard role. For a supported custom role, this value is the visibility that the separate specification defining the role gives for that render view. For a role the consumer does not support, this value is `false` unless the application decides to render the primitive anyway, as described under [Standard role predicate](#standard-role-predicate). If `p` has no hint, this value is `true`.

For an instance of primitive `p` through containing node `n`, let `nodeHintVisible(n, T)` be the ancestor-inclusive predicate `hintVisible(n, T)` from `KHR_node_visibility_hint` when that extension is present and supported; otherwise it is `true`. This includes every node hint on `n` and its ancestors in the same evaluated node-instance hierarchy. Let `coreVisible(n)` be the ancestor-inclusive predicate from `KHR_node_visibility` when that extension is present and supported; otherwise it is `true`.

The primitive role is shared by every node referencing the mesh. For each primitive instance and render view, the visual-rendering predicate uses the containing node instance `n` and its view tags `T`:

```text
renderPrimitiveInstance(n, p, T) =
    primitiveHintVisible(p, T)
    AND nodeHintVisible(n, T)
    AND coreVisible(n)
```

This predicate determines whether the primitive instance is drawn directly in that view. Rendering techniques and effects on shadows, reflections, and lighting are host-defined. `KHR_node_visibility` retains its own requirements.

> Implementation Note (Informative): Visibility can differ between views or instances, including instances that share a mesh. Shared resources and temporary rendering state need to accommodate those differences.

A primitive hint cannot make a primitive visible when its containing node is hidden, including when an ancestor's node hint makes `nodeHintVisible(n, T)` false. An `"always"` primitive imposes no additional primitive restriction; it cannot restore content hidden by a node hint. A visible containing node does not override a hidden primitive predicate.

Because `coreVisible(n)` is a logical AND over the containing node and all its ancestors, an invisible node higher in the hierarchy always takes priority: a `visible: false` at any level hides that node and its entire subtree, and no `visible: true` lower down can restore it. The following cases summarize this interaction for a primitive instance whose containing node has an ancestor, independent of any hint role:

| Ancestor `visible` | Containing node `visible` | Result |
|--------------------|---------------------------|--------|
| `true`  | `false` | Primitive instance hidden. |
| `false` | `true`  | Ancestor, containing node, and primitive instance hidden. |
| `false` | `false` | Ancestor, containing node, and primitive instance hidden. |
| `true`  | `true`  | Primitive instance visible, subject to the node and primitive hint predicates. |

This mirrors the normative semantics of `KHR_node_visibility`; it is restated here only to make the composition explicit and does not modify that extension.

When interacting extensions are used, the asset **MUST** list each one in `extensionsUsed`. Combined use does not change the rule for listing `KHR_mesh_primitive_visibility_hint` in `extensionsRequired` given under [Extension usage](#extension-usage); whether `KHR_node_visibility_hint` or `KHR_node_visibility` is listed there follows that extension's specification and the glTF 2.0 extension rules.

## Custom roles

A custom role is any nonempty role string other than the three standard values. A consumer supports a custom role only when it implements a separate specification or application profile that defines that role; every other custom role is unsupported by that consumer, and a primitive with that role falls under the role-support rule in [Standard role predicate](#standard-role-predicate).

Custom roles **SHOULD** be vendor-qualified, for example `"ACME_mirror_only"`, to reduce collisions. Their meaning comes from the separate specification or application profile that defines them and is not portable under this extension alone.

Custom-role semantics come only from a separate specification, such as a vendor extension or application profile. A primitive with a supported custom role is visible where that specification says, typically in render views with a custom view tag that it defines. That specification defines its own association with role tokens, data placement, behavior, and support contract; this extension does not prescribe a companion object or naming relationship. Any glTF extension used for that purpose **MUST** follow the ordinary `extensionsUsed` and `extensionsRequired` rules. Without separately supported semantics, the role is unsupported and the role-support rule applies.

### Custom-role authoring guidance (Informative)

Because a consumer that supports this extension is recommended not to render a primitive whose role it does not support, authors can reserve custom roles for primitives whose absence is acceptable in applications that do not support the role.

For example, a VR application can tag its mirror views `"third_person"` and `"ACME_mirror"`, a custom view tag defined by its application profile. If that profile also defines a custom role `"ACME_mirror_only"` that is visible only in views tagged `"ACME_mirror"`, an avatar can give a mirror-only detail primitive that role. The detail then appears in the application's mirrors but not in its other third-person views, and an application without that profile does not support the role and is recommended not to render the detail.

## Examples (Informative)

The following partial fragment omits accessors and buffers. It marks a head primitive as visible only in views tagged `"third_person"`.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_mesh_primitive_visibility_hint"],
  "nodes": [
    { "mesh": 0 }
  ],
  "meshes": [
    {
      "name": "AvatarBody",
      "primitives": [
        { "attributes": { "POSITION": 0 } },
        {
          "attributes": { "POSITION": 1 },
          "extensions": {
            "KHR_mesh_primitive_visibility_hint": {
              "role": "third_person",
              "label": "Head"
            }
          }
        }
      ]
    }
  ]
}
```

In a render view tagged `"third_person"`, both primitives are hint-visible. In a view without that tag, such as a view tagged only `"first_person"` or a view without tags, the second primitive is omitted from visual rendering.

The following partial fragment omits mesh attributes, accessors, and buffers. It demonstrates node-plus-primitive composition.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": [
    "KHR_node_visibility_hint",
    "KHR_mesh_primitive_visibility_hint"
  ],
  "nodes": [
    {
      "mesh": 0,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "first_person" }
      }
    }
  ],
  "meshes": [
    {
      "primitives": [
        {
          "extensions": {
            "KHR_mesh_primitive_visibility_hint": {
              "role": "third_person"
            }
          }
        }
      ]
    }
  ]
}
```

In a consumer that supports both hints, the node predicate is true only in views tagged `"first_person"` and the primitive predicate only in views tagged `"third_person"`, so the primitive instance is visible only in a view that carries both tags and is hidden in every other view. A consumer that supports this extension but not `KHR_node_visibility_hint` treats the node predicate as `true`, so it shows the primitive instance in every view tagged `"third_person"`.

### Ancestor hint example

Consider this node hierarchy, with the primitive inside `DetailMesh`:

```text
ThirdPersonParent (node hint: third_person)
  -> DetailMesh (node hint: always)
       -> primitive (primitive hint: always)
```

In a view tagged only `"first_person"`, the primitive's own predicate is true, but its containing node's ancestor-inclusive hint result is false because of `ThirdPersonParent`. The primitive is hidden. In a view tagged `"third_person"`, both hint predicates are true and the primitive is visible when `coreVisible` is true. A `visible: false` on either node hides the primitive in every view when `KHR_node_visibility` is supported.

If `DetailMesh` instead has a `"first_person"` node hint, the primitive is hint-visible only in a view carrying both `"first_person"` and `"third_person"` tags. A descendant node hint and a primitive hint can each add a restriction; neither removes an ancestor node-hint restriction. The primitive hint remains non-inherited.

## Validation boundary

The JSON Schema enforces only the extension object's local structure: `role` is required and nonempty, and `label` is nonempty when present.

A validator that claims to validate this extension **MUST** verify placement on a mesh primitive and the ordinary `extensionsUsed` and `extensionsRequired` declarations. Custom roles remain structurally valid; validation of separately specified custom semantics is outside this extension.

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and primitive-visibility demo.

## Known limitations (Informative)

- Per-primitive visual suppression may require splitting draw submissions in engines that otherwise batch mesh primitives.
- The host chooses which view tags each render view has and when they change.
- The standard predicate is binary; fades and transitions are host-defined.
- Custom-role behavior is portable only through a separately supported specification.

## Schema

- [mesh.primitive.KHR_mesh_primitive_visibility_hint.schema.json](./schema/mesh.primitive.KHR_mesh_primitive_visibility_hint.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
