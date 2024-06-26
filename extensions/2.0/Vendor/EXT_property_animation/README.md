# EXT\_property\_animation

## Contributors

* Alexey Knyazev
* Bryce Hutchings, Microsoft [@brycehutchings](https://twitter.com/brycehutchings)
* Don McCurdy, Google [@donrmccurdy](https://twitter.com/donrmccurdy)
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Jamie Marconi, Microsoft [@najadojo](https://twitter.com/najadojo)
* Lewis Weaver, Microsoft

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds a specification for animation data targeting properties,
such as material colors, texture transform matrices and extension properties.

## Extending Animations

Property animations can be added to an animation by adding the
`EXT_property_animation` extension to any glTF animation. For example, the
following defines an animation with two channels that modify a material's
baseColorFactor and roughnessFactor.
```json
"animations" : [
    {
        "channels" : [
            {
                "sampler" : 2,
                "target" : {
                    "node" : 0,
                    "path" : "scale"
                }
            }
        ],
        "extensions" : {
            "EXT_property_animation" : {
                "channels" : [
                    {
                        "sampler" : 0,
                        "target" : "/materials/1/pbrMetallicRoughness/roughnessFactor"
                    },
                    {
                        "sampler" : 1,
                        "target" : "/materials/1/pbrMetallicRoughness/baseColorFactor"
                    }
                ]
            }
        },
        "samplers" : [
            {
                "input" : 6,
                "interpolation" : "CUBICSPLINE",
                "output" : 7
            },
            {
                "input" : 8,
                "interpolation" : "LINEAR",
                "output" : 9
            },
            {
                "input" : 10,
                "interpolation" : "LINEAR",
                "output" : 11
            }
        ]
    }
]
 ```
`EXT_property_animation` adds a channels list separate from the core
specification's such that a different set of target values can be described.

### sampler

`sampler` has the same meaning and interpretation as the core specification's
`sampler` property. The property animation keyframes are defined in the normal
samplers list and the `sampler` reference indexes into this list for the
enclosing animation.

The sampler's output accessor type must match the targeted property's type.

### target

`target` is a [JSON Pointer](https://tools.ietf.org/html/rfc6901) in the glTF
JSON specifying a leaf property value. The property need not be present in the
original JSON; default or implicit properties can be animated. Because the
[JSON Pointer](https://tools.ietf.org/html/rfc6901) is able to reference any
JSON property `EXT_property_animation` allows animation of all current and
future glTF extensions. Extensions should define properties that are intended
to be animatable in their specification by referring to the syntax defined
here.

#### Valid target templates


|`channel.path`|Accessor Type|Component Type(s)|Description|
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

#### `extras`

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

* **JSON schema**: [animation.EXT_property_animation.schema.json](schema/animation.EXT_property_animation.schema.json)

## Known Implementations

* [Blender glTF 2.0 Exporter](https://github.com/najadojo/glTF-Blender-Exporter/compare/master...najadojo:EXT_property_animation)
Experiment/Proof of concept to export material property animations.
* [three.js](https://github.com/mrdoob/three.js/compare/dev...najadojo:EXT_property_animation)
Experiment/Proof of concept to import material property animations.
