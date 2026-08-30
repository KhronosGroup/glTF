# NV_attribute_micromap

## Contributors

- Nia Bickford, NVIDIA, [@NBickford-NV](https://github.com/NBickford-NV)
- Christoph Kubisch, NVIDIA, [@pixeljetstream](https://github.com/pixeljetstream)
- Martin-Karl Lefrancois, NVIDIA, [@mklefrancois](https://github.com/mklefrancois)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Depends on `NV_micromaps`.

## Overview

This extension adds support for arbitrary microtriangle attribute data in glTF micromeshes.

Mesh vertices often contain additional vertex attributes, such as normals, tangents, and texture coordinates. By default, microvertex attributes are linearly interpolated from base vertex attributes. Material textures, such as normal maps, can often represent the difference between interpolated and high-detail shading normals. However, storing microvertex attributes for each microvertex directly or in a compressed format can have storage and performance benefits.

*Attribute micromaps* (AMMs) **MAY** be used to store data for microvertex attributes. AMMs **MAY** also be used to store attribute data for faces of microtriangles.

## Extending Primitives

Here's an example (presented piece-by-piece) of how this extension extends glTF primitives. It describes a glTF scene with one attribute micromap (AMM), stored in a BARY file.

Microtriangle attribute information is added to a primitive by adding the `NV_attribute_micromap` extension. In this example, the primitive specifies normal information from the first micromap. A renderer would load any other attributes from other extensions, or interpolate them from the base mesh vertices.

```json
"meshes": [
    {
        "primitives": [
            {
                "indices": 1,
                "material": 0,
                "attributes": {
                    "POSITION": 2
                },
                "extensions": {
                    "NV_attribute_micromap": {
                        "NORMAL": 0
                    }
                }
            }
        ]
    }
]
```

`micromap` is an index into the `micromaps` array provided by the `NV_micromaps` extension:

```json
"extensions": {
    "NV_micromaps": {
        "micromaps": [
            {
                "uri": "example.bary"
            }
        ]
    }
}
```

## Properties

The table below lists all properties of this extension.

Several properties may be specified using an element offset into a property contained within the AMM, or as an accessor index (these glTF properties are named in `XOffset` and `X` pairs, where `X` is a string). When the accessor index property exists in a primitive, it overrides the AMM property.

Within a glTF primitive, all attributes' micromaps must use the same *value layout* and yield the same subdivision levels, index mapping buffers, and primitive flags. If the primitive has a displacement micromap, these restrictions also apply in combination with the displacement micromap.

| | Type | Description | Required |
|-|------|-------------|----------|
| **groupIndex** | `integer` | A micromap **MAY** contain multiple *groups*, much like how a scene file **MAY** contain multiple meshes. This is the index of the MM group to use. | No, default: `0` |
| **attributes** | `object` | A plain JSON object. For each key-value pair, the key corresponds to a mesh attribute semantic, and each value is the index of the micromap containing the data (using group 0) to use for microtriangles. **MUST** contain at least one key-value pair. | Yes |
| **mapIndicesOffset** | `integer` | The element offset into the AMM's index mapping buffer (in the BARY format, this is the `eMeshTriangleMappings` standard property). If the AMM has no index mapping buffer, it defaults to the identity mapping: triangle ID `i` uses mesh primitive `i`. | No, default, `0` |
| **mapIndices** | `integer` | The index of the `accessor`whose data overrides that referenced by `mapIndicesOffset`. The accessor's `type` **MUST** be `"SCALAR"`, and its `componentType` **MUST** be 5121, 5123, or 5125 (unsigned byte, short, or int). | No |
| **mapOffset** | `integer` | An offset to be added to each value of the index mapping buffer. It **MUST NOT** be negative. | No, default: `0` |
| **primitiveFlagsOffset** | `integer` | The element offset into the AMM's flags per base mesh primitive, such as adjacency bits for bridging between triangles with different subdivision levels. (In the BARY format, this is the `eMeshTriangleFlags` standard property). If the AMM has no primitive flags, the primitive flags default to all 0. | No, default: `0` |
| **primitiveFlags** | `integer` | The index of the `accessor` whose data overrides that referenced by `primitiveFlagsOffset`. The accessor's `type` **MUST** be `SCALAR`, and its `componentType` **MUST** be 5121 (unsigned byte). | No |

## Micromap Value Lookup

An index mapping buffer allows a block of micromap data to be used by multiple base mesh triangles.

The following informative pseudocode illustrates how the values of a `bary::Triangle` are read from a glTF triangle when storing micromap data in BARY format.

``` cpp
if (mapIndices)
{
  // The mapIndices glTF accessor has top priority.
  baryTriangleIndex = mapIndices[ triangleIndex ];
}
else if (bary.properties.mesh_triangle_mappings)
{
  // If the BARY eMeshTriangleMappings standard property type exists, use it.
  baryTriangleIndex = bary.properties.mesh_triangle_mappings[ triangleIndex + mapIndicesOffset ];
}
else
{
  // Otherwise, use the triangle index directly.
  baryTriangleIndex = triangleIndex;
}

// Always apply mapOffset and the group's offset.
baryTriangleIndex += mapOffset + bary.properties.groups[ groupIndex ].triangleFirst;
// Get the final primitive information.
baryTriangle = bary.properties.triangles[ baryTriangleIndex ];

// Get the byte offset where values for this primitive start
valuesByteOffset =  baryTriangle.valueByteOffset;
// Adjust it by the group's offset:
valuesByteOffset += bary.valuesInfo.valueByteSize * bary.properties.groups[ groupIndex ].valueFirst;
// Get a pointer to this primitive's data, starting at its first value:
valuesFirst      = &bary.values[ valuesByteOffset ];

// Then access values according to
//   bary.valuesInfo.valueFrequency (this must be ePerVertex for a DMM),
//   bary.valuesInfo.valueLayout (e.g. are the values in bird curve order?),
//   bary.valuesInfo.valueFormat (e.g. value type size and compression),
//   baryTriangle.subdivLevel (e.g. how many values are there?), and
//   baryTriangle.blockFormat (if applicable)
```

## Known Implementations

Currently supported in the [NVIDIA Micro-Mesh SDK](https://developer.nvidia.com/rtx/ray-tracing/micro-mesh).
