# KHR\_animation2

## Contributors

* Alexey Knyazev
* Ben Houston, Threekit
* Bryce Hutchings, Microsoft [@brycehutchings](https://twitter.com/brycehutchings)
* Don McCurdy
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Jamie Marconi, Microsoft [@najadojo](https://twitter.com/najadojo)
* Joe Herdman, [@jbherdman](https://github.com/jbherdman)
* Lewis Weaver, Microsoft
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is based on the animation features of glTF 2.0. The structure of the schemas are the same.  
  
The major change is, that the output values are mapped using a [JSON Pointer](https://datatracker.ietf.org/doc/html/rfc6901). Technically, the `path` of the Animation Channel Target has to be a JSON Pointer.  
  
### Motivation

At this point of time, one can only target the transformation or weight data of a node.  
With this extension, one can technically target any value (`scalar`, `vec2`, `vec3`, `vec4`, `quat` and scalars in an `array`) in glTF e.g.

* Color factors in materials
* Camera field of view

It even works on extensions and because of this, it is future proof:

* Light color factors
* Texture transformation values

In a first step, even using a JSON pointer, the targets and their expected behavior should be clearly defined:

#### Valid target templates

|`path`|Accessor Type|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`"/nodes/{}/translation"`|`"VEC3"`|`5126`&nbsp;(FLOAT)|XYZ translation vector|
|`"/nodes/{}/rotation"`|`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XYZW rotation quaternion|
|`"/nodes/{}/scale"`|`"VEC3"`|`5126`&nbsp;(FLOAT)|XYZ scale vector|
|`"/cameras/{}/orthographic/xmag"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Horizontal magnification of the view|
|`"/cameras/{}/orthographic/ymag"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Vertical magnification of the view|
|`"/cameras/{}/orthographic/zfar"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Distance to the far clipping plane|
|`"/cameras/{}/orthographic/znear"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Distance to the near clipping plane|
|`"/cameras/{}/perspective/aspectRatio"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Aspect ratio of the field of view|
|`"/cameras/{}/perspective/yfov"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Vertical field of view in radians|
|`"/cameras/{}/perspective/zfar"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Distance to the far clipping plane|
|`"/cameras/{}/perspective/znear"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Distance to the near clipping plane|
|`"/materials/{}/pbrMetallicRoughness/baseColorFactor"`|`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|The material's base color factor|
|`"/materials/{}/pbrMetallicRoughness/metallicFactor"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|The metalness of the material|
|`"/materials/{}/pbrMetallicRoughness/roughnessFactor"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|The roughness of the material|
|`"/materials/{}/alphaCutoff"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|The alpha cutoff value of the material|
|`"/materials/{}/emissiveFactor"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|The emissive color of the material|
|`"/materials/{}/normalTexture/scale"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Multiplier applied to each normal vector of the normal texture|
|`"/materials/{}/occlusionTexture/strength"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Multiplier controlling the amount of occlusion applied|
|`"/materials/{}/extensions/KHR_materials_pbrSpecularGlossiness/diffuseFactor"`|`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|The reflected diffuse factor of the material|
|`"/materials/{}/extensions/KHR_materials_pbrSpecularGlossiness/specularFactor"`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|The specular RGB color of the material|
|`"/materials/{}/extensions/KHR_materials_pbrSpecularGlossiness/glossinessFactor"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Angle, in radians, from centre of spotlight where falloff ends|
|`"/extensions/KHR_lights/lights/{}/color"`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|RGB value for light's color in linear space|
|`"/extensions/KHR_lights/lights/{}/intensity"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Brightness of light|
|`"/extensions/KHR_lights/lights/{}/innerConeAngle"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Angle, in radians, from centre of spotlight where falloff begins|
|`"/extensions/KHR_lights/lights/{}/outerConeAngle"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Brightness of light|
|`"/materials/{}{}/extensions/KHR_texture_transform/offset"`|`"VEC2"`|`5126`&nbsp;(FLOAT)|The offset of the UV coordinate origin as a factor of the texture dimensions|
|`"/materials/{}{}/extensions/KHR_texture_transform/rotation"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|Rotate the UVs by this many radians counter-clockwise around the origin|
|`"/materials/{}{}/extensions/KHR_texture_transform/scale"`|`"VEC2"`|`5126`&nbsp;(FLOAT)|The scale factor applied to the components of the UV coordinates|
|`"/nodes/{}/extensions/AVR_lights_static/strength"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|The influence of the lightmap on the final output|

##### `extras`

A channel may target `extras` at any allowed JSON Pointer value but
interpretation of the animated values is entirely application specific.

## Extension compatibility and fallback behavior

When possible, authoring tools should define reasonable initial values
for properties intended to be animated and mark the `EXT_property_animation`
extension as optional. Models including the extension optionally will still
render in all clients that support the core glTF 2.0 specification. Clients
that do not support the extension will fallback to an un-animated initial
state.

Where ever a property defines limitations to acceptable values an animation's
sampler must conform to those limitations.

## glTF Schema Updates

* **JSON schema**: [animation2.channel.target.schema.json](schema/animation2.channel.target.schema.json)

## Notes and clarifications

### `quat` and rotations

[Spherical Cubic Spline Interpolation](##Spherical-Cubic-Spline-Interpolation) is used when the animation sampler interpolation mode is set to CUBIC and the animated property is rotation, i.e., values of the animated property are unit quaternions.

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
                "target" : {
                    "path" : "/nodes/0/rotation"
                    "extensions": {
                        "KHR_animation2" : {}
                    }
               }
            }
        ],
        "samplers" : [
            {
                "input" : 0,
                "interpolation" : "LINEAR",
                "output" : 1
            }
        ]
    }
]
```

## Spherical Cubic Spline Interpolation

This form of rotation animation matches the de facto industry standard of animating rotations via spherical cubic interpolation ("sqlerp()", originally from [Shoemake (1985)](#Shoemake1985)).

The keyframes of a spherical cubic spline in glTF have input and output values where each input value corresponds to three output values (all unit quaternions): in-tangent, key-value, and out-tangent.

Given a set of keyframes

&nbsp;&nbsp;&nbsp;&nbsp;Input *t*<sub>*k*</sub> with Output in-tangent ***a***<sub>k</sub>, point ***v***<sub>*k*</sub>, and out-tangent ***b***<sub>k</sub> for *k* = 1,...,*n*

a spherical spline segment between two keyframes (k and k+1) is represented in a spherical cubic spline form:

&nbsp;&nbsp;&nbsp;&nbsp;***q***<sub>k</sub>(*t*) = ***sqlerp***(***v***<sub>k</sub>, ***b***<sub>k</sub>, ***a***<sub>k+1</sub>, ***v***<sub>k+1</sub>, *t*)


where

&nbsp;&nbsp;&nbsp;&nbsp;*t* is a value between 0 and 1  
&nbsp;&nbsp;&nbsp;&nbsp;***sqlerp***(***A***,***B***,***C***,***D***,*t*) = ***slerp***( ***slerp***(***A***,***D***,*t*), ***slerp***(***B***,***C***,*t*), 2*t - 2*t<sup>2</sup> )  
&nbsp;&nbsp;&nbsp;&nbsp;***slerp***() is spherical linear interpolation between two unit quaternions  
&nbsp;&nbsp;&nbsp;&nbsp;***q***(*t*) is the resulting unit quaternion value  

and where at input offset *t*<sub>*current*</sub> with keyframe index *k*

&nbsp;&nbsp;&nbsp;&nbsp;*t* = (*t*<sub>*current*</sub> - *t*<sub>*k*</sub>) / (*t*<sub>*k*+1</sub> - *t*<sub>*k*</sub>)  

> **Implementation Note:** The first in-tangent ***a***<sub>1</sub> and last out-tangent ***b***<sub>*n*</sub> should be zeros as they are not used in the spherical spline calculations.
## References

* Shoemake, Ken (1985):  Animating rotation with quaternion curves.  ACM SIGGRAPH Computer Graphics 19 (3), 245-254.<a name=Shoemake1985></a>

