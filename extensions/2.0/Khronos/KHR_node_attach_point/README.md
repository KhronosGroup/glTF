# KHR\_node\_attach\_point

## Contributors

- Ken Jakubzak, Meta
- *[Additional contributors TBD]*

## Status

Draft

## Dependencies

Written against the glTF 2.0 specification.

No other extension dependencies are required. This extension is fully self-contained.

## Overview

### The Problem

Real-time 3D applications routinely attach content to characters and objects at runtime: weapons grip to hands, hats sit atop heads, backpacks mount to spines, earrings dangle from ears, holstered pistols rest on hips. Every game engine, avatar platform, and XR framework solves this the same way -- an empty transform node positioned as a child of a skeleton bone, offset to the correct mounting location.

The glTF 2.0 core specification provides no mechanism for distinguishing these semantic mount points from any other empty node in the scene hierarchy. An engine loading a glTF file encounters dozens of nodes -- bones, organizational groups, locators, constraints, effects anchors -- with no way to identify which ones are intended as attachment locations. Applications resort to fragile heuristics: naming conventions (`HAT_ATTACH`, `weapon_r_socket`), `extras` metadata with proprietary schemas, or external sidecar files. Every exporter invents a different convention. Cross-platform accessory reuse is effectively zero.

The problem is acute in the avatar ecosystem. Meta Avatars, VRM, and Ready Player Me all define humanoid skeletons but none define standardized attach points. A hat authored for one avatar system cannot attach to another because the mount point name, offset, orientation, and even the concept of "this node is an attach point" differ between every content creator.

### The Solution

`KHR_node_attach_point` marks a glTF node as a semantic attachment location where runtime content (equipment, accessories, props, or other objects) can be mounted. This is the only thing this extension does.

The extension provides:

1. A **semantic name** identifying the attach point's purpose (e.g., `"hand_right"`, `"head_top"`, `"back_center"`).
2. An optional **category** for filtering which types of content may attach (e.g., `"weapon"`, `"headwear"`).

The node's own `translation`, `rotation`, and `scale` properties define the attach point's coordinate frame. The node's position in the scene hierarchy (typically as a child of a skeleton bone) defines the spatial relationship to the parent structure.

### What This Extension Is NOT

- **Not an attachment behavior system.** This extension defines where content *can* attach, not how attachment occurs at runtime. Parenting, snapping, physics constraints, and attachment rules are application logic.
- **Not a constraint system.** Aim constraints, IK targets, and procedural orientation belong to constraint extensions (e.g., `VRMC_node_constraint`).
- **Not a bone definition.** Skeleton bones and humanoid bone mappings are defined by glTF core and extensions like `VRMC_vrm`. An attach point is a child or sibling of a bone, never the bone itself.
- **Not a physics anchor.** Collision shapes, rigid body properties, and joint constraints belong to physics extensions.
- **Not an inventory or slot system.** Equipment slot management, allowed item lists, and inventory logic are application concerns.

### Terminology

| Term | Definition |
|------|-----------|
| **Attach point** | A glTF node annotated with this extension, marking it as a location where runtime content can be mounted. Analogous to Unreal Engine sockets, Godot `BoneAttachment3D` nodes, and Unity empty GameObjects used as mount points. |
| **Attached content** | Any runtime object (mesh, sub-hierarchy, particle system, etc.) that an application parents to an attach point node. Not defined by this extension. |
| **Coordinate frame** | The local space defined by the attach point node's transform. Attached content is positioned relative to this frame. |

---

## RFC 2119

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

---

## Extension Declaration

This extension is declared in the asset-level `extensionsUsed` array. Because this extension provides semantic metadata that does not affect rendering correctness, it SHOULD appear in `extensionsUsed` and SHOULD NOT appear in `extensionsRequired`. An asset without recognized attach points still renders correctly -- it simply lacks mount point functionality.

```json
{
  "extensionsUsed": [
    "KHR_node_attach_point"
  ]
}
```

If an authoring tool determines that attach point semantics are essential for the asset's intended use (e.g., a character model that is meaningless without equipment slots), it MAY list the extension in `extensionsRequired`:

```json
{
  "extensionsUsed": [
    "KHR_node_attach_point"
  ],
  "extensionsRequired": [
    "KHR_node_attach_point"
  ]
}
```

An implementation that does not support this extension but encounters it in `extensionsRequired` MUST reject the asset. If it appears only in `extensionsUsed`, the implementation SHOULD treat annotated nodes as ordinary transform nodes.

---

## Extension Specification

### Extension Placement

The extension data is placed on individual `node` objects within the glTF `nodes` array. Only nodes that serve as attach points carry this extension. An attach point node is typically an empty transform node (no `mesh`, `camera`, or `skin` properties) positioned as a child of a skeleton bone or other structural node, providing a named mounting location with an offset transform from the parent.

An attach point node MAY have a `mesh`, `camera`, or `skin` property if the use case warrants it (e.g., a debug visualization mesh for the attach point), but this is uncommon. The presence of the extension is what marks the node as an attach point, regardless of other node properties.

### Extension Data

When present on a node object, the extension contains the following properties:

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `name` | `string` | **Yes** | -- | Semantic identifier for the attach point. MUST be at least one character. MUST be unique among all `KHR_node_attach_point` names within the same glTF asset. |
| `category` | `string` | No | -- | Filtering category indicating what type of content is intended to attach to this point. MUST be at least one character when present. |

### Property Descriptions

#### `name` (required)

The semantic identifier for this attach point. This is the primary key by which applications discover and reference attach points at runtime. Applications SHOULD use this value for programmatic queries (e.g., "find the attach point named `hand_right`").

Names are free-form strings. This extension does not prescribe a vocabulary. Authors MAY use any name that fits their use case. The Example Attach Point Names section provides common names for humanoid characters, but these are illustrative examples, not a required vocabulary.

The `name` property MUST be unique among all nodes carrying `KHR_node_attach_point` within a single glTF asset. This enables unambiguous lookup by name. If two attach points serve similar purposes but at different locations, they MUST have distinct names (e.g., `hip_left` and `hip_right`, not two nodes both named `hip`).

The `name` property is distinct from the glTF node's own `name` property. The node `name` is a general-purpose display label; the attach point `name` is the semantic identifier used for content attachment. The two values MAY differ. For example, a node named `"WeaponGrip_R"` might have an attach point name of `"hand_right"`.

#### `category` (optional)

A filtering string indicating what type of content is intended for this attach point. Applications MAY use this value to restrict which items can attach to which points. For example, an application might prevent a `"weapon"` item from attaching to a `"headwear"` point.

Categories are free-form strings. This extension does not prescribe a vocabulary. Authors SHOULD use descriptive, lowercase category names. Multiple attach points MAY share the same category (e.g., both `hand_left` and `hand_right` might have category `"weapon"`).

When `category` is omitted, the attach point has no category restriction. Applications SHOULD treat uncategorized attach points as accepting any content.

---

## Symmetry Convention

For attach points that exist on both left and right sides of a model (e.g., hands, ears, shoulders), authors SHOULD use `_left` and `_right` suffixes to indicate symmetry. Applications that need symmetry information can infer pairing from names following this convention (e.g., `hand_left` pairs with `hand_right`, `ear_left` pairs with `ear_right`).

This approach keeps symmetry implicit in the naming convention rather than requiring explicit cross-references between nodes.

---

## Example Attach Point Names

The following names are provided as **examples** for humanoid character and avatar models. These are illustrative, not prescriptive. Authors MAY use any names that fit their use case. Custom names are expected and normal for domain-specific applications (vehicles, furniture, architectural products, robots, fantasy creatures, etc.).

| Name | Description | Typical Parent Bone |
|------|-------------|---------------------|
| `head_top` | Top of head (hats, helmets, crowns) | head |
| `head_face` | Face front (masks, face paint, face shields) | head |
| `nose_bridge` | Nose bridge (glasses, goggles, spectacles) | head |
| `eye_left` | Left eye region (monocles, AR lenses, eye patches) | head / leftEye |
| `eye_right` | Right eye region | head / rightEye |
| `ear_left` | Left ear (earrings, earbuds, hearing aids) | head |
| `ear_right` | Right ear | head |
| `neck` | Neck (necklaces, collars, scarves, ties) | neck |
| `shoulder_left` | Left shoulder (pauldrons, epaulettes) | leftShoulder |
| `shoulder_right` | Right shoulder | rightShoulder |
| `chest_front` | Chest front (badges, chest armor, brooches) | chest |
| `back_center` | Center back (backpacks, capes, wings, quivers) | chest / spine |
| `hand_left` | Left hand grip (weapons, tools, shields, torches) | leftHand |
| `hand_right` | Right hand grip | rightHand |
| `wrist_left` | Left wrist (watches, bracelets, wrist guards) | leftHand |
| `wrist_right` | Right wrist | rightHand |
| `belt_center` | Belt center (buckles, belt pouches, utility belt) | hips |
| `hip_left` | Left hip (holsters, pouches, side bags) | hips |
| `hip_right` | Right hip | hips |
| `foot_left` | Left foot (boots, ankle bracelets, spurs) | leftFoot |
| `foot_right` | Right foot | rightFoot |

Bone names in the "Typical Parent Bone" column are illustrative references to common humanoid skeleton conventions (VRM, Mixamo, Unity Humanoid). The attach point node is a child of the referenced bone, not the bone itself.

---

## JSON Examples

### Example 1: Simple Weapon Grip

A weapon mounting socket on the right hand.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_attach_point"],
  "nodes": [
    {
      "name": "J_Bip_R_Hand",
      "children": [1],
      "rotation": [0, 0, 0, 1],
      "translation": [0.25, 0.95, 0]
    },
    {
      "name": "WeaponGrip_R",
      "translation": [0.02, -0.01, 0.05],
      "rotation": [0, 0.707, 0, 0.707],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "hand_right",
          "category": "weapon"
        }
      }
    }
  ]
}
```

**Runtime behavior:** An application loading this asset can query for attach points with category `"weapon"` and discover `hand_right`. When the user equips a sword, the application parents the sword model to node 1 (`WeaponGrip_R`). The sword inherits the attach point's rotation, orienting it correctly for the grip.

### Example 2: Hat Attachment

A head-top attach point for headwear.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_attach_point"],
  "nodes": [
    {
      "name": "Head",
      "children": [1],
      "mesh": 0
    },
    {
      "name": "HatSocket",
      "translation": [0, 0.12, 0.02],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "head_top",
          "category": "headwear"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A hat model loaded separately can be parented to the `head_top` attach point. The translation offset `[0, 0.12, 0.02]` positions the hat correctly above the head mesh.

### Example 3: Full Avatar with Multiple Attach Points

A humanoid avatar with attach points for common equipment slots.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_attach_point"],
  "nodes": [
    { "name": "AvatarRoot", "children": [1, 2, 3, 4, 5] },
    { "name": "Skeleton", "skin": 0 },
    {
      "name": "HeadTop_Socket",
      "translation": [0, 1.75, 0],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "head_top",
          "category": "headwear"
        }
      }
    },
    {
      "name": "BackCenter_Socket",
      "translation": [0, 1.35, -0.15],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "back_center",
          "category": "backpack"
        }
      }
    },
    {
      "name": "HandLeft_Socket",
      "translation": [-0.45, 1.0, 0.1],
      "rotation": [0, 0, 0.707, 0.707],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "hand_left",
          "category": "weapon"
        }
      }
    },
    {
      "name": "HandRight_Socket",
      "translation": [0.45, 1.0, 0.1],
      "rotation": [0, 0, -0.707, 0.707],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "hand_right",
          "category": "weapon"
        }
      }
    }
  ]
}
```

**Runtime behavior:** An avatar customization system can enumerate all attach points and present equipment options filtered by category. The user can equip a hat to `head_top`, a backpack to `back_center`, and weapons to `hand_left` and `hand_right`.

### Example 4: Non-Humanoid (Vehicle)

A vehicle model with cargo and weapon mount attach points.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_attach_point"],
  "nodes": [
    { "name": "VehicleRoot", "children": [1, 2, 3], "mesh": 0 },
    {
      "name": "TurretMount",
      "translation": [0, 1.5, -0.5],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "turret_primary",
          "category": "weapon_mount"
        }
      }
    },
    {
      "name": "CargoRack_Left",
      "translation": [-1.2, 0.8, 0],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "cargo_left",
          "category": "cargo"
        }
      }
    },
    {
      "name": "CargoRack_Right",
      "translation": [1.2, 0.8, 0],
      "extensions": {
        "KHR_node_attach_point": {
          "name": "cargo_right",
          "category": "cargo"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A game can query for `category: "weapon_mount"` to find where turrets can be installed, and `category: "cargo"` to find storage locations.

---

## Conformance

### Authoring Requirements

1. The `name` property MUST be present and MUST be a non-empty string.
2. The `name` property MUST be unique among all `KHR_node_attach_point` extensions within the same glTF asset.
3. If `category` is present, it MUST be a non-empty string.
4. Attach point nodes SHOULD have a scale of `[1, 1, 1]`. Implementations MAY ignore scale on attach point nodes.

### Runtime Requirements

1. Implementations that support this extension SHOULD expose attach point data to application logic for runtime attachment operations.
2. Implementations SHOULD allow querying attach points by name and by category.
3. Attach point nodes SHOULD NOT be rendered as visible geometry unless they explicitly contain a mesh. They are metadata nodes.
4. Implementations that do not support this extension SHOULD treat annotated nodes as ordinary transform nodes. No visual content is affected.

### Fallback Behavior

If an implementation does not support `KHR_node_attach_point` and the extension appears only in `extensionsUsed` (not `extensionsRequired`), the implementation SHOULD render the model normally. The visual content of the asset is unaffected -- only the semantic attach point metadata is lost.

---

## Schema

### node.KHR\_node\_attach\_point

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "node.KHR_node_attach_point.schema.json",
  "title": "KHR_node_attach_point glTF Node Extension",
  "description": "Marks a node as a semantic attachment location where runtime content can be mounted.",
  "type": "object",
  "properties": {
    "name": {
      "type": "string",
      "description": "Semantic identifier for this attach point. Used for programmatic discovery and content attachment.",
      "minLength": 1
    },
    "category": {
      "type": "string",
      "description": "Filtering category indicating what type of content is intended to attach to this point.",
      "minLength": 1
    }
  },
  "required": ["name"],
  "additionalProperties": false
}
```

### Validation Rules (Non-Schema)

The following constraints cannot be expressed in JSON Schema and MUST be enforced by authoring tools or validators:

1. The `name` value MUST be unique among all `KHR_node_attach_point` extensions within the same glTF asset.

---

## Known Limitations

1. **No attachment behavior.** This extension marks *where* content can attach, not *how*. Attachment parenting, physics joints, snapping behavior, and detachment rules are application-level concerns.

2. **Name uniqueness is per-asset, not per-skeleton.** If a glTF asset contains multiple skeletons (e.g., two characters in a scene), each attach point name must still be unique across the entire asset. Authors who need multiple instances of the same logical attach point (e.g., `hand_right` on two different characters) MUST use distinct names (e.g., `character_a_hand_right` and `character_b_hand_right`). This is a trade-off: per-asset uniqueness enables simple name-based lookup at the cost of multi-skeleton ergonomics.

3. **No bounds or sizing metadata.** The extension does not define the maximum size, volume, or dimensional constraints of content that may attach to a point. An application cannot determine from the attach point alone whether a particular hat will fit a particular head. Sizing validation is an application concern.

4. **Category values are not standardized.** Categories are free-form strings with no enforced vocabulary. An asset using category `"weapon"` and another using `"weapons"` will not match in category-based queries. Authoring tools SHOULD suggest consistent category names, but the specification does not mandate a closed set.

5. **No attachment event system.** The extension provides no mechanism for triggering events, animations, or state changes when content is attached or detached. Attach/detach events are a runtime concern.

6. **Fragile under hierarchy editing.** Reparenting or removing skeleton bones invalidates attach point spatial relationships. Authoring tools MUST update attach point node positions when modifying the skeleton hierarchy. This is consistent with how glTF handles all parent-child spatial relationships.

---

## License

This extension specification is licensed under the Khronos Group Extension License.
See: <https://www.khronos.org/registry/gltf/license.html>
