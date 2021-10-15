
# KHR_lights_environment

## Contributors

* Norbert Nopper, <mailto:nopper@ux3d.io>
* Rickard Sahlin, <mailto:rickard.sahlin@ikea.com>
* Gary Hsu, Microsoft, <mailto:garyhsu@microsoft.com>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>
* Ben Houston, ThreeKit, <mailto:bhouston@threekit.com>

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.  
This extension depends on KHR_texture_ktx.    
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
The number of specified mip-levels shall be log2 of the min value from texture width and height, ie a full mipmap pyramid. 
This is to ensure deterministic texture memory usage.  

Cubemap images are declared as an array of images in the extension that is placed in the glTF root.  


  > **Implementation Note**: Implementations are free to ignore the pre-filtered mip-levels and generate the mip-levels for roughness values at runtime.  

## Declaring An Environment Light

The KHR_lights_environment extension defines an array of image-based lights at the root of the glTF, each scene can reference one these lights.    
Each environment light definition consists of a single cubemap that describes the specular radiance of the scene, the l=2 spherical harmonics coefficients for irradiance, luminance factor and bounding box for localized cubemap.  

The cubemap is defined as an integer reference to one of the texture sources declared in the this extension.   
These textures may use an extension to specify the format, for instance using KHR_texture_basisu or KHR_texture_float_bc6h.  

When the extension is used, images shall use `image/ktx2` as mimeType.  
The texture type of the KTX v2 file shall be 'Cubemap'  


The following will load the environment light using KHR_texture_ktx.  


```json  

"asset": {
"version": "2.0"
},
"extensionsUsed": [
    "KHR_texture_ktx",
    "KHR_lights_environment"
],
"textures": [
    {
        "extensions": {
            "KHR_texture_ktx": {
                "source": 0,
                "layer": 0
            }
        }
    }
],

"extensions": {
    "KHR_lights_environment" : {
        "images": [
            {
                "name": "environment cubemap 0",
                "uri": "cubemap0.ktx2",
                "mimeType": "image/ktx2"
            }
        ],  
        "textures": [
            {
                "extensions": {
                    "KHR_texture_ktx": {
                        "source": 0,
                        "layer": 0
                    }
                }
            }
        ],
        "lights": [
            {
                "name": "environment light 0",
                "luminance": 1.0,
                "irradianceCoefficients": [...3 x 9 array of floats...],
                "boundingBoxMin": [-100, -100, -100],
                "boundingBoxMax": [100, 100, 100],
                "cubemap": 0,
            }
        ]
    }
}
```

## Specular Radiance Cubemaps

The cubemap used for specular radiance is defined as a cubemap containing separate images for each cube face. 
The mip levels shall evenly map to roughness values from 0 to 1 in the PBR material and should be generated with a principled multi-scatter GGX normal distribution. The data in the maps represents illuminance in candela per square meter.  

If mip-levels are included they are considered to be an approximation of the reflection at varying roughness levels.  

Cube faces are defined in the KTX2 format specification, implementations of this extension must follow the KTX2 cubemap specification.  

The boundingbox for the cubemaps are declared, this means that the locality of the camera within the cubemap can be calculated.  
Making it possible to calculate the localized reflection vector allowing for lighting and reflections that not always uses the centerpoint of the cubemap as a reference point.  
[See Localized cubemaps](#localized-cubemaps)  


### Implementation Note 
This section is non-normative  

If mip-levels are not included, then the entire mip chain of images does not have be generated.  
Generally it is enough to generate mip-level such that the lowest-resolution mip have sufficient size to represent the maximally-blurred radiance map corresponding to roughness=1.  
The texture references defines the largest dimension of mip 0 and should give the loading runtime the information it needs to generate the remainder of the mip chain and sample the appropriate mip level in the shader.  


In a realtime renderer the cubemap textures may be used as a source for radiance, irradiance and reflection.  
A performant way of achieving this is to store the reflection at different roughness values in mip-levels,   
where mip-level 0 is for a mirror like reflection (roughness = 0) and the last mip-level is for maximum roughness (roughness = 0).  
The lod level is calculated like this:  
lod = roughness * (numberOfMipLevels - 1)  

It is recommended to map roughness = 1 to a mip-level size that is larger than 1 * 1 pixels to avoid blockiness, it is generally enough to stop at 16 * 16 pixels.  
To avoid oversampling of flat surfaces (low roughness values) it is recommended to use a 3D cubemap when possible.  

[Runtime filtering of mip-levels](https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling)  
[Creating prefiltered reflection map](https://docs.imgtec.com/Graphics_Techniques/PBR_with_IBL_for_PVR/topics/Assets/pbr_ibl__the_prefiltered_map.html)

## Localized Cubemaps

The cubemaps shall use a technique called localized cubemaps, also called local cubemaps or box projected cubemaps.  
This introduces a boundingbox (min / max) that makes it possible to calculate a local corrected reflection vector.  
Meaning that models displayed within the cubemap environment does not have to rely on the centerpoint for light and reflection lookup.  
Instead it is possible to place objects and the camera within this environment.  
The cubemap boundingbox is declared as a pair of 3D coordinates for min / max.  

### Implementaiton Note

Here is an example of a realtime implementation of reflections using local cubemap:  
[Implement reflections with a local cubemap](https://developer.arm.com/solutions/graphics-and-gaming/gaming-engine/unity/arm-guides-for-unity-developers/local-cubemap-rendering/implement-reflections-with-a-local-cubemap)



## Irradiance Coefficients

This extension may use spherical harmonic coefficients to define irradiance used for diffuse lighting.  
Coefficients are calculated for the first 3 SH bands (l=2) and take the form of a 9x3 array.  
If irradiance coefficients are supplied the specular cubemaps shall not be used as a contribution to diffuse lighting calculations.  
The occlusion parameter of a material will affect the contribution from the irradiance coefficients.  

### Implementation Note
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

`textures`  

| Property | Type   |  Description | Required |
|:-----------------------|:-----------|:------------------------------------------| :--------------------------|
| `textures` | texture [1-* ]  | Array of textures. |  :white_check_mark: Yes |  


`images`  
| Property | Type   |  Description | Required |
|:-----------------------|:-----------|:------------------------------------------| :--------------------------|
| `images` | image [1-* ]  | Array of images that reference KTX V 2 file containing at least one cubemap. |  :white_check_mark: Yes |  


`light` 

| Property | Type   |  Description | Required |
|:-----------------------|:-----------|:------------------------------------------| :--------------------------|
| `name` | String | Name of the light. | No |
| `luminanceFactor` | number  | Luminance factor for the environment, in candela per square meter.  | No, Default: `1.0` |
| `irradianceCoefficients` | number[9][3] | Declares spherical harmonic coefficients for irradiance up to l=2. This is a 9x3 array. | No |
| `boundingBoxMin` | number[3]  | Local boundingbox min. The minimum 3D point of the cubemap boundingbox. In world coordinates (meters) |  :white_check_mark: Yes |
| `boundingBoxMax` | number[3]  | Local boundingbox max. The maximum 3D point of the cubemap boundingbox. In world coordinates (meters) |  :white_check_mark: Yes |
| `cubemap` | integer  | Reference to texture to be used as cubemap. The texture source may use some other extension to include additional texture format support. | :white_check_mark: Yes |


## KTX v2 Images  

The texture type of the KTX v2 file shall be 'Cubemap'  



## glTF Schema Updates

- [glTF.KHR_lights_environment.schema.json](schema/glTF.KHR_lights_environment.schema.json)
- [environment.schema.json](schema/environment.schema.json)
- [scene.KHR_lights_environment.schema.json](schema/scene.KHR_lights_environment.schema.json)




## Known Implementations

* `TODO: Add implementations`

## Reference

[Irradiance Environment Maps](https://graphics.stanford.edu/papers/ravir_thesis/chapter4.pdf)
