# KHR_texture_basisu

## Contributors

* Members of 3D Formats Working Group

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify textures using KTX2 images with Basis Universal supercompression. An implementation of this extension can use such images as an alternative to the PNG or JPEG images available in glTF 2.0 for more efficient asset transmission and reducing GPU memory footprint. Furthermore, specifying mip map levels is possible.

When the extension is used, it's allowed to use value `image/ktx2` for the `image.mimeType` property.

## glTF Schema Updates

The `KHR_texture_basisu` extension is added to the `textures` object and specifies a `source` property that points to the index of the `image` which defines a reference to the KTX2 image with Basis Universal supercompression.

The following glTF will load `image.ktx2` in clients that support this extension, and otherwise fall back to `image.png`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_basisu"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_basisu": {
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
            "uri": "image.ktx2"
        }
    ]
}
```

When used in the glTF Binary (GLB) format the `image` that points to the KTX2 resource uses the `mimeType` value of `image/ktx2`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_basisu"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_basisu": {
                    "source": 1
                }
            }
        }
    ],
    "images": [
        {
            "mimeType": "image/png",
            "bufferView": 1
        },
        {
            "mimeType": "image/ktx2",
            "bufferView": 2
        }
    ]
}
```

### Using Without a Fallback

To use KTX2 image with Basis Universal supercompression without a fallback, define `KHR_texture_basisu` in both `extensionsUsed` and `extensionsRequired`. The `texture` object will then have its `source` property omitted as shown below.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_basisu"
    ],
    "extensionsRequired": [
        "KHR_texture_basisu"
    ],
    "textures": [
        {
            "extensions": {
                "KHR_texture_basisu": {
                    "source": 0
                }
            }
        }
    ],
    "images": [
        {
            "uri": "image.ktx2"
        }
    ]
}
```

### JSON Schema

[texture.KHR_texture_basisu.schema.json](schema/texture.KHR_texture_basisu.schema.json)

## Restrictions on KTX2 Images with Basis Universal Supercompression

When used with glTF 2.0, KTX2 images with Basis Universal supercompression must conform to these additional restrictions:

- Swizzling metadata (`KTXswizzle`) must be `rgba` or omitted.
- Orientation metadata (`KTXorientation`) must be `rd` or omitted.
- Colorspace information in the DFD must match the expected usage, namely:
  - Color primaries must be set to `KHR_DF_PRIMARIES_BT709`.
  - Transfer function must be either `KHR_DF_TRANSFER_LINEAR` or `KHR_DF_TRANSFER_SRGB`.

### Using KTX2 Images with Basis Universal Supercompression for Material Textures

When a texture referencing a KTX2 image with Basis Universal supercompression is used for glTF 2.0 material maps (such as base color), the KTX2 image must be of **2D** type as defined in the KTX2 Specification, namely:

- `pixelHeight` must be greater than 0.
- `pixelDepth` must be 0.
- `layerCount` must be 0.
- `faceCount` must be 1.

In general, a KTX2 image used for material texture should contain a full mip pyramid.

Other use cases (defined by other extensions) may impose different restrictions.

## Known Implementations

None.

## Resources

[KTX File Format Specification, version 2](https://github.khronos.org/KTX-Specification/)