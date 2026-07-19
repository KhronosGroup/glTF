<!--
Copyright 2015-2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# KHR_gaussian_splatting_wide_gamut_color

## Contributors

- Alexey Knyazev, Khronos
- Adam Morris, Cesium
- Zehui Lin, Huawei
- Michael Nikelsky, Autodesk

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Extends KHR_gaussian_splatting.

## Overview

This extension adds wide-gamut color support to the KHR_gaussian_splatting extension. It introduces several additional wide-gamut color space options for the `colorSpace` property. These additional color spaces enable more accurate color representation for high dynamic range (HDR) content and wide-gamut displays when using Gaussian splatting in glTF assets.

## Additional Color Spaces

| Color Space | Description |
| --- | --- |
| BT.2020-ITU | BT.2020-ITU color space. |
| BT.2020-linear | BT.2020 linear color space. |
| BT.2100-PQ | BT.2100 PQ (Perceptual Quantizer) color space. |
| BT.2100-HLG | BT.2100 HLG (Hybrid Log-Gamma) color space. |
| Display-P3 | Display P3 color space. |

## Extending the base KHR_gaussian_splatting extension

The base `KHR_gaussian_splatting` extension is required for this extension to be used. The new `colorSpace` values can be used in place of the existing ones defined in the base extension.

To extend the base extension, include both extensions in the glTF asset's `extensionsUsed` array:

```json
{
  "extensionsUsed": [
    "KHR_gaussian_splatting",
    "KHR_gaussian_splatting_wide_gamut_color"
  ]
}
```

Then add the `KHR_gaussian_splatting_wide_gamut_color` extension to the `extensions` property of the `KHR_gaussian_splatting` object:

```json
{
  "meshes": [
    {
      "primitives": [
        {
          // snip...
          "extensions": {
            "KHR_gaussian_splatting": {
              "colorSpace": "Display-P3",
              "extensions": {
                "KHR_gaussian_splatting_wide_gamut_color": {}
              }
            }
          }
        }
      ]
    }
  ]
}
```

There are no additional properties introduced by this extension; it solely extends the `colorSpace` options of the base extension.

## Known Implementations

- TODO: List of known implementations, with links to each if available.

## Resources

- BT.2020: https://www.itu.int/rec/R-REC-BT.2020
- BT.2100: https://www.itu.int/rec/r-rec-bt.2100
- Display P3: https://www.color.org/chardata/rgb/DisplayP3.xalter
