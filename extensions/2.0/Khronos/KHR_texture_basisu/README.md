<!--
Copyright 2020 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR_texture_basisu

## Contributors

- Mark Callow [@MarkCallow](https://github.com/MarkCallow)
- Rich Geldreich, Binomial, [@richgel999](https://github.com/richgel999)
- Gary Hsu, Microsoft, [@bghgary](https://twitter.com/bghgary)
- Arseny Kapoulkine, Roblox, [@zeuxcg](https://twitter.com/zeuxcg)
- Alexey Knyazev [@lexaknyazev](https://github.com/lexaknyazev)
- Don McCurdy, Google, [@donrmccurdy](https://twitter.com/donrmccurdy)

Copyright 2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify textures using KTX v2 images with Basis Universal supercompression. An implementation of this extension can use such images as an alternative to the PNG or JPEG images available in glTF 2.0 for more efficient asset transmission and reducing GPU memory footprint. Furthermore, specifying mip map levels is possible.

When the extension is used, it's allowed to use value `image/ktx2` for the `mimeType` property of images that are referenced by the `source` property of `KHR_texture_basisu` texture extension object.

At runtime, engines are expected to transcode a universal texture format into some block-compressed texture format supported by the platform.

## glTF Schema Updates

The `KHR_texture_basisu` extension is added to the `textures` object and specifies a `source` property that points to the index of the `image` which defines a reference to the KTX v2 image with Basis Universal supercompression.

The following glTF will load `image.ktx2` in clients that support this extension, and otherwise fall back to `image.png`.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_basisu"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_basisu": {
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
            "uri": "image.ktx2"
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
        "KHR_texture_basisu"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_basisu": {
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

To use KTX v2 image with Basis Universal supercompression without a fallback, define `KHR_texture_basisu` in both `extensionsUsed` and `extensionsRequired`. The `texture` object will then have its `source` property omitted as shown below.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_basisu"
    ],
    "extensionsRequired": [
        "KHR_texture_basisu"
    ],
    "textures": [
        {
            "extensions": {
                "KHR_texture_basisu": {
                    "source": 0
                }
            }
        }
    ],
    "images": [
        {
            "uri": "image.ktx2"
        }
    ]
}
```

### JSON Schema

[texture.KHR_texture_basisu.schema.json](schema/texture.KHR_texture_basisu.schema.json)

## KTX v2 Images with Basis Universal Supercompression

To cover a broad range of use cases, this extension allows two Basis Universal formats: ETC1S with BasisLZ and UASTC with optional Zstandard compression.

For the purposes of this extension, the following texture types are defined:

- **RGB:** A texture that uses red, green, and blue channels. Alpha channel is either assumed to be always equal to `1.0` (`255`) or unused and not sampled at runtime. Such textures MAY be encoded with the sRGB transfer function.
  > **Note:** RGB textures from the core glTF 2.0 specification include:
  > - `baseColorTexture` (opaque case)
  > - `normalTexture`
  > - `emissiveTexture`
  > - `metallicRoughnessTexture` (when be combined with `occlusionTexture`, otherwise red channel is ignored)

- **RGBA:** A texture that uses all four channels. Such textures MAY be encoded with the sRGB transfer function (not applied to Alpha).
  > **Note:** RGBA textures from the core glTF 2.0 specification include:
  > - `baseColorTexture` (non-opaque case)

- **Red:** A texture that uses only red channel. All other channels are unused and their values are not sampled at runtime. Such textures MUST NOT be encoded with the sRGB transfer function.
  > **Note:** Red textures from the core glTF 2.0 specification include:
  > - `occlusionTexture` (standalone)

- **Red-Green:** A texture that uses only red and green channels. Blue and Alpha channels are unused and their values are not sampled at runtime. Such textures MUST NOT be encoded with sRGB transfer function.
  > **Note:** The core glTF 2.0 specification has no examples of red-green textures.

### KTX header fields for ETC1S with BasisLZ payloads
  - `supercompressionScheme` MUST be `1` (BasisLZ).
  - DFD `colorModel` MUST be `KHR_DF_MODEL_ETC1S`.
  - RGB textures MUST use a single `KHR_DF_CHANNEL_ETC1S_RGB` DFD channel.
  - RGBA textures MUST use two DFD channels: `KHR_DF_CHANNEL_ETC1S_RGB` and `KHR_DF_CHANNEL_ETC1S_AAA`.
  - Red textures MUST use a single `KHR_DF_CHANNEL_ETC1S_RRR` DFD channel.
  - Red-Green textures MUST use two DFD channels: `KHR_DF_CHANNEL_ETC1S_RRR` and `KHR_DF_CHANNEL_ETC1S_GGG`.

### KTX header fields for UASTC payloads
  - `supercompressionScheme` MUST be `0` (None) or `2` (Zstandard).
  - DFD `colorModel` MUST be `KHR_DF_MODEL_UASTC`.
  - RGB textures MUST use a single `KHR_DF_CHANNEL_UASTC_RGB` DFD channel.
  - RGBA textures MUST use a single `KHR_DF_CHANNEL_UASTC_RGBA` DFD channel.
  - Red textures MUST use a single `KHR_DF_CHANNEL_UASTC_RRR` DFD channel.
  - Red-Green textures MUST use a single `KHR_DF_CHANNEL_UASTC_RG` DFD channel.

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
- When a texture refers to a sampler with mipmap minification or when the sampler is undefined, the KTX image SHOULD contain a full mip pyramid.
  > **Implementation Note**: Depending on the platform features available, engines may need to decompress the texture and generate missing mipmaps at runtime when a full mip pyramid is not available.
- `pixelWidth` and `pixelHeight` MUST be multiples of 4.
  > **Implementation Note**: Exporters may opt to make the dimensions powers of 2. Combined with all mip levels present this would ensure maximum compatibility.

### Using KTX v2 Images with Basis Universal Supercompression for Material Textures

When a texture referencing a KTX v2 image with Basis Universal supercompression is used for glTF 2.0 material maps (both color and non-color), the KTX v2 image MUST be of **2D** type as defined in the KTX v2 Specification, Section 4.1.

`KHR_DF_FLAG_ALPHA_PREMULTIPLIED` flag MUST NOT be set unless the material's specification requires premultiplied alpha.

Exporters SHOULD choose the most appropriate universal texture format based on the texture's usage and content.
> **Implementation Note**: As a general rule, textures with color data should use ETC1S while textures with non-color data (such as roughness-metallic or normal maps) should use UASTC.

When the high-quality GPU formats (i.e., ASTC or BC7) are not available at runtime, engines SHOULD transcode UASTC textures with non-color data to uncompressed RGBA to avoid artifacts.

## Known Implementations

Authoring:

- [gltfpack](https://github.com/zeux/meshoptimizer)
- [glTF-Transform CLI](https://gltf-transform.donmccurdy.com/cli.html)
- [Gestaltor](https://gestaltor.io/)
- [RapidCompact](https://rapidcompact.com/)

Viewing:

- [Babylon](https://www.babylonjs.com/)
- [PlayCanvas](https://playcanvas.com/)
- [three.js](https://threejs.org/)

## Resources

[KTX File Format Specification, version 2](https://github.khronos.org/KTX-Specification/)

[KTX Reference Software](https://github.com/KhronosGroup/KTX-Software/)

## Appendix: Full Khronos Copyright Statement

Copyright 2020 The Khronos Group Inc.

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
