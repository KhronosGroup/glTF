# EXT_texture_offset_scale

## Contributors

* Steven Vergenz, Microsoft ([steven.vergenz@microsoft.com](mailto:steven.vergenz@microsoft.com))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many techniques can be used to optimize resource usage for a 3d scene. Chief among them is the ability to minimize the number of textures the GPU must load. To achieve this, many engines encourage packing many objects' low-resolution textures into a single large texture atlas. The region of the resulting atlas that corresponds with each object is then defined by vertical and horizontal offsets, and the width and height of the region.

To support this use case, this extension adds `offsetS`, `offsetT`, `scaleS`, and `scaleT` properties to textureInfo structures. These properties would typically be implemented as transforms on the UV coordinates. In GLSL: `vec2 uvTransformed = uvFromMeshData * vec2(scaleS, scaleT) + vec2(offsetS, offsetT);`

This is equivalent to Unity's `Material#SetTextureOffset` and `Material#SetTextureScale`, or Three.js's `Texture#offset` and `Texture#repeat`.

## glTF Schema Updates

The `EXT_texture_offset_scale` extension may be defined on `textureInfo` structures. It may contain the following properties:

| Name      | Type     | Default | Description
|-----------|----------|---------|---------------------------------
| `offsetS` | `number` | `0.0`   | The value added to `uv.x`
| `offsetT` | `number` | `0.0`   | The value added to `uv.y`
| `scaleS`  | `number` | `1.0`   | The value multiplied with `uv.x`
| `scaleT`  | `number` | `1.0`   | The value multiplied with `uv.y`

All of these values are restricted to the range [-1, 1]. While negative values are allowed, they will only produce sane results if the texture sampler's `wrap` mode is `REPEAT`, or if the result of the final UV transformation is within the range [0, 1] (i.e. negative scale settings and correspondingly positive offsets).

### JSON Schema

[EXT_texture_offset_scale.textureInfo.schema.json](schema/EXT_texture_offset_scale.textureInfo.schema.json)

### Example JSON

This example utilizes only the top right quadrant of the source image.

```json
{
	"materials": [
		{
			"emissionTexture": {
				"source": 0,
				"extensions": {
					"EXT_texture_offset_scale": {
						"offsetS": 0.5,
						"scaleS": 0.5,
						"scaleT": 0.5
					}
				}
			}
		}
	]
}
```

This example inverts the T axis, effectively defining a bottom-left origin.

```json
{
	"materials": [
		{
			"emissionTexture": {
				"source": 0,
				"extensions": {
					"EXT_texture_offset_scale": {
						"offsetT": 1,
						"scaleT": -1
					}
				}
			}
		}
	]
}
```

## Known Implementations

* [UnityGLTF](https://github.com/KhronosGroup/UnityGLTF) (upcoming)

## Resources

*None*
