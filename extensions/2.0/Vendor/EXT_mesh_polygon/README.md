<!--
Copyright 2025 Bentley Systems, Incorporated
SPDX-License-Identifier: CC-BY-4.0
-->

# EXT_mesh_polygon

## Contributors

- Don McCurdy, Bentley Systems, [@donmccurdy](https://github.com/donmccurdy)
- TODO

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Extends glTF mesh primitives, adding an encoding of polygon primitive topology, including triangulation (indices) for backwards-compatible rendering. While the core glTF 2.0 specification already allows polygons to be triangulated and encoded as TRIANGLES, TRIANGLE_STRIP, or TRIANGLE_FAN primitive modes, the original topology — which triangles together form a polygon? — is lost without the additional specification and metadata provided by this extension.

## Extending Mesh Primitives

The `EXT_mesh_polygon` extension may be added to a mesh primitive, indicating that the primitive represents a series of polygons.

An extended mesh primitive **MUST** include `primitive.mode = 4` ("TRIANGLES"), `primitive.mode = 5` ("TRIANGLE_STRIP"), or `primitive.mode = 6` ("TRIANGLE_FAN").

An extended mesh primitive **MUST** include `indices`. Indices for each polygon must be contiguous: for a polygon composed of 4 triangles, indices defining these triangles must occupy a single contiguous range within the primitive's indices accessor. Primitive vertices associated with the polygon are not required to be contiguous.

The `EXT_mesh_polygon` extension includes the following additional properties, all required.

### count

Integer number of polygons encoded in the mesh primitive.

- **Type:** `number`
- **Required:** ✓ Yes
- **Minimum:** ≥ 1

### loopIndices

Index of the accessor containing indices of the polygons' exterior and interior loops. The accessor **MUST** have `SCALAR` type and an unsigned integer component type.

A polygon is composed of 1 or more loops, encoded as indices equivalent to `primitive.mode = 2` ("LINE_LOOP") topology. Each loop must be separated by the "primitive restart" value applicable to the accessor type:

| `accessor.componentType`     | restart value             |
| ---------------------------- | ------------------------- |
| `5121`&nbsp;(UNSIGNED_BYTE)  | `255` (0xFF)              |
| `5123`&nbsp;(UNSIGNED_SHORT) | `65535` (0xFFFF)          |
| `5125`&nbsp;(UNSIGNED_INT)   | `4294967295` (0xFFFFFFFF) |

The first loop in each polygon represents the polygon's exterior ring, or boundary. Additional loops, if any, represent interior rings ("holes") within the exterior ring. Polygons must be fully-connected — holes cannot intersect the exterior ring, and additional exterior rings ("islands") are not allowed, whether outside the exterior ring or within holes.

- **Type:** `number`
- **Required:** ✓ Yes
- **Minimum:** ≥ 0

### loopIndicesOffsets

Index of the accessor containing one integer offset per polygon in the primitive, indicating the first index of the first linear ring associated with that polygon. The accessor **MUST** have `SCALAR` type and an unsigned integer component type, and the accessor's `count` **MUST** be the same as `EXT_mesh_polygon.count` for the primitive.

All loops associated with a polygon MUST be contiguous, one (1) exterior ring followed immediately by zero or more interior rings.\

> **Implementation note:** The range of loop indices for the `nth` polygon is `loopIndicesOffsets[n]` to `loopIndicesOffsets[n+1]` if `n < count - 1`, otherwise `loopIndicesOffsets[n]` to the end of the `loopIndices` accessor.

- **Type:** `number`
- **Required:** ✓ Yes
- **Minimum:** ≥ 0

### indicesOffsets

Index of the accessor containing one integer offset per polygon in the primitive, indicating the first index of the first triangle associated with that polygon. The accessor **MUST** have `SCALAR` type and an unsigned integer component type, and the accessor's `count` **MUST** be the same as `EXT_mesh_polygon.count` for the primitive.

Indices for each polygon **MUST** be contiguous.

- **Type:** `number`
- **Required:** ✓ Yes
- **Minimum:** ≥ 0

## Additional Restrictions

Triangles in the extended mesh primitive **MUST** be associated with exactly one polygon. Loose triangles cannot be included, and a single triangle cannot be associated with multiple polygons.

Polygon `loopIndices` values **MUST** be associated with at least one triangle. Exterior and interior loops may not contain additional vertex indices missing from triangulation.

Polygon `loopIndices` values **MUST** be unique within each exterior or interior loop. The same vertex index cannot be used twice within a loop.

> **Implementation note:** As in LINE_LOOP topology, the first and last indices are defined to be connected by a line segment. Unlike in some geospatial formats, it is NOT necessary that the first index be repeated at the end of the loop to indicate a closed ring, and doing so would violate the requirement above.

## Example

_This section is non-normative._

The JSON example below shows a mesh having one mesh primitive, which contains 100 polygon primitives. The `indices` accessor defines the triangle indices required to draw the polygons, and the `loopIndices` accessor defines the `LINE_LOOP` indices delimiting the exterior (and interior, if holes are present) boundaries of each polygon. The `loopIndicesOffsets` and `indicesOffsets` accessors enable random access, allowing implementations to immediately find the particular triangle and loop indices associated with the Nth polygon.

```jsonc
{
  "extensionsUsed": ["EXT_mesh_polygon"],
  ...
  "meshes": [{
    "name": "MyMesh",
    "primitives": [{
      "mode": 4,
      "attributes": {
        "POSITION": 0,
      },
      "indices": 1,
      "extensions": {
		"EXT_mesh_polygon": {
	      "count": 100,
		  "loopIndices": 2,
	      "loopIndicesOffsets": 3,
	      "indicesOffsets": 4
		}
      }
    }]
  }]
}
```

Consider a simple mesh primitive containing three polygons, one with a single hole, and two without:

![Polygon encoding](./figures/polygon-encoding)

One valid encoding of `EXT_mesh_polygon` for this polygon set, based on `UNSIGNED_SHORT` indices and restart values, would be as follows:

```plaintext
indices
0 2 1 / 0 3 2 / 0 4 3
5 7 6 / 6 8 7
9 17 18 / 9 18 10 / 10 18 11 / ...

indicesOffsets
0 3 6

loopIndices
0 1 2 3 4 0xFFFF
5 6 7 8 0xFFFF
9 10 11 12 13 14 15 16 / 17 18 19 20

loopIndiceOffsets
0 6 11
```

## JSON Schema

The `"EXT_mesh_polygon"` string must be added to the root-level `extensionsUsed` array. Where preservation of polygon topology — not just display of equivalent triangles — is required, the string should also be added to the root-level `extensionsRequired` array. As the extension is intentionally backwards-compatible for rendering purposes, the extension is expected to be optional in most cases.

## Known Implementations

- TODO
