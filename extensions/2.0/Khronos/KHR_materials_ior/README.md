<!--
Copyright 2018-2021 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_materials\_ior

## Contributors

- Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
- Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
- Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)
- Emmett Lalish, Google [@elalish](https://github.com/elalish)
- Don McCurdy, Google [@donrmccurdy](https://twitter.com/donrmccurdy)
- Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
- Richard Sahlin, IKEA [@rsahlin](https://github.com/rsahlin)
- Eric Chadwick, Wayfair [echadwick-wayfair](https://github.com/echadwick-wayfair)
- Ben Houston, ThreeKit [@bhouston](https://github.com/bhouston)
- Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
- Sebastien Vandenberghe, Microsoft [@sebavanjs](https://twitter.com/sebavanjs)
- Nicholas Barlow, Microsoft
- Nicolas Savva, Autodesk [@nicolassavva-autodesk](https://github.com/nicolassavva-autodesk)
- Henrik Edstrom, Autodesk
- Bruce Cherniak, Intel
- Adam Morris, Target [@weegeekps](https://github.com/weegeekps)
- Sandra Voelker, Target
- Alex Jamerson, Amazon
- Thomas Dideriksen, Amazon
- Alex Wood, AGI [@abwood](https://github.com/abwood)
- Ed Mackey, AGI [@emackey](https://github.com/emackey)

Copyright 2018-2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

The dielectric BRDF of the metallic-roughness material in glTF uses a fixed value of 1.5 for the index of refraction. This is a good fit for many plastics and glass, but not for other materials like water or asphalt, sapphire or diamond. This extension allows users to set the index of refraction to a certain value.

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

The ior parameter affects the `dielectric_brdf` of the glTF 2.0 metallic-roughness material. The new `dielectric_brdf` that includes the `ior` parameter is given as follows:

```
dielectric_brdf =
  fresnel_mix(
    ior = ior,
    base = diffuse_brdf(
      color = baseColor),
    layer = specular_brdf(
      α = roughness^2))
```

Valid values for `ior` are numbers greater than or equal to one. As a special case, a value of zero is allowed as described below.

### Specular-Glossiness Backwards Compatibility Mode

Setting IOR to zero permanently switches the material into a special specular-glossiness backwards compatibility mode designed to ease content transition from the legacy specular-glossiness model (previously available via `KHR_materials_pbrSpecularGlossiness` extension) to the glTF 2.0 core metallic-roughness PBR model.

This mode has the following implications:

- The effective IOR becomes positive infinity and the Fresnel term **MUST** evaluate to `1.0` independently of the view or light direction.

- All material features **MUST** treat IOR as having a very large value representing positive infinity, subject to numerical precision. For example, this would cause the `dispersion` property (as defined in `KHR_materials_dispersion`) to have no effect on the material appearance.

- This mode cannot be toggled dynamically, e.g., with `KHR_animation_pointer` or `KHR_interactivity` extensions. If the IOR property is set to zero in JSON, glTF Asset Object Model updates of it **MUST** be ignored.

- A value of zero (as well as any other value less than one) **MUST NOT** be used in an animation sampler targeting the IOR property, even if the IOR is set to zero in glTF JSON.

## Implementation

*This section is non-normative*

The extension changes the computation of the Fresnel term defined in [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation) to the following:

```
dielectric_f0 = ((ior - 1)/(ior + 1))^2
```

Note that for the default index of refraction `ior = 1.5` this term evaluates to `dielectric_f0 = 0.04`.

## Interaction with other extensions

If `KHR_materials_specular` is used in combination with `KHR_materials_ior`, the Fresnel term is additionally scaled by the values given `specular` and `specularColor`. See [`KHR_materials_specular`](../KHR_materials_specular/README.md) for more information.

If `KHR_materials_transmission` is used in combination with `KHR_materials_ior`, the `ior` affects the strength of the transmission effect according to the Fresnel term. See [`KHR_materials_transmission`](../KHR_materials_transmission/README.md) for more information. If `KHR_materials_specular` is used in addition, the Fresnel term is additionally scaled by the values given in `specular` and `specularColor`.

If `KHR_materials_volume` is used in combination with `KHR_materials_ior`, the `ior` affects the refraction effect. See [`KHR_materials_volume`](../KHR_materials_volume/README.md) for more information.

## Schema

- [material.KHR_materials_ior.schema.json](schema/material.KHR_materials_ior.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2021 The Khronos Group Inc.

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
