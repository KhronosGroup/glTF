# KHR\_materials\_ior

## Contributors

* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
* Ben Houston, ThreeKit [@bhouston](https://github.com/bhouston)
* Richard Sahlin, IKEA [@rsahlin](https://github.com/rsahlin)
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Ed Mackey, AGI [@emackey](https://twitter.com/emackey)

Copyright (C) 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The dielectric BRDF of the metallic-roughness material in glTF uses a fixed value of 1.5 for the index of refraction. This is a good fit for many plastics and glass, but not for other materials like water or asphalt, sapphire or diamond. This extensions allows users to set the index of refraction to a certain value.

## Extending Materials

The index of refraction of a material is configured by adding the `KHR_materials_ior` extension to any glTF material. 

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_ior": {
                    "ior": 1.4,
                }
            }
        }
    ]
}
```

| |Type|Description|Required|
|-|----|-----------|--------|
| **ior** | `number` | The index of refraction. | No, default: `1.5`|

Typical values for the index of refraction range from 1 to 2. In rare cases values greater than 2 are possible. The following table shows some materials and their index of refraction:

| Material     | Index of Refraction |
|--------------|---------------------|
| Air          | 1.0                 |
| Water        | 1.33                |
| Eyes         | 1.38                |
| Window Glass | 1.52                |
| Sapphire     | 1.76                |
| Diamond      | 2.42                |

The reflectance at normal incidence (F0) of dielectric materials is computed from the IOR as follows:

```
f0 = ((ior - outside_ior) / (ior + outside_ior))^2
```

The `outside_ior` determines the index of refraction of the medium on the outside. If the renderer does not track the IOR when traversing nested dielectrics, it can assume `outside_ior = 1`.

For `ior = 1.5` and `outside_ior = 1`, the result is `f0 = 0.04`. This is the fixed value used in the glTF 2.0 metallic-roughness material. The extension makes this value configurable.


## Implementation

*This section is non-normative*

The extension changes the computation of the Fresnel term defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) to the following:

```
dielectricSpecular = ((ior - 1)/(ior + 1))^2
```

## Interaction with other extensions

If `KHR_materials_specular` is used in combination with `KHR_materials_ior`, f0 computed from IOR is multiplied by f0 computed from the specular parameters. See [`KHR_materials_specular`](../KHR_materials_specular/README.md) for more information.

If `KHR_materials_transmission` is used in combination with `KHR_materials_ior`, the `ior` affects the strength of the transmission effect according to the Fresnel term. See [`KHR_materials_transmission`](../KHR_materials_transmission/README.md) for more information. If `KHR_materials_specular` is used in addition, the rules for multiplying f0 from `ior` and f0 from the specular parameters apply as defined in `KHR_materials_specular`.

If `KHR_materials_volume` is used in combination with `KHR_materials_ior`, the `ior` affects the refraction effect. See [`KHR_materials_volume`](../KHR_materials_volume/README.md) for more information.

## Schema

- [glTF.KHR_materials_specular.schema.json](schema/glTF.KHR_materials_specular.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2020 The Khronos Group Inc.

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

Vulkan is a registered trademark and Khronos, OpenXR, SPIR, SPIR-V, SYCL, WebGL,
WebCL, OpenVX, OpenVG, EGL, COLLADA, glTF, NNEF, OpenKODE, OpenKCAM, StreamInput,
OpenWF, OpenSL ES, OpenMAX, OpenMAX AL, OpenMAX IL, OpenMAX DL, OpenML and DevU are
trademarks of The Khronos Group Inc. ASTC is a trademark of ARM Holdings PLC,
OpenCL is a trademark of Apple Inc. and OpenGL and OpenML are registered trademarks
and the OpenGL ES and OpenGL SC logos are trademarks of Silicon Graphics
International used under license by Khronos. All other product names, trademarks,
and/or company names are used solely for identification and belong to their
respective owners.
