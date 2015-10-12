<p align="center">
<img src="figures/glTF_300.jpg" />
</p>

*Draft, Version 1.0*

The GL Transmission Format (glTF) is a runtime asset delivery format for GL APIs: WebGL, OpenGL ES, and OpenGL.  glTF bridges the gap between 3D content creation tools and modern GL applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.


> 
_This is a draft specification; it is incomplete and may change before ratification.  We have made it available early in the spirit of transparency to receive community feedback.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback._

Editors

* Patrick Cozzi, Cesium
* Tony Parisi, WEVR

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
  * [IDs and Names](#ids-and-names)
  * [Scenes](#scenes)
  * [Accessing Binary Data](#binary-data)
  * [Geometry and Meshes](#geometry-and-meshes)
  * [Materials and Shading](#materials-and-shading)
  * [Cameras](#cameras)
  * [Lights](#lights)
  * [Animations](#animations)
  * [Metadata](#metadata)
  * [Specifying Extensions](#specifying-extensions)
* [Properties Reference](#properties)
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

glTF solves these problems by providing a vendor- and runtime-neutral format that can be loaded and rendered with minimal processing. The format combines an easily parseable JSON scene description with one or more binary files representing geometry, animations and other rich data. Binary data is stored in such a way that it can be loaded directly into GL buffers without additional parsing or other manipulation. Using this approach, glTF is able to faithfully preserve full hierarchial scenes with nodes, meshes, cameras, materials and animations, while enabling efficient delivery and fast loading.

<a name="glTFbasics"></a>

## glTF Basics

glTF assets are JSON files plus supporting external data. Specifically, a glTF asset is represented by:

* A JSON-formatted file (`.gltf`) containing a full scene description: node hierarchy, materials, cameras, as well as descriptor information for meshes, shaders, animations and other constructs
* Binary files (`.bin`) containing geometry and animation data, and other buffer-based data
* Image files (`.jpg`, `.png`, etc.) for textures
* GLSL text files (`.glsl`) for GLSL shader source code

Assets defined in other formats, such as images and GLSL shader source code, may be stored in external files referenced via URI or embedded directly into the JSON using  [data URIs](https://developer.mozilla.org/en/data_URIs).

<a name="designgoals"></a>

## Design Goals

glTF has been designed to meet the following goals:

* *Compact file sizes.* While web developers like to work with clear text as much as possible, clear text encoding is simply not practical for transmitting 3D data due to sheer size. The glTF JSON file itself is clear text, but it is compact and rapid to parse. All large data such as geometry and animations are stored in binary files that are much smaller than equivalent text representations.
* *Fast loading.* glTF data structures have been designed to mirror the GL API data as closely as possible, both in the JSON and binary files, to reduce load times. For example, binary data for meshes can be loaded directly into WebGL typed arrays with a simple data copy; no parsing or further processing is required.
* *Runtime-independence.* glTF makes no assumptions about the target application or 3D engine. glTF specifies no runtime behaviors other than rendering and animation.
* *Complete 3D scene representation.* Exporting single objects from a modeling package is not sufficient for many applications. Often, authors want to load entire scenes, including nodes, transformations, transform hierarchy, meshes, materials, cameras, and animations into their applications. glTF strives to preserve all of this information for use in the downstream application.
* *Extensibility.* While the initial base specification supports a rich feature set, there will be many opportunities for growth and improvement. glTF defines a mechanism that allows the addition of both general-purpose and vendor-specific extensions.

The design of glTF takes a pragmatic approach. The format is meant to mirror the GL APIs as closely as possible, but if it did only that, there would be no cameras, animations and other features typically found in both modeling tools and runtime systems, and much semantic information would be lost in the translation. By supporting these common constructs, glTF content can not only load and render, but it can be immediately usable in a wider range of applications and require less duplication of effort in the content pipeline.

The following are intentionally **not** goals for the initial design of glTF:

* *glTF is not a streaming format.* The binary data in glTF is inherently streamable, and the buffer design allows for fetching data incrementally. But there are no other streaming constructs in the format, and no conformance requirements for an implementation to stream data versus downloading it in its entirety before rendering.
* *glTF is not intended to be human-readable,* though by virtue of being represented in JSON, it is developer-friendly.

Version 1.0 of glTF does not define compression for geometry and other rich data. However, the design team believes that compression is a very important part of a transmission standard, and there is already work underway to define compression extensions.

> The 3D Formats Working Group is developing partnerships to define the codec options for geometry compression.  glTF defines the node hierarchy, materials, animations, and geometry, and will reference the external compression specs.

<a name="mimetypes"></a>
## File Extensions and MIME Types

<mark>*Todo: someone needs to get a MIME type RFC going with IANA...*</mark>

* `*.gltf` files use `model/gltf+json`
* `*.bin` files use `application/octet-stream`
* `*.glsl` files use `text/plain`
* Texture files use the official `image/*` type based on the specific image format. For compatibility with modern web browsers, the following image formats are supported: .jpg, .png, .bmp, and .gif. 

## URIs

glTF uses URIs to reference buffers, shaders, and image resources. These URIs may point to external files or be data URIs that embed resources in the JSON. This allows the application to decide the best approach for delivery: if different assets share many of the same geometries, animations, textures, or shaders, separate files may be preferred to reduce the total amount of data requested. With separate files, applications can progressively load data and do not need to load data for parts of a model that are not visible. If an application cares more about single-file deployment, embedding data may be preferred even though it increases the overall size due to base64 encoding and does not support progressive or on-demand loading.

<a name="concepts"></a>
# Concepts

*This section is non-normative.*

<a name="file-structure"></a>

<a name="ids-and-names"></a>
## IDs and Names

_IDs_ are internal string identifiers used to reference parts of a glTF asset, e.g., a `bufferView` refers to a `buffer` by specifying the buffer's ID.  For example:

```javascript
"buffers": {
    "a-buffer-id": {
        "byteLength": 1024,
        "type": "arraybuffer",
        "uri": "path-to.bin"
    }
},
"bufferViews": {
    "a-bufferView-id": {
        "buffer": "a-buffer-id",
        "byteLength": 512,
        "byteOffset": 0
    }
}
```

In this example, `"a-buffer-id"` and `"a-bufferView-id"` are IDs.  The bufferView refers to the buffer using the buffer's ID: `"buffer": "a-buffer-id"`.

IDs for top-level glTF dictionary objects (`accessors`, `animations`, `buffers`, `bufferViews`, `cameras`, `images`, `materials`, `meshes`, `nodes`, `programs`, `samplers`, `scenes`, `shaders`, `skins`, `techniques`, and `textures`) are in the same namespace and are unique. 

For example, the following is **not** allowed:

```javascript
"buffers": {
    "an-id": {
        // ...
    }
},
"bufferViews": {
    "an-id": { // Not allowed since this ID is already used
        // ...
    }
}
```

IDs for non top-level glTF dictionary objects (e.g., `animation.samplers`) are each in their own namespace.  IDs are unique within the object as enforced by JSON.  For example, the following **is** allowed:

```javascript
"animations": {
    "animation-0": {
        // ...
        "samplers": {
            "animation-sampler-id": {
                // ...
            }
        }
    },
    "animation-1": {
        // ...
        "samplers": {
            "animation-sampler-id": { // Does not collide with the sampler ID in the other animation
                // ...
            }
        }
    }
}
```

Whereas IDs are used for internal glTF references, _names_ are used for application-specific uses such as display.  glTF objects that are commonly accessed from an application have a `name` string property for this purpose.  These property values are not guaranteed to be unique as they are intended to contain values created when the asset was authored.

<a name="scenes"></a>
## Scenes

The glTF asset contains one or more *scenes*, the set of visual objects to render. Scenes are defined in a dictionary object `scenes`. An additional property, `scene` (note singular), identifies which of the scenes in the dictionary is to be displayed at load time. 

The following example defines a glTF asset with a single scene, `defaultScene`, that contains a single node, `node_1`.

```javascript
"scene": "defaultScene",
"scenes": {
    "defaultScene": {
        "nodes": [
            "node_1"
        ]
    }
},
```

### Nodes and Hierarchy

The glTF asset defines one or more *nodes*, that is, the objects comprising the scene to render. 

Each node can contain one or more meshes, a skin instance, a joint name, or a camera, defined in the `meshes`, `skeletons`, `skin`, `jointName`, and `camera` properties, respectively.

Nodes have a optional `name` property.

Node also have transform properties, as described in the next section.

Nodes are organized in a parent-child hierarchy known informally as the *node hierarchy*. 

The node hierarchy is defined using a node's `children` property, as in the following example:

```javascript
    "node-box": {
        "children": [
            "node_1",
            "node-camera_1"
        ],
        "name": "Box"
    },
```

The node `node-box` has two children, `node_1` and `node-camera_1`. Each of those nodes could in turn have their own children, creating a hierarchy of nodes.

>For Version 1.0 conformance, the glTF node hierarchy is not a directed acyclic graph (DAG) or *scene graph*, but a strict tree. That is, no node may be a direct or indirect descendant of more than one node. This restriction is meant to simplify implementation and facilitate conformance. The restriction may be lifted after Version 1.0.


### Transforms

Any node can define a local space transformation either by supplying a `matrix` property, or any of `translation`, `rotation`, and `scale`  properties (also known as *TRS properties*). `translation` and `scale` are `FLOAT_VEC3` values in the local coordinate system. `rotation` is a `FLOAT_VEC4` unit quaternion value, `(x, y, z, w)`, in the local coordinate system.

In the example below, `node-box` defines non-default rotation and translation.

```javascript
    "node-box": {
        "children": [
            "node_1",
            "node-camera_1"
        ],
        "name": "Box",
        "rotation": [
            0,
            0,
            1,
            0
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

```javascript
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


<mark>*Todo:conformance - what if author supplies both matrix and TRS? Which wins? Undefined?*</mark>

### Coordinate System and Units

glTF uses a right-handed coordinate system with Y up and positive Z pointing out of the screen.

The units for all linear distances are meters.

All angles are in radians unless otherwise specified.

<a name="accessing-binary-data"></a>
## Accessing Binary Data


### Buffers and Buffer Views

A *buffer* is data stored as a binary blob in a file. The buffer can contain a combination of geometry, animation, and skins. 

Binary blobs allow efficient creation of GL buffers and
textures since they require no additional parsing, except perhaps decompression. An asset can have any number of buffer files for flexibility for a wide array of applications.

All buffers are stored in the assets `buffers` property.

The following example defines a buffer. The `byteLength` property specifies the size of the buffer file. The `type` property specifies how the data is stored, either as a binary array buffer or text. The `uri` property is the URI to the buffer data. Buffer data may also be stored within the glTF file as base64-encoded data and reference via data URI. 

```javascript
    "buffers": {
        "duck": {
            "byteLength": 102040,
            "type": "arraybuffer",
            "uri": "duck.bin"
        }
    },
```

A *bufferView* represents a subset of data in a buffer, defined by an integer offset into the buffer specified in the `byteOffset` property, a `byteLength` property to specify length of the buffer view. The bufferView also defines a `target` property to indicate the target data type, either ARRAY_BUFFER, ELEMENT_ARRAY_BUFFER, or an object describing animation or skinning target data. This enables the implementation to readily create and populate the buffers in memory.

The following example defines two buffer views: an ELEMENT_ARRAY_BUFFER view `bufferView_29`, which holds the indices for an indexed triangle set, and `bufferView_30`, an ARRAY_BUFFER that holds the vertex data for the triangle set.


```javascript
    "bufferViews": {
        "bufferView_29": {
            "buffer": "duck",
            "byteLength": 25272,
            "byteOffset": 0,
            "target": 34963
        },
        "bufferView_30": {
            "buffer": "duck",
            "byteLength": 76768,
            "byteOffset": 25272,
            "target": 34962
        }
    },
```

buffers and bufferViews do not contain type information. They simply define the raw data for retrieval from the file. Objects within the glTF file (meshes, skins, animations) never access buffers or bufferViews directly, but rather via *accessors*.

Buffer data is little endian.

### Accessors

All large data for meshes, skins and animations is stored in buffers and retrieved via accessors.

An *accessor* defines a method for retrieving data as typed arrays from within a bufferView. The accessor specifies a component type (e.g. `FLOAT`) and a data type (e.g. `VEC3`), which when combined define the complete data type for each array. The accessor also specifies the location and size of the data within the bufferView using the properties `byteOffset` and `count`. count specifies the number of attributes within the bufferView, *not* the number of bytes.

All accessors are stored in the asset's `accessors` property.

The following fragment shows two accessors, a scalar accessor for retrieving a primitive's indices and a 3-float-component vector accessor for retrieving the primitive's position data.

```javascript
"accessors": {
    "accessor_21": {
        "bufferView": "bufferView_29",
        "byteOffset": 0,
        "byteStride": 0,
        "componentType": 5123,
        "count": 12636,
        "type": "SCALAR"
    },
    "accessor_23": {
        "bufferView": "bufferView_30",
        "byteOffset": 0,
        "byteStride": 12,
        "componentType": 5126,
        "count": 2399,
        "max": [
            0.961799,
            1.6397,
            0.539252
        ],
        "min": [
            -0.692985,
            0.0992937,
            -0.613282
        ],
        "type": "VEC3"
    },
```

#### Accessor Attribute Size

The following tables can be used to compute the size of an accessor's attribute type.

| `componentType` | Size in bytes |
|:-:|:-:|
| `5120` (BYTE) | 1 |
| `5121`(UNSIGNED_BYTE) | 1 |
| `5122` (SHORT) | 2 |
| `5123` (UNSIGNED_SHORT) | 2 |
| `5126` (FLOAT) | 4 |

| `type` | Number of components |
|:-:|:-:|
| `"SCALAR"` | 1 |
| `"VEC2"` | 2 |
| `"VEC3"` | 3 |
| `"VEC4"` | 4 |
| `"MAT2"` | 4 |
| `"MAT3"` | 9 |
| `"MAT4"` | 16 |

The size of an accessor's attribute type, in bytes, is 
`(size in bytes of the 'componentType') * (number of components defined by 'type')`.

For example:

```javascript
"accessor_1": {
    "bufferView": "bufferView_1",
    "byteOffset": 7032,
    "byteStride": 12,
    "componentType": 5126, // FLOAT
    "count": 586,
    "type": "VEC3"
}
```

In this accessor, the `componentType` is `5126` (FLOAT) so each component is four bytes.  The `type` is `"VEC3"` so there are three components.  The size of the attribute type is 12 bytes (`4 * 3`).

#### BufferView and Accessor Byte Alignment

The offset of an `accessor` into a `bufferView` (i.e., `accessor.byteOffset`) and the offset of an `accessor` into a `buffer` (i.e., `accessor.byteOffset + bufferView.byteOffset`) must be a multiple of the size of the accessor's attribute type.

> **Implementation Note:** This allows a runtime to efficiently create a single arraybuffer from a glTF `buffer` or an arraybuffer per `bufferView`, and then use an `accessor` to create a typed array view (e.g., `Float32Array`) into an arraybuffer without copying it because the byte offset of the typed array view is a multiple of the size of the type (e.g., `4` for `Float32Array`).

Consider the following example:

```javascript
"bufferView_1": {
    "buffer": "buffer_1",
    "byteLength": 17136,
    "byteOffset": 620,
    "target": 34963
},
"accessor_1": {
    "bufferView": "bufferView_1",
    "byteOffset": 4608,
    "byteStride": 0,
    "componentType": 5123, // UNSIGNED_SHORT
    "count": 5232,
    "type": "SCALAR"
}
```
The size of the accessor attribute type is two bytes (the `componentType` is unsigned short and the `type` is scalar).  The accessor's `byteOffset` is also divisible by two.  Likewise, the accessor's offset into `buffer_1` is `5228 ` (`620 + 4608`), which is divisible by two.


<a name="geometry-and-meshes"></a>

## Geometry and Meshes

Any node can contain one or more meshes, defined in its `meshes` property. Any node can contain one skinned mesh instance, defined using a combination of the properties `meshes`, `skeletons`, and `skin`. A node can either contain meshes or a single skinned mesh instance, but not both.

### Meshes

In glTF, meshes are defined as arrays of *primitives*. Primitives correspond to the data required for GL drawElements calls. Primitives specify one or more `attributes`, corresponding to the vertex attributes used in the draw calls. Indexed primitives also define an `indices` property. Attributes and indices are defined as accessors. Each primitive also specifies a material and a primitive type that coresponds to the GL primitive type (e.g. triangle set).

The following example defines a mesh containing one triangle set primitive:

```javascript
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

Each attribute is defined as a property of the `attributes` object. The name of the property corresponds to an enumerated value identifying the vertex attribute, such as `POSITION`. This value will be mapped to a specific named attribute within the GLSL shader for the mesh, as defined in the material technique's `parameters` dictionary property (see Materials and Shading, below). The value of the property is the ID  of an accessor that contains the data.

Valid attribute semantics include `POSITION`, `NORMAL`, `TEXCOORD`, `COLOR`, `JOINT`, `JOINTMATRIX`, and `WEIGHT`.  Attribute semantics can be of the form `[semantic]_[set_index]`, e.g, `TEXCOORD_0`, `TEXCOORD_1`, etc.

> **Implementation note:** Each primitive corresponds to one WebGL draw call (engines are, of course, free to batch draw calls). When a primitives `indices` property is defined, it references the accessor to use for index data, and GL's `drawElements` function should be used. When the `indices` property is not defined, GL's `drawArrays` function should be used with a count equal to the count property of any of the accessors referenced by `attributes` property (they are all equal for a given primitive).

                    
                    
### Skins

All skins are stored in the `skins` property of the asset, by name. Each skin is defined by a `bindShapeMatrix` property, which describes how to pose the skin's geometry for use with the joints; the `inverseBindMatrices` property, used to bring coordinates being skinned into the same space as each joint; and a `jointNames` array property that lists the joints used to animate the skin. Each joint name must correspond to the joint of a node in the hierarchy, as designated by the node's `jointName` property.
  

```javascript
    "skins": {
        "skin_1": {
            "bindShapeMatrix": [
                0,
                -0.804999,
                0.172274,
                0,
                0,
                0.172274,
                0.804999,
                0,
                -0.823226,
                0,
                0,
                0,
                -127.093,
                -393.418,
                597.2,
                1
            ],
            "inverseBindMatrices": "bind-matrices_1",
            "jointNames": [
                "Bone1",
                "Bone2",
            ]
        }
    },
```

#### Skin Instances

A skin is instanced within a node using a combination of the node's `meshes`, `skeletons` and `skin` properties. The meshes for a skin instance are defined in the `meshes` property. The `skeletons` property contains one or more skeletons, each of which is the root of a node hierarchy. The `skin` property contains the ID of the skin to instance. The example below defines a skin instance that uses a single mesh and skeleton.

```javascript
    "node_1": {
        "children": [],
        "meshes": [
            "skinned-mesh_1"
        ],
        "skeletons": [
            "skeleton-root_1"
        ],
        "skin": "skin_1"
```

#### Skinned Mesh Attributes

The mesh for a skin is defined with vertex attributes that are used in skinning calculations in the vertex shader. The following mesh skin defines `JOINT` and `WEIGHT` vertex attributes for a triangle mesh primitive:

```javascript
    "meshes": {
        "skinned-mesh_1": {
            "name": "skinned-mesh_1",
            "primitives": [
                {
                    "attributes": {
                        "JOINT": "accessor_179",
                        "NORMAL": "accessor_165",
                        "POSITION": "accessor_163",
                        "TEXCOORD_0": "accessor_167",
                        "WEIGHT": "accessor_176"
                    },
                    "indices": "accessor_161",
                    "material": "material_1",
                    "primitive": 4
                }
            ]
        }
    },
```


#### Joint Hierarchy

The joint hierarchy used in animation is simply the glTF node hierarchy, with each node designated as a joint using the `jointName` property. Any joints listed in the skin's `jointNames` property must correspond to a node that has the same `jointName` property. The following example defines a joint hierarchy of two joints with `root-node` at the root, identified as a joint using the joint name `Bone1`.

```javascript
    "nodes": {
        "root-node": {
            "children": [
                "child-node"
            ],
            "jointName": "Bone1",
            "name": "root",
            "rotation": [
                0.888952,
                0.414921,
                0.19392,
                0.0773304
            ],
            "scale": [
                1,
                1,
                1
            ],
            "translation": [
                4.61599,
                -2.032e-06,
                -5.08e-08
            ]
        },
        "child-node": {
            "children": [],
            "jointName": "Bone2",
            "name": "head",
            "rotation": [
                1,
                0,
                0,
                0
            ],
            "scale": [
                1,
                1,
                1
            ],
            "translation": [
                8.76635,
                4.318e-07,
                -1.778e-07
            ]
        },
```


<a name="materials-and-shading"></a>
## Materials and Shading

A material is defined as an instance of a shading technique along with parameterized values, e.g. light colors, specularity, or shininess. Shading techniques use JSON properties to describe data types and semantics for GLSL vertex and fragment shader programs.

Materials are stored in the assets `materials` property, which contains one or more material definitions. The following example shows a Blinn shader with ambient color, diffuse texture, emissive color, shininess and specular color.

```javascript
"materials": {
    "blinn-1": {
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
        "name": "blinn1"
    }
},
```

The `technique` property is optional; if it is not supplied, and no extension is present that defines material properties, then the object shall be rendered using a default material with 50% gray emissive color.

### Techniques

A technique describes the shading used for a material. The asset's techniques are stored in the `techniques` dictionary property.

The following example shows a technique and the properties it defines. This section describes each property in detail.

```javascript
"techniques": {
    "technique1": {
    	 // parameter definitions
        "parameters": {
            "ambient": {
                "type": 35666
            },
            "diffuse": {
                "type": 35678
            },
            "light0Color": {
                "type": 35665,
                "value": [
                    1,
                    1,
                    1
                ]
            },
            "light0Transform": {
                "semantic": "MODELVIEW",
                "node": "directionalLight1",
                "type": 35676
            }
            // more parameters
        },
    	// program, attributes and uniforms definitions
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
	    },
		// render states
		"states": {
		    "enable": [
		        2884,
		        2929
		    ]
		}
        
```

#### Parameters

Each technique has zero or more parameters; each parameter is defined by a type (GL types such as a floating point number, vector, texture, etc), a default value, and potentially a semantic describing how the runtime is to interpret the data to pass to the shader. When a material instances a technique, the name of each supplied value in its `values` property corresponds to one of the parameters defined in the technique. 

The above example illustrates several parameters. The property `ambient` is defined as a `FLOAT_VEC4` type; `diffuse` is defined as a `SAMPLER_2D`; and `light0color` is defined as a `FLOAT_VEC3` with a default color value of white. 


#### Semantics

Technique parameters may also optionally define a *semantic* - an enumerated value describing how the runtime is to interpret the data to be passed to the shader. 

In the above example, the parameter `light0Transform` defines the `MODELVIEW` semantic, which corresponds to the world space position of the node reference in the property `node`, in this case the node `directionalight1`, which refers to a node that contains a light source.

If no `node` property is supplied for a semantic, the semantic is implied in a context-specific manner: either to the node which is being rendered, or in the case of camera-specific semantics, the semantic applies to the current camera, as in the following fragment, which defines a parameter named `projectionMatrix` that is derived from the implementation's projection matrix.

```javascript
"projectionMatrix": {
    "semantic": "PROJECTION",
    "type": 35676
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


#### Program Instances

The `program` property of a technique creates an instance of a shader program. The value of the property is the ID of a program defined in the asset's `programs` dictionary object (see next section). A shader program may be instanced multiple times within the glTF asset.

Attributes and uniforms passed to the program instance's shader code are defined in the `attributes` and `uniforms` properties of the technique, respectively. The following example shows the definitions for a technique's program instance, attributes and techniques:


```javascript
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
    },
```

The `attributes` property specifies the vertex attributes of the data that will be passed to the shader. Each attribute's name is a string that corresponds to the attribute name in the GLSL source code. Each attribute's value is a string that references a parameters defined in the technique's `parameters` property, where the type and semantic of the attribute is defined.

The `uniforms` property specifies the uniform variables that will be passed to the shader. Each uniform's name is a string that corresponds to the uniform name in the GLSL source code. Each uniform's value is a string that references a parameter defined in the technique's `parameters` property, where the type and semantic of the uniform is defined.

#### Render States

Render states define the fixed-function GL state when a primitive is rendered. The technique's `states` property contains two properties:

* `enable`: an array of integers corresponding to boolean GL states that should be enabled using GL's `enable` function.
* `functions`: a dictionary object containing properties corresponding to the names of GL state functions to call.  Each property is an array, where the elements correspond to the arguments to the GL function.

Valid values for elements in the `enable` array are: `3042` (`BLEND`), `2884` (`CULL_FACE`), `2929` (`DEPTH_TEST`), `32823` (`POLYGON_OFFSET_FILL`), `32926` (`SAMPLE_ALPHA_TO_COVERAGE`), and `3089` (`SCISSOR_TEST`).  If any of these values are not in the array, the GL state should be disabled (which is the GL default state).  If the `enable` array is not defined in the `pass`, all of these boolean GL states are disabled.

Each property in `functions` indicates a GL function to call and the arguments to provide.  Valid property names are: `"blendColor"`, `"blendEquationSeparate"`, `"blendFuncSeparate"`, `"colorMask"`, `"cullFace"`, `"depthFunc"`, `"depthMask"`, `"depthRange"`, `"frontFace"`, `"lineWidth"`, `"polygonOffset"`, and `"scissor"`.  If a property is not defined, the GL state for that function should be set to the default value(s) shown in the example below.

The following example `states` object indicates to enable all boolean states (see the `enable` array) and use the default values for all the GL state functions (which could be omitted).

```javascript
"states": {
    "enable": [ 3042,  // BLEND
                    2884,  // CULL_FACE
                    2929,  // DEPTH_TEST
                    32823, // POLYGON_OFFSET_FILL
                    32926, // SAMPLE_ALPHA_TO_COVERAGE
                    3089   // SCISSOR_TEST
                    ], // empty by default
    "functions": {
      "blendColor": [0.0, 0.0, 0.0, 0.0], // (red, green, blue, alpha)
      "blendEquationSeparate": [
                 32774, // FUNC_ADD (rgb)
                 32774  // FUNC_ADD (alpha)
                 ],
      "blendFuncSeparate": [
                    1,     // ONE (srcRGB)
                    1,     // ONE (srcAlpha)
                    0,     // ZERO (dstRGB)
                    0,     // ZERO (dstAlpha)
                    ],
      "colorMask"  : [true, true, true, true], // (red, green, blue, alpha)
      "cullFace"   : [1029], // BACK
      "depthFunc"  : [LESS], // 513
      "depthMask"  : [true],
      "depthRange": [0.0, 1.0], // (zNear, zFar)
      "frontFace"  : [2305], // CCW
      "lineWidth"  : [1.0],
      "polygonOffset": [0.0, 0.0], // (factor, units)
      "scissor"    : [0, 0, 0, 0], // (x, y, width, height)
    }
}
```

The following example shows a typical `"states"` object for closed opaque geometry.  Culling and the depth test are enabled, and all other GL states are set to the default value (disabled).
```javascript
	"states": {
	    "enable": [
	        2884,
	        2929
	    ]
	}
```

> **Implementation Note**: It is recommended that a runtime use the minimal number of GL state function calls.  This generally means ordering draw calls by technique, and then only making GL state function calls for the states that vary between techniques.


#### Programs

GLSL shader programs are stored in the asset's `programs` property. This property contains one or more objects, one for each program.

Each shader program includes an `attributes` property, which specifies the vertex attributes that will be passed to the shader, and the properties `fragmentShader` and `vertexShader`, which reference the files for the fragment and vertex shader GLSL source code, respectively. 

```javascript
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

#### Shaders

Shader source files are stored in the asset's `shaders` property, which contains one or more shader source files. Each shader specifies a `type` (vertex or fragment, defined as GL enum types) and a `uri` to the file. Shader URIs may be URIs to external files or data URIs, allowing the shader content to be embedded as base64-encoded data in the asset.

```javascript
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

#### Textures

Textures can be used as uniform inputs to shaders. The following material definition specifies a diffuse texture using the `diffuse` parameter.

```javascript
"materials": {
    "material-1": {
        "technique": "technique1",
        "values": {
            "diffuse": "texture_file2",
        }
        "name": "material_1"
    }
},
```


All textures are stored in the asset's `textures` property. A texture is defined by an image file, denoted by the `source` property; `format` and `internalFormat` specifiers, corresponding to the GL texture format types; a `target` type for the sampler; a sampler identifier (`sampler`), and a `type` property defining the internal data format. Refer to the GL definition of `texImage2D()` for more details.

```javascript
"textures": {
    "texture_file2": {
        "format": 6408,
        "internalFormat": 6408,
        "sampler": "sampler_0",
        "source": "file2",
        "target": 3553,
        "type": 5121
    }
}
```

#### Images
 
Images referred to by textures are stored in the `images` property of the asset. Each image contains a URI to an external file in one of the supported images formats. Image data may also be stored within the glTF file as base64-encoded data and reference via data URI. For example:

```javascript
"images": {
    "file2": {
        "uri": "duckCM.png"
    }
},
```

#### Samplers

Samplers are stored in the `samplers` property of the asset. Each sampler specifies filter and wrapping options corresponding to the GL types. The following example defines a sampler with linear mag filtering, linear mipmap min filtering, and repeat wrapping in S and T.


```javascript
"samplers": {
    "sampler_0": {
        "magFilter": 9729,
        "minFilter": 9987,
        "wrapS": 10497,
        "wrapT": 10497
    }
},
```

> **Mipmapping Implementation Note**: When a sampler's minification filter (`minFilter`) uses mipmapping (`NEAREST_MIPMAP_NEAREST`, `NEAREST_MIPMAP_LINEAR`, `LINEAR_MIPMAP_NEAREST`, or `LINEAR_MIPMAP_LINEAR`), any texture referencing the sampler needs to have mipmaps, e.g., by calling GL's `generateMipmap()` function.


> **Non-Power-Of-Two Texture Implementation Note**: glTF does not guarantee that a texture's dimensions are a power-of-two.  At runtime, if a texture's width or height is not a power-of-two, the texture needs to be resized so its dimensions are powers-of-two if the `sampler` the texture references:
> * Has a wrapping mode (either `wrapS` or `wrapT`) equal to `REPEAT` or `MIRRORED_REPEAT`, or
> * Has a minification filter (`minFilter`) that uses mipmapping (`NEAREST_MIPMAP_NEAREST`, `NEAREST_MIPMAP_LINEAR`, `LINEAR_MIPMAP_NEAREST`, or `LINEAR_MIPMAP_LINEAR`).

<a name="cameras"></a>
## Cameras

Cameras define viewport projections. The projection can be perspective or orthographic. Cameras are contained in nodes and thus can be transformed. Their world-space positions can be used in shader calculations, and their projection matrices can be used in shader semantics auch as PROJECTION.

Cameras are stored in the asset's `cameras` property. Each camera defines a `type` property that designates the type of projection (perspective or orthographic), and either a `perspective` or `orthographic` property that defines the details.

The following example defines a perspective camera with supplied values for Y field of view (in degrees), aspect ratio, and near and far clipping planes.

```javascript
"cameras": {
    "camera_0": {
        "perspective": {
            "aspect_ratio": 1.5,
            "yfov": 37.8492,
            "zfar": 100,
            "znear": 0.01
        },
        "type": "perspective"
    }
}
```

<a name="animations"></a>
## Animations

glTF supports articulated and skinned animation via key frame animations of nodes' transforms. Key frame data is stored in buffers and referenced in animations using accessors.

> Note: glTF 1.0 only supports animating node transforms. A future version of the specification may support animating arbitrary properties, such as material colors and texture transform matrices.

All animations are stored in the `animations` property of the asset. An animation is defined as a set of channels (the `channels` property), a set of parameterized inputs (`parameters`) representing the key frame data, samplers that interpolate between the key frames (the `samplers` property) , and a `count` property indicating the number of key frame inputs present in the parameters data. The value in `count` must be less than or equal to number of entries in the shortest parameters value.

The following example defines an animating camera node. 

```javascript
        "animation_0": {
            "channels": [
                {
                    "sampler": "animation_0_scale_sampler",
                    "target": {
                        "id": "node-cam01-box",
                        "path": "scale"
                    }
                },
                {
                    "sampler": "animation_0_translation_sampler",
                    "target": {
                        "id": "node-cam01-box",
                        "path": "translation"
                    }
                },
                {
                    "sampler": "animation_0_rotation_sampler",
                    "target": {
                        "id": "node-cam01-box",
                        "path": "rotation"
                    }
                }
            ],
            "count": 901,
            "parameters": {
                "TIME": "animAccessor_0",
                "rotation": "animAccessor_3",
                "scale": "animAccessor_1",
                "translation": "animAccessor_2"
            },
            "samplers": {
                "animation_0_rotation_sampler": {
                    "input": "TIME",
                    "interpolation": "LINEAR",
                    "output": "rotation"
                },
                "animation_0_scale_sampler": {
                    "input": "TIME",
                    "interpolation": "LINEAR",
                    "output": "scale"
                },
                "animation_0_translation_sampler": {
                    "input": "TIME",
                    "interpolation": "LINEAR",
                    "output": "translation"
                }
            }
        },
```

*Channels* connect the output values of the key frame animation to a specific node in the hierarchy. A channel's `sampler` property contains the ID of one of the samplers present in the containing animation's `samplers` property. The `target` property is an object that identifies which node to animate using its `id` property, and which property of the node to animate using `path`. Valid path names are `"translation"`, `"rotation"`, and `"scale."`

The animation's *parameters* define the inputs to the animation: a set of floating point scalar values representing time (the `"TIME"` input); and sets of three-component floating-point vectors representing translation, rotation and scale. Each of these inputs is stored in a buffer and accessed via an accessor.

Interpolation between keys is defined using *samplers*, which take an input value, such as time, and generate an output value based on the inputs defined in the `parameters` property, using the interpolation formula specified in the `interpolation` property.

> Note: glTF 1.0 animation samplers only support linear interpolation.

glTF animations can be used to drive articulated or skinned animations. Skinned animation is achieved by animating the joints in the skin's joint hierarachy. (See the section on Skins above.)

        
<a name="metadata"></a>
## Metadata

Asset metadata is described in the `asset` property. The asset metadata contains the following properties:

* a `copyright` property denoting authorship
* a `generator` property describing the tool, if any, that generated the asset
* a `premultipliedAlpha` property specifying if the shaders were generated with premultiplied alpha (see WebGL `getContext()` with premultipliedAlpha)
* a profile designation
* a `version` property denoting the specification version

Only the `version` property is required. Example:

```javascript
"asset": {
    "generator": "collada2gltf@f356b99aef8868f74877c7ca545f2cd206b9d3b7",
    "premultipliedAlpha": true,
    "profile": "WebGL 1.0.2",
    "version": 0.8
}
```

<mark>*Todo: Patrick what do we say about profiles, if anything?.*</mark>

<a name="specifying-extensions"></a>
## Specifying Extensions

glTF defines an extension mechanism that allows the base format to be extended with new capabilities. Any glTF object can have an optional `extensions` property, as in the following example.

```javascript
"a_shader": {
    "extensions": {
        "binary_glTF": {
            "bufferView": // ...
        }
    }
}
```

All extensions used in a model are listed the top-level `extensionsUsed` dictionary object, e.g.,

```javascript
"extensionsUsed": [
   "EXT_binary_glTF"
]
```

For more information on glTF extensions, consult the [extensions registry specification](../extensions/README.md).

<a name="properties"></a>
# Properties Reference

* [accessor](#reference-accessor)
* [animation](#reference-animation)
   * [channel](#reference-animation.channel)
      * [target](#reference-animation.channel.target)
   * [sampler](#reference-animation.sampler)
* [asset](#reference-asset)
   * [profile](#reference-asset.profile)
* [buffer](#reference-buffer)
* [bufferView](#reference-bufferView)
* [camera](#reference-camera)
   * [orthographic](#reference-camera.orthographic)
   * [perspective](#reference-camera.perspective)
* [glTF](#reference-glTF) (root property for model)
* [image](#reference-image)
* [material](#reference-material)
   * [values](#reference-material.values)
* [mesh](#reference-mesh)
   * [primitive](#reference-mesh.primitive)
      * [attribute](#reference-mesh.primitive.attribute)
* [node](#reference-node)
* [program](#reference-program)
* [sampler](#reference-sampler)
* [scene](#reference-scene)
* [shader](#reference-shader)
* [skin](#reference-skin)
* [technique](#reference-technique)
   * [attribute](#reference-technique.attribute)
   * [parameters](#reference-technique.parameters)
   * [states](#reference-technique.states)
   * [uniform](#reference-technique.uniform)
* [texture](#reference-texture)


<!-- ======================================================================= -->
<a name="reference-accessor"></a>
## accessor

A typed view into a [bufferView](#reference-bufferView).  A bufferView contain raw binary data.  An accessors provides a typed view into a bufferView or a subset of a bufferView similar to how WebGL's `vertexAttribPointer()` defines an attribute in a buffer.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**bufferView**|`string`|The id of the bufferView.| :white_check_mark: Yes|
|**byteOffset**|`integer`|The offset relative to the start of the bufferView in bytes.| :white_check_mark: Yes|
|**byteStride**|`integer`|The stride, in bytes, between attributes referenced by this accessor.|No, default: `0`|
|**componentType**|`integer`|The datatype of components in the attribute.| :white_check_mark: Yes|
|**count**|`integer`|The number of attributes referenced by this accessor.| :white_check_mark: Yes|
|**type**|`string`|Specifies if the attribute is a scalar, vector, or matrix.| :white_check_mark: Yes|
|**max**|`number[1-16]`|Maximum value of each component in this attribute.|No|
|**min**|`number[1-16]`|Minimum value of each component in this attribute.|No|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [accessor.schema.json](schema/accessor.schema.json)
* **Example**: [accessors.json](schema/examples/accessors.json)

### accessor.bufferView :white_check_mark: 

The id of the bufferView.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**`: >= 1`

### accessor.byteOffset :white_check_mark: 

The offset relative to the start of the bufferView in bytes.  This must be a multiple of the size of the data type.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**:` >= 0`
* **Related WebGL functions**: `vertexAttribPointer()` offset parameter

### accessor.byteStride

The stride, in bytes, between attributes referenced by this accessor.  When this is zero, the attributes are tightly packed.

* **Type**: `integer`
* **Required**: No, default: `0`
* **Minimum**:` >= 0`
* **Maximum**:` <= 255`
* **Related WebGL functions**: `vertexAttribPointer()` stride parameter

### accessor.componentType :white_check_mark: 

The datatype of components in the attribute.  Valid values correspond to WebGL enums: `5120` (BYTE), `5121` (UNSIGNED_BYTE), `5122` (SHORT), `5123` (UNSIGNED_SHORT), and `5126` (FLOAT).  The corresponding typed arrays are `Int8Array`, `Uint8Array`, `Int16Array`, `Uint16Array`, and `Float32Array`, respectively.

* **Type**: `integer`
* **Required**: Yes
* **Allowed values**: `5120`, `5121`, `5122`, `5123`, `5126`

### accessor.count :white_check_mark: 

The number of attributes referenced by this accessor, not to be confused with the number of bytes or number of components.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**:` >= 1`

### accessor.type :white_check_mark: 

Specifies if the attribute is a scalar, vector, or matrix, and the number of elements in the vector or matrix.

* **Type**: `string`
* **Required**: Yes
* **Allowed values**: `"SCALAR"`, `"VEC2"`, `"VEC3"`, `"VEC4"`, `"MAT2"`, `"MAT3"`, `"MAT4"`

### accessor.max

Maximum value of each component in this attribute.  When both min and max arrays are defined, they have the same length.  The length is determined by the value of the type property.

* **Type**: `number[1-16]`
* **Required**: No

### accessor.min

Minimum value of each component in this attribute.  When both min and max arrays are defined, they have the same length.  The length is determined by the value of the type property.

* **Type**: `number[1-16]`
* **Required**: No

### accessor.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### accessor.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### accessor.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-animation"></a>
## animation

A keyframe animation.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**channels**|[`animation.channel[]`](#reference-animation.channel)|An array of channels, each of which targets an animation's sampler at a node's property.|No, default: `[]`|
|**parameters**|`object`|A dictionary object of strings whose values are ids of accessors with keyframe data, e.g., time, translation, rotation, etc.|No, default: `{}`|
|**samplers**|`object`|A dictionary object of [`animation.sampler`](#reference-animation.sampler)s that combines input and output parameters with an interpolation algorithm to define a keyframe graph (but not its target).|No, default: `{}`|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [animation.schema.json](schema/animation.schema.json)
* **Example**: [animations.json](schema/examples/animations.json)

### animation.channels

An array of channels, each of which targets an animation's sampler at a node's property.

* **Type**: [`animation.channel[]`](#reference-animation.channel)
* **Required**: No, default: `[]`

### animation.parameters

A dictionary object of strings whose values are ids of accessors with keyframe data, e.g., time, translation, rotation, etc.

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `string`

### animation.samplers

A dictionary object of [`animation.sampler`](#reference-animation.sampler)s that combines input and output parameters with an interpolation algorithm to define a keyframe graph (but not its target).

* **Type**: `object`
* **Required**: No, default: `{}`
* **Type of each property**: `object`

### animation.name

The user-defined name of this object.  This is not necessarily unique, e.g., an animation and a buffer could have the same name, or two animations could even have the same name.

* **Type**: `string`
* **Required**: No

### animation.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### animation.extras

Application-specific data.

* **Type**: `any`
* **Required**: No




<!-- ======================================================================= -->
<a name="reference-animation.channel"></a>
## animation.channel

Targets an animation's sampler at a node's property.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**sampler**|`string`|The id of a sampler in this animation to use to compute the value for the target.| :white_check_mark: Yes|
|**target**|[`animation.channel.target`](#reference-animation.channel.target)|The id of the node and TRS property to target.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [animation.channel.schema.json](schema/animation.channel.schema.json)

### channel.sampler :white_check_mark: 

The id of a sampler in this animation to use to compute the value for the target, e.g., a node's translation, rotation, or scale (TRS).

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**`: >= 1`

### channel.target :white_check_mark: 

The id of the node and TRS property to target.

* **Type**: [`animation.channel.target`](#reference-animation.channel.target)
* **Required**: Yes

### channel.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### channel.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-animation.channel.target"></a>
## animation.channel.target

The id of the node and TRS property that an animation channel targets.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**id**|`string`|The id of the node to target.| :white_check_mark: Yes|
|**path**|`string`|The name of the node's TRS property to modify.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [animation.channel.target.schema.json](schema/animation.channel.target.schema.json)

### target.id :white_check_mark: 

The id of the node to target.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**`: >= 1`

### target.path :white_check_mark: 

The name of the node's TRS property to modify.

* **Type**: `string`
* **Required**: Yes
* **Allowed values**: `"translation"`, `"rotation"`, `"scale"`

### target.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### target.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-animation.sampler"></a>
## animation.sampler

Combines input and output parameters with an interpolation algorithm to define a keyframe graph (but not its target).

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**input**|`string`|The id of a parameter in this animation to use as keyframe input, e.g., time.| :white_check_mark: Yes|
|**interpolation**|`string`|Interpolation algorithm.|No, default: `"LINEAR"`|
|**output**|`string`|The id of a parameter in this animation to use as keyframe output.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [animation.sampler.schema.json](schema/animation.sampler.schema.json)

### sampler.input :white_check_mark: 

The id of a parameter in this animation to use as keyframe input.  This parameter must have type `FLOAT`.  The values represent time in seconds with `time[0] >= 0.0`, and monotonically increasing values, i.e. `time[n + 1] >= time[n]`.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**`: >= 1`

### sampler.interpolation

Interpolation algorithm.  When an animation targets a node's rotation and the animation's interpolation is `"LINEAR"`, spherical linear interpolation (slerp) should be used to interpolate quaternions.

* **Type**: `string`
* **Required**: No, default: `"LINEAR"`
* **Allowed values**: `"LINEAR"`

### sampler.output :white_check_mark: 

The id of a parameter in this animation to use as keyframe output.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**`: >= 1`

### sampler.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### sampler.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-asset"></a>
## asset

Metadata about the glTF asset.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**copyright**|`string`|A copyright message suitable for display to credit the content creator.|No|
|**generator**|`string`|Tool that generated this glTF model.  Useful for debugging.|No|
|**premultipliedAlpha**|`boolean`|Specifies if the shaders were generated with premultiplied alpha.|No, default: `false`|
|**profile**|[`asset.profile`](#reference-asset.profile)|Specifies the target rendering API and version, e.g., WebGL 1.0.3.|No, default: `{}`|
|**version**|`string`|The glTF version.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [asset.schema.json](schema/asset.schema.json)
* **Example**: [asset.json](schema/examples/asset.json)

### asset.copyright

A copyright message suitable for display to credit the content creator.

* **Type**: `string`
* **Required**: No

### asset.generator

Tool that generated this glTF model.  Useful for debugging.

* **Type**: `string`
* **Required**: No

### asset.premultipliedAlpha

Specifies if the shaders were generated with premultiplied alpha.

* **Type**: `boolean`
* **Required**: No, default: `false`
* **Related WebGL functions**: `getContext()` with premultipliedAlpha

### asset.profile

Specifies the target rendering API and version, e.g., WebGL 1.0.3.

* **Type**: [`asset.profile`](#reference-asset.profile)
* **Required**: No, default: `{}`

### asset.version :white_check_mark: 

The glTF version.

* **Type**: `string`
* **Required**: Yes

### asset.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### asset.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-asset.profile"></a>
## asset.profile

Specifies the target rendering API and version, e.g., WebGL 1.0.3.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**api**|`string`|Specifies the target rendering API.|No, default: `"WebGL"`|
|**version**|`string`|The API version.|No, default: `"1.0.3"`|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [asset.profile.schema.json](schema/asset.profile.schema.json)

### profile.api

Specifies the target rendering API.

* **Type**: `string`
* **Required**: No, default: `"WebGL"`

### profile.version

The API version.

* **Type**: `string`
* **Required**: No, default: `"1.0.3"`

### profile.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### profile.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-buffer"></a>
## buffer

A buffer points to binary geometry, animation, or skins.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**uri**|`string`|The uri of the buffer.| :white_check_mark: Yes|
|**byteLength**|`integer`|The length of the buffer in bytes.|No, default: `0`|
|**type**|`string`|XMLHttpRequest `responseType`.|No, default: `"arraybuffer"`|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [buffer.schema.json](schema/buffer.schema.json)
* **Example**: [buffers.json](schema/examples/buffers.json)

### buffer.uri :white_check_mark: 

The uri of the buffer.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.

* **Type**: `string`
* **Required**: Yes
* **Format**: uri

### buffer.byteLength

The length of the buffer in bytes.

* **Type**: `integer`
* **Required**: No, default: `0`
* **Minimum**:` >= 0`

### buffer.type

XMLHttpRequest `responseType`.

* **Type**: `string`
* **Required**: No, default: `"arraybuffer"`
* **Allowed values**: `"arraybuffer"`, `"text"`

### buffer.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### buffer.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### buffer.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-bufferView"></a>
## bufferView

A view into a [buffer](#reference-buffer) generally representing a subset of the buffer.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**buffer**|`string`|The id of the buffer.| :white_check_mark: Yes|
|**byteOffset**|`integer`|The offset into the buffer in bytes.| :white_check_mark: Yes|
|**byteLength**|`integer`|The length of the bufferView in bytes.|No, default: `0`|
|**target**|`integer`|The target that the WebGL buffer should be bound to.|No|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [bufferView.schema.json](schema/bufferView.schema.json)
* **Example**: [bufferViews.json](schema/examples/bufferViews.json)

### bufferView.buffer :white_check_mark: 

The id of the buffer.

* **Type**: `string`
* **Required**: Yes
* **Minimum Length**`: >= 1`

### bufferView.byteOffset :white_check_mark: 

The offset into the buffer in bytes.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**:` >= 0`

### bufferView.byteLength

The length of the bufferView in bytes.

* **Type**: `integer`
* **Required**: No, default: `0`
* **Minimum**:` >= 0`

### bufferView.target

The target that the WebGL buffer should be bound to.  Valid values correspond to WebGL enums: `34962` (ARRAY_BUFFER) and `34963` (ELEMENT_ARRAY_BUFFER).  When this is not provided, the bufferView contains animation or skin data.

* **Type**: `integer`
* **Required**: No
* **Allowed values**: `34962`, `34963`
* **Related WebGL functions**: `bindBuffer()`

### bufferView.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### bufferView.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### bufferView.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-camera"></a>
## camera

A camera's projection.  A node can reference a camera id to apply a transform to place the camera in the scene.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**orthographic**|[`camera.orthographic`](#reference-camera.orthographic)|An orthographic camera containing properties to create an orthographic projection matrix.|No|
|**perspective**|[`camera.perspective`](#reference-camera.perspective)|A perspective camera containing properties to create a perspective projection matrix.|No|
|**type**|`string`|Specifies if the camera uses a perspective or orthographic projection.| :white_check_mark: Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [camera.schema.json](schema/camera.schema.json)
* **Example**: [cameras.json](schema/examples/cameras.json)

### camera.orthographic

An orthographic camera containing properties to create an orthographic projection matrix.

* **Type**: [`camera.orthographic`](#reference-camera.orthographic)
* **Required**: No

### camera.perspective

A perspective camera containing properties to create a perspective projection matrix.

* **Type**: [`camera.perspective`](#reference-camera.perspective)
* **Required**: No

### camera.type :white_check_mark: 

Specifies if the camera uses a perspective or orthographic projection.  Based on this, either the camera's `perspective` or `orthographic` property will be defined.

* **Type**: `string`
* **Required**: Yes
* **Allowed values**: `"perspective"`, `"orthographic"`

### camera.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### camera.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### camera.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-camera.orthographic"></a>
## camera.orthographic

An orthographic camera containing properties to create an orthographic projection matrix.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**xmag**|`number`|The floating-point horizontal magnification of the view.| :white_check_mark: Yes|
|**ymag**|`number`|The floating-point vertical magnification of the view.| :white_check_mark: Yes|
|**zfar**|`number`|The floating-point distance to the far clipping plane.| :white_check_mark: Yes|
|**znear**|`number`|The floating-point distance to the near clipping plane.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [camera.orthographic.schema.json](schema/camera.orthographic.schema.json)

### orthographic.xmag :white_check_mark: 

The floating-point horizontal magnification of the view.

* **Type**: `number`
* **Required**: Yes

### orthographic.ymag :white_check_mark: 

The floating-point vertical magnification of the view.

* **Type**: `number`
* **Required**: Yes

### orthographic.zfar :white_check_mark: 

The floating-point distance to the far clipping plane.

* **Type**: `number`
* **Required**: Yes
* **Minimum**:` >= 0`

### orthographic.znear :white_check_mark: 

The floating-point distance to the near clipping plane.

* **Type**: `number`
* **Required**: Yes
* **Minimum**:` >= 0`

### orthographic.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### orthographic.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-camera.perspective"></a>
## camera.perspective

A perspective camera containing properties to create a perspective projection matrix.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**aspectRatio**|`number`|The floating-point aspect ratio of the field of view.|No|
|**yfov**|`number`|The floating-point vertical field of view in radians.| :white_check_mark: Yes|
|**zfar**|`number`|The floating-point distance to the far clipping plane.| :white_check_mark: Yes|
|**znear**|`number`|The floating-point distance to the near clipping plane.| :white_check_mark: Yes|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

**JSON schema**: [camera.perspective.schema.json](schema/camera.perspective.schema.json)

### perspective.aspectRatio

The floating-point aspect ratio of the field of view.  When this is undefined, the aspect ratio of the canvas is used.

* **Type**: `number`
* **Required**: No
* **Minimum**:` >= 0`

### perspective.yfov :white_check_mark: 

The floating-point vertical field of view in radians.

* **Type**: `number`
* **Required**: Yes
* **Minimum**:` >= 0`

### perspective.zfar :white_check_mark: 

The floating-point distance to the far clipping plane.  zfar must be greater than znear.

* **Type**: `number`
* **Required**: Yes
* **Minimum**:` > 0`

### perspective.znear :white_check_mark: 

The floating-point distance to the near clipping plane.  zfar must be greater than znear.

* **Type**: `number`
* **Required**: Yes
* **Minimum**:` > 0`

### perspective.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### perspective.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-glTF"></a>
## glTF

**JSON schema**: [glTF.schema.json](schema/glTF.schema.json)

TODO
TODO
TODO
TODO
TODO


<!-- ======================================================================= -->
<a name="reference-image"></a>
## image

Image data used to create a texture.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**uri**|`string`|The uri of the image.| :white_check_mark: Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

* **JSON schema**: [image.schema.json](schema/image.schema.json)
* **Example**: [images.json](schema/examples/images.json)

### image.uri :white_check_mark: 

The uri of the image.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.  The image format must be jpg, png, bmp, or gif.

* **Type**: `string`
* **Required**: Yes
* **Format**: uri

### image.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### image.extensions

Dictionary object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: `object`

### image.extras

Application-specific data.

* **Type**: `any`
* **Required**: No


<!-- ======================================================================= -->
<a name="reference-material"></a>
## material

* **JSON schema**: [material.schema.json](schema/material.schema.json)
* **Example**: [materials.json](schema/examples/materials.json)


<!-- ======================================================================= -->
<a name="reference-material.values"></a>
## material.values

**JSON schema**: [material.values.schema.json](schema/material.values.schema.json)


<!-- ======================================================================= -->
<a name="reference-mesh"></a>
## mesh

* **JSON schema**: [mesh.schema.json](schema/mesh.schema.json)
* **Example**: [meshes.json](schema/examples/meshes.json)


<!-- ======================================================================= -->
<a name="reference-mesh.primitive"></a>
## mesh.primitive

**JSON schema**: [mesh.primitive.schema.json](schema/mesh.primitive.schema.json)


<!-- ======================================================================= -->
<a name="reference-mesh.primitive.attribute"></a>
## mesh.primitive.attribute

**JSON schema**: [mesh.primitive.attribute.schema.json](schema/mesh.primitive.attribute.schema.json)


<!-- ======================================================================= -->
<a name="reference-node"></a>
## node

* **JSON schema**: [node.schema.json](schema/node.schema.json)
* **Example**: [nodes.json](schema/examples/nodes.json)


<!-- ======================================================================= -->
<a name="reference-program"></a>
## program

* **JSON schema**: [program.schema.json](schema/program.schema.json)
* **Example**: [programs.json](schema/examples/programs.json)


<!-- ======================================================================= -->
<a name="reference-sampler"></a>
## sampler

* **JSON schema**: [sampler.schema.json](schema/sampler.schema.json)
* **Example**: [samplers.json](schema/examples/samplers.json)


<!-- ======================================================================= -->
<a name="reference-scene"></a>
## scene

* **JSON schema**: [scene.schema.json](schema/scene.schema.json)
* **Example**: [scenes.json](schema/examples/scenes.json)


<!-- ======================================================================= -->
<a name="reference-shader"></a>
## shader

* **JSON schema**: [shader.schema.json](schema/shader.schema.json)
* **Example**: [shaders.json](schema/examples/shaders.json)


<!-- ======================================================================= -->
<a name="reference-skin"></a>
## skin

* **JSON schema**: [skin.schema.json](schema/skin.schema.json)
* **Example**: [skins.json](schema/examples/skins.json)


<!-- ======================================================================= -->
<a name="reference-technique"></a>
## technique

* **JSON schema**: [technique.schema.json](schema/technique.schema.json)
* **Example**: [techniques.json](schema/examples/techniques.json)


<!-- ======================================================================= -->
<a name="reference-technique.attribute"></a>
## technique.attribute

**JSON schema**: [technique.attribute.schema.json](schema/technique.attribute.schema.json)


<!-- ======================================================================= -->
<a name="reference-technique.parameters"></a>
## technique.parameters

**JSON schema**: [technique.parameters.schema.json](schema/technique.parameters.schema.json)


<!-- ======================================================================= -->
<a name="reference-technique.states"></a>
## technique.states

**JSON schema**: [technique.states.schema.json](schema/technique.states.schema.json)


<!-- ======================================================================= -->
<a name="reference-technique.uniform"></a>
## technique.uniform

**JSON schema**: [technique.uniform.schema.json](schema/technique.uniform.schema.json)


<!-- ======================================================================= -->
<a name="reference-texture"></a>
## texture

* **JSON schema**: [texture.schema.json](schema/texture.schema.json)
* **Example**: [textures.json](schema/examples/textures.json)


<<<<<<< HEAD
<a name="binarytypes"></a>
# Binary Types Reference

This section will describe the format for each of the GL types stored in the binary file. A reference to the GL spec for each type might be enough

<mark>*Todo: Patrick please advise on how to tackle this*</mark>
=======
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
   * <a href="schema/material.schema.json">`material`</a>
   * <a href="schema/materialInstanceTechnique.schema.json">`material/instanceTechnique`</a>
   * <a href="schema/materialInstanceTechniqueValues.schema.json">`material/instanceTechnique/values`</a>
   * <a href="schema/mesh.schema.json">`mesh`</a>
   * <a href="schema/meshPrimitive.schema.json">`mesh/primitive`</a>
   * <a href="schema/meshPrimitiveAttribute.schema.json">`mesh/primitive/attribute`</a>
   * <a href="schema/node.schema.json">`node`</a>
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
>>>>>>> origin/spec-1.0

<a name="conformance"></a>
# Conformance

<mark>*Todo: how do we tackle this?*</mark>

<a name="acknowledgements"></a>
# Acknowledgments

* Brandon Jones, Google
* Tom Fili, Cesium
* Scott Hunter, Analytical Graphics, Inc.
* Ed Mackey, Analytical Graphics, Inc.

<a name="references"></a>
# References

* [WebGL 1.0.2 spec](https://www.khronos.org/registry/webgl/specs/1.0/)
* [COLLADA 1.5 spec](http://www.khronos.org/files/collada_spec_1_5.pdf)
