# NV_texture_swizzle

## Contributors

* Alexey Panteleev, NVIDIA, alpanteleev@nvidia.com

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides the ability to use image compression and storage formats that have more than 4 channels as textures in glTF assets. This is achieved by specifying a subset or slice of channels from such image and using those channels as a glTF texture. The extension does not specify which image formats may be used as sources for channel slicing, leaving that up to implementations.

The motivation for this extension is to support new neural texture compression formats. Such formats are designed to store multiple or all of the PBR texture channels for a single material in one container: this way, higher compression ratios can be achieved, and decompressing all channels at once is faster than doing it per-texture. The neural texture set can be used directly in the renderer's shaders or decompressed and transcoded to any other format such as BCn on load. Although the extension is designed primarily for neural texture compression, it can also be useful for other containers that support storing multiple channels, such as OpenEXR.

## Specification

The extension is added to the `textures` node and specifies a single property `options`. The `options` property contains an array of dictionaries following the same format. Each option dictionary contains a `source` property that points to the index of the `images` node which in turn poins to the source image file. Additionally, it specifies a `channels` property that contains an ordered array of up to 4 integers, indicating which channels from the image are to be used for this texture's red, green, blue and alpha channels, in that order. 0 means the first channel, 1 the second, and so on. A special value of -1 is reserved to mean that the channel may contain arbitrary data and is unused in this texture: this can be useful for e.g. `metallicRoughnessTexture` that doesn't use the red channel.

If multiple entries are specified in the `options` array, they should be considered equivalent, and the implementation is free to choose any of the options. This is intended to provide glTF assets referencing multichannel textures in multiple formats and/or containers, so that implementations could choose one that they support.

```json
"textures": [
    {
        "sampler": 0,
        "extensions": {
            "NV_texture_swizzle": {
                "options": [
                    {
                        "source": 0,
                        // Extract channels 0-2 as an RGB texture
                        "channels": [ 0, 1, 2 ]
                    }
                ]
            }
        }
    },
    {
        "sampler": 0,
        "extensions": {
            "NV_texture_swizzle": {
                "options": [
                    {
                        "source": 0,
                        // Store arbitrary data in the Red channel
                        // Swap channels 3-4 into the Green and Blue channels
                        "channels": [ -1, 4, 3 ]
                    }
                ]
            }
        }
    }
],
"images": [
    {
        "uri": "MaterialTextureSet.ntc"
    }
]
```

Optionally, an `arrayLayer` attribute may be added to the extension. In case the image format supports storing texture arrays, this attribute would specify the index of a single array layer that is used for the texture, as a non-negative integer. If the `arrayLayer` attribute is not specified, it is assumed to be zero.

Using `NV_texture_swizzle` on a texture does not preclude using other sources on the same texture, including regular images or e.g. `MSFT_texture_dds`. When multiple sources are specified, implementations may choose the image format that they support or prefer.

## glTF Schema Updates

* **JSON schema**: [glTF.NV_texture_swizzle.schema.json](schema/glTF.NV_texture_swizzle.schema.json)

## Known Implementations

* The NVIDIA [RTXNTC SDK](https://github.com/NVIDIA-RTX/RTXNTC) implements this extension in the NTC Renderer sample and in the glTF asset preparation script that converts materials to the NTC format.
