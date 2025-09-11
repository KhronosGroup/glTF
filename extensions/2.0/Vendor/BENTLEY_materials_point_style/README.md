<!--
Copyright 2015-2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# BENTLEY_materials_point_style

## Contributors

* Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This specification describes a minimal extension sufficient to meet Bentley Systems' requirements, with some suggestions for how it might be broadened and/or generalized while retaining a focus on CAD visualization.

## Specifying Point Styles

The `BENTLEY_materials_point_style` extension is applied to a material. When that material is used by a POINTS primitive, the extension dictates the size of the points.

The `size` property specifies the diameter of each point in pixels.

> Potential generalization: we assume round points. Permit square points? Permit the material's texture to be mapped to the point's bounding box to support arbitrary point shapes?

> Potential generalization: we currently assume points face the camera, which is sensible when using pixels as units of measure. But the glTF spec permits for points to have normal attributes for purposes of lighting; this could also be used to specify the plane in which they should be drawn.

> Potential generalization: permit size to be specified in meters.

## glTF Schema Updates


## Implementation Notes


## JSON Schema



