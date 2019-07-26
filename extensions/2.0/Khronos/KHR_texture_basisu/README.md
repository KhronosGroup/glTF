# KHR_texture_basisu

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

The `KHR_texture_basisu` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which defines a reference to the KTX2 image with Basis Universal supercompression.

The following glTF will load `image.ktx2` in clients that support this extension, and otherwise fall back to `image.png`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_basisu", "KHR_image_ktx2"
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

### JSON Schema

[texture.KHR_texture_basisu.schema.json](schema/texture.KHR_texture_basisu.schema.json)

## Restrictions on KTX2 Images with Basis Universal Supercompression

When used with glTF 2.0, KTX2 images with Basis Universal supercompression must conform to these additional restrictions:

- Swizzling metadata must be `rgba` or omitted.
- Orientation metadata must be `rd` or omitted 
- Colorspace information in the DFD must match the expected usage.

### Using KTX2 Images with Basis Universal Supercompression for Material Textures

When a KTX2 image with Basis Universal supercompression is used as a texture for glTF 2.0 core materials, it must be of **2D** type as defined in the KTX2 Specification, Section 4.3. Namely:

- `pixelHeight` must be greater than 0.
- `pixelDepth` must be 0.
- `layerCount` must be 0.
- `faceCount` must be 1.

Other use cases may impose different restrictions.

## Known Implementations

None.

## Resources

[KTX File Format Specification](https://github.com/KhronosGroup/KTX-Specification/)