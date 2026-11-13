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

Some assets combine content with different visibility needs in separate primitives of one mesh, for example a character's head and body, or a vehicle's cockpit interior and canopy exterior. This extension lets an author give a mesh primitive one or more visibility roles, which say when the primitive is meant to be visible. A primitive hint applies to that primitive only and is not inherited.

## Extension usage

The extension object **MUST** be attached to an object in a mesh's `primitives` array. An asset using this extension **MUST** list `KHR_mesh_primitive_visibility_hint` in `extensionsUsed` and **MUST NOT** list it in `extensionsRequired`. Except as recommended in [Recommended rendering](#recommended-rendering) and [Label](#label), a host **MAY** use, adapt, or ignore hints. An implementation that does not support this extension ignores hints, so they suppress no primitive there.

### Extension object

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `roles` | string[] | Yes | One or more distinct nonempty visibility roles. Their order has no meaning. |
| `label` | string | No | Nonempty human-readable label, preferred over the roles when a host or tool presents the hint to a user. It has no effect on visibility. |

## Roles

Roles, render views, and the *subject* have the meanings that [`KHR_node_visibility_hint`](../KHR_node_visibility_hint/README.md#roles) defines. In summary, a role names when the author intends content to be visible, role strings are exact and case-sensitive, and for each render view and instance of the asset the application's logic determines which roles apply; several can apply at once. Roles apply to an asset instance as a whole: this extension does not identify subjects within an asset, and representation of multiple subjects within a single asset is outside its scope.

`"always"` applies in every render view. `"first_person"` and `"third_person"` apply where the application's logic determines. Where the application has no logic for a standard role, that role applies in every render view, so an application with no logic hides nothing.

Any other nonempty string is a custom role. Custom roles **SHOULD** be vendor-qualified. A custom role applies where the application or the application profile that defines it says. A host ignores custom roles it does not recognize.

A hint allows a render view when at least one of its recognized roles applies there. A hint whose roles include `"always"`, or that has no recognized role, imposes no restriction.

## Primitive-instance composition

For a render view and an instance of primitive `p` through containing node `n`, let `T` be the set of roles that apply in that view for that instance. Let `primitiveHintVisible(p, T)` be `true` when `p` has no hint, and otherwise whether the hint on `p` allows that view. The hint is shared by every node that references the mesh.

Also, let `nodeHintVisible(n, T)` be the ancestor-inclusive predicate `hintVisible(n, T)` from `KHR_node_visibility_hint` when that extension is present and supported; otherwise it is `true`. Let `coreVisible(n)` be the ancestor-inclusive predicate from `KHR_node_visibility` when that extension is present and supported; otherwise it is `true`.

```text
renderPrimitiveInstance(n, p, T) =
    primitiveHintVisible(p, T)
    AND nodeHintVisible(n, T)
    AND coreVisible(n)
```

A primitive hint adds a restriction to the node hints and cannot restore a primitive that they or `KHR_node_visibility` hide, including when its roles include `"always"`. A visible containing node does not override a hidden primitive predicate.

## Recommended rendering

An application that applies `"first_person"` **SHOULD** apply `"third_person"` wherever `"first_person"` does not apply.

A host **SHOULD NOT** directly draw a primitive instance in a render view where `renderPrimitiveInstance` is `false` for that instance, and **SHOULD NOT** omit it because of a hint where that predicate is `true`. A host **MAY** depart from this section on an explicit user choice, such as an editing or diagnostic view that shows all content.

Rendering techniques and effects on shadows, reflections, and lighting are host-defined. The predicate is evaluated independently for each render view and primitive instance. Applying it does not change glTF-authored or animated material, primitive, mesh, or node data.

> Implementation Note (Informative): Visibility can differ between views or instances, including instances that share a mesh. Per-primitive omission can require splitting draw submissions in engines that batch primitives.

## Label

When a host or tool presents a hint to a user, it **SHOULD** identify the hint by `label` when present, otherwise by its roles.

## Example (Informative)

The following partial fragment omits accessors and buffers. It gives a head primitive the `"third_person"` role and a first-person arms primitive the `"first_person"` role.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_mesh_primitive_visibility_hint"],
  "nodes": [{ "mesh": 0 }],
  "meshes": [{
    "primitives": [
      { "attributes": { "POSITION": 0 } },
      { "attributes": { "POSITION": 1 },
        "extensions": { "KHR_mesh_primitive_visibility_hint": { "roles": ["third_person"], "label": "Head" } } },
      { "attributes": { "POSITION": 2 },
        "extensions": { "KHR_mesh_primitive_visibility_hint": { "roles": ["first_person"], "label": "First-person arms" } } }
    ]
  }]
}
```

| Primitive | `first_person` applies | `third_person` applies | Host with no logic, such as an inspector |
|-----------|------------------------|------------------------|------------------------------------------|
| Body | Drawn | Drawn | Drawn |
| Head | Not drawn | Drawn | Drawn |
| First-person arms | Drawn | Not drawn | Drawn |

The same pattern fits a vehicle mesh whose cockpit-interior primitive has `roles: ["first_person"]` and whose canopy-exterior primitive has `roles: ["third_person"]`.

## Validation boundary

The JSON Schema enforces only the extension object's local structure: `roles` is a required nonempty array of distinct nonempty strings, and `label` is nonempty when present.

A validator **MUST** verify placement on an object in a mesh's `primitives` array, the `extensionsUsed` declaration, and absence from `extensionsRequired`. Custom roles are valid. A validator **MAY** report, without treating the asset as invalid, `roles` that list `"always"` with other roles. Validators do not check host rendering behavior.

## Known implementations (Informative)

- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and primitive-visibility demo.

## Known limitations (Informative)

- Per-primitive visual suppression may require splitting draw submissions in engines that otherwise batch mesh primitives.
- The application decides which roles apply in each render view, and when that changes.
- The predicate is binary; fades and transitions are host-defined.
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
