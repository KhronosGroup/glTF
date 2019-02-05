# EXT_texture_webp

## Contributors

* Omar Shehata, Cesium, [@OmarShehata](https://github.com/OmarShehata)
* Pascal Massimino, Google, [@Skal65535](https://github.com/skal65535)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use WebP as a valid image format. A client that does not implement this extension can ignore the provided WebP image and continue to rely on the PNG and JPG textures available in the base specification. Defining a fallback texture is optional. The [best practices](#best-practices) section describes the intended use case of this extension and the expected behavior when using it without a fallback texture.

WebP is an image format developed and maintained by Google that provides superior lossless and lossy compression rates for images on the web. It is typically 26% smaller in size compared to PNGs and 25-34% smaller than comparable JPEG images - [source](https://developers.google.com/speed/webp/).  

## glTF Schema Updates

The `EXT_texture_webp` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which in turn points to the WebP image.

The following glTF will load `image.webp` in clients that support this extension, and otherwise fall back to `image.png`.

```
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

When used in the glTF Binary (.glb) format the `images` node that points to the WebP image uses the `mimeType` value of `image/webp`. 

```
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

Since WebP is not as widely supported as JPEG and PNG, it is recommended to use this extension only when transmission size is a significant bottleneck. For example, in geospatial applications the total texture payload can range from gigabytes to terabytes of data. In those situations, WebP textures can be used without a fallback to improve storage and transmission.

When a fallback image is defined, this extension should not be present in `extensionsRequired`. This will allow all clients to render the glTF correctly, and those that support this extension can request the optimized WebP version of the textures.

### Using Without a Fallback

To use WebP images without a fallback, define `EXT_texture_webp` in both `extensionsUsed` and `extensionsRequired`. The `texture` node will then have only an `extensions` property as shown below.

```
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

If a glTF contains a WebP with no fallback texture and the browser does not support WebP, the client should either display an error or decode the WebP image at runtime (see [resource](#resources) for decoding libraries).

## Known Implementations

CesiumJS uses it to significantly cut load times for massive models that contain gigabytes of texture data (see [implementation and sample model](https://github.com/AnalyticalGraphicsInc/cesium/pull/7486)). 

## Resources

Google's [WebP developer page](https://developers.google.com/speed/webp/) provides information about the format as well as [pre-compiled and source code versions](https://developers.google.com/speed/webp/download) of an encoder and a decoder. The [WebP Compression Study](https://developers.google.com/speed/webp/docs/webp_study) is a detailed comparison between JPEG and WebP.

For browsers that do not natively support WebP, the libraries [libwebpjs](http://libwebpjs.appspot.com) and [webp_js](https://webmproject.github.io/libwebp-demo/webp_js/index.html) decode WebP images in JavaScript. [Sharp](http://sharp.pixelplumbing.com/en/stable/) is a NodeJS library for fast encode/decode of WebP using native modules (built on top of Google's implementation above).
