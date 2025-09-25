# EXT\_materials\_anisotropy\_openpbr

## Contributors

* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.
* Requires the `KHR_materials_anisotropy` extension to also be defined on the material

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a way to interpret the `anisotropyStrength` parameter from `KHR_materials_anisotropy` using OpenPBR conventions instead of the default glTF interpretation. This allows materials to achieve consistent appearance when authoring content for OpenPBR-compliant renderers while maintaining compatibility with existing glTF workflows.

## Extending Materials

OpenPBR anisotropy interpretation can be enabled by adding the `EXT_materials_anisotropy_openpbr` extension as a sub-extension of `KHR_materials_anisotropy` on a glTF material. For example:

```json
{
    "materials": [
        {
            "name": "brushed_metal",
            "extensions": {
                "KHR_materials_anisotropy": {
                    "anisotropyStrength": 0.6,
                    "anisotropyRotation": 0.0,
                    "extensions": {
                        "EXT_materials_anisotropy_openpbr": {
                            "openPbrAnisotropyEnabled": true
                        }
                    }
                }
            }
        }
    ]
}
```

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**openPbrAnisotropyEnabled**    | `boolean`                                                                                       | Enables OpenPBR interpretation of anisotropy strength.  | No, default: `false`   |

When `openPbrAnisotropyEnabled` is `false` or not present, the standard glTF anisotropy calculations should be used as defined in `KHR_materials_anisotropy`.

When `openPbrAnisotropyEnabled` is `true`, implementations should interpret the anisotropic strength using the OpenPBR conventions described below.

## Anisotropic Strength Behavior

In both glTF and OpenPBR, the strength of the anisotropic effect in the tangent and bitangent direction is calculated using the material's specular roughness as well as the anisotropic strength parameter from the `KHR_materials_anisotropy` extension. It is these calculations that differ between glTF and OpenPBR. The rotation and texture packing remains the same.

In glTF: \
$alphaTangent = mix( alphaRoughness, 1.0, anisotropyStrength^2 )$ \
$alphaBiTangent = alphaRoughness$

In OpenPBR: \
$alphaTangent = alphaRoughness \sqrt{2 / (1+ (1-anisotropyStrength)^2)}$ \
$alphaBiTangent = alphaTangent(1 - anisotropyStrength)$


When `openPbrAnisotropyEnabled` is `true`, the anisotropic strength  from `KHR_materials_anisotropy` is applied according to the OpenPBR conventions to obtain the tangent and bitangent roughness values.
