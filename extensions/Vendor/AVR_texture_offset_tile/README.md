# AVR_texture_offset_tile

## Contributors

* Steven Vergenz, AltspaceVR ([steven@altvr.com](mailto:steven@altvr.com))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many techniques can be used to optimize resource usage for a 3d scene. Chief among them is the ability to minimize the number of textures the GPU must load. To achieve this, many engines encourage packing many objects' low-resolution textures into a single large texture atlas.  The region of the resulting atlas that corresponds with each object is then defined by vertical and horizontal offsets, and the width and height of the region.

To  support this use case, this extension adds `offsetS`, `offsetT`, `tileS`, and `tileT` properties to the texture structure. These properties would typically be implemented as transforms on the UV coordinates. In GLSL: `vec2 uvTransformed = uvFromMeshData * vec2(tileS, tileT) + vec2(offsetS, offsetT);`

This is equivalent to Unity's `Material#SetTextureOffset` and `Material#SetTextureScale`, or Three.js's `Texture#offset` and `Texture#repeat`.

## glTF Schema Updates

The `AVR_texture_offset_tile` extension may be defined on `texture` structures. It may contain the following properties:

| Name      | Type    | Default | Description
|-----------|---------|---------|---------------------------------
| `offsetS` | `float` | `0.0`   | The value added to `uv.x`
| `offsetT` | `float` | `0.0`   | The value added to `uv.y`
| `tileS`   | `float` | `1.0`   | The value multiplied with `uv.x`
| `tileT`   | `float` | `1.0`   | The value multiplied with `uv.y`

### JSON Schema

[AVR_texture_offset_tile.sampler.schema.json](schema/AVR_texture_offset_tile.texture.schema.json)

## Known Implementations

* [UnityGLTF](https://github.com/AltspaceVR/UnityGLTF) (upcoming)

## Resources

*None*
