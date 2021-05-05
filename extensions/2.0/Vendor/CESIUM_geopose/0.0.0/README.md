<!-- omit in toc -->
# CESIUM_geopose

**Version 0.0.0, May 17, 2021

<!-- omit in toc -->
## Contributors

* Sam Suhag, Cesium
* Sean Lilley, Cesium
* Peter Gagliardi, Cesium

## Status

Draft

## Dependencies

Written against the [OGC GeoPose Standard 1.0](https://github.com/opengeospatial/GeoPose/tree/main/standard).

## Optional vs. Required

This extension is optional, meaning it should be placed in the `extensionsUsed` list, but not in the `extensionsRequired` list.

## Overview

This extension to glTF enables static positioning and orienting of models on the Earth.

### GeoPose 1.0 Standard

GeoPose 1.0 is an OGC Implementation Standard for exchanging the location and orientation of real or virtual geometric objects (*Poses*) within reference frames anchored to the earth’s surface (*Geo*) or within other astronomical coordinate systems.

This extension implements [Standardization Target 2: Basic-Euler](https://github.com/opengeospatial/GeoPose/blob/main/standard/standard/standard/clause_7_normative_text.adoc#standardization-target-2-basic-euler) in the OGC GeoPose 1.0 Standard.

## Coordinate Systems

This extension uses WGS84([EPSG:4979](https://epsg.io/4979)) as the coordinate reference system for specifying the position with longitude and latitude specified in degrees. Height above (or below) the ellipsoid must be specified in meters.

```json
{
  "extensions": {
    "CESIUM_geopose": {
      "longitude": 46.7,
      "latitude": 25.067,
      "height": 691.0,
      "ypr": {
        "yaw": 0.0,
        "pitch": 0.0,
        "roll": 0.0
      }
    }
  }
}
```

## Schema Updates

Updates to the schema can be found in the [extension schema](schema/gltf.CESIUM_geopose.schema.json).