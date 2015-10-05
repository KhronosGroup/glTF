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

This extension defines four light types: `directional`, `point`, `spot`, and `ambient`; and the material types `blinn`, `phong`, `lambert` and `constant`, for Blinn-Phong, Phong, Lambert and Unlit ("constant" color) materials, respectively.

The [conformance](#conformance) section specifies what an implementation must to do when encountering this extension, and how the extension interacts with the materials defined in the base specification.

## Materials

### Extending Materials

Common materials are defined by adding an `extensions` property to any glTF material, and defining its `KHR_materials_common` property. For example, the following defines a material with a Lambert shader with 50% gray diffuse color:

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
                            "diffuse": [0.5,0.5,0.5,1]
                        }
                    }
                }
            }
        },
```

### Material Types

#### Common Material Shared Properties

Table 1 lists properties that are shared among several of the common material types.

Table 1. Common Material Shared Properties

| Property                     | Type         | Description | Default Value | Applies To |
|:----------------------------:|:------------:|:-----------:|:-------------:|:----------:|
| `ambient`                    | `FLOAT_VEC4` | RGBA value for ambient light reflected from the surface of the object.|[0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT` |
| `diffuse`                    | `FLOAT_VEC4` or `SAMPLER_2D` | RGBA value or texture sampler defining the amount of light diffusely reflected from the surface of the object. | [0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT` |
| `emission`                   | `FLOAT_VEC4` | RGBA value for light emitted by the surface of the object. | [0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `index_of_refraction`            | `FLOAT` | Declares the index of refraction for perfectly refracted light as a single scalar index between 0.0 and 1.0. | 1.0 | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `reflective`                    | `FLOAT_VEC4` or `SAMPLER_2D` | RGBA value or texture sampler defining the color of a perfect mirror reflection. | [0,0,0,1] | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `reflectivity`                    | `FLOAT` | Declares the amount of perfect mirror reflection to be added to the reflected light as a value between 0.0 and 1.0. | 0.0 | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `specular`                    | `FLOAT_VEC4` or `SAMPLER_2D` | RGBA value or texture sampler defining the color of light specularly reflected from the surface of the object [need to explain the units here k-factor right?]. | [0,0,0,1] | `BLINN`, `PHONG` |
| `shininess`                    | `FLOAT` | Defines the specularity or roughness of the specular reflection lobe of the object. | 0.0 |  `BLINN`, `PHONG` |
| `technique`                    | string | Declares the lighting technique used. Must be one of `BLINN`, `PHONG`, `LAMBERT`, or `CONSTANT` | "" | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |
| `transparency`                    | `FLOAT` | Declares the amount of transparency as an opacity value between 0.0 and 1.0. | 1.0 | `BLINN`, `PHONG`, `LAMBERT`, `CONSTANT` |

#### Blinn-Phong

Blinn-Phong approximation. Uses all of the common material properties defined in Table 1. The following example defines a Blinn-Phong lit material with a diffuse texture, moderate shininess and red specular highlights. 

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
        },
```

#### Phong

Phong lighting. Uses all of the common material properties defined in Table 1. The following example defines a Phong lit material with a white diffuse color, environment map texture and high reflectivity.

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
						    	1,
						    	1
						    ],
						    "relective": "texture_envmap",
						    "refectivity": 1
						}
					}
            }
        }
        },
```

#### Lambert

Lambert shading does not reflect specular highlights; therefore the common material properties `specular` and `shininess` are not used. All other properties from Table 1 apply.

The following example defines a Lambert lit material with a 50% gray emissive color and a diffuse texture map.

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
						    ],
						}
					}
            }
        }
        },
```

#### Constant

Constant shading does not reflect light sources in the scene; therefore the common material properties `ambient`, `diffuse`, `specular` and `shininess` are not used. All other properties from Table 1 apply.

The following example defines a Constant lit material with a 50% gray emissive color and 50% opacity.

```javascript
    "materials": {
        "blinn1": {
            "extensions": {
                "KHR_materials_common" : {
					   "technique" : "CONSTANT",
						"values": {
							"emissive": [
								0.5,
								0.5,
								0.5,
								1
							],
							"transparency": 0.5
						}
					}
            }
        }
        },
```

<a name="lights"></a>
## Lights

Lights define light sources in the scene.

Lights are contained in nodes and thus can be transformed. Their world-space positions can be used in shader calculations.

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
            },

        }
    },
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
    }
```

### Light Types

#### Common Light Shared Properties

Table 2 lists properties that are shared among several of the common light types.

Table 2. Common Light Shared Properties

| Property                     | Type         | Description | Default Value | Applies To |
|:----------------------------:|:------------:|:-----------:|:-------------:|:----------:|
| `color`                      | `FLOAT_VEC4` | RGBA value for light's color.|[0,0,0,1] | `ambient`, `directional`, `point`, `spot` |
| `direction`                   | `FLOAT_VEC4` | Vector defining direction of directional and spot lights. | [0,0,-1] | `directional`, `spot` |
| `distance`                   | `FLOAT` | Distance, in world units, over which the light affects objects in the scene. A value of zero indicates infinite distance. | 0 | `point`, `spot` |
| `type`                    | string | Declares the type of the light. Must be one of `ambient`, `directional`, `point` or `spot` | "" | `ambient`, `directional`, `point`, `spot` |

#### Directional

Directional lights are light sources that emit from infinitely far away in a specified direction. This light type uses the common properties `color` and `direction` described in Table 2.

### Point

Point lights emit light from a specific location over a given distance. In addition to the `color` and `distance` properties described in Table 2, point lights define the following properties:



Spot

Ambient

<a name="schema"></a>
## JSON Schema

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
