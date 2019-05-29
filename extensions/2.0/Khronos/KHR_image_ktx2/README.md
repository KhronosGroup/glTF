# KHR_image_ktx2

## Contributors

* Norbert Nopper, UX3D
* Alexey Knyazev
* Members of 3D Formats Working Group

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify images using the [KTX2 file format](http://github.khronos.org/KTX-Specification/) (KTX2). An implementation of this extension can use the images provided in the KTX2 files as an alternative to the PNG or JPG images available in glTF 2.0. Furthermore, specifying images having advanced features such as mip map levels, cube map faces, or arrays is possible.

When the extension is used, it's allowed to use value `image/ktx2` for the `image.mimeType` property.

A JSON form of the KTX2 header is provided in the extension object, so implementations can check whether the image is compatible with the GPU and allocate memory before fetching the actual image data. Also, this allows immediately requesting needed parts (e.g., mip levels) of the KTX2 file by skipping the binary KTX2 header.

Because of glTF compatibility requirements, the image cannot be used directly from any glTF core object, so additional extensions are required to use KTX2 images with glTF textures. The following example shows a complete glTF 2.0 asset containing only an uncompressed RGBA8 2D image with BGRA swizzling metadata and without precomputed mip levels.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": ["KHR_image_ktx2"],
    "images": [
        {
            "uri": "texture.ktx2",
            "extensions": {
                "KHR_image_ktx2" : {
                    "vkFormat": 37,
                    "pixelWidth": 512,
                    "pixelHeight": 512,
                    "levels": [
                        {
                            "byteOffset": 256,
                            "byteLength": 1048576,
                        }
                    ],
                    "metadata": {
                        "KTXswizzle": "bgra"
                    }
                }
            }
        }
    ]
}
```

### Validation and implementation notes

The information in the extension object must exactly match KTX2 header.

JSON type of each metadata entry depends on the type of the original value. The JSON-Schema provides types for metadata keys defined in the KTX2 specification.

This extension does not restrict texel formats or other features of the KTX2 images. Such limitations must be defined by other glTF extensions referring to this extension.

> **Implementation Note:** The core glTF 2.0 spec allows only PNG and JPEG images, so references to KTX2 images can appear only in extension objects.


## glTF Schema Updates

* **JSON schema**: [image.KHR_image_ktx2.schema.json](schema/image.KHR_image_ktx2.schema.json)

## Known Implementations

None.