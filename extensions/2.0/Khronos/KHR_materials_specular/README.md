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
| **specularColorTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A 3-channel texture that defines the F0 color of the specular reflection. Will be multiplied by specularColorFactor. | No |

## Implementation

The specular factor scales the microfacet BRDF in the dielectric BRDF. It also affects the diffuse BRDF; the less energy is reflected by the microfacet BRDF, the more can be shifted to the diffuse BRDF.

The specular color changes the F0 color of the Fresnel that is multiplied to the microfacet BRDF. The color at grazing angles (F90) is not changed.

```
dielectric_brdf =
  fresnel_mix(
    diffuse_brdf(baseColor, specular, specularColor),
    microfacet_brdf(roughness^2) * specular,
    ior = 1.5,
    f0 = ((1-ior)/(1+ior))^2 * specularColor)
```

This makes `f0` wavelength-dependent, i.e., it is now a 3-channel RGB value. For the directional albedo `E` and average albedo `Eavg`, this is reduced to 1 channel by taking the maximum of the RGB channels, This avoids unexpected color shifts caused by the subtraction.

```
                1           (1 - E(VdotN, max(f0))) * (1 - E(LdotN, max(f0)))
diffuse_brdf = -- * mix(1, ---------------------------------------------------, specular)
               pi                                1 - Eavg
```

The specular color does not affect the index of refraction that may also be used in `KHR_materials_transmission` to refract light rays going through the surface.
