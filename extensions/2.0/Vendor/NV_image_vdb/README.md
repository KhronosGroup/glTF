# NV_image_vdb

## Contributors

* Timothy Blut, NVIDIA, tblut@nvidia.com
* Lutz Kettner, NVIDIA, lkettner@nvidia.com

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF assets to use OpenVDB as a valid image format. This enables the use of volumetric data.

## glTF Schema Updates

The following example illustrates how the OpenVDB volume file `volume.vdb` can be defined as an image in the `images` array:
```json
"images": [
    {
        "uri": "volume.vdb"
    }
]
```

When OpenVDB images are defined by providing a `bufferView`, the `mimeType` value should be `application/x-vdb`:
```json
"images": [
    {
        "bufferView": 1,
        "mimeType": "application/x-vdb"
    }
]
```

## Known Uses

* [NV_materials_mdl glTF vendor extension](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/NV_materials_mdl)

## Resources

* [OpenVDB](https://www.openvdb.org/)
