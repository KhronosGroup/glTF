# KHR\_materials\_transmission

## Contributors

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Jim Eckerlein, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

Copyright (C) 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

* This extension optionally depends on `KHR_materials_ior`.
* This extension optionally depends on `KHR_materials_thickness`.

## Exclusions

## Overview

This extension defines a transmission weight used to mix diffuse and transission colors, allowing for translucent materials.

## Extending Materials

The transmission materials are defined by adding the `KHR_materials_transmission` extension to any compatible glTF material (excluding those listed above).  For example, the following defines a material like tinted glass using transmission parameters.

```json
{
    "materials": [
        {
            "name": "tinted_glass",
            "extensions": {
                "KHR_materials_transmission": {
                    "transmission": 0.8
                }
            }
        }
    ]
}
```

### Transmission

|                                  | Type                                                                            | Description                 | Required             |
|----------------------------------|---------------------------------------------------------------------------------|-----------------------------|----------------------|
|**transmission**                  | `number`                                                                        | The transmission intensity. | No, default: `0.0`   |

The approximation used to compute the transmission color is implementation dependant.

The `transmission` value determines how much of the transmission is visible. The transmission color is weighted together with the diffuse color: `transmission * f_transmission + (1 - transmission) * f_diffuse`. This linear combination is used as the new diffuse color. The transmitted color is computed using the specular BRDF distribution for the current material.

Because the specular BRDF distribution is used to compute the transmitted color similar to computing the reflected color, an effect like etched glass can be achived.

* When an index of refraction is defined, refraction is taken into account when computing the transmitted color.
* When a thickness is defined, it can be taken into account when computing the refracted ray, depending on the approximation model used in an implementation.

## Schema

- [glTF.KHR_materials_transmission.schema.json](schema/glTF.KHR_materials_transmission.schema.json)

## Reference

### Theory, Documentation and Implementations


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
