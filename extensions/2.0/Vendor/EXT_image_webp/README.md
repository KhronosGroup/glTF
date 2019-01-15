# EXT_image_webp

## Contributors

* Omar Shehata, Cesium, [@OmarShehata](https://github.com/OmarShehata)
* TBA, Google

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use WebP as a valid image format by extending the Image property. A client that does not implement this extension can ignore the provided WebP image and continue to rely on the PNG and JPG textures available in the base specification. The [best practices](#best-practices) section describes the expected behavior when using this extension without a fallback texture.

WebP is an image format developed and maintained by Google that provides superior lossless and lossy compression rates for images on the web. It is typically 26% smaller in size compared to PNGs and 25-34% smaller than comparable JPEG images - [source](https://developers.google.com/speed/webp/).  

## glTF Schema Updates

The `EXT_image_webp` extension may be defined on `image` structures. Its schema mirrors that of the the base Image property to simplify implementation.

The following glTF will load `image.webp` in clients that support this extension, and otherwise fall back to `image.png`.

```
"images": [
    {
        "uri": "image.png",
        "extensions": {
            "EXT_image_webp": {
                "uri": "image.webp"
            }
        }
    }
]
```

It can also be used with a `bufferView` instead just like the Image property in the base specification:

```
"images": [
    {
      "extensions": {
        "EXT_image_webp": {
          "bufferView": 3,
          "mimeType": "image/webp"
        }
      },
      "bufferView": 4,
      "mimeType": "image/png"
    }
  ]
```

### JSON Schema

[glTF.EXT_image_webp.schema.json](schema/glTF.EXT_image_webp.schema.json)

## Best Practices

It is recommended to provide a fallback PNG or JPEG image for clients that do not support this extension. When a fallback image is defined, this extension should be defined in `extensionsUsed`. This will allow all clients to render the glTF correctly, and those that support this extension will request the optimized WebP version of the textures.

### Using Without a Fallback

To use WebP images without a fallback, define `EXT_image_webp` in `extensionsRequired`. The `uri` of the base image object can then be set to an empty string.

```
"images": [
    {
        "uri": "",
        "extensions": {
        	"EXT_image_webp": {
        		"uri": "image.webp"
        	}
        }
    }
]
```

Not all browsers currently support WebP. If a glTF contains a WebP with no fallback texture and the browser does not support WebP, the client should either display an error or decode the WebP image at runtime.

## Known Implementations

CesiumJS uses it to significantly cut load times for massive models that contain gigabytes of texture data (see [implementation and sample model](https://github.com/AnalyticalGraphicsInc/cesium/pull/7486)). 

## Resources

Google's [WebP developer page](https://developers.google.com/speed/webp/) provides information about the format as well as [pre-compiled and source code versions](https://developers.google.com/speed/webp/download) of an encoder and a decoder.

The library [libwebpjs](http://libwebpjs.appspot.com) decodes WebP images in JavaScript for browsers that do not natively support it. [Sharp](http://sharp.pixelplumbing.com/en/stable/) is a NodeJS library for fast encode/decode of WebP (built on top of Google's implementation above).