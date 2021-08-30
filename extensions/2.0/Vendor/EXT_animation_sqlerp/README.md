# EXT_animation_sqlerp

## Contributors

* Joe Herdman, [@jbherdman](https://github.com/jbherdman)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds a new animation interpolation mode to glTF, based on the de facto industry standard of using spherical cubic interpolation (sqlerp) for quaternion animation.


## glTF Schema Updates

This extension allows for an additional interpolation-type in the `animation.sampler.interpolation` enumeration:
   * `"CUBICSLERP"` The animation's interpolation is computed using spherical cubic interpolation (sqlerp) with specified tangents. This interpolation mode is only valid when targeting a rotation. The number of output elements must equal three times the number of input elements. For each input element, the output stores three elements, an in-tangent quaternion, a spline quaternion, and an out-tangent quaternion. There must be at least two keyframes when using this interpolation.

See [CUBICSLERP Interpolation](#cubicslerp-interpolation) for additional information about spherical cubic interpolation (sqlerp).

The `EXT_animation_sqlerp` extension is added to the `animation.channel` object and specifies a `sampler` property.  The `sampler` property references an `animation.sampler` (within the same `animation`) which defines animation data using the new `CUBICSLERP` interpolation type.

The following glTF fragment demonstrates usage of the new `CUBICSLERP` interpolation type.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "EXT_animation_sqlerp"
    ],
    "animations": [
        {
            "channels": [
                {
                    "sampler": 1,
                    "target": {
                        "node": 0,
                        "path": "rotation"
                    },
                    "extensions": {
                        "EXT_animation_sqlerp": {
                            "sampler": 0
                        }
                    }
                }
            ],
            "samplers": [
                {
                    "input": 0,
                    "interpolation": "CUBICSLERP",
                    "output": 1
                },
                {
                    "input": 2,
                    "interpolation": "LINEAR",
                    "output": 3
                },
            ]
        }
    ],
    "nodes": [
        { "name": "example_node" }
    ]
}
```

In the above example, `animations[0].samplers[1]` refers to a fallback (`LINEAR`) version of the rotation animation, while `animations[0].samplers[0]` refers to the spherical cubic interpolation animation enabled by this extension.

### Using Without a Fallback

To use the new `CUBICSLERP` interpolation type without a fallback, define `EXT_animation_sqlerp` in both `extensionsUsed` and `extensionsRequired`.  The `animation.channel` object will then have its `sampler` property omitted as shown below.

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "EXT_animation_sqlerp"
    ],
    "extensionsRequired": [
        "EXT_animation_sqlerp"
    ],
    "animations": [
        {
            "channels": [
                {
                    "target": {
                        "node": 0,
                        "path": "rotation"
                    },
                    "extensions": {
                        "EXT_animation_sqlerp": {
                            "sampler": 0
                        }
                    }
                }
            ],
            "samplers": [
                {
                    "input": 0,
                    "interpolation": "CUBICSLERP",
                    "output": 1
                },
            ]
        }
    ],
    "nodes": [
        { "name": "example_node" }
    ]
}
```

### JSON Schema

TBD

## `CUBICSLERP` Interpolation

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

