# NV_opacity_micromap

## Contributors

- Nia Bickford, NVIDIA, [@NBickford-NV](https://github.com/NBickford-NV)
- Christoph Kubisch, NVIDIA, [@pixeljetstream](https://github.com/pixeljetstream)
- Martin-Karl Lefrancois, NVIDIA, [@mklefrancois](https://github.com/mklefrancois)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Depends on `NV_micromaps`.

References the Vulkan `VK_EXT_opacity_micromap` extension.

## Overview

This extension adds the ability for glTF primitives to express *opacity micromeshes*. These efficiently encode the opacity of microtriangles from a subdivided base mesh. In particular, using OMMs allows implementations of graphics API extensions such as `VK_EXT_opacity_micromap` to reduce any-hit shader invocations when ray tracing alpha-tested objects, such as foliage.

Opacity micromesh data comes from an *opacity micromesh* (OMM). This extension allows OMM data to be reused in several ways:

* Primitives **MAY** reference different regions of OMM data using *group* indices.
* Primitives **MAY** override several OMM properties, allowing common OMM data to be reused.
* Implementations **MAY** support an *index mapping buffer* that allows individual triangles' opacities to be reused.

## Extending Primitives

Here's an example (presented piece-by-piece) of how this extension extends glTF primitives. It describes a glTF asset with one opacity micromap (OMM) file. One glTF primitive references this OMM, and overrides the mesh-to-micromap-triangle mapping through the `mapIndices` accessors. A renderer would load the remaining settings from the OMM, other accessors, or use default values as described in the Properties section.

An opacity micromesh is defined by adding the `NV_opacity_micromap` extension to a glTF primitive. This indicates that instead of rendering each base triangle, the primitive **MUST** be rendered using an opacity micromap as defined in this spec and in `VK_EXT_opacity_micromap`.

```json
"meshes": [
    {
        "primitives": [
            {
                "indices": 0,
                "material": 0,
                "attributes": {
                    "POSITION": 1,
                    "NORMAL": 2,
                    "TEXCOORD_0": 3,
                    "TANGENT": 4
                },
                "extensions": {
                    "NV_opacity_micromap": {
                        "micromap": 0,
                        "mapIndices": 5
                    }
                }
            }
        ]
    }
]
```

`micromap` is an index into the `micromaps` array provided by the `NV_micromaps` extension.

```json
"extensions": {
    "NV_micromaps": {
        "micromaps": [
            {
                "uri": "example-opacity.bary"
            }
        ]
    }
}
```

`mapIndices` is the index of the accessor whose data overrides the OMM's index mapping buffer. In this example, we override it with unsigned short data; if we had 1000 triangles in the primitive, this accessor would contain 1000 elements, 2000 bytes of data. Accessors, buffer views, and buffers other than that used by the `mapIndices` in this example are replaced with ellipses here.

```json
"accessors": [
    {
        "bufferView": 0,
        "componentType": 5123,
        "type": "SCALAR",
        "count": 1000
    },
    ...
],
"bufferViews": [
    {
        "buffer": 0,
        "byteLength": 2000
    },
    ...
],
"buffers": [
    {
        "uri": "example.bin",
        "byteLength": 2000
    },
    ...
]
```

## Properties

The table below lists all properties of this extension.

The index mapping buffer may be specified in the OMM or using `mapIndices`. If both are specified, `mapIndices` takes precedence; if no index mapping buffer is specified, triangle `i` uses micromap primitive `i`.

| | Type | Description | Required |
|-|------|-------------|----------|
| **micromap** | `integer` | The index of the `micromap` in the `NV_micromaps` `micromaps` array to use for opacity information. | Yes |
| **groupIndex** | `integer` | A micromap **MAY** contain multiple *groups*, much like how a scene file **MAY** contain multiple meshes. This is the index of the MM group to use. | No, default: `0`. |
| **mapIndicesOffset** | `integer` | The element offset into the OMM's index mapping buffer (in the BARY format, this is the `eMeshTriangleMappings` standard property). If the OMM contains no index mapping buffer, then it defaults to the identity mapping: triangle ID `i` uses mesh primitive `i`. | No, default: `0`. |
| **mapIndices** | `integer` | The index of the `accessor` whose data overrides that referenced by `mapIndicesOffset`. The accessor's `type` **MUST** be `"SCALAR"`, and its `componentType` **MUST** be 5121, 5123, or 5125 (unsigned byte, short, or int). | No |
| **mapOffset** | `integer` | An offset to be added to each value of the index mapping buffer. It **MUST NOT** be negative. | No, default: `0`. |

## Micromap Value Lookup

An index mapping buffer allows a block of micromap data to be used by multiple base mesh triangles. When using the Vulkan `NV_displacement_micromap` extension, it is typically used as the `indexBuffer` field of `VkAccelerationStructureTrianglesOpacityMicromapEXT`.

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

switch(baryTriangleIndex)
{
case VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_TRANSPARENT_EXT:
case VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_OPAQUE_EXT:
case VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_UNKNOWN_TRANSPARENT_EXT:
case VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_UNKNOWN_OPAQUE_EXT:
  // All microtriangles in this triangle have the same opacity.
  // Handle special behavior, then exit.
  return;
default:
  // Proceed with fetching micromap data.
  break;
}

// Always apply mapOffset and the group's offset.
baryTriangleIndex += mapOffset + bary.properties.groups[ groupIndex ].triangleFirst;
// Get the final primitive information.
baryTriangle = bary.properties.triangles[ baryTriangleIndex ];

// Get the byte offset where values for this primitive start.
valuesByteOffset =  baryTriangle.valueByteOffset;
// Adjust it by the group's offset:
valuesByteOffset += bary.valuesInfo.valueByteSize * bary.properties.groups[ groupIndex ].valueFirst;
// Get a pointer to this primitive's data, starting at its first value:
valuesFirst      = &bary.values[ valuesByteOffset ];

// Then access values according to
//   bary.valuesInfo.valueFrequency (this must be ePerTriangle for an OMM),
//   bary.valuesInfo.valueLayout (e.g. are the values in bird curve order?,
//   bary.valuesInfo.valueFormat (e.g. value type size and compression),
//   baryTriangle.subdivLevel (e.g. how many values are there?), and
//   baryTriangle.blockFormat (if the format uses a compressed block format)
```

## Known Implementations

Currently supported in the [NVIDIA Micro-Mesh SDK](https://developer.nvidia.com/rtx/ray-tracing/micro-mesh).