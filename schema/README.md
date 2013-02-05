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
http://localhost/gltf/?json=examples/states/translucency.schema.json
```

Use `validate` to immediately validate the json with the schema:
```
http://localhost/gltf/?validate=true
```

The most common use is something like:
```
http://localhost/gltf/?schema=states.schema.json&json=examples/states/translucency.schema.json&validate=true
```

Also, JSON in general can be valdiated with [JSONLint](http://jsonlint.com/), which can be useful for validating the glTF schema itself.

# Schema Notes

The following notes can be used to create the glTF spec.

## Render states

See
   * Schema: [states.schema.json](states.schema.json)
      * All properties are optional and default to values from the [WebGL 1.0 spec](https://www.khronos.org/registry/webgl/specs/1.0/).
   * Examples:
      * [all.schema.json](examples/states/all.schema.json) - every state with it's default value.
      * [closed_opaque_object.schema.json](examples/states/closed_opaque_object.schema.json) - typical state for rendering a closed, opaque object: depth testing and backface culling.
      * [depth_test.schema.json](examples/states/depth_test.schema.json) - state with depth testing enabled.
      * [translucency.schema.json](examples/states/translucency.schema.json) - typical state for rendering a translucency object with alpha blending.

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
