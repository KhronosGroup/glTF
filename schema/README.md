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

## Comparison between COLLADA and glTF

### extra

glTF allows application-specific in most places using an `extra` property.  This is similar to `extra` in COLLADA, Pages 5-35 to 5-36, except `technique` is not required.

### id and name

COLLADA `id` attributes map to the name of an object, and the COLLADA `name` attribute maps to the name property of an object, for example:
```
<node id="nodeId" name="nodeName"> 
</node>
``` 
becomes
```javascript
"nodeId" : {
    "name" : "nodeName"
}
```

What's in COLLADA, but not in this version of glTF
* No `profile` or `technique`.  Instead, this can be negotiated via a REST API.  (platform, product name, etc.)

_TODO_
* Contributor/copyright/etc. (COLLADA `asset`) just at root?
* Does everything have a `name` property that needs one?

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

_TODO_
* _Schema_
   * _Need to figure out_ `up_axis`_, which is part of `asset` in COLLADA._
* _COLLADA2JSON_
   * _Loader and writer need to be updated to reflect the new organization and required properties, not all COLLADA properties._
   * _`yfov` is degrees; it should be radians since this is a final-stage format._

## Images

For compatibility with modern web browsers, the following image formats are supported: `.jpg`, `.png`, `.bmp`, and `.gif`.  The URL may be a [data URI](https://developer.mozilla.org/en/data_URIs) to facilitate storing all model assets in a single .json for easy deployment, drag and drop, etc.

In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:
* Renamed `mips_generate` to `generateMipmap`.

glTF 1.0 does not support:
* `image`: `renderable`, `create_2d`, `create_3d`, and `create_cube`.
* `init_from`: `array_index`, `mip_index`, `depth`, and `face`.

COLLADA References
* `library_images`.  Page 8-75.
* `image`.  Page 8-58 to 8-60.
* `init_from`.  Pages 8-62 to 8-63.

_TODO_
* _Schema_
   * _Rename to texture?_
   * _Support precomputed mips?_
   * _Allow blank texture as a render target and other ColladaFX features on 8-59?_
* _COLLADA2JSON_
   * _Add support for `generateMipmap`_
   * _Image conversion optimization step_
   * _Add option for data uri_
   
## Nodes

Nodes are based on [COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf), Pages 5-98 to 5-99.

In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:
* Only a single transform is supported, and it must be a 4x4 matrix.  COLLADA transformation elements such as `lookat` and `rotate` must be converted to a 4x4 matrix.  If several COLLADA transformation elements are used, they must be concatenated into a single 4x4 matrix.
* No `asset`.  COLLADA Pages 5-17 to 5-19.

_TODO_
   * _Schema_
      * _instance_camera_ - keep camera as a node in the scene graph?  What are the use cases?  Shadow mapping?_
      * _instance_controller_
      * _instance_light_
      * Allow only one? _meshes_, _camera_, or _lights_.
      * _Animations for all COLLADA transformation elements is going to be hard.  Need to scope it right._
   * _COLLADA2JSON_
      * _Convert all transformation elements to 4x4 matrix.  Do we already?_

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
* _Schema_
   * _Do we agree about separate precedence above?_
   * _I don't know anyone who uses `dither_enable` nowadays.  Is this something we want to include?_
   * _Currently, only a `line_width` of `1.0` is supported on most browsers on Windows because that is all that ANGLE supports._
   * _Need to look at OpenGL and OpenGL ES, which I think still includes `point_size_enable`._
* _COLLADA2JSON_
   * _Add to loader and writer.  Writer needs to derive state from common profile._