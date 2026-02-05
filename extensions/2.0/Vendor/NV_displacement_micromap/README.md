# NV_displacement_micromap

## Contributors

- Nia Bickford, NVIDIA, [@NBickford-NV](https://github.com/NBickford-NV)
- Christoph Kubisch, NVIDIA, [@pixeljetstream](https://github.com/pixeljetstream)
- Martin-Karl Lefrancois, NVIDIA, [@mklefrancois](https://github.com/mklefrancois)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Depends on `NV_micromaps`.

References the OpenGL `GL_HALF_FLOAT` type and the Vulkan `VK_NV_displacement_micromap` extension.

## Overview

This extension adds the ability for glTF primitives to define geometry in the form of a *displaced micromesh*. This allows highly detailed geometry, defined using displacements from a subdivided base mesh, to be stored in a compact format, which an implementation **MAY** efficiently render without conversion or expansion.

Displacements are per-microvertex, and do not depend on texture coordinates. Each triangle **MAY** have a different *subdivision level*, and *primitive flags* indicate how to generate microtriangles to avoid T-junctions. This allows different parts of a base mesh to be subdivided at different levels of detail while preserving watertightness properties.

*Directions* and *direction bounds* together define a volumetric shell around the base mesh that contains the micromesh geometry when using a bounded format. A tightly fitting shell improves quality under compression and quantization, and also enables efficient occlusion culling.

Displaced micromesh data comes from a *displacement micromap* (DMM). This extension allows DMM data to be reused in several ways:

* Primitives **MAY** reference different regions of DMM data using *group* indices.
* Primitives **MAY** override several DMM properties, allowing common DMM data to be reused.
* Implementations **MAY** support an *index mapping buffer* that allows individual triangles' displacements to be reused.

This extension also introduces a *half float* accessor component type (if not provided by another extension) to efficiently store floating-point data.

| Note                                                         |
| ------------------------------------------------------------ |
| This extension has been designed together with the BARY data format so that its micromesh data can be used directly with the Vulkan `VK_NV_displacement_micromap` extension, if the BARY data uses the `bary::Format::eDispC1_r11_unorm_block` value format. |

## Extending Accessor Component Types

This extension adds a *half float* accessor component type. A half float is 16 bits long, and contains data according to the binary16 format in the IEEE 754-2008 standard.

| `componentType` | Data Type    | Signed | Bits |
| --------------- | ------------ | ------ | ---- |
| 5131            | *half float* | Signed | 16   |

| Note                                              |
| ------------------------------------------------- |
| This corresponds to the OpenGL `HALF_FLOAT` type. |

## Extending Primitives

Here's an example (presented piece-by-piece) of how this extension extends glTF primitives. It describes a glTF asset with one DMM, stored in a BARY file. One glTF primitive references this DMM using the `micromap` property, selects group 1 using the `group` property, and overrides the DMM's direction bounds using the `directionBounds` property. A renderer would load the remaining displaced micromesh settings from the DMM, other accessors, or use default values as described in the Properties section.

A displaced micromesh is defined by adding the `NV_displacement_micromap` extension to a glTF primitive. This indicates that instead of rendering each base triangle, the primitive **MUST** be rendered using micromesh geometry as defined in this spec and in `VK_NV_displacement_micromap`.

```json
"meshes": [
    {
        "primitives": [
            {
                "indices": 1,
                "material": 0,
                "attributes": {
                    "POSITION": 2,
                    "NORMAL": 3,
                    "TANGENT": 4
                },
                "extensions": {
                    "NV_displacement_micromap": {
                        "micromap": 0,
                        "groupIndex": 1,
                        "directionBounds": 0
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
                "uri": "example-displacement.bary"
            }
        ]
    }
}
```

`directionBounds` is the index of an accessor whose data overrides the DMM's direction bounds for this primitive. This example overrides it with half float data. Accessors, buffer views, and buffers other than that used by the `directionBounds` in this example are replaced with ellipses here.

```json
"accessors": [
    {
        "bufferView": 0,
        "componentType": 5131,
        "type": "VEC2",
        "count": 1000
    },
    ...
],
"bufferViews": [
    {
        "buffer": 0,
        "byteLength": 4000
    },
    ...
],
"buffers": [
    {
        "uri": "example.bin",
        "byteLength": 4000
    },
    ...
]
```

## Properties

The table below lists all properties of this extension.

Several properties may be specified using an element offset into a property contained within a DMM, or as an accessor index (these glTF properties are named in `XOffset` and `X` pairs, where `X` is a string). When the accessor index property exists in a primitive, it overrides the DMM property.

|  | Type | Description | Required |
|-|------|-------------|----------|
| **micromap** | `integer` | The index of the `micromap` in the `NV_micromaps` `micromaps` array to use for displacement information. | Yes |
| **groupIndex** | `integer` | A micromap **MAY** contain multiple *groups*, much like how a scene file **MAY** contain multiple meshes. This is the index of the MM group to use. | No, default: `0` |
| **mapIndicesOffset** | `integer` | The element offset into the DMM's index mapping buffer (in the BARY format, this is the `eMeshTriangleMappings` standard property). If the DMM contains no index mapping buffer, then it defaults to the identity mapping: triangle ID `i` uses mesh primitive `i`. | No, default: `0` |
| **mapIndices** | `integer` | The index of the `accessor`whose data overrides that referenced by `mapIndicesOffset`. The accessor's `type` **MUST** be `"SCALAR"`, and its `componentType` **MUST** be 5121, 5123, or 5125 (unsigned byte, unsigned short, or unsigned int). | No |
| **mapOffset** | `integer` | An offset to be added to each value of the index mapping buffer. It **MUST NOT** be negative. | No, default: `0` |
| **directionsOffset** | `integer` | The element offset into the DMM's vertex displacement direction vectors (in the BARY format, this is the `eMeshDisplacementDirections` standard property). There is 1 direction per primitve vertex. If the DMM contains no directions, then they default to the vertex normals; if the primitive has no vertex normals, this property **MUST** be overridden. | No, default: `0` |
| **directions** | `integer` | The index of the `accessor` whose data overrides that referenced by `directionsOffset`. The accessor's `type` **MUST** be either `"VEC3"` with a `componentType` of 5126 (float), or it **MUST** be `"VEC4"` with a `componentType` of 5126 or 5131 (float or half float). | No |
| **directionBoundsOffset** | `integer` | The element offset into the DMM's vertex displacement direction bounds (in the BARY format, this is the `eMeshDisplacementDirectionBounds` standard property). There is 1 direction bound per primitive vertex. If the DMM contains no per-vertex displacement direction bounds, then they default to a *bias* of 0 and a *scale* of 1 for each vertex. | No, default: `0` |
| **directionBounds** | `integer` | The index of the `accessor` whose data overrides that referenced by `directionBoundsOffset`. The accessor's `type` **MUST** be `"VEC2"`, and its `componentType` **MUST** be 5126 or 5131 (float or half float). | No |
| **primitiveFlagsOffset** | `integer` | The element offset into the DMM's flags per base mesh primitive, such as adjacency bits for bridging between triangles with different subdivision levels. (In the BARY format, this is the `eMeshTriangleFlags` standard property). If the DMM does not contain primitive flags, then they default to all 0. | No, default: `0`. |
| **primitiveFlags** | `integer` | The index of the `accessor` whose data overrides that referenced by `primitiveFlagsOffset`. The accessor's `type` **MUST** be `"SCALAR"`, and its `componentType` **MUST** be 5121 (unsigned byte). | No |

## Micromap Value Lookup

An index mapping buffer allows a block of micromap data to be used by multiple base mesh triangles. When using the Vulkan `NV_displacement_micromap` extension, it is typically used as the `indexBuffer` field of `VkAccelerationStructureTriangleDisplacementMicromapNV`.

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
//   baryTriangle.blockFormat (if the format uses a compressed block format)
```

## Direction Bounds

Each vertex has a *direction bound* in the form of a 2-element vector, containing a *bias* and a *scale*. The scale adjusts the length of displacements, and the bias moves displacements in or out along the direction vector.

More specifically, direction bounds apply the following adjustments to vertex `v` before displacement is computed (direction bounds are indexed using the primitive's indices):

``` cpp
position_bounded[v]  = position[v] + direction[v] * direction_bounds[v].x;
direction_bounded[v] = direction[v] * direction_bounds[v].y;
```

The resulting displacement is applied on the linear interpolation of the triangle's adjusted vertex values:

``` cpp
uvec3 triangle = uvec3( indices[t * 3 + 0],
                        indices[t * 3 + 1],
                        indices[t * 3 + 2]);

position_microvertex  = linear_interpolate( position_bounded[triangle.x],
                                            position_bounded[triangle.y],
                                            position_bounded[triangle.z],
                                            microvertex_wuv);

direction_microvertex = linear_interpolate( direction_bounded[triangle.x],
                                            direction_bounded[triangle.y],
                                            direction_bounded[triangle.z],
                                            microvertex_wuv);
// Final position.
position_microvertex = position_microvertex + direction_microvertex * displacement;
```

where `microvertex_wuv` denotes the barycentric coordinates of the microvertex relative to its base triangle.

Note that BARY DMMs also have a *group bias* and *group scale*. These are mutually exclusive with the direction bounds: if direction bounds aren't referenced in glTF, then the group bias and scale act as a direction bound bias and scale for the entire primitive. More specifically, if the glTF primitive has `directionBounds` or the DMM has direction bounds, then the first component of the BARY group bias and scale **MUST** be 0.0 and 1.0, respectively.

## Known Implementations

Currently supported in the [NVIDIA Micro-Mesh SDK](https://developer.nvidia.com/rtx/ray-tracing/micro-mesh).