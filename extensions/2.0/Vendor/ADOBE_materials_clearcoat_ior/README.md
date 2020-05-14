# ADOBE\_materials\_clearcoat\_ior

## Contributors

* Mike Bond, Adobe, [@miibond](https://twitter.com/miibond)

## Status

Draft. The format of the IOR texture will depend on the final format chosen for KHR_materials_ior

## Dependencies

Written against the glTF 2.0 spec.
* Requires the `KHR_materials_clearcoat` extension to also be defined on the material

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a way to control the index of refraction (IOR) for the clearcoat layer that's provided by the `KHR_materials_clearcoat` extension. This effectively overrides the default IOR for clearcoat, which is `1.5`.

## Extending Materials

Adding IOR to the clearcoat can be done by adding the `ADOBE_materials_clearcoat_ior` extension to any glTF material that already has the `KHR_materials_clearcoat` extension defined on it.  For example, the following defines a material with a clearcoat that has an IOR of 3.0.

```json
{
    "materials": [
        {
            "name": "varnish",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0
                },
                "ADOBE_materials_clearcoat_ior": {
                    "clearcoatIorFactor": 3.0
                }
            }
        }
    ]
}
```

### Clearcoat IOR

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

As with the core glTF 2.0 spec, all coloured textures are assumed to be sRGB and all coloured factors are assumed to be linear.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatIorFactor**               | `number[4]`                                                                        | The clearcoat IOR factor.         | No, default: `[1.0, 1.0, 1.0, 1.0]`   |
|**clearcoatIorTexture**              | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The clearcoat layer's IOR texture. | No                   |
  


## Schema

- [glTF.ADOBE_materials_clearcoat_ior.schema.json](schema/glTF.KHR_materials_clearcoat.schema.json)
 
