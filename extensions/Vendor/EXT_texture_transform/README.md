# EXT_texture_transform

## Contributors

* Steven Vergenz, Microsoft ([steven.vergenz@microsoft.com](mailto:steven.vergenz@microsoft.com))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many techniques can be used to optimize resource usage for a 3d scene. Chief among them is the ability to minimize the number of textures the GPU must load. To achieve this, many engines encourage packing many objects' low-resolution textures into a single large texture atlas. The region of the resulting atlas that corresponds with each object is then defined by vertical and horizontal offsets, and the width and height of the region.

To support this use case, this extension adds `offset` and `scale` properties to textureInfo structures. These properties would typically be implemented as transforms on the UV coordinates. In GLSL: `vec2 uvTransformed = Scale * Uv + Offset;`

This is equivalent to Unity's `Material#SetTextureOffset` and `Material#SetTextureScale`, or Three.js's `Texture#offset` and `Texture#repeat`.

## glTF Schema Updates

The `EXT_texture_transform` extension may be defined on `textureInfo` structures. It may contain the following properties:

| Name       | Type       | Default      | Description
|------------|------------|--------------|---------------------------------
| `offset`   | `array[2]` | `[0.0, 0.0]` | The offset of the UV coordinate origin as a percentage of the texture dimensions.
| `scale`    | `array[2]` | `[1.0, 1.0]` | The scale factor applied to the components of the UV coordinates.
| `texCoord` | `integer`  |              | Overrides the textureInfo texCoord value if supplied, and if this extension is supported.

Though this extension's values are unbounded, they will only produce sane results if the texture sampler's `wrap` mode is `REPEAT`, or if the result of the final UV transformation is within the range [0, 1] (i.e. negative scale settings and correspondingly positive offsets).

> **Implementation Note**: For maximum compatibility, it is recommended that exporters generate UV coordinate sets both with and without transforms applied, use the post-transform set in the texture `texCoord` field, then the pre-transform set with this extension. This way, if the extension is not supported by the consuming engine, the model still renders correctly. Including both will increase the size of the model, so if including the fallback UV set is too burdensome, either add this extension to `extensionsRequired` or use the same texCoord value in both places.

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

* [UnityGLTF](https://github.com/KhronosGroup/UnityGLTF)
* [Babylon.js](https://www.babylonjs.com/)