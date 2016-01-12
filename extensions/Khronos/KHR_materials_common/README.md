# KHR\_materials\_common

## Contributors

* Tony Parisi, WEVR, [@auradeluxe](https://twitter.com/auradeluxe), <mailto:tparisi@gmail.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

This extension defines a set of common materials and lights for use with glTF. 

glTF does not define a lighting model, instead relegating shading of visuals to GLSL shader code defined within techniques in the glTF file. For many use cases, it is desirable to shade visuals with commonly understood and implemented shading techniques, such as Blinn and Lambert.

Many 3D tools and WebGL runtimes support built-in implementations of common material and light types. Using this extension, tools can export a much more compact representation of materials and lights. In addition, a runtimes with built-in lighting can apply its built-in light values to visuals shaded with common materials found in the glTF file. 

This extension defines four light types: `directional`, `point`, `spot`, and `ambient`; and the material types `blinn`, `phong`, `lambert` and `constant`, for Blinn, Phong, Lambert and Unlit ("constant" color) materials, respectively.

The [conformance](#conformance) section specifies what an implementation must to do when encountering this extension, and how the extension interacts with the materials defined in the base specification.

## Materials

### Extending Materials

Common materials are defined by adding an `extensions` property to any glTF material, and defining its `KHR_materials_common` property. Each material has two properties: `technique`, which specifies the shading technique used e.g. `BLINN`, and `values` which contains a set of technique-specific values.

For example, the following defines a material with a Lambert shader with 50% gray diffuse color:

```javascript
"materials": {
    "lambert1": {
        "extensions": {
            "KHR_materials_common" : {
                // one of CONSTANT,
                // BLINN,
                // PHONG,
                // LAMBERT
                "technique" : "LAMBERT",
                "values": {
                    "diffuse": [
                        0.5,
                        0.5,
                        0.5,
                        1
                    ]
                }
            }
        }
    }
}
```

### Material Types

#### Common Material Shared Properties

Table 1 lists properties that can appear in the `values` property of a common material, and that are shared among several of the common material types.

Table 1. Common Material Shared Properties

| Property                     | Type         | Description | Default Value | Applies To |
|:----------------------------:|:------------:|:-----------:|:-------------:|:----------:|
| `ambient`                    | `FLOAT_VEC4` | RGBA value for ambient light reflected from the surface of the object.|[0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `diffuse`                    | `FLOAT_VEC4` or string | RGBA value or texture ID defining the amount of light diffusely reflected from the surface of the object. | [0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT` |
| `doubleSided`                    | `boolean` | Declares whether backface culling should be disabled for this visual. Corresponds to disabling the `CULL_FACE` render state. | `false` | |
| `emission`                   | `FLOAT_VEC4` or string | RGBA value or texture ID for light emitted by the surface of the object. | [0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `specular`                    | `FLOAT_VEC4` or string | RGBA value or texture ID defining the color of light specularly reflected from the surface of the object. | [0,0,0,1] | `BLINN`, `PHONG` |
| `shininess`                    | `FLOAT` | Defines the specularity or roughness of the specular reflection lobe of the object. | 0.0 |  `BLINN`, `PHONG` |
| `transparency`                    | `FLOAT` | Declares the amount of transparency as an opacity value between 0.0 and 1.0. | 1.0 | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `transparent`                    | `boolean` | Declares whether the visual should be rendered using alpha blending. Corresponds to enabling the `BLEND` render state, setting the `depthMask` property to `false`, and defining blend equations and blend functions as described in the implementation note. | `false` | |

> **Implementation Note**: An implementation should render alpha-blended visuals in depth-sorted order. An implementation should define blend equations and blend functions that result in properly blended RGB and alpha values, such as in the following example:

```
   functions: {
       blendEquationSeparate: [
           WebGLConstants.FUNC_ADD,
           WebGLConstants.FUNC_ADD
       ],
       blendFuncSeparate: [
           WebGLConstants.ONE,
           WebGLConstants.ONE_MINUS_SRC_ALPHA,
           WebGLConstants.ONE,
           WebGLConstants.ONE_MINUS_SRC_ALPHA
       ]
   }
```


#### Blinn

When the value of `technique` is `BLINN`, this defines a material shaded according to the Blinn-Torrance-Sparrow lighting model or a close approximation.

This equation is complex and detailed via the ACM, so it is not detailed here. Refer to “Models of Light
Reflection for Computer Synthesized Pictures,” SIGGRAPH 77, pp 192-198 [http://portal.acm.org/citation.cfm?id=563893](http://portal.acm.org/citation.cfm?id=563893).

The following code illustrates the basic computation:

```
color = <emission> + <ambient> * al + <diffuse> * max(N * L, 0) + <specular> * max(H * N, 0)^<shininess>
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.
* `N` – Normal vector
* `L` – Light vector
* `I` – Eye vector
* `H` – Half-angle vector,calculated as halfway between the unit Eye and Light vectors, using the
equation H= normalize(I+L)

> **Implementation Note**: Writers should be aware about the range of the specular exponent (`shininess`), which is _not_ a normalized range. Concretely speaking, given the above equation, a `shininess` value of 1 corresponds to a very low shininess. For orientation: using the traditional OpenGL fixed function pipeline, the specular exponent was expected to be within [0, 128]. However, using glTF, larger `shininess` values are clearly possible.

Blinn shading uses all of the common material properties defined in Table 1. The following example defines a Blinn shaded material with a diffuse texture, moderate shininess and red specular highlights. 

```javascript
"materials": {
    "blinn1": {
        "extensions": {
            "KHR_materials_common" : {
                "technique" : "BLINN",
                "values": {
                    "diffuse": "texture_1",
                    "shininess": 10,
                    "specular": [
                        1,
                        0,
                        0,
                        1
                    ]
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

Phong lighting uses all of the common material properties defined in Table 1. The following example defines a Phong lit material with a yellow diffuse color.

```javascript
"materials": {
    "blinn1": {
        "extensions": {
            "KHR_materials_common" : {
                "technique" : "PHONG",
                "values": {
                    "diffuse": [
                        1,
                        1,
                        0,
                        1
                    ]
                }
            }
        }
    }
}
```

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

Lambert shading does not reflect specular highlights; therefore the common material properties `specular` and `shininess` are not used. All other properties from Table 1 apply.

The following example defines a Lambert shaded material with a 50% gray emissive color and a diffuse texture map.

```javascript
"materials": {
    "blinn1": {
        "extensions": {
            "KHR_materials_common" : {
                "technique" : "LAMBERT",
                "values": {
                    "diffuse": "texture_1",
                    "emission": [
                        0.5,
                        0.5,
                        0.5,
                        1
                    ]
                }
            }
        }
    }
}
```

#### Constant

When the value of `technique` is `CONSTANT`, this defines a material that describes a constantly
shaded surface that is independent of lighting.

The reflected color is calculated as:

```
color = <emission> + <ambient> * al
```

where

* `al` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.

Constant shading does not reflect light sources in the scene; therefore the common material properties `diffuse`, `specular` and `shininess` are not used. All other properties from Table 1 apply.

The following example defines a Constant lit material with an emissive texture and 50% opacity.

```javascript
"materials": {
    "blinn1": {
        "extensions": {
            "KHR_materials_common" : {
                "technique" : "CONSTANT",
                "values": {
                    "emission": "texture_2",
                    "transparency": 0.5
                }
            }
        }
    }
}
```

### Interaction Between Attribute Semantics and Common Materials

The base specification defines attribute semantics for mesh primitives. The common materials in this extension shall support the semantics `POSITION`, `NORMAL`, `TEXCOORD`, `COLOR`, `JOINT`, `JOINTMATRIX`, and `WEIGHT`. For array semantics such as texture coordinates, the implemention is only required to support the 0th set of coordinates i.e. `TEXCOORD_0`.
Since semantics are usually defined via technique parameters, they are not present in the glTF scene description when `KHR_materials_common` is used.
Therefore, writers using this extension must ensure that the attributes of the mesh are named exactly like the semantics:

```javascript
"meshes": {
    "mesh0":{         
        "primitives" : [
        {
           "attributes":{
              "NORMAL"  :"accessor23",
              "POSITION":"accessor42"
           },
           "indices" : "accessor13",
           "material": "material7",
           "mode":4
        }
     ]
    }
}
```

If a conforming implementation of this extension supports skinned animations, then the common materials described in this extension must also support hardware skinning in its vertex and fragment shader programs.


<a name="lights"></a>
## Lights

Lights define light sources in the scene.

Lights are contained in nodes and thus can be transformed. Their world-space positions can be used in shader calculations.

A conforming implementation of this extension must be able to load light data defined in the asset, but it is not obligated to render the asset using those lights. 

### Defining Lights

Lights are defined within a dictionary property in the file, by adding an `extensions` property to the top-level glTF object and defining its `KHR_materials_common` property with a `lights` dictionary object inside it. 

Each light defines a `type` property that designates the type of light (`ambient`, `directional`, `point` or `spot`); a property of that same name defines the details, such as color, attenuation and other light type-specific values. The following example defines a white-colored directional light.


```javascript
"extensions": {
    "KHR_materials_common" : {
        "lights": {
            "light1": {
                "directional": {
                    "color": [
                        1,
                        1,
                        1
                    ]
                },
                "type": "directional"
            }
        }
    }
}
```

IDs of any property within the `lights` extension property are treated like other glTF IDs, i.e. they are added to the global ID space.


### Adding Light Instances to Nodes

Lights are glTF scene objects: they have position and orientation, which can be animated. Lights are placed in the scene by defining an `extensions` property of a node, and within that a `KHR_materials_common` property that identifies a light using its `light` property.

```javascript
"nodes": {
    "node1" : {
        "children" : [
    ],
    "extensions": {
        "KHR_materials_common" : {
            "light" : "light1",
        }
    }            
}
```

For light types that have a direction (directional and spot lights), the light's direction is defined as the vector (0, 0, -1) and the rotation of the node orients the light accordingly.


### Light Types

#### Common Light Shared Properties

Table 2 lists properties that are shared among several of the common light types.

Table 2. Common Light Shared Properties

| Property                     | Type         | Description | Default Value | Applies To |
|:----------------------------:|:------------:|:-----------:|:-------------:|:----------:|
| `color`                      | `FLOAT_VEC4` | RGBA value for light's color.|[0,0,0,1] | `ambient`, `directional`, `point`, `spot` |
| `constantAttenuation`       | `FLOAT` | Constant attenuation of point and spot lights. | 0 | `point`, `spot` |
`directional`, `spot` |
| `distance`                   | `FLOAT` | Distance, in world units, over which the light affects objects in the scene. A value of zero indicates infinite distance. | 0 | `point`, `spot` |
| `linearAttenuation`       | `FLOAT` | Linear (distance-based) attenuation of point and spot lights. | 1 | `point`, `spot` |
| `quadraticAttenuation`       | `FLOAT` | Quadratic attenuation of point and spot lights. | 1 | `point`, `spot` |
| `type`                    | string | Declares the type of the light. Must be one of `ambient`, `directional`, `point` or `spot` | "" | `ambient`, `directional`, `point`, `spot` |

### Ambient

Ambient lights define constant lighting throughout the scene, as reflected in the `ambient` property of any material. Ambient lights support only the `color` common light property described in Table 2.

#### Directional

Directional lights are light sources that emit from infinitely far away in a specified direction. This light type uses the common light properties `color` and `direction` described in Table 2.

### Point

Point lights emit light in all directions over a given distance. Point lights support the `color`, `constantAttenuation`, `distance` and `linearAttenuation` common light properties described in Table 2. Point lights do not have a direction.


### Spot

Sport lights emit light in a directions over a given distance. Spot lights support the `color`, `constantAttenuation`, `direction`, `distance` and `linearAttenuation` common light properties described in Table 2. Spot lights also define the following properties:

Table 3. Spot Light Properties

| Property                     | Type         | Description | Default Value |
|:----------------------------:|:------------:|:-----------:|:-------------:|
| `falloffAngle`              | `FLOAT` | Falloff angle for the spot light, in radians.| PI / 2 |
| `falloffExponent`           | `FLOAT` | Falloff exponent for the spotlight. | 0 |


<a name="schema"></a>
## JSON Schema

*To-do: Patrick need schema for each of the material types... can you help?*

   * <a href="schema/light.schema.json">`light`</a>
   * <a href="schema/lightAmbient.schema.json">`light/ambient`</a>
   * <a href="schema/lightDirectional.schema.json">`light/directional`</a>
   * <a href="schema/lightPoint.schema.json">`light/point`</a>
   * <a href="schema/lightSpot.schema.json">`light/spot`</a>


<a name="conformance"></a>
## Conformance

If this extension is supported, it will be used to apply shading to any visuals defined with common material types. 

If a material contains this extension as well as a technique instance, the runtime can use its own shader implementation in favor of the supplied technique. 

If the extension is not supported, and no technique is supplied in the material, then default shading will be applied per the rules defined in the base specification. 
