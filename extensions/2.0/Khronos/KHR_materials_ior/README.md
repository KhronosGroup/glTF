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

In case `KHR_materials_volume` is enabled, the `ior` determines the refractive index of the volume below the surface. Light rays passing through the transmissive surface are bent according to the index of refraction of the outside and inside medium (refraction). As the volume cannot be parametrized with a 2-dimensional texture in UV space, the index of refraction (IOR) is a scalar, uniform value (`ior`).

| |Type|Description|Required|
|-|----|-----------|--------|
| **ior** | `number` | The index of refraction. | No, default: `1.5`|

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
f0 = ((ior - outside_ior) / (ior + outside_ior))^2
```

with `ior = 1.5`, corresponding to `f0 = 0.04`. The `outside_ior` determines the index of refraction of the medium on the outside. If the renderer does not track the IOR when traversing nested dielectrics, it can assume `outside_ior = 1`.
