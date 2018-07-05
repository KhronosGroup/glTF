# EXT_lights_imageBased

## Contributors

* Gary Hsu, Microsoft, <mailto:garyhsu@microsoft.com>
* Mike Bond, Adobe, <mailto:mbond@adobe.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides the ability to define image-based lights in a glTF scene. Image-based lights consist of an environment map that represents specular radiance for the scene as well as irradiance information.

Many 3D tools and engines support image-based global illumination but the exact technique and data formats employed vary. Using this extension, tools can export and engines can import image-based lights and the result should be highly consistent. 

This extension specifies exactly one way to format and reference the environment map to be used as well as one format for HDR image encoding. The goals of this are two-fold. First, it makes implementing support for this extension easier. Secondly, it ensures that rendering of the image-based lighting is consistent across runtimes.

A conforming implementation of this extension must be able to load the image-based environment data and render the PBR materials using this lighting. 

## Defining an Image-Based Light

The EXT_lights_imageBased extension defines an array of image-based lights at the root of the glTF and then each scene can reference one. Each image-based light definition consists of a single cubemap that describes the specular radiance of the scene, the l=2 spherical harmonics coefficients for diffuse irradiance and rotation and intensity values.

```javascript
"extensions": {
    "EXT_lights_imageBased" : {
        "lights": [
            {
                "intensity": 1.0,
                "rotation": [0, 0, 0, 1],
                "irradianceCoefficients": [3x9 array of floats],
                "specularImageSize": 256,
                "specularImages": [
                    [... 6 cube faces for mip 0 ...],
                    [... 6 cube faces for mip 1 ...],
                    ...
                    [... 6 cube faces for mip n ...]
                ],
            }
        ]
    }
}
```

## Prefiltered Specular Radiance Cubemaps

The cubemap used for specular radiance is defined along with its prefiltered mipmaps with each face of the cube defined as separate images. The various mip levels evenly map to roughness values from 0 to 1 in the PBR material and should be generated with a principled multi-scatter GGX normal distribution. The data in the maps represents illuminance in lux (lm/m^2).

The entire mip chain of images should not be provided. Instead, the lowest-resolution mip should have sufficient size to represent the maximally-blurred radiance map (say, 16x16) corresponding to roughness=1. The `specularImageSize` value defines the largest dimension of mip 0 and, taken together with the number of defined mips, should give the loading runtime the information it needs to generate the remainder of the mip chain and sample the appropriate mip level in the shader.

*(say something about tooling for generating these mips)*

Cube faces are defined in the following order and must be authored with the Y-axis inverted: 
1. Positive X
1. Negative X
1. Positive Y
1. Negative Y
1. Positive Z
1. Negative Z

## Irradiance Coefficients

This extension defines spherical harmonic coefficients up to l=2 for irradiance lighting. This takes the form of a 9x3 array.
https://cseweb.ucsd.edu/~ravir/papers/envmap/envmap.pdf

*(this section needs more about how the coefficients are used as well as a brief blurb about what SH are)*
*(also, we should probably just put some shader code in here for using the coefficients)*


## Adding Light Instances to Scenes

Esch scene can have a single IBL light attached to it by defining the `extensions.EXT_lights_imageBased` property and, within that, an index into the `lights` array using the `light` property.

```javascript
"scenes" : [
    {
        "extensions" : {
            "EXT_lights_imageBased" : {
                "light" : 0
            }
        }
    }            
]
```

### Image-Based Light Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, Default: `""` |
| `rotation` | Quaternion that represents the rotation of the IBL environment. | No, Default: `[0.0, 0.0, 0.0, 1.0]` |
| `intensity` | Brightness multiplier for environment. | No, Default: `1.0` |
| `irradianceCoefficients` | Declares spherical harmonic coefficients for irradiance up to l=2. This is a 9x3 array. | :white_check_mark: Yes |
| `specularImages` | Declares an array of the first N mips of the prefiltered cubemap. Each mip is, in turn, defined with an array of 6 images, one for each cube face. i.e. this is an Nx6 array. | :white_check_mark: Yes |
| `specularImageSize` | The maximum dimension (in pixels) of the first specular mip. This is needed to determine, pre-load, the total number of mips needed. | :white_check_mark: Yes |

