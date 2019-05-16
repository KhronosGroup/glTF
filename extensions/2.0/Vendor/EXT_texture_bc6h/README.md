# EXT_texture_bc6h

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

The `EXT_texture_bc6h` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which defines a reference to the KTX2 image with BC6H payload.

The following glTF will load `image_hi.ktx2` in clients that support this GPU format, and otherwise fall back to CTTF `image_lo.ktx2`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_cttf", "KHR_image_ktx2", "EXT_texture_bc6h"
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
                "EXT_texture_bc6h": {
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
            "uri": "image_bc6h.ktx2",
            "extensions": {
                "KHR_image_ktx2": {
                    "vkFormat": 144, // VK_FORMAT_BC6H_SFLOAT_BLOCK
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

[texture.EXT_texture_bc6h.schema.json](schema/texture.EXT_texture_bc6h.schema.json)

## Restrictions on KTX2 Features

KTX2 images must conform to these additional restrictions:

- Swizzling metadata must be `rgba` or omitted.
- Orientation metadata must be `rd` or omitted 
- Colorspace information in DFD must match the expected usage.
- `vkFormat` must be 143 (`VK_FORMAT_BC6H_UFLOAT_BLOCK`) or 144 (`VK_FORMAT_BC6H_SFLOAT_BLOCK`).

## Known Implementations

None.

## Resources

TBD.