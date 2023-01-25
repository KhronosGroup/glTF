# EXT_lights_ies

## Contributors

* Timothy Blut, NVIDIA, tblut@nvidia.com
* Lutz Kettner, NVIDIA, lkettner@nvidia.com
* Paul Arden, PTC, parden@ptc.com

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension enables the use of IES light profiles as light sources within a scene. IES light profiles describe a light's distribution using real-world measured data from light sources.

Many 3D tools and engines support IES light profiles. Using this extension, tools can export and engines can import scenes containing lights described by IES light profiles.

The light profiles are referenced by nodes and inherit the transform of that node. They describe point light sources within the scene. These light sources are defined as parameterized, infinitely small points that emit light in all directions with angularly varying intensities.

A conforming implementation of this extension must be able to load the light profiles defined in the asset and render the asset using those lights. The following IES light profile standards need to be supported: `IES LM-63-95`, `ANSI/IESNA LM-63-02`, and `ANSI/IES LM-63-19`. Implementations must support light profiles of `Type B` and `Type C` photometry. Support for light profiles of `Type A` photometry is optional. Please see the [implementation details](#implementation-details) for a list of fields that are not required and a description of the relevant differences between the aforementioned standard versions.

## Defining Light Profiles

Light profiles are defined within the array `lights` which is defined in the `extensions.EXT_lights_ies` property located in the top-level object of the asset.

Each light profile defines either the `uri` property, which contains the URI (or IRI) of an external IES file or a data-URI with embedded data, or the `bufferView` property, which points to a bufferView object that contains an IES light profile file. If the `bufferView` property is used then `mimeType` must also be defined and have the value `application/x-ies-lm-63`.

```javascript
"extensions": {
    "EXT_lights_ies" : {
        "lights": [
            {
                "uri": "light_profile.ies"
            },
            {
                "bufferView": 1,
                "mimeType": "application/x-ies-lm-63"
            }
        ]
    }
}
```

### Light Profile Properties

| Property | Description | Required |
|:---------|:------------|:---------|
| `uri` | The URI (or IRI) of the light profile. | No |
| `bufferView` | The index of the bufferView that contains the IES light profile. | No |
| `mimeType` | The light profile's media type. Must be `"application/x-ies-lm-63"`. | No |
| `name` | glTF name of the light profile. | No, Default: `""` |

## Adding Light Profile Instances to Nodes

Light profiles can be attached to a node by defining the `extensions.EXT_lights_ies` property and, within that, the `light` property which defines an index into the `lights` array. Optionally, the `multiplier` and `color` properties can be defined to scale the light's intensity and tint the light, respectively. The RGB value of the `color` property is in linear space, clamped to the `[0, 1]` range, and acts as a per color channel multiplier. Note that values other than `1.0` for `multiplier` and `(1.0, 1.0, 1.0)` for `color` change the energy output of the light source. An energy preserving tint can be achieved by modifying `multiplier` to compensate for the energy loss: `multiplier *= 3 / dot(color, color)`.

```javascript
"nodes": [
    {
        "extensions": {
            "EXT_lights_ies" : {
                "light": 0
            }
        }
    }
]
```

The light will inherit the transform of the node. The light's direction is defined as the 3-vector `(0.0, 0.0, -1.0)` and the rotation of the node orients the light accordingly. That is, an untransformed light points down the -Z axis. The light's intensity is unaffected by the node's global scale. The brightness of the light attenuates in a physically correct manner as distance increases from the light's position (i.e. brightness is inversely proportional to the square of the distance).

### Light Profile Instance Properties

| Property | Description | Required |
|:---------|:------------|:---------|
| `light` | Index of the light profile. | :white_check_mark: Yes |
| `multiplier` | Non-negative factor to scale the light's intensity. | No, Default: `1.0` |
| `color` | RGB value for the light's color in linear space. | No, Default: `[1.0, 1.0, 1.0]` |

## Coordinate System Mapping

The light source measurement sample locations on the spherical photometric web are defined by pairs of vertical and horizontal angles. The zero point `(0.0, 0.0)`, the point at which both angles are zero and the measured light source is generally aimed at, is mapped to the direction `(0.0, 0.0, -1.0)`. That is, the untransformed light points in the direction of the glTF coordinate system's -Z axis.

Thus, the X/Y plane is the horizontal photometric plane and the X/Z plane is the vertical photometric plane for all angular data specified. Note that for light profiles of `Type A` and `Type B` photometry the horizontal angles are specified in clockwise order, while positive rotation in the glTF coordinate system is counter-clockwise.

## Implementation Details

The following IES light profile fields are not required to be supported:

* Any keyword data listed after the first line and before the `TILT` section.
* The `TILT` section (`TILT=INCLUDE`, `TILT=<filename>` and related values).
* `<number of lamps>`, `<lumens per lamp>`, `<units type>`, `<width>`, `<length>`, `<height>`, `<file generation type>`, and `<input watts>`.

Furthermore, implementations should be aware of the following differences between IES standard versions:

* The first line of the IES file is either `IESNA:LM-63-1995`, `IESNA:LM-63-2002`, or `IES:LM-63-2019`, corresponding to the respective standard version.
* In the standard version `IES LM-63-95`, it is valid for light profiles of `Type C` photometry to have horizontal angles starting at 90 degrees and ending at 270 degrees.

## JSON Schema

* [glTF.EXT_lights_ies.schema.json](schema/glTF.EXT_lights_ies.schema.json)
* [node.EXT_lights_ies.schema.json](schema/node.EXT_lights_ies.schema.json)
* [lightProfile.schema.json](schema/lightProfile.schema.json)

## Known Implementations

* [NVIDIA Omniverse](https://www.nvidia.com/en-us/omniverse/)
* [NVIDIA Material Definition Language SDK](https://github.com/NVIDIA/MDL-SDK)

## Resources

* [IES LM-63-19 (IES Standard File Format for Electronic Transfer of Photometric Data and Related Information), IES Computer Committee](https://store.ies.org/product/lm-63-19-approved-method-ies-standard-file-format-for-the-electronic-transfer-of-photometric-data-and-related-information/)
