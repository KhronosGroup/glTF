# EXT_image_webp

## Contributors

* Omar Shehata, Cesium, [@OmarShehata](https://github.com/OmarShehata)
* Sean Lilley, Cesium, [@lilleyse](https://github.com/lilleyse)
* TODO: invite Google authors?

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

WebP is an image format developed and maintained by Google that provides superior lossless and lossy compression rates for images on the web. It is typically 26% smaller in size compared to PNGs and 25-34% smaller than comparable JPEG images - [source](https://developers.google.com/speed/webp/).  

This extension allows glTF models to use WebP as a valid image format.

## glTF Schema Updates

The `EXT_image_webp` extension may be defined on `image` structures. It must contain the following property:

| Name       | Type       | Description
|------------|------------|---------------------------------
| `uri`      | `string`   | The uri of the image.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.  The image format must be webp.


### JSON Schema

[glTF.EXT_image_webp.schema.json](schema/glTF.EXT_image_webp.schema.json)

### Example JSON

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

## Known Implementations

TODO: Implement it in Cesium and reference it here.

## Resources

* https://developers.google.com/speed/webp/

Decoders

* http://libwebpjs.appspot.com/
* https://medium.com/@kennethrohde/on-the-fly-webp-decoding-using-wasm-and-a-service-worker-33e519d8c21e

Decode in desktop / native apps

* Reference implementation: https://developers.google.com/speed/webp/download

Encode

* Reference implementation: https://developers.google.com/speed/webp/download
* https://sharp.dimens.io/en/stable/api-output/

#### Questions:

* Does this need to work with bufferViews too? Why would you define the image in a bufferView when you can define it as a dataUri? I guess if you want to reference it multiple times? No that doesn't make sense either...