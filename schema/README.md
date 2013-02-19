This doc can be used to create the glTF spec.

* <a href="#validation">Validation</a>
* <a href="#guidingprinciples">Guiding Principles</a>
* <a href="#comparison">Comparison between COLLADA and glTF</a>
* <a href="#schema">Schema</a>
   * <a href="#asset">Asset</a>
   * <a href="#camera">Camera</a>
   * <a href="#image">Image</a>
   * <a href="#node">Node</a>
   * <a href="#states">States</a>
* <a href="#references">References</a>
* <a href="#acknowledgments">Acknowledgments</a>

<a name="validation">
# Validation

The glTF schema is written using [JSON Schema 03](http://tools.ietf.org/html/draft-zyp-json-schema-03).

An asset can be validated against the schema using the glTF Validator (based on [JSV](https://github.com/garycourt/JSV)).  Setup a local webserver to point to the `schema` directory, and browse to index.html.  For example:
```
http://localhost/gltf/
```

There are a few query parameters.

Use `schema` to provide the schema to validate against:
```
http://localhost/gltf/?schema=states.schema.json
```

Use `json` to provide the asset's JSON to validate:
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

<a name="guidingprinciples">
# Guiding Principles

* Keep the client simple, negotiate via a REST API instead of on the client, e.g., for a `technique`.
* When a tradeoff needs to be made, put pain on the converter, not the end user.
* ...

<a name="comparison">
# Comparison between COLLADA and glTF

## Naming

COLLADA uses underscores `like_this`; glTF uses [camel case](http://en.wikipedia.org/wiki/CamelCase) `likeThis`.  Camel case is a more common naming convention in JSON.

## Angles

COLLADA uses degrees for angles.  To better match GLSL and most math libraries, glTF uses radians.

## extra

glTF allows application-specific metadata in most places using an `extra` property.  This is similar to `extra` in COLLADA, Pages 5-35 to 5-36, except `technique` is not required.

_TODO_
* _COLLADA2JSON_
   * _Needs to output `extra` objects_.
* _Loader_
   * _Needs to pass through `extra` objects_.

Alternatively, glTF could allow application-specific properties anywhere, but this has the potential to break backwards compatibility, e.g., if an asset uses a property name that is then used in a future version of glTF.  Therefore, glTF does not allow additional properties not in the spec on any objects, except `extra`.

## id and name

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

The `name` property is the user-facing name, and the object name is used internally to link parts of the asset, e.g., reference a `mesh` from a `node`. 

What's in COLLADA, but not in this version of glTF
* No `profile` or `technique`.  Instead, this can be negotiated via a REST API.  (platform, product name, etc.)

_TODO_
* Does everything have a `name` property that needs one?
* Support _sid_, especially for reusing animations.

<a name="schema">
# Schema

<a name="asset">
## Asset

See
* Schema: [asset.schema.json](asset.schema.json)
* Examples:
   * [asset.json](examples/glTF/asset.json) - Bare glTF model with an asset property.
   * [all.json](examples/asset/all.json) - Every property with example values.

The design of `asset` in COLLADA is focused on asset interchange for assets potentially authored by multiple artists.  In glTF, `asset` is used to credit all artists for display purposes.

Differences from COLLADA:
* `asset` is only defined on the root glTF property; in COLLADA, it is available as a child of many elements.  In practice, it is usually only a child of the `COLLADA` root node.
* `asset` just contains `copyright` that can be used to credit the model author(s), and `geographicLocation`.  If an application needs more information, it can be stored in the `extra` property.

[COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf) References
* `asset`. Pages 5-17 to 5-19
* `contributor`. Pages 5-27 to 5-28.
* `geographic_location`. Pages 5-40 to 5-41.

_TODO_
* _Schema_
   * _Need `unit` property?  Or always meters and apply a scale matrix to the root?_
   * _Also need url for copyright image?_
   * _Need to figure out_ `up_axis`.
* _COLLADA2JSON_
   * _Create `asset.copyright` from COLLADA author and copyright?_
   * _Convert COLLADA geographic_location.  glTF uses radians, not degrees._
   * _Convert kml/kmz location to geographic_location.  Convert kml/kmz orientation and scale to a transform._
   
<a name="camera">
## Camera

See
* Schema:
   * [camera.schema.json](camera.schema.json)
   * [perspective.schema.json](perspective.schema.json)
   * [orthographic.schema.json](orthographic.schema.json).
* Examples
   * [cameras.json](examples/glTF/cameras.json) - Bare glTF model with two cameras.
   * [orthographic_all.json](examples/camera/orthographic_all.json) - Orthographic camera using all properties.
   * [perspective.json](examples/camera/perspective.json) - Perspective camera using required properties.
   * [perspective_all.json](examples/camera/perspective_all.json) - Perspective camera using all properties.

In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:

* No `imager`.  COLLADA Pages 5-45 to 5-46.
* No `optics`.  COLLADA Pages 5-100 to 5-101.  `orthographic` and `perspective` are directly part of the camera.
   * A glTF camera represents a single camera; it doesn't have techniques.  This can be negotiated via a REST API.
* In COLLADA, both `orthographic` and `perspective` allow several different options for required properties.  glTF provides only one set of required properties for `orthographic` and `perspective` cameras, which map most commonly to computing orthographic and perspective projection matrices.  See below.

For `orthographic`, the following properties are required:
* `xmag` or `ymag`
   
For `perspective`, the following properties are required:
* `aspect_ratio`
* `yfov`

[COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf) References
* `camera`.  Pages 5-21 to 5-22.
* `orthographic`.  Pages 5-102 to 5-103.
* `perspective`.  Pages 5-108 to 5-109.

_TODO_
* _COLLADA2JSON_
   * _Loader and writer need to be updated to reflect the new organization and required properties, not all COLLADA properties._
   * _`yfov` is degrees; it should be radians since this is a final-stage format._

<a name="image">
## Image

See
* Schema: [image.schema.json](image.schema.json)
* Examples
   * [images.json](examples/glTF/images.json) - Bare glTF model with two images.
   * [all.json](examples/image/all.json) - Every property with example values.

For compatibility with modern web browsers, the following image formats are supported: `.jpg`, `.png`, `.bmp`, and `.gif`.  The URL may be a [data URI](https://developer.mozilla.org/en/data_URIs) to facilitate storing all model assets in a single .json for easy deployment, drag and drop, etc.

In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:
* Renamed `mips_generate` to `generateMipmap`.

glTF 1.0 does not support:
* `image`: `renderable`, `create_2d`, `create_3d`, and `create_cube`.
* `init_from`: `array_index`, `mip_index`, `depth`, and `face`.

[COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf) References
* `library_images`.  Page 8-75.
* `image`.  Page 8-58 to 8-60.
* `init_from`.  Pages 8-62 to 8-63.

_TODO_
* _Schema_
   * _Support precomputed mips?_
   * _Allow blank texture as a render target and other ColladaFX features on 8-59?_
* _COLLADA2JSON_
   * _Add support for `generateMipmap`_
   * _Image conversion to `.jpg` or `.png`_
   * _Add option for data uri_

<a name="node">
## Node

See
* Schema: [node.schema.json](node.schema.json)
* Examples
   * [nodes.json](examples/glTF/nodes.json) - Bare glTF model with two nodes.
   * [meshes.json](examples/node/meshes.json) - An interior node with a mesh (and children).
   * [meshes_leaf.json](examples/node/meshes_leaf.json) - A leaf node with a mesh.

In order to better map to OpenGL, OpenGL ES, and WebGL, glTF differs from COLLADA in the following ways:
* Only a single transform is supported, and it must be a 4x4 matrix.  COLLADA transformation elements such as `lookat` and `rotate` must be converted to a 4x4 matrix.  If several COLLADA transformation elements are used, they must be concatenated into a single 4x4 matrix.

[COLLADA 1.5](http://www.khronos.org/files/collada_spec_1_5.pdf) References
* `node.  Pages 5-98 to 5-99.

_TODO_
   * _Schema_
      * _instance_camera_ - keep camera as a node in the scene graph?  What are the use cases?  Shadow mapping?_
      * _instance_controller_
      * _instance_light_
      * Allow only one? _meshes_, _camera_, or _lights_.
      * _Animations for all COLLADA transformation elements is going to be hard.  Need to scope it right._
   * _COLLADA2JSON_
      * _Convert all transformation elements to 4x4 matrix.  Do we already?_

<a name="states">
## States

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
   
<a name="references">
# References

* [COLLADA 1.5 spec](http://www.khronos.org/files/collada_spec_1_5.pdf)
* [WebGL 1.0 spec](https://www.khronos.org/registry/webgl/specs/1.0/)

<a name="acknowledgments">
# Acknowledgments

* Tom Fili, Analytical Graphics, Inc., answers all of Patrick's COLLADA questions.

