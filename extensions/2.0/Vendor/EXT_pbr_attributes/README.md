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
* Milan Bulat, Foundry, [@speedy_milan](https://twitter.com/speedy_milan)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension improves the definition of metalness-roughness material model from Physically-Based Rendering (PBR), by adding additional PBR material parameters, which reference per vertex attributes. These additional material parameters, allow for a full per vertex PBR workflow representation.

Propsed material property to mesh attribute referencing allows DCC applications to save multiple materials for a single mesh into a glTF file, which then allows real-time engines to switch / blend between them at run-time. It also allows DCC applications to use glTF as a storage format, used for saving and loading artist defined shading configurations, while minimizing data loss and/or shading configuration change / conversion over multiple save and load cycle(s).

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
            "name": "game_asset_brand_new",
            "pbrMetallicRoughness": {
                "baseColorFactor": [1, 1, 1, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 0
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "attribSpace" : "sRGB",
                    "baseColorAttrib" : "_BASECOLOR_BRAND_NEW",
                    "roughnessAttrib" : "_ROUGHNESS_BRAND_NEW",
                    "metallicAttrib" : "METALLIC",
                }
            }
        },
        {
            "name": "game_asset_worn_out",
            "pbrMetallicRoughness": {
                "baseColorFactor": [0.8, 0.8, 0.8, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 0.8
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "attribSpace" : "sRGB",
                    "baseColorAttrib" : "_BASECOLOR_WORN_OUT",
                    "roughnessAttrib" : "_ROUGHNESS_WORN_OUT",
                    "metallicAttrib" : "METALLIC",
                }
            }
        }       
    ],
    "meshes": [
        {
            "name": "game_asset_1",
            "primitives": [
                {
                    "material": 0,
                    "mode": 4,
                    "attributes": {
                        "_BASECOLOR_WORN_OUT": 7,
                        "_ROUGHNESS_WORN_OUT": 6,
                        "METALLIC": 5,
                        "_ROUGHNESS_BRAND_NEW": 4,
                        "_BASECOLOR_BRAND_NEW": 3,
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

This enum provides information about the color space interpretation of the  vertex attributes used. If not present, `"linear"` is assumed. If color space is "sRGB", the implementation is required to convert the color to linear space in order to correctly interpolate via the fixed function pipeline.

If one or more of the attributes are referenced by the material, they must override the respective factor, for example, "roughnessAttrib" overrides "roughnessFactor" of the pbrMetallicRoughness material.

If a relevant texture (for example, metallicRoughnessTexture) is defined in the material, an implementation must multiply the texture values with both attribute value and constant factor.

### Attributes

Valid attribute semantic property names include `METALLIC`, `ROUGHNESS`.
Valid accessor type and component type for each attribute semantic property are defined below:

|Name|Accessor Type(s)|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`METALLIC`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Metallic Material Parameter|
|`ROUGHNESS`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Roughness Material Parameter|
 
This extension allows user defined attributes (prefixed with "_") to be used instead of the default ones. User defined attributes need to adhere to the  same type limitations as specified in the above table.

## Best Practices

The primary motivation of this extension is to allow PBR materials to be represented by additional vertex based material parameters.
For this use case, it is recommended to set both "metallicFactor" and "roughnessFactor" to fallback values.

If an exporter implementation chooses to add a metallicRoughnessTexture, the texture values take semantic precedence with regards to being linear shading parameters and the attribute values are interpreted as a factor.
Such configurations are defined for consistency and flexibility, but are not recommended.

### Caveats with regards to Core
`EXT_pbr_attributes` is generally best placed in `"extensionsUsed"`.
In case of an implementation not supporting this extension, the resulting fallback will be a vertex-colored result with constant metalness and roughness.
If sRGB was used for COLOR_0, the resulting color space and interpolation will be off.

Implementations concerned with these potentially undesirable results, maybe choose to add the extension to `"extensionsRequired"`.

For materials which have this extension, to be able to provide for clean separation between meshes and materials, the following two considerations apply:

When COLOR_0 attribute exists in the mesh, and baseColorAttrib is defined, baseColorAttrib has the priority, and automatic usage of COLOR_0 attribute (as per Core specification) is ignored. 

When COLOR_0 attribute exists in the mesh, and baseColorAttrib is not defined, 
COLOR_0 is again ignored (not automatically applied as per Core specification).

## Known Implementations

* Unbound from 0.2.6 onwards supports glTF 2.0 export with optional EXT_pbr_attributes 
* [TODO: add list]

## Resources

Example File: [painted_sphere.glb](examples/painted_sphere.glb)
