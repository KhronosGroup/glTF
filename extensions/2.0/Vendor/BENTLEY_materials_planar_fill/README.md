<!--
Copyright 2015-2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# BENTLEY_materials_planar_fill

## Contributors

* Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

3D modeling and computer-aided drafting (CAD) environments like SketchUp, MicroStation, and Revit make extensive use of non-photorealistic two- and three-dimensional visualizations involving planar polygons with filled interiors. The `BENTLEY_materials_planar_fill` allows the behavior of the interior fill to be customized.

This specification describes a minimal extension sufficient to meet Bentley Systems' requirements, with some suggestions for how it might be broadened and/or generalized while retaining a focus on CAD visualization.

## Specifying Planar Fill

The `BENTLEY_materials_planar_fill` extension is applied to a material. It specifies how the interior fill of a planar polygon should be rendered.

In a wireframe view, by default only the edges of polygons are displayed. The extension's `wireframeFill` property controls whether the fill is displayed in wireframe. `NONE` (0) indicates the fill is never drawn in wireframe, `ALWAYS` (1) indicates it is always drawn in wireframe, and `TOGGLE` indicates that it is drawn only if the user or application has chosen for toggleable fill to be drawn.

The extension's `backgroundFill` property, if `true`, specifies that the fill should be drawn in the same color as the view's background color. This permits an "invisible" polygon to mask out geometry behind it.

The extension's `behind` property, if `true`, specifies that the fill should be drawn behind other coplanar geometry representing the same object. This is useful for mitigating z-fighting when, for example, a hatch pattern is applied to a filled polygon, or text is drawn inside a filled rectangle.

> TBD: "representing the same object" has a specific meaning for Bentley (originating from the same element); it would need to be generalized somehow for other use cases.

## glTF Schema Updates


## Implementation Notes


## JSON Schema


