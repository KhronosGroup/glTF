# CESIUM_RTC

## Contributors

* Patrick Cozzi, Cesium, [@pjcozzi](https://twitter.com/pjcozzi)
* Tom Fili, Cesium, [@CesiumFili](https://twitter.com/CesiumFili)

## Status

Stable

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

Massive world graphics applications have position vertex attributes with precision requirements that result in jittering artifacts when naively rendered with 32-bit floating-point values.  This extension introduces the metadata required to implement the _Relative To Center_ (RTC) high-precision rendering technique described by [[Ohlarik08](http://help.agi.com/AGIComponents/html/BlogPrecisionsPrecisions.htm)].

In this technique, each position is defined relative to an origin (the _center_) such that 32-bit floating-point precision is adequate to describe the distance between each position and the center.  These relative positions are stored in the glTF vertex data.  At runtime, the positions are transformed with a modified model-view matrix that makes the center relative to the eye.  This avoids 32-bit subtraction of large translation components on the GPU.

## glTF Schema Updates

This extension adds:
* A new `CESIUM_RTC` property to the `extensions` property of the top-level glTF object.  `CESIUM_RTC` contains one property: `center`, an array of three numbers (`x`, `y`, `z`) that define the center in an application-specific coordinate system.  For example, in virtual globe applications, this may be WGS84 coordinates.  In a spatial data structure, this may be the origin of the node containing the model.  See [CESIUM_RTC.schema.json](CESIUM_RTC.schema.json) and Listing 2 below.
* The `CESIUM_RTC_MODELVIEW` parameter semantic, which modifies the `MODELVIEW` semantic to mean the RTC model-view matrix.  This matrix can be computed as shown in Listing 1.  The `MODELVIEWINVERSETRANSPOSE` semantic is still appropriate for transforming normals since RTC only introduces a translation which does not affect normals.

**Listing 1**: Computing the RTC model-view matrix.
```javascript
// Transform the RTC center point into eye coordinates using double-precision on the CPU
var viewMatrix = new Matrix4(/* ... */);
var rtcCenter = new Cartesian3(/* ... */);
var rtcCenterEye = new Cartesian3();
Matrix4.multiplyByPoint(viewMatrix, rtcCenter, rtcCenterEye);

// Compute the RTC model-view matrix by replacing the translation with the center with eye coordinates
var modelViewMatrix = new Matrix4(/* ... */);
var modelViewRTC = Matrix4.clone(modelViewMatrix);
modelViewRTC[12] = rtcCenterEye.x;  // Column-major 4x4 matrix layout
modelViewRTC[13] = rtcCenterEye.y;
modelViewRTC[14] = rtcCenterEye.z;
```

**Listing 2**: Example glTF JSON.
```javascript
"extensions": {
    "CESIUM_RTC": {
        "center": [6378137.0, 0.0, 0.0]
    }
}
```

**Listing 3**: Example parameter with the `CESIUM_RTC_MODELVIEW` semantic.
```javascript
"techniques": {
    "technique0": {
        "parameters": {
            "modelViewMatrix": {
                "semantic": "CESIUM_RTC_MODELVIEW",
                "type": 35676
             },
             // ...
         }
     }
 }
```

### JSON Schema

See [CESIUM_RTC.schema.json](CESIUM_RTC.schema.json).

## Known Implementations

* Cesium ([code](https://github.com/AnalyticalGraphicsInc/cesium/blob/bgltf/Source/Scene/Model.js))

## Resources

* [Ohlarik08] Deron Ohlarik. [Precisions, Precisions](http://help.agi.com/AGIComponents/html/BlogPrecisionsPrecisions.htm). 2008.
