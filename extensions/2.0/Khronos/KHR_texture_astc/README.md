<!--
Copyright 2024 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR_texture_astc

## Contributors

- Wasim Abbas [@wasimabbas-arm](https://github.com/wasimabbas-arm)
- Jan-Harald Fredriksen [@janharaldfredriksen-arm](https://github.com/janharaldfredriksen-arm)
- Peter Harris [@solidpixel](https://github.com/solidpixel)
- Mark Callow [@MarkCallow](https://github.com/MarkCallow)

Copyright 2023 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify textures using KTX v2 images with (ASTC compression)[https://github.com/ARM-software/astc-encoder/blob/main/Docs/FormatOverview.md]. An implementation of this extension can use such images as an alternative to the PNG or JPEG images available in glTF 2.0 for more efficient asset transmission, reducing GPU memory footprint, higher texture throughput and faster execution.

When this extension is used, it's allowed to use value `image/astc` for the `mimeType` property of images that are referenced by the `source` property of `KHR_texture_astc` texture extension object.

At runtime, engines can directly use the ktx images unlike EXT_texture_basisu which requires doing transcoding to other compressed formats. This also means you get support for all ASTC block formats and quality presets.

## glTF Schema Updates

The `KHR_texture_astc` extension is added to the `textures` object and specifies a `source` property that points to the index of the `image` which defines a reference to the KTX v2 image with ASTC compression.

The following glTF will load `astc_image.ktx2` in clients that support this extension, and fall back to `image.png` otherwise.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_astc"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_astc": {
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
            "uri": "astc_image.ktx2"
        }
    ]
}
```

When used in the glTF Binary (GLB) format the `image` that points to the KTX v2 resource uses the `mimeType` value of `image/ktx2`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_astc"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_astc": {
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
            "mimeType": "image/ktx2",
            "bufferView": 2
        }
    ]
}
```

### Using Without a Fallback

To use KTX v2 image with ASTC compression without a fallback, define `KHR_texture_astc` in both `extensionsUsed` and `extensionsRequired`. The `texture` object will then have its `source` property omitted as shown below.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_astc"
    ],
    "extensionsRequired": [
        "KHR_texture_astc"
    ],
    "textures": [
        {
            "extensions": {
                "KHR_texture_astc": {
                    "source": 0
                }
            }
        }
    ],
    "images": [
        {
            "uri": "astc_image.ktx2"
        }
    ]
}
```

### JSON Schema

[texture.KHR_texture_astc.schema.json](schema/texture.KHR_texture_astc.schema.json)

## KTX v2 Images with Astc compression 

To cover a broad range of use cases, this extension allows different ASTC block sizes as well as LDR, HDR and 3D images. These can be determined from the vkFormat of the KTX image.

For the purposes of this extension, the following texture types are defined:

- **RGB:** A texture that uses red, green, and blue channels. Alpha channel is either assumed to be always equal to `1.0` (`255`) or unused and not sampled at runtime. Such textures MAY be encoded with the sRGB transfer function.
  > **Note:** RGB textures from the core glTF 2.0 specification include:
  > - `baseColorTexture` (opaque case)
  > - `normalTexture`
  > - `emissiveTexture`
  > - `metallicRoughnessTexture` (when combined with `occlusionTexture`, otherwise red channel is ignored)

- **RGBA:** A texture that uses all four channels. Such textures MAY be encoded with the sRGB transfer function (not applied to Alpha).
  > **Note:** RGBA textures from the core glTF 2.0 specification include:
  > - `baseColorTexture` (non-opaque case)

- **Red:** A texture that uses only red channel. All other channels are unused and their values are not sampled at runtime. Such textures MUST NOT be encoded with the sRGB transfer function.
  > **Note:** Red textures from the core glTF 2.0 specification include:
  > - `occlusionTexture` (standalone)

- **Luminance-Alpha:** A texture that uses only Luminance and Alpha channels. To sample these textures use `.ga` swizzle in your shaders. Such textures MUST NOT be encoded with sRGB transfer function.
  > **Note:** The core glTF 2.0 specification has no examples of luminance-alpha textures.

### KTX header fields for ASTC payloads
  - `supercompressionScheme` MUST be `0` (None).
  - ASTC HDR vs LDR as well as block sizes can be determined from the vkFormat of the image
  - DFD `colorModel` MUST be `KHR_DF_MODEL_ASTC`.
  - DFD `channelId` MUST be `KHR_DF_CHANNEL_ASTC_DATA`.

### Additional requirements

Regardless of the format used, these additional restrictions apply for compatibility reasons:

- Swizzling metadata (`KTXswizzle`) MUST be `rgba` or omitted.
- Orientation metadata (`KTXorientation`) MUST be `rd` or omitted.
- Color space information in the DFD MUST match the expected usage, namely:
  - For textures with **color data** (e.g., base color maps),
    - `colorPrimaries` MUST be `KHR_DF_PRIMARIES_BT709`;
    - `transferFunction` MUST be `KHR_DF_TRANSFER_SRGB`.
  - For textures with **non-color data** (e.g., normal maps),
    - `colorPrimaries` MUST be `KHR_DF_PRIMARIES_UNSPECIFIED`;
    - `transferFunction` MUST be `KHR_DF_TRANSFER_LINEAR`.
- `pixelWidth` and `pixelHeight` MUST be multiples of 4.
- When a texture refers to a sampler with mipmap minification, the KTX image MUST contain a full mip pyramid.
- When a texture referencing a KTX v2 image with ASTC compression is used for glTF 2.0 material maps (both color and non-color), the KTX v2 image MUST be of **2D** type as defined in the KTX v2 Specification, Section 4.1.
- `KHR_DF_FLAG_ALPHA_PREMULTIPLIED` flag MUST NOT be set unless the material's specification requires premultiplied alpha.

## Known Implementations

Authoring:

- [KTX Reference Software](https://github.com/KhronosGroup/KTX-Software/)

Viewing:



## Resources

[KTX File Format Specification, version 2](https://github.khronos.org/KTX-Specification/)

[KTX Reference Software](https://github.com/KhronosGroup/KTX-Software/)

[ASTC encoder](https://github.com/ARM-software/astc-encoder)

## Appendix: Full Khronos Copyright Statement

Copyright 2023 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/adopters.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

Khronos® and Vulkan® are registered trademarks, and ANARI™, WebGL™, glTF™, NNEF™, OpenVX™,
SPIR™, SPIR-V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc.
OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in
China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc.
and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks
of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of
ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely
for identification and belong to their respective owners.
