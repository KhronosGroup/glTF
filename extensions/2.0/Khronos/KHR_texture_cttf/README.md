# KHR_texture_cttf

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

The `KHR_texture_cttf` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which defines a reference to the KTX2 image compatible with the CTTF specification.

The following glTF will load `image.ktx2` in clients that support this extension, and otherwise fall back to `image.png`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_cttf": {
                    "source": 1
                }
            }
        }
    ],
    "images": [
        {
            "uri": "image.png"
        },
        {
            "uri": "image.ktx2",
            "extensions": {
                "KHR_image_ktx2": {
                    "supercompressionScheme": -1, // TBD, CTTF
                    "pixelWidth": 512,
                    "pixelHeight": 512,
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

[texture.KHR_texture_cttf.schema.json](schema/texture.KHR_texture_cttf.schema.json)

## Restrictions on CTTF Images

CTTF is a subset of [KTX file format, version 2](https://github.com/KhronosGroup/KTX-Specification/) defined in **TBD**.

When used with glTF 2.0, CTTF images must conform to these additional restrictions:

- Swizzling metadata must be `rgba` or omitted.
- Orientation metadata must be `rd` or omitted 
- Colorspace information in the DFD must match the expected usage.

### Using CTTF Images for Material Textures

When a CTTF image is used as a texture for glTF 2.0 core materials, it must be of **2D** type as defined in KTX2, Section 4.3. Namely:

- `pixelHeight` must be greater than 0.
- `pixelDepth` must be 0.
- `numberOfArrayElements` must be 0.
- `numberOfFaces` must be 1.

Other use cases may impose different restrictions on CTTF image type.

## Known Implementations

None.

## Resources

TBD.