<!--
Copyright 2018-2020 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_materials\_clearcoat

## Contributors

- Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
- Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
- Alexey Knyazev [@lexaknyazev](https://github.com/lexaknyazev)
- Don McCurdy, Google [@donrmccurdy](https://twitter.com/donrmccurdy)
- Sebastien Vandenberghe, Microsoft [@sebavanjs](https://twitter.com/sebavanjs)
- Romain Guy, Google [@romainguy](https://twitter.com/romainguy)
- Ed Mackey, AGI [@emackey](https://github.com/emackey)
- Alex Wood, AGI [@abwood](https://github.com/abwood)

Copyright 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a clear coating that can be layered on top of an existing glTF material definition.  A clear coat is a common technique used in Physically-Based Rendering to represent a protective layer applied to a base material.  See [Theory, Documentation and Implementations](#theory-documentation-and-implementations)

## Extending Materials

The PBR clearcoat materials are defined by adding the `KHR_materials_clearcoat` extension to any compatible glTF material (excluding those listed above).  For example, the following defines a material like varnish using clearcoat parameters.

```json
{
    "materials": [
        {
            "name": "varnish",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0
                }
            }
        }
    ]
}
```

### Clearcoat

The following parameters are contributed by the `KHR_materials_clearcoat` extension:
|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatFactor**               | `number`                                                                        | The clearcoat layer intensity.         | No, default: `0.0`   |
|**clearcoatTexture**              | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo)             | The clearcoat layer intensity texture. | No                   |
|**clearcoatRoughnessFactor**      | `number`                                                                        | The clearcoat layer roughness.         | No, default: `0.0`   |
|**clearcoatRoughnessTexture**     | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo)             | The clearcoat layer roughness texture. | No                   |
|**clearcoatNormalTexture**        | [`normalTextureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-material-normaltextureinfo) | The clearcoat normal map texture.      | No                   |

If `clearcoatFactor` (in the extension) is zero, the whole clearcoat layer is disabled.

The values for clearcoat layer intensity and clearcoat roughness can be defined using factors, textures, or both. If the `clearcoatTexture` or `clearcoatRoughnessTexture` is not given, respective texture components are assumed to have a value of 1.0. All clearcoat textures contain RGB components in linear space. If both factors and textures are present, the factor value acts as a linear multiplier for the corresponding texture values.

```
clearcoat = clearcoatFactor * clearcoatTexture.r
clearcoatRoughness = clearcoatRoughnessFactor * clearcoatRoughnessTexture.g
```

If `clearcoatNormalTexture` is not given, no normal mapping is applied to the clear coat layer, even if normal mapping is applied to the base material.  Otherwise, `clearcoatNormalTexture` may be a reference to the same normal map used by the base material, or any other compatible normal map.

A mesh primitive using a clearcoat material with a clearcoat normal texture **MUST** have a defined tangent space, i.e., it **MUST** have `NORMAL` and `TANGENT` attributes or its base material **MUST** have a normal texture. When the mesh primitive does not have `NORMAL` or `TANGENT` vectors, they are computed as defined in the glTF 2.0 specification.

Since the glTF 2.0 specification does not mandate any particular tangent space derivation algorithm, mesh primitives using clearcoat materials with clearcoat normal textures **SHOULD** always provide their `NORMAL` and `TANGENT` vectors.

When the material has both `normalTexture` and `clearcoatNormalTexture` properties defined, these textures **SHOULD** use the same texture coordinates because they operate in the same tangent space and their texel values are usually correlated to each other.

The clearcoat effect is modeled via a microfacet BRDF. The BRDF is layered on top of the glTF 2.0 Metallic-Roughness material, including emission and all extensions, using a new `fresnel_coat` function:

```
# from glTF 2.0 Metallic-Roughness material (core)
material = mix(dielectric_brdf, metal_brdf, metallic)

# clearcoat
clearcoat_brdf = specular_brdf(
  normal = clearcoatNormal,
  α = clearcoatRoughness^2)

# layering
coated_material =
  fresnel_coat(
    normal = clearcoatNormal,
    ior = 1.5,
    weight = clearcoat,
    base = material,
    layer = clearcoat_brdf)
```

The `fresnel_coat` function adds a BRDF as a layer on top of another BSDF according to `weight` and a Fresnel term. The layer is weighted with `weight*fresnel(ior)`. The base is weighted with `1-(weight*fresnel(ior))`. `normal` is a float3 vector that affects the top layer, but not the base.

### Implementation

*This section is non-normative.*

Implementations of the BRDF or the layering operator can vary based on device performance and resource constraints.

#### Clearcoat BRDF

The specular BRDF for the clearcoat layer `clearcoat_brdf` is computed using the specular term from the glTF 2.0 Metallic-Roughness material, defined in [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation). Roughness and normal are taken from `clearcoatNormal` and `clearcoatRoughness`.

#### Layering

The `fresnel_coat` function is computed using the Schlick Fresnel term from the glTF 2.0 Metallic-Roughness material, defined in [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation).

```
function fresnel_coat(normal, ior, weight, base, layer) {
  f0 = ((1-ior)/(1+ior))^2
  fr = f0 + (1 - f0)*(1 - abs(dot(V, normal)))^5
  return mix(base, layer, weight * fr)
}
```

Applying the functions we arrive at the coated material

```
coated_material = mix(material, clearcoat_brdf(clearcoatRoughness^2), clearcoat * (0.04 + (1 - 0.04) * (1 - VdotNc)^5))
```

and finally, substituting and simplifying, using some symbols from [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation) and `Nc` for the clearcoat normal:

```
clearcoat_fresnel = 0.04 + (1 - 0.04) * (1 - abs(VdotNc))^5
clearcoat_alpha = clearcoatRoughness^2
clearcoat_brdf = D(clearcoat_alpha) * G(clearcoat_alpha) / (4 * abs(VdotNc) * abs(LdotNc))

coated_material = mix(material, clearcoat_brdf, clearcoat * clearcoat_fresnel)
```

#### Emission

The clearcoat layer is on top of emission in the layering stack. Consequently, the emission is darkened by the Fresnel term.

```
coated_emission = emission * (1 - clearcoat * clearcoat_fresnel)
```

#### Discussion

In order to make the material energy conserving with a simple layering function, we compute the microfacet Fresnel term with `NdotV` instead of `VdotH`. That means that we ignore the orientation of the microsurface. As the clearcoat roughness is usually very low the microfacets orientation is very close to the normal direction, and `NdotV ≈ NdotL`.

The simple layering function ignores many effects that occur between clearcoat and base layer. For example:
- The clearcoat layer is assumed to be infinitely thin. There is no refraction.
- The index of refraction of clearcoat and base layer do not influence each other. The Fresnel terms are computed independently.
- There is no scattering between layers.
- There is no diffraction.

More sophisticated layering techniques that improve the accuracy of the renderings are described in [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation).

## Schema

- [material.KHR_materials_clearcoat.schema.json](schema/material.KHR_materials_clearcoat.schema.json)

## Reference

### Theory, Documentation and Implementations

- [Autodesk Standard Surface - Coating](https://autodesk.github.io/standard-surface/#closures/coating)
- [AxF - Appearance exchange Format](https://www.xrite.com/-/media/xrite/files/whitepaper_pdfs/axf/axf_whitepaper_en.pdf)
- [Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)
- [Disney BRDF Explorer - disney.brdf](https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf)
- [Enterprise PBR Shading Model - Clearcoat](https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec-2020x.md.html#components/clearcoat)
- [Filament Material models - Clear coat](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/clearcoat)
- [Physically-Based Shading at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)
- [Substance Painter - Updated Clear Coat Shader](https://docs.substance3d.com/spdoc/version-2018-3-172823522.html#Version2018.3-UpdatedClearCoatShader)
- [THE PBR GUIDE BY ALLEGORITHMIC - PART 1](https://academy.substance3d.com/courses/the-pbr-guide-part-1)
- [THE PBR GUIDE BY ALLEGORITHMIC - PART 2](https://academy.substance3d.com/courses/the-pbr-guide-part-2)
- [Unreal Engine 4 Material - Clear Coat](https://docs.unrealengine.com/en-US/Engine/Rendering/Materials/MaterialInputs/index.html#clearcoat)

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

Khronos® and Vulkan® are registered trademarks, and ANARI™, WebGL™, glTF™, NNEF™, OpenVX™,
SPIR™, SPIR-V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc.
OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in
China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc.
and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks
of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of
ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely
for identification and belong to their respective owners.
