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
                    "iorTexture": 0
                }
            }
        }
    ]
}
```

The index of refraction typically is a value between 1 and 2, in rare cases it may be larger than 2. In contrast to other texturable values in glTF, `ior` and `iorTexture` are not multiplied: if no texture is given, `ior` is used, otherwise the texture overrides it.

| |Type|Description|Required|
|-|----|-----------|--------|
| **ior** | `number` | The index of refraction. | No, default: `1.5`|
| **iorTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A greyscale texture that defines the index of refraction as 1/ior. | No |

## Implementation

The index of refraction affects the Fresnel term in the dielectric BRDF:

```
dielectric_brdf =
  fresnel_mix(
    diffuse_brdf(baseColor),
    microfacet_brdf(roughness^2),
    ior)
```

In case `KHR_materials_transmission` is enabled, the index of refraction also affects the refraction of light rays passing through the transparent surface.
