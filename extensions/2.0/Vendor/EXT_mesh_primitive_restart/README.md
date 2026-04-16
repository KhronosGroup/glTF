<!--
Copyright 2025 Bentley Systems, Incorporated
SPDX-License-Identifier: CC-BY-4.0
-->

# EXT_mesh_primitive_restart

## Contributors

* Paul Connelly, Bentley Systems, [@pmconne](https://github.com/pmconne)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

"Primitive restart" is a feature of the input assembly stage that restarts the current primitive when the vertex index value is the maximum possible value for a given index buffer type. For example, the line strip primitive usually produces one continuous connected series of line segments, but with primitive restart enabled, a maximal vertex index value (e.g., 65535 for unsigned 16-bit integer indices) indicates the beginning of a new line string disconnected from those preceding it. Primitive restart can be useful for batching multiple line strips, line loops, triangle strips, or triangle fans into a single draw call. Alternatively, batching can be achieved by decomposing the primitives into lines or triangles, but this may introduce many redundant vertices, increasing the amount of data required to describe the geometry.

glTF 2.0 explicitly prohibits index buffers from containing maximal index values because support for primitive restart varies amongst graphics APIs. Per [section 3.7.2.1](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview) of the spec,

> `indices` accessor **MUST NOT** contain the maximum possible value for the component type used (i.e., 255 for unsigned bytes, 65535 for unsigned shorts, 4294967295 for unsigned ints).

This extension permits the above prohibition to be selectively relaxed, while providing a trivial fallback for implementations that don't support primitive restart.

## Example

Consider the simple example of a pair of line strings with a total of 5 vertices where vertices 0 and 1 make up the first line string and vertices 2, 3, and 4 make up the second. An unsigned byte index buffer representing these line strings using primitive restart would look like the following, where index 255 marks the disconnect between the two line strings:

```
[0, 1, 255, 2, 3, 4]
```

Over this same buffer, we can alternatively produce 2 separate accessors - one per line string - splitting at (and omitting) the prohibited primitive restart value:

```
[0, 1, 255, 2, 3, 4]
[0, 1]     [2, 3, 4]
```

This permits two equivalent representations of the geometry without duplicating any binary data. In glTF, the accessors look like this:

```json
    "accessors": [
        {
            "bufferView": 0,
            "count": 2,
            "componentType": 5121,
            "type": "SCALAR",
            "name": "Line string 1 indices"
        },
        {
            "bufferView": 0,
            "byteOffset": 3,
            "count": 3,
            "componentType": 5121,
            "type": "SCALAR",
            "name": "Line string 2 indices"
        },
        {
            "bufferView": 0,
            "componentType": 5121,
            "count": 6,
            "type": "SCALAR",
            "name": "All indices"
        },
        {
            "bufferView": 1,
            "componentType": 5126,
            "count": 5,
            "type": "VEC3",
            "max": [
                0.5,
                0.5,
                0.0
            ],
            "min": [
                -0.5,
                -0.5,
                0.0
            ],
            "name": "Positions accessor"
        }
    ],
```

The mesh looks like this:

```json
    "meshes": [
        {
            "primitives": [
                {
                    "attributes": {
                        "POSITION": 3
                    },
                    "indices": 0,
                    "material": 0,
                    "mode": 3
                },
                {
                    "attributes": {
                        "POSITION": 3
                    },
                    "indices": 1,
                    "material": 0,
                    "mode": 3
                }
            ],
            "extensions": {
                "EXT_mesh_primitive_restart": {
                    "primitiveGroups": [
                        {
                            "primitives": [
                                0,
                                1
                            ],
                            "indices": 2
                        }
                    ]
                }
            }
        }
    ],
```

By default, this mesh draws two separate line strip primitives, each using its own `indices` accessor. The `EXT_mesh_primitive_restart` extension specifies that both primitives can be replaced with a single one using a combined `indices` accessor containing primitive restart values.

## glTF Schema Updates

The `EXT_mesh_primitive_restart` extension is applied to a mesh. Its `primitiveGroups` property is a list of groups of primitives that can be replaced with a single primitive using primitive restart. Each group is described by a list of indices into `mesh.primitives`, along with the index of the accessor that supplies the vertex indices for the replacement primitive.

## Constraints

The extension is subject to the following constraints. Violation of any constraint renders the entire extension invalid, in which case the extension **SHOULD** be ignored and the `mesh.primitives` objects **SHOULD** be rendered as defined in the glTF 2.0 specification.

- A given primitive index **MUST NOT** appear in more than one primitive group.

- Each primitive index in a primitive group **MUST NOT** appear more than once in that group.

- Each primitive in each group **MUST** use one of the following topology types, as specified by the `mode` property: 2 (line loop), 3 (line strip), 5 (triangle strip), or 6 (triangle fan). No other topology types are permitted.

- All primitives in a given group **MUST** have identical property values (e.g., attributes, material, mode, etc), with the exception of `indices`. This includes the `extensions` property - e.g., if any primitive in a group has a `KHR_materials_variants` extension object, then all other primitives in the same group **MUST** have that extension with identical content.

- Each primitive in each group **MUST** define an `indices` property, i.e., they **MUST** use indexed geometry.

- The `indices` accessor specified by each primitive group **MUST** be a valid index accessor as per the base glTF 2.0 specification, i.e., their types **MUST** be scalar, their component types **MUST** be any of the unsigned integer types, and their buffer views (if defined) **MUST NOT** be used for any purpose other than vertex indices.

- Primitives referred to by this extension **MUST NOT** have morph targets.

## JSON Schema

- [EXT_mesh_primitive_restart.schema.json](schema/EXT_mesh_primitive_restart.schema.json)
- [primitiveGroup.schema.json](schema/primitiveGroup.schema.json)

## Known Implementations

- [iTwin.js](https://github.com/iTwin/itwinjs-core/pull/8312)
