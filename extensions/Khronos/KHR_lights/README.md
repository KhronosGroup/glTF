# KHR\_lights

## Contributors

* Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
* Thomas Kress, UX3D, <mailto:kress@ux3d.io>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a set of lights for use with glTF 2.0. 

Many 3D tools and engines support built-in implementations of light types. Using this extension, tools can export and engines can import these lights. 

This extension defines four light types: `ambient`, `directional`, `point` and `spot`.

## Lights

Lights define light sources in the scene.

`ambient` lights are contained in scenes and influence the whole scene.

The other lights are contained in nodes thus can be transformed.

A conforming implementation of this extension must be able to load light data defined in the asset and has to render the assets using those lights. 

### Defining Lights

Lights are defined within an dictionary property in the file, by adding an `extensions` property to the top-level glTF 2.0 object and defining its `KHR_lights` property with a `lights` property inside it.

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

`directional`, `point`, `spot` lights have position and/or orientation, which can be animated. These lights are placed in the scene by defining an `extensions` property of a node, and within that a `KHR_lights` property that identifies a light using its `light` property.

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

For light types that have a position (`point` and `spot` lights), the light's position is defined of the node's world location.
For light types that have a direction (`directional` and `spot` lights), the light's direction is defined as the vector `(0.0, 0.0, -1.0)` and the rotation of the node orients the light accordingly.

`ambient` lights are only valid referenced by a scene.

### Adding Light Instances to Scenes

`ambient` lights have no position and no orientation. These lights are placed in the scene by defining an `extensions` property of a scene, and within that a `KHR_lights` property that identifies a light using its `light` property.

`directional`, `point`, `spot` lights are only valid referenced by a node.

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

#### Light Shared Properties

Table 1. Light Shared Properties

| Property               | Description                     | Default Value | Applies To                                |
|:-----------------------|:--------------------------------|:--------------|:------------------------------------------|
| `color`                | RGB value for light's color.    | [0.0,0.0,0.0] | `ambient`, `directional`, `point`, `spot` |
| `type`                 | Declares the type of the light. | Mandatory     | `ambient`, `directional`, `point`, `spot` |
| `constantAttenuation`  | Constant attenuation.           | 1.0           | `point`, `spot`                           |
| `linearAttenuation`    | Linear attenuation.             | 0.0           | `point`, `spot`                           |
| `quadraticAttenuation` | Quadratic attenuation.          | 0.0           | `point`, `spot`                           |
| `falloffAngle`         | Falloff angle in radians.       | PI / 2.0      | `spot`                                    |
| `falloffExponent`      | Falloff exponent.               | 0.0           | `spot`                                    |

#### Ambient

Ambient lights define constant lighting throughout the scene. Ambient lights support only the `color` light property described in Table 1.
For Physically-Based Rendering (PBR) materials, this parameter is treated as having an environment map having this color value for image based lighting (IBL).

#### Directional

Directional lights are light sources that emit from infinitely far away in a specified direction. This light type uses the light properties `color` and `direction` described in Table 1.

#### Point

Point lights emit light in all directions over a distance. Point lights support the `color` property. `constantAttenuation`, `linearAttenuation` and `quadraticAttenuation` light properties embedded in the `positional` property describe the different attenuations.

```javascript
"extensions": {
    "KHR_lights" : {
        "lights": [
            {
                "positional": [
                    "constantAttenuation": 1.0
                ],
                "color": [
                    1.0,
                    1.0,
                    1.0
                ],
                "type": "point"
            }
        ]
    }
}
```

Point lights do not have a direction.

#### Spot

Spot lights emit light in a direction over a distance.  Spot lights support the `color` property. `constantAttenuation`, `linearAttenuation` and `quadraticAttenuation` light properties embedded in the `positional` property describe the different attenuations. Spot light specific properties `falloffAngle` and `falloffExponent` properties are embedded inside the `spot` property.

```javascript
"extensions": {
    "KHR_lights" : {
        "lights": [
            {
                "positional": [
                    "constantAttenuation": 1.0,
                    "spot": [
                        "falloffAngle": 1.57079632679,
                        "falloffExponent": 0.0
                    ]
                ],
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
