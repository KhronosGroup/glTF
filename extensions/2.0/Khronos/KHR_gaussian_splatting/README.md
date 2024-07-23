<!--
Copyright 2015-2024 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# KHR_gaussian_splatting

## Contributors

- Jason Sobotka, Cesium
- Adam Morris, Cesium
- Sean Lilley, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Gaussian splats are currently stored in mostly unstructured files such as PLY. This aims to bring that format into glTF in a simple and straightfoward way. The position, rotation, scale, and diffuse color are stored as standard attributes on a point primitive. If the point primitive contains the extension the renderer can know to render the point primitive as Gaussian Splats instead of a points.

This approach allows for an easy fallback in the event the glTF is loaded within a renderer that doesn't support Gaussian Splats. In this scenario, the glTF file will render as a sparse point cloud to the user.

### Splat Data Mapping

| Splat Data | glTF Attribute |
| --- | --- |
| Position | POSITION |
| Color (Diffuse, Spherical Harmonic 0) | COLOR_0 RGB channels |
| Opacity (Spherical Harmonic 0 Alpha) | COLOR_0 A channel |
| Rotation | _ROTATION |
| Scale | _SCALE |

Spherical Harmonic channels 1 through 15, which map the splat specular, are currently unused by the extension.

## Sample

Extending glTF node:

```
{
    "accessors": [
        {
            "type": "VEC3",
            "componentType": 5126 // FLOAT
        },
        {
            "type": "VEC4",
            "componentType": 5121, // UNSIGNED_BYTE
            "normalized": true
        },
        {
            "type": "VEC4", // quaternion
            "componentType": 5126 // FLOAT, could be quantized with KHR_mesh_quantization + EXT_meshopt_compression
        },
        {
            "type": "VEC3",
            "componentType": 5126 // FLOAT, could be quantized with KHR_mesh_quantization + EXT_meshopt_compression
        }
    ],
    "meshes": [
        {
            "primitives": [
                {
                    "mode": 0, // POINTS
                    "attributes": {
                        "POSITION": 0,
                        "COLOR_0": 1,
                        "_ROTATION": 2,
                        "_SCALE": 3
                    },
                    "extensions": {
                        "KHR_gaussian_splatting": {
                            // May need to consider spherical harmonics in the future. Separate extension?
                            // Quantization scale for additional attributes goes here if needed
                        }
                    }
                }
            ]
        }
    ]
}
```

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://github.com/CesiumGS/cesium/tree/splat-shader).

## Resources

https://github.com/mkkellogg/GaussianSplats3D/issues/47#issuecomment-1801360116