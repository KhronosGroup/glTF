<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# CESIUM_gaussian_splatting

## Contributors

Jason Sobotka, Cesium
Aras P.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Gaussian splats are currently stored in mostly unstructured files. This extension is an attempt to bring them to glTF and offer a structured, efficient method for streaming and rendering them.

This method uses textures to store data for efficient GPU loading.

Data formats within textures:
 * Positions (R10G10B10A2 good default),
 * Rotations (always R10G10B10A2),
 * Scales (R5G6B5 good default),
 * Color+opacity (R8G8B8A8 is easy),

## Sample

Extending glTF node:

```
{
    "accessors": [
        {
            "type": "VEC3",
            "componentType": "FLOAT" // Enum
        },
        {
            "type": "VEC4",
            "componentType": "UNSIGNED_BYTE", // Enum
            "normalized": true
        },
        {
            "type": "VEC4", // quaternion
            "componentType": "FLOAT" // could be quantized with KHR_mesh_quantization + EXT_meshopt_compression
        },
        {
            "type": "VEC3",
            "componentType": "FLOAT" // could be quantized with KHR_mesh_quantization + EXT_meshopt_compression
        }
    ],
    "meshes": [
        {
            "primitives": [
                {
                    "mode": 0, // POINT CLOUD
                    "attributes": {
                        "POSITION": 0,
                        "COLOR_0": 1,
                        "_ROTATION": 2,
                        "_SCALE": 3
                    },
                    "extensions": {
                        "CESIUM_gaussian_splats": {
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

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.

## Implementation Notes

This doesn't specify any specific rendering techniques. This is purely for organizing the data in a structured manner.

## Resources

https://github.com/mkkellogg/GaussianSplats3D/issues/47#issuecomment-1801360116