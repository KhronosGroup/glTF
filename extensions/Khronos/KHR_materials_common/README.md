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

This extension defines four light types: `diffuse`, `point`, `spot`, and `ambient`; and the material types `blinn`, `phong`, `lambert` and `constant`, for Blinn, Blinn-Phong, Lamber and Unlit ("constant" color) materials, respectively.

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

### Common Material Types

Blinn

Phong

Lambert

Constant

<a name="lights"></a>
## Lights

Lights define light sources in the scene.

Lights are contained in nodes and thus can be transformed. Their world-space positions can be used in shader calculations.

### Defining Lights

Lights are defined within a dictionary property in the file, by adding an `extensions` property to the top-level glTF object and defining its `KHR_materials_common` property with a `lights` dictionary object inside it. 

Each light defines a `type` property that designates the type of light (`ambient`, `directional`, `point` or `spot`); then, a property of that name defines the details, such as color, attenuation and other light type-specific values. The following example defines a white-colored directional light.


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

Directional

Point

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
