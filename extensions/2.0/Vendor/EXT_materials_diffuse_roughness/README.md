# EXT\_materials\_diffuse\_roughness

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/miibond)

## Status

Draft.

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a way to control the roughness of the base diffuse surface, separate from the specular roughness. This is equivalent to the [OpenPBR specification for base_diffuse_roughness](https://academysoftwarefoundation.github.io/OpenPBR/#model/basesubstrate/glossy-diffuse). Specifically, this extension changes the diffuse BRDF [defined in the glTF spec](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#dielectrics) to depend on an additional roughness parameter. This models the "flattening" effect which is often observed in real rough diffuse surfaces such as sand stone.

![EON Diffuse Roughness](meetmat.png)
The material on the right appears more rough than a simple Lambertian surface.

## Extending Materials

Adding diffuse roughness can be done by adding the `EXT_materials_diffuse_roughness` extension to any glTF material.  For example, the following defines a material with a diffuse roughness of 1.0 while maintaining a specular roughness of 0.5.

```json
{
    "materials": [
        {
            "name": "rock",
            "pbrMetallicRoughness": {
                "roughnessFactor": 0.5
            }
            "extensions": {
                "EXT_materials_diffuse_roughness": {
                    "diffuseRoughnessFactor": 1.0
                },
                
            }
        }
    ]
}
```

### Diffuse Roughness BRDF

The default diffuse BRDF of glTF defines a purely Lamertian surface. This extension essentially accounts for a rough surface's tendency to respond to light retroreflectively. That is, based on viewing angle additional This extension adds a scalar to 
Roughness blends between a simple, Lambertian diffuse and a more complex...
OpenPBR uses an energy-conserving version of Oren Nayer (link).
Using Burley diffuse (Disney 2012) is also possible and cheaper.
`diffuseRoughnessFactor` of 0.0 essentially provides Lambertian diffuse which matches default glTF 2.0 spec.

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**diffuseRoughnessFactor**               | `number`                                                                        | The diffuse roughness factor.         | No, default: `1.0`   |
|**diffuseRoughnessTexture**              | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The diffuse roughness texture to be multiplied by the factor. | No                   |

```
diffuse_brdf(color, roughness) = mix(Lambertian, EON, roughness)
```

## Schema

- [glTF.EXT_materials_diffuse_roughness.schema.json](schema/glTF.EXT_materials_diffuse_roughness.schema.json)
 
