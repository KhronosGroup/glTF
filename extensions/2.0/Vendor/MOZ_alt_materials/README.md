# MOZ_alt_materials

## Contributors

* Robert Long, Mozilla

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to define alternate rendering techniques for a given material.

A typical use-case for this extension would be extending a material that uses `pbrMetallicRoughness` to also support `KHR_materials_unlit`. Implementors may choose to support this extension to allow for progressive enhancement of lighting techniques on different platforms. Alternate materials are still expected to have reasonable fallback support to their own `pbrMetallicRoughness` values, however they are not required to approximate the original material. For example a model may have a metallic `pbrMetallicRoughness` material and a flat `KHR_materials_unlit` alternate model. In this case a desktop user could be provided the PBR material and a mobile user, the unlit material.

To minimize the cost of downloading the glTF asset, avoid packing all texture maps into a binary glTF. Loaders implementing this extension should conditionally download the texture maps for the specified rendering technique.

This extension should only be added to metallic roughness PBR materials. This PBR material will be used as the fallback material if the loader does not support this extension.

An example of defining an alternate unlit material using the `KHR_materials_unlit` extension:

```json
"materials": [
    {
        "pbrMetallicRoughness": {
            "baseColorTexture": {
                "index": 0
            },
            "metallicFactor": 1,
            "roughnessFactor": 1,
        },
        "normalTexture": {
            "index": 1
        },
        "extensions": {
            "MOZ_alt_materials": {
                "KHR_materials_unlit": 1
            }
        }
    },
    {
        "pbrMetallicRoughness": {
            "baseColorFactor": [ 1.0, 1.0, 1.0, 1.0 ],
            "baseColorTexture": {
                "index": 2
            },
            "roughnessFactor": 0.9,
            "metallicFactor": 0.0
        },
        "extensions": {
            "KHR_materials_unlit": {}
        }
    }
]
```

## glTF Schema Updates

If a `material` contains an `extension` property and the `extension` defines its `MOZ_alt_materials` property then any of the materials, whose indices are defined as the values of the `MOZ_alt_materials` object, may be used instead of the `material`.

The keys of the `MOZ_alt_materials` may include `KHR_materials_unlit`, `KHR_materials_pbrSpecularGlossiness` as well as any current or future material extension names or creator defined semantic variables (ex. "TOON_SHADING" or "MAT_CAP" which may point at a `KHR_techniques_webgl` material).

### JSON Schema

* **JSON schema**: [material.MOZ_alt_materials.schema.json](schema/material.MOZ_alt_materials.schema.json)

## Known Implementations

None
