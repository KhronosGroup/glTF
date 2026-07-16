# KHR\_node\_camera\_hint

## Contributors

- Ken Jakubzak, Meta
- *[Additional contributors TBD]*

## Status

Draft

## Dependencies

Written against the glTF 2.0 specification.

**Required dependencies:** None. This extension depends only on the glTF 2.0 core specification.

## Overview

### The Problem

Every glTF viewer computes initial camera position differently. There is no standardized mechanism for a content creator to express "this is how my model should be viewed."

The result is that the same model looks different in every viewer:

| Viewer | Distance Formula | Default FOV | Elevation Angle |
|--------|-----------------|-------------|-----------------|
| Google model-viewer | 1.5x AABB diagonal | 30deg | 75deg |
| Babylon.js Sandbox | 2.5x bounding sphere radius | 45deg | 60deg |
| Sketchfab | 2x bounding sphere radius | 45deg | 60deg |
| Godot scene viewer | 2x AABB diagonal | 65deg | 30deg |
| Three.js (default) | Manual / geometry center | 50deg | Manual |
| filament gltf\_viewer | 2x AABB diagonal | 30deg | 60deg |

These heuristics work adequately for simple, symmetric objects. They fail for:

- **Asymmetric objects.** A shoe viewed from the side, not the front. The AABB center is not the visual center of interest.
- **Tall thin objects.** A floor lamp -- the bounding box center is midway up the pole, not the lampshade.
- **Characters and avatars.** The face is the point of interest, not the geometric centroid at hip level.
- **Multi-part assemblies.** A furniture arrangement -- the interesting view is not the geometric center of the scene.
- **Objects with large ground planes.** A small figurine on a pedestal gets framed at the pedestal center, not the figurine.

Content creators currently work around this by configuring camera parameters outside the glTF file -- in HTML attributes (model-viewer), platform-specific settings (Sketchfab), or application code. This information is not portable: the same model loaded in a different viewer loses all camera intent.

### The Solution

`KHR_node_camera_hint` allows content creators to embed camera positioning suggestions directly in the glTF asset as annotated scene nodes. Each camera hint is a standard glTF node whose transform defines a spatial position and orientation, annotated with a semantic `role` that tells viewers how to interpret the hint. An optional `camera` reference points to a glTF camera resource, allowing the hint to carry projection parameters and any camera extensions without reinventing them.

Camera hints are **advisory**. Runtimes MAY use them, adapt them, or ignore them. They are presentation suggestions, not rendering mandates.

### What This Extension Is NOT

- **Not a camera definition.** glTF 2.0 already defines cameras in the `cameras` array with perspective and orthographic projection parameters. Camera hints define POSITIONS and FRAMING INTENT. When projection parameters are desired, the hint references an existing glTF camera rather than redefining projection concepts.
- **Not a rendering specification.** Camera hints do not define depth of field, motion blur, lens distortion, post-processing, or any visual effect. They define where to put a camera and what to look at.
- **Not a constraint system.** Camera hints do not specify orbit angle limits, pan boundaries, zoom constraints, or auto-rotation behavior. These are runtime interaction concerns outside the scope of this extension.

### Terminology

| Term | Definition |
|------|-----------|
| **Camera hint** | A glTF node annotated with this extension. The node's world-space transform defines a recommended camera position and/or orientation. The extension data provides semantic context for how the hint should be interpreted. |
| **Role** | The semantic purpose of a camera hint, expressed as a string. Standard vocabulary roles provide interoperability guarantees across implementations. Custom roles are permitted for domain-specific use cases. |
| **Camera reference** | An optional index into the glTF `cameras` array. When present, the referenced camera's projection parameters (FOV, clip planes, type) and any camera extensions apply to this hint as advisory suggestions. |
| **Target node** | An optional reference to another glTF node that the camera should orient toward. When specified, the camera's look-at direction is defined by the vector from the hint node to the target node, rather than by the hint node's rotation. |

---

## RFC 2119

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC 2119](https://www.ietf.org/rfc/rfc2119.txt).

---

## Extension Declaration

This extension is declared in the asset-level `extensionsUsed` array. It SHOULD NOT appear in `extensionsRequired` in most cases, because camera hints are presentation suggestions that do not affect rendering correctness.

```json
{
  "extensionsUsed": [
    "KHR_node_camera_hint"
  ]
}
```

An implementation that does not support this extension SHOULD still render the model normally. The absence of camera hint support results in the viewer falling back to its own camera positioning heuristics -- the standard behavior today. No visual content is lost.

If an asset's correct presentation **depends** on camera positioning (e.g., a product showcase where framing is critical to the experience), the asset MAY list this extension in `extensionsRequired`. In this case, implementations that do not support this extension MUST reject the asset.

---

## Extension Specification

### Extension Placement

The extension data is placed on individual `node` objects within the glTF `nodes` array. Each camera hint node is typically an empty transform node (no `mesh`, `skin`, or `camera` property) whose position and orientation encode the camera placement suggestion.

A single glTF asset MAY contain multiple camera hint nodes. Multiple hints with different roles are expected and common -- for example, a character avatar might include `portrait`, `full_body`, `orbit_center`, and `first_person` hints simultaneously.

### Extension Data

When present on a node object, the extension contains the following properties:

| Property | Type | Required | Default | Description |
|----------|------|----------|---------|-------------|
| `role` | `string` | **Yes** | -- | The semantic purpose of this camera hint. See Standard Role Vocabulary and Custom Roles sections. |
| `camera` | `integer` | No | -- | Index of a glTF camera in the root `cameras` array whose projection parameters and extensions apply to this hint. MUST be a valid index into the glTF `cameras` array. |
| `targetNode` | `integer` | No | -- | Index of a glTF node that this camera should orient toward. MUST be a valid index into the glTF `nodes` array. The camera hint node MUST NOT reference itself. |
| `label` | `string` | No | -- | Human-readable label for this camera position. Intended for UI display in model viewers, camera selection menus, and authoring tools. |

### Property Descriptions

#### `role` (required)

The semantic purpose of this camera hint. This string tells runtimes how to interpret the node's transform.

This extension defines a **standard vocabulary** of role values (see Standard Role Vocabulary) that provide interoperability guarantees across implementations. Runtimes that support this extension SHOULD recognize and correctly interpret all standard vocabulary roles.

Custom roles beyond the standard vocabulary are permitted (see Custom Roles). Applications SHOULD gracefully handle unrecognized roles by treating them as general camera positions.

#### `camera` (optional)

Index of a glTF camera in the root `cameras` array. When specified, the referenced camera's projection parameters (FOV, clip planes, projection type) and any camera extensions apply to this hint as advisory suggestions.

This property is distinct from the `node.camera` property in the glTF core specification:

- **`node.camera`** (core glTF) tells a renderer: "render from this node using this camera's projection."
- **`camera`** (this extension) tells a viewer: "when using this hint, consider these projection parameters and camera extensions."

By referencing a glTF camera resource, camera hints can leverage the full camera definition -- including any camera extensions (depth of field, lens distortion, etc.) -- without needing to redefine projection concepts inline. This is advisory: runtimes MAY override projection parameters for platform-specific reasons (e.g., VR headsets with fixed FOV, mobile devices with constrained draw distance).

Multiple camera hints MAY reference the same camera, and a camera hint MAY reference the same camera that is attached to the node via `node.camera`.

#### `targetNode` (optional)

Index of a glTF node that this camera should orient toward. When specified, the camera's look-at direction is computed from the vector between the hint node's world-space position and the target node's world-space position.

When `targetNode` is specified, the hint node's `rotation` MAY be ignored by the runtime in favor of the computed look-at orientation.

The `targetNode` MUST be a valid index into the glTF `nodes` array. A camera hint node MUST NOT reference itself via `targetNode`.

#### `label` (optional)

Human-readable label for this camera position. Intended for UI display in model viewers, camera selection menus, and authoring tools. MUST be a non-empty string when present.

### How the Node Transform Encodes Camera Information

Camera hint nodes follow the standard glTF coordinate conventions:

- The camera looks down the node's **local negative Z axis**.
- The camera's up direction is the node's **local positive Y axis**.
- `translation` defines the camera position in the parent node's coordinate space.
- `rotation` defines the camera orientation as a unit quaternion.
- `scale` SHOULD be `[1, 1, 1]`. Implementations SHOULD ignore scale on camera hint nodes.

---

## Standard Role Vocabulary

This extension defines a standard vocabulary of role values. Runtimes that support this extension SHOULD recognize and correctly interpret all standard vocabulary roles. The standard vocabulary provides interoperability guarantees: a viewer encountering `role: "portrait"` can rely on its meaning across all assets.

| Role | Description | Node Transform Interpretation |
|------|-------------|-------------------------------|
| `"default_view"` | Recommended initial camera position when loading the asset. Model viewers SHOULD use this as the starting viewpoint. If multiple `default_view` hints exist, the viewer SHOULD use the first one encountered. | Node **position** is the camera position. Node **rotation** defines the camera orientation (looking down the node's local negative Z axis, with local positive Y as up). |
| `"detail"` | Close-up detail shot of a specific feature or region. Multiple `detail` hints in a single asset are common (face detail, logo detail, material swatch). Use `label` to differentiate them. | Node **position** is the camera position. Node **rotation** defines the camera orientation. |
| `"first_person"` | First-person or VR viewpoint. Typically placed between a character's eyes. Used for avatar embodiment, first-person games, and VR experiences. | Node **position** is the eye-level viewpoint origin. Node **rotation** defines the initial forward direction. |
| `"framing_target"` | Bounding target for auto-framing algorithms. The viewer should ensure this node's position is visible and well-framed in the viewport. | Node **position** is the point of interest. The viewer computes camera distance and angle to frame this point appropriately. |
| `"full_body"` | Full character or product view. Framed to include the entire extent of the subject with appropriate margin. | Node **position** is the camera position. Node **rotation** defines the camera orientation. |
| `"orbit_center"` | Pivot point for orbit camera controls. The camera should orbit around this node's position at a runtime-determined distance. | Node **position** is the orbit center. Node rotation is ignored by the orbit controller but MAY be used as a reference orientation. |
| `"portrait"` | Head-and-shoulders framing suitable for profile pictures, avatar thumbnails, and character selection screens. | Node **position** is the camera position. Node **rotation** defines the camera orientation. |
| `"third_person"` | Third-person camera position, typically behind and above a character. Used for over-the-shoulder views, action games, and character-following cameras. | Node **position** is the camera position. Node **rotation** defines the camera orientation. Often used with `targetNode` to track the character. |

---

## Custom Roles

The `role` property is a free-form string. This extension does not restrict the set of valid role values. The standard vocabulary above provides interoperability guarantees; custom roles provide domain-specific flexibility at the cost of reduced portability.

### Naming Convention

Custom roles SHOULD follow these conventions:

- **Lowercase with underscores**, matching the standard vocabulary style (e.g., `establishing_shot`, not `EstablishingShot`).
- **Vendor prefix** to avoid collisions with future standard vocabulary additions and with other vendors' custom roles. The recommended format is `VENDORNAME_role_name` (e.g., `ACME_establishing_shot`, `MYAPP_selfie_cam`).

### Runtime Handling

Runtimes that encounter an unrecognized role SHOULD treat the hint as a general camera position. The node's transform, `camera` reference, and `label` remain useful even when the role semantics are unknown. Runtimes SHOULD NOT reject an asset or ignore a camera hint solely because its role is not in the standard vocabulary.

### Examples of Custom Roles

| Role | Domain | Description |
|------|--------|-------------|
| `ACME_establishing_shot` | Cinematics | Wide establishing shot of a scene or environment. |
| `ACME_walkthrough_start` | Architecture | Starting position for an architectural walkthrough. |
| `MYAPP_selfie_cam` | Social | Front-facing selfie camera position for social apps. |

---

## Relationship to glTF Cameras

glTF 2.0 defines cameras in the top-level `cameras` array. A glTF camera is an **optical system** -- it specifies how a 3D scene is projected to 2D:

```json
{
  "cameras": [
    {
      "type": "perspective",
      "perspective": {
        "yfov": 0.7854,
        "znear": 0.01,
        "zfar": 1000.0
      }
    }
  ]
}
```

When a camera is attached to a node via the `node.camera` property, the node's transform defines the camera's position and orientation. The `cameras` entry defines the projection.

Camera hints **reference** glTF cameras rather than redefining projection concepts. This design enables camera hints to leverage any camera properties and any camera extensions without modification to this extension:

| Aspect | glTF Camera (`node.camera`) | Camera Hint (`KHR_node_camera_hint`) |
|--------|------------|-------------|
| **Purpose** | Define an optical system for rendering | Suggest a good viewpoint for interaction |
| **Projection** | Required — specifies perspective/orthographic parameters | Optional — references a glTF camera for advisory projection parameters |
| **Camera extensions** | Supports any camera extension (DOF, lens effects, etc.) | Inherits camera extensions via the `camera` reference |
| **Semantics** | "Render from here with these optics" | "This is a good place to put a camera" |
| **Orbit target** | Not defined (no concept of what the camera orbits around) | `orbit_center` role explicitly defines the orbit pivot |
| **Multiple roles** | No role concept -- all cameras are functionally identical | `role` string distinguishes purpose (default view vs. portrait vs. detail) |
| **Interaction model** | Not defined (viewer decides) | `role` implies interaction (orbit, fixed, embodied) |
| **Usage by viewers** | Often ignored -- most viewers compute their own camera | Designed to be consumed by viewers as presentation guidance |

Camera hints and glTF cameras are **complementary**. The `camera` property in this extension provides an advisory link to a glTF camera's projection and extensions. The `node.camera` property in core glTF provides a functional link for rendering. Both can coexist on the same node without conflict.

---

## JSON Examples

### Example 1: Orbit Center for a Product Model

A product model (shoe) with an orbit center placed at the visual center of the product, not the geometric center of the bounding box.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
  "nodes": [
    {
      "name": "ShoeMesh",
      "mesh": 0
    },
    {
      "name": "OrbitCenter",
      "translation": [0, 0.06, 0.02],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "orbit_center",
          "label": "Product Spin"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A product viewer discovers the `orbit_center` node and uses its world-space position as the pivot point for orbit controls. The user can rotate around the shoe at this point, which is at the visual center of the product (slightly above the sole, offset toward the toe box) rather than the bounding box center.

### Example 2: Portrait Camera with Camera Reference

An avatar model with a portrait camera hint that references a glTF camera for projection parameters.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
  "cameras": [
    {
      "name": "PortraitLens",
      "type": "perspective",
      "perspective": {
        "yfov": 0.35,
        "znear": 0.01,
        "zfar": 100.0
      }
    }
  ],
  "nodes": [
    {
      "name": "AvatarRoot",
      "children": [1, 2, 3]
    },
    {
      "name": "AvatarBody",
      "mesh": 0,
      "skin": 0
    },
    {
      "name": "FaceCenter",
      "translation": [0, 1.65, 0.05]
    },
    {
      "name": "PortraitCamera",
      "translation": [0.15, 1.62, 0.65],
      "rotation": [0, 0.087, 0, 0.996],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "portrait",
          "camera": 0,
          "targetNode": 2,
          "label": "Profile Picture"
        }
      }
    }
  ]
}
```

**Runtime behavior:** An avatar marketplace reads the `portrait` hint and positions the camera at the hint's transform, oriented toward the face center node. The referenced camera (index 0) suggests a narrow FOV of 0.35 radians (approximately 20 degrees), producing a telephoto effect that minimizes perspective distortion on facial features -- the same technique used in portrait photography. The runtime MAY override these projection parameters for platform reasons.

### Example 3: Full Body and Detail Cameras on Same Avatar

A character model with multiple camera hints sharing and using separate camera resources.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
  "cameras": [
    {
      "name": "StandardLens",
      "type": "perspective",
      "perspective": {
        "yfov": 0.5236,
        "znear": 0.01,
        "zfar": 200.0
      }
    },
    {
      "name": "MacroLens",
      "type": "perspective",
      "perspective": {
        "yfov": 0.25,
        "znear": 0.005,
        "zfar": 50.0
      }
    }
  ],
  "nodes": [
    {
      "name": "CharacterRoot",
      "children": [1, 2, 3, 4]
    },
    {
      "name": "CharacterBody",
      "mesh": 0,
      "skin": 0
    },
    {
      "name": "BodyCenter",
      "translation": [0, 1.0, 0]
    },
    {
      "name": "FullBodyCamera",
      "translation": [0.8, 1.2, 3.0],
      "rotation": [-0.034, 0.130, 0.004, 0.991],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "full_body",
          "camera": 0,
          "targetNode": 2,
          "label": "Character Select"
        }
      }
    },
    {
      "name": "ChestLogoDetail",
      "translation": [0.1, 1.35, 0.45],
      "rotation": [0, 0.05, 0, 0.999],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "detail",
          "camera": 1,
          "label": "Team Logo"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A character selection screen uses the `full_body` hint to show the complete character with appropriate margin, using the "StandardLens" camera's FOV. A "detail view" button in the UI switches to the `detail` hint, zooming in on the chest logo with the "MacroLens" camera's narrow FOV and closer near clip plane. The `label` properties populate the UI menu: "Character Select" and "Team Logo."

### Example 4: First-Person Viewpoint with Camera Reference

A VR avatar with a first-person camera hint that references a camera with VR-appropriate projection parameters.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
  "cameras": [
    {
      "name": "VRCamera",
      "type": "perspective",
      "perspective": {
        "yfov": 1.5708,
        "znear": 0.05,
        "zfar": 500.0
      }
    }
  ],
  "nodes": [
    {
      "name": "Head",
      "children": [1],
      "mesh": 3
    },
    {
      "name": "FirstPersonView",
      "translation": [0, 0.065, 0.06],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "first_person",
          "camera": 0,
          "label": "First-Person View"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A VR runtime discovers the `first_person` hint and uses its transform (relative to the Head bone) as the viewpoint origin. The referenced camera suggests a wide FOV of ~90 degrees appropriate for VR, a near clip plane of 0.05 meters (close enough for hand interactions but far enough to prevent rendering the inside of the avatar's head mesh), and a far clip plane of 500 meters. The runtime MAY override all of these for its own VR hardware requirements.

### Example 5: Custom Domain-Specific Roles

An architectural walkthrough with vendor-prefixed custom camera roles and camera references.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
  "cameras": [
    {
      "name": "ArchitecturalLens",
      "type": "perspective",
      "perspective": {
        "yfov": 0.7,
        "znear": 0.1,
        "zfar": 1000.0
      }
    }
  ],
  "nodes": [
    {
      "name": "BuildingExterior",
      "mesh": 0
    },
    {
      "name": "HeroShot",
      "translation": [25.0, 8.0, 40.0],
      "rotation": [-0.1, 0.2, 0.02, 0.97],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "ACME_establishing_shot",
          "camera": 0,
          "label": "Building Exterior"
        }
      }
    },
    {
      "name": "LobbyView",
      "translation": [5.0, 1.6, 12.0],
      "rotation": [0, 0.707, 0, 0.707],
      "extensions": {
        "KHR_node_camera_hint": {
          "role": "ACME_walkthrough_start",
          "label": "Lobby Entrance"
        }
      }
    }
  ]
}
```

**Runtime behavior:** An architectural visualization app discovers the vendor-prefixed custom roles `ACME_establishing_shot` and `ACME_walkthrough_start`. A runtime that recognizes these ACME-specific roles can provide specialized behavior (e.g., cinematic transitions). A runtime that does not recognize them treats each hint as a general camera position, using the node transforms and `label` properties to present camera options to the user.

---

## Interaction with Other Extensions

### VRM firstPerson (VRMC\_vrm)

VRM 1.0 defines a `firstPerson` system that controls which meshes are visible in first-person view:

```json
{
  "VRMC_vrm": {
    "firstPerson": {
      "meshAnnotations": [
        { "node": 0, "type": "thirdPersonOnly" },
        { "node": 1, "type": "firstPersonOnly" }
      ]
    }
  }
}
```

VRM's `firstPerson` controls **mesh visibility** (what to render in first-person). `KHR_node_camera_hint` with `role: "first_person"` controls **camera position** (where the viewpoint is). These are complementary:

- VRM `firstPerson` answers: "Which meshes should be visible when the camera is inside the avatar?"
- Camera hint `first_person` answers: "Where exactly should the camera be positioned inside the avatar?"

VRM defines the viewpoint origin implicitly via `lookAt.offsetFromHeadBone`. A `first_person` camera hint provides the same information explicitly as a scene node, with the advantage of being animatable and discoverable through standard glTF node traversal.

No VRMC\_vrm specification changes are required. The two systems compose without conflict.

### KHR\_node\_lookat\_target

`KHR_node_lookat_target` marks a node as a passive positional target for look-at systems. Camera hints with `targetNode` can reference look-at target nodes, creating a clear separation of concerns:

- The **camera hint** defines WHERE the camera is positioned and what role it serves.
- The **look-at target** defines WHAT the camera (or other system) should orient toward.

A `targetNode` reference and a `KHR_node_lookat_target` annotation on the referenced node are complementary. The `targetNode` property provides the functional linkage (aim the camera here). The `KHR_node_lookat_target` extension on the referenced node provides semantic discovery (this node is a tracking target).

---

## Conformance

### Authoring Requirements

1. The `role` property MUST be present and MUST be a non-empty string.
2. If `camera` is specified, it MUST be a valid index into the glTF `cameras` array.
3. If `targetNode` is specified, it MUST be a valid index into the glTF `nodes` array.
4. A camera hint node MUST NOT reference itself via `targetNode`.
5. Camera hint nodes SHOULD have a scale of `[1, 1, 1]`.
6. Multiple camera hint nodes with the same `role` are permitted. If multiple hints with role `default_view` exist, authoring tools SHOULD place the preferred one first in the node traversal order.
7. The `label` property, when present, SHOULD be a concise, human-readable string suitable for UI display.

### Runtime Requirements

1. Implementations that support this extension SHOULD use camera hint data to inform camera positioning when loading or displaying an asset.
2. Implementations SHOULD gracefully handle unrecognized `role` values by treating them as general camera positions. The node's transform and `label` remain useful even when the role is unknown.
3. When `camera` is specified, implementations SHOULD use the referenced camera's projection parameters. Implementations MAY override these for platform-specific reasons (e.g., VR headsets with fixed FOV, mobile devices with constrained draw distance).
4. When `targetNode` is specified, implementations SHOULD orient the camera toward the target node's world-space position.
5. When `targetNode` is not specified, implementations SHOULD use the node's rotation to determine camera orientation.
6. Implementations that do not support this extension SHOULD still render the model using their default camera positioning heuristics. Camera hints are presentation guidance, not rendering requirements.
7. Camera hint nodes SHOULD NOT be rendered as visible geometry. They are metadata nodes.

### Fallback Behavior

If an implementation does not support `KHR_node_camera_hint` and the extension appears only in `extensionsUsed` (not `extensionsRequired`), the implementation SHOULD render the model using its default camera positioning algorithm. The visual content of the asset is unaffected -- only the initial camera position and framing intent are lost. This makes the extension safe for broad deployment: assets with camera hints degrade gracefully to standard viewer behavior in implementations that do not support the extension.

---

## Schema

### node.KHR\_node\_camera\_hint

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "node.KHR_node_camera_hint.schema.json",
  "title": "KHR_node_camera_hint glTF Node Extension",
  "description": "Marks a node as an authored camera positioning suggestion. The node's world-space transform defines the recommended camera placement. The runtime MAY use this as a default view, orbit center, portrait framing, or other camera-related viewpoint.",
  "type": "object",
  "properties": {
    "role": {
      "type": "string",
      "description": "The semantic purpose of this camera hint. Determines how runtimes interpret the node's transform. Standard vocabulary values provide interoperability guarantees; custom values are permitted for domain-specific use cases.",
      "minLength": 1
    },
    "camera": {
      "type": "integer",
      "description": "Index of a glTF camera in the root cameras array whose projection parameters (FOV, clip planes, type) and any camera extensions apply to this hint. Advisory — runtimes MAY override.",
      "minimum": 0
    },
    "targetNode": {
      "type": "integer",
      "description": "Index of a glTF node that this camera should orient toward. When specified, the camera's look-at direction is computed from the vector between this node and the target node.",
      "minimum": 0
    },
    "label": {
      "type": "string",
      "description": "Human-readable label for this camera position. Used for UI display in model viewers and camera selection menus.",
      "minLength": 1
    }
  },
  "required": ["role"],
  "additionalProperties": false
}
```

### Validation Rules (Non-Schema)

The following constraints cannot be expressed in JSON Schema and MUST be enforced by authoring tools or validators:

1. `camera` MUST be a valid index into the asset's `cameras` array (i.e., `0 <= camera < cameras.length`).
2. `targetNode` MUST be a valid index into the asset's `nodes` array (i.e., `0 <= targetNode < nodes.length`).
3. A camera hint node MUST NOT reference its own index via `targetNode`.

---

## Known Limitations

1. **No orbit constraints.** This extension defines an orbit center but not orbit angle limits, minimum/maximum orbit distances, or pan boundaries. These are common features in product viewers (model-viewer's `min-camera-orbit` / `max-camera-orbit`). They are intentionally deferred: orbit constraints are interaction parameters that vary significantly across viewer implementations. A future revision or companion extension may address orbit constraints if industry consensus emerges.

2. **No auto-rotation.** Auto-rotation (spinning the model slowly for product showcase) is a common viewer feature with no standardized parameterization. This extension does not define auto-rotation behavior. It is a runtime presentation choice.

3. **No transition behavior.** When a viewer transitions between camera hints (e.g., switching from `full_body` to `detail`), the transition style (cut, smooth interpolation, dolly) is not specified. This is a runtime implementation choice.

4. **No grouping mechanism.** When an asset contains many camera hints (e.g., an architectural walkthrough with 20+ viewpoints), there is no built-in mechanism for organizing hints into groups or sequences. The `label` property provides human-readable differentiation, but structured grouping is not supported. Authoring tools may use the node hierarchy (parent-child relationships) as an informal grouping mechanism.

5. **Index-based references are fragile under editing.** Adding, removing, or reordering nodes in the `nodes` array invalidates `targetNode` references. Similarly, modifying the `cameras` array invalidates `camera` references. Authoring tools MUST update these index values when modifying the respective arrays. This is consistent with how glTF handles index-based references elsewhere (e.g., `node.children`, `node.mesh`, `skin.skeleton`).

6. **Custom role portability.** Custom roles provide domain-specific flexibility but are not guaranteed to be recognized by arbitrary viewers. For maximum portability, prefer standard vocabulary roles. Runtimes that encounter unrecognized roles SHOULD still use the node's transform and `label` as a general camera position.

7. **Hint suggestions may be overridden.** Runtimes MAY ignore any or all camera hint properties. An asset author cannot guarantee that their camera hints will be used. This is by design -- camera hints are advisory -- but it means authors cannot rely on hints for experiences that require exact camera control. For exact camera control, use glTF camera nodes with animation.

---

## License

This extension specification is licensed under the Khronos Group Extension License.
See: <https://www.khronos.org/registry/gltf/license.html>
