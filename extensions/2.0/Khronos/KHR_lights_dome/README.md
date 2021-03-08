# KHR_lights_dome

## Contributors

* Ben Houston, Threekit, <https://twitter.com/BenHouston3D>
* Romain Guy, Google, <https://twitter.com/romainguy>

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
- The cube map data, if specified via a PNG, is assumed to be encoded in R11G11B10F, rather than RGBD.
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

Note that for this extension, each saved image must be flipped about its vertical axis to correspond with the way <a href="https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#images">glTF references texture space</a>.

### R11G11B10F

The rational for [R11G11B10F](https://www.khronos.org/opengl/wiki/Small_Float_Formats) is that it is a native HDR format for the GPU within in a small form factor, just 32 bits per pixel.  It is required to be natively supported on all WebGL 2 and WebGPU supporting implementation.  This precision has proven to be sufficient for read-only HDR maps based on practical usage.  Some have argued that having independent exponents for each channel can allow for increased color accuracy as well across multiple exposure values.

#### Decoding/Encoding

TODO: Ensure that this actually works!  -Ben, March 8, 2021.

For platforms that do not support WebGL 2 and WebGPU, it is required that one can decode R11G11B10F manually.  To faciliate this, the following code snippets are provided:

```glsl
lowp vec4 packR11G11B10(mediump vec3 raw){
    mediump vec3 idx = floor(log2(max(raw,0.00001)));
    idx = max(min(idx,16.0),-15.0);
    mediump vec3 num = max((raw*exp2(-idx) -1.0),0.0) ;
    return (vec4((idx +geta),floor(num.b * 32.0))*8.0
        + floor(vec4(num.rg,fract(num.rg * 8.0))*8.0)
        )/255.0;
}

mediump vec3 unpackR11G11B10(lowp vec4 raw){
    mediump vec4 lo = floor(raw*255.0+0.5)/8.0;
    mediump vec4 hi= floor(lo);
    lo -=  hi;
    return (vec3(lo.rg + lo.ba/8.0,hi.a/32.0) +1.0)
        * exp2(hi.rgb-geta);
}
```

[Source](https://github.com/qeouo/nanka/blob/2561f0b6d9b86483028c873cee96406f990839b0/hdrpaint/src/lib/rastgl.js#L192)
### Future HDR Image Support
This extension expects 4-channel PNG's to contain R11G11B10F data. However, this should not interfere with any future glTF extensions that provide HDR support.  If an image extension supports true HDR data, then it is assumed that this extension will also support loading that data in its native format.

### Image-Based Light Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, default: `` |
| `intensity` | Brightness multiplier for environment. | No, Default: `1.0` |
| `cubeImages` | Declares an array of the 6 cubemap images, which of which is square and has the resolution of cubeImageSize. | :white_check_mark: Yes |
| `cubeImageSize` | The dimension (in pixels) of the first specular mip. This is needed to determine, pre-load, the total number of mips needed. | :white_check_mark: Yes |

