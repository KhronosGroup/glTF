# EXT_lights_ies

## Contributors

* Timothy Blut, NVIDIA, tblut@nvidia.com
* Lutz Kettner, NVIDIA, lkettner@nvidia.com
* Paul Arden, PTC, parden@ptc.com

## Status

Draft
<!--TODO: Draft or Stable-->

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension enables the use of IES light profiles as light sources within a scene. IES light profiles describe a light's distribution using real-world measured data from light sources.

Many 3D tools and engines support IES light profiles. Using this extension, tools can export and engines can import scenes containing lights described by IES light profiles.

These lights are referenced by nodes and inherit their transform. This also allows changing the light's orientation.

A conforming implementation of this extension must be able to load the light profiles defined in the asset and render the asset using those lights. The following IES light profile standards need to be supported: `IES LM-63-95`, `ANSI/IESNA LM-63-02`, and `ANSI/IES LM-63-19`. Please see the [implementation details](#implementation-details) for a list of fields that are not required and a description of the relevant differences between the aforementioned standard versions.

## Defining Light Profiles

Light profiles are defined within the array `lights` which is defined in the `extensions.KHR_lights_ies` property located in the top-level object of the asset.

Each light profile defines either the `uri` property, which contains the URI (or IRI) of an IES file, or the `bufferView` property, which points to a bufferView object that contains an IES light profile file. If the `bufferView` property is used then `mimeType` must also be defined and have the value `application/x-ies-lm-63`.

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

Light profiles can be attached to a node by defining the `extensions.KHR_lights_ies` property and, within that, the `light` property which defines an index into the `lights` array. Optionally, the `multiplier` and `color` properties can be defined to scale the light's intensity and tint the light, respectively. The `color` property acts as a per-wavelength multiplier.

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

The light will inherit the transform of the node. The light's direction is defined as the 3-vector `(0.0, 0.0, -1.0)` and the rotation of the node orients the light accordingly. That is, an untransformed light points down the -Z axis. The light's transform is affected by the node's world scale, but the light's intensity is unaffected.

### Light Profile Instance Properties

| Property | Description | Required |
|:---------|:------------|:---------|
| `light` | Index of the light profile. | :white_check_mark: Yes |
| `multiplier` | Non-negative factor to scale the light's intensity. | No, Default: `1.0` |
| `color` | RGB value for the light's color in linear space. | No, Default: `[1.0, 1.0, 1.0]` |

## Coordinate System Mapping

The light source measurement sample locations on the spherical photometric web are defined by pairs of vertical and horizontal angles. The zero point `(0.0, 0.0)`, the point at which both angles are zero and the measured light source is generally aimed at, is mapped to the direction `(0.0, 0.0, -1.0)`. That is, the untransformed light points in the direction of the glTF coordinate system's -Z axis.

The mapping of the +X axis depends on the photometric type of the light profile. The following points on the spherical photometric web are in the notation used in `IES LM-75-01/R12` and denote the points to which the +X axis is aimed at for each photometric type:

* Type A: `(0Y, 90X)`
* Type B: `(90H, 0V)`
* Type C: `(90V, 270L)`

Lastly, the +Y axis can be computed as the cross product between the +X and -Z axes. 

Thus, the X/Y plane is the horizontal photometric plane and the X/Z plane is the vertical photometric plane for all angular data specified. Note that for light profiles of `Type A` and `Type B` photometry the horizontal angles are specified in clockwise order, while positive rotation in the glTF coordinate system is counter-clockwise.

## Implementation Details

The following IES light profile fields are not required to be supported:

* Any keyword data listed after the first line and before the `TILT` section.
* The `TILT` section (`TILT=INCLUDE`, `TILT=<filename>` and related values).
* Light profiles of `Type A` photometry.
* `<number of lamps>`, `<lumens per lamp>`, `<units type>`, `<width>`, `<length>`, `<height>`, `<file generation type>`, and `<input watts>`.

Furthermore, implementations should be aware of the following differences between IES standard versions:

* The first line of the IES file is either `IESNA:LM-63-1995`, `IESNA:LM-63-2002`, or `IES:LM-63-2019`, corresponding to the respective standard version.
* In the standard version `IES LM-63-95`, it is valid for light profiles of `Type C` photometry to have horizontal angles starting at 90 degrees and ending at 270 degrees.

## JSON Schema

* [glTF.EXT_lights_ies.schema.json](schema/glTF.EXT_lights_ies.schema.json)
* [node.EXT_lights_ies.schema.json](schema/node.EXT_lights_ies.schema.json)
* [light.schema.json](schema/light.schema.json)

## Known Implementations

* [NVIDIA Omniverse](https://www.nvidia.com/en-us/omniverse/)

## Resources

* [IES LM-63-19 (IES Standard File Format for Electronic Transfer of Photometric Data and Related Information), IES Computer Committee](http://www.iesna.org)
* [IES LM-75-01/R12 (Goniophotometer Types and Photometric Coordinates), IES Computer Commitee](http://www.iesna.org)
