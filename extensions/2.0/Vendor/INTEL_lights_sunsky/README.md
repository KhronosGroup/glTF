# INTEL_lights_sunsky

## Contributors

-   Johannes Günther, Intel, [@johguenther](https://github.com/johguenther)
-   Bruce Cherniak, Intel, [@BruceCherniak](https://github.com/BruceCherniak)
-   Isha Sharma, Intel, [@isharma25](https://github.com/isharma25)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines parameters for a procedural sun-sky model, an
environmental light source, which provides natural illumination for the
scene and which is optionally visible in the background.

<figure>
<img src="./sunsky.png" alt="Sun and sky at day and at dusk." />
<figcaption aria-hidden="true">Two different settings of the sunsky
light, affecting illumination and background; left: clear sky at day,
right: hazy sky at dusk.</figcaption>
</figure>

## Specifying the Sunsky Light

```json
"extensions": {
    "INTEL_lights_sunsky": {
        "lights": [
            {
                "visible": true,
                "intensity": 0.025,
                "elevation": 1.1,
                "azimuth": 2.5,
                "turbidity": 2.0,
                "albedo": 0.3,
                "horizonExtension": 0.01
            }
        ]
    }
}
```
A sunsky light is instantiated by referencing it by a `node`, which also
allow for rotation and orientation of the light (e.g., placing the
zenith at +Z in the world).

```json
"nodes" : [
    {
        "extensions" : {
            "INTEL_lights_sunsky" : {
                "light" : 0
            }
        }
    }            
]
```

## Properties

No property is required, all have default values.

| Property   | Description  | Default |
|------------|--------------|---------|
| `visible`  | whether seen in the background | `true` |
| `intensity`| brightness scaling factor | `0.025` to bring the intrinsic radiance into “normal” dynamic range |
| `elevation`| angle (in `-pi/2`..`pi/2`) to the horizon, i.e., to the plane with normal +Y (the zenith) | `pi/2` |
| `azimuth`  | angle (in `0`..`2pi`) to +X in the horizon plane; increasing azimuth is a counterclockwise rotation around +Y | `0` |
| `turbidity`| atmospheric turbidity due to particles, between `1` (clear sky) and `10` (hazy)| `3` |
| `albedo`   | ground reflectance, in `0`..`1` | `0.3` |

Note that the `azimuth` angle is redundant to a rotation of the node the
sunsky light is attached to, it is supported for convenience.

<!--
## glTF Schema Updates

TODO

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.
-->

## Known Implementations

-   Intel [OSPRay Studio](https://www.ospray.org/ospray_studio/), since
    v0.11.0

## Resources

-   Lukáš Hošek, Alexander Wilkie, “[An Analytic Model for Full Spectral
    Sky-Dome
    Radiance](https://cgg.mff.cuni.cz/projects/SkylightModelling/)”, ACM
    Transactions on Graphics (Proceedings of ACM SIGGRAPH 2012), 31(4),
    2012
-   Alexander Wilkie, Petr Vevoda, Thomas Bashford-Rogers, Lukáš Hošek,
    Tomáš Iser, Monika Kolářová, Tobias Rittig, and Jaroslav Křivánek,
    “[A Fitted Radiance and Attenuation Model for Realistic
    Atmospheres](https://doi.org/10.1145/3450626.3459758)”, ACM
    Transactions on Graphics (Proceedings of SIGGRAPH 2021),
    40(4), 2021.
