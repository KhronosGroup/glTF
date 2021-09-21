# KHR\_materials\_iridescence

## Khronos 3D Formats Working Group

* Pascal Schoen, adidas  
* Ben Houston, threekit  
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)  

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

TODO

## Extending Materials

The iridescence materials are defined by adding the `KHR_materials_iridescence` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_iridescence": {
                    "iridescenceFactor": 1.0,
                    "iridescenceIOR": 1.8,
                    "iridescenceThicknessMaximum": 1200.0
                }
            }
        }
    ]
}
```

### Iridescence

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                              | Type                                                                | Description                                                                                      | Required              |
|------------------------------|---------------------------------------------------------------------|--------------------------------------------------------------------------------------------------|-----------------------|
|**iridescenceFactor**         | `number`                                                            | The iridescence intensity.                                                                       | No, default: `0.0`    |
|**iridescenceTexture**        | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The iridescence intensity texture.                                                               | No                    |
|**iridescenceIOR**               | `number`                                                            | The index of refraction of the dielectric thin-film layer.                                       | No, default: `1.8`    |
|**iridescenceThicknessMinimum**  | `number`                                                            | The minimum thickness of the thin-film layer.                                                    | No, default: `400.0`  |
|**iridescenceThicknessMaximum**  | `number`                                                            | The maximum thickness of the thin-film layer.                                                    | No, default: `1200.0` |
|**iridescenceThicknessTexture**  | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The thickness texture of the thin-film layer to blend between the minimum and maximum thickness. | No                    |

TODO

## Appendix

TODO

## Reference

### Theory, Documentation and Implementations

[Real-time Image-based Lighting of Microfacet BRDFs with Varying Iridescence](https://cg.cs.uni-bonn.de/en/publications/paper-details/kneiphof-2019-iridescence/)  
[A Practical Extension to Microfacet Theory for the Modeling of Varying Iridescence](https://hal.archives-ouvertes.fr/hal-01518344/document)  
