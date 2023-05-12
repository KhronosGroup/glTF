# NV_image_exr

## Contributors

* Timothy Blut, NVIDIA, tblut@nvidia.com
* Lutz Kettner, NVIDIA, lkettner@nvidia.com

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF assets to use OpenEXR as a valid image format. This enables the use of high-dynamic range (HDR) and color precision texture data.

## glTF Schema Updates

The following example illustrates how the OpenEXR image file `image.exr` can be defined as an image in the `images` array:
```json
"images": [
    {
        "uri": "image.exr"
    }
]
```

When OpenEXR images are defined by providing a `bufferView`, the `mimeType` value should be `image/x-exr`:
```json
"images": [
    {
        "bufferView": 1,
        "mimeType": "image/x-exr"
    }
]
```

## Manual Detection of OpenEXR Images

An implementation can manually determine if an image in the `images` array is an OpenEXR image by checking the first four bytes to be `0x76`, `0x2f`, `0x31`, and `0x01` in this order. This can be needed if an implementation does not want to rely on correct MIME types or file extensions.

## Known Uses

* [NV_material_mdl glTF vendor extension](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/NV_materials_mdl)

## Resources

* [OpenEXR](https://www.openexr.com/)
