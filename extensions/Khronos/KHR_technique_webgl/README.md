# KHR_technique_webgl

## Contributors

* TODO: Name, affiliation, and contact info for each contributor

## Status

Stub. DO NOT IMPLEMENT!

## Dependencies

Written against the glTF 2.0 spec.

## Overview

TODO

## glTF Schema Updates


### Techniques

A technique describes the shading used for a material. The asset's techniques are stored in the `techniques` dictionary property.

The following example shows a technique and the properties it defines. This section describes each property in detail.

```json
{
    "techniques": [
        {
            "parameters": {
                "normal": {
                    "type": 35665
                },
                "position": {
                    "type": 35665
                },
                "texcoord0": {
                    "type": 35664
                },
                "ambient": {
                    "type": 35666
                },
                "diffuse": {
                    "type": 35678
                },
                "emission": {
                    "type": 35666
                },
                "light0Color": {
                    "type": 35665,
                    "value": [
                        1,
                        1,
                        1
                    ]
                },
                "shininess": {
                    "type": 5126
                },
                "specular": {
                    "type": 35678
                },
                "light0Transform": {
                    "semantic": "MODELVIEW",
                    "node": 1,
                    "type": 35676
                },
                "modelViewMatrix": {
                    "semantic": "MODELVIEW",
                    "type": 35676
                },
                "normalMatrix": {
                    "semantic": "MODELVIEWINVERSETRANSPOSE",
                    "type": 35676
                },
                "projectionMatrix": {
                    "semantic": "PROJECTION",
                    "type": 35676
                }
            },
            "attributes": {
                "a_normal": "normal",
                "a_position": "position",
                "a_texcoord0": "texcoord0"
            },
            "program": 0,
            "uniforms": {
                "u_ambient": "ambient",
                "u_diffuse": "diffuse",
                "u_emission": "emission",
                "u_shininess": "shininess",
                "u_specular": "specular",
                "u_light0Color": "light0Color",
                "u_light0Transform": "light0Transform",
                "u_modelViewMatrix": "modelViewMatrix",
                "u_normalMatrix": "normalMatrix",
                "u_projectionMatrix": "projectionMatrix"
            },
            "states": {
                "enable": [
                    2884,
                    2929
                ]
            }
        }
    ]
}
```

#### Parameters

Each technique has zero or more parameters; each parameter is defined by a type (GL types such as a floating point number, vector, texture, etc.), a default value, and potentially a semantic describing how the runtime is to interpret the data to pass to the shader. When a material instances a technique, the name of each supplied value in its `values` property corresponds to one of the parameters defined in the technique.

The above example illustrates several parameters. The property `ambient` is defined as a `FLOAT_VEC4` type; `diffuse` is defined as a `SAMPLER_2D`; and `light0color` is defined as a `FLOAT_VEC3` with a default color value of white.


#### Semantics

Technique parameters may also optionally define a *semantic*, an enumerated value describing how the runtime is to interpret the data to be passed to the shader.

In the above example, the parameter `light0Transform` defines the `MODELVIEW` semantic, which corresponds to the world space position of the node referenced in the property `node`, in this case the node `directionalight1`, which refers to a node that contains a light source.

If no `node` property is supplied for a semantic, the semantic is implied in a context-specific manner: either to the node which is being rendered, or in the case of camera-specific semantics, to the current camera. In the following fragment, which defines a parameter named `projectionMatrix` that is derived from the implementation's projection matrix, the semantic would be applied to the camera.

```json
{
    "techniques": [
        {
            "parameters": {
                "projectionMatrix": {
                    "semantic": "PROJECTION",
                    "type": 35676
                }
            }
        }
    ]
}
```

Table 1. Uniform Semantics

| Semantic                     | Type         | Description |
|:----------------------------:|:------------:|-------------|
| `LOCAL`                      | `FLOAT_MAT4` | Transforms from the node's coordinate system to its parent's.  This is the node's matrix property (or derived matrix from translation, rotation, and scale properties). |
| `MODEL`                      | `FLOAT_MAT4` | Transforms from model to world coordinates using the transform's node and all of its ancestors. |
| `VIEW`                       | `FLOAT_MAT4` | Transforms from world to view coordinates using the active camera node. |
| `PROJECTION`                 | `FLOAT_MAT4` | Transforms from view to clip coordinates using the active camera node. |
| `MODELVIEW`                  | `FLOAT_MAT4` | Combined `MODEL` and `VIEW`. |
| `MODELVIEWPROJECTION`        | `FLOAT_MAT4` | Combined `MODEL`, `VIEW`, and `PROJECTION`. |
| `MODELINVERSE`               | `FLOAT_MAT4` | Inverse of `MODEL`. |
| `VIEWINVERSE`                | `FLOAT_MAT4` | Inverse of `VIEW`. |
| `PROJECTIONINVERSE`          | `FLOAT_MAT4` | Inverse of `PROJECTION`. |
| `MODELVIEWINVERSE`           | `FLOAT_MAT4` | Inverse of `MODELVIEW`. |
| `MODELVIEWPROJECTIONINVERSE` | `FLOAT_MAT4` | Inverse of `MODELVIEWPROJECTION`. |
| `MODELINVERSETRANSPOSE`      | `FLOAT_MAT3` | The inverse-transpose of `MODEL` without the translation.  This translates normals in model coordinates to world coordinates. |
| `MODELVIEWINVERSETRANSPOSE`  | `FLOAT_MAT3` | The inverse-transpose of `MODELVIEW` without the translation.  This translates normals in model coordinates to eye coordinates. |
| `VIEWPORT`                   | `FLOAT_VEC4` | The viewport's x, y, width, and height properties stored in the `x`, `y`, `z`, and `w` components, respectively.  For example, this is used to scale window coordinates to [0, 1]: `vec2 v = gl_FragCoord.xy / viewport.zw;` |
| `JOINTMATRIX`                | `FLOAT_MAT4[]` | Array parameter; its length (`parameter.count`) must be greater than or equal to the length of `jointNames` array of a skin being used. Each element transforms mesh coordinates for a particular joint for skinning and animation. |

For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_SIMULATION_TIME`.

#### Program Instances

The `program` property of a technique creates an instance of a shader program. The value of the property is the index of a program defined in the asset's `programs` dictionary object (see next section). A shader program may be instanced multiple times within the glTF asset.

Attributes and uniforms passed to the program instance's shader code are defined in the `attributes` and `uniforms` properties of the technique, respectively. The following example shows the definitions for a technique's program instance, attributes and techniques:


```json
{
    "techniques": [
        {
            "parameters": {},
            "attributes": {
                "a_normal": "normal",
                "a_position": "position",
                "a_texcoord0": "texcoord0"
            },
            "program": 0,
            "uniforms": {
                "u_ambient": "ambient",
                "u_diffuse": "diffuse",
                "u_emission": "emission",
                "u_light0Color": "light0Color",
                "u_light0Transform": "light0Transform",
                "u_modelViewMatrix": "modelViewMatrix",
                "u_normalMatrix": "normalMatrix",
                "u_projectionMatrix": "projectionMatrix",
                "u_shininess": "shininess",
                "u_specular": "specular"
            }
        }
    ]
}
```

The `attributes` property specifies the vertex attributes of the data that will be passed to the shader. Each attribute's name is a string that corresponds to the attribute name in the GLSL source code. Each attribute's value is a string that references a parameter defined in the technique's `parameters` property, where the type and semantic of the attribute is defined.

The `uniforms` property specifies the uniform variables that will be passed to the shader. Each uniform's name is a string that corresponds to the uniform name in the GLSL source code. Each uniform's value is a string that references a parameter defined in the technique's `parameters` property, where the type and semantic of the uniform is defined.

#### Render States

Render states define the fixed-function GL state when a primitive is rendered. The technique's `states` property contains two properties:

* `enable`: an array of integers corresponding to Boolean GL states that should be enabled using GL's `enable` function.
* `functions`: a dictionary object containing properties corresponding to the names of GL state functions to call.  Each property is an array, where the elements correspond to the arguments of the GL function.

Valid values for elements in the `enable` array are `3042` (`BLEND`), `2884` (`CULL_FACE`), `2929` (`DEPTH_TEST`), `32823` (`POLYGON_OFFSET_FILL`), and `32926` (`SAMPLE_ALPHA_TO_COVERAGE`).  If any of these values are not in the array, the GL state should be disabled (which is the GL default state).  If the `enable` array is not defined in the `states`, all of these Boolean GL states are disabled.

Each property in `functions` indicates a GL function to call and the arguments to provide.  Valid property names are `"blendColor"`, `"blendEquationSeparate"`, `"blendFuncSeparate"`, `"colorMask"`, `"cullFace"`, `"depthFunc"`, `"depthMask"`, `"depthRange"`, `"frontFace"`, `"lineWidth"`, and `"polygonOffset"`.  If a property is not defined, the GL state for that function should be set to the default value(s) shown in the example below.

The following example `states` object indicates to enable all Boolean states (see the `enable` array) and use the default values for all the GL state functions (which could be omitted).

```json
{
    "techniques": [
        {
            "states": {
                "enable": [
                    3042,
                    2884,
                    2929,
                    32823,
                    32926
                ],
                "functions": {
                    "blendColor": [0.0, 0.0, 0.0, 0.0],
                    "blendEquationSeparate": [
                        32774,
                        32774
                    ],
                    "blendFuncSeparate": [1, 0, 1, 0],
                    "colorMask": [true, true, true, true],
                    "cullFace": [1029],
                    "depthFunc": [513],
                    "depthMask": [true],
                    "depthRange": [0.0, 1.0],
                    "frontFace": [2305],
                    "lineWidth": [1.0],
                    "polygonOffset": [0.0, 0.0]
                }
            }
        }
    ]
}
```

The following example shows a typical `"states"` object for closed opaque geometry.  Culling and the depth test are enabled, and all other GL states are set to the default value (disabled).
```json
{
    "techniques": [
        {
			"states": {
	    		"enable": [
	        		2884,
	        		2929
	    		]
			}
		}
	]
}
```

> **Implementation Note**: It is recommended that a runtime use the minimal number of GL state function calls.  This generally means ordering draw calls by technique, and then making GL state function calls only for the states that vary between techniques.


#### Programs

GLSL shader programs are stored in the asset's `programs` property. This property contains one or more objects, one for each program.

Each shader program includes an `attributes` property, which specifies the vertex attributes that will be passed to the shader, and the properties `fragmentShader` and `vertexShader`, which reference the files for the fragment and vertex shader GLSL source code, respectively.

```json
{
    "programs": [
        {
            "attributes": [
                "a_normal",
                "a_position",
                "a_texcoord0"
            ],
            "fragmentShader": 0,
            "vertexShader": 1
        }
    ]
}
```

#### Shaders

Shader source files are stored in the asset's `shaders` dictionary property, which contains one or more shader source files. Each shader specifies a `type` (vertex or fragment, defined as GL enum types) and a `uri` to the file (or a reference to a bufferView). Shader URIs may be URIs to external files or data URIs, allowing the shader content to be embedded as base64-encoded data in the asset.

```json
{
	"shaders": [
      {
          "type": 35633,
          "bufferView": 4
      },
    	{
        	"type": 35632,
        	"uri": "duck0FS.glsl"
    	},
    	{
        	"type": 35633,
        	"uri": "duck0VS.glsl"
    	}
	]
}
```    
### Properties Reference

|   |Type|Description|Required|
|---|----|-----------|--------|
|**programs**|`object`|A dictionary object of [`program`](#reference-program) objects.|No, default: `{}`|
|**shaders**|`object`|A dictionary object of [`shader`](#reference-shader) objects.|No, default: `{}`|
|**techniques**|`object`|A dictionary object of [`technique`](#reference-technique) objects.|No, default: `{}`|

### glTF.programs

A dictionary object of shader [`program`](#reference-program) objects.  The name of each program is an ID in the global glTF namespace that is used to reference the program.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`

### glTF.shaders

A dictionary object of [`shader`](#reference-shader) objects.  The name of each shader is an ID in the global glTF namespace that is used to reference the shader.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`

### glTF.techniques

A dictionary object of [`technique`](#reference-technique) objects.  The name of each technique is an ID in the global glTF namespace that is used to reference the technique.  A technique is a template for a material appearance.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`


<!-- ======================================================================= -->
<a name="reference-program"></a>
## program

A shader program, including its vertex and fragment shader, and names of vertex shader attributes.

**Related WebGL functions**: `attachShader()`, `bindAttribLocation()`, `createProgram()`, `deleteProgram()`, `getProgramParameter()`, `getProgramInfoLog()`, `linkProgram()`, `useProgram()`, and `validateProgram()`

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**attributes**|`string[]`|Names of GLSL vertex shader attributes.|No, default: `[]`|
|**fragmentShader**|`string`|The ID of the fragment [`shader`](#reference-shader).| :white_check_mark: Yes|
|**vertexShader**|`string`|The ID of the vertex [`shader`](#reference-shader).| :white_check_mark: Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [program.schema.json](schema/program.schema.json)
* **Example**: [programs.json](schema/examples/programs.json)

### program.attributes

Names of GLSL vertex shader attributes.

* **Type**: `string[]`
   * Each element in the array must have length between `1` and `256`.
* **Required**: No, default: `[]`
* **Related WebGL functions**: `bindAttribLocation()`

### program.fragmentShader :white_check_mark:

The ID of the fragment [`shader`](#reference-shader).

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**: ` >= 1`

### program.vertexShader :white_check_mark:

The ID of the vertex [`shader`](#reference-shader).

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**: ` >= 1`

### program.name

The user-defined name of this object.  This is not necessarily unique, e.g., a program and a buffer could have the same name, or two programs could even have the same name.

* **Type**: `string`
* **Required**: No

### program.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### program.extras

Application-specific data.

* **Type**: `any`
* **Required**: No

<!-- ======================================================================= -->
<a name="reference-shader"></a>
## shader

A vertex or fragment shader.

**Related WebGL functions**: `createShader()`, `deleteShader()`, `shaderSource()`, `compileShader()`, `getShaderParameter()`, and `getShaderInfoLog()`

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**uri**|`string`|The uri of the GLSL source.| :white_check_mark: Yes|
|**type**|`integer`|The shader stage.| :white_check_mark: Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [shader.schema.json](schema/shader.schema.json)
* **Example**: [shaders.json](schema/examples/shaders.json)

### shader.uri :white_check_mark:

The uri of the GLSL source.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.

* **Type**: `string`
* **Required**: Yes
* **Format**: uri

### shader.type :white_check_mark:

The shader stage.  Allowed values are `35632` (FRAGMENT_SHADER) and `35633` (VERTEX_SHADER).

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**: `35632`, `35633`

### shader.name

The user-defined name of this object.  This is not necessarily unique, e.g., a shader and a buffer could have the same name, or two shaders could even have the same name.

* **Type**: `string`
* **Required**: No

### shader.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### shader.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-technique"></a>
## technique

A template for material appearances.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**parameters**|`object`|A dictionary object of [`technique.parameters`](#reference-technique.parameters) objects.|No, default: `{}`|
|**attributes**|`object`|A dictionary object of strings that maps GLSL attribute names to technique parameter IDs.|No, default: `{}`|
|**program**|`string`|The ID of the program.| :white_check_mark: Yes|
|**uniforms**|`object`|A dictionary object of strings that maps GLSL uniform names to technique parameter IDs.|No, default: `{}`|
|**states**|[`technique.states`](#reference-technique.states)|Fixed-function rendering states.|No, default: `{}`|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [technique.schema.json](schema/technique.schema.json)
* **Example**: [techniques.json](schema/examples/techniques.json)

### technique.parameters

A dictionary object of [`technique.parameters`](#reference-technique.parameters) objects.  Each parameter defines an attribute or uniform input, and an optional semantic and value.  Each parameter must be referenced by the attributes or uniforms dictionary properties.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`

### technique.attributes

A dictionary object of strings that maps GLSL attribute names to technique parameter IDs.  Each string must also be in the parameters dictionary object.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `string`

### technique.program :white_check_mark:

The ID of the program.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**: ` >= 1`

### technique.uniforms

A dictionary object of strings that maps GLSL uniform names to technique parameter IDs.  Each string must also be in the parameters dictionary object.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `string`

### technique.states

Fixed-function rendering states.

* **Type**: [`technique.states`](#reference-technique.states)
* **Required**: No, default: `{}`

### technique.name

The user-defined name of this object.  This is not necessarily unique, e.g., a technique and a buffer could have the same name, or two techniques could even have the same name.

* **Type**: `string`
* **Required**: No

### technique.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### technique.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-technique.parameters"></a>
## technique.parameters

An attribute or uniform input to a [`technique`](#reference-technique), and an optional semantic and value.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**count**|`integer`|When defined, the parameter is an array of count elements of the specified type.  Otherwise, the parameter is not an array.|No|
|**node**|`string`|The id of the [`node`](#reference-node) whose transform is used as the parameter's value.|No|
|**type**|`integer`|The datatype.| :white_check_mark: Yes|
|**semantic**|`string`|Identifies a parameter with a well-known meaning.|Depends (see below)|
|**value**|`number[]`, `boolean[]`, or `string[]`|The value of the parameter.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [technique.parameters.schema.json](schema/technique.parameters.schema.json)

### parameter.count

When defined, the parameter is an array of `count` elements of the specified type.  Otherwise, the parameter is not an array.  When defined, `value`'s length equals to `count`, times the number of components in the `type`, e.g., `3` for `FLOAT_VEC3`.

An array parameter of scalar values is not the same as a vector parameter of the same size; for example, when `count` is `2` and `type` is `5126` (FLOAT), the parameter is an array of two floating-point values, not a `FLOAT_VEC2`.

When a parameter is an attribute, `count` can not be defined since GLSL does not support arrays of attributes.  When a parameter is a uniform and a glTF-defined semantic is used, the semantic must be `JOINTMATRIX`; application-specific parameters can be arrays and, therefore, define `count`.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 1`

### parameter.node

The id of the [`node`](#reference-node) whose transform is used as the parameter's value.  When this is defined, `type` must be `35676` (FLOAT_MAT4), therefore, when the semantic is `"MODELINVERSETRANSPOSE"`, `"MODELVIEWINVERSETRANSPOSE"`, or `"VIEWPORT"`, the `node` property can't be defined.

* **Type**: `string`
* **Required**: No
* **Minimum Length**: ` >= 1`

### parameter.type :white_check_mark:

The datatype. All valid values correspond to WebGL enums and depend on whether parameter is attribute or uniform. Allowed values are `5120` (BYTE), `5121` (UNSIGNED_BYTE), `5122` (SHORT), `5123` (UNSIGNED_SHORT), `5124` (INT), `5125` (UNSIGNED_INT), `5126` (FLOAT), `35664` (FLOAT_VEC2), `35665` (FLOAT_VEC3), `35666` (FLOAT_VEC4), `35667` (INT_VEC2), `35668` (INT_VEC3), `35669` (INT_VEC4), `35670` (BOOL), `35671` (BOOL_VEC2), `35672` (BOOL_VEC3), `35673` (BOOL_VEC4), `35674` (FLOAT_MAT2), `35675` (FLOAT_MAT3), `35676` (FLOAT_MAT4), and `35678` (SAMPLER_2D).

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**: `5120`, `5121`, `5122`, `5123`, `5124`, `5125`, `5126`, `35664`, `35665`, `35666`, `35667`, `35668`, `35669`, `35670`, `35671`, `35672`, `35673`, `35674`, `35675`, `35676`, `35678`

### parameter.semantic

Identifies a parameter with a well-known meaning.  Uniform semantics include `"LOCAL"` (FLOAT_MAT4), `"MODEL"` (FLOAT_MAT4), `"VIEW"` (FLOAT_MAT4), `"PROJECTION"` (FLOAT_MAT4), `"MODELVIEW"` (FLOAT_MAT4), `"MODELVIEWPROJECTION"` (FLOAT_MAT4), `"MODELINVERSE"` (FLOAT_MAT4), `"VIEWINVERSE"` (FLOAT_MAT4), `"PROJECTIONINVERSE"` (FLOAT_MAT4), `"MODELVIEWINVERSE"` (FLOAT_MAT4), `"MODELVIEWPROJECTIONINVERSE"` (FLOAT_MAT4), `"MODELINVERSETRANSPOSE"` (FLOAT_MAT3), `"MODELVIEWINVERSETRANSPOSE"` (FLOAT_MAT3), `"VIEWPORT"` (FLOAT_VEC4), `"JOINTMATRIX"` (FLOAT_MAT4[]).  Attribute semantics include `"POSITION"`, `"NORMAL"`, `"TEXCOORD"`, `"COLOR"`, `"JOINT"`, and `"WEIGHT"`.  `"TEXCOORD"` and `"COLOR"` attribute semantic property names must be of the form `[semantic]_[set_index]`, e.g., `"TEXCOORD_0"`, `"TEXCOORD_1"`, `"COLOR_1"` etc.  For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_TEMPERATURE`.

* **Type**: `string`
* **Required**: No, except this property is required when the parameter is an attribute, i.e., when the parameter is referenced from [`technique.attributes`](#reference-technique.attributes).

### parameter.value

The value of the parameter. The length is determined by the values of the `type` and `count` (if present) properties.  A [`material`](#reference-material) value with the same name, when specified, overrides this value.  This must be `undefined` when the parameter is an attribute, i.e., it is in [technique.attributes](#reference-technique.attributes).

* **Type of each property**: `number[]`, `boolean[]`, or `string[]`
* **Required**: No

### parameter.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### parameter.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-technique.states"></a>
## states

Fixed-function rendering states.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**enable**|`integer[]`|WebGL states to enable.|No, default: `[]`|
|**functions**|[`technique.states.functions`](#reference-technique.states.functions)|Arguments for fixed-function rendering state functions other than `enable()`/`disable()`.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [technique.states.schema.json](schema/technique.states.schema.json)

### states.enable

WebGL states to enable.  States not in the array are disabled.  Valid values for each element correspond to WebGL enums: `3042` (BLEND), `2884` (CULL_FACE), `2929` (DEPTH_TEST), `32823` (POLYGON_OFFSET_FILL), and `32926` (SAMPLE_ALPHA_TO_COVERAGE).

* **Type**: `integer[]`
   * Each element in the array must be unique.
   * Each element in the array must be one of the following values: `3042`, `2884`, `2929`, `32823`, `32926`, `3089`.
* **Required**: No, default: `[]`
* **Related WebGL functions**: `enable()` and `disable()`

### states.functions

Arguments for fixed-function rendering state functions other than `enable()`/`disable()`.

* **Type**: [`technique.states.functions`](#reference-technique.states.functions)
* **Required**: No

### states.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### states.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-technique.states.functions"></a>
## functions

Arguments for fixed-function rendering state functions other than `enable()`/`disable()`.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**blendColor**|`number[4]`|Floating-point values passed to `blendColor()`. [red, green, blue, alpha]|No, default: `[0,0,0,0]`|
|**blendEquationSeparate**|`integer[2]`|Integer values passed to `blendEquationSeparate()`.|No, default: `[32774,32774]`|
|**blendFuncSeparate**|`integer[4]`|Integer values passed to `blendFuncSeparate()`.|No, default: `[1,0,1,0]`|
|**colorMask**|`boolean[4]`|Boolean values passed to `colorMask()`. [red, green, blue, alpha].|No, default: `[true,true,true,true]`|
|**cullFace**|`integer[1]`|Integer value passed to `cullFace()`.|No, default: `[1029]`|
|**depthFunc**|`integer[1]`|Integer values passed to `depthFunc()`.|No, default: `[513]`|
|**depthMask**|`boolean[1]`|Boolean value passed to `depthMask()`.|No, default: `[true]`|
|**depthRange**|`number[2]`|Floating-point values passed to `depthRange()`. [zNear, zFar]|No, default: `[0,1]`|
|**frontFace**|`integer[1]`|Integer value passed to `frontFace()`.|No, default: `[2305]`|
|**lineWidth**|`number[1]`|Floating-point value passed to `lineWidth()`.|No, default: `[1]`|
|**polygonOffset**|`number[2]`|Floating-point value passed to `polygonOffset()`.  [factor, units]|No, default: `[0,0]`|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [technique.states.functions.schema.json](schema/technique.states.functions.schema.json)

### functions.blendColor

Floating-point values passed to `blendColor()`. [red, green, blue, alpha]

* **Type**: `number[4]`
   * Each element in the array must be greater than or equal to `0` and less than or equal to `1`.
* **Required**: No, default: `[0,0,0,0]`
* **Related WebGL functions**: `blendColor()`

### functions.blendEquationSeparate

Integer values passed to `blendEquationSeparate()`. [rgb, alpha]. Valid values correspond to WebGL enums: `32774` (FUNC_ADD), `32778` (FUNC_SUBTRACT), and `32779` (FUNC_REVERSE_SUBTRACT).

* **Type**: `integer[2]`
   * Each element in the array must be one of the following values: `32774`, `32778`, `32779`.
* **Required**: No, default: `[32774,32774]`
* **Related WebGL functions**: `blendEquationSeparate()`

### functions.blendFuncSeparate

Integer values passed to `blendFuncSeparate()`. [srcRGB, dstRGB, srcAlpha, dstAlpha]. Valid values correspond to WebGL enums: `0` (ZERO), `1` (ONE), `768` (SRC_COLOR), `769` (ONE_MINUS_SRC_COLOR), `774` (DST_COLOR), `775` (ONE_MINUS_DST_COLOR), `770` (SRC_ALPHA), `771` (ONE_MINUS_SRC_ALPHA), `772` (DST_ALPHA), `773` (ONE_MINUS_DST_ALPHA), `32769` (CONSTANT_COLOR), `32770` (ONE_MINUS_CONSTANT_COLOR), `32771` (CONSTANT_ALPHA), `32772` (ONE_MINUS_CONSTANT_ALPHA), and `776` (SRC_ALPHA_SATURATE).

* **Type**: `integer[4]`
   * Each element in the array must be one of the following values: `0`, `1`, `768`, `769`, `774`, `775`, `770`, `771`, `772`, `773`, `32769`, `32770`, `32771`, `32772`, `776`.
* **Required**: No, default: `[1,0,1,0]`
* **Related WebGL functions**: `blendFuncSeparate()`

### functions.colorMask

Boolean values passed to `colorMask()`. [red, green, blue, alpha].

* **Type**: `boolean[4]`
* **Required**: No, default: `[true,true,true,true]`
* **Related WebGL functions**: `colorMask()`

### functions.cullFace

Integer value passed to `cullFace()`. Valid values correspond to WebGL enums: `1028` (FRONT), `1029` (BACK), and `1032` (FRONT_AND_BACK).

* **Type**: `integer[1]`
   * Each element in the array must be one of the following values: `1028`, `1029`, `1032`.
* **Required**: No, default: `[1029]`
* **Related WebGL functions**: `cullFace()`

### functions.depthFunc

Integer values passed to `depthFunc()`. Valid values correspond to WebGL enums: `512` (NEVER), `513` (LESS), `515` (LEQUAL), `514` (EQUAL), `516` (GREATER), `517` (NOTEQUAL), `518` (GEQUAL), and `519` (ALWAYS).

* **Type**: `integer[1]`
   * Each element in the array must be one of the following values: `512`, `513`, `515`, `514`, `516`, `517`, `518`, `519`.
* **Required**: No, default: `[513]`
* **Related WebGL functions**: `depthFunc()`

### functions.depthMask

Boolean value passed to `depthMask()`.

* **Type**: `boolean[1]`
* **Required**: No, default: `[true]`
* **Related WebGL functions**: `depthMask()`

### functions.depthRange

Floating-point values passed to `depthRange()`. zNear must be less than or equal to zFar. [zNear, zFar]

* **Type**: `number[2]`
   * Each element in the array must be greater than or equal to `0` and less than or equal to `1`.
* **Required**: No, default: `[0,1]`
* **Related WebGL functions**: `depthRange()`

### functions.frontFace

Integer value passed to `frontFace()`.  Valid values correspond to WebGL enums: `2304` (CW) and `2305` (CCW).

* **Type**: `integer[1]`
   * Each element in the array must be one of the following values: `2304`, `2305`.
* **Required**: No, default: `[2305]`
* **Related WebGL functions**: `frontFace()`

### functions.lineWidth

Floating-point value passed to `lineWidth()`.

* **Type**: `number[1]`
   * Each element in the array must be greater than `0`.
* **Required**: No, default: `[1]`
* **Related WebGL functions**: `lineWidth()`

### functions.polygonOffset

Floating-point value passed to `polygonOffset()`.  [factor, units]

* **Type**: `number[2]`
* **Required**: No, default: `[0,0]`
* **Related WebGL functions**: `polygonOffset()`

### functions.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### functions.extras

Application-specific data.

* **Type**: `any`
* **Required**: No





### JSON Schema

* [`program`](#reference-program)
* [`shader`](#reference-shader)
* [`technique`](#reference-technique)
   * [`parameters`](#reference-technique.parameters)
   * [`states`](#reference-technique.states)
      * [`functions`](#reference-technique.states.functions)


## Known Implementations

* TODO: List of known implementations, with links to each if available.

## Resources

* TODO: Resources, if any.
