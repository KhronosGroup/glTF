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

If this extension is supported, it will be used to apply shading to any visuals defined with common material types. If it is not supported, and no technique is supplied in the material, then default shading will be applied per the rules defined in the base specification.

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

## Lights

### Defining Lights

Lights are defined within a dictionary property in the file, by adding an `extensions` property to the top-level glTF object and defining its `KHR_materials_common` property with a `lights` dictionary object inside it. The `lights` property can contain one or more objects, as follows:

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

The `type` property specifies the type of light. Valid light types are `directional`, `point`, `spot` and `ambient`. A property with name corresponding to the `type` property's value defines the parameter values for the light. In the above example, the `directional` property contains an object with a diffuse light color stored in the `color` property.

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

### Common Light Types

Directional

Point

Spot

Ambient


## Conformance