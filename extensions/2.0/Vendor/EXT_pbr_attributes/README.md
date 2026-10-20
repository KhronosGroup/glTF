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

TODO: 
* Update sample file
* Update schema file

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines the metalness-roughness material model from Physically-Based Rendering (PBR) per vertex attribute.
These additional attribute semantics referenced in the material definition.

### Example
![\[Comparison\]](Figures/vertex_metal_rough_comparison.png)

Left: per-vertex albedo only. Right: per-vertex albedo attributes extended with metalness-roughness

[painted_sphere.glb](examples/painted_sphere.glb)

## glTF Schema Updates

### Examples 
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
            "name": "material_basic_def",
            "pbrMetallicRoughness": {
                "baseColorFactor": [1, 1, 1, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 0
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "roughnessAttrib" : "_ROUGHNESS",
                    "metallicAttrib"  : "_METALLIC",
                }
            }
        },
        {
            "name": "material_0",
            "pbrMetallicRoughness": {
                "baseColorFactor": [1, 1, 1, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 0
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "baseColorSpace"  : "sRGB",  
                    "baseColorAttrib" : "_ALBEDO",
                    "roughnessAttrib" : "_ROUGHNESS",
                    "metallicAttrib"  : "_METALLIC",
                }
            }
        },
        {
            "name": "material_1",
            "pbrMetallicRoughness": {
                "baseColorFactor": [0.8, 0.8, 0.8, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 0.8
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "baseColorSpace"  : "sRGB",
                    "baseColorAttrib" : "_WORN_ALBEDO",
                    "roughnessAttrib" : "_WORN_ROUGHNESS",
                    "metallicAttrib"  : "_METALLIC",
                }
            }
        },
        {
            "name": "material_canonical",
            "pbrMetallicRoughness": {
                "baseColorFactor": [0.8, 0.8, 0.8, 1],
                "metallicFactor" : 1,
                "roughnessFactor": 0.8
            },
            "extensions" : {
                "EXT_pbr_attributes" : {
                    "baseColorSpace"  : "sRGB",
                    "baseColorAttrib" : "_ALBEDO",
                    "roughnessAttrib" : "_ROUGHNESS",
                    "metallicAttrib"  : "_METALLIC",
                    "occulsionAttrib" : "_OCCLUSION",
                    "emissiveAttrib"  : "_EMISSIVE"
                }
            }
        }       

    ],
    "meshes": [
        {
            "name": "asset_0",
            "primitives": [
                {
                    "material": 0,
                    "mode": 4,
                    "attributes": {
                        "_METALLIC": 5,
                        "_ROUGHNESS": 4,
                        "COLOR_0": 3,
                        "NORMAL": 2,
                        "POSITION": 1
                    },
                    "indices": 0
                }
            ]
        },
        {
            "name": "asset_1",
            "primitives": [
                {
                    "material": 1,
                    "mode": 4,
                    "attributes": {
                        "_ALBEDO": 8,
                        "_WORN_ALBEDO": 7,
                        "_WORN_ROUGHNESS": 6,
                        "_METALLIC": 5,
                        "_ROUGHNESS": 4,
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

If one or more of the attributes are referenced by the material, they must _replace_ the respective factor. For example, "roughnessAttrib" overrides "roughnessFactor" of the pbrMetallicRoughness material.

If a relevant texture (for example, metallicRoughnessTexture) is defined in the material, an implementation must multiply the texture values with both attribute value and constant factor.

### Attributes

Valid attribute semantic property names can be defined in the material.
`COLOR_0` if present, will be the default baseColor (albedo) but can be overridden in the material.
Valid accessor type and component type for each attribute semantic property are defined below:
                    "baseColorAttrib" : "_ALBEDO",
                    "roughnessAttrib" : "_ROUGHNESS",
                    "metallicAttrib"  : "_METALLIC",
                    "occulsionAttrib" : "_OCCLUSION",
                    "emissiveAttrib"  : "_EMISSIVE"

|Name|Accessor Type(s)|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|value of(`metallicAttrib`)|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Metallic Material Parameter|
|value of(`roughnessAttrib`)|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Roughness Material Parameter|
|value of(`occulsionAttrib`)|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|Ambient Occlusion Factor|
|value of(`emissiveAttrib`)|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|Emissive color|
|value of(`baseColorAttrib`)|SEE COLOR_0|SEE COLOR_0|SEE COLOR_0|
 

## Best Practices

The primary motivation of this extension is to allow PBR materials to be represented per-vertex.
For this use case, it is recommended to set both "metallicFactor" and "roughnessFactor" to fallback values (for example: the average over all per-vertex values).

If an exporter implementation chooses to add a metallicRoughnessTexture, the texture values take semantic precedence with regards to being linear shading parameters and the attribute values are interpreted as a factor.
Such configurations are defined for consistency and flexibility, but are not recommended.

### Caveats with regards to Core
`EXT_pbr_attributes` is best placed in `"extensionsUsed"`.
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
