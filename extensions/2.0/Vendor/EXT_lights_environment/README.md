# EXT_lights_environment

## Contributors

* `TODO: Add contributors`

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides the ability to define an environment light (also known as [sky light](https://docs.unrealengine.com/en-US/Engine/Rendering/LightingAndShadows/LightTypes/SkyLight/index.html)) in a glTF scene. In general, this environment is used for image based lighting in rasterizers and pathtracers.  

The environment light is defined by a panorama image (also know as [equirectangular image](https://docs.blender.org/manual/en/latest/render/lights/world.html)). For encoding the panorama image and for supporting [High-dynamic-range rendering](https://en.wikipedia.org/wiki/High-dynamic-range_rendering), the widely used [RGBE image format](https://en.wikipedia.org/wiki/RGBE_image_format) is used as a `.hdr` file:  
The panorama image and `.hdr` file format is supported in paint tools like Gimp or Photoshop, 3D modelling tools like Blender or 3ds Max, Game Engines like Unity or Unreal Engine and Web Engines like Babylon.js or three.js.  
In general, this format is already used to define environment lights in todays 3D engines.  

The pre-filtering modules already exist in these engines, which is required for this extension.
Pre-filtering and/or sampling the panorma image is implicitly definded by the [materials BRDF](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation). 

### Why no pre-filtering (also known as pre-sampling)?

* Pathtracers do not require this extra data
* Rasterizers can decide between Spherical Harmonics vs. pre-filtered diffuse lighting
* Rasterizers can decide ...
  * Numbers of pre-filtering samples
  * Pre-filtering algorithm and optimizations
  * Direct panorama image sampling vs. cube map sampling  
    * Cube map resolution
    * Cube map mip-map optimization
* Pre-filtering generates more data to be transfered  
  * Each BRDF potentially needs a new look up table and a mip-mapped cube map
* Graphics API independent
  * DirectX vs. OpenGL 

## Defining an environment light

The `EXT_lights_environment` extension requires to extend the following glTF schemas.

### Defining the panorama image

```json
"images": [
  {
    "uri": "any_other_image.png"
  },
  {
    "uri": "doge2.hdr"
  }
]
```

This means, that the `mimeType` inside [image.schema.json](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/image.schema.json) has to support `image/vnd.radiance`.

### Defining the environment light

One or more environment lights cane be defined inside [glTF.schema.json](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/glTF.schema.json)

```json
"extensions": {
  "EXT_lights_environment": {
    "environments": [
      {
        "source": 0,
        "intensity": 1.0,
        "frontside": "+X"
      }
    ]
  }
}
```    

Both the data inside the `.hdr` file and the intensity are linear values.

At this point of time, there is no convention, where the front side of a panorma image **and** which axis direction the front side is. Because of this, the following is defined:

The directions in UV coordinates (0.0 is left (for u) and bottom (for v) of the image:  
`+X direction   (0.5 , 0.5)`  
`-X direction   (1.0 , 0.5)  (and (0.0 , 0.5) for sampling)`  
`+Y direction   (0.5 , 1.0)`  
`-Y direction   (0.5 , 0.0)`  
`+Z direction   (0.75, 0.5)`  
`-Z direction   (0.25, 0.5)`  

This implies, that by default, the `+X` direction is the front side. By defining a frontside with e.g. `+Z`, the environment has to be rotated by 90 degrees. This can be solved in the following way:

* Adapt sampling by using offsets into the panorama image
* Swap X and Z cube map sides accordingly plus rotate Y cube map sides
* Rotate the sampling vector during runtime

The directions are based on the glTF [coordinate system](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#coordinate-system-and-units).

The environment light is expressed as Luminance which is [Candela per meter squared](https://docs.unrealengine.com/en-US/Engine/Rendering/LightingAndShadows/PhysicalLightUnits/index.html) (cd/m2).

### Using the environment light

The environment light is utilized by a scene.

```json
"scenes": [
    {
        "nodes": [
            0
        ],
        "extensions": {
            "EXT_lights_environment": {
                "environment": 0
            }
        }
    }
  ]
```

## glTF Schema Updates

- [glTF.EXT_lights_environment.schema.json](schema/glTF.EXT_lights_environment.schema.json)
- [environment.schema.json](schema/environment.schema.json)
- [scene.EXT_lights_environment.schema.json](schema/scene.EXT_lights_environment.schema.json)

## Known Implementations

* `TODO: Add implementations`
