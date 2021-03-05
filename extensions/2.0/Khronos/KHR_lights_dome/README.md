# KHR_lights_dome

## Contributors

* Ben Houston, Threekit, <https://twitter.com/BenHouston3D>

## Status

Proposed.

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides the ability to define a single image-based dome light in a glTF scene.  A dome light illuminate the scene as if the light was coming from a sphere outside of the scene extents.  Only one dome light per scene is supported to avoid confusing this dome light extension with being a specular or diffuse light probe solution.

Many 3D tools and engines support image-based global illumination but the exact technique and data formats employed vary.  This extension differs from the EXT_lights_image_based extension in that it does not specify the exact implementation of global illumination from the dome light but rather specifies the precise meaning of the dome light, leaving the implementation as an exercise for the engine.  It also makes interchange of the data simplier because this extension does not make assumptions of implementation.

The extension supports only a single type of dome light, an image-based dome light, enough though other simplifer dome light specifications exist (spherical harmonic, solid color, etc.).  The lack of choice is motivated by the fact that all other types of dome lights can be viewed as simplification of a sufficiently high resolution image-based dome light.

The extension also specifies that all cube maps images must be encoded in RGBE if they are low dynamic range.  If the image format is high dynamic range it is assumed to be in linearly encoded.  While RGBE is unlikely to be the run-time format, it is the format that has the largest dynamic range and is also the most well established format for 8-bit per channel HDR images.  It is expected that the run-time engine will re-encode this input image format to whatever is appropriate for the engine itself.

This format is purposely highly opinionated in order to simplify implementation.

## Defining Dome Lights

The KHR_lights_dome extension defines a single dome light at the root of the glTF.  A dome light contains an intensity, and a cube map that contains the specular image-based light.  The combination of the image along with the intensities is measured in nits.

```javascript
"scenes" : [
    {
        "extensions": {
            "EXT_lights_dome" : {
                "intensity": 1.0,
                "cubeImageSize": 256,
                "cubeImages": [
                    [... 6 cube faces ...],
                ],
            }
        }
    }
]
```

## Units

The dome light, which is the combination of the decoded pixel values of the high dynamic range multiplied by the intensity, is assumed to be in nits (lm/sr/m^2).

## CubeMap Images

Cube faces are defined in the following order and adhere to the standard orientations as shown in the image bellow.
1. Positive X
1. Negative X
1. Positive Y
1. Negative Y
1. Positive Z
1. Negative Z

<figure>
<img src="./figures/Cube_map.svg"/>
<figcaption><em>Cube map orientation reference.<br>Image by <a href="//commons.wikimedia.org/w/index.php?title=User:Microwerx&amp;action=edit&amp;redlink=1" class="new" title="User:Microwerx (page does not exist)">Microwerx</a> - <span class="int-own-work" lang="en">Own work</span>, <a href="https://creativecommons.org/licenses/by-sa/4.0" title="Creative Commons Attribution-Share Alike 4.0">CC BY-SA 4.0</a>, <a href="https://commons.wikimedia.org/w/index.php?curid=48935423">Link</a></em></figcaption>
</figure>

Note that for this extension, each saved image must be flipped about its vertical axis to correspond with the way <a href="https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#images">glTF references texture space</a>.

https://en.wikipedia.org/wiki/Cube_mapping

### Image-Based Light Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, default: `` |
| `intensity` | Brightness multiplier for environment. | No, Default: `1.0` |
| `cubeImageEncoding` | The encoding of the image map.  Must be `Linear` or `RGBE`.  | No, Defaults: `Linear` |
| `cubeImages` | Declares an array of the first N mips of the prefiltered cubemap. Each mip is, in turn, defined with an array of 6 images, one for each cube face. i.e. this is an Nx6 array. | :white_check_mark: Yes |
| `cubeImageSize` | The dimension (in pixels) of the first specular mip. This is needed to determine, pre-load, the total number of mips needed. | :white_check_mark: Yes |

