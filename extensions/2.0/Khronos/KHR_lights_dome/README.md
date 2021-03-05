# KHR_lights_dome

## Contributors

* Ben Houston, Threekit, <https://twitter.com/BenHouston3D>

## Status

Proposed.

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides the ability to define a single image-based dome light in a glTF scene.  A dome light illuminate the scene as if the light was coming from a sphere outside of the scene extents.  Only one dome light per scene is supported to avoid confusing this dome light extension with being a specular or diffuse light probe solution.

Many 3D tools and engines support image-based global illumination but the exact technique and data formats employed vary.

### Differences from EXT_lights_image_based

This extension is similar to the EXT_lights_image_based extension but differs in a bunch of important ways.  These differences are motivated by trying to not presuppose the implementation of the rendering method, rather than letting that leak into the data transfer format specification.  It also simplifies what is stored to the minimum, any additional data can be derived as necessary by the rendering engine.

The differences are:

- The cube map is stored in non-prefiltered form (in nits units), rather than prefiltered (in lux units.)
- Only the top level full resolution cube map is stored, rather than the mip chain.
- The cube map is assumed to be encoded in RGBE if low dynamic range, rather than RGBD.
- No spherical harmonic is stored, if needed it can be generated at run-time.
- No rotation quaternion on the image cube map is specified.

The aim of these differences is to simplify the adoption of this extension by rendering engines.

## Defining Dome Lights

The KHR_lights_dome extension defines a list of dome lights at the root of the glTF.  A dome light contains an intensity, and a cube map that contains the image-based light.  The combination of the decoded image pixels values and the intensity parameter results in a value measured in nits (lm/sr/m^2.)

```javascript
"extensions": {
    "EXT_lights_dome" : {
        "lights": [
            {
                "intensity": 1.0,
                "cubeImageSize": 256,
                "cubeImages": [
                    [... 6 cube faces ...],
                ],
            }
        ]
    }
]
```

## Using Dome Lights

The dome lights can then be specified at a scene root by indexing.

```javascript
"scenes" : [
    {
        "extensions" : {
            "EXT_lights_dome" : {
                "light" : 0
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

### Future HDR Image Support
This extension expects 4-channel PNG's to contain RGBE data. However, this should not interfere in any future glTF extensions that provide HDR support. e.g. if there is an extension that defines RGBD or other type of packing packing for PNG's that is in use for the texture/image, that should override the assumed RGBE packing. Support for other HDR file types (e.g. CTTF formats or .hdr/.exr files) should not interfere either as they won't be PNG's.

### Image-Based Light Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, default: `` |
| `intensity` | Brightness multiplier for environment. | No, Default: `1.0` |
| `cubeImages` | Declares an array of the 6 cubemap images, which of which is square and has the resolution of cubeImageSize. | :white_check_mark: Yes |
| `cubeImageSize` | The dimension (in pixels) of the first specular mip. This is needed to determine, pre-load, the total number of mips needed. | :white_check_mark: Yes |

