# KHR\_materials\_common

## Contributors

* Tony Parisi, WEVR, [@auradeluxe](https://twitter.com/auradeluxe), <mailto:tparisi@gmail.com>
* Robert Taglang, Cesium, [@lasalvavida](https://github.com/lasalvavida)

## Status

Draft

## Dependencies

Written against the glTF draft 2.0 spec.

## Overview

glTF uses Physically-Based Rendering (PBR) to define its materials. The renderer is usually responsible for deciding how to shade the material. This extension defines several common lighting models which can be used to force the renderer to conform to a particular shading technique.

This extension defines the material types `BLINN`, `PHONG`, `LAMBERT` and `CONSTANT`, for Blinn, Phong, Lambert and Unlit ("constant" color) materials, respectively.

## Materials

### Extending Materials

Common materials are defined by adding a `KHR_materials_common` property to the `extensions` object for the material. The extension defines a `technique` property to specify the lighting model.

For example, the following defines a material with a Lambert shader with 50% base color:

```javascript
"materials": [
    {
        "pbrMetallicRoughness" : {
            "baseColorFactor" : [0.5, 0.5, 0.5, 1.0]
        },
        "extensions" : {
            "KHR_materials_common" : {
                "technique" : 2 // lambert
            }
        }
    }
]
```

### Material Types

The `technique` property is assigned a value from the enum of material types. Table 1 lists the enum values used for each lighting model.

Table 1. Material Type Enumeration

| Material Type | Value |
|:-------------:|:-----:|
| `BLINN`       | 0     |
| `PHONG`       | 1     |
| `LAMBERT`     | 2     |
| `CONSTANT`    | 3     |

### Material Parameters

Materials shaded with this extension use the properties from the [PBR metallic roughness material](../../../specification/2.0#material) and [PBR specular glossiness extension](../KHR_materials_pbrSpecularGlossiness) to define the colors and textures needed for shading. This set of parameters will be referred to as `ambient`, `diffuse`, `emission`, `specular`, and `shininess` to match the literature on the subject. This extension defines which parameters are needed by which lighting models and how they should be computed from the material properties.

#### Ambient

__Applies To:__ `ALL`

The ambient light reflected from the surface of the object.

`material.occlusionTexture` is used as the ambient texture if it is defined. If undefined, it has a value of `[0.0, 0.0, 0.0, 1.0]`.

#### Diffuse

__Applies To:__ `BLINN`, `PHONG`, `LAMBERT`

The light diffusely reflected from the surface of the object.

`diffuse` is computed by multiplying `pbrMetallicRoughness.baseColorFactor` and `pbrMetallicRoughness.baseColorTexture`.  If the `KHR_materials_pbrSpecularGlossiness` extension is defined, the `diffuseFactor` and `diffuseTexture` properties are also multiplied into `diffuse`.

#### Emission

__Applies To:__ `ALL`

The light emitted by the surface of the object.

`emission` is computed by multiplying `emissiveFactor` and `emissiveTexture`.

#### Specular

__Applies To:__ `BLINN`, `PHONG`

The light specularly reflected from the surface of the object.

`specular` requires the `KHR_materials_pbrSpecularGlossiness` extension to be defined. It is computed by multiplying the `specularFactor` and `specularGlossinessTexture` properties. If the extension is undefined, it has a value of `[0.0, 0.0, 0.0, 1.0]`.

#### Shininess

__Applies To:__ `BLINN`, `PHONG`

`shininess` is the ratio of `pbrMetallicRoughness.metallicFactor` and `pbrMetallicRoughness.roughnessFactor`. If `roughnessFactor` is `0`, the base shininess is just `metallicFactor`. If the `KHR_materials_pbrSpecularGlossiness` extension is defined, the `glossinessFactor` should be multiplied into `shininess`.

### Lighting Model Shading

This section describes the shading for each lighting model and references the relevant literature. The `ambient`, `diffuse`, `emission`, `specular`, and `shininess` parameters referred to in this section are as described in [material parameters](#material-parameters). References to lights in this section refer to any lights in the scene as defined by the renderer or explicitly defined in the glTF using the [KHR_lights](../KHR_lights) extension.

#### Blinn

The material is shaded according to the Blinn-Torrance-Sparrow lighting model or a close approximation.

This equation is complex and detailed via the ACM, so it is not detailed here. Refer to “Models of Light
Reflection for Computer Synthesized Pictures,” SIGGRAPH 77, pp 192-198 [http://portal.acm.org/citation.cfm?id=563893](http://portal.acm.org/citation.cfm?id=563893).

The following code illustrates the basic computation:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0) + <specular> * max(H * N, 0)^<shininess * 128>
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `H` – Half-angle vector,calculated as halfway between the unit Eye and Light vectors, using the
equation H= normalize(I+L)

> **Implementation Note**: Writers should be aware that the range of the specular exponent (`shininess`), is a normalized range. Concretely speaking, given the above equation, a `shininess` value of 1 corresponds to a very low shininess. For orientation: using the traditional OpenGL fixed function pipeline, the specular exponent was expected to be within [0, 128], hence the multiplication.

#### Phong

When the value of `technique` is `PHONG`, this defines a material with Phong shading. Phong shading produces a specularly shaded surface that reflects ambient, diffuse, and specular reflection, where the specular reflection is shaded according the Phong BRDF approximation:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0) + <specular> * max(R * I, 0)^<shininess * 128>
```

where:

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `R` – Perfect reflection vector (reflect (L around N))

> **Implementation Note**: For the interpretation of the specular exponent parameter (`shininess`), see the section about the [Blinn](#blinn) lighting model.

#### Lambert

When the value of `technique` is `LAMBERT`, this defines a material shaded using Lambert shading. The result is based on Lambert’s Law, which states that when light hits a rough surface, the light is
reflected in all directions equally. The reflected color is calculated as:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0)
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector

Lambert shading does not reflect specular highlights; therefore the common material properties `specular` and `shininess` are not used.

#### Constant

When the value of `technique` is `CONSTANT`, this defines a material that describes a constantly
shaded surface that is independent of lighting.

The reflected color is calculated as:

```
color = <emission> + <ambient> * al
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.

Constant shading does not reflect light sources in the scene; therefore the common material properties `diffuse`, `specular` and `shininess` are not used.

### Properties Reference

* [`KHR_materials_common glTF extension`](#reference-khr_materials_common-gltf-extension) (root object)


---------------------------------------
<a name="reference-khr_materials_common-gltf-extension"></a>
## KHR_materials_common glTF extension

glTF extension to use common lighting models for shading.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**technique**|`integer`|Enum value defining which lighting model to use for the material.| :white_check_mark: Yes|

Additional properties are allowed.

### KHR_materials_common glTF extension.technique :white_check_mark:

Enum value defining which lighting model to use for the material.

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**:
   * `0` BLINN
   * `1` PHONG
   * `2` LAMBERT
   * `3` CONSTANT
