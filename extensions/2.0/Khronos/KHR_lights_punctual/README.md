# KHR\_lights\_punctual

## Contributors

* Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
* Thomas Kress, UX3D, <mailto:kress@ux3d.io>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a set of lights for use with glTF 2.0. Lights define light sources within a scene.

Many 3D tools and engines support built-in implementations of light types. Using this extension, tools can export and engines can import these lights. 

This extension defines three "punctual" light types: `directional`, `point` and `spot`. Punctual lights are defined as parameterized, infinitely small points that emit light in well-defined directions and intensities.

These lights are referenced by nodes and inherit the transform of that node.

A conforming implementation of this extension must be able to load light data defined in the asset and has to render the asset using those lights. 

## Defining Lights

Lights are defined within a dictionary property in the glTF manifest file, by adding an `extensions` property to the top-level glTF 2.0 object and defining a `KHR_lights_punctual` property with a `lights` array inside it.

Each light defines a mandatory `type` property that designates the type of light (`directional`, `point` or `spot`). The following example defines a white-colored directional light.

```javascript
"extensions": {
    "KHR_lights_punctual" : {
        "lights": [
            {
                "color": [
                    1.0,
                    1.0,
                    1.0
                ],
                "type": "directional"
            }
        ]
    }
}
```

## Adding Light Instances to Nodes

Lights must be attached to a node by defining the `extensions.KHR_lights_punctual` property and, within that, an index into the `lights` array using the `light` property.

```javascript
"nodes" : [
    {
        "extensions" : {
            "KHR_lights_punctual" : {
                "light" : 0
            }
        }
    }            
]
```

The light will inherit the transform of the node. For light types that have a position (`point` and `spot` lights), the light's position is defined as the node's world location.
For light types that have a direction (`directional` and `spot` lights), the light's direction is defined as the 3-vector `(0.0, 0.0, 1.0)` and the rotation of the node orients the light accordingly.

## Light Types

All light types share the common set of properties listed below.

### Light Shared Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, Default: `""` |
| `color` | RGB value for light's color in linear space. | No, Default: `[1.0, 1.0, 1.0]` |
| `intensity` | Brightness of light in. The units that this is defined in depend on the type of light. `point` and `spot` lights use luminous intensity in candela (lm/sr) while `directional` lights use illuminance in lux (lm/m<sup>2</sup>) | No, Default: `1.0` |
| `type` | Declares the type of the light. | :white_check_mark: Yes |
| `range` | Hint defining a distance cutoff at which the light's intensity may be considered to have reached zero. Supported only for `point` and `spot` lights. | No, Default: `0.0` |

## Range Property

The range property (allowed only on point and spot lights) defines a distance cutoff at which the light's intensity must be considered zero, meaning the light no longer affects the surrounding area. This can be useful to cull geometry that a light may not visibly affect, potentially having a significant positive impact on rendering performance. It is required that, when given a non-zero value, rendering engines ignore the light beyond this range.

Within the range of the light, attenuation should follow the inverse square law as closely as possible, although some non-quadratic falloff near the edge of the range may be used to avoid a hard cutoff. A `range` of 0 implies that no cutoff should be used, attenuating only according to inverse square law.

A recommended implementation for this attenuation with a cutoff range is as follows:

**attenuation = max( min( 1.0 - ( current_distance / range )<sup>4</sup>, 1 ), 0 ) / current_distance<sup>2</sup>**

### Directional

Directional lights are light sources that act as though they are infinitely far away and emit light in the direction of the local +z axis. This light type inherits the orientation of the node that it belongs to but ignores position and scale. Because it is at an infinite distance, the light is not attenuated. Its intensity is defined in lumens per metre squared, or lux (lm/m<sup>2</sup>).

### Point

Point lights emit light in all directions from its position in space (and ignore rotation and scale). The brightness of the light attenuates in a physically correct manner as distance increases from the light's position (i.e. brightness goes like the inverse square of the distance). Point light intensity is defined in candela, which is lumens per square radian (lm/sr).

### Spot

Spot lights emit light in a cone in the direction of the local +z axis. The angle and falloff of the cone is defined using two numbers, the `innerConeAngle` and `outerConeAngle`. As with point lights, the brightness also attenuates in a physically correct manner as distance increases from the light's position (i.e. brightness goes like the inverse square of the distance). Spot light intensity refers to the brightness inside the `innerConeAngle` (and at the location of the light) and is defined in candela, which is lumens per square radian (lm/sr). Engines that don't support two angles for spotlights should use `outerConeAngle` as the spotlight angle (leaving `innerConeAngle` to implicitly be `0`).

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `innerConeAngle` | Angle, in radians, from centre of spotlight where falloff begins. Must be greater than or equal to `0` and less than `outerConeAngle`. | No, Default: `0` |
| `outerConeAngle` | Angle, in radians, from centre of spotlight where falloff ends.  Must be greater than `innerConeAngle` and less than or equal to `PI / 2.0`. | No, Default: `PI / 4.0` |

```javascript
"extensions": {
    "KHR_lights_punctual" : {
        "lights": [
            {
                "spot": {
                    "innerConeAngle": 0.785398163397448,
                    "outerConeAngle": 1.57079632679,
                },
                "color": [
                    1.0,
                    1.0,
                    1.0
                ],
                "type": "spot"
            }
        ]
    }
}
```

## Inner and Outer Cone Angles

There should be a smooth attenuation of brightness between the `innerConeAngle` and `outerConeAngle` angles. In reality, this "angular" attenuation is very complex as it depends on the physical size of the spotlight and the shape of the sheath around the bulb. 

Conforming implementations will model this angular attenuation with a curve that follows a steeper decline in brightness before leveling off when moving from the inner to the outer angle.

It is common to model this falloff by interpolating between the cosine of each angle. This is an efficient approximation that provides decent results.

Reference code:

```c++
// These two values can be calculated on the CPU and passed into the shader
float lightAngleScale = 1.0f / max(0.001f, cos(innerConeAngle) - cos(outerConeAngle));
float lightAngleOffset = -cos(outerConeAngle) * lightAngleScale;

// Then, in the shader:
float cd = dot(spotlightDir, normalizedLightVector);
float angularAttenuation = saturate(cd * lightAngleScale + lightAngleOffset);
angularAttenuation *= angularAttenuation;
```

