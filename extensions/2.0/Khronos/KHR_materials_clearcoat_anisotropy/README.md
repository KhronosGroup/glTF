<!--
Copyright 2024-2025 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_materials\_clearcoat\_anisotropy

## Contributors

- Mike Bond, Adobe [@MiiBond](https://github.com/MiiBond)

Copyright 2024-2025 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.
- This extension should be used as a sub-extension of `KHR_materials_clearcoat`.

## Overview

This extension defines anisotropic properties for the clearcoat layer of a material. It allows the clearcoat layer to exhibit anisotropic behavior, where the specular reflection is stretched in one direction, similar to brushed metal or directionally polished surfaces. This extension modifies the microfacet distribution of the clearcoat layer in exactly the same way that the base layer [KHR_materials_anisotropy](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_anisotropy/README.md) does.

## Extending Materials

Anisotropy can be added to the clearcoat layer by adding the `KHR_materials_clearcoat_anisotropy` sub-extension to any compatible glTF material that also uses `KHR_materials_clearcoat`. For example, the following defines a material with anisotropic clearcoat:

```json
{
    "materials": [
        {
            "name": "brushed_metal_coated",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0,
                    "clearcoatRoughnessFactor": 0.1,
                    "extensions": {
                    "KHR_materials_clearcoat_anisotropy": {
                        "clearcoatAnisotropyStrength": 0.8,
                        "clearcoatAnisotropyRotation": 1.57
                    }
                }
            }
        }
    ]
}
```

### Clearcoat Anisotropy Parameters

The following parameters are contributed by the `KHR_materials_clearcoat_anisotropy` extension:

|                                    | Type                                                                            | Description                                  | Required             |
|------------------------------------|---------------------------------------------------------------------------------|----------------------------------------------|----------------------|
|**clearcoatAnisotropyStrength**     | `number`                                                                        | The clearcoat anisotropy strength. When the anisotropy texture is present, this value is multiplied by the texture's blue channel.      | No, default: `0.0`   |
|**clearcoatAnisotropyRotation**     | `number`                                                                        | The clearcoat anisotropy rotation in tangent, bitangent space, measured in radians counter-clockwise from the tangent. When the anisotropy texture is present, this value provides additional rotation to the vectors in the texture.          | No, default: `0.0`   |
|**clearcoatAnisotropyTexture**      | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo)             | The clearcoat anisotropy texture. Red and green channels represent the anisotropy direction in [−1,1] tangent, bitangent space to be rotated by the anisotropy rotation. The blue channel contains strength as [0,1] to be multiplied by the anisotropy strength.          | No                   |

If `clearcoatAnisotropyStrength` is zero, no anisotropic behavior is applied to the clearcoat layer.

The values for clearcoat anisotropy strength and rotation can be defined using factors, textures, or both. If the `clearcoatAnisotropyTexture` is not given, respective texture components are assumed to have default values. All clearcoat anisotropy textures contain RGB components in linear space. If both factors and textures are present, the factor value acts as a linear multiplier for the corresponding texture values.

## Anisotropy

This extension modifies the microfacet distribution of the clearcoat layer in exactly the same way that the base layer [KHR_materials_anisotropy](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_anisotropy/README.md) does. Please reference that specification for details.

## Schema

- [material.KHR_materials_clearcoat_anisotropy.schema.json](schema/material.KHR_materials_clearcoat_anisotropy.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2024-2025 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.