# EXT_texture_webp

## Contributors

* Omar Shehata, Cesium, [@OmarShehata](https://github.com/OmarShehata)
* Pascal Massimino, Google, [@Skal65535](https://github.com/skal65535)

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF assets to use WebP as a valid image format.

WebP is an image format developed and maintained by Google that provides superior lossless and lossy compression rates for images on the web. It is typically 26% smaller in size compared to PNGs and 25-34% smaller than comparable JPEG images.

A client implementation that does not support this extension may be able to ignore the provided WebP image if an optional fallback PNG or JPEG image is present. The [best practices](#best-practices) section describes the use cases of this extension with and without a fallback image.

## glTF Schema Updates

The `EXT_texture_webp` extension is added to the texture object and specifies a `source` property that contains the index of the WebP image object.

The following glTF will load `image.webp` in client implementations that support this extension, and otherwise fall back to `image.png`.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_webp": {
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
        "uri": "image.webp"
    }
]
```

When an image is stored in a buffer and accessed via a buffer view, the WebP image object uses the `mimeType` value of `image/webp`.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_webp": {
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
        "mimeType": "image/webp",
        "bufferView": 2
    }
]
```

### JSON Schema

[glTF.EXT_texture_webp.schema.json](schema/glTF.EXT_texture_webp.schema.json)

## Best Practices

It is recommended to use this extension when transmission size is a significant bottleneck. For example, in geospatial applications the total texture payload can range from gigabytes to terabytes of data. In those situations, WebP textures can be used without a fallback to improve storage and transmission.

When a fallback image is defined, this extension should not be present in `extensionsRequired`. This will allow all client implementations to render the glTF asset correctly, and those that support this extension can use the WebP version of the textures.

WebP images using extended file format (RFC 9649, Section 2.7) should not contain `ICCP`, `ANIM`, or `EXIF` chunks.

### Using Without a Fallback

To use WebP images without a JPEG or PNG fallback, `EXT_texture_webp` must be present in both `extensionsUsed` and `extensionsRequired`. The texture object will then have only an `extensions` property as shown below.

```json
"textures": [
    {
        "extensions": {
            "EXT_texture_webp": {
                "source": 1
            }
        }
    }
]
```

## Known Implementations

CesiumJS uses it to significantly cut load times for massive models that contain gigabytes of texture data (see [implementation and sample model](https://github.com/AnalyticalGraphicsInc/cesium/pull/7486)).

## Resources

[RFC 9649, WebP Image Format](https://www.rfc-editor.org/info/rfc9649)

Google's [WebP developer page](https://developers.google.com/speed/webp/) provides information about the format as well as pre-compiled and source code versions of the reference codec implementation

[Sharp](http://sharp.pixelplumbing.com/) is a Node.js library for fast encode/decode of WebP using native modules
