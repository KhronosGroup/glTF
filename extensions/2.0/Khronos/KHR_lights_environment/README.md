
# KHR_lights_environment

## Contributors

* Norbert Nopper, <mailto:nopper@ux3d.io>
* Rickard Sahlin, <mailto:rickard.sahlin@ikea.com>
* Gary Hsu, Microsoft, <mailto:garyhsu@microsoft.com>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>
* Ben Houston, ThreeKit, <mailto:bhouston@threekit.com>

## Status

Draft

Open issues:
Should Spherical Harmonics be supported or should clients be forced to evaluate the specular cubemap for diffuse contribution.  
The question comes down to if resolution supplied by 9 coefficients SH is enough for most usecases?
Since the SH are optional, and I believe the quality using 9 coefficients is reasonable,  I am leaning towards keeping them.   

If compressed texture format is used how should pre-filtering for roughness reflection be done?
May lead to unpredicted results of clients shall decode then create mip-levels since the result will likely take much more GPU memory since compression is likely not supported on target.  

## Dependencies

Written against the glTF 2.0 spec.  
This extension depends on KHR_texture_ktx
This extension may use KHR_texture_basisu or KHR_texture_float_bc6h to support compressed texture formats.

## Overview
This extension provides the ability to define image-based lights in a glTF scene using KTX v2 images as defined by KHR_texture_ktx.  
Image-based lights (environment map, light map) consist of a cubemap map that represents specular radiance for the scene, irradiance information and is the source for environment reflection.   
This can be used on it's own - ie a glTF asset with only environment map data - for a usecase where the IBL needs to be distributed.  
It can also be used together with model data, for usecases where a model shall be displayed in a defined environment.  

Many 3D tools and engines support image-based global illumination but the exact technique and data formats employed vary.  
Using this extension, tools can export and engines can import image-based lights and the result should be highly consistent.   

This extension specifies exactly one way to format and reference the environment map to be used, the goals of this are two-fold.  
First, it makes implementing support for this extension easier.  
Secondly, it ensures that rendering of the image-based lighting is consistent across runtimes.

A conforming implementation of this extension must be able to load the image-based environment data and render the PBR materials using this lighting.  

The environment light is defined by a cubemap, this is to simplify realtime implementations as these are likely to use cubemap texture format.  
Cubemaps shall be supplied without pre-filtered mip-maps for roughness values > 0, pre-filtering shall be done by the client.  
[See Specular radiance cubemaps](#specular-radiance-cubemaps)  

If a compressed texture format is used then pre-filtered mip-levels for roughness values > 0 shall be specified.  

## Declaring an environment light

The KHR_lights_environment extension defines an array of image-based lights at the root of the glTF and then each scene can reference one.  
Each environment light definition consists of a single cubemap that describes the specular radiance of the scene, the l=2 spherical harmonics coefficients for diffuse irradiance and intensity values.  
The cubemap is defined by texture references to a KTX2 file containing a cubemap.  
These files can contain compressed textures using KHR_texture_basisu or compressed float texture format as defined by KHR_texture_float_bc6h.   

When the extension is used, images shall use `image/ktx2` as mimeType for cubemaps that are referenced by the `specularCubemap` property of KHR_lights_environment extension object.  
The texture type of the KTX v2 file shall be 'Cubemap'  


The following will load the environment light using KHR_texture_basisu on clients that supports that extension,  otherwise fall back to using KTX2 using VK_FORMAT_R8G8B8_SRGB.  


```json
"textures": [
    {
        "source": 0,
        "extensions": {
            "KHR_texture_basisu": {
                "source": 1
            }
        }
    }
],
"images": [
        {
            "name": "environment cubemap 0",
            "uri": "cubemap0.ktx2",
            "mimeType": "image/ktx2"
        },
        {
            "name": "environment cubemap basisu",
            "uri": "cubemap_basisu.ktx2",
            "mimeType": "image/ktx2"
        }
],
"extensions": {
    "KHR_lights_environment" : {
        "lights": [
            {
                "intensity": 1.0,
                "irradianceCoefficients": [...3 x 9 array of floats...],
                "specularCubemaps": [0],
            }
        ]
    }
}
```

## Specular Radiance Cubemaps

The cubemap used for specular radiance is defined as a cubemap containing separate images for each cube face. 
The mip levels shall evenly map to roughness values from 0 to 1 in the PBR material and should be generated with a principled multi-scatter GGX normal distribution. The data in the maps represents illuminance in candela per square meter (nit).

If mip-levels are not included, then the entire mip chain of images should not be generated.  
Instead, the lowest-resolution mip should have sufficient size to represent the maximally-blurred radiance map corresponding to roughness=1.  
The texture references defines the largest dimension of mip 0 and should give the loading runtime the information it needs to generate the remainder of the mip chain and sample the appropriate mip level in the shader.  

Cube faces are defined in the KTX2 format specification, users of this extension must follow the KTX2 cubemap specification.  

## Implementation Note 
This section is non-normative  

The cubemap textures are used both as direct light contribution and as source for reflection.  
A performant way of achieving this is to store the reflection at different roughness values in mip-levels,   
where mip-level 0 is for a mirror like reflection (roughness = 0) and the last mip-level is for maximum roughness (roughness = 0).  
It is recommended to map roughness = 1 to a mip-level size that is larger than 1 * 1 pixels to avoid blockiness, it is generally enough to stop at 16 * 16 pixels.  
To avoid oversampling of flat surfaces (low roughness values) it is recommended to use a 3D cubemap when possible.  

[Runtime filtering of mip-levels](https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling)


## Irradiance Coefficients

This extension may use spherical harmonic coefficients to define irradiance used for diffuse lighting.  
Coefficients are calculated for the first 3 SH bands (l=2) and take the form of a 9x3 array.  

## Implementation Note
This section is non-normative  

Implementations may calculate the irradiance from the specular radiance cubemap and use instead.  
One possible benefit with spherical harmonics is that it is generally enough evaluate the harmonics on a per vertex basis, instead of sampling a cubemap on a per fragment basis.  

[Realtime Image Based Lighting using Spherical Harmonics](https://metashapes.com/blog/realtime-image-based-lighting-using-spherical-harmonics/)  
[An Efficient Representation for Irradiance Environment Maps](http://graphics.stanford.edu/papers/envmap/)  

### Using the environment light

The environment light is utilized by a scene.  
Each scene can have a single environment light attached to it by defining the `extensions.KHR_lights_environment` property and, within that, an index into the `lights` array using the `light` property.

```json
"scenes": [
    {
        "nodes": [
            0
        ],
        "extensions": {
            "KHR_lights_environment": {
                "light": 0
            }
        }
    }
  ]
```


### Environment Light Properties

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

[Irradiance Environment Maps](https://graphics.stanford.edu/papers/ravir_thesis/chapter4.pdf)
