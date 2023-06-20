
# KHR_environment_map

## Contributors

* Norbert Nopper, <mailto:nopper@ux3d.io>
* Rickard Sahlin, <mailto:rickard.sahlin@ikea.com>
* Gary Hsu, Microsoft, <mailto:garyhsu@microsoft.com>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>
* Ben Houston, ThreeKit, <mailto:bhouston@threekit.com>
* Dominick D'Aniello, Mozilla <mailto:netpro2k@gmail.com>
* Sebastian Vanderbeghe, <mailto:sevan@microsoft.com>  

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.  

## Overview
This extension provides the ability to define one or moreenvironment maps to a glTF using KTX v2 cubemaps and/or irradiance coefficients.    
The extension provide two ways of supplying the surrounding environment.
Texture-based by means of a cubemap and irradiance coefficients by means of spherical harmonics.  

The texture cubemap can be seen as a way of representing the environment in which one or more glTF models are placed.  
The irradiance coefficients contain the non-directed light contribution integrated from the scene, these may be supplied or can be calculated by the implementations.    

This extension can be used on it's own - ie a glTF asset with only environment map data - for a usecase where the environmental map needs to be distributed.  
It may also be used together with model data, for usecases where a model shall be displayed in a controlled environment.  

Many 3D tools and engines support image-based 'global illumination' but the exact technique and data formats employed vary.  
Using this extension, tools can export and engines can import environment maps and the result should be highly consistent.   

This extension specifies exactly one way to format and reference the environment map to be used, the goals of this are two-fold.  
First, it makes implementing support for this extension easier.  
Secondly, it ensures that rendering of the environment map is consistent across runtimes.

A conforming implementation of this extension must be able to load the environment data and render the PBR materials using this.  

Cubemap environment images are declared as an array of images in the extension, this extension is then placed in the glTF root.  
These images shall be supplied using a KTX V2 file containing at least one cubemap.  

### Pre Filtering Of Cubemaps

The environment is defined by a cubemap, this is to simplify realtime implementations as these are likely to use cubemap texture format.  
Cubemaps shall be supplied without pre-filtered mip-maps for roughness values > 0, pre-filtering shall be done by the client  
- unless for compressed texture image formats that may include pre-filtered mip-maps.    
[See Specular radiance cubemaps](#specular-radiance-cubemaps)  


## Declaring An Environment Map

The KHR_environment_map extension defines an array of environment cubemaps in the extensions root of the glTF, each scene can reference one these cubemaps.    
Each environment may reference one cubemap, consisting of 6 textures. Cubemaps are declared in the root `images` array and reference a KTX V2 file.   
Each environment definition consists a single cubemap that describes the incoming radiance to the scene, the l=2 spherical harmonics coefficients for irradiance, luminance factor and bounding box for localized cubemap.  

The cubemap is defined as an integer reference to one of the texture sources declared in the this extension. 
This texture source shall contain a cubemap at the layer defined by the texture.   

When this extension is used, images shall use `image/ktx2` as mimeType.  
The texture type of the KTX v2 file shall be 'Cubemap'  

The following will declare one cubemap to be referenced by a scene.  


```json  

"asset": {
"version": "2.0"
},
"extensionsUsed": [
    "KHR_environment_map"
],

"extensions": {
    "KHR_environment_map" : {
        "cubemaps": [
            {
                "source": 0
                "layer": 0
            }
        ],
        "environment_maps": [
            {
                "name": "environment map 0",
                "irradianceCoefficients": [...3 x 9 array of floats...],
                "boundingBoxMin": [-100, -100, -100],
                "boundingBoxMax": [100, 100, 100],
                "cubemap": 0,
            }
        ]
    }
},
"images": [
    {
        "name": "environment cubemap 0",
        "uri": "cubemap0.ktx2",
        "mimeType": "image/ktx2"
    }
]  

```

## Radiance Cubemaps

The cubemap used for specular radiance is defined as a cubemap containing separate images for each cube face.  
The data in the maps represents illuminance in candela per square meter.  
Using this information it is possible to calculate the non-direct light contribution to the scene if irradiance coefficients are not included.  

Cube faces are defined in the KTX2 format specification, implementations of this extension must follow the KTX2 cubemap specification.  

If specular radiance cubemap is supplied it shall be used as contribution to directed light incoming to the scene and as a source for environment reflections.  

If the boundingbox for the cubemaps are declared, this means that the locality of the camera within the cubemap can be calculated.  
Making it possible to calculate the localized reflection vector allowing for lighting and reflections that not always uses the centerpoint of the cubemap as a reference point.  
[See Localized cubemaps](#localized-cubemaps)  


**Implementation Notes** 
This section is non-normative  

One common way of sampling the reflected component of light contribution from the environment is using pre-filtered maps.  
In short this is a process where the view-independent directional diffuse contribution is separated from the view-dependent ideal specular contribution.  
This information is then stored as varying degrees of diffused vs specular light contribution in mip-levels of the cubemap and can then be used as a source for reflection from not only ideal specular surfaces.  

The mip levels used for reflection shall evenly map to roughness values from 0 to 1 in the PBR material.  
If needed these shall be generated by the implementation.  
Exactly how this is done is up to the implementation, for a general realtime usecase it is usually enough to create the mip-levels using linear filtering.  
The texture references defines the largest dimension of mip 0 and should give the loading runtime the information it needs to generate the remainder of the mip chain and sample the appropriate mip level in the shader.  

In a realtime renderer the cubemap textures may be used as a source for radiance, irradiance and reflection.  
A performant way of achieving this is to store the reflection at different roughness values in mip-levels,   
where mip-level 0 is for a mirror like reflection (roughness = 0) and the last mip-level, where one texel lookup equals the normal distribution over the hemisphere (roughness = 1).  


[Runtime filtering of mip-levels](https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling)  
[Creating prefiltered reflection map](https://docs.imgtec.com/Graphics_Techniques/PBR_with_IBL_for_PVR/topics/Assets/pbr_ibl__the_prefiltered_map.html)
[A Global Illumination Solution for General Reflectance Distributions](https://www.graphics.cornell.edu/pubs/1991/SAWG91.pdf)

## Localized Cubemaps

The cubemaps may use a technique called localized cubemaps, also called local cubemaps or box projected cubemaps.  
This introduces a boundingbox (min / max) that makes it possible to calculate a local corrected reflection vector.  
Meaning that models displayed within the cubemap environment does not have to rely on the centerpoint for reflection lookup from an infinite distance.    
Instead it is possible to place objects and the camera within this environment.  The cubemap boundingbox is declared as a pair of 3D coordinates for min / max.  


### Implementation Note

If boundingbox min and max is not specified the distance to the cubemap shall be considered to be boundless.  
This means that only directionality based on cubemap centerpoint shall be used.  

Here is an example of a realtime implementation of reflections using local cubemap:  
[Implement reflections with a local cubemap](https://developer.arm.com/solutions/graphics-and-gaming/gaming-engine/unity/arm-guides-for-unity-developers/local-cubemap-rendering/implement-reflections-with-a-local-cubemap)



## Irradiance Coefficients

This extension may use spherical harmonic coefficients to define irradiance as a contribution for diffuse (lambertian) lighting.  
Coefficients are calculated for the first 3 SH bands (l=2) and take the form of a 9x3 array.  

If irradiance coefficients are supplied the specular radiance cubemaps shall not be used as a contribution to diffuse lighting calculations, instead the irradiance coefficients shall be used.   

The occlusion parameter of a material will affect the contribution from the irradiance coefficients.  
If occlusion is included in the material definition the diffuse contribution shall be calculated using the occlusion factor.  


### Implementation Note
This section is non-normative  

If irradiance coefficients are not defined, implementations may calculate irradiance from the specular radiance cubemap.  
One possible benefit with spherical harmonics is that it is generally enough evaluate the harmonics on a per vertex basis, instead of sampling a cubemap on a per fragment basis. 

### Using the environment map

The environment map is utilized by a scene.  
Each scene can have a single environment map attached to it by defining the `extensions.KHR_environment_map` property and, within that, an index into the `environment_maps` array using the `environment_map` property.

```json
"scenes": [
    {
        "nodes": [
            0
        ],
        "extensions": {
            "KHR_environment_map": {
                "environment_map": 0
            }
        }
    }
  ]
```


### Environment Map Properties

The environment map declaration present in the root of the glTF object must contain one or more cubemaps and images specification.  
Cubemaps are declared as an array of image and layer indexes.   
Images are declared as an array of image objects referencing KTX v2 image files.  

`cubemaps`  

| Property | Type   |  Description | Required |
|:-----------------------|:-----------|:------------------------------------------| :--------------------------|
| `source` | integer  | Index of KTX V2 file containing cubemap in the specified layer |  :white_check_mark: Yes |
| `layer` | integer  | Layer of the KTX V2 file containing the cubemap, if no value is supplied the default value of 0 is used |  No |
| `intensity`| number | Intensity of cubemap values. The cubemap texel values shall be scaled by this value. Defaults to 1 if no value specified. | No |

`images`  
| Property | Type   |  Description | Required |
|:-----------------------|:-----------|:------------------------------------------| :--------------------------|
| `images` | Image [1-* ]  | Array of images that reference KTX V 2 file containing at least one cubemap. Mimetype must be 'image/ktx2'. |  :white_check_mark: Yes |  


`environment_map` 

| Property | Type   |  Description | Required |
|:-----------------------|:-----------|:------------------------------------------| :--------------------------|
| `name` | String | Name of the environment map. | No |
| `irradianceCoefficients` | number[9][3] | Declares spherical harmonic coefficients for irradiance up to l=2. This is a 9x3 array. | No |
| `irradianceFactor` | Scale factor for irradiance coefficients. Each irradiance coefficient shall be scaled by this factor. Defaults to 1 of not specified | No |
| `boundingBoxMin` | number[3]  | Local boundingbox min. The minimum 3D point of the cubemap boundingbox. In world coordinates (meters) |  No |
| `boundingBoxMax` | number[3]  | Local boundingbox max. The maximum 3D point of the cubemap boundingbox. In world coordinates (meters) |  No |
| `cubemap` | integer  | Reference to texture source to be used as specular radiance cubemap, this references one of the images declared by this extensions images array.  | :white_check_mark: Yes |
| `ior` | number | Index of refraction of the media within the environment. If scene and environment is stand-alone the IOR is considered to be infinite. If glTF is referenced, for instance by glXF, the media shall be considered to be contained within the boundingbox. If no value is specified the IOR shall default to 1.0 for air. | No |


## KTX v2 Images  

The texture type of the KTX v2 file shall be 'Cubemap'  
The texture format must be one of the following:  

VK_FORMAT_R8G8B8_SRGB  
VK_FORMAT_R8G8B8_UNORM  
VK_FORMAT_E5B9G9R9_UFLOAT_PACK32  
VK_FORMAT_B10G11R11_UFLOAT_PACK32  
VK_FORMAT_R16G16B16_SFLOAT  
VK_FORMAT_R16G16B16_UNORM  


## glTF Schema Updates

- [glTF.KHR_lights_environment.schema.json](schema/glTF.KHR_lights_environment.schema.json)
- [environment.schema.json](schema/environment.schema.json)
- [scene.KHR_lights_environment.schema.json](schema/scene.KHR_lights_environment.schema.json)




## Known Implementations

* `TODO: Add implementations`

## References

[Irradiance Environment Maps](https://graphics.stanford.edu/papers/ravir_thesis/chapter4.pdf)
[A Global Illumination Solution for General Reflectance Distributions](https://www.graphics.cornell.edu/pubs/1991/SAWG91.pdf)
[Realtime Image Based Lighting using Spherical Harmonics](https://metashapes.com/blog/realtime-image-based-lighting-using-spherical-harmonics/)
[An Efficient Representation for Irradiance Environment Maps](http://graphics.stanford.edu/papers/envmap/)  
[Runtime filtering of mip-levels](https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling)  
[Creating prefiltered reflection map](https://docs.imgtec.com/Graphics_Techniques/PBR_with_IBL_for_PVR/topics/Assets/pbr_ibl__the_prefiltered_map.html)

