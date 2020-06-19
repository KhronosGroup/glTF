# KHR\_materials\_sheen

## Contributors

* Sebastien Vandenberghe, Microsoft, [@sebavan](https://github.com/sebavan)
* Alexis Vaginay, Microsoft, [@Popov72](https://github.com/Popov72)
* Tobias Häußler, Dassault System, [@proog128](https://github.com/proog128)
* Ed Mackey, Analytical Graphics, Inc.
* Romain Guy, Google, [@romainguy](https://github.com/romainguy)
* Mike Bond, Adobe, [@MiiBond](https://github.com/MiiBond)
* Don McCurdy, Google, [@donmccurdy](https://twitter.com/donrmccurdy)
* Gary Hsu, Microsoft, [@bghgary](https://twitter.com/bghgary)
* Eliphas NUIT, Dassault System, [@EliphasNUIT](https://github.com/EliphasNUIT)
* Aidin Abedi, Epic Games, [@aidinabedi](https://github.com/aidinabedi)

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

This extension defines a sheen that can be layered on top of an existing glTF material definition. A sheen layer is a common technique used in Physically-Based Rendering to represent cloth and fabric materials, for example. See [Theory, Documentation and Implementations](#theory-documentation-and-implementations)

## Extending Materials

The PBR sheen materials are defined by adding the `KHR_materials_sheen` extension to any compatible glTF material (excluding those listed above). 
For example, the following defines a material like velvet.

```json
{
    "materials": [
        {
            "name": "velvet",
            "extensions": {
                "KHR_materials_sheen": {
                    "sheenColorFactor": [0.9, 0.9, 0.9]
                }
            }
        }
    ]
}
```

### Sheen

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                            | Description                            | Required                       |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|--------------------------------|
|**sheenColorFactor**                   | `array`                                                                         | The sheen color in linear space        | No, default: `[0.0, 0.0, 0.0]` |
|**sheenTexture**         | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The sheen color (RGB) and roughness (Alpha) texture.<br> The sheen color is in sRGB transfer function | No               |
|**sheenRoughnessFactor**               | `number`                                                                        | The sheen roughness.                   | No, default: `0.0`             |

The sheen roughness is independent from the material roughness to allow materials like this one, with high material roughness and small sheen roughness:

![Cushion](./figures/cushion.png)

If a texture is defined: 
* The sheen color is computed with : `sheenColor = sheenColorFactor * sampleLinear(sheenTexture).rgb`.
* The sheen roughness is computed with : `sheenRoughness = sheenRoughnessFactor * sample(sheenTexture).a`.

Otherwise, `sheenColor = sheenColorFactor` and `sheenRoughness = sheenRoughnessFactor`

The sheen formula `f_sheen` is a new BRDF, different from the specular and clear coat BRDF:
```glsl
sheenTerm = sheenColor * sheenDistribution * sheenVisibility;
```

As you notice, there is no fresnel term for the sheen layer.

### Sheen distribution

The sheen distribution follows the "Charlie" sheen definition from ImageWorks (Estevez and Kulla):
```glsl
alphaG = sheenRoughness * sheenRoughness
invR = 1 / alphaG
cos2h = NdotH * NdotH
sin2h = 1 - cos2h
sheenDistribution = (2 + invR) * pow(sin2h, invR * 0.5) / (2 * PI);
```

### Sheen visibility

The "Charlie" sheen visibility is also defined in the same document:
```glsl
float l(float x, float alphaG)
{
    float oneMinusAlphaSq = (1.0 - alphaG) * (1.0 - alphaG);
    float a = mix(21.5473, 25.3245, oneMinusAlphaSq);
    float b = mix(3.82987, 3.32435, oneMinusAlphaSq);
    float c = mix(0.19823, 0.16801, oneMinusAlphaSq);
    float d = mix(-1.97760, -1.27393, oneMinusAlphaSq);
    float e = mix(-4.32054, -4.85967, oneMinusAlphaSq);
    return a / (1.0 + b * pow(x, c)) + d * x + e;
}

float lambdaSheen(float cosTheta, float alphaG)
{
    return abs(cosTheta) < 0.5 ? exp(l(cosTheta, alphaG)) : exp(2.0 * l(0.5, alphaG) - l(1.0 - cosTheta, alphaG));
}

sheenVisibility = 1.0 / ((1.0 + lambdaSheen(NdotV, alphaG) + lambdaSheen(NdotL, alphaG)) * (4.0 * NdotV * NdotL));
```

However, depending on device performance and resource constraints, one can use a simpler visibility term, like the one defined by Ashikhmin (but that will make the BRDF not energy conserving when using the albedo-scaling technique described below):
```glsl
sheenVisibility = 1 / (4 * (NdotL + NdotV - NdotL * NdotV))
```

### Sheen layering

#### Albedo-scaling technique

The sheen layer can be combined with the base layer with an albedo-scaling technique described in Estevez and Kulla:

```glsl
float max3(vec3 v) { return max(max(v.x, v.y), v.z); }

albedoScaling = min(1.0 - max3(sheenColor) * E(VdotN), 1.0 - max3(sheenColor) * E(LdotN))

f = f_sheen + f_base * albedoScaling
```

The values `E(x)` can be looked up in a table which can be found in section 6.2.3 of [Enterprise PBR Shading Model](#theory-documentation-and-implementations) if you use the "Charlie" visibility term. If you use Ashikhmin instead, you can get the lookup table by using the [cmgen tool from Filament](#theory-documentation-and-implementations), with the `--ibl-dfg` and `--ibl-dfg-cloth` flags: the table is in the blue channel of the generated picture. The lookup must be done with `x = VdotN` and `y = sheenRoughness`.

If you want to trade a bit of accuracy for more performance, you can use the `VdotN` term only and thus avoid doing multiple lookups for `LdotN`. The albedo scaling term is simplified to:
```glsl
albedoScaling = 1.0 - max3(sheenColor) * E(VdotN)
```

In this simplified form, it can be used to scale the base layer for both direct and indirect lights:
```glsl
specular_direct *= albedoScaling;
diffuse_direct *= albedoScaling;
environmentIrradiance_indirect *= albedoScaling
specularEnvironmentReflectance_indirect *= albedoScaling
```

## Reference

### Theory, Documentation and Implementations

[Filament Material models - Cloth model](https://google.github.io/filament/Materials.md.html#materialmodels/clothmodel)

[cmgen tool from Filament](https://github.com/google/filament)

[NP13 David Neubelt, Matt Pettineo – “Crafting a Next-Gen Material Pipeline for The Order: 1886”, SIGGRAPH 2013](https://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf)

[EK17 Alejandro Conty Estevez, Christopher Kulla – “Production Friendly Microfacet Sheen BRDF”, SIGGRAPH 2017](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_sheen.pdf)

[AS07 Michael Ashikhmin, Simon Premoze – “Distribution-based BRDFs”, 2007](http://www.cs.utah.edu/~premoze/dbrdf/dBRDF.pdf)

[cloth-shading](https://knarkowicz.wordpress.com/2018/01/04/cloth-shading/)

[Enterprise PBR Shading Model - Sheen](https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec-2021x.md.html#components/sheen)

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

In particular, the referenced paragraphs of ISO 16684-1:2019 are normative and so are
INCLUDED in the Scope of this Specification and may contain contributions from non-members
of Khronos that are not covered by the Khronos Intellectual Property Rights Policy.
The links to https://github.com/adobe/xmp-docs are purely informative and so are EXCLUDED
from the Scope of this Specification, but are provided for convenience.
The references to ISO 16684-1:2019 shall remain normative if there are differences to
information provided at any informative links.

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