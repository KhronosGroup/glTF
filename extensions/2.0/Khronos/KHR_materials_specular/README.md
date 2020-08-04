# KHR\_materials\_specular

## Contributors

* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
* Ed Mackey, AGI [@emackey](https://twitter.com/emackey)
* Richard Sahlin, IKEA [@rsahlin](https://github.com/rsahlin)
* Ben Houston, ThreeKit [@bhouston](https://github.com/bhouston)
* Emmett Lalish, Google [@elalish](https://github.com/elalish)
* Don McCurdy, Google [@donrmccurdy](https://twitter.com/donrmccurdy)
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Sebastien Vandenberghe, Microsoft [@sebavanjs](https://twitter.com/sebavanjs)

Copyright (C) 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension adds two parameters to the metallic-roughness material: `specular` and `specularColor`.

`specular` allows users to configure the strength of the specular reflection in the dielectric BRDF. A value of zero disables the specular reflection, resulting in a pure diffuse material. The metal BRDF is not affected by the parameter.

`specularColor` changes the F0 color of the specular reflection in the dielectric BRDF, allowing artists to use effects known from the specular-glossiness material (`KHR_materials_pbrSpecularGlossiness`) in the metallic-roughness material.

## Extending Materials

The strength of the specular reflection is defined by adding the `KHR_materials_specular` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularFactor": 1.0,
                    "specularColorFactor": [1.0, 1.0, 1.0],
                }
            }
        }
    ]
}
```

Factor and texture are combined by multiplication to describe a single value.

| |Type|Description|Required|
|-|----|-----------|--------|
| **specularFactor** | `number` | The strength of the specular reflection. | No, default: `1.0`|
| **specularColorFactor** | `number[3]` | The F0 color of the specular reflection (RGB). | No, default: `[1.0, 1.0, 1.0]`|
| **specularTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A 4-channel texture that defines the F0 color of the specular reflection (RGB channels, encoded in sRGB) and the specular factor (A). Will be multiplied by specularFactor and specularColorFactor. | No |

The specular factor scales the microfacet BRDF in the dielectric BRDF. It also affects the diffuse BRDF; the less energy is reflected by the microfacet BRDF, the more can be shifted to the diffuse BRDF. The following image shows specular factor increasing from 0 to 1.

![](figures/specular.png)

The specular color changes the F0 color of the Fresnel that is multiplied to the microfacet BRDF. The color at grazing angles (F90) is not changed. As with specular factor, the diffuse BRDF will be weighted with the directional-dependent remaining energy according to the Fresnel. The weight is an RGB color, involving the complementary to specular color. To make it easy to use and ensure energy conservation, the RGB color is converted to scalar via `max(r, g, b)`. The following images show specular color increasing from [0,0,0] to [1,1,1] (top) and from [0,0,0] to [1,0,0] (bottom).

![](figures/specular-color.png)
![](figures/specular-color-2.png)

## Implementation

*This section is non-normative.*

The extension changes the computation of the Fresnel term defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) to the following:

```
dielectricSpecularF0 = 0.04 * specularFactor * specularTexture.a *
                              specularColorFactor * specularTexture.rgb
dielectricSpecularF90 = specularFactor * specularTexture.a

F0  = lerp(dielectricSpecularF0, baseColor.rgb, metallic)
F90 = lerp(dielectricSpecularF90, 1, metallic)

F = F0 + (F90 - F0) * (1 - VdotH)^5
```

As the Fresnel term `F` is now an RGB value, the diffuse component is also affected:

```
c_diff = lerp(baseColor.rgb * (1 - max(F0.r, F0.g, F0.b)), black, metallic)
diffuse = c_diff / PI
f_diffuse = (1 - max(F.r, F.g, F.b)) * diffuse
```

## Interaction with other extensions

If `KHR_materials_ior` is used in combination with `KHR_materials_specular`, the constant `0.04` is replaced by the value computed from the IOR.

```
dielectricSpecularF0 = ((ior - outside_ior) / (ior + outside_ior))^2 * specularFactor * specularTexture.a * specularColorFactor * specularTexture.rgb
dielectricSpecularF90 = specularFactor * specularTexture.a
```

`outside_ior` is typically set to 1.0, the index of refraction of air.

If `KHR_materials_transmission` is used in combination with `KHR_materials_specular`, the ratio of transmission and reflection computed from the Fresnel term also depends on `dielectricSpecularF0` and `dielectricSpecularF90`. The following images show a thin, transmissive material.

Specular from 0 to 1:

![](figures/specular-thin.png)

Specular color from [0,0,0] to [1,1,1] (top) and [0,0,0] to [1,0,0]:

![](figures/specular-color-thin.png)
![](figures/specular-color-thin-2.png)

If `KHR_materials_transmission` and `KHR_materials_volume` are used in combination with `KHR_materials_specular`, specular factor and specular color have no effect on the refraction angle. The direction of the refracted light ray is only based on the index of refraction defined in `KHR_materials_ior`. The ratio of transmission and reflection computed from the Fresnel term still depends on `dielectricSpecularF0` and `dielectricSpecularF90`. The following images show a refractive material.

Specular from 0 to 1:

![](figures/specular-refraction.png)

Specular color from [0,0,0] to [1,1,1] (top) and [0,0,0] to [1,0,0]:

![](figures/specular-color-refraction.png)
![](figures/specular-color-refraction-2.png)

## Conversions

### Materials with reflectance parameter

Material models that define F0 in terms of reflectance at normal incidence can be converted with the help of `KHR_materials_ior`. Typically, the reflectance ranges from 0% to 8%, given as a value between 0 and 1, with 0.5 (=4%) being the default. This default corresponds to an IOR of 1.5. F0 is then computed from the `reflectanceFactor` in range 0..1 in the following way:

```
dielectricSpecularF0 = 0.08 * reflectanceFactor
```

| reflectance | reflectanceFactor | IOR      |
|-------------|-------------------|----------|
| 0%          | 0                 | 1.0      |
| 4%          | 0.5               | 1.5      |
| 8%          | 1                 | 1.788789 |

As shown in the table, the constant 0.08 corresponds to an index of refraction of 1.788789. By setting `ior` in `KHR_materials_ior` to this value, the behavior of `specularColorFactor` becomes identical to `reflectanceFactor` (the RGB components are equal). In JSON:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularColorFactor": [reflectanceFactor],
                },
                "KHR_materials_ior": {
                    "ior": 1.788789
                }
            }
        }
    ]
}
```

### Specular-glossiness materials

Materials that use the specular-glossiness workflow (`KHR_materials_pbrSpecularGlossiness`) can be converted with help of the `KHR_materials_ior`. The `ior` parameter has to be set to 0 or inf. In JSON:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularColorFactor":
                        [KHR_materials_pbrSpecularGlossiness__specularFactor],
                },
                "KHR_materials_ior": {
                    "ior": 0
                }
            }
        }
    ]
}
```

This makes it possible to add advanced effects like clearcoat (`KHR_materials_clearcoat`) and sheen (`KHR_materials_sheen`) to traditional specular-glossiness materials.

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
