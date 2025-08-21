# KHR_avatar_mesh_annotation

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
Can be used alongside rendering-related extensions.

## Overview

The `KHR_avatar_mesh_annotation` extension enables arbitrary per-mesh metadata annotations for avatar models. This provides a generalized way for creators and tools to semantically tag portions of geometry for gameplay, rendering, accessibility, customization, or runtime logic.

Unlike expression bindings or skeletons, this extension focuses on **attaching structured metadata to glTF mesh primitives or submeshes**. Examples include:

- Indicating occlusion, hit zones, body regions, or cosmetics
- Tagging subregions with gameplay affordances (e.g., "touchable", "selectable")
- Supporting accessibility metadata (e.g., limb representation for low-vision aid)
- Facilitating layered rendering systems or shadow pass rules

## Motivation

Avatar models often contain complex geometry with overlapping purposes (e.g. hair, clothing, limbs, devices, accessories). Many applications need to reason about specific mesh subsets for:

- Avatar customization layers
- Haptic or interaction targeting
- Runtime layering and decal application
- Streaming/LOD switching

This extension provides a consistent, schema-driven approach to labeling such mesh regions without requiring custom parsing or glTF structural abuse.

## Schema

Annotations are attached at the **primitive level** (i.e., a `mesh.primitives[i]`), and stored in the `KHR_avatar_mesh_annotation` extension:

```json
{
  "meshes": [
    {
      "primitives": [
        {
          "attributes": { ... },
          "extensions": {
            "KHR_avatar_mesh_annotation": {
              "tags": ["eyewear", "selectable"],
                            "region": "face"
            }
          }
        }
      ]
    }
  ]
}
```

### Properties

| Property     | Type     | Description                                                          |
| ------------ | -------- | -------------------------------------------------------------------- |
| `tags`       | string[] | List of free-form labels applicable to this primitive                |
| `customData` | object   | Optional freeform object for runtime-specific annotations (optional) |

## Use Cases & Examples

### 2. Touch Targets

```json
"tags": ["touchable", "interactionZone"]
```

Used by interaction or accessibility systems to identify meaningful geometry.

### 3. Region Metadata for Accessibility

```json
"tags": ["left_hand"]
```

This can drive voiceover descriptions, haptics, or visual aids for users with assistive needs.

### 4. Selectable Cosmetic Submeshes

```json
"tags": ["eyewear", "customizable"]
```

Allows filtering of optional accessories or togglable cosmetic parts.

## Implementation Notes

- Annotations apply at the granularity of `mesh.primitive`, enabling fine control.
- Tags are non-authoritative but enable semantic interoperability across runtimes.
- `customData` allows developers to store app-specific structured metadata (e.g., color categories, tracking weights, heatmaps).
- If this extension is not understood by a client, it should be safely ignored with no visual or functional change.

## Relation to Other Extensions

- **Complements `KHR_materials_variants`** by labeling which primitives may be toggled/swapped.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
