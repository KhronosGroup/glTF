# MSFT_texture_dds 

## Contributors

* Saurabh Bhatia, Microsoft
* Gary Hsu, Microsoft
* Marc Appelsmeier, Microsoft
* Dag Frommhold, Microsoft 
* John Copic, Microsoft 

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify textures using the DirectDraw Surface file format (DDS). An implementation of this extension can use the textures provided in the DDS files as an alternative to the PNG or JPG textures available in glTF 2.0.

The extension is added to the `textures` node and specifies a `source` property that points to the index of the `images` node which in turn points to the DDS texture file. A client that does not understand this extension can ignore the DDS file and continue to rely on the PNG or JPG textures specified.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "MSFT_texture_dds": {
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
        "uri": "DDSTexture.dds"
    }
]
```
When used in the glTF Binary (.glb) format the `images` node that points to the DDS file uses the `mimeType` value of *image/vnd-ms.dds*.

```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "MSFT_texture_dds": {
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
        "mimeType": "image/vnd-ms.dds",
        "bufferView": 2
    }
]
```

## glTF Schema Updates

* **JSON schema**: [glTF.MSFT_texture_dds.schema.json](schema/glTF.MSFT_texture_dds.schema.json)

## Known Implementations

This extension is used by Windows Mixed Reality Home and 3D Launchers for Windows Mixed Reality to improve performance by including DDS textures. [glTF-Toolkit](https://github.com/Microsoft/glTF-Toolkit) can be used to generate files that use this extension.

## License

Microsoft has made this Specification available under the Open Web Foundation Agreement Version 1.0, which is available at https://www.openwebfoundation.org/the-agreements/the-owf-1-0-agreements-granted-claims/owfa-1-0.
