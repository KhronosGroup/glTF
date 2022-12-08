# MSFT_packing_occlusionRoughnessMetallic

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

This extension defines three additional properties:
- `occlusionRoughnessMetallicTexture`: Specifies the index of a texture with the packing Occlusion (R), Roughness (G), Metallic (B)
- `roughnessMetallicOcclusionTexture`: Specifies the index of a texture with the packing Roughness (R), Metallic (G), Occlusion (B)
- `normalTexture`: Specifies the index of a texture which contains two channel (RG) normal map. 

The extension should only be used when creating glTF assets for engines that support this packing and is not meant to be a general purpose packing extension. Any client that does not support this extension can safely ignore these additional packed textures and rely on the default packing in glTF 2.0. This extension can also be used along with other extensions like [MSFT_texture_dds](../MSFT_texture_dds/README.md) to store the packed textures in DDS files. 

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
                },
            },
            "normalTexture": {
                "index": 2
            },
            "occlusionTexture": {
                "index": 3
            },
            "extensions": {
                "MSFT_packing_occlusionRoughnessMetallic": {
                    "occlusionRoughnessMetallicTexture": {
                        "index": 4
                    },
                    "normalTexture": {
                        "index": 5
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
                    "source": 6
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
            "name": "occlusionTexture",
            "source": 3
        },
        {
            "name": "occlusionRoughnessMetallic",
            "extensions": {
                "MSFT_texture_dds": {
                    "source": 4
                }
            }
        },
        {
            "name": "normalTexture_RG",
            "extensions": {
                "MSFT_texture_dds": {
                    "source": 5
                }
            }
        }
    ],
    "images": [
        {
            "mimeType": "image/png",
            "bufferView": 0
        },
        {
            "mimeType": "image/png",
            "bufferView": 1
        },
        {
            "mimeType": "image/png",
            "bufferView": 2
        },
        {
            "mimeType": "image/png",
            "bufferView": 3
        },
        {
            "mimeType": "image/vnd-ms.dds",
            "bufferView": 4
        },
        {
            "mimeType": "image/vnd-ms.dds",
            "bufferView": 5
        },
        {
            "mimeType": "image/vnd-ms.dds",
            "bufferView": 6
        }
    ]
}
```

## glTF Schema Updates

* **JSON schema**: [glTF.MSFT_packing_occlusionRoughnessMetallic.schema.json](schema/glTF.MSFT_packing_occlusionRoughnessMetallic.schema.json)


## Known Implementations

This extension is used by Windows Mixed Reality Home and 3D Launchers for Windows Mixed Reality to improve performance by using the specially packed textures. [glTF-Toolkit](https://github.com/Microsoft/glTF-Toolkit) can be used to generate files that use this extension.

## License

Microsoft has made this Specification available under the Open Web Foundation Agreement Version 1.0, which is available at https://www.openwebfoundation.org/the-agreements/the-owf-1-0-agreements-granted-claims/owfa-1-0.
