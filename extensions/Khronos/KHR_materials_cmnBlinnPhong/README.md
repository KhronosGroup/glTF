# KHR\_materials\_cmnBlinnPhong

## Contributors

* Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
* Thomas Kress, UX3D, <mailto:kress@ux3d.io>
* many others

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a Blinn-Phong model based material for use with glTF 2.0 in the same way as the core specification defines Physically Based Rendering (PBR) based materials. Other extension may define additional commonly used materials. For example, if a need for a Phong model arises, an extension named e.g. `cmnPhong` could be specified. 

Given the long history of the Blinn-Phong model and the vast amount of collective experience with applying this model, the demand for convenient use of such materials still exists.

This extension defines a single material type, Blinn-Phong. 

## Extending Materials

The common Blinn-Phong material is defined by adding the `KHR_materials_cmnBlinnPhong` extension to any glTF material. 
For example, the following defines a material having all possible parameters.:

```
{
    "materials": [
        {
            "name": "All parameters",
            "extensions": {
                "KHR_materials_cmnBlinnPhong": {
                    "diffuseFactor" : [
                        0.8, 
                        0.8, 
                        0.8, 
                        1.0
                    ], 
                    "diffuseTexture" : {
                        "index" : 0
                    }, 
                    "shininessFactor" : 2.0, 
                    "specularFactor" : [
                        0.1, 
                        0.1, 
                        0.1
                    ],
                    "specularShininessTexture" : {
                        "index" : 1
                    }, 
                }
            }

        }
    ]
}
```
### Blinn - Phong

The Blinn-Phong material model is defined by the following properties:
* `diffuse` - Reflected diffuse color of the material
* `specular` - Specular color of the material
* `shininess` - Shininess factor of the specular color

TODO: EXPLAIN FURTHER 

The following table lists the allowed types and ranges for the Blinn-Phong material model:

|                            |Type         |Description|Required|
|----------------------------|-------------|-----------|--------|
|**diffuseFactor**           | `number[4]` | The reflected diffuse RGBA factor of the material.|No, default: `[1.0,1.0,1.0,1.0]`|
|**diffuseTexture**          | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The diffuse texture.|No|
|**specularFactor**          | `number[3]` | The specular RGB color of the material.      |No, default: `[1.0,1.0,1.0]`|
|**shininessFactor**         | `number`    | The shininess of the material.|No, default: `1.0`          |
|**specularShininessTexture**| [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)|The specular shininess texture.|No|

Additional properties are allowed and may lead to undefined behaviour in conforming viewers.

The material is intended for shading according to the Blinn-Phong lighting model or a close approximation.

This underlying equation is not complex and detailed via the ACM. Please refer to “Models of Light
Reflection for Computer Synthesized Pictures,” SIGGRAPH 77, pp 192-198 [http://portal.acm.org/citation.cfm?id=563893](http://portal.acm.org/citation.cfm?id=563893), and in particular to the "Simple Highlight Model" section.

The following code illustrates the basic computation:

```
color = emissiveTerm + occlusionTerm * (diffuseTerm * max(N * L, 0) + specularTerm * max(H * N, 0)^shineTerm)
```

where (all vectors normalized)

* `diffuseTerm` – diffuseFactor * diffuseTexture
* `specularTerm` – specularFactor * specularShininessTexture.rgb
* `shineTerm` – shininessFactor * specularShininessTexture.a
* `emissiveTerm` – emissiveFactor * emissiveTexture // core Materials spec
* `occlusionTerm` – occlusionFactor * occlusionTexture // core Materials spec
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `H` – Half-angle vector, calculated as halfway between the unit Eye and Light vectors, using the
equation H = normalize (I+L)

> **Implementation Note**: Writers should be aware of the range of the specular exponent (`shininess`), which is _not_ a normalized range. Concretely speaking, given the above equation, a `shininess` value of 1.0 corresponds to a very low shininess. For orientation: using the traditional OpenGL fixed function pipeline, the specular exponent was expected to be within [0, 128]. However, using glTF, larger `shininess` values are clearly possible. [Mention caution with `shininess < 1` ?]

The following example defines a Blinn shaded material with a diffuse texture, moderate shininess and red specular highlights. 

```javascript
    "materials": [
        {
            "name": "shiny_textured",
            "extensions": {
                "KHR_materials_cmnBlinnPhong": {
                    "diffuseFactor" : [
                        0.8, 
                        0.8, 
                        0.8, 
                        1.0
                    ], 
                    "diffuseTexture" : {
                        "index" : 0
                    }, 
                    "shininessFactor" : 10.0, 
                    "specularFactor" : [
                        1, 
                        0, 
                        0
                    ]
                }
            }

        }
    ]

```
