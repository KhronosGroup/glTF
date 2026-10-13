# EXT_texture_svg

## Contributors

- K. S. Ernest (iFire) Lee, Godot Engine contributor, V-Sekai contributor.
- Aaron Franke, The Mirror Megaverse Inc.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF models to use SVG as a valid image format. A client that does not implement this extension can ignore the provided SVG image and continue to rely on the PNG and JPG textures available in the base specification. Defining a fallback texture is optional. The [best practices](#best-practices) section describes the intended use case of this extension and the expected behavior when using it without a fallback texture.

SVG is a vector image format that can be scaled to any size without loss of quality. SVG is supported by all major game engines and web browsers.

SVG images are ideal for assets with simple shapes or text. For instance, a glTF asset like a billboard displaying a logo or a sign with substantial text would benefit from SVGs. This allows for clear visibility at any distance without the need for large raster images that increase file size.

For non-vectorizable artwork, PNG and JPEG raster images can be embedded into SVG to allow for mixing vector shapes and text with complex detailed raster images.

## glTF Schema Updates

The `EXT_texture_svg` extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which in turn points to the SVG image.

The following glTF will load `image.svg` in clients that support this extension, and otherwise fall back to `image.png`.

```
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_svg": {
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
        "uri": "image.svg"
    }
]
```

When used in the glTF Binary (.glb) format the `images` node that points to the SVG image uses the `mimeType` value of `image/svg+xml`.

```
"textures": [
    {
        "source": 0,
        "extensions": {
            "EXT_texture_svg": {
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
        "mimeType": "image/svg+xml",
        "bufferView": 2
    }
]
```

### JSON Schema

[texture.EXT_texture_svg.schema.json](schema/texture.EXT_texture_svg.schema.json)

## Best Practices

SVG images are a great choice for images with simple shapes and images with text. For example, a glTF asset with a billboard that displays a simple logo with text would strongly benefit from SVG images, or a glTF asset with a sign with a lot of text would want to use SVG to allow for sharp text at any distance without having to use a large raster image that would greatly increase the file size.

Since SVG is not a part of the base glTF spec, it is recommended to provide a raster image fallback for clients that do not support this extension. The fallback image should be high enough of a resolution that the text is still legible.

The SVG format has a variety of features, not all of which are supported by all SVG implementations. If a client does not support a feature used in the SVG image, it should fall back to the raster image.

Support for animated SVGs is not typically found in target glTF implementations.

When a fallback image is defined, this extension should not be present in `extensionsRequired`. This will allow all clients to render the glTF correctly, and those that support this extension can read the SVG version of the textures.

### Using Without a Fallback

To use SVG images without a fallback, define `EXT_texture_svg` in both `extensionsUsed` and `extensionsRequired`. The `texture` node will then have only an `extensions` property as shown below.

```
"textures": [
    {
        "extensions": {
            "EXT_texture_svg": {
                "source": 1
            }
        }
    }
]
```

If a glTF contains a SVG with no fallback texture, `EXT_texture_svg` should be added to the `extensionsRequired` array, such that clients that do not support SVG will not attempt to load the file.

## Known Implementations

None

## Resources

- https://en.wikipedia.org/wiki/SVG
- https://developer.mozilla.org/en-US/docs/Web/SVG
- https://www.w3.org/TR/SVG/
