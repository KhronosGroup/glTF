# KHR_techniques_webgl

## Contributors

* TODO: Name, affiliation, and contact info for each contributor

## Status

Stub. DO NOT IMPLEMENT!

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF to define instances of shading techniques with external shader programs along with their parameterized values.  Shading techniques use JSON properties to describe data types and semantics for GLSL vertex and fragment shader programs.  

This extension specification is targeting WebGL 1.0, and can be supported in any WebGL 1.0-based engine if the device supports the necessary WebGL extensions.

## glTF Schema Updates

`KHR_techniques_webgl` introduces a list of the asset's shading techniques used in `techniques`. The technique and values are specified by adding a `KHR_techniques_webgl` extension to any glTF material.

### Extending Materials

If a `material` contains an `extensions` property and the `extensions` property defines its `KHR_techniques_webgl` property, then the specified shading technique can be used, as specified in the asset's `techniques`.

```json
"materials": [
    {
        "extensions": {
            "KHR_techniques_webgl": {
                "technique": 0,
                "values" : {
                    "u_light0Color": [
                        1.0,
                        0.8,
                        1.0
                    ]
                }
            }
        }
    }
],
```

#### Technique

The `technique` property points to the index of a technique listed in the asset's `KHR_techniques_webgl` `techniques` property.

#### Values

The `values` property defines the list of values, each of which is passed to the technique uniform of the same name, and when specified, overrides the corresponding uniform `value`.

## Extensions

`KHR_techniques_webgl` is defined in the asset's top level `extensions` property with the following additional values.

### Techniques

A technique describes the shading used for a material. The list of the asset's techniques are stored in the `techniques` property. Each technique must includes the `program` property and list any attribute or uniforms by name in the `attributes` and `uniforms` dictionary properties which define the objects to be passed as inputs to the program.

The following example shows a technique and the properties it defines. This section describes each property in detail.

```json
{
    "techniques": [
        {
            "program": 0,
            "attributes": {
                "a_normal":  {
                    "type": 35665,
                    "semantic": "NORMAL"
                },
                "a_position": {
                    "type": 35665,
                    "semantic": "POSITION"
                },
                "a_texcoord0": {
                    "type": 35664,
                    "semantic": "TEX_COORD"
                }
                },
            "uniforms": {
                "u_ambient": {
                    "type": 35666
                },
                "u_diffuse": {
                    "type": 35678
                },
                "u_emission": {
                    "type": 35666
                },
                "u_shininess": {
                    "type": 5126
                },
                "u_specular": {
                    "type": 35678
                },
                "u_light0Color": {
                    "type": 35665,
                    "value": [
                        1,
                        1,
                        1
                    ]
                },
                "u_light0Transform": {
                    "semantic": "MODELVIEW",
                    "node": 1,
                    "type": 35676
                },
                "u_modelViewMatrix": {
                    "semantic": "MODELVIEW",
                    "type": 35676
                },
                "u_normalMatrix": {
                    "semantic": "MODELVIEWINVERSETRANSPOSE",
                    "type": 35676
                },
                "u_projectionMatrix": {
                    "semantic": "PROJECTION",
                    "type": 35676
                }
            }
        }
    ]
}
```

#### Program Instances

The `program` property of a technique creates an instance of a shader program. The value of the property is the index of a program defined in the asset's `programs` dictionary object (see next section). A shader program may be instanced multiple times within the glTF asset.

Attributes and uniforms passed to the program instance's shader code are defined in the `attributes` and `uniforms` properties of the technique, respectively.

#### Attributes

The `attributes` dictionary property specifies the vertex attributes of the data that will be passed to the shader. Each attribute's name is a string that corresponds to the attribute name in the GLSL source code. Each attribute's value is an [attribute](#reference-technique.attributes) object, where the type (GL types such as a floating point number, vector, texture, etc.) and semantic of the attribute is defined.

#### Uniforms

The `uniforms` dictionary property specifies the uniform variables that will be passed to the shader. Each uniform's name is a string that corresponds to the uniform name in the GLSL source code. Each uniform's value is a string that references a [uniform](#reference-technique.uniforms) object, where the type (GL types such as a floating point number, vector, texture, etc.) and potentially the semantic and default value of the uniform is defined.

When a material instances a technique, the name of each supplied value in its `values` property must correspond to one of the uniforms defined in the technique.

The above example illustrates several uniforms. The property `u_ambient` is defined as a `FLOAT_VEC4` type; and `u_light0color` is defined as a `FLOAT_VEC3` with a default color value of white.

##### Semantics

Technique uniforms may also optionally define a *semantic*, an enumerated value describing how the runtime is to interpret the data to be passed to the shader.

In the above example, the uniform `u_light0Transform` defines the `MODELVIEW` semantic, which corresponds to the world space position of the node referenced in the property `node`, in this case the node `1`, which refers to the id of a node that contains a light source.

If no `node` property is supplied for a semantic, the semantic is implied in a context-specific manner: either to the node which is being rendered, or in the case of camera-specific semantics, to the current camera. In the following fragment, which defines a uniform named `u_projectionMatrix` that is derived from the implementation's projection matrix, the semantic would be applied to the camera.

```json
{
    "techniques": [
        {
            "uniforms": {
                "u_projectionMatrix": {
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
| `JOINTMATRIX`                | `FLOAT_MAT4[]` | Array parameter; its length (`uniform.count`) must be greater than or equal to the length of `jointNames` array of a skin being used. Each element transforms mesh coordinates for a particular joint for skinning and animation. |

For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_SIMULATION_TIME`.

#### Programs

GLSL shader programs are stored in the asset's `programs` property. This property contains one or more objects, one for each program.

Each shader program includes an `attributes` property, which specifies the vertex attributes that will be passed to the shader, and the properties `fragmentShader` and `vertexShader`, which reference the files for the fragment and vertex shader GLSL source code, respectively, as defined the id of the item in the `shaders` dictionary property.

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

Shader source files are stored in the asset's `shaders` dictionary property, which contains one or more shader source files. Each shader specifies a `type` (vertex or fragment, defined as GL enum types) and either a `uri` to the file or a reference to a bufferView. Shader URIs may be URIs to external files or data URIs, allowing the shader content to be embedded as base64-encoded data in the asset. 

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

## Properties Reference

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
|**glExtensions**|`string[]`|Names of WebGL 1.0 extensions to enable.|No, default: `[]`|
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

### program.glExtensions

Names of GLSL 1.0 extensions to enable.

* **Type**: `string[]`
   * Each element in the array must have length between `1` and `256`.
* **Required**: No, default: `[]`
* **Related WebGL functions**: `getExtension()`

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
|**uri**|`string`|The uri of the GLSL source.|No|
|**bufferView**|`integer`|The bufferView of the GLSL source.|No|
|**type**|`integer`|The shader stage.| :white_check_mark: Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Exactly one of `uri` or `bufferView` must be provided. Additional properties are not allowed.

* **JSON schema**: [shader.schema.json](schema/shader.schema.json)
* **Example**: [shaders.json](schema/examples/shaders.json)

### shader.uri

The uri of the GLSL source.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.

* **Type**: `string`
* **Required**: No
* **Format**: uri

### shader.bufferView

The index of the bufferView that contains the shader source. Use this instead of the shader's uri property.

* **Type**: `integer`
* **Required**: No

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
|**attributes**|`object`|A dictionary object of [`technique.attributes`](#reference-technique.attributes) objects.|No, default: `{}`|
|**program**|`string`|The ID of the program.| :white_check_mark: Yes|
|**uniforms**|`object`|A dictionary object of  [`technique.uniforms`](#reference-technique.uniforms) objects.|No, default: `{}`|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [technique.schema.json](schema/technique.schema.json)
* **Example**: [techniques.json](schema/examples/techniques.json)

### technique.attributes

A dictionary object of [`technique.attributes`](#reference-technique.attributes) objects.  Each object defines an attribute input.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `string`

### technique.program :white_check_mark:

The ID of the program.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**: ` >= 1`

### technique.uniforms

A dictionary object of [`technique.uniforms`](#reference-technique.uniforms) objects.  Each object defines a uniform input, and an optional semantic and value.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `string`

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
<a name="reference-technique.attributes"></a>
## technique.attributes

An object defining an attribute input to a [`technique`](#reference-technique).

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**type**|`integer`|The datatype.| :white_check_mark: Yes|
|**semantic**|`object`|Identifies an attribute with a well-known meaning.|Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [technique.attribute.schema.json](schema/technique.attribute.schema.json)

### attribute.type :white_check_mark:

The datatype. All valid values correspond to WebGL enums. Allowed values are `5120` (BYTE), `5121` (UNSIGNED_BYTE), `5122` (SHORT), `5123` (UNSIGNED_SHORT), `5124` (INT), `5125` (UNSIGNED_INT), `5126` (FLOAT), `35664` (FLOAT_VEC2), `35665` (FLOAT_VEC3), `35666` (FLOAT_VEC4), `35667` (INT_VEC2), `35668` (INT_VEC3), `35669` (INT_VEC4), `35670` (BOOL), `35671` (BOOL_VEC2), `35672` (BOOL_VEC3), `35673` (BOOL_VEC4), `35674` (FLOAT_MAT2), `35675` (FLOAT_MAT3), `35676` (FLOAT_MAT4), and `35678` (SAMPLER_2D).

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**: `5120`, `5121`, `5122`, `5123`, `5124`, `5125`, `5126`, `35664`, `35665`, `35666`, `35667`, `35668`, `35669`, `35670`, `35671`, `35672`, `35673`, `35674`, `35675`, `35676`, `35678`

### attribute.semantic

Identifies a attribute with a well-known meaning.  Attribute semantics include `"POSITION"`, `"NORMAL"`, `"TEXCOORD"`, `"COLOR"`, `"JOINT"`, and `"WEIGHT"`.  `"TEXCOORD"` and `"COLOR"` attribute semantic property names must be of the form `[semantic]_[set_index]`, e.g., `"TEXCOORD_0"`, `"TEXCOORD_1"`, `"COLOR_1"` etc.  For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_TEMPERATURE`.

* **Type**: `string`
* **Required**: Yes

### attribute.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### attribute.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-technique.uniforms"></a>
## technique.uniforms

An object defining a uniform input to a [`technique`](#reference-technique), and an optional semantic and value.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**count**|`integer`|When defined, the uniform is an array of count elements of the specified type.  Otherwise, the uniform is not an array.|No|
|**node**|`string`|The id of the [`node`](#reference-node) whose transform is used as the uniform's value.|No|
|**type**|`integer`|The datatype.| :white_check_mark: Yes|
|**semantic**|`string`|Identifies a uniform with a well-known meaning.|No|
|**value**|`number[]`, `boolean[]`, or `string[]`|The value of the uniform.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [technique.uniforms.schema.json](schema/technique.uniforms.schema.json)

### uniform.count

When defined, the uniform is an array of `count` elements of the specified type.  Otherwise, the uniform is not an array.  When defined, `value`'s length equals to `count`, times the number of components in the `type`, e.g., `3` for `FLOAT_VEC3`.

An array uniform of scalar values is not the same as a vector uniform of the same size; for example, when `count` is `2` and `type` is `5126` (FLOAT), the uniform is an array of two floating-point values, not a `FLOAT_VEC2`.

When a uniform is a glTF-defined semantic is used, the semantic must be `JOINTMATRIX`; application-specific uniforms can be arrays and, therefore, define `count`.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 1`

### uniform.node

The id of the [`node`](#reference-node) whose transform is used as the uniform's value.  When this is defined, `type` must be `35676` (FLOAT_MAT4), therefore, when the semantic is `"MODELINVERSETRANSPOSE"`, `"MODELVIEWINVERSETRANSPOSE"`, or `"VIEWPORT"`, the `node` property can't be defined.

* **Type**: `string`
* **Required**: No
* **Minimum Length**: ` >= 1`

### uniform.type :white_check_mark:

The datatype. All valid values correspond to WebGL enums. Allowed values are `5120` (BYTE), `5121` (UNSIGNED_BYTE), `5122` (SHORT), `5123` (UNSIGNED_SHORT), `5124` (INT), `5125` (UNSIGNED_INT), `5126` (FLOAT), `35664` (FLOAT_VEC2), `35665` (FLOAT_VEC3), `35666` (FLOAT_VEC4), `35667` (INT_VEC2), `35668` (INT_VEC3), `35669` (INT_VEC4), `35670` (BOOL), `35671` (BOOL_VEC2), `35672` (BOOL_VEC3), `35673` (BOOL_VEC4), `35674` (FLOAT_MAT2), `35675` (FLOAT_MAT3), `35676` (FLOAT_MAT4), and `35678` (SAMPLER_2D).

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**: `5120`, `5121`, `5122`, `5123`, `5124`, `5125`, `5126`, `35664`, `35665`, `35666`, `35667`, `35668`, `35669`, `35670`, `35671`, `35672`, `35673`, `35674`, `35675`, `35676`, `35678`

### uniform.semantic

Identifies a uniform with a well-known meaning.  Uniform semantics include `"LOCAL"` (FLOAT_MAT4), `"MODEL"` (FLOAT_MAT4), `"VIEW"` (FLOAT_MAT4), `"PROJECTION"` (FLOAT_MAT4), `"MODELVIEW"` (FLOAT_MAT4), `"MODELVIEWPROJECTION"` (FLOAT_MAT4), `"MODELINVERSE"` (FLOAT_MAT4), `"VIEWINVERSE"` (FLOAT_MAT4), `"PROJECTIONINVERSE"` (FLOAT_MAT4), `"MODELVIEWINVERSE"` (FLOAT_MAT4), `"MODELVIEWPROJECTIONINVERSE"` (FLOAT_MAT4), `"MODELINVERSETRANSPOSE"` (FLOAT_MAT3), `"MODELVIEWINVERSETRANSPOSE"` (FLOAT_MAT3), `"VIEWPORT"` (FLOAT_VEC4), `"JOINTMATRIX"` (FLOAT_MAT4[]).  For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_TEMPERATURE`.

* **Type**: `string`
* **Required**: No

### uniform.value

The value of the uniform. The length is determined by the values of the `type` and `count` (if present) properties.  A [`material`](#reference-material) value with the same name, when specified, overrides this value.

* **Type of each property**: `number[]`, `boolean[]`, or `string[]`
* **Required**: No

### uniform.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### uniform.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


### JSON Schema

* [`program`](#reference-program)
* [`shader`](#reference-shader)
* [`technique`](#reference-technique)
   * [`attributes`](#reference-technique.attributes)
   * [`uniforms`](#reference-technique.uniforms)

## Known Implementations

* TODO: List of known implementations, with links to each if available.

## Resources

* TODO: Resources, if any.
