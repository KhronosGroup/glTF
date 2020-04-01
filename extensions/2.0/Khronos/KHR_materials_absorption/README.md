# KHR\_materials\_absorption

## Contributors

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Jim Eckerlein, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

Copyright (C) 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

* Requires the `KHR_materials_transmission` extension.

## Exclusions

## Overview

This extension adds an absorption effect to transmissive materials, varying by the material's thickness.

## Extending Materials

The absorption property can be defined per material by adding the `KHR_materials_absorption` extension to any glTF material.
The extension is ignored if the material it is applied to does not have the `KHR_materials_transmission` extension enabled.
To achive a non-uniform color absorption, a thickness map is required.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_absorption": {
                    "absorptionColor": [0.0, 0.36, 2.3],
                },
                "KHR_materials_transmission": {
                    "transmission": 1.0
                },
                "KHR_materials_thickness": {
                    "thickness": 2.0,
                    "thicknessTexture": {
                        "index" : 0,
                        "texCoord" : 0
                    }
                }
            }
        }
    ]
}
```

### Absorption

|                                  | Type                                                                            | Description                 | Required             |
|----------------------------------|---------------------------------------------------------------------------------|-----------------------------|----------------------|
|**absorptionColor**               | `array`                                                                         | The absorption color.       | No                   |

The absorption effect is applied on a given transmission color as follows:

```
f_transmission *= exp(-absorptionColor * distance);
```

The `distance` variable signifies the distance of the light ray travelling through the material, therefore controlling the color fall-off.

The required `KHR_materials_transmission` extension optionally depends on the `KHR_materials_ior` extension. If an index of refraction is therefore defined, it should be taken into account when computing the refracted light's travel distance through the medium.

## Schema

- [glTF.KHR_materials_absorption.schema.json](schema/glTF.KHR_materials_absorption.schema.json)

## Reference

### Theory, Documentation and Implementations

[Filament Material models - Absorption](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/absorption)

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
