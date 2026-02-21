# KHR_texture_float_bc6h

## Contributors

* Rickard Sahlin, mailto:rickard.sahlin@ikea.com  
* Alexey Knyazev [@lexaknyazev](https://github.com/lexaknyazev)  
* Don McCurdy, Google, [@donrmccurdy](https://twitter.com/donrmccurdy)  
* Norbert Nopper, mailto:nopper@ux3d.io  

Copyright (C) 2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.  

## Overview

This extension adds the ability to specify textures using KTX v2 images with compressed float format as defined by VK_FORMAT_BC6H_UFLOAT_BLOCK.  
It allows specification of a source texture with increased dynamic range to be used.  
This source texture shall be used in shader (BRDF) calculations in a way that retains the increased range.  
However, this extension does not define the way pixel data is written to framebuffer or how swapchain is presented using the increased range.  

When the extension is used, it's allowed to use value `image/ktx2` for the `mimeType` property of images that are referenced by the `source` property of `KHR_texture_float_bc6h` texture extension object.

The [conformance](#conformance) section specifies what an implementation must do when encountering this extension, and how the extension interacts with the attributes defined in the base specification.    

## glTF Schema Updates

The `KHR_texture_float_bc6h` extension is added to the `textures` object and specifies a `source` property that points to the index of the `image` which defines a reference to the KTX v2 image in VK_FORMAT_BC6H_UFLOAT_BLOCK.

### Using this extension

The following glTF will load `image.ktx2` in clients that support this extension, otherwise fallback to `image.png`  


```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_float_bc6h"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_float_bc6h": {
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
        "KHR_texture_float_bc6h"
    ],
    "textures": [
        {
            "source": 0,
            "extensions": {
                "KHR_texture_float_bc6h": {
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

### Using this extension without a fallback

To use KTX v2 image with half float without a fallback, `KHR_texture_float_bc6h` in both extensionsUsed and extensionsRequired.  
The texture object will then have its source property omitted as shown below.


```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_texture_float_bc6h"
    ],
    "extensionsRequired": [
        "KHR_texture_float_bc6h"
    ],
    "textures": [
        {
            "extensions": {
                "KHR_texture_float_bc6h": {
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

[texture.KHR_texture_float_bc6h.schema.json](schema/texture.KHR_texture_float_bc6h.schema.json)

## KTX v2 Images with VK_FORMAT_BC6H_UFLOAT_BLOCK

To cover usecases where a texture source shall have increased dynamic range.  

- Swizzling metadata (`KTXswizzle`) MUST be `rgba` or omitted.
- Orientation metadata (`KTXorientation`) MUST be `rd` or omitted.
- Color space information in the DFD MUST match the expected usage, namely:
  - For textures with **color data** (e.g., base color maps),
    - `colorPrimaries` MUST be `KHR_DF_PRIMARIES_BT709`;
    - `transferFunction` MUST be `KHR_DF_TRANSFER_LINEAR`.
  - For textures with **non-color data** (e.g., normal maps),
    - `colorPrimaries` MUST be `KHR_DF_PRIMARIES_UNSPECIFIED`;
    - `transferFunction` MUST be `KHR_DF_TRANSFER_LINEAR`.


### Using KTX v2 Images with VK_FORMAT_BC6H_UFLOAT_BLOCK for Material Textures

When a texture referencing a KTX v2 image with VK_FORMAT_BC6H_UFLOAT_BLOCK is used for glTF 2.0 material maps (both color and non-color), the KTX v2 image MUST be of **2D** type as defined in the KTX v2 Specification, Section 4.1.

`KHR_DF_FLAG_ALPHA_PREMULTIPLIED` flag MUST NOT be set unless the material's specification requires premultiplied alpha.


# Conformance

Below is the recommended process when a loader encounters a glTF asset with the compressed half float extension set:
* If `KHR_texture_float_bc6h` is in extensionsRequired and the loader does not support the extension, then the loader must fail loading the asset.  
* If the loader does not support the compressed half float extension and `KHR_texture_float_bc6h` is not in extensionsRequired, then load the glTF asset using the image in the source of the texture object.  


# Implementation note

_This section is non normative_

Implementations are expected to check hardware support for VK_FORMAT_BC6H_UFLOAT_BLOCK. 
If not present the texture may be transcoded to a suitable supported format for instance using the [BC6H-Decoder-WASM](https://github.com/KhronosGroup/BC6H-Decoder-WASM) library.  

## Known Implementations

Authoring:


Viewing:


## Resources

[KTX File Format Specification, version 2](https://github.khronos.org/KTX-Specification/)

[KTX Reference Software](https://github.com/KhronosGroup/KTX-Software/)

## Appendix: Full Khronos Copyright Statement

Copyright 2021 The Khronos Group Inc.

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
