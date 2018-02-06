# KHR\_lights

## Contributors

* Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
* Thomas Kress, UX3D, <mailto:kress@ux3d.io>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a set of lights for use with glTF 2.0. 

Many 3D tools and engines support built-in implementations of light types. Using this extension, tools can export and engines can import these lights. 

This extension defines five light types: `ambient`, `directional`, `point` and `spot`.

## Lights

Lights define light sources within a scene.

`ambient` lights are not transformable and, thus, can only be defined on the scene. All other lights are contained in nodes and inherit the transform of that node.

A conforming implementation of this extension must be able to load light data defined in the asset and has to render the asset using those lights. 

### Defining Lights

Lights are defined within an dictionary property in the glTF manifest file, by adding an `extensions` property to the top-level glTF 2.0 object and defining a `KHR_lights` property with a `lights` array inside it.

Each light defines a mandatory `type` property that designates the type of light (`ambient`, `directional`, `point` or `spot`). The following example defines a white-colored directional light.

```javascript
"extensions": {
    "KHR_lights" : {
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

### Adding Light Instances to Nodes

Nodes can have lights attached to them, just like any other objects that have a transform. Only lights of types `directional`, `point`, and `spot` have position and/or orientation so only these light types are allowed. These lights are attached to a node by defining the `extensions.KHR_lights` property and, within that, an index into the `lights` array using the `light` property.

```javascript
"nodes" : [
    {
        "extensions" : {
            "KHR_lights" : {
                "light" : 0
            }
        }
    }            
]
```

For light types that have a position (`point` and `spot` lights), the light's position is defined as the node's world location.
For light types that have a direction (`directional` and `spot` lights), the light's direction is defined as the 3-vector `(0.0, 0.0, 1.0)` and the rotation of the node orients the light accordingly.

### Adding Light Instances to Scenes

Lights that have no transform information can be attached to scenes. `ambient` lights have no position, no orientation and no range. They are therefore global and belong to a scene rather than a node. These lights are attached to the scene by defining the `extensions.KHR_lights` property and, within that, an index into the `lights` array using the `light` property.

```javascript
"scenes" : [
    {
        "extensions" : {
            "KHR_lights" : {
                "light" : 0
            }
        }
    }            
]
```

### Light Types

All light types share the common set of properties listed below.

#### Light Shared Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, Default: `""` |
| `color` | RGB value for light's color in linear space. | No, Default: `[1.0, 1.0, 1.0]` |
| `intensity` | Brightness of light in. The units that this is defined in depend on the type of light. `point` and `spot` lights use luminous intensity in candela (lm/sr) while `directional` lights use illuminance in lux (lm/m^2) | No, Default: `1.0` |
| `type` | Declares the type of the light. | :white_check_mark: Yes |

#### Ambient

Ambient lights define constant lighting throughout the scene. They are referenced only by a scene object and only 1 can be referenced per scene.

#### Directional

Directional lights are light sources that act as though they are infinitely far away and emit light in the direction of the +z axis. This light type inherits the orientation of the node that it belongs to. Because it is at an infinite distance, the light is not attenuated. It's intensity is defined in lumens per metre squared, or lux (lm/m^2).

#### Point

Point lights emit light in all directions from a position in space. The brightness of the light attenuates in a physically correct manner as distance increases from the light's position (i.e. brightness goes like the inverse square of the distance). Point light intensity is defined in candela, which is lumens per square radian (lm/sr).

#### Spot

Spot lights emit light in a cone over a distance. The angle and falloff of the cone is defined using two numbers, the `innerConeAngle` and `outerConeAngle`. As with point lights, the brightness also attenuates in a physically correct manner as distance increases from the light's position (i.e. brightness goes like the inverse square of the distance). Spot light intensity refers to the brightness inside the `innerConeAngle` and is defined in candela, which is lumens per square radian (lm/sr). Engines that don't support two angles for spotlights should use `outerConeAngle` as the spotlight angle (leaving `innerConeAngle` to implicitly be `0`).

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `innerConeAngle` | Angle, in radians, from centre of spotlight where falloff begins. Must be greater than `0`, less than `outerConeAngle` and less than `PI / 2.0`. | No, Default: `0` |
| `outerConeAngle` | Angle, in radians, from centre of spotlight where falloff ends.  Must be greater than `0`, greater than `innerConeAngle` and less than `PI / 2.0`. | No, Default: `PI / 4.0` |

```javascript
"extensions": {
    "KHR_lights" : {
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