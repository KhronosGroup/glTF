# EXT\_property\_animation

## Contributors

* Bryce Hutchings, Microsoft [@brycehutchings](https://twitter.com/brycehutchings)
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Jamie Marconi, Microsoft [@najadojo](https://twitter.com/najadojo)
* Lewis Weaver, Microsoft

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds a specification for animation data targeting arbitrary
properties, such as material colors, texture transform matrices and
extension properties.

## Extending Animations

Property animations can be added to an animation by adding the
`EXT_property_animation` extension to any glTF animation. For example, the
following defines an animation with two channels that modify a material's
baseColorFactor and roughnessFactor.
```json
"animations" : [
    {
        "channels" : [], 
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
original JSON; default or implicit properties can be animated. `target` must
reference a leaf property on an object that fits the data types supported by
accessor elements. These types are `MAT4`, `MAT3`, `MAT2`, `VEC4`, `VEC3`,
`VEC2`, and `SCALAR`. Properties that don't fit these data types cannot be
animated. Animation of boolean or enum values is prevented as well as any
indexed object reference within the glTF JSON (i.e. a property animation can't
describe the value `"material": 0` or a new vertex buffer accessor such as
`"POSITION" : 4`).

Because the [JSON Pointer](https://tools.ietf.org/html/rfc6901) is able to
reference any JSON property `EXT_property_animation` allows animation of all
current and future glTF extensions. The path value would simply include
`/extensions/EXT_my_extension/my_property`.

## Extension compatibility and fallback behavior

When possible, authoring tools should define reasonable initial values
for properties intended to be animated and mark the `EXT_property_animation`
extension as optional. Models including the extension optionally will still
render in all clients that support the core glTF 2.0 specification. Clients
that do not support the extension will fallback to an un-animated initial
state.

## glTF Schema Updates

* **JSON schema**: [animation.EXT_property_animation.schema.json](schema/animation.EXT_property_animation.schema.json)

## Known Implementations

* [Blender glTF 2.0 Exporter](https://github.com/najadojo/glTF-Blender-Exporter/compare/master...najadojo:EXT_property_animation)
Experiment/Proof of concept to export material property animations.
* [three.js](https://github.com/mrdoob/three.js/compare/dev...najadojo:EXT_property_animation)
Experiment/Proof of concept to import material property animations.
