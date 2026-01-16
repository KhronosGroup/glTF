# KHR_gaussian_splatting_lgsc

## Contributors
- Imed Bouazizi, Qualcomm
- Geert van der Auwera, Qualcomm

## Status
Draft

## Dependencies
Written against the glTF 2.0 specification.

This extension depends on `KHR_gaussian_splatting`.

## Overview
L-GSC v1.0 is a codec for Gaussian Splat representations. It compresses the standard Gaussian splat attribute arrays into a single binary payload and later decompresses them back into attribute arrays suitable for rendering.

This extension enables glTF to carry L-GSC compressed Gaussian splat data in a `bufferView`, instead of uncompressed attributes or meshopt compressed attributes.

## L-GSC codec description
This section is non normative.

L-GSC (Lite Gaussian Splat Codec) is a scalar quantization and gzip based codec optimized for fast decoding. It encodes the standard INRIA Gaussian splat attributes (POSITION, SH DC, SH rest, OPACITY, SCALE, ROTATION) into a framed binary payload with the following structure:

- Header segment (uncompressed): number of splats, per attribute bit depths, codec flags, and per dimension min/max values used for quantization.
- Geometry segment: quantized positions, optionally Morton sorted to improve compression. Interleaving flags control whether values are stored by point or by dimension.
- Attribute segment: quantized SH DC, SH rest, opacity, scale, and rotation data, encoded in a fixed attribute order.

Before quantization, optional preprocess steps may be applied to improve compression. These include YUV conversion for SH rest coefficients, sigmoid mapping for opacity, rearranging SH rest coefficients to group channels, and a quaternion packing scheme that stores the index of the largest component and three remaining components. Each segment is length prefixed; geometry and attribute segments are typically gzip compressed.

Decoding reverses the process by reading the header metadata, inverse quantizing each stream, and applying the inverse transforms. The codec treats splats as order agnostic, so the original point order may not be preserved if Morton sorting is enabled.

## Adding L-GSC compressed data to primitives
If a primitive contains the `KHR_gaussian_splatting` extension and that object includes `extensions.KHR_gaussian_splatting_lgsc`, then L-GSC decoding is required for that primitive.

At this time, there is no requirement for a backup uncompressed buffer.

The extension MUST be listed in `extensionsUsed` along with `KHR_gaussian_splatting`.

```json
"extensionsUsed": [
  "KHR_gaussian_splatting",
  "KHR_gaussian_splatting_lgsc"
]
```

It MUST also be listed in `extensionsRequired`.

```json
"extensionsRequired": [
  "KHR_gaussian_splatting",
  "KHR_gaussian_splatting_lgsc"
]
```

Note: `KHR_gaussian_splatting_lgsc` extends `KHR_gaussian_splatting`, so both are required to interpret the decoded attributes correctly.

## Geometry type
The `mode` of the primitive MUST be `POINTS`.

## Schema example
Example L-GSC extension shown below. This extension only affects primitives containing Gaussian splat data.

```json
{
  "meshes": [
    {
      "primitives": [
        {
          "attributes": {
            "POSITION": 0,
            "KHR_gaussian_splatting:OPACITY": 1,
            "KHR_gaussian_splatting:SH_DEGREE_0_COEF_0": 2,
            "KHR_gaussian_splatting:SCALE": 3,
            "KHR_gaussian_splatting:ROTATION": 4,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_0": 5,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_1": 6,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_2": 7
          },
          "material": 0,
          "mode": 0,
          "extensions": {
            "KHR_gaussian_splatting": {
              "kernel": "ellipse",
              "colorSpace": "lin_rec709_scene",
              "extensions": {
                "KHR_gaussian_splatting_lgsc": {
                  "bufferView": 0,
                  "numPoints": 590392,
                  "shDegree": 1,
                  "compLevel": 2,
                  "flags": 0,
                  "attributes": {
                    "POSITION": 0,
                    "KHR_gaussian_splatting:OPACITY": 1,
                    "KHR_gaussian_splatting:SH_DEGREE_0_COEF_0": 2,
                    "KHR_gaussian_splatting:SCALE": 3,
                    "KHR_gaussian_splatting:ROTATION": 4,
                    "KHR_gaussian_splatting:SH_DEGREE_1_COEF_0": 5,
                    "KHR_gaussian_splatting:SH_DEGREE_1_COEF_1": 6,
                    "KHR_gaussian_splatting:SH_DEGREE_1_COEF_2": 7
                  }
                }
              }
            }
          }
        }
      ]
    }
  ],
  "buffers": [
    {
      "uri": "0.lgsc",
      "byteLength": 8123456
    }
  ],
  "bufferViews": [
    {
      "buffer": 0,
      "byteLength": 8123456
    }
  ]
}
```

## Extension properties

### bufferView
Type: `integer`. Required.

The index of the `bufferView` that contains the L-GSC compressed bitstream.

### attributes
Type: `object`. Required.

Maps `KHR_gaussian_splatting` attribute semantic names to accessor indices for the decompressed streams.

At minimum it MUST contain the following keys.

- `POSITION`
- `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0`
- `KHR_gaussian_splatting:OPACITY`
- `KHR_gaussian_splatting:ROTATION`
- `KHR_gaussian_splatting:SCALE`

`KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` through `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6` MAY be present to carry higher-order spherical harmonic coefficients, depending on `shDegree`.

### numPoints
Type: `integer`. Required.

The number of Gaussian splats encoded in the bitstream.

The accessor `count` for each listed decompressed attribute MUST match `numPoints`.

### shDegree
Type: `integer`. Required. Range 0 to 3.

The spherical harmonic degree represented by the decompressed data. The number of higher order coefficient vectors is

`F = (shDegree + 1) * (shDegree + 1) - 1`

These coefficients are mapped to `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` through `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6`.

- `shDegree = 0` means only `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` is present.
- `shDegree = 1` means `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` to `KHR_gaussian_splatting:SH_DEGREE_1_COEF_2` may be present.
- `shDegree = 2` means `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` to `KHR_gaussian_splatting:SH_DEGREE_2_COEF_4` may be present.
- `shDegree = 3` means `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` to `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6` may be present.

### compLevel
Type: `integer`. Optional. Range 0 to 2.

The compression level used during encoding. This is informative metadata.

### flags
Type: `integer`. Optional. Default 0.

Reserved bit flags for future use.

## Attribute mapping
This extension uses the same attribute names as `KHR_gaussian_splatting`.

| Splat data | glTF attribute | Accessor type | Component type | Required |
| --- | --- | --- | --- | --- |
| Position | `POSITION` | `VEC3` | `FLOAT` | yes |
| Diffuse color (SH0 DC) | `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` | `VEC3` | `FLOAT` | yes |
| Alpha | `KHR_gaussian_splatting:OPACITY` | `SCALAR` | `FLOAT` | yes |
| Rotation | `KHR_gaussian_splatting:ROTATION` | `VEC4` | `FLOAT` | yes |
| Scale | `KHR_gaussian_splatting:SCALE` | `VEC3` | `FLOAT` | yes |
| SH coefficients | `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` to `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6` | `VEC3` | `FLOAT` | no |

## Accessors
Accessors MUST be defined for each attribute listed in `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_lgsc.attributes`.

Each accessor MUST define `componentType`, `count`, and `type`.

The accessor `count` MUST match `numPoints`.

The accessor `type` and `componentType` describe the decompressed data after decoding.

## Conformance
A loader MUST follow this process.

1. If the loader does not support `KHR_gaussian_splatting_lgsc`, it MUST fail to load the asset when the extension is required.
2. If the loader does support `KHR_gaussian_splatting_lgsc` then it MUST do the following.
   1. Read the L-GSC payload from `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_lgsc.bufferView`.
   2. Decode the payload to obtain attribute arrays for the attributes referenced by `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_lgsc.attributes`.
   3. When processing those referenced attributes, the loader MUST ignore any accessor `bufferView` and `byteOffset` and instead use the decoded streams.
   4. Any primitive attributes not listed in `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_lgsc.attributes` MUST be processed normally.

## Implementation
This section is non normative.

A common implementation path is to decode the payload once, then either populate GPU buffers matching the accessor layout, or render directly from the decoded arrays.

An implementation may choose to also populate `COLOR_0` for point-cloud fallback. This attribute is outside of the `KHR_gaussian_splatting` semantics and is ignored by this extension.

## Schema
- [`schema/KHR_gaussian_splatting_lgsc.schema.json`](schema/KHR_gaussian_splatting_lgsc.schema.json)

## Known implementations
- Reference codec library and sample API usage are available in the Qualcomm lite-3Dgsplat-codec repository.

## Resources
- https://github.com/qualcomm/lite-3Dgsplat-codec
