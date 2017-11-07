# EXT_texture_transform

## Contributors

* Steven Vergenz, Microsoft ([steven.vergenz@microsoft.com](mailto:steven.vergenz@microsoft.com))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many techniques can be used to optimize resource usage for a 3d scene. Chief among them is the ability to minimize the number of textures the GPU must load. To achieve this, many engines encourage packing many objects' low-resolution textures into a single large texture atlas. The region of the resulting atlas that corresponds with each object is then defined by vertical and horizontal offsets, and the width and height of the region.

To support this use case, this extension adds `offset`, `rotation`, and `scale` properties to textureInfo structures, or alternatively a column-major `matrix` array. These properties would typically be implemented as transforms on the UV coordinates. In GLSL:

```glsl
varying in vec2 Uv;

#ifdef BY_COMPONENTS
    uniform vec2 Offset, Scale;
    uniform float Rotation;

    mat3 translation = mat3(1,0,0,0,1,0, Offset.x, Offset.y, 1);
    mat3 rotation = mat3(
      cos(Rotation), sin(Rotation), 0,
     -sin(Rotation), cos(Rotation), 0,
                  0,             0, 1
    );
    mat3 scale = mat3(Scale.x, 0, 0, 0, Scale.y, 0, 0, 0, 1);

    mat3 matrix = translation * rotation * scale;
#else
    uniform mat3 matrix;
#endif

vec2 uvTransformed = ( matrix * vec3(Uv.xy, 1) ).xy;
```

This is equivalent to Unity's `Material#SetTextureOffset` and `Material#SetTextureScale`, or Three.js's `Texture#offset` and `Texture#repeat`.

## glTF Schema Updates

The `EXT_texture_transform` extension may be defined on `textureInfo` structures. It may contain the following properties:

| Name       | Type       | Default             | Description
|------------|------------|---------------------|---------------------------------
| `offset`   | `array[2]` | `[0.0, 0.0]`        | The offset of the UV coordinate origin as a percentage of the texture dimensions.
| `rotation` | `number`   | `0.0`               | Rotate the texture by this many radians counter-clockwise.
| `scale`    | `array[2]` | `[1.0, 1.0]`        | The scale factor applied to the components of the UV coordinates.
| `matrix`   | `array[9]` | 3x3 identity matrix | Transform the UV coordinates applied to this texture by this matrix.

**NOTE**: Though this extension's values are unbounded, they will only produce sane results if the texture sampler's `wrap` mode is `REPEAT`, or if the result of the final UV transformation is within the range [0, 1] (i.e. negative scale settings and correspondingly positive offsets).

### JSON Schema

[EXT_texture_transform.textureInfo.schema.json](schema/EXT_texture_transform.textureInfo.schema.json)

### Example JSON

This example utilizes only the top right quadrant of the source image.

```json
{
  "materials": [{
    "emissionTexture": {
      "source": 0,
      "extensions": {
        "EXT_texture_transform": {
          "offset": [0.5, 0.0],
          "scale": [0.5, 0.5]
        }
      }
    }
  }]
}
```

This example inverts the T axis, effectively defining a bottom-left origin.

```json
{
  "materials": [{
    "emissionTexture": {
      "source": 0,
      "extensions": {
        "EXT_texture_transform": {
          "offset": [0, 1],
          "scale": [1, -1]
        }
      }
    }
  }]
}
```

## Known Implementations

* [UnityGLTF](https://github.com/KhronosGroup/UnityGLTF) (upcoming)

## Resources

*None*
