# KHR\_animation2

## Contributors

* Alexey Knyazev
* Ben Houston
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)  
* scurest TODO: Use real name if wanted.

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is based on the animation features of glTF 2.0. The structure of the schemas are the same, however the properties are changed and simplified.  
  
The change is, that the output values are mapped using a [JSON Pointer](https://datatracker.ietf.org/doc/html/rfc6901).
  
At this point of time, one can only target the transformation or weight data of a node.  
With this approach, one can technically target any value (`scalar`, `vec2`, `vec3`, `vec4`, `quat` and scalars in an `array`) in glTF e.g.

* Color factors in materials
* Camera field of view

It even works on extensions and because of this, it is future proof:

* Light color factors
* Texture transformation values

In a first step, even using a JSON pointer, the targets and their expected behaviour should be clearly defined.  

### Valid interpolations

|        |STEP|LINEAR|SLERP|CUBICSPLINE|
|--------|----|------|-----|-----------|
|`scalar`|X   |X     |     |X          |
|`vec2`  |X   |X     |     |X          |
|`vec3`  |X   |X     |     |X          |
|`vec4`  |X   |X     |     |X          |
|`quat`  |X   |      |X    |           |

## Examples

The following snippet shows the changes for [`animations`](https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AnimatedCube/glTF/AnimatedCube.gltf#L117) from the [AnimatedCube](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube) asset.

```javascript
"animations" : [
    {
        "channels" : [
            {
                "name" : "Targeting x, y, y, w for a rotation of node at index 0."
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
