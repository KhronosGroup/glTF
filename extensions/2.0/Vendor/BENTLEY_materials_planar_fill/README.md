<!--
Copyright 2015-2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# BENTLEY_materials_planar_fill

## Contributors

* Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)
* Mark Schlosser, Bentley Systems, [@markschlosseratbentley](https://github.com/markschlosseratbentley)
* Daniel Zhong, Bentley Systems, [@danielzhong](https://github.com/danielzhong)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Two- and three-dimensional planar polygons with filled interiors are fundamental elements in many 3D modeling and computer-aided design (CAD) environments. The `BENTLEY_materials_planar_fill` extension allows the behavior of a polygon's interior fill to be customized with the intent of being able to indicate a variety of meanings useful to CAD applications.

This specification describes a minimal extension sufficient to meet Bentley Systems' requirements, with some suggestions for how it might be broadened and/or generalized while retaining a focus on CAD visualization.

## Extending Materials

Planar fill behavior is defined by adding the `BENTLEY_materials_planar_fill` extension to any glTF material. The extension properties control how the interior fill of a planar polygon should be rendered.

Example:

```json
{
    "materials": [
        {
            "name": "CAD_Surface",
            "pbrMetallicRoughness": {
                "baseColorFactor": [0.8, 0.8, 0.8, 1.0]
            },
            "extensions": {
                "BENTLEY_materials_planar_fill": {
                    "wireframeFill": 2,
                    "backgroundFill": false,
                    "behind": false
                }
            }
        }
    ]
}
```

## Properties

The following properties are defined for this extension:

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `wireframeFill` | `integer` | Specifies whether and when the interior fill is displayed in wireframe view mode. | No, Default: `0` |
| `backgroundFill` | `boolean` | If `true`, the fill should be drawn using the view's background color. | No, Default: `false` |
| `behind` | `boolean` | If `true`, the fill should be drawn behind other coplanar geometry. | No, Default: `false` |

### wireframeFill

In a wireframe view, by default only the edges of polygons are displayed. The `wireframeFill` property controls whether the fill is displayed in wireframe mode.

| Value | Name | Description |
|:------|:-----|:------------|
| `0` | NONE | The fill is never drawn in wireframe view. |
| `1` | ALWAYS | The fill is always drawn in wireframe view. |
| `2` | TOGGLE | The fill is drawn only if the user or application has enabled toggleable fill display. |

### backgroundFill

When `backgroundFill` is `true`, the fill should be drawn using the same color as the view's background color. This permits an "invisible" polygon to mask out geometry behind it.

### behind

When `behind` is `true`, the fill should be drawn behind other coplanar geometry belonging to the same logical object. This is useful for mitigating z-fighting when, for example:

- A hatch pattern is applied to a filled polygon
- Text is drawn inside a filled rectangle
- Line work overlays a solid surface

> **Implementation Note:** The concept of "belonging to the same logical object" has a specific meaning for Bentley (originating from the same element). Implementations should define how they determine coplanarity and object association.

## glTF Schema Updates

The following additional property is defined for the material:

**material.BENTLEY_materials_planar_fill**

| Property | Description | Required |
|:---------|:------------|:---------|
| `wireframeFill` | Integer enum controlling fill visibility in wireframe mode. Valid values are `0` (NONE), `1` (ALWAYS), `2` (TOGGLE). | No, Default: `0` |
| `backgroundFill` | Boolean indicating whether fill uses the view's background color. | No, Default: `false` |
| `behind` | Boolean indicating whether fill should be drawn behind coplanar geometry. | No, Default: `false` |

### JSON Schema

- [material.BENTLEY_materials_planar_fill.schema.json](schema/material.BENTLEY_materials_planar_fill.schema.json)

## Examples

### Basic Toggleable Fill

A material that displays fill only when the user has enabled fill display locally:

```json
{
    "materials": [
        {
            "name": "toggleable_fill_material",
            "pbrMetallicRoughness": {
                "baseColorFactor": [0.5, 0.5, 0.5, 1.0]
            },
            "extensions": {
                "BENTLEY_materials_planar_fill": {
                    "wireframeFill": 2
                }
            }
        }
    ]
}
```

### Masking Polygon

A material that creates an invisible mask to hide geometry behind it:

```json
{
    "materials": [
        {
            "name": "mask_material",
            "pbrMetallicRoughness": {
                "baseColorFactor": [1.0, 1.0, 1.0, 1.0]
            },
            "extensions": {
                "BENTLEY_materials_planar_fill": {
                    "wireframeFill": 1,
                    "backgroundFill": true
                }
            }
        }
    ]
}
```

### Hatched Surface

A material for a surface that should appear behind its associated hatch pattern:

```json
{
    "materials": [
        {
            "name": "hatched_surface_material",
            "pbrMetallicRoughness": {
                "baseColorFactor": [0.9, 0.9, 0.8, 1.0]
            },
            "extensions": {
                "BENTLEY_materials_planar_fill": {
                    "wireframeFill": 1,
                    "behind": true
                }
            }
        }
    ]
}
```

## Implementation Notes

### Wireframe Mode Detection

This extension assumes that the rendering engine has the concept of a "wireframe view mode" or similar. Engines that do not support such a mode may ignore the `wireframeFill` property and always render the fill.

### Depth Buffer Considerations

When `behind` is `true`, implementations should use appropriate depth offset techniques (such as polygon offset) to ensure the fill renders behind coplanar geometry. Care should be taken to avoid visual artifacts that can occur with excessive or insufficient depth bias.

### Fallback Behavior

When this extension is not supported by a viewer, the material should render normally using its core PBR properties. The planar fill customizations will not be applied, and the geometry will display as a standard filled surface.

Assets using this extension should list this extension in the `extensionsUsed` array, avoiding also listing it in the `extensionsRequired` array ([glTF documentation here](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#specifying-extensions)), unless the visual appearance depends critically on the extension behavior.

```json
{
    "extensionsUsed": [
        "BENTLEY_materials_planar_fill"
    ]
}
```

## Known Implementations

* TBD
