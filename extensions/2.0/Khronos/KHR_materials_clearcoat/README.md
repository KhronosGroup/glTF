# KHR\_materials\_clearcoat

## Khronos 3D Formats Working Group

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Alexey Knyazev [@lexaknyazev](https://github.com/lexaknyazev)
* Don McCurdy, Google [@donrmccurdy](https://twitter.com/donrmccurdy)
* Sebastien Vandenberghe, Microsoft [@sebavanjs](https://twitter.com/sebavanjs)
* Romain Guy, Google [@romainguy](https://twitter.com/romainguy)
* Ed Mackey, AGI [@emackey](https://twitter.com/emackey)
* Alex Wood, AGI [@abwood](https://twitter.com/abwood)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

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

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatFactor**               | `number`                                                                        | The clearcoat layer intensity.         | No, default: `0.0`   |
|**clearcoatTexture**              | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The clearcoat layer intensity texture. | No                   |
|**clearcoatRoughnessFactor**      | `number`                                                                        | The clearcoat layer roughness.         | No, default: `0.0`   |
|**clearcoatRoughnessTexture**     | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The clearcoat layer roughness texture. | No                   |
|**clearcoatNormalTexture**        | [`normalTextureInfo`](/specification/2.0/README.md#reference-normaltextureinfo) | The clearcoat normal map texture.      | No                   |
  
The clearcoat formula `f_clearcoat` is computed using the specular term from the glTF 2.0 Metallic-Roughness material, defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation).  Use F0 equal `0.04`, base color white `1.0, 1.0, 1.0`, metallic value `0.0`, and the clearcoat roughness value defined in this extension as follows:

```
clearcoatRoughness = clearcoatRoughnessFactor * clearcoatRoughnessTexture.g
```

The following abstract code describes how the base and clearcoat layers should be blended together:
  
```
clearcoatBlendFactor = clearcoatTexture.r * clearcoatFactor * fresnel(0.04, NdotV)

color = mix(f_emissive + f_diffuse + f_specular, f_clearcoat, clearcoatBlendFactor)
```

If `clearcoatFactor` is zero, the clearcoat layer is disabled.

The values for clearcoat layer intensity and clearcoat roughness can be defined using factors, textures, or both. If the `clearcoatTexture` or `clearcoatRoughnessTexture` is not given, respective texture components are assumed to have a value of 1.0. If both factors and textures are present, the factor value acts as a linear multiplier for the corresponding texture values.

If `clearcoatNormalTexture` is not given, no normal mapping is applied to the clear coat layer, even if normal mapping is applied to the base material.  Otherwise, `clearcoatNormalTexture` may be a reference to the same normal map used by the base material, or any other compatible normal map.

## Reference

### Theory, Documentation and Implementations

[Autodesk Standard Surface - Coating](https://autodesk.github.io/standard-surface/#closures/coating)  
[AxF - Appearance exchange Format](https://www.xrite.com/-/media/xrite/files/whitepaper_pdfs/axf/axf_whitepaper_en.pdf)  
[Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)  
[Disney BRDF Explorer - disney.brdf](https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf)  
[Enterprise PBR Shading Model - Clearcoat](https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec-2020x.md.html#components/clearcoat)  
[Filament Material models - Clear coat](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/clearcoat)   
[Physically-Based Shading at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)  
[Substance Painter - Updated Clear Coat Shader](https://docs.substance3d.com/spdoc/version-2018-3-172823522.html#Version2018.3-UpdatedClearCoatShader)  
[THE PBR GUIDE BY ALLEGORITHMIC - PART 1](https://academy.substance3d.com/courses/the-pbr-guide-part-1)  
[THE PBR GUIDE BY ALLEGORITHMIC - PART 2](https://academy.substance3d.com/courses/the-pbr-guide-part-2)  
[Unreal Engine 4 Material - Clear Coat](https://docs.unrealengine.com/en-US/Engine/Rendering/Materials/MaterialInputs/index.html#clearcoat)  
