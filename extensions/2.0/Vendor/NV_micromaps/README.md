# NV_micromaps

## Contributors

- Nia Bickford, NVIDIA, [@NBickford-NV](https://github.com/NBickford-NV)
- Christoph Kubisch, NVIDIA, [@pixeljetstream](https://github.com/pixeljetstream)
- Martin-Karl Lefrancois, NVIDIA, [@mklefrancois](https://github.com/mklefrancois)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. References the BARY file format.

## Overview

This extension adds the ability for glTF assets to have *micromap* (MM) data stored in buffers or files. A micromap stores an array of (formatted, possibly compressed) *values*, and **MAY** store additional *micromap properties*, such as vertex displacement directions.

Each glTF `primitive` **MAY** have extension properties that indicate how to use a micromap. The properties and the micromap together specify how to subdivide and modify a *base mesh* to produce a virtual *micromesh*.

This asset-level extension defines a `micromaps` array. Each of the elements in the `micromaps` array is an object that contains properties, much like an element of `images`: micromap data can be embedded, or linked as an external resource.

| Note                                                         |
| ------------------------------------------------------------ |
| Micromaps can be used in many different ways, and are typically used to store highly detailed microgeometry information in a compact format. For instance, *displacement micromaps* produce microgeometry by displacing vertices, *opacity micromaps* store per-microtriangle opacity information, and *attribute micromaps* can store microvertex attributes such as shading normals. Because applications may support a subset of micromap capabilities, this extension specifies the common interface for defining micromaps, to be used by other extensions.<br /><br />Types of micromap data can be used by graphics API extensions, such as `VK_EXT_opacity_micromap` and `VK_NV_displacement_micromap`. Importantly, implementations may render micromeshes without storing a subdivided mesh in memory.<br /><br />We reference the BARY file format for storing micromap data in this extension; in addition, applications may use custom micromap formats.<br /><br />In many cases, micromap extensions are designed so that the glTF file defines a valid asset if the micromap extensions are ignored. |

## Extending Assets

The glTF asset's `extensions` array **MAY** contain an `NV_micromaps` object, which in turn **MAY** contain a `micromaps` array. The term *micromap* refers interchangeably to an element of the `micromaps` array, or the data within a micromap format referenced at some level by an element of the `micromaps` array.

The following glTF asset snippet shows an example of such a `micromaps` array. This example array defines two micromaps: micromap 0 references a BARY file, and micromap 1 has a reference to a `bufferView`; although defined outside this snippet, this `bufferView` would reference micromap data within a buffer.

```json
"extensions": {
    "NV_micromaps": {
        "micromaps": [
            {
                "uri": "example-opacity.bary"
            },
            {
                "bufferView": 10,
                "mimeType": "model/vnd.bary"
            }
        ]
    }
}
```

## Properties
Each element of the `micromaps` array **MAY** contain a `mimeType`, and **MUST** contain one of
* a URI (or IRI) to an external file in one of the supported micromap formats, or
* a Data URI with embedded data, or
* a reference to a `bufferView`; in that case `mimeType` **MUST** be defined.

BARY micromaps **SHOULD** use the `model/vnd.bary` Media Type, and/or the `.bary` file extension. The format of the micromap data **MUST** match the `mimeType` property when the latter is defined.

Implementations **MAY** need to manually determine the media type of a micromap. In such a case, the 6 pattern bytes `0xAB 0x42 0x41 0x52 0x59 0x20` **SHOULD** be used to determine whether the media type is `model/vnd.bary`.

## Known Implementations

Currently supported in the [NVIDIA Micro-Mesh SDK](https://developer.nvidia.com/rtx/ray-tracing/micro-mesh).
