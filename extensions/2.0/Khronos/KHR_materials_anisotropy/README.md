# KHR\_materials\_anisotropy

## Contributors

- Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)
- Ed Mackey, AGI [@emackey](https://github.com/emackey)
- Alex Wood, AGI [@abwood](https://github.com/abwood)
- Nicolas Savva, Autodesk, [@nicolassavva-autodesk](https://github.com/nicolassavva-autodesk)
- Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
- Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
- Don McCurdy, Individual Contributor [@donrmccurdy](https://github.com/donmccurdy)
- Emmett Lalish, Google [@elalish](https://github.com/elalish)
- Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)
- Bruce Cherniak, Intel
- Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
- Ben Houston, Threekit, [@bhouston](https://twitter.com/BenHouston3D)
- Jim Eckerlein, UX3D
- Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
- Eric Chadwick, Wayfair, [echadwick-wayfair](https://github.com/echadwick-wayfair)

Copyright (C) 2021-2023 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines the anisotropic property of a material as observable with brushed metals for example.
An asymmetric specular lobe model is introduced to allow for such phenomena.
The visually distinct feature of that lobe is the elongated appearance of the specular reflection.

## Extending Materials

Sample values:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_anisotropy": {
                    "anisotropyStrength": 0.6,
                    "anisotropyRotation": 1.57,
                    "anisotropyTexture": {
                        "index": 0
                    }
                }
            }
        }
    ]
}
```

|                         | Type     | Description               | Required           |
| ----------------------- | -------- | ------------------------- | ------------------ |
| **anisotropyStrength**  | `number` | The anisotropy strength. When the anisotropy texture is present, this value is multiplied by the texture's blue channel. | No, default: `0.0` |
| **anisotropyRotation**  | `number` | The rotation of the anisotropy in tangent, bitangent space, measured in radians counter-clockwise from the tangent. When the anisotropy texture is present, this value provides additional rotation to the vectors in the texture. | No, default: `0.0` |
| **anisotropyTexture**   | [`textureInfo`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The anisotropy texture. Red and green channels represent the anisotropy direction in $[-1, 1]$ tangent, bitangent space to be rotated by the anisotropy rotation. The blue channel contains strength as $[0, 1]$ to be multiplied by the anisotropy strength. | No |

## Anisotropy

Two new material properties are introduced: a strength parameter and the direction in which the specular reflection elongates relative to the surface tangents.
The strength parameter is a dimensionless number in the range $[0, 1]$ and increases the roughness along a chosen direction. The default direction aligns with the tangent to the mesh as described in the glTF 2.0 specification, [Meshes section](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes).

A mesh primitive using an anisotropy material **MUST** have a defined tangent space, i.e., it **MUST** have `NORMAL` and `TANGENT` attributes or its base material **MUST** have a normal texture. When the mesh primitive does not have `NORMAL` or `TANGENT` vectors, they are computed as defined in the glTF 2.0 specification.

Since the glTF 2.0 specification does not mandate any particular tangent space derivation algorithm, mesh primitives using anisotropy materials **SHOULD** always provide their `NORMAL` and `TANGENT` vectors.

When the material has both `normalTexture` and `anisotropyTexture` properties defined, these textures **SHOULD** use the same texture coordinates because they operate in the same tangent space and their texel values are usually correlated to each other.

The anisotropy texture, when supplied, encodes XY components of the anisotropy direction vector in tangent space as red and green values, and the anisotropy strength as blue values, all stored with linear transfer function. After dequantization, red and green texel values **MUST** be mapped as follows: red $[0, 1]$ to X $[-1, 1]$, green $[0, 1]$ to Y $[-1, 1]$.  Blue does not require remapping.  When the anisotropy texture is not supplied, the default dequantized texel value is $(1.0; 0.5; 1.0)$, which corresponds to the $(1; 0)$ direction vector (+X axis) and full strength.

The direction of this XY vector specifies the per-texel direction of increased anisotropy roughness in tangent, bitangent space, prior to being rotated by the `anisotropyRotation` property value.  After dequantization, the blue channel contains strength as $[0, 1]$ to be multiplied by the `anisotropyStrength` property value to determine the per-texel anisotropy strength.

> **Note:** The direction vector of the anisotropy is the direction in which highlights will be stretched. The direction of the micro-grooves in the material causing the anisotropy will run perpendicular.

The direction of increased anisotropy roughness (as determined above, by default the tangent direction), has its "alpha roughness" (the square of the roughness) increased according to the following formula:

*directionAlphaRoughness* = mix( *materialAlphaRoughness*, 1.0, *strength*<sup>2</sup> )

In the above, *materialAlphaRoughness* is defined as:

*materialAlphaRoughness* = *materialRoughness*<sup>2</sup>

Finally, *mix* is defined as:

mix( *x*, *y*, *m* ) = *x* * ( 1.0 - *m* ) + ( *y* * *m* )

The roughness of the perpendicular direction (by default, the bitangent direction) is equal to the material's specified roughness.

These two different *alphaRoughness* values, call them *&alpha;<sub>t</sub>* and *&alpha;<sub>b</sub>* contribute to an extended BRDF distribution term from Burley, parameterized here in terms of the halfway vector, $\boldsymbol h$, between the view and light directions:

$$
D(\boldsymbol h) = \frac{1}{\pi \alpha_t \alpha_b\left( (\boldsymbol h \cdot \boldsymbol t)^2/\alpha_t^2 + (\boldsymbol h \cdot \boldsymbol b)^2/\alpha_b^2 + (\boldsymbol h \cdot \boldsymbol n)^2 \right)^2}
$$

From this distribution function a masking/shadowing function approximation can also be derived, similarly as for the single-$\alpha$ distribution function.

## Implementation

*This section is non-normative.*

In the following example, `u_AnisotropyStrength` is set to this extension's `anisotropyStrength`, and `u_AnisotropyRotation` is initialized as `[ cos(anisotropyRotation), sin(anisotropyRotation) ]`.

The default value of `anisotropyRotation` is zero, so when this parameter is not supplied by glTF, `u_AnisotropyRotation` will be `[ 1.0, 0.0 ]`.

### Individual lights

For a directional or point light where the light direction is a single vector, the following provides a sample implementation:

```glsl
uniform float u_AnisotropyStrength;
uniform vec2 u_AnisotropyRotation;

float anisotropy = u_AnisotropyStrength;
vec2 direction = u_AnisotropyRotation;

#if HAS_ANISOTROPY_MAP
vec3 anisotropyTex = texture(u_AnisotropyTextureSampler, uv).rgb;
direction = anisotropyTex.rg * 2.0 - vec2(1.0);
direction = mat2(u_AnisotropyRotation.x, u_AnisotropyRotation.y, -u_AnisotropyRotation.y, u_AnisotropyRotation.x) * normalize(direction);
anisotropy *= anisotropyTex.b;
#endif

vec3 anisotropicT = normalize(TBN * vec3(direction, 0.0));
vec3 anisotropicB = normalize(cross(normal_geometric, anisotropicT));

float TdotL = dot(anisotropicT, l);
float BdotL = dot(anisotropicB, l);
float TdotH = dot(anisotropicT, h);
float BdotH = dot(anisotropicB, h);

f_specular += intensity * NdotL * BRDF_specularAnisotropicGGX(f0, f90, alphaRoughness,
    VdotH, NdotL, NdotV, NdotH,
    BdotV, TdotV, TdotL, BdotL, TdotH, BdotH, anisotropy);
```

The anisotropic GGX can be approximated as follows:

```glsl
vec3 BRDF_specularAnisotropicGGX(vec3 f0, vec3 f90, float alphaRoughness,
    float VdotH, float NdotL, float NdotV, float NdotH, float BdotV, float TdotV,
    float TdotL, float BdotL, float TdotH, float BdotH, float anisotropy)
{
    float at = mix(alphaRoughness, 1.0, anisotropy * anisotropy);
    float ab = alphaRoughness;

    vec3 F = F_Schlick(f0, f90, VdotH);
    float V = V_GGX_anisotropic(NdotL, NdotV, BdotV, TdotV, TdotL, BdotL, at, ab);
    float D = D_GGX_anisotropic(NdotH, TdotH, BdotH, at, ab);

    return F * V * D;
}

float D_GGX_anisotropic(float NdotH, float TdotH, float BdotH, float at, float ab)
{
    float a2 = at * ab;
    vec3 f = vec3(ab * TdotH, at * BdotH, a2 * NdotH);
    float w2 = a2 / dot(f, f);
    return a2 * w2 * w2 / M_PI;
}

float V_GGX_anisotropic(float NdotL, float NdotV, float BdotV, float TdotV, float TdotL, float BdotL,
    float at, float ab)
{
    float GGXV = NdotL * length(vec3(at * TdotV, ab * BdotV, NdotV));
    float GGXL = NdotV * length(vec3(at * TdotL, ab * BdotL, NdotL));
    float v = 0.5 / (GGXV + GGXL);
    return clamp(v, 0.0, 1.0);
}
```

The parametrization of `at` and `ab`, denoting linear roughness values along both anisotropic directions respectively.

### Image-based lighting

For image-based lighting (IBL), generally some form of PMREM (prefiltered mipmapped radiance environment map) is used since calculating the contribution from each pixel individually is too slow for realtime graphics. In this case one must sample from this PMREM since a light vector is not available.

Since the PMREM is sampled for a single roughness value and direction, one sample is not in general enough to get a very accurate approximation of the lighting. Implementors are encouraged to trade off speed and accuracy with their sampling strategies.

The two $\alpha$ values are defined in such a way that the material's base roughness (also the minimum of the two directions) is a good choice for sampling the PMREM. The center of the reflection distribution is a good direction to start sampling, especially if using only a single sample. This can be approximated with the bent normal technique:

```glsl
vec3 bentNormal = cross(anisotropicB, viewDir);
bentNormal = normalize(cross(bentNormal, anisotropicB));
// This heuristic can probably be improved upon
float a = pow2(pow2(1.0 - anisotropy * (1.0 - roughness)));
bentNormal = normalize(mix(bentNormal, normal, a));
vec3 reflectVec = reflect(-viewDir, bentNormal);
// Mixing the reflection with the normal is more accurate both with and without anisotropy and keeps rough objects from gathering light from behind their tangent plane.
reflectVec = normalize(mix(reflectVec, bentNormal, roughness * roughness));
f_specular += SamplePMREM(envMap, reflectVec, roughness);
```

Further samples should be placed on both sides along the `anisotropicT` direction, spaced according to $\alpha_t$. This is more important when $\alpha_b$ is small, meaning the base material is shiny.

## Schema

- [material.KHR_materials_anisotropy.schema.json](schema/material.KHR_materials_anisotropy.schema.json)

## Reference

- [Google Filament - Anisotropic model](https://google.github.io/filament/Filament.md.html#materialsystem/anisotropicmodel)
- [Google Filament Materials Guide - Anisotropic model](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/anisotropy)
- [Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)
- [Christopher Kulla and Alejandro Conty. 2017. Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf)

## Appendix: Full Khronos Copyright Statement

Copyright 2021-2023 The Khronos Group Inc.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast,
or otherwise exploited in any manner without the express prior written permission
of Khronos.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
https://www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters under the process defined by Khronos for this specification;
see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

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
SPIR™, SPIR&#8209;V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc.
OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in
China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc.
and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks
of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of
ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely
for identification and belong to their respective owners.
