# KHR_texture_basisu

## Contributors

* Members of 3D Formats Working Group
* Arseny Kapoulkine, [@zeuxcg](https://twitter.com/zeuxcg)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify textures using KTX v2 images with Basis Universal supercompression. An implementation of this extension can use such images as an alternative to the PNG or JPEG images available in glTF 2.0 for more efficient asset transmission and reducing GPU memory footprint. Furthermore, specifying mip map levels is possible.

When the extension is used, it's allowed to use value `image/ktx2` for the `mimeType` property of images that are referenced by the `source` property of `KHR_texture_basisu` texture extension object.

At runtime, engines are expected to transcode a universal texture format into some block-compressed texture format supported by the platform.

## glTF Schema Updates

The `KHR_texture_basisu` extension is added to the `textures` object and specifies a `source` property that points to the index of the `image` which defines a reference to the KTX v2 image with Basis Universal supercompression.

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

When used in the glTF Binary (GLB) format the `image` that points to the KTX v2 resource uses the `mimeType` value of `image/ktx2`.

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

To use KTX v2 image with Basis Universal supercompression without a fallback, define `KHR_texture_basisu` in both `extensionsUsed` and `extensionsRequired`. The `texture` object will then have its `source` property omitted as shown below.

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

## KTX v2 Images with Basis Universal Supercompression

To cover a broad range of use cases, this extension allows two Basis Universal formats: ETC1S with BasisLZ and UASTC with optional Zstandard compression.

- KTX header fields for ETC1S with BasisLZ payloads
  - `supercompressionScheme` must be `1` (BasisLZ).
  - DFD `colorModel` must be `KHR_DF_MODEL_ETC1S`.
  - RGB textures must use a single DFD channel with value `KHR_DF_CHANNEL_ETC1S_RGB`.
  - RGBA textures must use two DFD channels with values `KHR_DF_CHANNEL_ETC1S_RGB` and `KHR_DF_CHANNEL_ETC1S_AAA`.

- KTX header fields for UASTC payloads
  - `supercompressionScheme` must be `0` (None) or `2` (Zstandard).
  - DFD `colorModel` must be `KHR_DF_MODEL_UASTC`.
  - RGB textures must use a single DFD channel with value `KHR_DF_CHANNEL_UASTC_RGB`.
  - RGBA textures must use a single DFD channel with value `KHR_DF_CHANNEL_UASTC_RGBA`.

Regardless of the format used, these additional restrictions apply for compatibility reasons:

- Swizzling metadata (`KTXswizzle`) must be `rgba` or omitted.
- Orientation metadata (`KTXorientation`) must be `rd` or omitted.
- Colorspace information in the DFD must match the expected usage, namely:
  - For textures with color data, `colorPrimaries` must be set to `KHR_DF_PRIMARIES_BT709`; `transferFunction` to `KHR_DF_TRANSFER_SRGB`.
  - For textures with non-color data, `colorPrimaries` must be set to `KHR_DF_PRIMARIES_UNSPECIFIED`; `transferFunction` to `KHR_DF_TRANSFER_LINEAR`.
- `KHR_DF_FLAG_ALPHA_PREMULTIPLIED` flag must not be set unless the material's specification requires premultiplied alpha.
- When a texture refers to a sampler with mipmap minification or when the sampler is undefined, the KTX image should contain a full mip pyramid.
  > **Implementation Note**: Depending on the platform features available, engines may need to decompress the texture and generate missing mipmaps at runtime when a full mip pyramid is not available.
- `pixelWidth` and `pixelHeight` must be multiples of 4.
  > **Implementation Note**: Exporters may opt to make the dimensions powers of 2. Combined with all mip levels present this would ensure maximum compatibility.

### Using KTX v2 Images with Basis Universal Supercompression for Material Textures

When a texture referencing a KTX v2 image with Basis Universal supercompression is used for glTF 2.0 material maps (such as base color), the KTX v2 image must be of **2D** type as defined in the KTX v2 Specification, Section 4.1.

Exporters should choose the most appropriate universal texture format based on the texture's usage and content. As a general rule, textures with color data should use ETC1S while textures with non-color data (such as roughness-metallic or normal maps) should use UASTC.

When the high-quality GPU formats (ASTC or BC7) are not available at runtime, engines should transcode UASTC textures with non-color data to uncompressed RGBA to avoid artifacts.

## Known Implementations

None.

## Resources

[KTX File Format Specification, version 2](https://github.khronos.org/KTX-Specification/)

[KTX Reference Software](https://github.com/KhronosGroup/KTX-Software/)