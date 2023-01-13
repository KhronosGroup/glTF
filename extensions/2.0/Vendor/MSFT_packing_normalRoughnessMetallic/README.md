# MSFT_packing_normalRoughnessMetallic

## Contributors

* Saurabh Bhatia, Microsoft
* Gary Hsu, Microsoft
* Marc Appelsmeier, Microsoft
* Dag Frommhold, Microsoft 
* John Copic, Microsoft 

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds support for alternate texture packing and is meant to be used for Windows Mixed Reality Home and 3D Launchers for Windows Mixed Reality.

This extension defines an additional property for `normalRoughnessMetallicTexture`. This property specifies the index of a texture with the packing Normal (RG), Roughness (B), Metallic (A). This specialized packing is meant to provide an optimized alternative to the one specified in the core glTF 2.0 specification. 

The extension should only be used when creating glTF assets for engines that support this packing and is not meant to be a general purpose packing extension. Any client that does not support this extension can safely ignore these additional packed texture and rely on the default packing in glTF 2.0. This extension can also be used along with other extensions like [MSFT_texture_dds](../MSFT_texture_dds/README.md) to store the packed textures in DDS files. 

The example below shows how this extension can be used along with the MSFT_texture_dds extension in a glTF Binary (.glb) file:

```json
{
    "materials": [
        {
            "pbrMetallicRoughness": {
                "baseColorTexture": {
                    "index": 0
                },
                "roughnessMetallicTexture": {
                    "index": 1
                }
            },
            "normalTexture": {
                "index": 2
            },
            "extensions": {
                "MSFT_packing_normalRoughnessMetallic": {
                    "normalRoughnessMetallicTexture": {
                        "index": 3
                    }
                }
            }
        }
    ],
    "textures": [
        {
            "name": "baseColorTexture",
            "source": 0,
            "extensions": {
                "MSFT_texture_dds": {
                    "source": 3
                }
            }
        },
        {
            "name": "roughnessMetallicTexture",
            "source": 1
        },
        {
            "name": "normalTexture",
            "source": 2
        },
        {
            "name": "normalRoughnessMetallic",
            "extensions": {
                "MSFT_texture_dds": {
                    "source": 4
                }
            }
        },
    ],
    "images": [
        {
            "name": "baseColorTexture.png",
            "mimeType": "image/png",
            "bufferView": 0
        },
        {
            "name": "roughnessMetallicTexture.png",
            "mimeType": "image/png",
            "bufferView": 1
        },
        {
            "name": "normalTexture.png",
            "mimeType": "image/png",
            "bufferView": 2
        },
        {
            "name": "baseColorTexture.dds",
            "mimeType": "image/vnd-ms.dds",
            "bufferView": 3
        },
        {
            "name": "normalRoughnessMetallic.dds",
            "mimeType": "image/vnd-ms.dds",
            "bufferView": 4
        }
    ]
}
```

## glTF Schema Updates

* **JSON schema**: [glTF.MSFT_packing_normalRoughnessMetallic.schema.json](schema/glTF.MSFT_packing_normalRoughnessMetallic.schema.json)


## Known Implementations

This extension is used by Windows Mixed Reality Home and 3D Launchers for Windows Mixed Reality to improve performance by using the specially packed textures. [glTF-Toolkit](https://github.com/Microsoft/glTF-Toolkit) can be used to generate files that use this extension.

## License

Microsoft has made this Specification available under the Open Web Foundation Agreement Version 1.0, which is available at https://www.openwebfoundation.org/the-agreements/the-owf-1-0-agreements-granted-claims/owfa-1-0.
