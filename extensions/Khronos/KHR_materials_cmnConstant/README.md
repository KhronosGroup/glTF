# KHR\_materials\_cmnConstant

## Contributors

* Don McCurdy, Google, <mailto:donmccurdy@google.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines an unlit ("constant") model based material for use with glTF 2.0 in the same way as the core specification defines Physically Based Rendering (PBR) based materials. Other extension may define additional commonly used materials. 

In mobile devices with limited resources, unlit materials remain a performant alternative to higher-quality shading models.

This extension defines a single material type, Constant.

## Extending Materials

The common Constant material is defined by adding the `KHR_materials_cmnConstant` extension to any glTF material. 
For example, the following defines a material having all possible parameters:

```
{
    "materials": [
        {
            "name": "CustomEmissive",
            "extensions": {
                "KHR_materials_cmnConstant": {
                    "emissionFactor" : [
                        0.8, 
                        0.8, 
                        0.8, 
                        1.0
                    ],
                    "emissionTexture" : {
                        "index" : 0
                    }
                }
            }

        }
    ]
}
```

### Constant

The Constant material model describes a constantly shaded surface that is independent of lighting. The material is defined by the following properties:

* `emission` - Emitted color of the material

The following table lists the allowed types and ranges for the constant model:

|                    |Type         |Description|Required|
|--------------------|-------------|-----------|--------|
|**emissionFactor**  | `number[4]` | The emissive RGBA factor of the material. | No, default: `[1.0,1.0,1.0,1.0]` |
|**emissionTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The emissive texture. | No |

Additional properties are allowed and may lead to undefined behaviour in conforming viewers.

The material is intended for unlit shading. Color is calculated as:

```
color = <emission> + <ambient> * al
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
