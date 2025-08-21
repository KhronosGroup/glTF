# KHR_avatar_mesh_annotation_rendering

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Individual Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Individual Contributor
- Nick Burkard, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.  
Dependent on: `KHR_avatar`

## Overview

The `KHR_avatar_mesh_annotation_rendering` extension provides structured metadata for avatar mesh primitives related to **visibility and render-time behavior**. It complements the core glTF material and LOD systems by exposing semantic visibility rules such as “first-person only”, “always visible”, etc.

This extension is intended to support use cases such as:

- Hiding certain parts of an avatar (like head or hair) in first-person mode
- Marking components that must always be rendered (e.g., eyes)
- Toggling cosmetic parts based on camera mode or gameplay state
- Managing specialized render passes (shadows, reflections, etc.)

## Use Cases

- First-person camera hiding for head/hair geometry
- Consistent behavior for accessories across render modes
- Semantic control of pass inclusion (e.g., do not render in reflection pass)
- Optimization by excluding off-screen or nested mesh content

## Extension Schema

This extension is defined per `mesh.primitive`, allowing fine-grained render behavior control.

```json
{
  "meshes": [
    {
      "primitives": [
        {
          "extensions": {
            "KHR_avatar_mesh_annotation_rendering": {
              "renderVisibility": "firstPersonOnly",
              "renderTags": ["eyewear", "forceRender"]
            }
          }
        }
      ]
    }
  ]
}
```

### Properties

| Property         | Type        | Description                                                                 |
|------------------|-------------|-----------------------------------------------------------------------------|
| `renderVisibility` | string    | Controls camera-based visibility. Enum: `"always"` | `"firstPersonOnly"` | `"thirdPersonOnly"` | `"never"` |
| `renderTags`     | string[]    | Optional list of semantic labels for custom rendering logic                |

## Recommended Usage Patterns

### First-Person Hiding

```json
"renderVisibility": "thirdPersonOnly"
```

Used to hide geometry (e.g., head, hair, hats) in first-person mode to prevent obstruction.

### Always-Visible Elements

```json
"renderVisibility": "always"
```

Used for elements like top meshes that should always remain active regardless of view or culling.

## Implementation Notes

- These properties have no effect unless supported by the runtime.
- Runtimes may map `renderVisibility` values to camera tag groups or dynamic material toggles.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
