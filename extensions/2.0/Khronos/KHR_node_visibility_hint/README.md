# KHR\_node\_visibility\_hint

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

**Builds on:** [`KHR_node_visibility`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_node_visibility). This extension is the declarative, view-context layer; its resolved effect is applied through `KHR_node_visibility`'s `visible` semantics (see [Relationship to KHR_node_visibility](#relationship-to-khr_node_visibility)). Implementations that do not support `KHR_node_visibility` MAY still realize the hint by hiding/showing the node directly.

## Overview

### The Problem

Many experiences need a mesh to be visible in one view context but not another. The canonical case is **first-person embodiment**: an avatar's head, hair, or hat should be hidden when the camera is inside the head (first-person / VR), but visible from the outside (third-person). Symmetrically, a mesh may exist **only** for first-person (e.g., detailed gloves seen when you look at your own hands).

glTF 2.0 has no portable way to express this. `KHR_node_visibility` provides a runtime on/off toggle, but not *which view context* a node belongs to. Today this intent lives in vendor formats — most notably VRM's `firstPerson.meshAnnotations` (`both` / `firstPersonOnly` / `thirdPersonOnly` / `auto`) — or is hardcoded in application logic. Loaded in a generic viewer, that intent is lost: the avatar renders the inside of its own head in first-person, or shows meshes that should have been hidden.

### The Solution

`KHR_node_visibility_hint` annotates a node with a semantic **`role`** describing the view context(s) in which it should be visible. It follows the vocabulary and advisory pattern established by [`KHR_node_camera_hint`](#relationship-to-khr_node_camera_hint): the view-context tokens (`first_person`, `third_person`) are shared, so a runtime that activates a `first_person` camera hint knows to hide `third_person` nodes and show `first_person` ones.

Visibility hints are **advisory**. A runtime resolves the active view context, computes each annotated node's visibility, and applies it — building on `KHR_node_visibility` where available. Runtimes MAY ignore hints; the model still renders.

### What This Extension Is NOT

- **Not a runtime on/off toggle.** That is `KHR_node_visibility` (optionally driven by `KHR_interactivity`). This extension declares *view-context membership*, which resolves to that on/off state.
- **Not a camera or viewpoint definition.** Where the first-person/third-person camera *is* comes from `KHR_node_camera_hint` (`role: "first_person"` / `"third_person"`). This extension only says *what renders* in each context.
- **Not a rendering or LOD system.** It does not define fade, cross-dissolve, distance-based culling, or level-of-detail switching.

### Terminology

| Term | Definition |
|------|-----------|
| **Visibility hint** | A glTF node annotated with this extension, declaring the view context(s) in which the node's subtree should render. |
| **View context** | The active viewing mode a runtime is presenting, e.g. `first_person` or `third_person`. The set of contexts and how they are entered is a runtime concern; the shared tokens align with `KHR_node_camera_hint`. |
| **Role** | The semantic view-context membership of a node, expressed as a string. Standard vocabulary roles provide interoperability; custom roles are permitted. |

---

## RFC 2119

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

---

## Extension Declaration

Declared in the asset-level `extensionsUsed` array. It SHOULD NOT appear in `extensionsRequired` in most cases, because visibility hints are presentation guidance that degrade gracefully.

```json
{
  "extensionsUsed": ["KHR_node_visibility_hint"]
}
```

An implementation that does not support this extension SHOULD render all nodes normally (equivalent to every node being `always`). No visual content is lost, though first-person/third-person culling intent is.

---

## Extension Specification

### Extension Placement

The extension data is placed on individual `node` objects within the glTF `nodes` array. It applies to that node and, by inheritance, to its **subtree** (see Inheritance).

### Extension Data

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `role` | `string` | **Yes** | -- | The view context in which this node is visible. See Standard Role Vocabulary and Custom Roles. |
| `label` | `string` | No | -- | Human-readable label for authoring tools and inspectors. MUST be non-empty when present. |

### Inheritance

A visibility hint applies to the annotated node and all of its descendants. A descendant that carries its own `KHR_node_visibility_hint` **overrides** the inherited role for its own subtree. Nodes with no hint (and no annotated ancestor) behave as `always`.

### Resolving Visibility

Given the runtime's active view context `C`, a node's **context-visible** state is:

- `always` → visible in all contexts.
- `first_person` → visible only when `C` is `first_person`.
- `third_person` → visible only when `C` is `third_person`.
- *custom* → runtime-defined; unrecognized roles SHOULD be treated as `always`.

The node is rendered only if it is context-visible **and** not otherwise hidden. When `KHR_node_visibility` is present, the effective result is the logical AND of the context-visible state and `KHR_node_visibility.visible`; i.e. an explicit `visible: false` (e.g. from `KHR_interactivity`) always hides the node regardless of view context.

---

## Standard Role Vocabulary

Runtimes that support this extension SHOULD recognize and correctly interpret all standard vocabulary roles.

| Role | Description |
|------|-------------|
| `"always"` | Visible in all view contexts. Equivalent to the default (no annotation); used to state intent explicitly or to override an inherited role. |
| `"first_person"` | Visible only in the first-person view context (paired with `KHR_node_camera_hint` `role: "first_person"`). Example: first-person-only detailed hands/gloves. |
| `"third_person"` | Hidden in first-person, visible otherwise (paired with `KHR_node_camera_hint` `role: "third_person"`). Example: head, hair, or hat meshes that would occlude an inside-the-head first-person camera. |

---

## Custom Roles

The `role` property is a free-form string; this extension does not restrict its values. The standard vocabulary provides interoperability; custom roles provide domain-specific flexibility at the cost of portability.

- **Lowercase with underscores**, matching the standard style (e.g. `mirror_only`).
- **Vendor prefix** to avoid collisions, formatted `VENDORNAME_role_name` (e.g. `ACME_mirror_only`, `MYAPP_photo_mode_only`).

Runtimes that encounter an unrecognized role SHOULD treat the node as `always` (always visible) rather than rejecting the asset or hiding the node.

---

## Relationship to KHR_node_visibility

`KHR_node_visibility` defines a node's boolean `visible` state (default `true`), the runtime mechanism for hiding a node and its subtree — commonly toggled via `KHR_interactivity`. `KHR_node_visibility_hint` **builds on** it:

- This extension declares, at authoring time, *which view context* a node belongs to.
- A runtime resolves the active view context into a boolean and applies it through `KHR_node_visibility` — conceptually setting the node's `visible` state for the current context.
- The two compose: the rendered result is `context_visible(role, activeContext) && KHR_node_visibility.visible`. An explicit runtime `visible: false` wins; the hint never forces a node visible against an explicit off.

A node MAY carry both extensions: the hint provides the view-context baseline; `KHR_node_visibility` (with interactivity) provides explicit runtime control.

## Relationship to KHR_mesh_primitive_visibility_hint

`KHR_node_visibility_hint` operates at **node** granularity (a node and its subtree). When a view-context-specific region is baked into a shared mesh and cannot be separated by node, use [`KHR_mesh_primitive_visibility_hint`](../KHR_mesh_primitive_visibility_hint/README.md), which applies the same `role` vocabulary to an individual mesh primitive. The two are deliberately separate extensions so that support for each can be advertised and required independently: prefer node granularity where possible (simpler, more widely supported), and reach for primitive granularity only for baked-in geometry. When a node is hidden, its primitives are hidden regardless of any primitive-level role.

## Relationship to KHR_node_camera_hint

The two extensions are complementary and share view-context vocabulary:

- `KHR_node_camera_hint` (`role: "first_person"` / `"third_person"`) defines **where the camera is** for each context.
- `KHR_node_visibility_hint` (`first_person` / `third_person` / `always`) defines **what renders** in each context.

When a runtime activates a `first_person` camera hint, it enters the `first_person` context and hides `third_person` nodes; activating a `third_person` camera hint reveals them again.

## Relationship to VRM firstPerson (VRMC\_vrm)

This extension generalizes VRM 1.0's `firstPerson.meshAnnotations`:

| VRM `meshAnnotations[].type` | `KHR_node_visibility_hint` `role` |
|------------------------------|-----------------------------------|
| `both` | `always` |
| `firstPersonOnly` | `first_person` |
| `thirdPersonOnly` | `third_person` |
| `auto` | *(not generalized; runtime-specific headless-mesh generation is out of scope)* |

A converter MAY translate VRM mesh annotations to this extension by mapping node references to the annotated nodes and the `type` value per the table above.

---

## JSON Examples

### Example 1: Hide the head in first-person

The head subtree is annotated `third_person`, so it renders from the outside but not when the camera is inside the head.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_visibility_hint"],
  "nodes": [
    { "name": "AvatarRoot", "children": [1, 2] },
    { "name": "Body", "mesh": 0 },
    {
      "name": "Head",
      "mesh": 1,
      "extensions": {
        "KHR_node_visibility_hint": {
          "role": "third_person",
          "label": "Head (hidden in first-person)"
        }
      }
    }
  ]
}
```

### Example 2: First-person-only gloves + explicit always

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_visibility_hint"],
  "nodes": [
    { "name": "AvatarRoot", "children": [1, 2, 3] },
    {
      "name": "Body",
      "mesh": 0,
      "extensions": { "KHR_node_visibility_hint": { "role": "always" } }
    },
    {
      "name": "FPGloves",
      "mesh": 1,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "first_person", "label": "First-Person Gloves" }
      }
    },
    {
      "name": "Hair",
      "mesh": 2,
      "extensions": { "KHR_node_visibility_hint": { "role": "third_person" } }
    }
  ]
}
```

### Example 3: Composing with KHR_node_visibility

`Hat` is `third_person` (view-context baseline) and can still be toggled off entirely at runtime via `KHR_node_visibility`.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_visibility_hint", "KHR_node_visibility"],
  "nodes": [
    {
      "name": "Hat",
      "mesh": 0,
      "extensions": {
        "KHR_node_visibility_hint": { "role": "third_person" },
        "KHR_node_visibility": { "visible": true }
      }
    }
  ]
}
```

**Runtime behavior:** In third-person the hat renders (context-visible `true` AND `visible: true`). In first-person it is hidden (context-visible `false`). If gameplay sets `KHR_node_visibility.visible = false`, the hat is hidden in every context.

---

## Conformance

### Authoring Requirements

1. `role` MUST be present and MUST be a non-empty string.
2. `label`, when present, SHOULD be a concise, human-readable string.
3. Annotating a node with `always` is permitted and equivalent to omitting the extension; it MAY be used to state intent or override an inherited role.
4. Authors SHOULD annotate the highest node whose entire subtree shares a view context, relying on inheritance rather than annotating every mesh node.

### Runtime Requirements

1. Implementations SHOULD resolve the active view context and render each node according to its (possibly inherited) `role`.
2. Implementations SHOULD apply the resolved visibility through `KHR_node_visibility` semantics where supported, combining with any explicit `visible` state via logical AND.
3. Implementations SHOULD treat unrecognized `role` values as `always`.
4. Implementations that do not support this extension SHOULD render all nodes (treat every node as `always`).

### Fallback Behavior

If the extension appears only in `extensionsUsed`, an unsupporting implementation renders every node normally. Only first-person/third-person culling intent is lost; the visual content is otherwise complete.

---

## Schema

- [node.KHR_node_visibility_hint.schema.json](./schema/node.KHR_node_visibility_hint.schema.json)

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "node.KHR_node_visibility_hint.schema.json",
  "title": "KHR_node_visibility_hint glTF Node Extension",
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

1. **View contexts are runtime-defined.** This extension standardizes the `first_person`/`third_person` tokens (shared with `KHR_node_camera_hint`) but does not define how a runtime enters a context or how many contexts exist. Custom contexts (e.g. `mirror`) are expressible only as custom roles.
2. **No `auto` equivalent.** VRM's `auto` (runtime headless-mesh generation) is intentionally not generalized; authors should provide explicit meshes annotated `third_person`.
3. **Binary per context.** A node is either visible or not in a context; there is no partial/fade visibility. Transitions are a runtime concern.
4. **Index/hierarchy sensitivity.** Because the role is inherited through the node hierarchy, restructuring the subtree can change which nodes are affected. Authoring tools SHOULD preserve annotations when reparenting.

---

## License

This extension specification is licensed under the Khronos Group Extension License.
See: <https://www.khronos.org/registry/gltf/license.html>
