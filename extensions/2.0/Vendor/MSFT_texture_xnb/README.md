# MSFT_texture_xnb

## Contributors

* Dean Ellis, MonoGame

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify textures using the XNA Binary format (XNB). An implementation of this extension can use the textures provided in the XNB files as an alternative to the PNG or JPG textures available in glTF 2.0.

The extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which in turn points to the XNB texture file. A client that does not understand this extension can ignore the XNB file and continue to rely on the PNG or JPG textures specified.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "MSFT_texture_xnb": {
                "source": 1
            }
        }
    }
],
"images": [
    {
        "uri": "defaultTexture.png"
    },
    {
        "uri": "XNBTexture.xnb"
    }
]
```

When used in the glTF Binary (.glb) format the `images` node that points to the XNB file uses the `mimeType` value of *image/vnd-ms.xnb*.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "MSFT_texture_xnb": {
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
        "mimeType": "image/vnd-ms.xnb",
        "bufferView": 2
    }
]
```

## glTF Schema Updates

* **JSON schema**: [glTF.MSFT_texture_xnb.schema.json](schema/glTF.MSFT_texture_xnb.schema.json)

## Known Implementations

This extension is used by users of XNA like frameworks such as MonoGame,FNA and KNI to load glTF files at runtime by including XNB textures.

## License

MonoGame has made this Specification available under the Open Web Foundation Agreement Version 1.0, which is available at https://www.openwebfoundation.org/the-agreements/the-owf-1-0-agreements-granted-claims/owfa-1-0.
