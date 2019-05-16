# EXT_texture_astc_hdr

## Contributors

* Alexey Knyazev
* Members of 3D Formats Working Group

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Requires `KHR_image_ktx2` extension.

## Overview

**TBD**

## glTF Schema Updates

The `EXT_texture_astc_hdr` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which defines a reference to the KTX2 image with ASTC HDR payload.

The following glTF will load `image_hi.ktx2` in clients that support this GPU format, and otherwise fall back to CTTF `image_lo.ktx2`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_cttf", "KHR_image_ktx2", "EXT_texture_astc_hdr"
    ],
    "extensionsRequired": [
        "KHR_texture_cttf", "KHR_image_ktx2"
    ],
    "textures": [
        {
            "name": "Cubemap IBL texture",
            "extensions": {
                "KHR_texture_cttf": {
                    "source": 0
                },
                "EXT_texture_astc_hdr": {
                    "source": 1
                }
            }
        }
    ],
    "images": [
        {
            "uri": "image_cttf.ktx2",
            "extensions": {
                "KHR_image_ktx2": {
                    "supercompressionScheme": -1, // TBD, CTTF
                    "pixelWidth": 512,
                    "pixelHeight": 512,
                    "faces": 6,
                    "levels": [
                        {
                            "byteOffset": 1024,
                            "byteLength": 10485,
                        }
                    ]
                }
            }
        },
        {
            "uri": "image_astc.ktx2",
            "extensions": {
                "KHR_image_ktx2": {
                    "vkFormat": -1, // Vulkan ASTC HDR, TBD
                    "pixelWidth": 512,
                    "pixelHeight": 512,
                    "faces": 6,
                    "levels": [
                        {
                            "byteOffset": 1024,
                            "byteLength": 10485,
                        }
                    ]
                }
            }
        }
    ]
}
```

### JSON Schema

[texture.EXT_texture_astc_hdr.schema.json](schema/texture.EXT_texture_astc_hdr.schema.json)

## Restrictions on KTX2 Features

KTX2 images must conform to these additional restrictions:

- Swizzling metadata must be `rgba` or omitted.
- Orientation metadata must be `rd` or omitted 
- Colorspace information in DFD must match the expected usage.
- `vkFormat` must be from ASTC HDR family.

## Known Implementations

None.

## Resources

TBD.