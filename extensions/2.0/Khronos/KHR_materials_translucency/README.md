# KHR\_materials\_translucency

## Contributors

* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)

## Acknowledgments

TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds a Lambertian diffuse transmission BSDF to the metallic-roughness material. Thin, dielectric objects like leaves or paper transmit some of the incoming light to the opposite side of the surface. For media with short scattering distances and therefore dense scattering behavior, a diffuse transmission lobe is a phenomenological plausible and cheap approximation.

## Extending Materials

The effect is activated by adding the `KHR_materials_translucency` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_translucency": {
                    "translucencyFactor": 0.7,
                    "translucencyTexture": 0,
                }
            }
        }
    ]
}
```

The strength of the effect is controlled by `translucencyFactor` and `translucencyTexture`, combined via multiplication to describe a single value.

| |Type|Description|Required|
|-|----|-----------|--------|
| **translucencyFactor** | `number` | The percentage of non-specularly reflected light that is transmitted through the surface via the Lambertian diffuse transmission, i.e., the strength of the translucency effect. | No, default: `0`|
| **translucencyTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A greyscale texture that defines the strength of the translucency effect. Will be multiplied by specularFactor and specularColorFactor. | No |

Increasing the strength of the translucency effect via the `translucency` parameter takes away energy from the diffuse reflection BSDF and passes it to the diffuse transmission BSDF. The specular reflection BSDF and Fresnel weighting are not affected. The diffuse color affects both the diffuse reflection and the diffuse transmission BSDF.

## Implementation

*This section is non-normative.*

The extension changes the diffuse term defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) to the following:

```
translucency = translucencyFactor * translucencyTexture.r
diffuse = (c_diff/pi) * lerp(diffuseBRDF, diffuseBTDF, translucency)
```

The `diffuseBRDF` is 1 if view and light direction point into the same hemisphere wrt. the normal direction, and 0 otherwise. The `diffuseBTDF` is 1 if view and light direction point into different hemispheres wrt. the normal direction, and 0 otherwise.

## Interaction with other extensions

If `KHR_materials_transmission` is used in combination with `KHR_materials_translucency`, the transmission effect overrides the tanslucency effect. The calculation given in `KHR_materials_translucency` still holds, but uses the modified diffuse term defined above: *f*<sub>*diffuse*</sub> = (1 - *F*) * (1 - *T*) * *diffuse*.

If `KHR_materials_volume` is used in combination with `KHR_materials_translucency`, the volume is entered and left through the diffuse transmission lobe. Scattering and absorption inside the volume are computed as usual, the random walk through the volume is not affected by the surface BSDF.

## Schema

- [glTF.KHR_materials_translucency.schema.json](schema/glTF.KHR_materials_translucency.schema.json)
