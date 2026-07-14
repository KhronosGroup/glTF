# KHR\_mesh\_primitive\_visibility\_hint

## Contributors

- Ken Jakubzak, Meta
- 0b5vr / pixiv Inc.
- Aaron Franke, Independent Contributor
- Hideaki Eguchi / VirtualCast, Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta

## Status

Draft

## Dependencies

Written against the glTF 2.0 specification.

**Related:** [`KHR_node_visibility_hint`](../KHR_node_visibility_hint/README.md) provides the same view-context vocabulary at **node** granularity. This extension is the **primitive**-granularity counterpart. They are deliberately separate extensions so that support for each can be declared and required independently (see [Extension Declaration](#extension-declaration) and [Relationship to KHR_node_visibility_hint](#relationship-to-khr_node_visibility_hint)).

## Overview

### The Problem

Node-level visibility (`KHR_node_visibility_hint`, `KHR_node_visibility`) can hide a whole node's subtree — ideal when a view-context-specific region (a head, hair, a hat) is authored as its own node. But geometry is frequently **baked into a single mesh**: a body mesh whose head is one of its `primitives`, or a merged mesh optimized for draw-call count. In that case there is no node to hide, and node-level visibility cannot express "hide only the head part in first-person."

There is no portable way in glTF 2.0 to say "this *primitive* is only visible in this view context." The intent is either lost or hardcoded per application.

### The Solution

`KHR_mesh_primitive_visibility_hint` annotates an individual mesh **primitive** with a semantic **`role`** describing the view context(s) in which it renders — using the same vocabulary as `KHR_node_visibility_hint` and the same view-context tokens (`first_person`, `third_person`) as `KHR_node_camera_hint`. A runtime resolves the active view context and skips rendering primitives whose role does not match.

Primitive visibility hints are **advisory**, but note that **not every engine or runtime supports per-primitive culling**. Because this is a distinct extension, an author whose model depends on primitive-level hiding can list it in `extensionsRequired`, and a runtime that cannot honor it will reject the asset rather than silently rendering hidden geometry (e.g. the inside of a head in first-person).

### What This Extension Is NOT

- **Not node visibility.** For whole-node/subtree visibility use `KHR_node_visibility_hint` (view context) or `KHR_node_visibility` (on/off). This extension exists specifically for geometry that cannot be separated by node.
- **Not a material variant.** It does not swap materials or geometry; it includes or excludes a primitive from rendering per view context. (`KHR_materials_variants` addresses material swapping.)
- **Not an LOD or culling-distance system.**

### Terminology

| Term | Definition |
|------|-----------|
| **Primitive visibility hint** | A glTF mesh primitive annotated with this extension, declaring the view context(s) in which the primitive renders. |
| **View context** | The active viewing mode a runtime presents, e.g. `first_person` or `third_person`. Shared tokens align with `KHR_node_camera_hint` and `KHR_node_visibility_hint`. |
| **Role** | The semantic view-context membership of a primitive, expressed as a string. |

---

## RFC 2119

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

---

## Extension Declaration

Declared in the asset-level `extensionsUsed` array.

```json
{
  "extensionsUsed": ["KHR_mesh_primitive_visibility_hint"]
}
```

Because per-primitive culling is not universally supported, authoring intent determines placement in `extensionsRequired`:

- If the model still reads acceptably when hidden primitives are shown (the annotation is a refinement), keep it in `extensionsUsed` only — unsupporting runtimes render every primitive and lose only the view-context culling.
- If correct presentation **depends** on hiding a primitive (e.g. a first-person camera would otherwise render the inside of a head baked into the body mesh), the author SHOULD list this extension in `extensionsRequired`. Runtimes that do not support it MUST then reject the asset.

This is the core reason primitive visibility is a separate extension from node visibility: the two capabilities can be advertised and required independently.

---

## Extension Specification

### Extension Placement

The extension data is placed on individual objects within a mesh's `primitives` array (`meshes[*].primitives[*].extensions`).

### Extension Data

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `role` | `string` | **Yes** | -- | The view context in which this primitive is visible. See Standard Role Vocabulary and Custom Roles. |
| `label` | `string` | No | -- | Human-readable label for authoring tools and inspectors. MUST be non-empty when present. |

### No Inheritance

Unlike `KHR_node_visibility_hint`, this annotation applies to the **single primitive** it is attached to. Primitives are leaves; there is no subtree to inherit the role. Each primitive that needs a non-`always` context must be annotated individually.

### Resolving Visibility

Given the runtime's active view context `C`, a primitive is rendered when:

- `always` → in all contexts (equivalent to no annotation).
- `first_person` → only when `C` is `first_person`.
- `third_person` → only when `C` is `third_person`.
- *custom* → runtime-defined; unrecognized roles SHOULD be treated as `always`.

Unlike the node extension, this extension does not build on `KHR_node_visibility` — there is no core per-primitive visibility state to drive. A runtime realizes the hint by directly including or excluding the primitive from the draw for the active context. If the primitive's containing node is hidden (by `KHR_node_visibility` or `KHR_node_visibility_hint`), the primitive is hidden regardless of its own role (node-level hiding takes precedence).

### Implementation Note: Hiding a Primitive in Game Engines

Many real-time engines batch a mesh's primitives (sub-meshes) into a single renderer and cannot cheaply exclude one primitive from the draw; disabling the whole renderer would incorrectly hide the mesh's other primitives as well. For these engines it is RECOMMENDED to realize a hidden primitive by swapping the non-matching sub-mesh to a fully transparent **invisible material/shader** (for example, a transparent material with alpha `0` and depth writes disabled), and restoring the original material when the active view context matches the primitive's `role` again. This removes only the hinted primitive from the visible result while leaving the rest of the mesh intact. Engines that *can* cull an individual primitive MAY instead skip it in the draw directly. Both are valid, non-normative realizations of this advisory hint; the choice is an engine-specific concern.

---

## Standard Role Vocabulary

| Role | Description |
|------|-------------|
| `"always"` | Visible in all view contexts. Equivalent to omitting the extension; used to state intent explicitly. |
| `"first_person"` | Visible only in the first-person view context. Example: a first-person-only hands primitive inside a shared arm mesh. |
| `"third_person"` | Hidden in first-person, visible otherwise. Example: a head primitive baked into a body mesh that would occlude an inside-the-head first-person camera. |

---

## Custom Roles

The `role` property is a free-form string. Custom roles SHOULD be lowercase-with-underscores and vendor-prefixed (`VENDORNAME_role_name`, e.g. `ACME_mirror_only`). Runtimes that encounter an unrecognized role SHOULD treat the primitive as `always` (always visible).

---

## Relationship to KHR_node_visibility_hint

The two extensions share vocabulary and semantics but differ in granularity and capability:

| | `KHR_node_visibility_hint` | `KHR_mesh_primitive_visibility_hint` |
|--|----------------------------|--------------------------------------|
| **Placement** | `node` | `mesh.primitive` |
| **Scope** | The node and its subtree (inherited) | The single primitive (no inheritance) |
| **Builds on** | `KHR_node_visibility` (`visible` state) | Nothing — the runtime culls the primitive directly |
| **When to use** | The view-context region is (or can be) its own node | The region is baked into a shared mesh and cannot be split by node |
| **Support** | Widely feasible (node hide/show) | Requires per-primitive culling, not universal |

**Prefer node granularity when possible** — it is simpler and more widely supported. Reach for primitive granularity only when the geometry cannot be separated by node. A single asset MAY use both; because they are separate extensions, a runtime and an author can reason about, advertise, and require each independently — giving a complete picture of the visibility mechanisms a model needs.

When a node is hidden by a node-level mechanism, its primitives are hidden regardless of their own primitive-level roles (node-level hiding wins).

---

## JSON Example

A single body mesh whose head is a separate primitive that must be hidden in first-person.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_mesh_primitive_visibility_hint"],
  "meshes": [
    {
      "name": "AvatarBody",
      "primitives": [
        { "attributes": { "POSITION": 0 }, "material": 0 },
        {
          "attributes": { "POSITION": 1 },
          "material": 1,
          "extensions": {
            "KHR_mesh_primitive_visibility_hint": {
              "role": "third_person",
              "label": "Head (baked into body mesh)"
            }
          }
        }
      ]
    }
  ]
}
```

**Runtime behavior:** In third-person both primitives render. In first-person the runtime skips the second (head) primitive, so the camera inside the head does not render its interior. The first primitive (no annotation) is treated as `always`. If a runtime does not support primitive culling and the extension is in `extensionsRequired`, it rejects the asset instead of showing the head.

---

## Conformance

### Authoring Requirements

1. `role` MUST be present and MUST be a non-empty string.
2. `label`, when present, SHOULD be a concise, human-readable string.
3. Annotating a primitive with `always` is permitted and equivalent to omitting the extension.
4. If correct presentation depends on hiding a primitive in some context, the author SHOULD list this extension in `extensionsRequired`.
5. Authors SHOULD prefer node-level visibility (`KHR_node_visibility_hint`) when the region can be a separate node, using this extension only for baked-in geometry.

### Runtime Requirements

1. Implementations that support this extension SHOULD render each annotated primitive only when the active view context matches its (self-only) `role`.
2. Implementations SHOULD treat unrecognized `role` values as `always`.
3. If the containing node is hidden by a node-level mechanism, the primitive MUST NOT render regardless of its role.
4. Implementations that do not support this extension SHOULD render all primitives (treat each as `always`). If the extension is required and unsupported, the asset MUST be rejected.

### Fallback Behavior

If the extension appears only in `extensionsUsed`, an unsupporting implementation renders every primitive normally — only view-context culling of primitives is lost. Authors who cannot tolerate that loss MUST use `extensionsRequired`.

---

## Schema

- [mesh.primitive.KHR_mesh_primitive_visibility_hint.schema.json](./schema/mesh.primitive.KHR_mesh_primitive_visibility_hint.schema.json)

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "mesh.primitive.KHR_mesh_primitive_visibility_hint.schema.json",
  "title": "KHR_mesh_primitive_visibility_hint glTF Mesh Primitive Extension",
  "type": "object",
  "properties": {
    "role": { "type": "string", "minLength": 1 },
    "label": { "type": "string", "minLength": 1 }
  },
  "required": ["role"],
  "additionalProperties": false
}
```

---

## Known Limitations

1. **Per-primitive culling is not universal.** Some engines cannot cheaply exclude a single primitive from a batched mesh. This is why the capability is a separate, independently requirable extension.
2. **No inheritance.** Every affected primitive must be annotated individually.
3. **View contexts are runtime-defined.** The `first_person`/`third_person` tokens are shared with `KHR_node_camera_hint`/`KHR_node_visibility_hint`, but how a runtime enters a context is out of scope.
4. **No partial visibility.** A primitive is either drawn or not for a given context; there is no fade.

---

## License

This extension specification is licensed under the Khronos Group Extension License.
See: <https://www.khronos.org/registry/gltf/license.html>
