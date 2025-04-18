# KHR\_spz\_compression

## Contributors

- Jason Sobotka, Cesium
- Adam Morris, Cesium
- Sean Lilley, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Table of Contents

- [Overview](#overview)
- [Adding Gaussian Splats](#adding-spz-compressed-data-to-primitives)
  - [Extension Properties](#extension-properties)
  - [Accessors](#accessors)
- [Conformance](#conformance)
- [Implementation](#implementation)
- [Schema](#schema)
- [Known Implementations](#known-implementations)
- [Resources](#resources)

## Overview

SPZ is a compression format from Niantic Labs designed for Gaussian Splatting and available as open source under the MIT licensed. SPZ was purpose built for Gaussian Splatting and offers a balance of high compression with minimal visual fidelity loss.

This extension allows glTF to support streaming SPZ compressed data instead of uncompressed or meshopt compressed Gaussian splat data.

## Adding Gaussian Splats compressed with SPZ to Primitives

If a primitive contains an `extension` property which defines `KHR_spz_compression` then SPZ compression is required. At this time, there is no requirement for a backup uncompressed buffer.

The extension must be listed in `extensionsUsed` along with `KHR_gaussian_splatting`

```json
  "extensionsUsed" : [
    "KHR_spz_compression", "KHR_gaussian_splatting"
  ]
```

It must also be listed in `extensionsRequired`

```json
  "extensionsRequired" : [
    "KHR_spz_compression"
  ]
```

Note that while `KHR_spz_compression` is required, `KHR_gaussian_splatting` is not and may fallback to rendering points.

### Geometry Type

The `mode` of the `primitive` must be `POINTS`.

### Schema Example

Example SPZ extension shown below. This extension only affects any `primitive` nodes containting Gaussian splat data.

```json
  "meshes": [{
      "primitives": [{
          "attributes": {
            "_ROTATION": 4,
            "COLOR_0": 2,
            "_SCALE": 3,
            "_OPACITY": 1,
            "POSITION": 0
          },
          "material": 0,
          "mode": 0,
          "extensions": {
            "KHR_spz_compression": {
              "bufferView": 0,
              "numPoints": 590392,
              "shDegree": 0,
              "fractionalBits": 12,
              "flags": 0,
              "attributes": {
                "_ROTATION": 4,
                "COLOR_0": 2,
                "_SCALE": 3,
                "_OPACITY": 1,
                "POSITION": 0
              }
            }
          }
        }]
    }],
  "buffers": [{
      "uri": "0.spz",
      "byteLength": 9753142
    }],
  "bufferViews": [{
      "buffer": 0,
      "byteLength": 9753142
    }],
```

### Extension Properties

#### bufferView

This property points to the bufferView containing the compressed SPZ data.

#### attributes

This contains the attributes that will map into the compressed SPZ data indicated by `bufferView`. At minimum it will contain `POSITION`, `COLOR_0`, `_ROTATION`, and `_SCALE`. `COLOR_1` to `COLOR_15` are used to hold the spherical harmonic data and will vary depending on how many are present.

| Splat Data | glTF Attribute | Accessor Type | Component Type | Required |
| --- | --- | --- | --- | --- |
| Position | POSITION | VEC3 | float | yes |
| Color (Spherical Harmonic 0 (Diffuse)) | COLOR_0 | VEC3 or VEC4 | unsigned byte or float normalized | yes |
| Alpha | _OPACITY | SCALAR | float or unsigned byte | no |
| Rotation | _ROTATION | VEC4 | float | yes |
| Scale | _SCALE | VEC3 | float | yes |
| Spherical Harmonics | COLOR_1 - COLOR_15 | VEC4 | float | no |

If `_OPACITY` is omitted, see [Implementation](#implementation) below.

#### SPZ header properties embedded in the compressed data stream

The following properties are within the SPZ stream header. They are exposed here to be informative and facilitate decoder setup prior to any decompression occurring.

##### numPoints

The number of Gaussian splats in the compressed data.

##### fractionalBits

The number of fraction bits to use for `POSITION` quantization.

##### shDegree

The degree of spherical harmonics contained in the compressed data. This can range from 0 to 3.

##### flags

A set of bit flags reserved for future use. Currently it is only used for an anti-aliasing flag.

### Accessors

```json
  "accessors": [{
      "componentType": 5126,
      "count": 590392,
      "type": "VEC3"
    }, {
      "componentType": 5126,
      "count": 590392,
      "type": "SCALAR"
    }, {
      "componentType": 5121,
      "count": 590392,
      "type": "VEC3"
    }, {
      "componentType": 5126,
      "count": 590392,
      "type": "VEC3"
    }, {
      "componentType": 5126,
      "count": 590392,
      "type": "VEC4"
    }],
```

At minimum accessors will be defined for `POSITION`, `COLOR_0`, `_ROTATION`, and `_SCALE`. Each must have `componentType`, `count`, and `type` defined.

Accessor `type` is defined for the resulting type after decompression and dequantization has occurred.

The accessor `count` must match `numPoints` in `KHR_spz_compression`.

## Conformance

The recommended process for handling SPZ compression is as follows:

- If the loader does not support `KHR_spz_compression`, it must fail.
- If the load does support `KHR_spz_compression` then:
  - The loader must process `KHR_spz_compression` data first. The loader must get the data from `KHR_spz_compression`'s `bufferView` property.
  - The loader then must process `attributes` of the `primitive`. When processing the loader must ignore any `bufferView` and `byteOffset` in the `accessor` and instead use values derived from the decompressed data streams. This data can be used to populate the `accessors` or render directly.
  - Any attributes not listed in `KHR_spz_compression`'s `attributes` but are listed in the `primitive` must then be processed by the loader.

## Implementation

*This section is non-normative.*

After the SPZ data stream has been decompressed, when populating the `accessors` you may insert the alpha values into `COLOR_0` instead of a separate `_OPACITY` attribute.

The `COLOR_0` property type will be `VEC3` if it doesn't contain the alpha channel and `VEC4` otherwise. These values can be converted to `unsigned byte` at this time if desired otherwise they can be left `float`.

## Schema

[SPZ Compression Schema](./schema/mesh.primitive.KHR_spz_compression.schema.json)

## Known Implementations

This is currently implemented within  [3D Tiles and CesiumJS as an experimental feature](https://github.com/CesiumGS/cesium/tree/splat-spz-concept).

## Resources

[https://github.com/nianticlabs/spz](https://github.com/nianticlabs/spz)

[https://github.com/drumath2237/spz-loader/tree/main](https://github.com/drumath2237/spz-loader/tree/main)
