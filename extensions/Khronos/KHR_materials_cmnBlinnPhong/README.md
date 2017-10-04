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

This extension defines a set of common materials for use with glTF 2.0 in the
same way as the core specification defines Physically Based Rendering (PBR)
based materials. Other extensions may define additional materials.

This extension defines three material types: Blinn-Phong, Lambert, and
Constant.

## Properties

The following table lists the allowed types and ranges for the Blinn-Phong material model:

|                            |Type         | Technique(s) | Description | Required |
|----------------------------|-------------|--------------|-------------|----------|
|**technique**               | `BLINNPHONG`, `LAMBERT`, `CONSTANT` | — |The shading model of the material. | Yes |
|**diffuseFactor**           | `number[4]` | `LAMBERT`, `BLINNPHONG` | The reflected diffuse RGBA factor of the material. | No, default: `[1.0,1.0,1.0,1.0]` |
|**diffuseTexture**          | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | `LAMBERT`, `BLINNPHONG` | The diffuse texture. | No |
|**specularFactor**          | `number[3]` | `BLINNPHONG` | The specular RGB color of the material. | No, default: `[1.0,1.0,1.0]` |
|**shininessFactor**         | `number`    | `BLINNPHONG` | The shininess of the material. | No, default: `1.0` |
|**specularShininessTexture**| [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | `BLINNPHONG` | The specular shininess texture. | No |

Additional properties are allowed and may lead to undefined behaviour in
conforming viewers.

## Techniques

For all technique computations, the following definitions are assumed:
*(all vectors normalized)*

* `diffuseTerm` – diffuseFactor * diffuseTexture
* `specularTerm` – specularFactor * specularShininessTexture.rgb
* `shineTerm` – shininessFactor * specularShininessTexture.a
* `emissiveTerm` – emissiveFactor * emissiveTexture // core Materials spec
* `occlusionTerm` – occlusionFactor * occlusionTexture // core Materials spec
* `aL` – A constant amount of ambient light contribution coming from the scene,
i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `H` – Half-angle vector, calculated as halfway between the unit Eye and Light
vectors, using the equation H = normalize (I+L)

### Constant

The `CONSTANT` technique defines shading that is independent of lighting, or
unlit. No new properties are included with Constant materials, but emissive
color (from the core glTF 2.0 specification) is used.

The reflected color is calculated as:

```
color = TODO
```

> **Implementation Note**: The constant shading model is visually identical to
> Lambert or Blinn-Phong models in which all unused factors are set to zero.
> Accordingly, engines may implement only the Blinn-Phong model and use it for
> simpler shading models if preferred.

<details>
    <summary>Example: Constant</summary>
    TODO
</details>

### Lambert

The `LAMBERT` technique defines a material shaded using Lambert shading. The
result is based on Lambert’s Law, which states that when light hits a rough
surface, the light is reflected in all directions equally. The reflected
color is calculated as:

```
color = TODO
```

> **Implementation Note**: The Lambert shading model is visually identical to a
> Blinn-Phong model in which all unused factors are set to zero. Accordingly,
> engines may implement only the Blinn-Phong model and use it for simpler
> shading models if preferred.

<details>
    <summary>Example: Lambert</summary>
    TODO
</details>

### Blinn-Phong

The `BLINNPHONG` technique is intended for shading according to the Blinn-Phong lighting
model or a close approximation, and is defined by the following properties:

* `diffuse` - Reflected diffuse color of the material
* `specular` - Specular color of the material
* `shininess` - Shininess factor of the specular color

TODO: EXPLAIN FURTHER 

This underlying equation is not complex and detailed via the ACM. Please refer
to “Models of Light Reflection for Computer Synthesized Pictures,” SIGGRAPH 77,
pp 192-198 [http://portal.acm.org/citation.cfm?id=563893](http://portal.acm.org/citation.cfm?id=563893),
and in particular to the "Simple Highlight Model" section.

The following code illustrates the basic computation:

```
color = TODO
```

> **Implementation Note**: Writers should be aware of the range of the specular
> exponent (`shininess`), which is _not_ a normalized range. Concretely
> speaking, given the above equation, a `shininess` value of 1.0 corresponds to
> a very low shininess. For orientation: using the traditional OpenGL fixed
> function pipeline, the specular exponent was expected to be within [0, 128].
> However, using glTF, larger `shininess` values are clearly possible. [Mention
> caution with `shininess < 1` ?]

<details>
    <summary>Example: Blinn-Phong</summary>
The following example defines a Blinn-Phong shaded material with a diffuse texture,
moderate shininess and red specular highlights. 

```json
    "materials": [
        {
            "name": "shiny_textured",
            "extensions": {
                "KHR_materials_cmnBlinnPhong": {
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
glTF material. For example, the following defines a Blinn-Phong material having
all possible parameters:

```json
{
    "materials": [
        {
            "name": "A Blinn-Phong material",
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
