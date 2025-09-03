# KHR_mesh_annotation_renderiew

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

## Overview

The `KHR_mesh_annotation_renderview` extension provides structured metadata for character mesh primitives related to **visibility and render-time behavior**. It complements the core glTF material and LOD systems by exposing semantic visibility rules such as “first-person only” and “always visible”.

This extension is intended to support use cases such as hiding certain parts of an character (like head or hair) in first-person mode, or marking components that must always be rendered (e.g., eyes)

This was thought of as part of the avatar/character extension proposals; as character models often contain complex geometry with overlapping purposes (e.g. hair, clothing, limbs, devices, accessories). Many applications need to reason about specific mesh subsets, or attribute specific information about them (with it beiong on a per-primitive basis; as each primitive can be a different entity entirely). Enabling visibility on a per-node basis would result in downchain nodes being disabled when it's not intended/desired; having a granular setting for this allows for more flexibility. 

## Use Cases

- First-person camera hiding for head/hair geometry or other geometry
- Denoting when there's a completely different alternate mesh for one perspective versus the other


## Extension Schema

This extension is defined per `mesh.primitive`, allowing fine-grained render behavior control.

```json
{
  "meshes": [
    {
      "primitives": [
        {
          "attributes": { ... },
          "extensions": {
            "KHR_mesh_annotation": {
              "tags": ["eyewear", "selectable"],
              "customData": { },
              "extensions": {
              "KHR_mesh_annotation_renderview": {
                "renderVisibility": "firstPersonOnly"
              }
              }
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

These properties have no effect unless supported by the runtime.

Runtimes may choose to ignore renderVisibility settings if it directly causes downchain issues with their experiences (or a generalized incompatibility). In this case, our recommendation for 
those runtimes or tools is to take these settings into account in some capacity if implementing their own mesh-culling logic for a given model. Of course given different uses of a model, your mileage may vary with this. For example, if used with a character in an experience that has a different camera perspective than what the original content creator thought of when baking the asset; it's up to the developer to determine what's more important (respecting the render visibility setting or the continuity of said experience).

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
