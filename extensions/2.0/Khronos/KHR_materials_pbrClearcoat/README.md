# KHR\_materials\_pbrClearcoat

## Khronos 3D Formats Working Group

* Alexey Knyazev
* Ed Mackey
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Acknowledgments

* TODO: Add names

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

## Extending Materials

The PBR clearcoat materials are defined by adding the `KHR_materials_pbrClearcoat` extension to any glTF material. 
For example, the following defines a material like varnish using clearcoat parameters.

```json
{
    "materials": [
        {
            "name": "varnish",
            "extensions": {
                "KHR_materials_pbrClearcoat": {
                    "clearcoatFactor": 1.0
                }
            }
        }
    ]
}
```

### Clearcoat

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatFactor**               | `number`                                                            | The clearcoat layer intensity.         | No, default: `0.0`   |
|**clearcoatTexture**              | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The clearcoat layer intensity texture. | No                   |
|**clearcoatRoughnessFactor**      | `number`                                                            | The clearcoat layer roughness.         | No, default: `0.5`   |
|**clearcoatRoughnessTexture**     | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The clearcoat layer roughness texture. | No                   |
|**clearcoatNormalTexture**        | `normalTextureInfo`                                                 | The clearcoat normal map texture.      | No                   |

The clearcoat formula is the same as the specular part from the Metallic-Roughness Material.  
In this case, F0 is 0.04 for dielectrics using `clearcoatRoughnessFactor` default value 0.5. So a roughness factor of 0.0 maps to F0 of 0.0 and a roughness factor of 1.0 to F0 of 1.0.

If textures are not set, the default values of the clearcoat textures are used and the values are not inherited from the underlying material. If one wants to have the same textures, one have to explicitly set the same texture index.

## Appendix

## Reference

### Theory and Documentation

[Allegorithmic PBR Guide Part 2](https://academy.substance3d.com/courses/the-pbr-guide-part-2)
[Disney BRDF from BRDF Explorer](https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf)  
[Physically-Based Shading at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)  

### Exisiting Implementations

[Blender](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)
[Filament](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/clearcoat)
[Substance Painter](https://docs.substance3d.com/spdoc/version-2018-3-172823522.html#Version2018.3-UpdatedClearCoatShader)
