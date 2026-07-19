# KHR\_data\_sizes

## Contributors

* Mike Bond, Adobe, [@MiiBond](https://twitter.com/MiiBond)

## Status

WIP

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines the expected data size of the glTF asset; both packed (i.e. file/download size) and unpacked (i.e. in-memory). This is useful for selectively loading parts of the asset based on application needs (e.g. device capabilities, bandwidth constraints, etc.). Data sizes are divided by asset category; textures, geometry and animations, and then further divided by other factors that may affect data size such as extension availability.

The data size extension is defined at the root level of the glTF asset.

```json
{
    "extensions": {
        "KHR_data_sizes": {
            "download": {
                "images": {
                    "1024_max": 12345,
                    "2048_max": 12345,
                    "KHR_basis": {
                        "1024_max": 12345, 
                        "2048_max": 12345
                    }
                },
                "geometry": { 
                    "MSFT_lod": {
                        "LOD0": {
                            "KHR_mesh_draco": 12345,
                            "default": 12345
                        }
                    },
                    "KHR_mesh_draco": 12345, 
                    "default": 12345
                },
                "animation": 12345
            },
            "memory": {
                "images": { 
                    "KHR_basis": {
                        "1024_max": 12345, 
                        "2048_max": 12345
                    },
                    "default": {
                        "1024_max": 12345,
                        "2048_max": 12345
                    }
                },
                "geometry": 12345,
                "animation": 12345
            }
        }
    }
}
```

## Properties

### Download

This section describes how data contributes to file size. This is most likely important to applications that need to transfer the data over a network connection and might want to selectively download based on capabilities or bandwidth. e.g. if the GLB contains compressed and uncompressed geometry, the viewer can select to download only the uncompressed if it doesn't support the needed extension (KHR_mesh_draco or KHR_mesh_quantization). Or, the runtime might choose to download only the first LOD (MSFT_lod).
This section is divided by asset type; images, geometry and animation.

### Memory

This section describes how much GPU memory the data consumes. Knowing this can be critical, especially with heavy assets on low-end devices. For example, before uploading textures to the GPU, a viewer can shrink images as appropriate or discard high-res mips from a KTX file.
This section is divided by asset type; images, geometry and animation.

## Notes about GLB

Since GLB is a single, binary blob, you might think embedding this information is of no use since you can't read it until after downloading the whole package. However, if the server hosting the GLB accepts range-requests, runtimes can load only the manifest for the GLB first and then subsequently load whatever data is needed from inside the GLB. 
