# KHR\_materials\_ior

## Khronos 3D Formats Working Group

* TODO

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The dielectric BRDF of the metallic-roughness material in glTF uses a fixed value of 1.5 for the index of refraction. This is a good fit for many plastics and glass, but not for other materials like water or asphalt, sapphire or diamond. This extensions allows users to set the index of refraction to a certain value.

## Extending Materials

The index of refraction of a material is configured by adding the `KHR_materials_ior` extension to any glTF material. 

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_ior": {
                    "ior": 1.4,
                }
            }
        }
    ]
}
```

| |Type|Description|Required|
|-|----|-----------|--------|
| **ior** | `number` | The index of refraction. | No, default: `1.5`|

Typical values for the index of refraction range from 1 to 2. In rare cases values greater than 2 are possible. The following table shows some materials and their index of refraction:

| Material     | Index of Refraction |
|--------------|---------------------|
| Air          | 1.0                 |
| Water        | 1.33                |
| Eyes         | 1.38                |
| Window Glass | 1.52                |
| Sapphire     | 1.76                |
| Diamond      | 2.42                |

The reflectance at normal incidence (F0) of dielectric materials is computed from the IOR as follows:

```
f0 = ((ior - outside_ior) / (ior + outside_ior))^2
```

The `outside_ior` determines the index of refraction of the medium on the outside. If the renderer does not track the IOR when traversing nested dielectrics, it can assume `outside_ior = 1`.

For `ior = 1.5` and `outside_ior = 1`, the result is `f0 = 0.04`. This is the fixed value used in the glTF 2.0 metallic-roughness material. The extension makes this value configurable.


## Implementation

*This section is non-normative*

The extension changes the computation of the Fresnel term defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) to the following:

```
dielectricSpecular = ((ior - 1)/(ior + 1))^2
```

## Interaction with other extensions

If `KHR_materials_ior` is used in combination with `KHR_materials_specular`, f0 computed from IOR is multiplied by f0 computed from the specular parameters.

```
dielectricSpecularF0 = ((ior - outside_ior) / (ior + outside_ior))^2 * specularFactor * specularTexture.a * specularColorFactor * specularColorTexture.rgb
dielectricSpecularF90 = specularFactor * specularTexture.a

F0  = lerp(dielectricSpecularF0, baseColor.rgb, metallic)
F90 = lerp(dielectricSpecularF90, 1, metallic)

F = F0 + (F90 - F0) * (1 - VdotH)^5
```

If `KHR_materials_volume` is used in combination with `KHR_materials_ior`, the `ior` affects not only the Fresnel term, but also determines the refractive index of the volume below the surface. Light rays passing through the transmissive surface are bent according to the index of refraction of the outside and inside medium (refraction). As the volume cannot be parametrized with a 2-dimensional texture in UV space, the index of refraction (IOR) is a scalar, uniform value (`ior`).

## Schema

- [glTF.KHR_materials_specular.schema.json](schema/glTF.KHR_materials_specular.schema.json)
