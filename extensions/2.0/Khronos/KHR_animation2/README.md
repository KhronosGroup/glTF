# KHR\_animation2

## Contributors

* Alexey Knyazev
* Ben Houston
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)  
* [scurest](https://github.com/scurest)  

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is based on the animation features of glTF 2.0. The structure of the schemas are the same, however the properties are changed and simplified.  
  
The major change is, that the output values are mapped using a [JSON Pointer](https://datatracker.ietf.org/doc/html/rfc6901). Furthermore, the explicit interpolation type `SLERP` for quaternions is introduced.  
  
At this point of time, one can only target the transformation or weight data of a node.  
With this extension, one can technically target any value (`scalar`, `vec2`, `vec3`, `vec4`, `quat` and scalars in an `array`) in glTF e.g.

* Color factors in materials
* Camera field of view

It even works on extensions and because of this, it is future proof:

* Light color factors
* Texture transformation values

In a first step, even using a JSON pointer, the targets and their expected behavior should be clearly defined.  

### Valid interpolations

The following table provides an overview, which interpolation type is valid for which data type:  

|        |STEP|LINEAR|SLERP|CUBICSPLINE|
|--------|----|------|-----|-----------|
|`scalar`|X   |X     |     |X          |
|`vec2`  |X   |X     |     |X          |
|`vec3`  |X   |X     |     |X          |
|`vec4`  |X   |X     |     |X          |
|`quat`  |X   |      |X    |           |

### JSON Pointer

For a `scalar` value, the JSON Pointer targets the glTF property.  
This means, the property is replaced by the interpolated value.
  
For any other case, the JSON Pointer targets the glTF property as well but must be an `array` property.  
This means, that the two or more elements do replace the values in the array.  

## Examples

The following snippet shows the changes for [`animations`](https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AnimatedCube/glTF/AnimatedCube.gltf#L117) from the [AnimatedCube](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube) asset.

```javascript
"animations" : [
    {
        "channels" : [
            {
                "name" : "Targeting x, y, z, w for a rotation of node at index 0."
                "sampler" : 0,
                "targetpointer" : "nodes/0/rotation"
            }
        ],
        "samplers" : [
            {
                "input" : 0,
                "interpolation" : "SLERP",
                "output" : 1
            }
        ]
    }
]
```
