# AVR_sampler_offset_tile

## Contributors

* Steven Vergenz, AltspaceVR ([steven@altvr.com](mailto:steven@altvr.com))

## Status

Draft

## Dependencies

Written against the glTF draft 2.0 spec.

## Overview

Many techniques can be used to optimize resource usage for a 3d scene. Chief among them is the ability to minimize the number of textures the GPU must load. To achieve this, many engines encourage packing many objects' low-resolution textures into a single large texture atlas. The region of the resulting atlas that corresponds with each object is then defined by vertical and horizontal offsets, and the width and height of the region.

To support this use case, this extension adds `offsetS`, `offsetT`, `tileS`, and `tileT` properties to the sampler structure. These properties would typically be implemented like this in GLSL: `vec2 uvTransformed = uvFromMeshData * vec2(tileS, tileT) + vec2(offsetS, offsetT);`

## glTF Schema Updates

The `AVR_sampler_offset_tile` extension may be defined on `sampler` structures. It may contain the following properties:

| Name      | Type    | Default | Description
----------------------------------------------
| `offsetS` | `float` | `0.0`   | The value added to `uv.x`
| `offsetT` | `float` | `0.0`   | The value added to `uv.y`
| `tileS`   | `float` | `1.0`   | The value multiplied with `uv.x`
| `tileT`   | `float` | `1.0`   | The value multiplied with `uv.y`

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.

## Known Implementations

* TODO: List of known implementations, with links to each if available.

## Resources

* TODO: Resources, if any.