# KHR\_materials\_common

## Contributors

* Tony Parisi, WEVR, [@auradeluxe](https://twitter.com/auradeluxe), <mailto:tparisi@gmail.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 1.0 spec.

## Overview

This extension defines a set of common materials and lights for use with glTF.

glTF does not define a lighting model, instead relegating shading of visuals to GLSL shader code defined within techniques in the glTF file. For many use cases, it is desirable to shade visuals with commonly understood and implemented shading techniques, such as Blinn and Lambert.

Many 3D tools and WebGL runtimes support built-in implementations of common material and light types. Using this extension, tools can export a much more compact representation of materials and lights. In addition, a runtimes with built-in lighting can apply its built-in light values to visuals shaded with common materials found in the glTF file.

This extension defines four light types: `directional`, `point`, `spot`, and `ambient`; and the material types `BLINN`, `PHONG`, `LAMBERT`, and `CONSTANT`, for Blinn, Phong, Lambert, and Unlit ("constant" color) materials, respectively.

The [conformance](#conformance) section specifies what an implementation must to do when encountering this extension, and how the extension interacts with the materials defined in the base specification.

## Materials

### Extending Materials
Common materials are defined by adding an `extensions` property to any glTF material, and defining its `KHR_materials_common` property. Each material usually has at least two properties: `technique`, which specifies the shading technique used e.g. `BLINN`, and `values` which contains a set of technique-specific values. Extension object can have also several other properties to assist runtime and set proper render states (for detailed explanation, see reference below).

For example, the following defines a double-sided material with a Lambert shader with 50% gray diffuse color:

```json
{
    "materials": {
        "lambert1": {
            "extensions": {
                "KHR_materials_common": {
                    "doubleSided": true,
                    "transparent": false,
                    "technique": "LAMBERT",
                    "values": {
                        "diffuseFactor": [0.5, 0.5, 0.5, 1]
                    }
                }
            }
        }
    }
}
```

#### Table 1. Material Extension Properties
| Property      | JSON Type | Description | Required |
|:-------------:|:---------:|:------------|:--------|
| `doubleSided` | `boolean` | Declares whether backface culling should be disabled for this visual. Corresponds to disabling the `CULL_FACE` render state. | No, default: `false` |
| `jointCount`  | `integer` | Declares maximum number of joints used by meshes which use this material. This allows to pre-generate vertex shader skinning code before mesh loading. | No, default: `0` |
| `technique`   | `string`  | Declares used material model. Valid values are `BLINN`, `PHONG`, `LAMBERT`, and `CONSTANT`. | :white_check_mark: Yes |
| `transparent` | `boolean` | Declares whether the visual should be rendered using alpha blending. Corresponds to enabling the BLEND render state, setting the depthMask property to false, and defining blend equations and blend functions as described in the implementation note. | No, default: `false` |
| `values` | `object` | Contains technique-specific properties. See **Table 2** for details.| No, default: `{}` |

> **Implementation Note**: An implementation should render alpha-blended visuals in depth-sorted order. An implementation should define blend equations and blend functions that result in properly blended RGB and alpha values, such as in the following example:
```javascript
gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_ADD);
gl.blendFuncSeparate(gl.ONE, gl.ONE_MINUS_SRC_ALPHA, gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
```

### Material Types

#### Common Material Shared Properties
Following table lists properties that can appear in the `values` property of a common material, and that are shared among several of the common material types.

##### Table 2. Common Material Shared Properties
| Property          | JSON Type   | Description | Default Value | Applies To |
|:-----------------:|:-----------:|:-----------|:-------------:|:----------:|
| `ambientFactor`   | `number[3]` | Per-component factors to apply to values of `ambientTexture`. | `[0, 0, 0]` | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `ambientTexture`  | `string`    | ID of RGB texture defining the amount of ambient light reflected from the surface of the object.| _See below_ | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `diffuseFactor`   | `number[4]` | Per-component factors to apply to values of `diffuseTexture`. | `[0, 0, 0, 1]` | `BLINN`, `PHONG`, `LAMBERT` |
| `diffuseTexture`  | `string`    | ID of RGBA texture defining the amount of light diffusely reflected from the surface of the object. | _See below_ | `BLINN`, `PHONG`, `LAMBERT` |
| `emissionFactor`  | `number[3]` | Per-component factors to apply to values of `emissionTexture`. | `[0, 0, 0]` | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `emissionTexture` | `string`    | ID of RGB texture for defining the amount of light emitted by the surface of the object. | _See below_ | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `specularFactor`  | `number[3]` | Per-component factors to apply to values of `specularTexture`. | `[0, 0, 0]` | `BLINN`, `PHONG` |
| `specularTexture` | `string`    | ID of RGB texture defining the amount of light specularly reflected from the surface of the object. | _See below_ | `BLINN`, `PHONG` |
| `shininess`       | `number`    | Specularity or roughness of the specular reflection lobe of the object. | `0.0` | `BLINN`, `PHONG` |
| `transparency`    | `number`    | Amount of transparency as an opacity value between 0.0 and 1.0. | `1.0` | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |

If a texture is not given, all respective texture components are assumed to have a value of `1.0`.

#### Constant
When the value of `technique` is `CONSTANT`, this defines a material that describes a constantly shaded surface that is independent of lighting (except ambient).

The reflected color is calculated as:

```
color = <emission> + <ambient> * al
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.

Constant shading does not reflect light sources in the scene; therefore the common material properties `diffuseFactor`, `diffuseTexture`, `specularFactor`, `specularTexture`, and `shininess` are not used. All other properties from **Table 2** apply.

The following example defines a Constant-lit material with an emissive texture and 50% opacity.

```json
{
    "materials": {
        "constant1": {
            "extensions": {
                "KHR_materials_common": {
                    "technique": "CONSTANT",
                    "values": {
                        "emissionFactor": [1, 1, 1],
                        "emissionTexture": "texture_2",
                        "transparency": 0.5
                    }
                }
            }
        }
    }
}
```

#### Lambert
When the value of `technique` is `LAMBERT`, this defines a material shaded using Lambert shading. The result is based on Lambert’s Law, which states that when light hits a rough surface, the light is reflected in all directions equally. The reflected color is calculated as:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0)
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector

Lambert shading does not reflect specular highlights; therefore the common material properties `specularFactor`, `specularTexture`, and `shininess` are not used. All other properties from **Table 2** apply.

The following example defines a Lambert-shaded material with a 50% gray emissive color and a diffuse texture map.

```json
{
    "materials": {
        "lambert1": {
            "extensions": {
                "KHR_materials_common": {
                    "technique": "LAMBERT",
                    "values": {
                        "diffuseFactor": [1, 1, 1, 1],
                        "diffuseTexture": "texture_1",
                        "emissionFactor": [0.5, 0.5, 0.5, 1]
                    }
                }
            }
        }
    }
}
```

#### Phong
When the value of `technique` is `PHONG`, this defines a material with Phong shading. Phong shading produces a specularly shaded surface that reflects ambient, diffuse, and specular reflection, where the specular reflection is shaded according the Phong BRDF approximation:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0) + <specular> * max(R * I, 0)^<shininess>
```

where:

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `R` – Perfect reflection vector (reflect (L around N))

> **Implementation Note**: For the interpretation of the specular exponent parameter (`shininess`), see the section about the [Blinn](#blinn) lighting model.

Phong lighting uses all of the common material properties defined in **Table 2**. The following example defines a Phong-lit material with a yellow diffuse color.

```json
{
    "materials": {
        "phong1": {
            "extensions": {
                "KHR_materials_common": {
                    "technique": "PHONG",
                    "values": {
                        "diffuseFactor": [1, 1, 0, 1]
                    }
                }
            }
        }
    }
}
```

#### Blinn
When the value of `technique` is `BLINN`, this defines a material shaded according to the Blinn-Torrance-Sparrow lighting model or a close approximation.

This equation is complex and detailed via the ACM, so it is not detailed here. Refer to [“Models of Light Reflection for Computer Synthesized Pictures” SIGGRAPH 77, pp 192-198](http://portal.acm.org/citation.cfm?id=563893).

The following code illustrates the basic computation:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0) + <specular> * max(H * N, 0)^<shininess>
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `H` – Half-angle vector, calculated as halfway between the unit Eye and Light vectors, using the equation `H = normalize(I+L)`

> **Implementation Note**: Writers should be aware about the range of the specular exponent (`shininess`), which is _not_ a normalized range. Concretely speaking, given the above equation, a `shininess` value of 1 corresponds to a very low shininess. For orientation: using the traditional OpenGL fixed function pipeline, the specular exponent was expected to be within `[0, 128]`. However, using glTF, larger `shininess` values are clearly possible.

Blinn shading uses all of the common material properties defined in **Table 2**. The following example defines a Blinn shaded material with a diffuse texture, moderate shininess and red specular highlights.

```json
{
    "materials": {
        "blinn1": {
            "extensions": {
                "KHR_materials_common": {
                    "technique": "BLINN",
                    "values": {
                        "diffuseTexture": "texture_1",
                        "shininess": 10,
                        "specularFactor": [1, 0, 0]
                    }
                }
            }
        }
    }
}
```

### Interaction Between Attribute Semantics and Common Materials
The base specification defines attribute semantics for mesh primitives. The common materials in this extension shall support the semantics `POSITION`, `NORMAL`, `TEXCOORD`, `COLOR`, `JOINT`, and `WEIGHT`. For array semantics such as texture coordinates, the implementation is only required to support the 0th set of coordinates i.e. `TEXCOORD_0`.

Since semantics are usually defined via technique parameters, they are not present in the glTF scene description when `KHR_materials_common` is used.
Therefore, writers using this extension must ensure that the attributes of the mesh are named exactly like the semantics:

```json
{
    "meshes": {
        "mesh0": {
            "primitives": [
                {
                    "attributes": {
                        "NORMAL": "accessor23",
                        "POSITION": "accessor42"
                    },
                    "indices": "accessor13",
                    "material": "material7",
                    "mode": 4
                }
            ]
        }
    }
}
```

Moreover, referenced accessors must have exact `type` for each semantic (see **Table 3** below).

##### Table 3. Attribute types
| Semantic name | `accessor.type` | Required |
| ------------- | :-------------: | :---- |
| POSITION      | `"VEC3"`        | Yes, for all material types. |
| NORMAL        | `"VEC3"`        | Yes, for all material types except `CONSTANT`. |
| TEXCOORD_0    | `"VEC2"`        | No, unless textures are used. |
| JOINT         | `"VEC4"`        | No, unless mesh is skinned. |
| WEIGHT        | `"VEC4"`        | No, unless mesh is skinned. |


If a conforming implementation of this extension supports skinned animations, then the common materials described in this extension must also support hardware skinning in its vertex and fragment shader programs.

<a name="lights"></a>
## Lights
Lights define light sources in the scene.

Lights are contained in nodes and thus can be transformed. Their world-space positions can be used in shader calculations.

A conforming implementation of this extension must be able to load light data defined in the asset, but it is not obligated to render the asset using those lights.

### Defining Lights
Lights are defined within a dictionary property in the file, by adding an `extensions` property to the top-level glTF object and defining its `KHR_materials_common` property with a `lights` dictionary object inside it.

Each light defines a `type` property that designates the type of light (`ambient`, `directional`, `point`, or `spot`); a property of that same name defines the details, such as color, attenuation and other light type-specific values. The following example defines a white-colored directional light.


```json
{
    "extensions": {
        "KHR_materials_common": {
            "lights": {
                "light1": {
                    "type": "directional",
                    "directional": {
                        "color": [1, 1, 1]
                    }
                }
            }
        }
    }
}
```

#### Table 4. `KHR_materials_common.light` Properties
| Property      | JSON Type | Description | Required |
|:-------------:|:---------:|:------------|:---------|
| `type`        | `string`            | Type of the light. Must be one of `ambient`, `directional`, `point`, or `spot`. | :white_check_mark: Yes |
| `ambient`     | `light.ambient`     | Ambient light specific properties. See **Table 5** for details. | No |
| `directional` | `light.directional` | Directional light specific properties. See **Table 5** for details. | No |
| `point`       | `light.point`       | Point light specific properties. See **Table 5** for details. | No |
| `spot`        | `light.spot`        | Spot light specific properties. See **Table 5** for details.| No |


IDs of any property within the `lights` extension property are treated like other glTF IDs, i.e. they are added to the global ID space.

### Adding Light Instances to Nodes
Lights are glTF scene objects: they have position and orientation, which can be animated. Lights are placed in the scene by defining an `extensions` property of a node, and within that a `KHR_materials_common` property that identifies a light using its `light` property.

```json
{
    "nodes": {
        "node1": {
            "rotation": [ 1.0, 0.0, 0.0, 0.0 ],
            "extensions": {
                "KHR_materials_common": {
                    "light": "light1"
                }
            }
        }
    }
}
```

For light types that have a direction (directional and spot lights), the light's direction is defined as the vector (0, 0, -1) and the rotation of the node orients the light accordingly.


### Light Types

#### Common Light Shared Properties
Table 5 lists properties that are shared among common light types.

##### Table 5. Common Light Shared Properties
| Property               | JSON Type    | Description  | Default Value | Applies To |
|:----------------------:|:------------:|:------------:|:-------------:|:----------:|
| `color`                | `number[3]`  | RGB value for light's color.|`[1, 1, 1]` | `ambient`, `directional`, `point`, `spot` |
| `constantAttenuation`  | `number`     | Constant attenuation of point and spot lights. | `1.0` | `point`, `spot` |
| `linearAttenuation`    | `number`     | Linear (distance-based) attenuation of point and spot lights. | `0.0` | `point`, `spot` |
| `quadraticAttenuation` | `number`     | Quadratic attenuation of point and spot lights. | `0.0` | `point`, `spot` |
| `falloffAngle`         | `number`     | Falloff angle for the spot light, in radians.| `PI` | `spot` |
| `falloffExponent`      | `number`     | Falloff exponent for the spot light. | `0.0` | `spot` |

#### Ambient
Ambient lights define constant lighting throughout the scene, as reflected in the `ambient` property of any material. Ambient lights support only the `color` common light property described in **Table 5**.

#### Directional
Directional lights are light sources that emit from infinitely far away in a specified direction. Directional lights support only the `color` common light property described in **Table 5**.

#### Point
Point lights emit light in all directions over a given distance. Point lights support the `color`, `constantAttenuation`, `linearAttenuation` and `quadraticAttenuation` common light properties described in **Table 5**. Point lights do not have a direction.

#### Spot
Spot lights emit light in a direction over a given distance. Spot lights support the `color`, `constantAttenuation`, `linearAttenuation`, `quadraticAttenuation`, `falloffAngle` and `falloffExponent`  common light properties described in **Table 5**. Spot lights also define the following properties:

> **Implementation Note**: Attenuation factors are used to calculate the total attenuation of this light given a distance. The equation used is:
```
A = constantAttenuation + (Distance * linearAttenuation) + ((Distance ^ 2) * quadraticAttenuation)
```

<a name="schema"></a>
## JSON Schema
   * [`glTF`](schema/glTF.KHR_materials_common.schema.json)
   * [`node`](schema/node.KHR_materials_common.schema.json)
   * [`light`](schema/light.KHR_materials_common.schema.json)
   * [`light/ambient`](schema/light.ambient.KHR_materials_common.schema.json)
   * [`light/directional`](schema/light.directional.KHR_materials_common.schema.json)
   * [`light/point`](schema/light.point.KHR_materials_common.schema.json)
   * [`light/spot`](schema/light.spot.KHR_materials_common.schema.json)
   * [`material`](schema/material.KHR_materials_common.schema.json)
   * [`material/values`](schema/material.values.KHR_materials_common.schema.json)

<a name="conformance"></a>
## Conformance

If this extension is supported, it will be used to apply shading to any visuals defined with common material types.

If a material contains this extension as well as a technique instance, the runtime can use its own shader implementation in favor of the supplied technique.

If the extension is not supported, and no technique is supplied in the material, then default shading will be applied per the rules defined in the base specification.
