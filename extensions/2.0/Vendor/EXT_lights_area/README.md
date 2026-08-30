# EXT_lights_area

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines area lights with physically accurate intensity values for use with glTF 2.0.

Area lights are flat surfaces that uniformly emit light from one side (the side facing in the -Z direction in local space).
This extension defines two area light types: `rect` (rectangle) and `disk` (circle). These lights are referenced by nodes and inherit the transform of that node, allowing them to be placed in a scene.

## Defining Area Lights

As with KHR_lights_punctual, area lights are defined as a `lights` array inside the `EXT_lights_area` extension at the root of the document.

The following example defines both a rectangular and a disk area light:

```json
"extensions": {
    "EXT_lights_area": {
        "lights": [
            {
                "color": [1.0, 1.0, 1.0],
                "intensity": 1000.0,
                "type": "rect",
                "size": 1.0,
                "rect": {
                    "aspect": 2.0
                }
            },
            {
                "color": [1.0, 0.9, 0.8],
                "intensity": 1500.0,
                "type": "disk",
                "size": 1.2
            }
        ]
    }
}
```

## Adding Light Instances to Nodes

Lights must be attached to a node by defining the `extensions.EXT_lights_area` property and, within that, an index into the `lights` array using the `light` property.

```json
"nodes": [
    {
        "extensions": {
            "EXT_lights_area": {
                "light": 0
            }
        }
    }
]
```

The light will inherit the transform of the node in the following manner:
1. The light's centre is defined as the node's world location.
2. The light's direction is defined as the 3-vector `(0.0, 0.0, -1.0)` and the rotation of the node orients the light accordingly. That is, an untransformed light points down the -Z axis.
3. The light's size is defined as its "size" property multiplied by the absolute value of the largest component of the node's world scale, i.e., the scaling is always uniform and positive.

## Light Types

### Light Shared Properties

All light types share the common set of properties listed below.

| Property | Type | Description | Required | Default |
|:---------|:-----|:------------|:---------|:--------|
| `name` | `string` | Name of the light. | No | Default: `""` |
| `color` | `number[3]` | RGB value for the light's color in linear space. | No | `[1.0, 1.0, 1.0]` |
| `intensity` | `number` | The luminance of the light surface in nits (cd/m²). Must be a positive value. | No | `1000.0` |
| `type` | `string` | Declares the type of the light. | :white_check_mark: Yes | `rect` |
| `size` | `number` | Defines the size of the light in local space. For `rect` lights, this is the dimension along the local y-axis. For `disk` lights, this is the diameter of the light. | No | `1.0` |

### Rectangle Lights

When a light's `type` is `rect`, the `rect` property contains the following optional properties.

| Property | Type | Description | Required | Default |
|:---------|:-----|:------------|:---------|:--------|
| `aspect` | `number` | Defines the relative size of the width of the rectangle compared to the height. | No | `1.0` |

The rectangle area light's shape extends from -width/2 to +width/2 along the local X-axis, and from -height/2 to +height/2 along the Y-axis where width and height are defined as follows:

`width = world scale * size * aspect` \
`height = world scale * size`

`world scale` is the largest component of the absolute value of the node's scale.

### Disk Lights

When a light's `type` is `disk`, the light is circular and no additional properties are defined.

The disk light's shape extends in a circle around the origin in the local XY-plane. The diameter is given by `world scale * size` where `world scale` is the largest component of the absolute value of the node's scale.

## Light Emission

Area lights emit light uniformly from one side of the surface (the side facing in the -Z direction in local space). The light emission follows Lambert's cosine law. The total luminous flux (in lumens) emitted by the light is:

`Flux = intensity × area × π`

Where:

* `intensity` is in nits (cd/m²)
* `area` is the surface area of the light in square meters:
  * For rectangular lights: `area = width × height`
  * For disk lights: `area = π × radius²`

The resulting flux is measured in lumens. Notice that when the intensity value remains constant, the total emitted flux will change proportionally to the change in area.

### Luminance Units

The `intensity` property is specified in nits (cd/m²), which is the standard unit for surface luminance. This provides physically accurate lighting values:

* Typical indoor lighting: 100-500 nits
* Bright office lighting: 1,000 nits  
* Sunlit white paper: 30,000 nits
* Direct sunlight: 1,000,000,000+ nits

## JSON Schema

* [light.EXT_lights_area.schema.json](schema/light.area.schema.json)