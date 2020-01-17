# KHR\_materials\_clearcoat

## Khronos 3D Formats Working Group

* Alexey Knyazev
* Don McCurdy
* Ed Mackey
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Romain Guy
* Tobias Haeussler

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

TODO  

## Extending Materials

The PBR clearcoat materials are defined by adding the `KHR_materials_clearcoat` extension to any glTF material. 
For example, the following defines a material like varnish using clearcoat parameters.

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

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatFactor**               | `number`                                                                        | The clearcoat layer intensity.         | No, default: `0.0`   |
|**clearcoatTexture**              | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The clearcoat layer intensity texture. | No                   |
|**clearcoatRoughnessFactor**      | `number`                                                                        | The clearcoat layer roughness.         | No, default: `0.0`   |
|**clearcoatRoughnessTexture**     | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The clearcoat layer roughness texture. | No                   |
|**clearcoatNormalTexture**        | [`normalTextureInfo`](/specification/2.0/README.md#reference-normaltextureinfo) | The clearcoat normal map texture.      | No                   |
  
The clearcoat formula `f_clearcoat` is the same as the specular term from the Metallic-Roughness material, using F0 equal 0.04.
The following abstract code describes how the base and clearcoat layers should be blended together:  
  
```
blendFactor = clearcoatFactor * fresnel(0.04, NdotV)
f = (f_emissive + f_diffuse) * (1.0 - blendFactor) + mix(f_specular, f_clearcoat, blendFactor)
```

If `clearcoatFactor = 0`, the clearcoat layer is disabled and the material is behaving like the core Metallic-Roughness material:

```
f = f_emissive + f_diffuse + f_specular
```
  
If textures are not set, the default values of the clearcoat layer textures are used and the values are not inherited from the underlying Metallic-Roughness (or other) material. If one wants to have the same textures, one have to explicitly set the same texture sources.

## Appendix

TODO

## Reference

### Theory, Documentation and Implementations

[Autodesk Standard Surface - Coating](https://autodesk.github.io/standard-surface/#closures/coating)  
[AxF - Appearance exchange Format](https://www.xrite.com/-/media/xrite/files/whitepaper_pdfs/axf/axf_whitepaper_en.pdf)  
[Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)  
[Disney BRDF Explorer - disney.brdf](https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf)  
[Enterprise PBR Shading Model - Clearcoat](https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec.md.html#components/clearcoat)  
[Filament Material models - Clear coat](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/clearcoat)   
[Physically-Based Shading at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)  
[Substance Painter - Updated Clear Coat Shader](https://docs.substance3d.com/spdoc/version-2018-3-172823522.html#Version2018.3-UpdatedClearCoatShader)  
[THE PBR GUIDE BY ALLEGORITHMIC - PART 1](https://academy.substance3d.com/courses/the-pbr-guide-part-1)  
[THE PBR GUIDE BY ALLEGORITHMIC - PART 2](https://academy.substance3d.com/courses/the-pbr-guide-part-2)  
[Unreal Engine 4 Material - Clear Coat](https://docs.unrealengine.com/en-US/Engine/Rendering/Materials/MaterialInputs/index.html#clearcoat)  
