# KHR_lights_image_based

## Contributors

* Gary Hsu, Microsoft
* Mike Bond, Adobe
* Norbert Nopper, UX3D
* Alexey Knyazev
* `Please add or remove!`

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec, requires `KHR_texture_basisu` extension, interacts with `EXT_texture_bc6h` and `EXT_texture_astc_hdr` extensions.

## Overview

This extension provides the ability to define image-based lights in a glTF scene. Image-based lights consist of an environment map that represents specular radiance for the scene as well as irradiance information.

Many 3D tools and engines support image-based global illumination but the exact technique and data formats employed vary. Using this extension, tools can export and engines can import image-based lights and the result should be highly consistent. 

This extension specifies exactly one way to format and reference the environment map to be used. The goals of this are two-fold. First, it makes implementing support for this extension easier. Secondly, it ensures that rendering of the image-based lighting is consistent across runtimes.

A conforming implementation of this extension must be able to load the image-based environment data and render the PBR materials using this lighting.

## Defining an Image-Based Light

The `KHR_lights_image_based` extension defines a array of image-based lights at the root of the glTF and then each scene can reference one. Each image-based light definition consists of a single cubemap that describes the specular radiance of the scene, the l=2 spherical harmonics coefficients or another cubemap for diffuse irradiance, and also rotation, brighness factor, and offset values.

An example using SH for diffuse irradiance:

```json
"extensions": {
    "KHR_lights_image_based" : {
        "imageBasedLights": [
            {
                "rotation": [0, 0, 0, 1],
                "brightnessFactor": 1.0,
                "brightnessOffset": 0.0,
                "specularEnvironmentTexture": 0,
                "diffuseSphericalHarmonics": [
                    [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                    [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                    [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
                ]
            }
        ]
    }
}
```

An example using a cubemap for diffuse irradiance:

```json
"extensions": {
    "KHR_lights_image_based" : {
        "imageBasedLights": [
            {
                "rotation": [0, 0, 0, 1],
                "brightnessFactor": 1.0,
                "brightnessOffset": 0.0,
                "specularEnvironmentTexture": 0,
                "diffuseEnvironmentTexture": 1
            }
        ]
    }
}
```

## Specular BRDF integration and Irradiance Coefficients

This extension uses a prefiltered environment map to define the specular lighting utilizing the split sum approximation. More information:
- [Specular BRDF integration in Google Filament](https://google.github.io/filament/Filament.md.html#lighting/imagebasedlights/processinglightprobes)
- [Pre-Filtered Environment Map in Unreal Engine 4](https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf)

This extension can use spherical harmonic coefficients to define irradiance used for diffuse lighting. Coefficients are calculated for the first 3 SH bands (l=2) and take the form of a 9x3 array. More information:
- [Realtime Image Based Lighting using Spherical Harmonics](https://metashapes.com/blog/realtime-image-based-lighting-using-spherical-harmonics/)
- [An Efficient Representation for Irradiance Environment Maps](http://graphics.stanford.edu/papers/envmap/)

## Adding Light Instances to Scenes

Each scene can have a single IBL light attached to it by defining the `extensions` and `KHR_lights_image_based` property and, within that, an index into the `imageBasedLights` array using the `imageBasedLight` property.

```json
"scenes" : [
    {
        "extensions" : {
            "KHR_lights_image_based" : {
                "imageBasedLight" : 0
            }
        }
    }            
]
```

## Shader Code Implementation

```
finalSampledColor = sampledColor * brightnessFactor + brightnessOffset;
```

## Image Encoding

### Texture Restrictions

- A glTF `texture` used for IBL cannot have a `KHR_texture_transform` extension defined.
- `specularEnvironmentTexture` and `diffuseEnvironmentTexture` (when used) must refer to a texture referring to a KTX2 image of **Cubemap** type as defined in KTX2, Section 4.3. Namely:
  - `pixelHeight` must be greater than 0.
  - `pixelDepth` must be 0.
  - `layerCount` must be 0.
  - `faceCount` must be 6.
- The image must contain mip levels.
- The transfer function for all image formats must be linear.
- Cubemaps orientation must be aligned with [glTF coordinate system](../../../../specification/2.0#coordinate-system-and-units). 

### Normal Quality

Data must be stored as a KTX2 image with Basis Universal supercompression and with alpha channel conforming to RGBD (or RGBE, **TBD**) encoding.

### High Quality (Optional)

Data can be stored as a KTX2 image with BC6H or ASTC HDR payload and linked via `EXT_texture_bc6h` or `EXT_texture_astc_hdr` extensions respectively.

Future extensions may allow additional formats by extending `texture` objects in a similar way.

Assets expected to be publicly released should always provide a normal quality fallback.

### Example with three encodings for the same texture

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_lights_image_based", "KHR_texture_basisu", "KHR_image_ktx2", "EXT_texture_bc6h", "EXT_texture_astc_hdr"
    ],
    "extensionsRequired": [
        "KHR_lights_image_based", "KHR_texture_basisu", "KHR_image_ktx2"
    ],
    "textures": [
        {
            "name": "Specular Environment Texture",
            "extensions": {
                "KHR_texture_basisu": {
                    "source": 0
                },
                "EXT_texture_bc6h": {
                    "source": 1
                },
                "EXT_texture_astc_hdr": {
                    "source": 2
                }
            }
        }
    ],
    "images": [
        {
            "uri": "image_basisu.ktx2"
        },
        {
            "uri": "image_bc6h.ktx2"
        },
        {
            "uri": "image_astc.ktx2"
        }
    ]
}
```

## glTF Schema Updates

* **JSON schema**: 
- [glTF.KHR_lights_image_based.schema.json](schema/glTF.KHR_lights_image_based.schema.json)
- [imageBasedLight.schema.json](schema/imageBasedLight.schema.json)
- [scene.KHR_lights_image_based.schema.json](schema/scene.KHR_lights_image_based.schema.json)

## Known Implementations

None.