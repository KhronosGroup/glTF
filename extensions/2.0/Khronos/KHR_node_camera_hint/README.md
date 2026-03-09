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

`KHR_node_camera_hint` allows content creators to embed camera positioning suggestions directly in the glTF asset as annotated scene nodes. Each camera hint is a standard glTF node whose transform defines a spatial position and orientation, annotated with a semantic `role` that tells viewers how to interpret the hint.

Camera hints are **advisory**. Runtimes MAY use them, adapt them, or ignore them. They are presentation suggestions, not rendering mandates.

### What This Extension Is NOT

- **Not a camera definition.** glTF 2.0 already defines cameras in the `cameras` array with perspective and orthographic projection parameters. Camera hints define POSITIONS and FRAMING INTENT, not optical systems. They are complementary to glTF cameras, not replacements.
- **Not a rendering specification.** Camera hints do not define depth of field, motion blur, lens distortion, post-processing, or any visual effect. They define where to put a camera and what to look at.
- **Not a constraint system.** Camera hints do not specify orbit angle limits, pan boundaries, zoom constraints, or auto-rotation behavior. These are runtime interaction concerns outside the scope of this extension.

### Terminology

| Term | Definition |
|------|-----------|
| **Camera hint** | A glTF node annotated with this extension. The node's world-space transform defines a recommended camera position and/or orientation. The extension data provides semantic context for how the hint should be interpreted. |
| **Role** | The semantic purpose of a camera hint, expressed as a free-form string. The role indicates how a runtime should interpret the node's transform (e.g., as a camera position, an orbit pivot, or a framing target). |
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
| `role` | `string` | **Yes** | -- | The semantic purpose of this camera hint. Free-form string identifying how runtimes should interpret the node's transform. |
| `fovHint` | `number` | No | -- | Suggested vertical field of view in radians. MUST be greater than 0 and less than pi (~3.14159). |
| `nearHint` | `number` | No | -- | Suggested near clipping plane distance in meters. MUST be greater than 0. |
| `farHint` | `number` | No | -- | Suggested far clipping plane distance in meters. MUST be greater than 0. MUST be greater than `nearHint` when both are specified. |
| `targetNode` | `integer` | No | -- | Index of a glTF node that this camera should orient toward. MUST be a valid index into the glTF `nodes` array. The camera hint node MUST NOT reference itself. |
| `label` | `string` | No | -- | Human-readable label for this camera position. Intended for UI display in model viewers, camera selection menus, and authoring tools. |

### Property Descriptions

#### `role` (required)

The semantic purpose of this camera hint. This is a free-form string that tells runtimes how to interpret the node's transform.

Roles are free-form strings. This extension does not prescribe a vocabulary. Authors MAY use any role that fits their use case. The Example Role Values section provides common roles for product visualization and character presentation, but these are illustrative examples, not a required vocabulary.

Applications SHOULD interpret recognized role values appropriately and SHOULD gracefully handle unrecognized roles by treating them as general camera positions.

#### `fovHint` (optional)

Suggested vertical field of view in radians for perspective cameras. MUST be greater than 0 and less than pi (~3.14159).

This is a suggestion; runtimes MAY override this value for platform-specific reasons (e.g., VR headsets with fixed FOV, mobile devices with performance-constrained rendering).

#### `nearHint` / `farHint` (optional)

Suggested near and far clipping plane distances in meters. Both MUST be greater than 0. When both are specified, `farHint` MUST be greater than `nearHint`.

These are suggestions; runtimes MAY override these values based on platform constraints.

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

## Example Role Values

The following roles are provided as **examples** for common camera hint use cases. These are illustrative, not prescriptive. Authors MAY use any role that fits their use case. Custom roles are expected and normal for domain-specific applications (cinematics, VR experiences, architectural visualization, etc.).

| Role | Description | Node Transform Interpretation |
|------|-------------|-------------------------------|
| `orbit_center` | Pivot point for orbit camera controls. The camera should orbit around this node's position at a runtime-determined distance. | Node **position** is the orbit center. Node rotation is ignored by the orbit controller but MAY be used as a reference orientation. |
| `framing_target` | Bounding target for auto-framing algorithms. The viewer should ensure this node's position is visible and well-framed in the viewport. | Node **position** is the point of interest. The viewer computes camera distance and angle to frame this point appropriately. |
| `default_view` | Recommended initial camera position when loading the asset. Model viewers SHOULD use this as the starting viewpoint. If multiple `default_view` hints exist, the viewer SHOULD use the first one encountered. | Node **position** is the camera position. Node **rotation** defines the camera orientation (looking down the node's local negative Z axis, with local positive Y as up). |
| `portrait` | Head-and-shoulders framing suitable for profile pictures, avatar thumbnails, and character selection screens. | Node **position** is the camera position. Node **rotation** defines the camera orientation. |
| `full_body` | Full character or product view. Framed to include the entire extent of the subject with appropriate margin. | Node **position** is the camera position. Node **rotation** defines the camera orientation. |
| `third_person` | Third-person camera position, typically behind and above a character. Used for over-the-shoulder views, action games, and character-following cameras. | Node **position** is the camera position. Node **rotation** defines the camera orientation. Often used with `targetNode` to track the character. |
| `detail` | Close-up detail shot of a specific feature or region. Multiple `detail` hints in a single asset are common (face detail, logo detail, material swatch). Use `label` to differentiate them. | Node **position** is the camera position. Node **rotation** defines the camera orientation. |
| `first_person` | First-person or VR viewpoint. Typically placed between a character's eyes. Used for avatar embodiment, first-person games, and VR experiences. | Node **position** is the eye-level viewpoint origin. Node **rotation** defines the initial forward direction. |

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

Camera hints are a **different concept**:

| Aspect | glTF Camera | Camera Hint |
|--------|------------|-------------|
| **Purpose** | Define an optical system for rendering | Suggest a good viewpoint for interaction |
| **Projection** | Specifies perspective/orthographic parameters (FOV, aspect, clip planes) | No projection -- `fovHint`, `nearHint`, `farHint` are optional suggestions |
| **Semantics** | "Render from here with these optics" | "This is a good place to put a camera" |
| **Orbit target** | Not defined (no concept of what the camera orbits around) | `orbit_center` role explicitly defines the orbit pivot |
| **Multiple roles** | No role concept -- all cameras are functionally identical | `role` string distinguishes purpose (default view vs. portrait vs. detail) |
| **Interaction model** | Not defined (viewer decides) | `role` implies interaction (orbit, fixed, embodied) |
| **Usage by viewers** | Often ignored -- most viewers compute their own camera | Designed to be consumed by viewers as presentation guidance |

Camera hints and glTF cameras are **complementary**. A camera hint MAY coexist on the same node as a `node.camera` reference, combining spatial positioning (from the hint) with projection parameters (from the camera). However, camera hints typically stand alone -- the runtime applies its own projection parameters to the suggested position.

A camera hint with `fovHint`, `nearHint`, and `farHint` provides **suggestions** for projection parameters without requiring a full glTF camera definition. These hints are advisory: the runtime MAY use different values based on platform constraints (VR headsets have fixed FOV, mobile devices have performance-constrained clip planes).

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

### Example 2: Portrait Camera for Avatar Profile

An avatar model with a portrait camera hint for generating profile pictures and thumbnails.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
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
          "fovHint": 0.35,
          "targetNode": 2,
          "label": "Profile Picture"
        }
      }
    }
  ]
}
```

**Runtime behavior:** An avatar marketplace reads the `portrait` hint and positions the camera at the hint's transform, oriented toward the face center node. The narrow `fovHint` (0.35 radians, approximately 20 degrees) produces a telephoto effect that minimizes perspective distortion on facial features -- the same technique used in portrait photography.

### Example 3: Full Body and Detail Cameras on Same Avatar

A character model with multiple camera hints: a full-body view for character selection and a detail view of a logo on the chest.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
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
          "fovHint": 0.5236,
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
          "fovHint": 0.25,
          "label": "Team Logo"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A character selection screen uses the `full_body` hint to show the complete character with appropriate margin. A "detail view" button in the UI switches to the `detail` hint, zooming in on the chest logo. The `label` properties populate the UI menu: "Character Select" and "Team Logo."

### Example 4: First-Person Viewpoint with FOV and Clip Plane Hints

A VR avatar with a first-person camera hint placed between the eyes, including FOV and clipping plane suggestions to prevent seeing inside the avatar's own head geometry.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
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
          "fovHint": 1.5708,
          "nearHint": 0.05,
          "farHint": 500.0,
          "label": "First-Person View"
        }
      }
    }
  ]
}
```

**Runtime behavior:** A VR runtime discovers the `first_person` hint and uses its transform (relative to the Head bone) as the viewpoint origin. The `fovHint` of ~90 degrees suggests a wide field of view appropriate for VR. The `nearHint` of 0.05 meters ensures the near clip plane is close enough for hand interactions but far enough to prevent rendering the inside of the avatar's head mesh. The `farHint` of 500 meters provides a reasonable draw distance.

### Example 5: Custom Domain-Specific Roles

An architectural walkthrough with custom camera roles.

```json
{
  "asset": { "version": "2.0" },
  "extensionsUsed": ["KHR_node_camera_hint"],
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
          "role": "establishing_shot",
          "fovHint": 0.7,
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
          "role": "walkthrough_start",
          "label": "Lobby Entrance"
        }
      }
    }
  ]
}
```

**Runtime behavior:** An architectural visualization app discovers the custom roles `establishing_shot` and `walkthrough_start`. Even if it doesn't recognize these specific roles, it can present them to the user using the `label` property and use the node transforms as camera positions.

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
2. If `targetNode` is specified, it MUST be a valid index into the glTF `nodes` array.
3. A camera hint node MUST NOT reference itself via `targetNode`.
4. If both `nearHint` and `farHint` are specified, `farHint` MUST be greater than `nearHint`.
5. `fovHint` MUST be greater than 0 and less than pi (~3.14159) radians.
6. `nearHint` MUST be greater than 0.
7. `farHint` MUST be greater than 0.
8. Camera hint nodes SHOULD have a scale of `[1, 1, 1]`.
9. Multiple camera hint nodes with the same `role` are permitted. If multiple hints with role `default_view` exist, authoring tools SHOULD place the preferred one first in the node traversal order.
10. The `label` property, when present, SHOULD be a concise, human-readable string suitable for UI display.

### Runtime Requirements

1. Implementations that support this extension SHOULD use camera hint data to inform camera positioning when loading or displaying an asset.
2. Implementations SHOULD gracefully handle unrecognized `role` values by treating them as general camera positions. The node's transform and `label` remain useful even when the role is unknown.
3. `fovHint`, `nearHint`, and `farHint` are suggestions. Implementations MAY override these values for platform-specific reasons (e.g., VR headsets with fixed FOV, mobile devices with constrained draw distance).
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
      "description": "The semantic purpose of this camera hint. Determines how runtimes interpret the node's transform.",
      "minLength": 1
    },
    "fovHint": {
      "type": "number",
      "description": "Suggested vertical field of view in radians. Runtimes MAY override this for platform reasons.",
      "exclusiveMinimum": 0,
      "exclusiveMaximum": 3.14159265358979
    },
    "nearHint": {
      "type": "number",
      "description": "Suggested near clipping plane distance in meters.",
      "exclusiveMinimum": 0
    },
    "farHint": {
      "type": "number",
      "description": "Suggested far clipping plane distance in meters. When both nearHint and farHint are specified, farHint MUST be greater than nearHint.",
      "exclusiveMinimum": 0
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

1. `targetNode` MUST be a valid index into the asset's `nodes` array (i.e., `0 <= targetNode < nodes.length`).
2. A camera hint node MUST NOT reference its own index via `targetNode`.
3. When both `nearHint` and `farHint` are present, `farHint` MUST be greater than `nearHint`.

---

## Known Limitations

1. **No orbit constraints.** This extension defines an orbit center but not orbit angle limits, minimum/maximum orbit distances, or pan boundaries. These are common features in product viewers (model-viewer's `min-camera-orbit` / `max-camera-orbit`). They are intentionally deferred: orbit constraints are interaction parameters that vary significantly across viewer implementations. A future revision or companion extension may address orbit constraints if industry consensus emerges.

2. **No auto-rotation.** Auto-rotation (spinning the model slowly for product showcase) is a common viewer feature with no standardized parameterization. This extension does not define auto-rotation behavior. It is a runtime presentation choice.

3. **No transition behavior.** When a viewer transitions between camera hints (e.g., switching from `full_body` to `detail`), the transition style (cut, smooth interpolation, dolly) is not specified. This is a runtime implementation choice.

4. **No grouping mechanism.** When an asset contains many camera hints (e.g., an architectural walkthrough with 20+ viewpoints), there is no built-in mechanism for organizing hints into groups or sequences. The `label` property provides human-readable differentiation, but structured grouping is not supported. Authoring tools may use the node hierarchy (parent-child relationships) as an informal grouping mechanism.

5. **Index-based targetNode is fragile under editing.** Adding, removing, or reordering nodes in the `nodes` array invalidates `targetNode` references. Authoring tools MUST update `targetNode` values when modifying the node list. This is consistent with how glTF handles index-based references elsewhere (e.g., `node.children`, `node.mesh`, `skin.skeleton`).

6. **Hint suggestions may be overridden.** Runtimes MAY ignore any or all camera hint properties. An asset author cannot guarantee that their camera hints will be used. This is by design -- camera hints are advisory -- but it means authors cannot rely on hints for experiences that require exact camera control. For exact camera control, use glTF camera nodes with animation.

---

## License

This extension specification is licensed under the Khronos Group Extension License.
See: <https://www.khronos.org/registry/gltf/license.html>