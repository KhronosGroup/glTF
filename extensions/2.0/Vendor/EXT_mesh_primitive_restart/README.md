<!--
Copyright 2025 Bentley Systems, Incorporated
SPDX-License-Identifier: CC-BY-4.0
-->

# EXT_mesh_primitive_restart

## Contributors

- Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)
- Don McCurdy, Bentley Systems, [@donmccurdy](https://github.com/donmccurdy)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

"Primitive restart" is a feature of the input assembly stage that restarts the current primitive when the vertex index value is the maximum possible value for a given index buffer type. For example, the line strip primitive usually produces one continuous connected series of line segments, but with primitive restart enabled, a maximal vertex index value (e.g., 65535 for unsigned 16-bit integer indices) indicates the beginning of a new line string disconnected from those preceding it. Primitive restart can be useful for batching multiple line strips, line loops, triangle strips, or triangle fans into a single draw call. Alternatively, batching can be achieved by decomposing the primitives into lines or triangles, but this may introduce many redundant vertices, increasing the amount of data required to describe the geometry.

glTF 2.0 explicitly prohibits index buffers from containing maximal index values because support for primitive restart varies amongst graphics APIs. Per [section 3.7.2.1](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview) of the spec,

> `indices` accessor **MUST NOT** contain the maximum possible value for the component type used (i.e., 255 for unsigned bytes, 65535 for unsigned shorts, 4294967295 for unsigned ints).

This extension removes the restriction above, allowing `indices` accessors to contain the maximum possible value for the component type in select primitive draw modes, and specifying that these values indicate primitive restart commands.

Because the extension does not provide a way to specify fallback indices without restart values, files that use the extension must specify it in `extensionsRequired` array - the extension is not optional.

> **Implementation Note:** Implementations on graphics APIs without primitive restart may still support the extension, by rewriting primitive indices. Compared to processing many more primitives and accessors, the extension may still provide performance advantages even for these implementations, in assets with many primitives.

## Extending Mesh Indices

When the `EXT_mesh_primitive_restart` extension is supported, `indices` accessors may contain the maximum possible index values, as primitive restart values, for the following primitive draw modes:

- `2 LINE_LOOP`
- `3 LINE_STRIP`
- `5 TRIANGLE_STRIP`
- `6 TRIANGLE_FAN`

The applicable primitive restart value is determined by the accessor component type:

| `accessor.componentType`     | restart value |
| ---------------------------- | ------------- |
| `5121`&nbsp;(UNSIGNED_BYTE)  | `255`         |
| `5123`&nbsp;(UNSIGNED_SHORT) | `65535`       |
| `5125`&nbsp;(UNSIGNED_INT)   | `4294967295`  |

## Example

Consider the simple example of a pair of line strings with a total of 5 vertices where vertices 0 and 1 make up the first line string and vertices 2, 3, and 4 make up the second. An unsigned byte index buffer representing these line strings using primitive restart would look like the following, where index 255 marks the disconnect between the two line strings:

```
[0, 1, 255, 2, 3, 4]
```

Without `EXT_mesh_primitive_restart`, this pair of line strings would require two separate mesh primitives - one per line string - with two separate indices accessors, splitting at (and omitting) the prohibited primitive restart value:

```
[0, 1, 255, 2, 3, 4]
[0, 1]     [2, 3, 4]
```

For large collections of line strings and other primitive topologies, encoding indices with restart values can greatly reduce the number of mesh primitives and accessors required, and the associated JSON data.

## JSON Schema

The `"EXT_mesh_primitive_restart"` string must be added to the root-level `extensionsUsed` and `extensionsRequired` arrays. The extension is always required. No additional extensions are added to meshes or mesh primitives; all mesh primitives with applicable draw modes are permitted to use primitive restart values.

## Known Implementations

- TODO
