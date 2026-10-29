# KHR_gaussian_splatting_compression_lgsc

## Contributors
- Imed Bouazizi, Qualcomm
- Geert van der Auwera, Qualcomm

## Status
Draft

## Dependencies
Written against the glTF 2.0 specification.

This extension depends on the ratified `KHR_gaussian_splatting` extension.

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
If a primitive contains the `KHR_gaussian_splatting` extension and that object includes `extensions.KHR_gaussian_splatting_compression_lgsc`, then L-GSC decoding is required for that primitive.

At this time, there is no requirement for a backup uncompressed buffer.

The extension MUST be listed in `extensionsUsed` along with `KHR_gaussian_splatting`.

```json
"extensionsUsed": [
  "KHR_gaussian_splatting",
  "KHR_gaussian_splatting_compression_lgsc"
]
```

It MUST also be listed in `extensionsRequired`.

```json
"extensionsRequired": [
  "KHR_gaussian_splatting",
  "KHR_gaussian_splatting_compression_lgsc"
]
```

Note: `KHR_gaussian_splatting_compression_lgsc` extends `KHR_gaussian_splatting`, so both are required to interpret the decoded attributes correctly.

## Geometry type
The `mode` of the primitive MUST be `POINTS`.

## Schema example
Example L-GSC extension shown below. This extension only affects primitives containing Gaussian splat data.

This example assumes data trained in `srgb_rec709_display`. The `colorSpace` MUST describe the reconstructed color values, as defined by `KHR_gaussian_splatting`. Without an additional extension defining another color space, it MUST be `srgb_rec709_display` or `lin_rec709_display`. Scene-referred data MUST NOT be relabeled as display-referred without the corresponding color rendering.

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
              "colorSpace": "srgb_rec709_display",
              "extensions": {
                "KHR_gaussian_splatting_compression_lgsc": {
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

The `attributes` object MUST contain every spherical harmonic coefficient for each degree from 0 through `shDegree`. Partial degrees and missing lower degrees are invalid. It MUST NOT contain spherical harmonic coefficients above `shDegree`.

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
- `shDegree = 1` requires all coefficients of degrees 0 through 1, including `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` through `KHR_gaussian_splatting:SH_DEGREE_1_COEF_2`.
- `shDegree = 2` requires all coefficients of degrees 0 through 2, including `KHR_gaussian_splatting:SH_DEGREE_2_COEF_0` through `KHR_gaussian_splatting:SH_DEGREE_2_COEF_4`.
- `shDegree = 3` requires all coefficients of degrees 0 through 3, including `KHR_gaussian_splatting:SH_DEGREE_3_COEF_0` through `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6`.

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
| SH coefficients | `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` to `KHR_gaussian_splatting:SH_DEGREE_3_COEF_6` | `VEC3` | `FLOAT` | All coefficients through `shDegree` |

## Decoded attribute values

The decoded attribute arrays MUST conform to the ellipse kernel attribute definitions of `KHR_gaussian_splatting`.

- `POSITION` values MUST be expressed in the glTF coordinate system and units.
- `KHR_gaussian_splatting:SCALE` values MUST be linear and nonnegative. L-GSC v1.0 uses the INRIA log-scale representation. The encoder MUST convert positive linear glTF scales to natural logarithms before passing them to the reference codec. The loader MUST apply the exponential function to each decoded log-scale component before exposing the attribute values. A zero linear scale has no finite logarithm and MUST NOT be passed directly through this encoding path.
- `KHR_gaussian_splatting:OPACITY` values MUST be linear values in the range [0, 1]. The L-GSC v1.0 reference API accepts and returns INRIA opacity logits. The loader MUST apply the sigmoid function to the returned logits. The internal codec sigmoid preprocessing and its inverse do not replace this conversion. An implementation that exposes already activated opacity values MUST NOT apply the sigmoid function twice.
- `KHR_gaussian_splatting:ROTATION` values MUST be unit quaternions in glTF (x, y, z, w) order. The loader MUST reconstruct packed components, reorder components when required, and normalize the quaternion before exposing the attribute values.
- Spherical harmonic attributes MUST contain RGB coefficients in the basis and ordering defined by `KHR_gaussian_splatting`. For each degree l, coefficient indices 0 through 2l correspond to orders m = -l through +l. Codec channel rearrangement and color transforms MUST be reversed. The zeroth-order attribute stores SH coefficients, not reconstructed diffuse RGB values.

When converting data from another coordinate system, positions, rotations, and spherical harmonic coefficients MUST be transformed consistently into the glTF coordinate system. These requirements apply after codec decompression and before rendering. They do not change the L-GSC bitstream format.

## Accessors
Accessors MUST be defined for each attribute listed in `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_compression_lgsc.attributes`.

Each accessor MUST define `componentType`, `count`, and `type`.

The accessor `count` MUST match `numPoints`.

The accessor `type` and `componentType` describe the decompressed data after decoding.

## Conformance
A loader MUST follow this process.

1. If the loader does not support `KHR_gaussian_splatting_compression_lgsc`, it MUST fail to load the asset when the extension is required.
2. If the loader does support `KHR_gaussian_splatting_compression_lgsc` then it MUST do the following.
   1. Read the L-GSC payload from `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_compression_lgsc.bufferView`.
   2. Decode the payload to obtain attribute arrays for the attributes referenced by `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_compression_lgsc.attributes`.
   3. Convert the decoded arrays to the values specified in [Decoded attribute values](#decoded-attribute-values).
   4. When processing those referenced attributes, the loader MUST ignore any accessor `bufferView` and `byteOffset` and instead use the decoded streams.
   5. Any primitive attributes not listed in `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_compression_lgsc.attributes` MUST be processed normally.

## Implementation
This section is non normative.

A common implementation path is to decode the payload once, then either populate GPU buffers matching the accessor layout, or render directly from the decoded arrays.

An implementation may choose to also populate `COLOR_0` for point-cloud fallback. This attribute is outside of the `KHR_gaussian_splatting` semantics and is ignored by this extension.

## Schema
- [`schema/KHR_gaussian_splatting_compression_lgsc.schema.json`](schema/KHR_gaussian_splatting_compression_lgsc.schema.json)

## Known implementations
- Reference codec library and sample API usage are available in the Qualcomm lite-3Dgsplat-codec repository.

## Resources
- https://github.com/qualcomm/lite-3Dgsplat-codec
