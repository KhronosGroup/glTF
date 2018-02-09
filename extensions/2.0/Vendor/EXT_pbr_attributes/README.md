# EXT_pbr_attributes

## Contributors and Acknowledgements

* Florian Hoenig, Unbound Technologies, [@rianflo](http://twitter.com/rianflo)
* Michael Saenger, Unbound Technologies, [@abductee_org](http://twitter.com/abductee_org)
* Alex Evans, Mediamolecule, [@mmalex](https://twitter.com/mmalex)
* Gary Hsu, Microsoft, [@bghgary](https://twitter.com/bghgary)
* Warren Moore, metalbyexample.com, [@warrenm](https://twitter.com/warrenm)
* David Farrell, Oculus, [@Nosferalatu](https://twitter.com/Nosferalatu)
* Don McCurdy, Google, [@donrmccurdy](https://twitter.com/donrmccurdy)
* Ocean Quigley, Facebook, [@oceanquigley](https://twitter.com/oceanquigley)
* Scott Nagy, Microsoft, [@visageofscott](https://twitter.com/visageofscott)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines the metalness-roughness material model from Physically-Based Rendering (PBR) as a per vertex attribute. These additional attributes, together with the already existing albedo (COLOR_0), complete a full PBR workflow representation per vertex.

### Example
![\[Comparison\]](Figures/vertex_metal_rough_comparison.png)

Left: per-vertex albedo only. Right: per-vertex albedo attributes extended with metalness-roughness

[painted_sphere.glb](examples/painted_sphere.glb)

## glTF Schema Updates

```C
{
    "asset": {
        "generator": "Unbound/PlayEngine",
        "version": "2.0"
    },
    "scene": 0,
    "extensionsUsed": [
        "EXT_pbr_attributes"
    ],
    "materials": [
        {
            "name": "vertexPBRMat",
            "pbrMetallicRoughness": {
                "baseColorFactor": [1, 1, 1, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 1
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "baseColorAttribSpace" : "sRGB"
                }
            }
        }
    ],
    "meshes": [
        {
            "name": "myMesh_metallic_roughness",
            "primitives": [
                {
                    "material": 0,
                    "mode": 4,
                    "attributes": {
                        "ROUGHNESS": 4,
                        "METALLIC": 5,
                        "COLOR_0": 3,
                        "NORMAL": 2,
                        "POSITION": 1
                    },
                    "indices": 0
                }
            ]
        },
        {
            "name": "myMesh_roughness_only",
            "primitives": [
                {
                    "material": 0,
                    "mode": 4,
                    "attributes": {
                        "ROUGHNESS": 4,
                        "COLOR_0": 3,
                        "NORMAL": 2,
                        "POSITION": 1
                    },
                    "indices": 0
                }
            ]
        }
    ]
}
```

### JSON Schema


This extension adds on additional `enum` to the `materials` section. 

[Schema for color space selection](Schema/glTF.EXT_pbr_attributes.schema.json)

This enum provides information about the color space interpretation of the baseColor (COLOR_0) vertex attribute. If not present, `"linear"` is assumed. If color space is sRGB, the implementation is required to convert the color to linear space in order to correctly interpolate via the fixed function pipeline.

### Attributes

Valid attribute semantic property names include `METALLIC`, `ROUGHNESS`.
Valid accessor type and component type for each attribute semantic property are defined below:

|Name|Accessor Type(s)|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`METALLIC`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Metallic Material Parameter|
|`ROUGHNESS`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Roughness Material Parameter|

If one or more of the attributes are present they must replace the respective "metallicFactor" and "roughnessFactor" of the pbrMetallicRoughness material.
If a metallicRoughnessTexture texture is defined in the referenced material, an implementation must multiply the texture values with both attribute value and constant factor.

## Best Practices

The primary motivation of this extension is to allow PBR materials to be represented primarily by vertex attributes. For this use case, both "metallicFactor" and "roughnessFactor" in the material should be set to desired fallback values. Loaders should pay attention to floating point precision such that 1.0 is exactly represented.

If an exporter implementation chooses to add a metallicRoughnessTexture, the texture values take semantic precedence with regards to being linear shading parameters and the attribute values are interpreted as a factor.
Such configurations are defined for consistency and flexibility, but are not recommended.

### Caveats with regards to Core
`EXT_pbr_attributes` is generally best placed in `"extensionsUsed"`.
In case of an implementation not supporting this extension, the resulting fallback will be a vertex-colored result with constant metalness and roughness.
If sRGB was used for COLOR_0, the resulting color space and interpolation will be off.

Implementations concerned with these potentially undesirable results, maybe choose to add the extension to `"extensionsRequired"`.

## Known Implementations

* Unbound from 0.2.6 onwards supports glTF 2.0 export with optional EXT_pbr_attributes 
* [TODO: add list]

## Resources

Example File: [painted_sphere.glb](examples/painted_sphere.glb)
