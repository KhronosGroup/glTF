<!--
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_materials\_clearcoat\_ior

## Contributors

- Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.
- Requires the `KHR_materials_clearcoat` extension to also be defined on the material.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

The clearcoat BRDF in `KHR_materials_clearcoat` uses a fixed value of 1.5 for the index of refraction (IOR). This extension allows users to override that default and set the IOR for the clearcoat layer explicitly, analogous to how `KHR_materials_ior` sets the IOR for the base layer.

## Extending Materials

The index of refraction of the clearcoat layer is configured by adding the `KHR_materials_clearcoat_ior` extension as a sub-extension to `KHR_materials_clearcoat`.

```json
{
  "materials": [
    {
      "extensions": {
        "KHR_materials_clearcoat": {
          "clearcoatFactor": 1.0,
          "extensions": {
            "KHR_materials_clearcoat_ior": {
              "clearcoatIor": 1.4
            }
          }
        }
      }
    }
  ]
}
```

| |Type|Description|Required|
|-|----|-----------|--------|
| **clearcoatIor** | `number` | The index of refraction of the clearcoat layer. | No, default: `1.5` |

Typical values for the index of refraction range from 1 to 2. In rare cases values greater than 2 are possible.

### Layering

The clearcoat effect is modeled as a microfacet BRDF layered on top of the base material using `fresnel_coat` as defined in `KHR_materials_clearcoat`. This extension updates the IOR parameter of that Fresnel term to use `clearcoatIor` instead of the default `1.5`.

```
# clearcoat
clearcoat_brdf = specular_brdf(
  normal = clearcoatNormal,
  α = clearcoatRoughness^2)

# layering
coated_material =
  fresnel_coat(
    normal = clearcoatNormal,
    ior = clearcoatIor,
    weight = clearcoat,
    base = material,
    layer = clearcoat_brdf)
```

### Implementation

This extension changes only the computation of the Fresnel term for the clearcoat layer:

```
dielectric_f0_clearcoat = ((clearcoatIor - 1)/(clearcoatIor + 1))^2
```

Note that for the default index of refraction `clearcoatIor = 1.5` this term evaluates to `0.04`.

## Schema

- [material.KHR_materials_clearcoat_ior.schema.json](schema/material.KHR_materials_clearcoat_ior.schema.json)
