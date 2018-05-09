# CESIUM_z_up

## Contributors

* Gabby Getz, Cesium, [@gabbygetz](https://twitter.com/gabbygetz)
* Sean Lilley, Cesium, [@lilleyse](https://twitter.com/lilleyse)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The [glTF coordinate system](../../../../specification/2.0#coordinate-system-and-units) a right-handed 3-axis (x, y, z) Cartesian coordinate system where the positive _y_-axis as up. However, different 3D tools or engines may define different directions for the _y_ and _z_-axis. This extension resolves ambiguity when orienting a _y_-up glTF asset in a pipeline with a coordinate system where the positive _z_-axis is up.

There is precedent for a _z_-up coordinate system, in particular for use in a 3D geospatial context. Globe engines and 3D models often use global [earth-centered, earth-fixed (ECEF)](https://www.e-education.psu.edu/geog862/node/1793) reference frame, whose origin is the Earth's center and the _z_-axis is the north pole. Furthermore, when translating from a standard geodetic coordinate system with latitude, longitude, and height (such as [WGS 84](https://epsg.io/4326)) into 3-axis Cartesian coordinate system, the _x_ and _y_ axes are congruent with latitude and longitude, and the _z_-axis with height.

In addition to Cesium, other 3D applications such as 3DS Max, SketchUp, and Unreal engine treat the _z_-axis as up.

When loading a glTF asset wth the `CESIUM_z_up` extension, an implementation must treat the mesh geometry as _z_-up, applying a transformation to the node hierarchy if needed.

To transform an asset with the `CESIUM_z_up` extension with positions in _z_-up to a _y_-up coordinate system at runtime, rotate the model about the _x_-axis by -&pi;/2 radians. Equivalently, apply the following matrix transform to the node hierarchy:
```json
[
1.0,  0.0, 0.0, 0.0,
0.0,  0.0, 1.0, 0.0,
0.0, -1.0, 0.0, 0.0,
0.0,  0.0, 0.0, 1.0
]
```

## glTF Schema Updates

List the `CESIUM_z_up` extension in the asset's top level `extensionsRequired` and `extensionsUsed` properties.

```json
{
    "extensionsRequired": [
        "CESIUM_z_up"
    ],
    "extensionsUsed": [
        "CESIUM_z_up"
    ]
}
```
