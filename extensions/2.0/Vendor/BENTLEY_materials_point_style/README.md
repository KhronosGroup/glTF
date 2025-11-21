<!--
Copyright 2025 Bentley Systems, Incorporated
SPDX-License-Identifier: CC-BY-4.0
-->

# BENTLEY_material_point_style

## Contributors

* Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)
* Mark Schlosser, Bentley Systems, [@markschlosseratbentley](https://github.com/markschlosseratbentley)
* Daniel Zhong, Bentley Systems, [@danielzhong](https://github.com/danielzhong)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Points are fundamental elements in many 3D modeling and computer-aided design (CAD) environments. In such visualizations, individual point primitives may differ in their styling, mainly focused around their width in screen pixels. Such width can be used to indicate a variety of meanings useful to CAD applications.

This specification describes a minimal extension sufficient to meet Bentley Systems' requirements in supporting CAD-style visualizations of points using glTF.

## Specifying Point Styles

The `BENTLEY_material_point_style` extension is applied to a material. When that material is used by a `POINTS` primitive (indicated by a value of `0` for [`mesh.primitive.mode`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_mesh_primitive_mode)), the extension dictates the width of those points in pixels.

### Width

The `width` property specifies the diameter of each point in pixels. When specified, this value must be greater than zero.

For each point, implementations should render the point such that a filled circle is centered at the point's pixel screen position with a pixel diameter equal to `width`. This point faces the camera.

## Implementation Notes

Some graphics APIs may not require that implementations directly support point sizes greater than one. For example, [Section 3.4 of the OpenGL ES 3.0 Specification](https://registry.khronos.org/OpenGL/specs/es/3.0/es_spec_3.0.pdf) only guarantees a maximum point size of at least one via `ALIASED_POINT_SIZE_RANGE`. In such cases, points with widths greater than one could be rendered as quads which are shaped into circles using a shader.

## JSON Schema

* [material.BENTLEY_material_point_style.schema.json](schema/material.BENTLEY_material_point_style.schema.json)
