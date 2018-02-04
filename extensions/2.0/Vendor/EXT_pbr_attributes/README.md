


# EXT_pbr_attributes

## Contributors

* Florian Hoenig, Unbound Technologies, inc., [@rianflo](http://twitter.com/rianflo)
* Michael Saenger, Unbound Technologies, inc., [@abductee_org](http://twitter.com/abductee_org)

## Status

Draft

## Dependencies

Written against the glTF draft 2.0 spec.

## Overview

This extension defines the metalness-roughness material model from Physically-Based Rendering (PBR) as a per vertex attribute. These additional attributes, together with the already existing albedo (COLOR_0), complete a full PBR workflow representation per vertex.

### Example
![\[Comparison\]](Figures/vertex_metal_rough_comparison.png)

Left: per-vertex albedo only. Right: per-vertex albedo attributes extended with metalness-roughness

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
    ...
    "materials": [
        {
            "name": "gold",
            "pbrMetallicRoughness": {
                "baseColorFactor": [ 1.000, 0.766, 0.336, 1.0 ],
                "metallicFactor": 1.0,
                "roughnessFactor": 0.0
            }
        }
    ],
    ...
    "meshes": [
        {
            "name": "myMesh_metallic_roughness",
            "primitives": [
                {
                    "material": 0,
                    "mode": 4,
                    "attributes": {
	                      "METALLIC_ROUGHNESS": 4,
			                  "COLOR_0": 3,
                        "NORMAL": 2,
                        "POSITION": 1
                    },
                    "indices": 0
                }
            ]
        },
        {
            "name": "myMesh_metallic_roughness_seperate",
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

    ],
    ...
} 
```

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.

### Attributes

Valid attribute semantic property names include `METALLIC_ROUGHNESS`, `METALLIC`, `ROUGHNES`.
Valid accessor type and component type for each attribute semantic property are defined below.

|Name|Accessor Type(s)|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`METALLIC_ROUGHNESS`|`"VEC2"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XY Comined PBR Parameters where *x* is PBR Metallic Material Parameter and *y* is PBR Roughness Material Parameter|
|`METALLIC`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Metallic Material Parameter|
|`ROUGHNESS`|`"SCALAR"`|`5126`&nbsp;(FLOAT)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|PBR Roughness Material Parameter|

If one or more of the attributes are present the replace the respective   "metallicFactor" and "roughnessFactor" of the pbrMetallicRoughness material.


## Known Implementations

Unbound from 0.2.7 onwards supports glTF 2.0 export with optional EXT_pbr_attributes 

## Resources

* TODO: Resources, if any.
