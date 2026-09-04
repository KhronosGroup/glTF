# EXT_texture_qoi

## Contributors

* Sean, [@spnda](https://github.com/spnda)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use QOI as a valid image format. A client that does not implement this extension can ignore the provided QOI image and continue to rely on the PNG and JPG textures available in the base specification. Defining a fallback texture is optional.

QOI is an image format developed and maintained by Dominic Szablewski that provides lossless image compression with a similar size compared to PNGs exported by libpng and stbi, while being much faster to decode and encode - [source](https://qoiformat.org/).

## glTF Schema Updates

The `EXT_texture_qoi` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which in turn points to the QOI image.

The following glTF will load `image.qoi` in clients that support this extension, and otherwise fall back to `image.png`.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_qoi": {
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
        "uri": "image.qoi"
    }
]
```

When used in the glTF Binary (.glb) format the `images` node that points to the QOI image uses the `mimeType` value of `image/qoi`.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_qoi": {
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
        "mimeType": "image/qoi",
        "bufferView": 2
    }
]
```

### Using without a fallback

To use QOI images without a fallback, define `EXT_texture_qoi` in both `extensionsUsed` and `extensionsRequired`. The `texture` node will then only have an `extensions` property as shown below.

```json
"textures": [
    {
        "extensions": {
            "EXT_texture_qoi": {
                "source": 0
            }
        }
    }
],
```

If a glTF contains a QOI texture with no fallback texture, the client should either display an error or use a default texture.

## Known implementations

* [fastgltf](https://github.com/spnda/fastgltf), a glTF library aimed at greatly improving speeds in every way.

## Resources

The [QOI website](https://qoiformat.org/) and the [QOI format specification](https://qoiformat.org/qoi-specification.pdf) contains everything you need to know about the format, including implementations, benchmarks, and the file structure.
