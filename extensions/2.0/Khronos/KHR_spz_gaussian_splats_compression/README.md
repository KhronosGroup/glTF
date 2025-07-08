# KHR\_spz\_gaussian\_splats\_compression

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

This extension defines support for compressing 3D Gaussian splats stored within in glTF using the SPZ compression format for efficient streaming and storage.

SPZ is a compression format from Niantic Spatial designed for Gaussian splats. Currently, it is open sourced under the MIT license. The SPZ format is primarily used in the Niantic Spatial Scaniverse app. It was purpose built for Gaussian splats and offers a balance of high compression with minimal visual fidelity loss, and allows for storing some or all of the 3D Gaussian's spherical harmonics.

At rest, the 3D Gaussian splats are stored within the SPZ compression format. Upon being decompressed, values are mapped to the glTF attributes defined by `KHR_gaussian_splatting` for rendering. The [conformance](#conformance) section defines what an implementation must do when encountering this extension, and how the extension interacts with the base specification.

## Compressing 3D Gaussian splats using SPZ

If a primitive contains an `extension` property which defines both `KHR_gaussian_splatting` and `KHR_spz_gaussian_splats_compression` then support for SPZ compression is required. There is no requirement for a backup uncompressed buffer.

The extension must be listed in `extensionsUsed` alongside `KHR_gaussian_splatting`.

```json
  "extensionsUsed" : [
    "KHR_gaussian_splatting",
    "KHR_spz_gaussian_splats_compression"
  ]
```

It must also be listed in `extensionsRequired`. When `KHR_spz_gaussian_splats_compression` is in use, the `KHR_gaussian_splatting` extension must also be listed as required.

```json
  "extensionsRequired" : [
    "KHR_gaussian_splatting",
    "KHR_spz_gaussian_splats_compression"
  ]
```

### Geometry Type

Geometry type requirements are defined by the `KHR_gaussian_splatting` extension.

### Schema Example

Example SPZ extension shown below. This extension only affects any `primitive` nodes containting Gaussian splat data. Note that unlike the base `KHR_gaussian_splatting` extension, the `indices` property is excluded, and a `bufferView` is provided by the extension. This bufferview points to where the SPZ blob is stored.

```json
  "meshes": [{
      "primitives": [{
          "attributes": {
            "POSITION": 0,
            "COLOR_0": 1,
            "_SCALE": 2,
            "_ROTATION": 3,
            "_SH_DEGREE_1_COEF_0": 4,
            "_SH_DEGREE_1_COEF_1": 5,
            "_SH_DEGREE_1_COEF_2": 6
          },
          "material": 0,
          "mode": 0,
          "extensions": {
            "KHR_spz_gaussian_splats_compression": {
              "bufferView": 0,
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

#### attributes

Attribute requirements are defined in the base `KHR_gaussian_splatting` extension.

### Accessors

Accessor requirements are defined in the base `KHR_gaussian_splatting` extension with the following adjustments:

 - SPZ compressed attributes must not include `bufferView` nor `byteOffset`. (See: [Conformance](#conformance))
 - Accessor `type` is defined for the resulting type after decompression and dequantization has occurred.
 - The accessor `count` must match the number of points in the compressed SPZ data.

## Conformance

The recommended process for handling SPZ compression is as follows:

- If the loader does not support `KHR_gaussian_splatting` and `KHR_spz_gaussian_splats_compression`, it must fail.
- If the loader does support `KHR_gaussian_splatting` and `KHR_spz_gaussian_splats_compression` then:

  - The loader must process `KHR_spz_gaussian_splats_compression` data first. The loader must get the data from `KHR_spz_gaussian_splats_compression`'s `bufferView` property.
  - SPZ compressed attributes must not include `bufferView` nor `byteOffset` in their accessor declarations. Any attributes falling outside the SPZ format, however, must be stored as regular glTF attributes and must therefore include `bufferView` (and optionally `byteOffset`) in their accessor definitions.
  
When compressing or decompressing the SPZ data to be stored within the glTF, you must specify a Left-Up-Front (`LUF`) coordinate system in the SPZ `PackOptions` or `UnpackOptions` within the SPZ library. This ensures that the data is compressed and decompressed appropriately for glTF.

## Schema

[SPZ Compression Schema](./schema/mesh.primitive.KHR_spz_gaussian_splats_compression.schema.json)

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

[https://github.com/nianticlabs/spz](https://github.com/nianticlabs/spz)

[https://github.com/drumath2237/spz-loader/tree/main](https://github.com/drumath2237/spz-loader/tree/main)
