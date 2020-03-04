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
                    "specularTexture": 0,
                    "specularColorFactor": [1.0, 1.0, 1.0],
                    "specularColorTexture": 0
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
| **specularColorTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A 1-channel luminance or 3-channel RGB texture that defines the F0 color of the specular reflection. Will be multiplied by specularColorFactor. | No |

### Conversions

Material models that define f0 in terms of reflectance at normal incidence (with 0 meaning 0%, 0.5 meaning 4%, and 1 meaning 8%) can be converted to `specularColorFactor` and `specularColorTexture` as follows:

```
specularColorFactor = 2 * reflectanceFactor
specularColorTexture = reflectanceTexture
```

Alternatively, the index of refraction defined in `KHR_materials_ior` can be set to the value 1.788789, resulting in a f0 scale factor of 0.08.

In addition, it is possible to convert specular-glossiness materials (`KHR_materials_pbrSpecularGlossiness`) to metallic-roughness by combining `KHR_materials_specular` and `KHR_materials_ior`. The conversion is lossless and makes effects like sheen (`KHR_materials_sheen`) and clearcoat (`KHR_materials_clearcoat`) available to specular-glossiness materials.

```
ior = inf
specularColorFactor = specularFactor_specgloss
specularColorTexture = specularGlossinessTexture_specgloss[0:2]
```

The conversion is not compatible with transmissive materials defined via `KHR_materials_transmission` and `KHR_materials_volume`.

## Implementation

The specular factor scales the microfacet BRDF in the dielectric BRDF. It also affects the diffuse BRDF; the less energy is reflected by the microfacet BRDF, the more can be shifted to the diffuse BRDF.

The specular color changes the F0 color of the Fresnel that is multiplied to the microfacet BRDF. The color at grazing angles (F90) is not changed.

```
dielectric_brdf =
  fresnel_mix(
    diffuse_brdf(baseColor),
    microfacet_brdf(roughness^2),
    f0 = 0.04 * specular * specularColor,
    f90 = specular)
```

Based on [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation), the modified `fresnel_mix` operation is defined as follows:

```
fresnel_mix(base, layer, ior) = base * (1 - fr(ior)) + layer * fr(ior)
fr(ior) = f0 + (f90 - f0) * (1 - cos)^5
f0 = 0.04 * specular * specularColor
f90 = specular
```

If `KHR_materials_ior` is used in combination with `KHR_materials_specular`, the constant `0.04` is replaced by the value computed from the IOR:

```
f0 = ((ior - outside_ior) / (ior + outside_ior))^2 * specular * specularColor
f90 = specular
```

The specular factor and specular color do not affect the index of refraction of the volume below the surface, defined in `KHR_materials_volume`. As a result, `specular` and `specularColor` do not take part in computing the outgoing direction for refraction.
