# KHR\_materials\_common

## Contributors

* Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
* Thomas Kress, UX3D, <mailto:kress@ux3d.io>
* Don McCurdy, Google, <mailto:donmccurdy@google.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a set of common materials for use with glTF 2.0, as
alternatives to the Physically Based Rendering (PBR) based materials provided
by the core specification. Other extensions may define additional materials.

The `KHR_materials_common` extension defines three material types: Blinn-Phong,
Lambert, and Constant.

## Properties

The following table lists the allowed types and ranges for the common materials:

|                            |Type         | Technique(s) | Description | Required |
|----------------------------|-------------|--------------|-------------|----------|
|**technique**               | `BLINNPHONG`, `LAMBERT`, `CONSTANT` | — |The shading model of the material. | Yes |
|**diffuseFactor**           | `number[4]` | `LAMBERT`, `BLINNPHONG` | The reflected diffuse RGBA factor of the material. | No, default: `[1.0,1.0,1.0,1.0]` |
|**diffuseTexture**          | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | `LAMBERT`, `BLINNPHONG` | The diffuse texture. | No |
|**specularFactor**          | `number[3]` | `BLINNPHONG` | The specular RGB color of the material. | No, default: `[1.0,1.0,1.0]` |
|**shininessFactor**         | `number`    | `BLINNPHONG` | The shininess of the material. | No, default: `1.0` |
|**specularShininessTexture**| [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | `BLINNPHONG` | The specular shininess texture. | No |

Additional properties are allowed and may lead to undefined behaviour in
conforming viewers. Emissive and occlusion values are not defined 
explicitly by this extension, but are inherited from the core material
definition.

## Techniques

For all technique computations, the following definitions are assumed:
*(all vectors normalized)*

* `diffuseTerm` – diffuseFactor * diffuseTexture
    * *Reflected diffuse color of the material.*
* `specularTerm` – specularFactor * specularShininessTexture.rgb
    * *Specular color of the material.*
* `shineTerm` – shininessFactor * specularShininessTexture.a
    * *Shininess component of the specular color.*
* `emissiveTerm` – emissiveFactor * emissiveTexture
    *  [*From core glTF material specification.*](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#additional-maps)
* `occlusionTerm` – occlusionFactor * occlusionTexture
    *  [*From core glTF material specification.*](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#additional-maps)
* `aL` – Ambient light
    * *Constant sum of all ambient light contributions from the scene.*
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `H` – Half-angle vector
    * *Calculated as halfway between the unit Eye and Light vectors, using the equation `H = normalize(I+L)`.*

### Constant

The `CONSTANT` technique defines shading that is independent of lighting, or
unlit. No extension properties are included with Constant materials, but
emissive color (from the core glTF 2.0 specification) is used.

The displayed color is calculated as:

```
color = emissiveTerm + occlusionTerm
```

> **Implementation Note**: The constant shading model is visually identical to
> Lambert or Blinn-Phong models in which all unused factors are set to zero.
> Accordingly, engines may implement only the Blinn-Phong model and use it to
> emulate simpler shading models if preferred.

<details>
    <summary>Example: Constant</summary>
The following example defines a Constant material with an emissive red color.

```json
    "materials": [
        {
            "name": "red_constant_material",
            "emissiveFactor": [ 1.0, 0.0, 0.0 ],
            "extensions": {
                "KHR_materials_cmnBlinnPhong": {
                    "technique": "CONSTANT"
                }
            }
        }
    ]
```
</details>

### Lambert

The `LAMBERT` technique defines a material shaded using Lambert shading. The
result is based on Lambert’s Law, which states that when light hits a rough
surface, the light is reflected in all directions equally. The reflected
color is calculated as:

```
color = emissiveTerm + occlusionTerm * diffuseTerm * max(N * L, 0)
```

> **Implementation Note**: The Lambert shading model is visually identical to a
> Blinn-Phong model in which all unused factors are set to zero. Accordingly,
> engines may implement only the Blinn-Phong model and use it to emulate
> simpler shading models if preferred.

<details>
    <summary>Example: Lambert</summary>
The following example defines a Lambert shaded material with a diffuse texture,
moderate shininess and red specular highlights. 

```json
    "materials": [
        {
            "name": "lambert_textured_material",
            "emissiveTexture" : {
                "index" : 0
            },
            "extensions": {
                "KHR_materials_common": {
                    "technique": "LAMBERT",
                    "diffuseTexture" : {
                        "index" : 1
                    }
                }
            }
        }
    ]
```
</details>

### Blinn-Phong

The `BLINNPHONG` technique is intended for shading according to the Blinn-Phong
lighting model or a close approximation. The underlying equation is detailed by
ACM: “Models of Light Reflection for Computer Synthesized Pictures,” SIGGRAPH
77, pp 192-198
[http://portal.acm.org/citation.cfm?id=563893](http://portal.acm.org/citation.cfm?id=563893).
In particular see the section, "Simple Highlight Model."

The following code illustrates the computation:

```
color = emissiveTerm + occlusionTerm * (diffuseTerm * max(N * L, 0) + specularTerm * max(H * N, 0)^shineTerm)
```

> **Implementation Note**: Writers should be aware of the range of the specular
> exponent (`shininess`), which is _not_ a normalized range. Concretely
> speaking, given the above equation, a `shininess` value of 1.0 corresponds to
> a very low shininess. For orientation: using the traditional OpenGL fixed
> function pipeline, the specular exponent was expected to be within [0, 128].
> However, using glTF, larger `shininess` values are clearly possible.

> **TODO:** Mention caution with `shininess < 1`?

<details>
    <summary>Example: Blinn-Phong</summary>
The following example defines a Blinn-Phong shaded material with a diffuse texture, moderate shininess and red specular highlights.

```json
    "materials": [
        {
            "name": "shiny_blinnphong_material",
            "extensions": {
                "KHR_materials_common": {
                    "technique": "BLINNPHONG",
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
</details>

## Extending Materials

Materials are defined by adding the `KHR_materials_common` extension to any
glTF material. When the extension is provided alongside a core glTF material
or another material extension, client implementations may choose the most
appropriate material for performance or other constraints.

For example, a Blinn-Phong material using all possible parameters:

```json
{
    "materials": [
        {
            "name": "complete_blinnphong_material",
            "emissiveFactor" : [
                0.2, 
                0.2, 
                0.2
            ], 
            "emissiveTexture" : {
                "index" : 0
            },
            "extensions": {
                "KHR_materials_common": {
                    "technique": "BLINNPHONG",
                    "diffuseFactor" : [
                        0.8, 
                        0.8, 
                        0.8, 
                        1.0
                    ], 
                    "diffuseTexture" : {
                        "index" : 1
                    },
                    "shininessFactor" : 2.0, 
                    "specularFactor" : [
                        0.1, 
                        0.1, 
                        0.1
                    ],
                    "specularShininessTexture" : {
                        "index" : 2
                    }, 
                }
            }

        }
    ]
}
```
