# EXT_texture_avif

## Contributors

- Leon Radley, [@leon](https://github.com/leon)
- Don McCurdy, [@donmccurdy](https://github.com/donmccurdy)
- Alexey Knyazev, [@lexaknyazev](https://github.com/lexaknyazev)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use AVIF as a valid image format. A client that does not implement this extension can ignore the provided AVIF image and continue to rely on the PNG and JPG textures available in the base specification. Defining a fallback texture is optional. The [best practices](#best-practices) section describes the intended use case of this extension and the expected behavior when using it without a fallback texture.

AVIF is an image format developed by the The Alliance for Open Media.

## glTF Schema Updates

The `EXT_texture_avif` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which in turn points to the AVIF image.

The following glTF will load `image.avif` in clients that support this extension, and otherwise fall back to `image.png`.

```
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_avif": {
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
        "uri": "image.avif"
    }
]
```

When used in the glTF Binary (.glb) format the `images` node that points to the AVIF image uses the `mimeType` value of `image/avif`.

```
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_avif": {
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
        "mimeType": "image/avif",
        "bufferView": 2
    }
]
```

### JSON Schema

[glTF.EXT_texture_avif.schema.json](schema/glTF.EXT_texture_avif.schema.json)

## Best Practices

Since AVIF is not as widely supported as JPEG and PNG, until [browser support](https://caniuse.com/avif) is good you will have to create a separate version of your asset with this extension and then do feature detection to load the avif version.
AVIF has support for image sequences much like GIF, but this extension is only targeting using AVIF as a replacement for jpeg and png.
When a fallback image is defined, this extension should not be present in `extensionsRequired`. This will allow all clients to render the glTF correctly, and those that support this extension can request the optimized AVIF version of the textures.

### Using Without a Fallback

To use AVIF images without a fallback, define `EXT_texture_avif` in both `extensionsUsed` and `extensionsRequired`. The `texture` node will then have only an `extensions` property as shown below.

```
"textures": [
    {
        "extensions": {
            "EXT_texture_avif": {
                "source": 1
            }
        }
    }
]
```

If a glTF contains a AVIF with no fallback texture and the browser does not support AVIF, the client should either display an error or decode the AVIF image at runtime (see [resource](#resources) for decoding libraries).

## Known Implementations

## Resources

- [AVIF specification](https://aomediacodec.github.io/av1-avif/)
- [AVIF reference implementation](https://github.com/AOMediaCodec/libavif/).
- [Can I Use Browser Support](https://caniuse.com/avif)
- [Sharp](https://sharp.pixelplumbing.com/) is a NodeJS library for fast encode/decode of AVIF using native modules (built on top of libvips which has support for AVIF.
