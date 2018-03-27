# MOZ_alt_materials

## Contributors

* Robert Long, Mozilla

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to define alternate rendering techniques for a given material. A typical use-case for this extension would be defining a physically based material for high end platforms and an unlit material on low end platforms. It could also be used for defining rendering techniques for multiple engines.

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

### JSON Schema

* **JSON schema**: [material.MOZ_alt_materials.schema.json](schema/material.MOZ_alt_materials.schema.json)

## Known Implementations

None
