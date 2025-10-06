# KHR\_gaussian\_splatting\_compression\_spz\_2

## Contributors

- Jason Sobotka, Cesium
- Renaud Keriven, Cesium
- Adam Morris, Cesium
- Sean Lilley, Cesium
- Projit Bandyopadhyay, Niantic Spatial
- Daniel Knoblauch, Niantic Spatial
- Ronald Poirrier, Esri
- Jean-Philippe Pons, Esri

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

Depends on the `KHR_gaussian_splatting` extension specification for attribute definitions.

## Table of Contents

- [Overview](#overview)
- [Compressing 3D Gaussian splats using SPZ](#compressing-3d-gaussian-splats-using-spz)
  - [Geometry Type](#geometry-type)
  - [Schema Example](#schema-example)
  - [Extension Properties](#extension-properties)
  - [Accessors](#accessors)
- [Conformance](#conformance)
- [Schema](#schema)
- [Known Implementations](#known-implementations)
- [Resources](#resources)

## Overview

This extension defines support for compressing 3D Gaussian splats stored within in glTF using [v2 of the SPZ compression format](https://github.com/nianticlabs/spz/tree/v2.0.0) for efficient streaming and storage.

SPZ is a compression format from Niantic Spatial designed for Gaussian splats. Currently, it is open sourced under the MIT license. The SPZ format is primarily used in the Niantic Spatial Scaniverse app. It was purpose built for Gaussian splats and offers a balance of high compression with minimal visual fidelity loss, and allows for storing some or all of the 3D Gaussian's spherical harmonics.

At rest, the 3D Gaussian splats are stored within the SPZ compression format. Upon being decompressed, values are mapped to the glTF attributes defined by `KHR_gaussian_splatting` for rendering. The [conformance](#conformance) section defines what an implementation must do when encountering this extension, and how the extension interacts with the base specification.

## Compressing 3D Gaussian splats using SPZ

To use this extension, it must be defined in the `extensions` property in a `KHR_gaussian_splatting` extension definition. Any mesh primitive using `KHR_gaussian_splatting` that is using this extension will use the SPZ payload to retreive the values for `POSITION`, `COLOR_0`, `KHR_gaussian_splatting:SCALE`, `KHR_gaussian_splatting:ROTATION`, and all `KHR_gaussian_splatting:SH_DEGREE_ℓ_COEF_n` attributes. The [attribute mapping](#attribute-mapping) section defines how the SPZ data is mapped to these attributes.

The extension must then be listed in `extensionsUsed` alongside `KHR_gaussian_splatting`.

```json
  "extensionsUsed" : [
    "KHR_gaussian_splatting",
    "KHR_gaussian_splatting_compression_spz"
  ]
```

### Inheritance from base extension

As this extension extends the base extension, all components of the base extension are inherited. Any modifications to the base definition are explicitly defined in the sections below.

### Schema Example

Example SPZ extension shown below. This extension only affects any `primitive` nodes containting Gaussian splat data. A `bufferView` is provided by the extension which points to where the SPZ blob is stored.

```json
  "meshes": [{
      "primitives": [{
          "attributes": {
            "POSITION": 0,
            "COLOR_0": 1,
            "KHR_gaussian_splatting:SCALE": 2,
            "KHR_gaussian_splatting:ROTATION": 3,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_0": 4,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_1": 5,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_2": 6
          },
          "material": 0,
          "mode": 0,
          "extensions": {
            "KHR_gaussian_splatting": {
              "extensions": {
                "KHR_gaussian_splatting_compression_spz": {
                  "bufferView": 0,
                }
              }
            }
          }
        }]
    }],
  "buffers": [{
      "uri": "0.bin",
      "byteLength": 9753142
    }],
  "bufferViews": [{
      "buffer": 0,
      "byteLength": 9753142
    }],
```

### Extension Properties

#### bufferView

This property points to the bufferView containing the Gaussian splat data compressed with SPZ.

### Attribute Mapping

Data may be used directly from SPZ or may be mapped to the placeholder attributes this extension provides. When mapping to attributes, the data from SPZ is mapped from the `GaussianCloudData` struct in SPZ to the following attributes in glTF. Several fields require additional conversion to make them glTF and renderer ready.

| SPZ `GaussianCloudData` field | glTF Attribute | Required Conversion |
| --- | --- |
| `positions` | `POSITION` | |
| `colors` | `COLOR_0` RGB components | Compute _0.5 + 0.282095 * x_ to get the color between 0 and 1. |
| `alphas` | `COLOR_0` A component | Compute the sigmoid to get the alpha between 0 and 1. (Sigmoid formula: _1 / (1 + e^-x)_) |
| `scales` | `KHR_gaussian_splatting:SCALE` | Compute the base-e exponential of each scale value. (e.g. _e^x_ or `std::exp(x)`) |
| `rotations` | `KHR_gaussian_splatting:ROTATION` | |
| `sh` index 0 to 2 | `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` | |
| `sh` index 3 to 5 | `KHR_gaussian_splatting:SH_DEGREE_1_COEF_2` | |
| `sh` index 6 to 8 | `KHR_gaussian_splatting:SH_DEGREE_1_COEF_3` | |
| `sh` index 9 to 11 | `KHR_gaussian_splatting:SH_DEGREE_2_COEF_0` | |
| `sh` index 12 to 14 | `KHR_gaussian_splatting:SH_DEGREE_2_COEF_1` | |
| `sh` index 15 to 17 | `KHR_gaussian_splatting:SH_DEGREE_2_COEF_2` | |
| `sh` index 18 to 20 | `KHR_gaussian_splatting:SH_DEGREE_2_COEF_3` | |
| `sh` index 21 to 23 | `KHR_gaussian_splatting:SH_DEGREE_2_COEF_4` | |
| `sh` index 24 to 26 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_0` | |
| `sh` index 27 to 29 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_1` | |
| `sh` index 30 to 32 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_2` | |
| `sh` index 33 to 35 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_3` | |
| `sh` index 36 to 38 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_4` | |
| `sh` index 39 to 41 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_5` | |
| `sh` index 42 to 44 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6` | |

### Accessors

Accessor requirements are modified from the base `KHR_gaussian_splatting` extension with the following adjustments to definition:

 - Accessor `type` is defined for the resulting type after decompression and dequantization has occurred.
 - The accessor `count` must match the number of points in the compressed SPZ data.

## Conformance

The recommended process for handling SPZ compression is as follows:

- If the loader does not support `KHR_gaussian_splatting_compression_spz` and `accessor.bufferView` is undefined, other extensions may be providing the data.
- If the loader does not support `KHR_gaussian_splatting_compression_spz` and `accessor.bufferView` is defined, accessor data should be sourced as usual.
- If the loader does support `KHR_gaussian_splatting_compression_spz` then the loader must process `KHR_gaussian_splatting_compression_spz` data first. The loader must get the data from `KHR_gaussian_splatting_compression_spz`'s `bufferView` extension property.

This allows for a graceful fallback when an implementation does not support this extension.
  
When compressing or decompressing the SPZ data to be stored within the glTF, you must specify a Left-Up-Front (`LUF`) coordinate system in the SPZ `PackOptions` or `UnpackOptions` within the SPZ library. This ensures that the data is compressed and decompressed appropriately for glTF.

## Schema

[SPZ Compression Schema](./schema/mesh.primitive.KHR_gaussian_splatting_compression_spz.schema.json)

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

[https://github.com/nianticlabs/spz](https://github.com/nianticlabs/spz)

[https://github.com/drumath2237/spz-loader/tree/main](https://github.com/drumath2237/spz-loader/tree/main)
