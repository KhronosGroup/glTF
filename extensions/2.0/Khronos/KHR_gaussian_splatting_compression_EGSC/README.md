# KHR_gaussian_splatting_compression_EGSC

## Contributors
- Chenxi Tu, Huawei
- Pei Wang, Huawei
- Xin Zhao, Huawei
- Hao Li, Huawei
- Kangying Cai, Huawei
- Yiyi Liao, Zhejiang University
- Lu Yu, Zhejiang University
- Huiwen Ren, Xiaohongshu (RedNote)

## Status
Draft

## Dependencies
Based on the glTF 2.0 specification.

This extension extends `KHR_gaussian_splatting` to add EGSC compression support.

## Overview
EGSC (Efficient Gaussian Splatting Codec) introduces a hybrid compression framework designed for Gaussian splatting data. It compresses standard Gaussian attributes into a single binary payload, which is then decompressed into rendering-ready arrays. Furthermore, leveraging hardware-accelerated texture formats(e.g., ASTC/UASTC), EGSC stores high-order Spherical Harmonics (SH) coefficients as GPU-native textures. This allows the compressed high-order SH data to be accessed directly by the GPU without CPU-side decompression, significantly accelerating decoding while reducing both VRAM usage and DDR bandwidth overhead. This makes EGSC essential for  delivering high-quality 3DGS experiences on resource-constrained devices like mobile phones.

EGSC is also adopted by UWA (UHD World Association) in its `6DoF Interactive 3D Image Format (6DoF-IIF)` Standard for delivering 3DGS contents.

This extension defines how EGSC-compressed gaussian splat data is carried within a glTF `bufferView`.

## EGSC codec description
This section is non normative.

Social media sharing and e-commerce require a 3DGS codec that delivers high compression efficiency, fast encoding and decoding, rendering-friendly decoding, low power consumption, and minimal memory usage. EGSC addresses these demands by proposing a hybrid compression framework that leverages existing hardware coding capabilities. EGSC decoder supports both fully decoded before rendering and online decoding during rendering. To balance compression efficiency with coding latency, EGSC employs attribute-specific coding strategies, applying distinct compression methods to different Gaussian attributes.

## EGSC data encapsulation in glTF
Through the `KHR_gaussian_splatting_compression_EGSC` extension, EGSC-compressed Gaussian splat data can be encapsulated directly withiin glTF. This extension is placed under the primitive and extends the base KHR_gaussian_splatting extension.

When a primitive carries the `KHR_gaussian_splatting` extension and its extensions include `KHR_gaussian_splatting_compression_EGSC`, the Gaussian Splating attributes of that primitive MUST be interpreted using EGSC decoder.

The extension MUST be declared in both extensionsUsed and extensionsRequired:

```json
"extensionsUsed": [
  "KHR_gaussian_splatting",
  "KHR_gaussian_splatting_compression_EGSC"
],

"extensionsRequired": [
  "KHR_gaussian_splatting",
  "KHR_gaussian_splatting_compression_EGSC"
]
```

Note: `KHR_gaussian_splatting_compression_EGSC` is an extension that builds upon `KHR_gaussian_splatting`. Accordingly, both extensions MUST be present and supported to properly decode and render the asset.


## Schema example
The following example illustrates the EGSC extension applied to a Gaussian splat primitive.

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
              "sortingMethod": "cameraDistance",
              "projection": "perspective",
              "extensions": {
                "KHR_gaussian_splatting_compression_EGSC": {
                  "bufferView": 0
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
      "uri": "0.egsc",
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
Required. The index of the bufferView resource containing the EGSC-compressed bitstream.

## Accessors
Accessor requirements are modified from the base KHR_gaussian_splatting extension with the following adjustments to definition:

* Accessor type is defined for the resulting type after decompression and dequantization has occurred.
* The accessor count must match the number of points in the compressed EGSC data.

## Conformance
Loaders conforming to this extension MUST adhere to the following:

1. Loaders that do not recognize `KHR_gaussian_splatting_compression_EGSC` MUST reject the asset when the extension is listed in `extensionsRequired`.
2. Loaders that do support this extension MUST:
   1. Retrieve the EGSC payload from `KHR_gaussian_splatting.extensions.KHR_gaussian_splatting_compression_EGSC.bufferView`.
   2. Decode the payload to obtain attribute arrays. Depending on the renderer's capabilities:
      * If the renderer performs full decoding: All attributes are fully decompressed into GPU-accessible buffers.
      * If hardware support is available: High-order SH coffecients in GPU texture formats can be loaded directly, thereby reducing VRAM usage.


## Schema
- [`schema/KHR_gaussian_splatting_compression_EGSC.schema.json`](schema/KHR_gaussian_splatting_compression_EGSC.schema.json)


## Resources
- Codec code link: to be updated (coming soon)
