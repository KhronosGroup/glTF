
# KHR_lights_environment

## Contributors

* Norbert Nopper, <mailto:nopper@ux3d.io>
* Rickard Sahlin, <mailto:rickard.sahlin@ikea.com>
* Gary Hsu, Microsoft, <mailto:garyhsu@microsoft.com>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview
This extension provides the ability to define image-based lights in a glTF scene.  
Image-based lights consist of an environment map that represents specular radiance for the scene as well as irradiance information.  
This can be used on it's own - ie a glTF asset with only environment map data - for a usecase where the IBL needs to be distributed.  
It can also be used together with model data, for usecases where a model shall be displayed in a defined environment.  

Many 3D tools and engines support image-based global illumination but the exact technique and data formats employed vary.  
Using this extension, tools can export and engines can import image-based lights and the result should be highly consistent.   

This extension specifies exactly one way to format and reference the environment map to be used.  
The goals of this are two-fold.  
First, it makes implementing support for this extension easier.  
Secondly, it ensures that rendering of the image-based lighting is consistent across runtimes.

A conforming implementation of this extension must be able to load the image-based environment data and render the PBR materials using this lighting.  

This extension is based on EXT_lights_image_based, with added support for KTX2 as a carrier for cubemap and HDR texture formats.  

The environment light is defined by a cubemap.  
Cubemaps can be supplied pre-filtered, by including the pre-filtered mip-levels, otherwise filtering will be performed at load time.  
[TBD - point to relevant filtering algorithm]  
If a compressed texture format is used then pre-filtered mip-levels must be included.  

## Declaring an environment light

The KHR_lights_environment extension defines an array of image-based lights at the root of the glTF and then each scene can reference one.  
Each environment light definition consists of a single cubemap that describes the specular radiance of the scene, the l=2 spherical harmonics coefficients for diffuse irradiance and intensity values.  
The cubemap is defined by texture references to KTX2 files containing a cubemap, these files can contain compressed textures using KHR_texture_basisu.  
KTX2 files may use the following formats for HDR support:  
VK_FORMAT_B10G11R11_UFLOAT_PACK32  
VK_FORMAT_E5B9G9R9_UFLOAT_PACK32  
VK_FORMAT_BC6H_UFLOAT_BLOCK  

If image source contains a cubemap pyramid it must be pre-filtered and contain the needed mip-levels see Specular Radiance Cubemaps.  

```json
"extensions": {
    "KHR_lights_environment" : {
        "lights": [
            {
                "intensity": 1.0,
                "irradianceCoefficients": [...3 x 9 array of floats...],
                "specularCubemaps": [... Reference to KTX2 cubemaps...],
            }
        ]
    }
}
```

## Specular Radiance Cubemaps

The cubemap used for specular radiance is defined as separate images for each cube face. 
The mip levels shall evenly map to roughness values from 0 to 1 in the PBR material and should be generated with a principled multi-scatter GGX normal distribution. The data in the maps represents illuminance in candela per square meter (nit).

If mip-levels are not included, then the entire mip chain of images should not be generated.
Instead, the lowest-resolution mip should have sufficient size to represent the maximally-blurred radiance map (say, 16x16) corresponding to roughness=1. The texture references defines the largest dimension of mip 0 and, taken together with the number of defined mips, should give the loading runtime the information it needs to generate the remainder of the mip chain and sample the appropriate mip level in the shader.

Cube faces are defined in the KTX2 format specification and this extension adheres this.  


## Irradiance Coefficients

This extension uses spherical harmonic coefficients to define irradiance used for diffuse lighting.  
Coefficients are calculated for the first 3 SH bands (l=2) and take the form of a 9x3 array.  
[Realtime Image Based Lighting using Spherical Harmonics](https://metashapes.com/blog/realtime-image-based-lighting-using-spherical-harmonics/)
[An Efficient Representation for Irradiance Environment Maps](http://graphics.stanford.edu/papers/envmap/)

### Using the environment light

The environment light is utilized by a scene.  

```json
"scenes": [
    {
        "nodes": [
            0
        ],
        "extensions": {
            "KHR_lights_environment": {
                "environment": 0
            }
        }
    }
  ]
```

## Adding Light Instances to Scenes

Each scene can have a single IBL light attached to it by defining the `extensions.KHR_lights_image_based` property and, within that, an index into the `lights` array using the `light` property.

```javascript
"scenes" : [
    {
        "extensions" : {
            "KHR_lights_image_based" : {
                "light" : 0
            }
        }
    }            
]
```

### Image-Based Light Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No |
| `intensity` | Brightness multiplier for environment. | No, Default: `1.0` |
| `irradianceCoefficients` | Declares spherical harmonic coefficients for irradiance up to l=2. This is a 9x3 array. | :white_check_mark: Yes |
| `specularCubemaps` | Cubemap texture | :white_check_mark: Yes |




## glTF Schema Updates

- [glTF.KHR_lights_environment.schema.json](schema/glTF.KHR_lights_environment.schema.json)
- [environment.schema.json](schema/environment.schema.json)
- [scene.KHR_lights_environment.schema.json](schema/scene.KHR_lights_environment.schema.json)

## Known Implementations

* `TODO: Add implementations`

## Reference

* `TODO: Add references`
