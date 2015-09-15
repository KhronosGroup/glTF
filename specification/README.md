<p align="center">
<img src="figures/glTF_300.jpg" />
</p>

*Draft, Version 1.0*

The GL Transmission Format (glTF) is a runtime asset delivery format for GL APIs: WebGL, OpenGL ES, and OpenGL.  glTF bridges the gap between 3D content creation tools and modern GL applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.


> 
_This is a draft specification; it is incomplete and may change before ratification.  We have made it available early in the spirit of transparency to receive community feedback.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback._

Editors

* Patrick Cozzi, Cesium
* Tony Parisi, Third Eye

Contributors

* Fabrice Robinet, Individual Contributor (Previous Editor and Incubator)
* Remi Arnaud, Starbreeze Studios
* Uli Klumpp, Individual Contributor


# Contents

* [Introduction](#introduction)
  * [Motivation](#motivation)
  * [glTF Basics](#glTFbasics)
  * [Design Goals](#designgoals)
  * [File Extensions and MIME Types](#mimetypes)
* [Concepts](#concepts)
  * [File Structure](#file-structure)
  * [The Scene Graph](#scene-graph)
  * [Accessing Binary Data](#binary-data)
  * [Geometry and Meshes](#geometry-and-meshes)
  * [Materials and Shading](#materials-and-shading)
  * [Cameras](#cameras)
  * [Lights](#lights)
  * [Animations](#animations)
  * [Metadata](#metadata)
  * [Specifying Extensions](#specifying-extensions)
* [Properties Reference](#properties)
* [Binary Types Reference](#binarytypes)
* [JSON Schema](#schema)
* [Conformance](#conformance)
* [Acknowledgements](#acknowledgements)
* [References](#references)

<a name="introduction"></a>
# Introduction

*This section is non-normative.*

The GL Transmission Format (glTF) is a runtime asset delivery format for GL APIs: WebGL, OpenGL ES, and OpenGL.  glTF bridges the gap between 3D content creation tools and modern GL applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.

<a name="motivation"></a>
## Motivation

Traditional 3D modeling formats have been designed to store data for offline use, primarily to support authoring workflows on desktop systems. Industry-standard 3D interchange formats allow for sharing assets between different modeling tools, and within the content pipeline in general. However, neither of these types of formats are optimized for download speed or fast loading at runtime. Files tend to grow very large, and applications need to do a significant amount of processing to load such assets into GL-based applications.

Applications seeking high performance rarely load modeling formats directly; instead, they process models offline as part of a custom content pipeline, converting the assets to a proprietary format optimized for their runtime application.  This has led to a fragmented market of incompatible proprietary runtime formats, and duplicated efforts in the content creation pipeline. 3D assets exported for one application can not be reused in another application without going back to the original modeling tool-specific source and performing another proprietary export step.

With the advent of mobile- and web-based 3D computing, new classes of applications have emerged that require fast, dynamic loading of standardized 3D assets. Digital marketing solutions, e-commerce product visualizations and online model-sharing sites are just a few of the connected 3D applications being built today using WebGL or OpenGL ES. Beyond the need for efficient delivery, many of these online applications can benefit from a standard, interoperable format to enable sharing and reuse of assets between users, between applications, and within heterogeneous, distributed content pipelines.

glTF solves these problems by providing a vendor- and runtime-neutral format that can be loaded and rendered with minimal processing. The format combines an easily parseable JSON scene description with one or more binary files representing geometry, animations and other rich data. Binary data is stored in such a way that it can be loaded directly into GL buffers without additional parsing or other manipulation. Using this approach, glTF is able to faithfully preserve full hierarchial scenes with nodes, meshes, cameras, lights, materials and animations, while enabling efficient delivery and fast loading.

<a name="glTFbasics"></a>

## glTF Basics

glTF assets are JSON files plus supporting external data. Specifically, a glTF asset is represented by:

* A JSON-formatted file (`.gltf`) containing a full scene description: node hierarchy, materials, lights, cameras, as well as descriptor information for meshes, shaders, animations and other constructs
* Binary files (`.bin`) containing geometry and animation data, and other buffer-based data
* Image files (`.jpg`, `.png`, etc.) for textures
* GLSL text files (`.glsl`) for GLSL shader source code

Assets defined in other formats, such as images and GLSL shader source code, may be stored in external files referenced via URI or embedded directly into the JSON using  [data URIs](https://developer.mozilla.org/en/data_URIs).

<mark>*Todo: diagram here*</mark>

<mark>*Todo: sample fragment here*</mark>

<a name="designgoals"></a>

## Design Goals

glTF has been designed to meet the following goals:

* *Compact file sizes.* While web developers like to work with clear text as much as possible, clear text encoding is simply not practical for transmitting 3D data due to sheer size. The glTF JSON file itself is clear text, but it is compact and rapid to parse. All large data such as geometry and animations are stored in binary files that are much smaller than equivalent text representations.
* *Fast loading.* glTF data structures have been designed to mirror the GL API data as closely as possible, both in the JSON and binary files, to reduce load times. For example, binary data for meshes can be loaded directly into WebGL typed arrays with a simple data copy; no parsing or further processing is required.
* *Runtime-independence.* glTF makes no assumptions about the target application or 3D engine. glTF specifies no runtime behaviors other than rendering and animation.
* *Complete 3D scene representation.* Exporting single objects from a modeling package is not sufficient for many applications. Often, authors want to load entire scenes, including nodes, transformations, transform hierarchy, meshes, materials, lights, cameras, animations, into their applications. glTF strives to preserve all of this information for use in the downstream application.
* *Extensibility.* While the initial base specification supports a rich feature set, there will be many opportunities for growth and improvement. glTF defines a mechanism that allows the addition of both general-purpose and vendor-specific extensions.

The design of glTF takes a pragmatic approach. The format is meant to mirror the GL APIs as closely as possible, but if it did only that, there would be no lights, cameras, animations and other features typically found in both modeling tools and runtime systems, and much semantic information would be lost in the translation. By supporting these common constructs, glTF content can not only load and render, but it can be immediately usable in a wider range of applications and require less duplication of effort in the content pipeline.

The following are intentionally **not** goals for the initial design of glTF:

* *glTF is not a streaming format.* The binary data in glTF is inherently streamable, and the buffer design allows for fetching data incrementally. But there are no other streaming constructs in the format, and no conformance requirements for an implementation to stream data versus downloading it in its entirety before rendering.
* *glTF is not intended to be human-readable,* though by virtue of being represented in JSON, it is developer-friendly.

Version 1.0 of glTF does not define compression for geometry and other rich data. However, the design team believes that compression is a very important part of a transmission standard, and there is already work underway to define compression extensions.

> The 3D Formats Working Group is developing partnerships to define the codec options for geometry compression.  glTF defines the node hierarchy, materials, animations, and geometry, and will reference the external compression specs.

<a name="mimetypes"></a>
## File Extensions and MIME Types

<mark>*Todo: someone needs to get a MIME type recommendation going with IETF...*</mark>

* `*.gltf` files use `model/gltf+json`
* `*.bin` files use `application/octet-stream`
* `*.glsl` files use `text/plain`
* Texture files use the official `image/*` type based on the specific image format.


<a name="concepts"></a>
# Concepts

*This section is non-normative.*

<a name="file-structure"></a>
## File Structure
<mark>*Todo:take a quick tour of the JSON file, how properties are laid out, etc.*</mark>

<a name="scene-graph"></a>
## The Scene Graph

### Nodes and Hierarchy

The glTF asset defines one or more *nodes*, that is, the objects comprising the scene to render. 

Each node can contain one or more meshes, a skin, a camera or a light, defined in the `meshes`, `instanceSkin`, `camera` and `light` properties, respectively.

Nodes have a optional `name` property.

Node also have transform properties, as described in the next section.

Nodes are organized in a parent-child hierarchy known informally as the *node hierarchy*. 

The node hierarchy is defined using a node's `children` property, as in the following example:

```json
    "node-box": {
        "children": [
            "node_1",
            "node-camera_1"
        ],
        "name": "Box",
    },
```

The node `node-box` has two children, `node_1` and `node-camera_1`. Each of those nodes could in turn have their own children, creating a hierarchy of nodes.

>For Version 1.0 conformance, the glTF node hierarchy is not a directed acyclic graph (DAG) or *scene graph*, but a strict tree. That is, no node may be a direct or indirect descendant of more than one node. This restriction is meant to simplify implementation and facilitate conformance. The restriction may be lifted after Version 1.0.


### Transforms

Any node can define a local space transformation either by supplying a `matrix` property, or any of `translation`, `rotation`, and `scale`  properties (also known as *TRS properties*). 

In the example below, `node-Box03` is defines non-default rotation and translation.

```json
    "node-box": {
        "children": [
            "node_1",
            "node-camera_1"
        ],
        "name": "Box",
        "rotation": [
            0.0787344,
            -0.00904895,
            0.996855,
            2.91345
        ],
        "scale": [
            1,
            1,
            1
        ],
        "translation": [
            -17.7082,
            -11.4156,
            2.0922
        ]
    },
```

The next example defines the transformation for a camera node using the `matrix` property rather than using the individual TRS values:

```json
    "node-camera_1": {
        "camera": "camera_1",
        "children": [],
        "matrix": [
            -0.99975,
            -0.00679829,
            0.0213218,
            0,
            0.00167596,
            0.927325,
            0.374254,
            0,
            -0.0223165,
            0.374196,
            -0.927081,
            0,
            -0.0115543,
            0.194711,
            -0.478297,
            1
        ],
        "name": "Camera01"
    },
```

<a name="accessing-binary-data"></a>
## Accessing Binary Data


### Buffers and Buffer Views

A *buffer* is data stored as binary blobs in a file. The buffer can contain a combination of geometry, animation, and skins. Buffer data is little endian.

Binary blobs allow efficient creation of GL buffers and
textures since they require no additional parsing, except perhaps decompression. An asset can have any number of buffer files for flexibility for a wide array of applications.

A *bufferView* represents a subset of data in a buffer, defined by an integer offset into the buffer, and an integer data length. The bufferView also defines a target data type (ARRAY_BUFFER,
ELEMENT_BUFFER, animation/skin) so that the implementation can readily create and populate buffers in memory.

buffers and bufferViews do not contain type information. They simply define the raw data for retrieval from the file. Objects within the glTF file (meshes, skins, animations) never access buffers or bufferViews directly, but rather via *accessors*.

### Accessors

All large data for meshes, skins and animations is stored in buffers and retrieved via accessors.

An *accessor* defines a method for retrieving data as typed arrays from within a bufferView. The accessor specifies a component type (e.g. FLOAT) and a data type (e.g. VEC3), which when combined define the complete data type for each array. The accessor also specifies the location and size of the data within the bufferView using the properties `byteOffset` and `count`. count specifies the number of attributes within the bufferView, *not* the number of bytes.

<a name="geometry-and-meshes"></a>

## Geometry and Meshes

Any node can contain one or more meshes, defined in its `meshes` property. Any node can contain one skin, defined in its `instanceSkin` property.

### Meshes

In glTF, meshes are defined as arrays of *primitives*. Primitives correspond to the data required for GL drawElements calls. Primitives specify one or more `attributes`, corresponding to the vertex attributes used in the draw calls. Indexed primitives also define an `indices` property. Attributes and indices are defined as accessors. Each primitive also specifies a material and a primitive type that coresponds to the GL primitive type (e.g. triangle set).

The following example defines a mesh containing one triangle set primitive:

```json
    "primitives": [
        {
            "attributes": {
                "NORMAL": "accessor_25",
                "POSITION": "accessor_23",
                "TEXCOORD_0": "accessor_27"
            },
            "indices": "accessor_21",
            "material": "blinn3-fx",
            "primitive": 4
        }
    ]
```
                    
                    
### Skins

*TBD*


<a name="materials-and-shading"></a>
## Materials and Shading

A material is defined as an instance of a shading technique along with parameterized values, e.g. light colors, specularity, or shininess. Shading techniques use JSON properties to describe data types and semantics for GLSL vertex and fragment shader programs.

Materials are stored in the assets `materials` property, a dictionary containing one or more material definitions. The following example shows a Blinn shader with ambient color, diffuse color, emission, shininess and specular color.

```
"materials": {
    "blinn-1": {
        "instanceTechnique": {
            "technique": "technique1",
            "values": {
                "ambient": [
                    0,
                    0,
                    0,
                    1
                ],
                "diffuse": "texture_file2",
                "emission": [
                    0,
                    0,
                    0,
                    1
                ],
                "shininess": 38.4,
                "specular": [
                    0,
                    0,
                    0,
                    1
                ]
            }
        },
        "name": "blinn3"
    }
},
```


### Techniques

<mark>*Todo: Patrick I'll need help describing techniques and passes and how they relate to each other. Any concept document I can crib for this?*</mark>

### Passes

<mark>*Todo: Patrick please write up.*</mark>

### Programs, Attributes and Uniforms

#### Programs

GLSL shader programs are stored in the asset's `programs` property. This property is a dictionary containing one or more named objects, one for each program.

Each shader program includes an `attributes` property, which specifies the vertex attributes that will be passed to the shader, and the properties `fragmentShader` and `vertexShader`, which reference the files for the fragment and vertex shader GLSL source code, respectively. 

```json
    "programs": {
        "program_0": {
            "attributes": [
                "a_normal",
                "a_position",
                "a_texcoord0"
            ],
            "fragmentShader": "duck0FS",
            "vertexShader": "duck0VS"
        }
    },
```

Shader source files are stored in the asset's `shaders` property, a dictionary containing one or more named shader source files. Each shader specifies a `type` (vertex or fragment, defined as GL enum types) and a `uri` to the file. Shader URIs may be URIs to external files or data URIs, allowing the shader content to be embedded as base64-encoded data in the asset.

```json
    "shaders": {
        "duck0FS": {
            "type": 35632,
            "uri": "duck0FS.glsl"
        },
        "duck0VS": {
            "type": 35633,
            "uri": "duck0VS.glsl"
        }
    },
```    

#### Program Instances

A shader program may be instanced multiple times within the glTF asset, via the `instanceProgram` property of the render pass. `instanceProgram` specifies the program identifier, and a `attributes` and `uniforms` properties.

```json
    "instanceProgram": {
        "attributes": {
            "a_normal": "normal",
            "a_position": "position",
            "a_texcoord0": "texcoord0"
        },
        "program": "program_0",
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
    },
```

#### Attributes

The `attributes` property specifies the vertex attributes of the data that will be passed to the shader. Each attribute's name is a string that corresponds to the attribute name in the GLSL source code. Each attribute's value is a string that references a parameters defined in the technique's `parameters` property, where the type and semantic of the attribute is defined.

#### Uniforms

The `uniforms` property specifies the uniform variables that will be passed to the shader. Each uniform's name is a string that corresponds to the uniform name in the GLSL source code. Each uniform's value is a string that references a parameter defined in the technique's `parameters` property, where the type and semantic of the uniform is defined.

<mark>*Todo:Patrick why did we decide to duplicate the attributes in the instanceProgram? Or alternately, why do we even specify the attributes in the program object at all?*</mark>


### Render States

<mark>*Todo: Patrick can you help a brother out? How are render states actually used? Do you use them in Cesium?*</mark>

### Common Techniques

In addition to supporting arbitrary GLSL shader programs, glTF allows the ability to specify common shading techniques such as Blinn, Lambert and Phong without the need to supply the GLSL shader code. This is included as a hint for runtimes that have built-in support for the common techniques.


```json
    "details": {
        "commonProfile": {
            "extras": {
                "doubleSided": false
            },
            "lightingModel": "Blinn",
            "parameters": [
                "ambient",
                "diffuse",
                "emission",
                "light0Color",
                "light0Transform",
                "modelViewMatrix",
                "normalMatrix",
                "projectionMatrix",
                "shininess",
                "specular"
            ],
            "texcoordBindings": {
                "diffuse": "TEXCOORD_0"
            }
        },
        "type": "COLLADA-1.4.1/commonProfile"
```


<mark>*Todo: Patrick we really need to think about conformance here. Does a conforming implementation have to support both shaders and common techniques? Or just shaders, where the common techniques are a hint? Or just the common techniques... if the latter, then what does a conforming implementation do with shader code? Ignore it in favor of the common technique?*</mark>

### Textures

Textures can be used as uniform inputs to shaders. 

```
"materials": {
    "blinn-1": {
        "instanceTechnique": {
            "technique": "technique1",
            "values": {
                "ambient": [
                    0,
                    0,
                    0,
                    1
                ],
                "diffuse": "texture_file2",
                "emission": [
                    0,
                    0,
                    0,
                    1
                ],
                "shininess": 38.4,
                "specular": [
                    0,
                    0,
                    0,
                    1
                ]
            }
        },
        "name": "blinn3"
    }
},
```

<a name="cameras"></a>
## Cameras

<a name="lights"></a>
## Lights

<a name="animations"></a>
## Animations

<a name="metadata"></a>
## Metadata

<a name="specifying-extensions"></a>
## Specifying Extensions

<mark>*Todo: Patrick please do preliminary writeup.*</mark>

<a name="properties"></a>
# Properties Reference

This section will have a detailed reference on each property. e.g. `asset`. This is more than what is currently in the schema. A one-line description and comment isn't enough, we need to fully specify behavior for each property.

<a name="binarytypes"></a>
# Binary Types Reference

This section will describe the format for each of the GL types stored in the binary file. A reference to the GL spec for each type might be enough

<mark>*Todo: Patrick please advise on how to tackle this*</mark>

<a name="schema"></a>
# JSON Schema

   * <a href="schema/accessor.schema.json">`accessor`</a>
   * <a href="schema/animation.schema.json">`animation`</a>
   * <a href="schema/animationChannel.schema.json">`animation/channel`</a>
   * <a href="schema/animationChannelTarget.schema.json">`animation/channel/target`</a>
   * <a href="schema/animationParameter.schema.json">`animation/parameter`</a>
   * <a href="schema/animationSampler.schema.json">`animation/sampler`</a>
   * <a href="schema/asset.schema.json">`asset`</a>
   * <a href="schema/buffer.schema.json">`buffer`</a>
   * <a href="schema/bufferView.schema.json">`bufferView`</a>
   * <a href="schema/camera.schema.json">`camera`</a>
   * <a href="schema/cameraOrthographic.schema.json">`camera/orthographic`</a>
   * <a href="schema/cameraPerspective.schema.json">`camera/perspective`</a>
   * <a href="schema/extension.schema.json">`extension`</a>
   * <a href="schema/extras.schema.json">`extras`</a>
   * <a href="schema/glTF.schema.json">`glTF`</a> (root property for model)
   * <a href="schema/image.schema.json">`image`</a>
   * <a href="schema/light.schema.json">`light`</a>
   * <a href="schema/lightAmbient.schema.json">`light/ambient`</a>
   * <a href="schema/lightDirectional.schema.json">`light/directional`</a>
   * <a href="schema/lightPoint.schema.json">`light/point`</a>
   * <a href="schema/lightSpot.schema.json">`light/spot`</a>
   * <a href="schema/material.schema.json">`material`</a>
   * <a href="schema/materialInstanceTechnique.schema.json">`material/instanceTechnique`</a>
   * <a href="schema/materialInstanceTechniqueValues.schema.json">`material/instanceTechnique/values`</a>
   * <a href="schema/mesh.schema.json">`mesh`</a>
   * <a href="schema/meshPrimitive.schema.json">`mesh/primitive`</a>
   * <a href="schema/meshPrimitiveAttribute.schema.json">`mesh/primitive/attribute`</a>
   * <a href="schema/node.schema.json">`node`</a>
   * <a href="schema/nodeInstanceSkin.schema.json">`node/instanceSkin`</a>
   * <a href="schema/program.schema.json">`program`</a>
   * <a href="schema/sampler.schema.json">`sampler`</a>
   * <a href="schema/scene.schema.json">`scene`</a>
   * <a href="schema/shader.schema.json">`shader`</a>
   * <a href="schema/skin.schema.json">`skin`</a>
   * <a href="schema/technique.schema.json">`technique`</a>
   * <a href="schema/techniqueParameters.schema.json">`technique/parameters`</a>
   * <a href="schema/techniquePass.schema.json">`technique/pass`</a>
   * <a href="schema/techniquePassDetails.schema.json">`technique/pass/details`</a>
   * <a href="schema/techniquePassDetailsCommonProfile.schema.json">`technique/pass/details/commonProfile`</a>
   * <a href="schema/techniquePassDetailsCommonProfileTexcoordBindings.schema.json">`technique/pass/details/commonProfile/texcoordBindings`</a>
   * <a href="schema/techniquePassInstanceProgram.schema.json">`technique/pass/instanceProgram`</a>
   * <a href="schema/techniquePassInstanceProgramAttribute.schema.json">`technique/pass/instanceProgram/attribute`</a>
   * <a href="schema/techniquePassInstanceProgramUniform.schema.json">`technique/pass/instanceProgram/uniform`</a>
   * <a href="schema/techniquePassStates.schema.json">`technique/pass/states`</a>
   * <a href="schema/texture.schema.json">`texture`</a>

<a name="conformance"></a>
# Conformance

<mark>*Todo: how do we tackle this?*</mark>

<a name="acknowledgements"></a>

# Annotated Example

## The Scene

## The Shader Source Code

The following example shows the GLSL source code for a Phong shader program that will be referenced by the example JSON used throughout this section. The vertex shader computes the screen space position of each vertex, using the world space transform defined in `u_modelViewMatrix` and the perspective projection defined in `u_projectionMatrix`. 

In addition, the shader computes the varying values `v_normal`, `v_texcoord0` and `v_light0Direction` that will be used in the fragment shader. The normal vector and light direction are computed using matrices that are based on the state of the current camera and lights. The runtime determines how to pass these values based on *semantics*, described later in this section.

```
precision highp float;
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;
uniform mat3 u_normalMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
attribute vec2 a_texcoord0;
varying vec2 v_texcoord0;
varying vec3 v_light0Direction;
uniform mat4 u_light0Transform;
void main(void) {
vec4 pos = u_modelViewMatrix * vec4(a_position,1.0);
v_normal = u_normalMatrix * a_normal;
v_texcoord0 = a_texcoord0;
v_light0Direction = mat3(u_light0Transform) * vec3(0.,0.,1.);
gl_Position = u_projectionMatrix * pos;
}
```

The fragment shader computes the fragment color using the varying values `v_normal`, `v_texcoord0` and `v_light0Direction` computed by the vertex shader, as well as several material properties defined as uniform variables, including ambient color, emissive color and a diffuse texture.

```
precision highp float;
varying vec3 v_normal;
uniform vec4 u_ambient;
varying vec2 v_texcoord0;
uniform sampler2D u_diffuse;
uniform vec4 u_emission;
uniform vec4 u_specular;
uniform float u_shininess;
varying vec3 v_light0Direction;
uniform vec3 u_light0Color;
void main(void) {
vec3 normal = normalize(v_normal);
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
vec3 diffuseLight = vec3(0., 0., 0.);
vec4 emission;
vec4 ambient;
vec4 specular;
ambient = u_ambient;
diffuse = texture2D(u_diffuse, v_texcoord0);
emission = u_emission;
specular = u_specular;
vec3 specularLight = vec3(0., 0., 0.);
{
float specularIntensity = 0.;
float attenuation = 1.0;
vec3 l = normalize(v_light0Direction);
vec3 h = normalize(l+vec3(0.,0.,1.));
specularIntensity = max(0., pow(max(dot(normal,h), 0.) , u_shininess)) * attenuation;
specularLight += u_light0Color * specularIntensity;
diffuseLight += u_light0Color * max(dot(normal,l), 0.) * attenuation;
}
specular.xyz *= specularLight;
color.xyz += specular.xyz;
diffuse.xyz *= diffuseLight;
color.xyz += diffuse.xyz;
color.xyz += emission.xyz;
color = vec4(color.rgb * diffuse.a, diffuse.a);
gl_FragColor = color;
}

```



# Acknowledgments

* Brandon Jones, for the first version of Three.js loader and all his support in the early days of this project.
* Tom Fili, Analytical Graphics, Inc.
* Scott Hunter, Analytical Graphics, Inc.
* Ed Mackey, Analytical Graphics, Inc.

<a name="references"></a>
# References

* [WebGL 1.0.2 spec](https://www.khronos.org/registry/webgl/specs/1.0/)
* [COLLADA 1.5 spec](http://www.khronos.org/files/collada_spec_1_5.pdf)
