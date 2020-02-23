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

Typical values for the index of refraction range from 1 to 2. In rare cases values greater than 2 are possible. The following table shows some materials and their index of refraction:

| Material     | Index of Refraction |
|--------------|---------------------|
| Air          | 1.0                 |
| Water        | 1.33                |
| Eyes         | 1.38                |
| Window Glass | 1.52                |
| Sapphire     | 1.76                |
| Diamond      | 2.42                |

The index of refraction (IOR) can be defined either as a scalar value (`ior`) or as a texture (`f0Texture`). If both are given at the same time, `f0Texture` overrides `ior`.

| |Type|Description|Required|
|-|----|-----------|--------|
| **ior** | `number` | The index of refraction. | No, default: `1.5`|
| **f0Texture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A 1-channel luminance texture that defines the index of refraction in terms of the reflectance at normal incidence f0. | No |
| **maxF0** | `number` | When using `f0Texture`, a texture value of 1.0 is interpret as `maxF0`. Values between 0 and 1 are linearly scaled between 0 and `maxF0`. | No, default: `0.08` |

The texture stores the IOR in terms of the reflectance at normal incidence, a value in the range 0..1. The mapping works as follows:

```
f0 = ((ior - 1) / (ior + 1))^2
ior = 2 / (1 - sqrt(f0)) - 1
```

As f0 usually is a small value (IOR in range 1..2 corresponds to f0 in range 0..0.08), an optional scale factor can be applied to the texture via `maxF0`.

In case `KHR_materials_volume` is enabled, the `ior` affects the bending of light rays (refraction) passing through the transparent surface and it determines the refractive index of the volume below the surface. As the volume cannot be parametrized with a 2-dimensional texture in UV space, `f0Texture` and `maxF0` are ignored.

## Implementation

The index of refraction affects the Fresnel term in the dielectric BRDF:

```
dielectric_brdf =
  fresnel_mix(
    diffuse_brdf(baseColor),
    microfacet_brdf(roughness^2),
    ior)
```

[Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) defines the `fresnel_mix` operation as

```
fresnel_mix(base, layer, ior) = base * (1 - fr(ior)) + layer * fr(ior)
fr(ior) = f0 + (1 - f0) * (1 - cos)^5
f0 = ((ior - 1) / (ior + 1))^2
```

with `ior = 1.5`, corresponding to `f0 = 0.04`. The following pseudo-code shows how to compute `f0` from `ior`, `f0Texture` and `maxF0`:

```
if (f0Texture is enabled) {
  f0 = fetch(f0Texture, uv) * maxF0;
} else {
  f0 = ((ior - 1) / (ior + 1))^2
}
```
