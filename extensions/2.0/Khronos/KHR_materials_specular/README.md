# KHR\_materials\_specular

## Khronos 3D Formats Working Group

* TODO

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds two parameters to the metallic-roughness material: `specular` and `specularColor`.

`specular` allows users to configure the strength of the specular reflection in the dielectric BRDF. A value of zero disables the specular reflection, resulting in a pure diffuse material. The metal BRDF is not affected by the parameter.

`specularColor` changes the F0 color of the specular reflection in the dielectric BRDF, allowing artists to use effects known from the specular-glossiness material (`KHR_materials_pbrSpecularGlossiness`) in the metallic-roughness material.


## Extending Materials

The strength of the specular reflection is defined by adding the `KHR_materials_specular` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularFactor": 1.0,
                    "specularColorFactor": [1.0, 1.0, 1.0],
                    "specularTexture": 0,
                }
            }
        }
    ]
}
```

Factor and texture are combined by multiplication to describe a single value.

| |Type|Description|Required|
|-|----|-----------|--------|
| **specularFactor** | `number` | The strength of the specular reflection. | No, default: `1.0`|
| **specularTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A grayscale texture that defines the specular factor. Will be multiplied by specularFactor. | No |
| **specularColorFactor** | `number[3]` | The F0 color of the specular reflection (RGB). | No, default: `[1.0, 1.0, 1.0]`|
| **specularColorTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A texture that defines the F0 color of the specular reflection (RGB channels, encoded in sRGB) and the specular factor (A). Will be multiplied by specularFactor and specularColorFactor. | No |

The specular factor scales the microfacet BRDF in the dielectric BRDF. It also affects the diffuse BRDF; the less energy is reflected by the microfacet BRDF, the more can be shifted to the diffuse BRDF. The following image shows specular factor increasing from 0 to 1.

![](figures/specular.png)

The specular color changes the F0 color of the Fresnel that is multiplied to the microfacet BRDF. The color at grazing angles (F90) is not changed. As with specular factor, the diffuse BRDF will take the remaining energy, but in this case it is directional-dependent. The following images show specular color increasing from [0,0,0] to [1,1,1] (top) and from [0,0,0] to [1,0,0] (bottom).

![](figures/specular-color.png)
![](figures/specular-color-2.png)

## Implementation

*This section is non-normative.*

The extension changes the computation of the Fresnel term defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) to the following:

```
dielectricSpecularF0 = 0.04 * specularFactor * specularTexture.a *
                              specularColorFactor * specularColorTexture.rgb
dielectricSpecularF90 = specularFactor * specularTexture.a

F0  = lerp(dielectricSpecularF0, baseColor.rgb, metallic)
F90 = lerp(dielectricSpecularF90, 1, metallic)

F = F0 + (F90 - F0) * (1 - VdotH)^5
```

## Interaction with other extensions

If `KHR_materials_ior` is used in combination with `KHR_materials_specular`, the constant `0.04` is replaced by the value computed from the IOR.

```
dielectricSpecularF0 = ((ior - outside_ior) / (ior + outside_ior))^2 * specularFactor * specularTexture.a * specularColorFactor * specularColorTexture.rgb
dielectricSpecularF90 = specularFactor * specularTexture.a
```

`outside_ior` is typically set to 1.0, the index of refraction of air.

If `KHR_materials_transmission` is used in combination with `KHR_materials_specular`, the ratio of transmission and reflection computed from the Fresnel term also depends on `dielectricSpecularF0` and `dielectricSpecularF90`. The following images show a thin, transmissive material.

Specular from 0 to 1:

![](figures/specular-thin.png)

Specular color from [0,0,0] to [1,1,1] (top) and [0,0,0] to [1,0,0]:

![](figures/specular-color-thin.png)
![](figures/specular-color-thin-2.png)

If `KHR_materials_transmission` and `KHR_materials_volume` are used in combination with `KHR_materials_specular`, specular factor and specular color have no effect on the refraction angle. The direction of the refracted light ray is only based on the index of refraction defined in `KHR_materials_ior`. The ratio of transmission and reflection computed from the Fresnel term still depends on `dielectricSpecularF0` and `dielectricSpecularF90`. The following images show a refractive material.

Specular from 0 to 1:

![](figures/specular-refraction.png)

Specular color from [0,0,0] to [1,1,1] (top) and [0,0,0] to [1,0,0]:

![](figures/specular-color-refraction.png)
![](figures/specular-color-refraction-2.png)

## Conversions

### Materials with reflectance parameter

Material models that define F0 in terms of reflectance at normal incidence can be converted with the help of `KHR_materials_ior`. Typically, the reflectance ranges from 0% to 8%, given as a value between 0 and 1, with 0.5 (=4%) being the default. This default corresponds to an IOR of 1.5. F0 is then computed from the `reflectanceFactor` in range 0..1 in the following way:

```
dielectricSpecularF0 = 0.08 * reflectanceFactor
```

| reflectance | reflectanceFactor | IOR      |
|-------------|-------------------|----------|
| 0%          | 0                 | 1.0      |
| 4%          | 0.5               | 1.5      |
| 8%          | 1                 | 1.788789 |

As shown in the table, the constant 0.08 corresponds to an index of refraction of 1.788789. By setting `ior` in `KHR_materials_ior` to this value, the behavior of `specularColorFactor` becomes identical to `reflectanceFactor` (the RGB components are equal). In JSON:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularColorFactor": <reflectanceFactor>,
                },
                "KHR_materials_ior": {
                    "ior": 1.788789
                }
            }
        }
    ]
}
```

### Specular-glossiness materials

Materials that use the specular-glossiness workflow (`KHR_materials_pbrSpecularGlossiness`) can be converted with help of the `KHR_materials_ior`. The `ior` parameter has to be set to 0 or inf. In JSON:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_specular": {
                    "specularColorFactor":
                        <KHR_materials_pbrSpecularGlossiness__specularFactor>,
                },
                "KHR_materials_ior": {
                    "ior": 0
                }
            }
        }
    ]
}
```

This makes it possible to add advanced effects like clearcoat (`KHR_materials_clearcoat`) and sheen (`KHR_materials_sheen`) to traditional specular-glossiness materials.

## Schema

- [glTF.KHR_materials_specular.schema.json](schema/glTF.KHR_materials_specular.schema.json)
