# KHR\_materials\_clearcoat

## Khronos 3D Formats Working Group

* Alexey Knyazev
* Don McCurdy
* Ed Mackey
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Romain Guy

## Acknowledgments

* TODO: Add names

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
  
The clearcoat formula is the same as the specular term from the Metallic-Roughness material.
Using F0 equal 0.04:  
  
```
blendFactor = clearcoatFactor * fresnel(0.04, NdotV)
f = (f_emissive + f_diffuse + f_specular) * (1.0 - blendFactor) + f_clearcoat * blendFactor
```
(see `pbr-coated.glsl` from Substance Painter)  

If `clearcoatFactor = 0` the clearcoat layer is disabled and the material is behaving like the core Metallic-Roughness material:

```
f = f_emissive + f_diffuse + f_specular
```
  
If textures are not set, the default values of the clearcoat layer textures are used and the values are not inherited from the underlying Metallic-Roughness material. If one wants to have the same textures, one have to explicitly set the same texture source.

## Appendix

TODO

## Reference

### Theory and Documentation

[Allegorithmic PBR Guide Part 2](https://academy.substance3d.com/courses/the-pbr-guide-part-2)  
[Autodesk Clearcoat](https://autodesk.github.io/standard-surface/#closures/coating)  
[Dassault Clearcoat](https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec.md.html#components/clearcoat)  
[Disney BRDF from BRDF Explorer](https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf)    
[Physically-Based Shading at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)
[Unreal Clearcoat](https://docs.unrealengine.com/en-US/Engine/Rendering/Materials/MaterialInputs/index.html#clearcoat)  

### Different Implementations

[Blender](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)  
[Filament](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/clearcoat)  
[Substance Painter](https://docs.substance3d.com/spdoc/version-2018-3-172823522.html#Version2018.3-UpdatedClearCoatShader)  
  