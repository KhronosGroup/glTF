# KHR\_materials\_specular

## Khronos 3D Formats Working Group

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

TODO  

## Extending Materials

The specular materials are defined by adding the `KHR_materials_specular` extension to any glTF material. 

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularFactor": 0.5
                }
            }
        }
    ]
}
```

### Specular

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                     | Type                                                                | Description            | Required             |
|---------------------|---------------------------------------------------------------------|------------------------|----------------------|
|**specularFactor**   | `number`                                                            | The specular.          | No, default: `0.5`   |
|**specularTexture**  | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The specular  texture. | No                   |

This extension is overwriting the default `F0 = 0.04` for non-metallic materials in the Metallic-Roughness workflow. Instead, `F0 = 0.08 * specularFactor * specularTexture` is used.

## Appendix

TODO

## Reference

### Theory, Documentation and Implementations

[Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)  
[Disney BRDF Explorer - disney.brdf](https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf)  
[Physically-Based Shading at Disney](https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf)  
[THE PBR GUIDE BY ALLEGORITHMIC - PART 1](https://academy.substance3d.com/courses/the-pbr-guide-part-1)  
[THE PBR GUIDE BY ALLEGORITHMIC - PART 2](https://academy.substance3d.com/courses/the-pbr-guide-part-2)  
