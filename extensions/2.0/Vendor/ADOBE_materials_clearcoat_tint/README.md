# ADOBE\_materials\_clearcoat\_tint

## Contributors

- Mike Bond, Adobe, [@miibond](https://twitter.com/miibond)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.
- Requires the `KHR_materials_clearcoat` extension to also be defined on the material

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a way to tint the clear coating provided by the `KHR_materials_clearcoat` extension.

## Extending Materials

Adding tint to clearcoat materials can be done by adding the `ADOBE_materials_clearcoat_tint` extension to any glTF material that already has the `KHR_materials_clearcoat` extension defined on it.  For example, the following defines a material with a dark red varnish.

```json
{
    "materials": [
        {
            "name": "varnish",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0
                },
                "ADOBE_materials_clearcoat_tint": {
                    "clearcoatTintFactor": [
                      0.4,
                      0.1,
                      0.1
                    ]
                }
            }
        }
    ]
}
```

### Clearcoat Tint

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix B](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation) for more details on the BRDF calculations.

As with the core glTF 2.0 spec, all coloured textures are assumed to be sRGB and all coloured factors are assumed to be linear.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatTintFactor**    | `number[3]`                                                                                       | The clearcoat tint's colour factor.  | No, default: `[1.0, 1.0, 1.0]`   |
|**clearcoatTintTexture**   | [`textureInfo`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo)  | The clearcoat layer's tint texture.  | No                               |

The clearcoat tint defines the transmittance of light through the coat. That is, it defines what colour of light is allowed to pass through. As with other factor/texture combinations, these two values are multiplied.
```
clearcoat_tint.rgb = clearcoatTintFactor.rgb * clearcoatTintTexture.rgb
```
The `clearcoat_tint` modifies the formula for `f_clearcoat` (as defined in [KHR_materials_clearcoat](/extensions/2.0/Khronos/KHR_materials_clearcoat/README.md)) only slightly. The `(f_emissive + f_diffuse + f_specular)` term is simply multiplied by the clearcoat tint to represent the absorption of all light coming from the layer below.

```
clearcoatBlendFactor = clearcoatTexture.r * clearcoatFactor
clearcoatFresnel = fresnel(0.04, NdotV)

color = mix(vec3(1.0), clearcoat_tint, clearcoatBlendFactor) * (f_emissive + f_diffuse + f_specular) * (1.0 - clearcoatBlendFactor * clearcoatFresnel) +
        f_clearcoat * clearcoatBlendFactor
```

## Schema

- [material.ADOBE_materials_clearcoat_tint.schema.json](schema/material.ADOBE_materials_clearcoat_tint.schema.json)
 
