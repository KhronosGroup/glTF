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
This extension defines two area light shapes: `rect` (rectangle) and `disk` (circle). These lights are referenced by nodes and inherit the transform of that node, allowing them to be placed in a scene.

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
                "shape": "rect",
                "width": 2.0,
                "height": 1.0
            },
            {
                "color": [1.0, 0.9, 0.8],
                "intensity": 1500.0,
                "shape": "disk",
                "radius": 1.2
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

## Behaviour

The light will inherit the transform of the node that it is attached to. Area lights are centered at the origin in local space:

* **Rectangular lights**: extend from -width/2 to +width/2 along the local X-axis, and from -height/2 to +height/2 along the Y-axis
* **Disk lights**: extend in a circle of the specified radius 
around the origin in the local XY-plane

Area lights emit light uniformly from one side of the surface (the side facing in the -Z direction in local space). The light emission follows Lambert's cosine law.

**Lambert's Cosine Law**: This law states that the radiant intensity (or luminous intensity) observed from a perfectly diffusing surface is directly proportional to the cosine of the angle between the direction of the incident light and the surface normal. In practical terms, this means that an area light appears brightest when viewed straight-on (perpendicular to the surface) and dimmer as the viewing angle increases, creating realistic soft lighting that mimics real-world diffuse light sources like LED panels or softboxes.

## Properties

### Light Properties

| Property | Type | Description | Required | Default |
|:---------|:-----|:------------|:---------|:--------|
| `color` | `number[3]` | RGB color of the light in linear space. Each component should be in the range [0, 1]. | No | `[1.0, 1.0, 1.0]` |
| `intensity` | `number` | The luminance of the light surface in nits (cd/m²). Must be a positive value. | No | `1000.0` |
| `shape` | `string` | The shape of the area light. Must be either `"rect"` or `"disk"`. | Yes | - |
| `width` | `number` | Width of the rectangular light in meters. Must be a positive value. Required for `rect` lights, ignored for `disk` lights. | Conditional | `1.0` |
| `height` | `number` | Height of the rectangular light in meters. Must be a positive value. Required for `rect` lights, ignored for `disk` lights. | Conditional | `1.0` |
| `radius` | `number` | Radius of the disk light in meters. Must be a positive value. Required for `disk` lights, ignored for `rect` lights. | Conditional | `1.0` |

### Node Extension Properties

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `light` | `integer` | Index of the light in the lights array. | Yes |

## Implementation Notes

### Luminance Units

The `intensity` property is specified in nits (cd/m²), which is the standard unit for surface luminance. This provides physically accurate lighting values:

* Typical indoor lighting: 100-500 nits
* Bright office lighting: 1,000 nits  
* Sunlit white paper: 30,000 nits
* Direct sunlight: 1,000,000,000+ nits

### Light Emission

Area lights emit light uniformly across their surface following Lambert's cosine law. The total luminous flux (in lumens) emitted by the light is:

```
Flux = luminance × area × π
```

Where:

* `luminance` is in nits (cd/m²)
* `area` is the surface area of the light in square meters:
  * For rectangular lights: `area = width × height`
  * For disk lights: `area = π × radius²`
* The result is in lumens

### Transform Inheritance

The light inherits the full transform of its parent node, including:

* Position (translation)
* Orientation (rotation) 
* Scale

If the node is scaled:

* For rectangular lights: the width and height are scaled accordingly
* For disk lights: the radius is scaled by the average of the X and Y scale factors
* The intensity value remains constant, but the total emitted flux will change proportionally to the change in area

## JSON Schema

* [light.EXT_lights_area.schema.json](schema/light.area.schema.json)