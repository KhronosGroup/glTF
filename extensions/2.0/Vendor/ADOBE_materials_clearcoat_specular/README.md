# ADOBE\_materials\_clearcoat\_specular

## Contributors

* Mike Bond, Adobe, [@miibond](https://twitter.com/miibond)

## Status

Draft.

## Dependencies

Written against the glTF 2.0 spec.
* Requires the `KHR_materials_clearcoat` extension to also be defined on the material

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a way to control the index of refraction (IOR) and specular F0 for the clearcoat layer that's provided by the `KHR_materials_clearcoat` extension. This overrides the default IOR for clearcoat (which is `1.5`) and also provides a way to modulate the F0 reflectivity. This is exactly analogous to the way that the `KHR_materials_ior` and `KHR_materials_specular` extensions work together to modify F0 reflectivity.

## Extending Materials

Adding reflectivity information to the clearcoat can be done by adding the `ADOBE_materials_clearcoat_specular` extension to any glTF material that already has the `KHR_materials_clearcoat` extension defined on it.  For example, the following defines a material with a clearcoat that has an IOR of 3.0.

```json
{
    "materials": [
        {
            "name": "varnish",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0
                },
                "ADOBE_materials_clearcoat_specular": {
                    "clearcoatIor": 3.0
                }
            }
        }
    ]
}
```

### Clearcoat Reflectivity

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation) for more details on the BRDF calculations.

As with the core glTF 2.0 spec, all colored textures are assumed to be sRGB and all colored factors are assumed to be linear.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatIor**               | `number`                                                                        | The clearcoat IOR.         | No, default: `1.5`   |
|**clearcoatSpecularFactor**               | `number`                                                                        | The clearcoat specular factor.         | No, default: `1.0`   |
|**clearcoatSpecularTexture**              | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo)             | The clearcoat layer's specular amount, stored in the `B` channel of a texture. | No                   |

The clearcoat Fresnel contribution is usually calculated just as described for the base layer in [Appendix B](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation).
This extension modifies the Fresnel contribution of clearcoat as follows:
The `clearcoatIor` can be used to calculate the reflectance at normal incidence for the coating (`coatBaseF0`) that will replace the default `F0` of `0.04`.

```
coatBaseF0 = ((clearcoatIor - outside_ior) / (clearcoatIor + outside_ior))^2
```

Then, this can be used with the specular factor to determine the overall Fresnel contribution for the clear coating.

```
coatSpecularF0 = coatBaseF0 * clearcoatSpecularFactor * clearcoatSpecularTexture.b;
coatSpecularF90 = clearcoatSpecularFactor * clearcoatSpecularTexture.b;

clearcoatFresnel = coatSpecularF0 + (coatSpecularF90 - coatSpecularF0) * (1 - VdotH)^5;
```

Note that the specular factor texture data is read from the `B` channel so that the same RGB texture can be shared between the `KHR_materials_clearcoat` and `ADOBE_materials_clearcoat_specular` extensions.

## Schema

- [glTF.ADOBE_materials_clearcoat_specular.schema.json](schema/glTF.ADOBE_materials_clearcoat_specular.schema.json)
 
