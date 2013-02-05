The glTF schema is written using [JSON Schema 03](http://tools.ietf.org/html/draft-zyp-json-schema-03).

An asset can be validated against the schema using the glTF Validator (based on [JSV](https://github.com/garycourt/JSV)).  Setup a local webserver to point to this directory, and browse to index.html.  For example:
```
http://localhost/gltf/
```

There are a few query parameters.

Use `schema` to provide the schema to validate against:
```
http://localhost/gltf/?schema=states.schema.json
```

Use `json` to provide the json to validate:
```
http://localhost/gltf/?json=examples/states/translucency.json
```

Use `validate` to immediately validate the json with the schema:
```
http://localhost/gltf/?validate=true
```

The most common use is something like:
```
http://localhost/gltf/?schema=states.schema.json&json=examples/states/translucency.json&validate=true
```

Also, JSON in general can be valdiated with [JSONLint](http://jsonlint.com/), which can be useful for validating the glTF schema itself.

# Schema Notes

The following notes can be used to create the glTF spec.

## General differences between COLLADA and glTF

_TODO_
* _No `extra`.  COLLADA Pages 5-35 to 5-36.  How exactly do we extend glTF?  Should be trivial, e.g., `extra : { whatever-custom-schema }`._

## Camera

See
* Schema: [camera.schema.json](camera.schema.json), [perspective.schema.json](perspective.schema.json), and [orthographic.schema.json](orthographic.schema.json).
* Examples
   * [cameras.json](examples/glTF/cameras.json) - Bare glTF model with two cameras.
   * [orthographic_all.json](examples/camera/orthographic_all.json) - Orthographic camera using all properties.
   * [perspective.json](examples/camera/perspective.json) - Perspective camera using required properties.
   * [perspective_all.json](examples/camera/perspective_all.json) - Perspective camera using all properties.

Cameras are based on [COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf), Pages 5-21 to 5-22, 5-102 to 5-103, and 5-108 to 5-109.

In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:

* No `asset`.  COLLADA Pages 5-17 to 5-19.
* No `imager`.  COLLADA Pages 5-45 to 5-46.
* No `optics`.  COLLADA Pages 5-100 to 5-101.  `orthographic` and `perspective` are directly part of the camera.
   * A glTF camera represents a single camera; it doesn't have techniques.  This can be negotiated via a REST API.
* In COLLADA, both `orthographic` and `perspective` allow several different options for required properties.  glTF provides only one set of required properties for `orthographic` and `perspective` cameras, which map most commonly to computing orthographic and perspective projection matrices.  See below.

For `orthographic`, the following properties are required:
* `xmag` or `ymag`
   
For `perspective`, the following properties are required:
* `aspect_ratio`
* `yfov`

TODO: interaction with `node`.

_TODO_
* _COLLADA2JSON: loader and writer need to be updated to reflect the new organization and required properties, not all COLLADA properties._
* _COLLADA2JSON: `yfov` is degrees; it should be radians since this is a final-stage format._
* _Need to figure out_ `up_axis`_, which is part of `asset` in COLLADA._
* _This does not support off-center perspective projections or infinite perspective projections.  I'm sure that is fine._

## Render states

See
* Schema: [states.schema.json](states.schema.json)
   * All properties are optional and default to values from the [WebGL 1.0 spec](https://www.khronos.org/registry/webgl/specs/1.0/).
* Examples:
   * [all.json](examples/states/all.json) - every state with its default value.
   * [closed_opaque_object.json](examples/states/closed_opaque_object.json) - typical state for rendering a closed, opaque object: depth testing and backface culling.
   * [depth_test.json](examples/states/depth_test.json) - state with depth testing enabled.
   * [translucency.json](examples/states/translucency.json) - typical state for rendering a translucency object with alpha blending.

Render states are based on the GLES2 profile in [COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf), Pages 8-120 to 8-125.  In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:

* `blend_func`, renamed
   * `src` to `sfactor`
   * `dst` to `dfactor`
* `blend_func_separate`, renamed
   * `src_rgb` to `srcRGB`
   * `dest_rgb` to `dstRGB`
   * `src_alpha` to `srcAlpha`
   * `dest_alpha` to `dstAlpha`
* Changed `color_mask` from an array of four booleans to an object with four properties: `red`, `green`, `blue`, and `alpha`.
* Removed `point_size_enable`; instead, we just assign to `gl_PointSize` in WebGL based on `point_size`.
* `polygon_offset`
   * Replaced float2 array with `factor` and `units` properties.
* `scissor` `width` and `height` default to zero, not "When a GL context is first attached to a window, width and height are set to the dimensions of that window."
* Added `sample_coverage_enable`.

Like COLLADA, glTF includes:
* `blend_equation` and `blend_equation_separate`
* `both blend_func` and `blend_func_separate`
* `stencil_func` and `stencil_func_separate`
* `stencil_mask` and `stencil_mask_separate`
* `stencil_op` and `stencil_op_separate`

The separate version of a property takes precedence over its counterpart.  For example, if both `blend_equation` and `blend_equation_separate` are provided, `blend_equation_separate` takes precedence.
   
_TODO_
   * _Do we agree about separate precedence above?_
   * _I don't know anyone who uses `dither_enable` nowadays.  Is this something we want to include?_
   * _Currently, only a `line_width` of `1.0` is supported on most browsers on Windows because that is all that ANGLE supports._
   * _Need to look at OpenGL and OpenGL ES, which I think still includes `point_size_enable`._
