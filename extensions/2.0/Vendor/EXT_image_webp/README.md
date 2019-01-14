# EXT_image_webp

## Contributors

* Omar Shehata, Cesium, [@OmarShehata](https://github.com/OmarShehata)
* TBA, Google

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use WebP as a valid image format by extending the Image property. The [best practices](#best-practices) section defines what an implementation must do when encountering this extension and how this extension interacts with the images defined in the base specification.

WebP is an image format developed and maintained by Google that provides superior lossless and lossy compression rates for images on the web. It is typically 26% smaller in size compared to PNGs and 25-34% smaller than comparable JPEG images - [source](https://developers.google.com/speed/webp/).  

## glTF Schema Updates

The `EXT_image_webp` extension may be defined on `image` structures. It must contain either a `uri` or a `bufferView` property as described in the following table.

| Name       | Type       | Description
|------------|------------|---------------------------------
| `uri`      | `string`   | The uri of the image.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.  The image format must be webp.
| `bufferView`  | `integer`   | The index of the bufferView that contains the image. Use this instead of the image's uri property. Mime type is assumed to be `image/webp`.

Additional properties are allowed.

### JSON Schema

[glTF.EXT_image_webp.schema.json](schema/glTF.EXT_image_webp.schema.json)

### Example JSON

To load a WebP image via a URI, the `EXT_image_webp` is defined on any glTF image property as shown below.

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

If a client does not support this extension, it will load `image.png` as expected in the base specification.

## Best Practices

It is recommended to provide a fallback PNG or JPEG image for clients that do not support this extension.

To use WebP images without a fallback, define `EXT_image_webp` in `extensionsRequired` instead of `extensionsUsed`. The `uri` of the base image object can then be set to an empty string.

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

A client that implements this extension must load the WebP image provided even if a fallback is defined. Note that either a `uri` or a `bufferView` must be defined on the base image object even if they are ignored by the client implementing this extension.

Not all browsers currently support WebP. It is left up to client whether to display an error or decode the WebP image at runtime.

## Known Implementations

TODO: Implement it in CesiumJS and reference it here.

## Resources

Google's [WebP developer page](https://developers.google.com/speed/webp/) provides information about the format as well as [pre-compiled and source code versions](https://developers.google.com/speed/webp/download) of an encoder and a decoder.

The library [libwebpjs](http://libwebpjs.appspot.com) decodes WebP images in JavaScript for browsers that do not natively support it. [Sharp](http://sharp.pixelplumbing.com/en/stable/) is a NodeJS library for fast encode/decode of WebP (built on top of Google's implementation above).