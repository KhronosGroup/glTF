<!--
Copyright 2013-2017 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR_techniques_webgl

## Contributors

* Gabby Getz, Cesium, [@gabbygetz](https://twitter.com/gabbygetz)
* Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)
* Remi Arnaud, Starbreeze Studios, [@remi_arnaud](https://twitter.com/remi_arnaud)
* Patrick Cozzi, Cesium, [@pjcozzi](https://twitter.com/pjcozzi)
* Tony Parisi, Unity, [@auradeluxe](https://twitter.com/auradeluxe)
* Fabrice Robinet, Individual Contributor, [@fabricerobinet](https://twitter.com/fabricerobinet)

Copyright 2013-2017 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc. See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Archived Draft, Never Ratified

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF to define instances of shading techniques with external shader programs along with their parameterized values.  Shading techniques use JSON properties to describe data types and semantics for GLSL vertex and fragment shader programs.

This extension specification is targeting WebGL 1.0, and can be supported in any WebGL 1.0-based engine if the device supports the necessary WebGL extensions.

The [conformance](#conformance) section specifies how the extension interacts with the attributes defined in the base specification, as well as how the extension interacts with other extensions.

* **Abridged example glTF**: [sample_techniques.gltf](examples/sample_techniques.gltf)

## glTF Schema Updates

`KHR_techniques_webgl` introduces an array of shading techniques used in the asset in `techniques`. A technique and any uniform values are specified in any glTF material by adding a `KHR_techniques_webgl` extension.

### Extending Materials

A shading technique and any corresponding uniform values are specified by adding the `KHR_techniques_webgl` extension to any glTF material. For example, the following defines a material which selects the technique at index `0` from the `techniques` array and specifies a blue color for the `u_light0Color` uniform.

```json
"materials": [
    {
        "extensions": {
            "KHR_techniques_webgl": {
                "technique": 0,
                "values" : {
                    "u_light0Color": [
                        0.8,
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

The `KHR_techniques_webgl.technique` property references the index of a technique listed in the asset's `KHR_techniques_webgl.techniques` property.

#### Values

The `KHR_techniques_webgl.values` dictionary property defines the uniform values of a [`technique.uniform`](#uniforms) of the same key, and when specified, overrides the corresponding uniform `value`. The value supplied must conform to the `type` and `count` properties, if present, of the corresponding `Uniform` object, and must be present if no `value` is supplied in the referenced `Uniform` object.

## Extension

`KHR_techniques_webgl` is defined in the asset's top level `extensions` property with the following additional values.

```json
{
    "extensions": {
        "KHR_techniques_webgl": {
            "programs": [
                {
                    "fragmentShader": 0,
                    "vertexShader": 1
                }
            ],
            "shaders": [
                {
                    "type": 35632,
                    "uri": "duck0FS.glsl"
                },
                {
                    "type": 35633,
                    "uri": "duck0VS.glsl"
                }
            ],
            "techniques": [
                {
                    "program": 0,
                    "attributes": {
                        "a_position": {
                            "semantic": "POSITION"
                        }
                    },
                    "uniforms": {
                        "u_modelViewMatrix": {
                            "type": 35676,
                            "semantic": "MODELVIEW"
                        }
                    }
                }
            ]
        }
    }
}
```

#### Programs

GLSL shader programs are stored in the asset's `KHR_techniques_webgl.programs` property. This property contains one or more objects, one for each program.

Each shader program includes the properties `fragmentShader` and `vertexShader`, which reference the index of the shader in the `KHR_techniques_webgl.shaders` property for the fragment and vertex shader GLSL source code, respectively.

```json
{
    "programs": [
        {
            "fragmentShader": 0,
            "vertexShader": 1
        }
    ]
}
```

#### Shaders

One or more shader source files are listed in the asset's `KHR_techniques_webgl.shaders` property. Each shader specifies a `type` (vertex or fragment, defined as GL enum types) and either a `uri` to the file or a reference to a `bufferView`. Shader URIs may be URIs to external files or data URIs, allowing the shader content to be embedded as base64-encoded data in the asset.

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

##### Shader Requirements

Supplied shaders must respect values of material [`doubleSided`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#double-sided), [`alphaMode`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#alpha-coverage), and [`alphaCutoff`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#alpha-coverage) properties.

> **Implementation Note**: TODO: Reference shader implementation notes on `doubleSided` once included in the main spec, including how to account for normal and tangent-space calculations.

The value of a material's `alphaCutoff` property should be passed to the technique shaders using a uniform with the semantic `ALPHACUTOFF`. A uniform with this semantic will ignore a supplied default value and the corresponding material uniform value.

##### File Extension and MIME Type

External shader files may have any extension. The preferred MIME type is `text/plain`.

### Techniques

One or more techniques are listed in the `KHR_techniques_webgl.techniques` property. A technique describes the shading used for an asset's material. Each technique must include the `program` property and define any attributes or uniforms to be passed as inputs to the program by their key in the `attributes` and `uniforms` dictionary properties.

The following example shows a technique and the properties it defines. This section describes each property in detail.

```json
{
    "techniques": [
        {
            "program": 0,
            "attributes": {
                "a_normal":  {
                    "semantic": "NORMAL"
                },
                "a_position": {
                    "semantic": "POSITION"
                },
                "a_texcoord0": {
                    "semantic": "TEXCOORD_0"
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
                    "type": 35678,
                    "value": {
                        "index": 0
                    }
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
                    "semantic": "MODEL",
                    "node": 1,
                    "type": 35676
                },
                "u_modelViewMatrix": {
                    "semantic": "MODELVIEW",
                    "type": 35676
                },
                "u_normalMatrix": {
                    "semantic": "MODELVIEWINVERSETRANSPOSE",
                    "type": 35675
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

The `program` property of a technique creates an instance of a shader program. The value of the property is the index of a program defined in the asset's `KHR_techniques_webgl.programs` property. A shader program may be instanced multiple times within the glTF asset.

Attributes and uniforms passed to the program instance's shader code are defined in the `attributes` and `uniforms` properties of the technique, respectively.

#### Attributes

The `attributes` dictionary property specifies the vertex attributes of the data that will be passed to the shader. Each attribute's key is a string that corresponds to the attribute name in the GLSL source code. Each attribute's value is an [attribute](#reference-attribute) object, where the semantic of the attribute is defined. The semantic corresponds with the mesh attribute semantic specified in the [mesh primitive](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-mesh-primitive), the value of which is the index of the accessor containing attribute's data. It's invalid to specify a semantic that does not exist in the mesh data.

#### Uniforms

The `uniforms` dictionary property specifies the uniform variables that will be passed to the shader. Each uniform's key is a string that corresponds to the uniform name in the GLSL source code. Each uniform's value is a string that references a [uniform](#reference-uniform) object, where the type (GL types such as a floating point number, vector, etc.) and potentially the semantic and default value of the uniform is defined.

When a material instances a technique, the name of each supplied value in its `KHR_techniques_webgl.values` property must correspond to one of the uniforms defined in the technique.

Uniforms which specify the `SAMPLER_2D` type use a [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) object as a value to reference a texture, (see [conformance](#conformance) for objects with extended with `KHR_texture_transform`).

The above example illustrates several uniforms. The property `u_ambient` is defined as a `FLOAT_VEC4` type; and `u_light0Color` is defined as a `FLOAT_VEC3` with a default color value of white.

##### Semantics

Technique uniforms may also optionally define a *semantic*, an enumerated value describing how the runtime is to interpret the data to be passed to the shader.

In the above example, the uniform `u_light0Transform` defines the `MODEL` semantic, which corresponds to the world space position of the node referenced in the property `node`, in this case the node `1`.

If no `node` property is supplied for a uniform, the semantic is implied in a context-specific manner: either to the node which is being rendered, or in the case of camera-specific semantics, to the current camera. In the following fragment, which defines a uniform named `u_projectionMatrix` that is derived from the implementation's projection matrix, the semantic would be applied to the camera.

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

This extension defines the `ALPHACUTOFF` uniform semantic, which corresponds to the value of a material's `alphaCutoff` property, which must be respected by the supplied technique, (see [Shader Requirements](#shader-requirements)). A uniform with this semantic will ignore a supplied default value and the corresponding material uniform value.

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
| `MODELINVERSETRANSPOSE`      | `FLOAT_MAT3` | The inverse-transpose of `MODEL` without the translation.  This transforms normals in model coordinates to world coordinates. |
| `MODELVIEWINVERSETRANSPOSE`  | `FLOAT_MAT3` | The inverse-transpose of `MODELVIEW` without the translation.  This transforms normals in model coordinates to eye coordinates. |
| `VIEWPORT`                   | `FLOAT_VEC4` | The viewport's x, y, width, and height properties stored in the `x`, `y`, `z`, and `w` components, respectively.  For example, this is used to scale window coordinates to [0, 1]: `vec2 v = gl_FragCoord.xy / viewport.zw;` |
| `JOINTMATRIX`                | `FLOAT_MAT4[]` | Array parameter; its length (`uniform.count`) must be greater than or equal to the length of `jointNames` array of a skin being used. Each element transforms mesh coordinates for a particular joint for skinning and animation. |
| `ALPHACUTOFF`                | `FLOAT` | The value of the material's [`alphaCutoff`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#alpha-coverage) property. |

For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_SIMULATION_TIME`.

## Conformance

Implementations should continue to respect material [`doubleSided`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#double-sided), [`alphaMode`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#alpha-coverage), and [`alphaCutoff`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#alpha-coverage) properties by modifying the render state. The default state of the WebGL context should not be assumed.

[`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) objects referenced by the `KHR_techniques_webgl` extension may not use the `KHR_texture_transform` extension. The `offset`, `rotation`, and `scale` transforms applied to the texture by using `KHR_texture_transform` can be applied with this extension by providing uniforms for these values and performing the necessary transformations in the supplied GLSL shader code.

## Properties Reference

#### JSON Schemas

* [`KHR_techniques_webgl glTF extension`](#reference-khr_techniques_webgl-gltf-extension)
    * [`Program`](#reference-program)
    * [`Shader`](#reference-shader)
    * [`Technique`](#reference-technique)
        * [`Attribute`](#reference-attribute)
        * [`Uniform`](#reference-uniform)
* [`KHR_techniques_webgl material extension`](#reference-khr_techniques_webgl-material-extension)

---------------------------------------
<a name="reference-khr_techniques_webgl-gltf-extension"></a>
## KHR_techniques_webgl glTF extension

Instances of shading techniques with external shader programs along with their parameterized values.  Shading techniques describe data types and semantics for GLSL vertex and fragment shader programs.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**programs**|Program `[1-*]`|An array of [`Program`](#reference-program) objects.| :white_check_mark: Yes|
|**shaders**|Shader `[1-*]`|An array of [`Shader`](#reference-shader) objects.| :white_check_mark: Yes|
|**techniques**|Technique `[1-*]`|An array of [`Technique`](#reference-technique) objects.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [glTF.KHR_techniques_webgl.schema.json](schema/glTF.KHR_techniques_webgl.schema.json)

### programs :white_check_mark:

An array of [`Program`](#reference-program) objects.

* **Type**: Program `[1-*]`
* **Required**: Yes

### shaders :white_check_mark:

An array of [`Shader`](#reference-shader) objects.

* **Type**: Shader `[1-*]`
* **Required**: Yes

### techniques :white_check_mark:

An array of [`Technique`](#reference-technique) objects.  A technique is a template for a material appearance.

* **Type**: Technique `[1-*]`
* **Required**: Yes

### extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### extras

Application-specific data.

* **Type**: `any`
* **Required**: No




---------------------------------------
<a name="reference-program"></a>
## Program

A shader program, including its vertex and fragment shaders.

**Related WebGL functions**: `attachShader()`, `bindAttribLocation()`, `createProgram()`, `deleteProgram()`, `getProgramParameter()`, `getProgramInfoLog()`, `linkProgram()`, `useProgram()`, and `validateProgram()`

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**fragmentShader**|`integer`|The index of the fragment shader.| :white_check_mark: Yes|
|**vertexShader**|`integer`|The index of the vertex shader.| :white_check_mark: Yes|
|**glExtensions**|`string` `[1-*]`|The names of required WebGL 1.0 extensions.|No|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [program.schema.json](schema/program.schema.json)

### program.fragmentShader :white_check_mark:

The index of the fragment shader.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: ` >= 0`

### program.vertexShader :white_check_mark:

The index of the vertex shader.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: ` >= 0`

### program.glExtensions

The names of required WebGL 1.0 extensions.

* **Type**: `string` `[1-*]`
   * Each element in the array must have length between `1` and `256`.
* **Required**: No
* **Related WebGL functions**: `getExtension()`

### program.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### program.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### program.extras

Application-specific data.

* **Type**: `any`
* **Required**: No




---------------------------------------
<a name="reference-shader"></a>
## Shader

A vertex or fragment shader. Exactly one of `uri` or `bufferView` must be provided for the GLSL source.

**Related WebGL functions**: `createShader()`, `deleteShader()`, `shaderSource()`, `compileShader()`, `getShaderParameter()`, and `getShaderInfoLog()`

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**uri**|`string`|The uri of the GLSL source.|No|
|**type**|`integer`|The shader stage.| :white_check_mark: Yes|
|**bufferView**|`integer`|The index of the bufferView that contains the GLSL shader source. Use this instead of the shader's uri property.|No|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [shader.schema.json](schema/shader.schema.json)

### shader.uri

The uri of the GLSL source.  Relative paths are relative to the `.gltf` file.  Instead of referencing an external file, the uri can also be a data-uri.

* **Type**: `string`
* **Required**: No
* **Format**: uriref

### shader.type :white_check_mark:

The shader stage.  All valid values correspond to WebGL enums.

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**:
   * `35632` FRAGMENT_SHADER
   * `35633` VERTEX_SHADER

### shader.bufferView

The index of the bufferView that contains the GLSL shader source. Use this instead of the shader's uri property.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### shader.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### shader.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### shader.extras

Application-specific data.

* **Type**: `any`
* **Required**: No




---------------------------------------
<a name="reference-technique"></a>
## Technique

A template for material appearances.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**program**|`integer`|The index of the program.| :white_check_mark: Yes|
|**attributes**|`object`|A dictionary object of [`Attribute`](#reference-attribute) objects.|No, default: `{}`|
|**uniforms**|`object`|A dictionary object of [`Uniform`](#reference-uniform) objects.|No, default: `{}`|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [technique.schema.json](schema/technique.schema.json)

### technique.program :white_check_mark:

The index of the program.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: ` >= 0`

### technique.attributes

A dictionary object of [`Attribute`](#reference-attribute) objects.  Each object defines an attribute input with a type and a semantic.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`

### technique.uniforms

A dictionary object of [`Uniform`](#reference-uniform) objects.  Each object defines a uniform input with a type and an optional semantic and value.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`

### technique.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### technique.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### technique.extras

Application-specific data.

* **Type**: `any`
* **Required**: No



---------------------------------------
<a name="reference-attribute"></a>
## Attribute

An attribute input to a technique and the corresponding semantic.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**semantic**|`string`|Identifies a mesh attribute semantic.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [technique.attribute.schema.json](schema/technique.attribute.schema.json)

### attribute.semantic :white_check_mark:

Identifies a mesh attribute semantic.  Attribute semantics include `"POSITION"`, `"NORMAL"`, `"TEXCOORD"`, `"COLOR"`, `"JOINT"`, and `"WEIGHT"`.  `"TEXCOORD"` and `"COLOR"` attribute semantic property names must be of the form `[semantic]_[set_index]`, e.g., `"TEXCOORD_0"`, `"TEXCOORD_1"`, `"COLOR_1"`, etc.  For forward-compatibility, application-specific semantics must start with an underscore, e.g., `"_SIMULATION_TIME"`.

* **Type**: `string`
* **Required**: Yes

### attribute.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### attribute.extras

Application-specific data.

* **Type**: `any`
* **Required**: No





---------------------------------------
<a name="reference-uniform"></a>
## Uniform

A uniform input to a technique, and an optional semantic and value.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**count**|`integer`|When defined, the uniform is an array of count elements of the specified type.  Otherwise, the uniform is not an array.|No|
|**node**|`integer`|The index of the node whose transform is used as the uniform's value.|No|
|**type**|`integer`|The uniform type.| :white_check_mark: Yes|
|**semantic**|`string`|Identifies a uniform with a well-known meaning.|No|
|**value**|`any`|The value of the uniform.|No|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [technique.uniform.schema.json](schema/technique.uniform.schema.json)

### uniform.count

When defined, the uniform is an array of `count` elements of the specified type.  Otherwise, the uniform is not an array.  When defined, `value`'s length must equal `count`, times the number of components in the `type`, e.g., `3` for `FLOAT_VEC3`.  A uniform array of scalar values is not the same as a uniform vector of the same size; for example, when count is 2 and type is `5126` (`FLOAT`), the uniform is an array of two floating-point values, not a `FLOAT_VEC2`. When a glTF-defined semantic is used, the semantic must be `JOINTMATRIX`; application-specific uniforms can be arrays and, therefore, define `count`.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 1`

### uniform.node

The index of the node whose transform is used as the uniform's value.  When this is defined, `type` must be `35676` (`FLOAT_MAT4`), therefore, when the semantic is `"MODELINVERSETRANSPOSE"`, `"MODELVIEWINVERSETRANSPOSE"`, or `"VIEWPORT"`, the node property can't be defined.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### uniform.type :white_check_mark:

The uniform type.  All valid values correspond to WebGL enums.

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**:
   * `5124` INT
   * `5126` FLOAT
   * `35664` FLOAT_VEC2
   * `35665` FLOAT_VEC3
   * `35666` FLOAT_VEC4
   * `35667` INT_VEC2
   * `35668` INT_VEC3
   * `35669` INT_VEC4
   * `35670` BOOL
   * `35671` BOOL_VEC2
   * `35672` BOOL_VEC3
   * `35673` BOOL_VEC4
   * `35674` FLOAT_MAT2
   * `35675` FLOAT_MAT3
   * `35676` FLOAT_MAT4
   * `35678` SAMPLER_2D

### uniform.semantic

Identifies a uniform with a well-known meaning.  Uniform semantics include `"LOCAL"` (`FLOAT_MAT4`), `"MODEL"` (FLOAT_MAT4), `"VIEW"` (FLOAT_MAT4), `"PROJECTION"` (FLOAT_MAT4), `"MODELVIEW"` (FLOAT_MAT4), `"MODELVIEWPROJECTION"` (FLOAT_MAT4), `"MODELINVERSE"` (FLOAT_MAT4), `"VIEWINVERSE"` (FLOAT_MAT4), `"PROJECTIONINVERSE"` (FLOAT_MAT4), `"MODELVIEWINVERSE"` (FLOAT_MAT4), `"MODELVIEWPROJECTIONINVERSE"` (FLOAT_MAT4), `"MODELINVERSETRANSPOSE"` (FLOAT_MAT3), `"MODELVIEWINVERSETRANSPOSE"` (FLOAT_MAT3), `"VIEWPORT"` (FLOAT_VEC4), `"JOINTMATRIX"` (FLOAT_MAT4[]).  For forward-compatibility, application-specific semantics must start with an underscore, e.g., `"_SIMULATION_TIME"`.

* **Type**: `string`
* **Required**: No

### uniform.value

The value of the uniform. The length is determined by the values of the `type` and `count` (if present) properties.  A material uniform value with the same name, when specified, overrides this value.

* **Type**: `any`
* **Required**: No

### uniform.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### uniform.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### uniform.extras

Application-specific data.

* **Type**: `any`
* **Required**: No

---------------------------------------
<a name="reference-khr_techniques_webgl-material-extension"></a>
## KHR_techniques_webgl material extension

The technique to use for a material and any additional uniform values.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**technique**|`integer`|The index of the technique.| :white_check_mark: Yes|
|**values**|`object`|Dictionary object of uniform values.|No, default: `{}`|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [material.KHR_techniques_webgl.schema.json](schema/material.KHR_techniques_webgl.schema.json)

### technique :white_check_mark:

The index of the technique.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: ` >= 0`

### values

Dictionary object of uniform values. Uniforms with the same name as the technique's uniform must conform to the referenced `type` and `count` (if present) properties, and override the technique's uniform value.  A uniform value must be supplied here if not present in the technique uniforms.

* **Type**: `object`
* **Required**: No, default: `{}`

### extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### extras

Application-specific data.

* **Type**: `any`
* **Required**: No


---------------------------------------


## References
_This section is non-normative._

* [WebGL 1.0 Specification](https://www.khronos.org/registry/webgl/specs/latest/1.0/), Khronos Group

## Appendix: Full Khronos Copyright Statement

Copyright 2013-2017 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements necessary for compliance and so are outside the Scope of this Specification. These parts of the Specification are marked as being non-normative, or identified as Implementation Notes.

Where this Specification includes normative references to external documents, only the specifically identified sections and functionality of those external documents are in Scope. Requirements defined by external documents not created by Khronos may contain contributions from non-members of Khronos not covered by the Khronos Intellectual Property Rights Policy.

This specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

This specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional copyright license to use and reproduce the unmodified specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms. Parties desiring to implement the specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos IP Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this specification; see https://www.khronos.org/adopters.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

Vulkan is a registered trademark and Khronos, OpenXR, SPIR, SPIR-V, SYCL, WebGL, WebCL, OpenVX, OpenVG, EGL, COLLADA, glTF, NNEF, OpenKODE, OpenKCAM, StreamInput, OpenWF, OpenSL ES, OpenMAX, OpenMAX AL, OpenMAX IL, OpenMAX DL, OpenML and DevU are trademarks of The Khronos Group Inc. ASTC is a trademark of ARM Holdings PLC, OpenCL is a trademark of Apple Inc. and OpenGL and OpenML are registered trademarks and the OpenGL ES and OpenGL SC logos are trademarks of Silicon Graphics International used under license by Khronos. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
