# AVR_texture_offset_tile

## Contributors

* Steven Vergenz, AltspaceVR ([steven@altvr.com](mailto:steven@altvr.com))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many techniques can be used to optimize resource usage for a 3d scene. Chief among them is the ability to minimize the number of textures the GPU must load. To achieve this, many engines encourage packing many objects' low-resolution textures into a single large texture atlas. The region of the resulting atlas that corresponds with each object is then defined by vertical and horizontal offsets, and the width and height of the region.

To support this use case, this extension adds `offsetS`, `offsetT`, `tileS`, and `tileT` properties to the texture structure. These properties would typically be implemented as transforms on the UV coordinates. In GLSL: `vec2 uvTransformed = uvFromMeshData * vec2(tileS, tileT) + vec2(offsetS, offsetT);`

This is equivalent to Unity's `Material#SetTextureOffset` and `Material#SetTextureScale`, or Three.js's `Texture#offset` and `Texture#repeat`.

## glTF Schema Updates

The `AVR_texture_offset_tile` extension may be defined on `texture` structures. It may contain the following properties:

| Name      | Type     | Default | Description
|-----------|----------|---------|---------------------------------
| `offsetS` | `number` | `0.0`   | The value added to `uv.x`
| `offsetT` | `number` | `0.0`   | The value added to `uv.y`
| `tileS`   | `number` | `1.0`   | The value multiplied with `uv.x`
| `tileT`   | `number` | `1.0`   | The value multiplied with `uv.y`

All of these values are restricted to the range [-1, 1]. While negative values are allowed, they will only produce sane results if the texture sampler's `wrap` mode is `REPEAT`, or if the result of the final UV transformation is within the range [0, 1] (i.e. negative tile settings and correspondingly positive offsets).

### JSON Schema

[AVR_texture_offset_tile.texture.schema.json](schema/AVR_texture_offset_tile.texture.schema.json)

### Example JSON

This example utilizes only the top left quadrant of the source image.

```json
{
	"textures": [
		{
			"source": 0,
			"sampler": 0,
			"extensions": {
				"AVR_texture_offset_tile": {
					"offsetS": 0.5,
					"tileS": 0.5,
					"tileT": 0.5
				}
			}
		}
	]
}
```

This example inverts the T axis, effectively defining a bottom-left origin.

```json
{
	"textures": [
		{
			"source": 0,
			"sampler": 0,
			"extensions": {
				"AVR_texture_offset_tile": {
					"offsetT": 1,
					"tileT": -1
				}
			}
		}
	]
}
```

## Known Implementations

* [UnityGLTF](https://github.com/AltspaceVR/UnityGLTF) (upcoming)

## Resources

*None*
