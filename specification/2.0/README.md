<p align="center">
<img src="../figures/gltf.png" />
</p>

*Version 2.0*

The GL Transmission Format (glTF) is an API-neutral runtime asset delivery format.  glTF bridges the gap between 3D content creation tools and modern GL applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.

Last Updated: February 27, 2017

Editors

* Patrick Cozzi, Cesium
* Tony Parisi, WEVR

Contributors

* Fabrice Robinet, Individual Contributor (Previous Editor and Incubator)
* Remi Arnaud, Starbreeze Studios
* Uli Klumpp, Individual Contributor
* Neil Trevett, NVIDIA
* Alexey Knyazev, Individual Contributor

Copyright (C) 2013-2017 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.

# Contents

* [Introduction](#introduction)
  * [Motivation](#motivation)
  * [glTF Basics](#gltf-basics)
  * [Design Goals](#design-goals)
  * [File Extensions and MIME Types](#file-extensions-and-mime-types)
  * [JSON Encoding](#json-encoding)
  * [URIs](#uris)
* [Concepts](#concepts)
  * [Asset](#asset)
  * [Indices and Names](#indices-and-names)
  * [Coordinate System and Units](#coordinate-system-and-units)
  * [Scenes](#scenes)
    * [Nodes and Hierarchy](#nodes-and-hierarchy)
    * [Transformations](#transformations)
  * [Binary Data Storage](#binary-data-storage)
    * [Buffers and Buffer Views](#buffers-and-buffer-views)
      * [GLB-stored Buffer](#glb-stored-buffer)
    * [Accessors](#accessors)
        * [Accessor Element Size](#accessor-element-size)
        * [Sparse Accessors](#sparse-accessors)
    * [Data Alignment](#data-alignment)   
  * [Geometry](#geometry)
    * [Meshes](#meshes)
      * [Tangent-space definition](#tangent-space-definition)
      * [Morph Targets](#morph-targets)
    * [Skins](#skins)
      * [Skinned Mesh Attributes](#skinned-mesh-attributes)
      * [Joint Hierarchy](#joint-hierarchy)
    * [Instantiation](#instantiation)
  * [Texture Data](#texture-data)
    * [Textures](#textures)
    * [Images](#images)
    * [Samplers](#samplers)
  * [Materials](#materials)
    * [Metallic-Roughness Material](#metallic-roughness-material)
    * [Additional Maps](#additional-maps)
    * [Alpha Coverage](#alpha-coverage)
  * [Cameras](#cameras)
    * [Projection Matrices](#projection-matrices)
  * [Animations](#animations)
  * [Specifying Extensions](#specifying-extensions)
* [Properties Reference](#properties-reference)
* [Acknowledgments](#acknowledgments)
* [Appendix A: GLB File Format Specification](#appendix-a-glb-file-format-specification)
* [Appendix B: Tangent Space Recalculation](#appendix-b-tangent-space-recalculation)
* [Appendix C: BRDF Implementation](#appendix-c-brdf-implementation)
* [Appendix D: Full Khronos Trademark Statement](#appendix-d-full-khronos-trademark-statement)

# Introduction

The GL Transmission Format (glTF) is an API-neutral runtime asset delivery format.  glTF bridges the gap between 3D content creation tools and modern graphics applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.

## Motivation

*This section is non-normative.*

Traditional 3D modeling formats have been designed to store data for offline use, primarily to support authoring workflows on desktop systems. Industry-standard 3D interchange formats allow for sharing assets between different modeling tools, and within the content pipeline in general. However, neither of these types of formats is optimized for download speed or fast loading at runtime. Files tend to grow very large, and applications need to do a significant amount of processing to load such assets into GPU-accelerated applications.

Applications seeking high performance rarely load modeling formats directly; instead, they process models offline as part of a custom content pipeline, converting the assets into a proprietary format optimized for their runtime application.  This has led to a fragmented market of incompatible proprietary runtime formats and duplicated efforts in the content creation pipeline. 3D assets exported for one application cannot be reused in another application without going back to the original modeling, tool-specific source and performing another proprietary export step.

With the advent of mobile- and web-based 3D computing, new classes of applications have emerged that require fast, dynamic loading of standardized 3D assets. Digital marketing solutions, e-commerce product visualizations, and online model-sharing sites are just a few of the connected 3D applications being built today using WebGL or OpenGL ES. Beyond the need for efficient delivery, many of these online applications can benefit from a standard, interoperable format to enable sharing and reuse of assets between users, between applications, and within heterogeneous, distributed content pipelines.

glTF solves these problems by providing a vendor- and runtime-neutral format that can be loaded and rendered with minimal processing. The format combines an easily parseable JSON scene description with one or more binary files representing geometry, animations, and other rich data. Binary data is stored in such a way that it can be loaded directly into GPU buffers without additional parsing or other manipulation. Using this approach, glTF is able to faithfully preserve full hierarchical scenes with nodes, meshes, cameras, materials, and animations, while enabling efficient delivery and fast loading.

## glTF Basics

*This section is non-normative.*

glTF assets are JSON files plus supporting external data. Specifically, a glTF asset is represented by:

* A JSON-formatted file (`.gltf`) containing a full scene description: node hierarchy, materials, cameras, as well as descriptor information for meshes, animations, and other constructs
* Binary files (`.bin`) containing geometry and animation data, and other buffer-based data
* Image files (`.jpg`, `.png`, etc.) for textures

Assets defined in other formats, such as images, may be stored in external files referenced via URI, stored side-by-side in GLB container, or embedded directly into the JSON using [data URIs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URIs).

Valid glTF asset must specify its version.

<p align="center">
<img src="figures/files.png" width="50%" />
</p>

## Design Goals

*This section is non-normative.*

glTF has been designed to meet the following goals:

* *Compact file sizes.* While web developers like to work with clear text as much as possible, clear text encoding is simply not practical for transmitting 3D data due to sheer size. The glTF JSON file itself is clear text, but it is compact and rapid to parse. All large data such as geometry and animations are stored in binary files that are much smaller than equivalent text representations.
* *Fast loading.* glTF data structures have been designed to mirror the GPU API data as closely as possible, both in the JSON and binary files, to reduce load times. For example, binary data for meshes could be viewed as JavaScript Typed Arrays and be loaded directly into GPU buffers with a simple data copy; no parsing or further processing is required.
* *Runtime-independence.* glTF makes no assumptions about the target application or 3D engine. glTF specifies no runtime behaviors other than rendering and animation.
* *Complete 3D scene representation.* Exporting single objects from a modeling package is not sufficient for many applications. Often, authors want to load entire scenes, including nodes, transformations, transform hierarchy, meshes, materials, cameras, and animations into their applications. glTF strives to preserve all of this information for use in the downstream application.
* *Extensibility.* While the initial base specification supports a rich feature set, there will be many opportunities for growth and improvement. glTF defines a mechanism that allows the addition of both general-purpose and vendor-specific extensions.

The design of glTF takes a pragmatic approach. The format is meant to mirror the GPU APIs as closely as possible, but if it did only that, there would be no cameras, animations, or other features typically found in both modeling tools and runtime systems, and much semantic information would be lost in the translation. By supporting these common constructs, glTF content can not only load and render, but it can be immediately usable in a wider range of applications and require less duplication of effort in the content pipeline.

The following are outside the scope of the initial design of glTF:

* *glTF is not a streaming format.* The binary data in glTF is inherently streamable, and the buffer design allows for fetching data incrementally. But there are no other streaming constructs in the format, and no conformance requirements for an implementation to stream data versus downloading it in its entirety before rendering.
* *glTF is not intended to be human-readable,* though by virtue of being represented in JSON, it is developer-friendly.

Version 2.0 of glTF does not define compression for geometry and other rich data. However, the design team believes that compression is a very important part of a transmission standard, and there is already work underway to define compression extensions.

> The 3D Formats Working Group is developing partnerships to define the codec options for geometry compression.  glTF defines the node hierarchy, materials, animations, and geometry, and will reference the external compression specs.

## File Extensions and MIME Types

* `*.gltf` files use `model/gltf+json`
* `*.bin` files use `application/octet-stream`
* Texture files use the official `image/*` type based on the specific image format. For compatibility with modern web browsers, the following image formats are supported: `image/jpeg`, `image/png`.

## JSON encoding

To simplify client-side implementation, glTF has following restrictions on JSON format and encoding.

1. JSON must use UTF-8 encoding without BOM.
2. All strings defined in this spec (properties names, enums) use only ASCII charset and must be written as plain text, e.g., `"buffer"` instead of `"\u0062\u0075\u0066\u0066\u0065\u0072"`.

   > **Implementation Note:** This allows generic glTF loader to not have full Unicode support. Application-specific strings (e.g., value of `"name"` property) could use any charset.
3. Names (keys) within JSON objects must be unique, i.e., duplicate keys aren't allowed.

## URIs

glTF uses URIs to reference buffers and image resources. These URIs may point to external resources or be data URIs that embed resources in the JSON. Embedded resources use "data" URI scheme ([RFC-2397](https://tools.ietf.org/html/rfc2397)) so they can easily be [decoded with JavaScript](https://developer.mozilla.org/en-US/docs/Web/API/WindowBase64/Base64_encoding_and_decoding).

Clients are required to support only embedded resources and relative external references (in a sense of [RFC3986](https://tools.ietf.org/html/rfc3986#section-4.2)). Clients are free to support other schemes (such as `http://`) depending on expected usage.

This allows the application to decide the best approach for delivery: if different assets share many of the same geometries, animations, or textures, separate files may be preferred to reduce the total amount of data requested. With separate files, applications can progressively load data and do not need to load data for parts of a model that are not visible. If an application cares more about single-file deployment, embedding data may be preferred even though it increases the overall size due to base64 encoding and does not support progressive or on-demand loading.

# Concepts

<p align="center">
<img src="figures/dictionary-objects.png" /><br/>
The top-level arrays in a glTF asset.  See the <a href="#properties">Properties Reference</a>.
</p>

## Asset

Each glTF asset must have an `asset` property. In fact, it's the only required top-level property for JSON to be a valid glTF. The `asset` object must contain glTF version and optionally other metadata such as `generator` or ``copyright`. For example,

```json
{
    "asset": {
        "version": "2.0",
        "generator": "collada2gltf@f356b99aef8868f74877c7ca545f2cd206b9d3b7",
        "copyright": "2017 (c) Khronos Group"
    }
}
```

## Indices and Names

Entities of a glTF asset are referenced by their indices in corresponding arrays, e.g., a `bufferView` refers to a `buffer` by specifying the buffer's index in `buffers` array.  For example:

```json
{
    "buffers": [
        {
            "byteLength": 1024,
            "uri": "path-to.bin"
        }
    ],
    "bufferViews": [
        {
            "buffer": 0,
            "byteLength": 512,
            "byteOffset": 0
        }
    ]
}
```

In this example, `buffers` and `bufferViews` have only one element each. The bufferView refers to the buffer using the buffer's index: `"buffer": 0`.

Whereas indices are used for internal glTF references, _names_ are used for application-specific uses such as display. Any top-level glTF object can have a `name` string property for this purpose. These property values are not guaranteed to be unique as they are intended to contain values created when the asset was authored.

For property names, glTF uses [camel case](http://en.wikipedia.org/wiki/CamelCase) `likeThis`. Camel case is a common naming convention in JSON and WebGL.

## Coordinate System and Units

glTF uses a right-handed coordinate system, that is, the cross product of X and Y yields Z. glTF defines the y axis as up.

The units for all linear distances are meters.

All angles are in radians.

Positive rotation is counterclockwise.


## Scenes

The glTF asset contains zero or more *scenes*, the set of visual objects to render. Scenes are defined in a `scenes` array. An additional property, `scene` (note singular), identifies which of the scenes in the array is to be displayed at load time.

When `scene` is undefined, scene of index 0 should be displayed. When `scenes` array is undefined, nothing is rendered at load time. 

The following example defines a glTF asset with a single scene, that contains a single node.

```json
{
    "nodes": [
        {
            "name": "singleNode"
        }
    ],
    "scenes": [
        {
            "name": "singleScene",
            "nodes": [
                0
            ]
        }
    ],
    "scene": 0
}
```

### Nodes and Hierarchy

The glTF asset can define *nodes*, that is, the objects comprising the scene to render.

Nodes have an optional `name` property.

Nodes also have transform properties, as described in the next section.

Nodes are organized in a parent-child hierarchy known informally as the *node hierarchy*.

The node hierarchy is defined using a node's `children` property, as in the following example:

```json
{
    "nodes": [
        {
            "name": "Car",
            "children": [1, 2, 3, 4]
        },
        {
            "name": "wheel_1"
        },
        {
            "name": "wheel_2"
        },
        {
            "name": "wheel_3"
        },
        {
            "name": "wheel_4"
        }        
    ]
}
```

The node named `Car` has four children. Each of those nodes could in turn have its own children, creating a hierarchy of nodes.

>For Version 2.0 conformance, the glTF node hierarchy is not a directed acyclic graph (DAG) or *scene graph*, but a strict tree. That is, no node may be a direct or indirect descendant of more than one node. This restriction is meant to simplify implementation and facilitate conformance. The restriction may be lifted later.

### Transformations

Any node can define a local space transformation either by supplying a `matrix` property, or any of `translation`, `rotation`, and `scale`  properties (also known as *TRS properties*). `translation` and `scale` are `FLOAT_VEC3` values in the local coordinate system. `rotation` is a `FLOAT_VEC4` unit quaternion value, `(x, y, z, w)`, in the local coordinate system.

TRS properties are converted to matrices and postmultiplied in the `T * R * S` order to compose the transformation matrix; first the scale is applied to the vertices, then the rotation, and then the translation.

When a node is targeted for animation (referenced by an `animation.channel.target`), only TRS properties may be present; `matrix` will not be present. 

In the example below, node named `Box` defines non-default rotation and translation.

```json
{
    "nodes": [
        {
            "name": "Box",
            "rotation": [
                0,
                0,
                0,
                1
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
        }
    ]
}
```

The next example defines the transformation for a node with attached camera using the `matrix` property rather than using the individual TRS values:

```json
{
    "nodes": [
        {
            "name": "node-camera",
            "camera": 1,
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
            ]
        }
    ]
}
```

## Binary Data Storage

### Buffers and Buffer Views

A *buffer* is data stored as a binary blob. The buffer can contain a combination of geometry, animation, and skins.

Binary blobs allow efficient creation of GPU buffers and textures since they require no additional parsing, except perhaps decompression. An asset can have any number of buffer files for flexibility for a wide array of applications.

Buffer data is little endian.

All buffers are stored in the asset's `buffers` array.

The following example defines a buffer. The `byteLength` property specifies the size of the buffer file. The `uri` property is the URI to the buffer data. Buffer data may also be stored within the glTF file as base64-encoded data and reference via data URI.

```json
{
   "buffers": [
       {
           "byteLength": 102040,
           "uri": "duck.bin"
       }
   ]
}
```

A *bufferView* represents a subset of data in a buffer, defined by an integer offset into the buffer specified in the `byteOffset` property, a `byteLength` property to specify length of the buffer view.
When buffer view has a `target` property, its data could be directly fed into appropriate GPU buffer: either ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER.

The following example defines two buffer views: the first is an ELEMENT_ARRAY_BUFFER, which holds the indices for an indexed triangle set, and the second is an ARRAY_BUFFER that holds the vertex data for the triangle set.

```json
{
    "bufferViews": [
        {
            "buffer": 0,
            "byteLength": 25272,
            "byteOffset": 0,
            "target": 34963
        },
        {
            "buffer": 0,
            "byteLength": 76768,
            "byteOffset": 25272,
            "byteStride": 32,
            "target": 34962
        }
    ]
}
```

Buffer view could have `byteStride` property. It means byte-distance between consequential elements and it is mandatory when `target` equals to ARRAY_BUFFER. When `target` is ELEMENT_ARRAY_BUFFER, `byteStride` must be 0.

Buffers and buffer views do not contain type information. They simply define the raw data for retrieval from the file. Objects within the glTF file (meshes, skins, animations) access buffers or buffer views via *accessors*.

#### GLB-stored Buffer

glTF asset could use GLB file container to pack all resources into one file. glTF Buffer referring to GLB-stored `BIN` chunk, must have `buffer.uri` property undefined, and it must be the first element of `buffers` array. In the following example, the first buffer objects refers to GLB-stored data, while the second points to external resource:

```json
{
    "buffers": [
        {
            "byteLength": 35884
        },
        {
            "byteLength": 504,
            "uri": "external.bin"
        }
  ]
}
```

See [Appendix A](#appendix-a-glb-file-format-specification) for details on GLB File Format.

### Accessors

All large data for meshes, skins, and animations is stored in buffers and retrieved via accessors.

An *accessor* defines a method for retrieving data as typed arrays from within a `bufferView`. The accessor specifies a component type (e.g. `5126 (GL_FLOAT)`) and a data type (e.g. `VEC3`), which when combined define the complete data type for each array element. The accessor also specifies the location and size of the data within the `bufferView` using the properties `byteOffset` and `count`. The latter specifies the number of elements within the `bufferView`, *not* the number of bytes. Elements could be, e.g., vertex indices, vertex attributes, animation keyframes, etc.

All accessors are stored in the asset's `accessors` array.

The following fragment shows two accessors, the first is a scalar accessor for retrieving a primitive's indices, and the second is a 3-float-component vector accessor for retrieving the primitive's position data.

```json
{
    "accessors": [
        {
            "bufferView": 0,
            "byteOffset": 0,
            "componentType": 5123,
            "count": 12636,
            "max": [
                4212
            ],
            "min": [
                0
            ],
            "type": "SCALAR"
        },
        {
            "bufferView": 1,
            "byteOffset": 0,
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
        }
    ]
}
```

#### Accessor Element Size

The following tables can be used to compute the size of element accessible by accessor.

| `componentType` | Size in bytes |
|:-:|:-:|
| `5120` (BYTE) | 1 |
| `5121`(UNSIGNED_BYTE) | 1 |
| `5122` (SHORT) | 2 |
| `5123` (UNSIGNED_SHORT) | 2 |
| `5125` (UNSIGNED_INT) | 4 |
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

Element size, in bytes, is
`(size in bytes of the 'componentType') * (number of components defined by 'type')`.

For example:

```json
{
    "accessors": [
        {
            "bufferView": 1,
            "byteOffset": 7032,
            "componentType": 5126,
            "count": 586,
            "type": "VEC3"
        }
    ]
}
```

In this accessor, the `componentType` is `5126` (FLOAT), so each component is four bytes.  The `type` is `"VEC3"`, so there are three components.  The size of each element is 12 bytes (`4 * 3`).

#### Sparse Accessors

Sparse encoding of arrays is often more memory-efficient than dense encoding when describing incremental changes with respect to a reference array.
This is often the case when encoding morph targets (it is, in general, more efficient to describe a few displaced vertices in a morph target than transmitting all morph target vertices).

glTF 2.0 extends the accessor structure to enable efficient transfer of sparse arrays.
Similarly to a standard accessor, a sparse accessor initializes an array of typed elements from data stored in a `bufferView` . On top of that, a sparse accessor includes a `sparse` dictionary describing the elements that deviate from their initialization value. The `sparse` dictionary contains the following mandatory properties:
- `count`: number of displaced elements.
- `indices`: strictly increasing array of integers of size `count` and specific `componentType` that stores the indices of those elements that deviate from the initialization value.
- `values`: array of displaced elements corresponding to the indices in the `indices` array.

The following fragment shows an example of `sparse` accessor with 10 elements deviating from the initialization array.

```json
{
    "accessors": [
        {
            "bufferView": 0,
            "byteOffset": 0,
            "componentType": 5123,
            "count": 12636,
            "type": "VEC3",
            "sparse": {
                "count": 10,
                "indices": {
                    "bufferView": 1,
                    "byteOffset": 0,
                    "componentType": 5123
                },
                "values": {
                    "bufferView": 2,
                    "byteOffset": 0
                }
            }
        }
    ]
}
```
A sparse accessor differs from a regular one in that `bufferView` property isn't required. When it's omitted, the sparse accessor is initialized as an array of zeros of size `(size of the accessor element) * (accessor.count)` bytes.
A sparse accessor `min` and `max` properties correspond, respectively, to the minimum and maximum component values once the sparse substitution is applied.

#### Data Alignment

The offset of an `accessor` into a `bufferView` (i.e., `accessor.byteOffset`) and the offset of an `accessor` into a `buffer` (i.e., `accessor.byteOffset + bufferView.byteOffset`) must be a multiple of the size of the accessor's component type.

When `byteStride` of referenced `bufferView` equals `0` (or not defined), it means that accessor elements are tightly packed, i.e., actual stride equals size of the element. When `bufferView.byteStride` is defined, it must be a multiple of the size of the accessor's component type.

Each `accessor` must fit its `bufferView`, i.e., `accessor.byteOffset + STRIDE * (accessor.count - 1) + SIZE_OF_ELEMENT` must be less than or equal to `bufferView.length`.

For performance and compatibility reasons, vertex attributes must be aligned to 4-byte boundaries inside `bufferView` (i.e., `accessor.byteOffset` and `bufferView.byteStride` must be multiples of 4). 

> **Implementation Note:** For JavaScript, this allows a runtime to efficiently create a single ArrayBuffer from a glTF `buffer` or an ArrayBuffer per `bufferView`, and then use an `accessor` to turn a typed array view (e.g., `Float32Array`) into an ArrayBuffer without copying it because the byte offset of the typed array view is a multiple of the size of the type (e.g., `4` for `Float32Array`).

Consider the following example:

```json
{
    "bufferViews": [
        {
            "buffer": 0,
            "byteLength": 17136,
            "byteOffset": 620,
            "target": 34963
        }
    ],
    "accessors": [
        {
            "bufferView": 0,
            "byteOffset": 4608,
            "componentType": 5123,
            "count": 5232,
            "type": "SCALAR"
        }
    ]
}
```
Accessing binary data defined by example above could be done like this:

```js
var typedView = new Uint16Array(buffer, accessor.byteOffset + accessor.bufferView.byteOffset, accessor.count);
```

The size of the accessor component type is two bytes (the `componentType` is unsigned short). The accessor's `byteOffset` is also divisible by two. Likewise, the accessor's offset into buffer `0` is `5228 ` (`620 + 4608`), which is divisible by two.


## Geometry

Any node can contain one mesh, defined in its `mesh` property. Mesh can be skinned using a information provided in referenced `skin` object. Mesh can have morph targets.

### Meshes

In glTF, meshes are defined as arrays of *primitives*. Primitives correspond to the data required for GPU draw calls. Primitives specify one or more `attributes`, corresponding to the vertex attributes used in the draw calls. Indexed primitives also define an `indices` property. Attributes and indices are defined as references to accessors containing corresponding data. Each primitive also specifies a material and a primitive type that corresponds to the GPU primitive type (e.g., triangle set).

> **Implementation note:** Splitting one mesh into *primitives* could be useful to limit number of indices per draw call.

If `material` is not supplied, and no extension is present that defines material properties, then the object should be rendered using a 50% gray emissive color.  

The following example defines a mesh containing one triangle set primitive:

```json
{
    "meshes": [
        {
            "primitives": [
                {
                    "attributes": {
                        "NORMAL": 25,
                        "POSITION": 23,
                        "TEXCOORD_0": 27
                    },
                    "indices": 21,
                    "material": 3,
                    "mode": 4
                }
            ]
        }
    ]
}
```

Each attribute is defined as a property of the `attributes` object. The name of the property corresponds to an enumerated value identifying the vertex attribute, such as `POSITION`. The value of the property is the index of an accessor that contains the data.

Valid attribute semantic property names include `POSITION`, `NORMAL`, `TEXCOORD`, `COLOR`, `JOINT`, and `WEIGHT`.  `TEXCOORD` and `COLOR` attribute semantic property names must be of the form `[semantic]_[set_index]`, e.g., `TEXCOORD_0`, `TEXCOORD_1`, `COLOR_1`, etc.  For forward-compatibility, application-specific semantics must start with an underscore, e.g., `_TEMPERATURE`.

> **Implementation note:** Each primitive corresponds to one WebGL draw call (engines are, of course, free to batch draw calls). When a primitive's `indices` property is defined, it references the accessor to use for index data, and GL's `drawElements` function should be used. When the `indices` property is not defined, GL's `drawArrays` function should be used with a count equal to the count property of any of the accessors referenced by the `attributes` property (they are all equal for a given primitive).

#### Tangent-space definition

**TODO**

#### Morph Targets

Morph Targets are defined by extending the Mesh concept.
A Morph Target is a morphable Mesh where primitives' attributes are obtained by adding the original attributes to a weighted sum of targets attributes.
For instance, the Morph Target vertices `POSITION` for the primitive at index *i* are computed in this way:
```
primitives[i].attributes.POSITION + 
  weights[0] * primitives[i].targets[0].POSITION +
  weights[1] * primitives[i].targets[1].POSITION +
  weights[2] * primitives[i].targets[2].POSITION + ...
```
Morph Targets are implemented via the `targets` property defined in the Mesh `primitives`. Each target in the `targets` array is a dictionary mapping a primitive attribute to an accessor containing Morph Target displacement data, currently only two attributes ('POSITION' and 'NORMAL') are supported. All primitives are required to list the same number of `targets` in the same order.
The `weights` array is optional, it stores the default targets weights, in the absence of `node.weights` the primitives attributes are resolved using these weights. When this property is missing the default targets weights are assumed to be zero.

The following example extends the Mesh defined in the previous example to a morphable one by adding two Morph Targets:
```json
{
    "primitives": [
        {
            "attributes": {
                "NORMAL": 25,
                "POSITION": 23,
                "TEXCOORD_0": 27
            },
            "indices": 21,
            "material": 3,
            "mode": 4,
            "targets": [
                {
                    "NORMAL": 35,
                    "POSITION": 33
                },
                {
                    "NORMAL": 45,
                    "POSITION": 43
                }
            ]
        }
    ],
    "weights": [0, 0.5]
}
```

After applying morph targets to vertex positions and normals, tangent space must be recalculated. See [Appendix B](#appendix-b-tangent-space-recalculation) for details.

### Skins

All skins are stored in the `skins` array of the asset. Each skin is defined by the `inverseBindMatrices` property (which points to an accessor with IBM data), used to bring coordinates being skinned into the same space as each joint; and a `joints` array property that lists the nodes indices used as joints to animate the skin. The order of joints is defined in the `skin.joints` array and it must match the order of `inverseBindMatrices` data. The `skeleton` property points to node that is the root of a joints hierarchy. 

> **Implementation Note:** Matrix, defining how to pose the skin's geometry for use with the joints ("Bind Shape Matrix") should be premultiplied to mesh data or to Inverse Bind Matrices. 

```json
{    
    "skins": [
        {
            "inverseBindMatrices": 11,
            "joints": [
                1,
                2
            ],
            "skeleton": 1
        }
    ]
}
```

#### Skinned Mesh Attributes

The mesh for a skin is defined with vertex attributes that are used in skinning calculations in the vertex shader. The `JOINT` attribute data contains the indices of the joints from corresponding `joints` array that should affect the vertex. The `WEIGHT` attribute data defines the weights indicating how strongly the joint should influence the vertex. The following mesh skin defines `JOINT` and `WEIGHT` vertex attributes for a triangle mesh primitive:

```json
{
    "meshes": [
        {
            "name": "skinned-mesh_1",
            "primitives": [
                {
                    "attributes": {
                        "JOINT": 179,
                        "NORMAL": 165,
                        "POSITION": 163,
                        "TEXCOORD_0": 167,
                        "WEIGHT": 176
                    },
                    "indices": 161,
                    "material": 1,
                    "mode": 4
                }
            ]
        }
    ]
}
```

The number of joints that influence one vertex is limited to 4, so referenced accessors must have `VEC4` type and following component formats:

* **`JOINT`**: `UNSIGNED_BYTE` or `UNSIGNED_SHORT`
* **`WEIGHT`**: `FLOAT`, or normalized `UNSIGNED_BYTE`, or normalized `UNSIGNED_SHORT`

#### Joint Hierarchy

The joint hierarchy used for controlling skinned mesh pose is simply the glTF node hierarchy, with each node designated as a joint. The following example defines a joint hierarchy of two joints.

**TODO: object-space VS world-space joints**

For more details of vertex skinning, refer to [glTF Overview](figures/gltfOverview-0.2.0.png).

### Instantiation

A mesh is instantiated by `node.mesh` property. The same mesh could be used by many nodes, which could have different transformations. For example:

```json
{
    "nodes": [
        {
            "mesh": 11
        },
        {
            "mesh": 11,
            "translation": [
                -20,
                -1,
                0
            ]            
        }
    ]
}

```

A Morph Target is instanced within a node using:
- The Morph Target referenced in the `mesh` property.
- The Morph Target `weights` overriding the `weights` of the Morph Target referenced in the `mesh` property.
The example below instatiates a Morph Target with non-default weights.

```json
{
    "nodes": [
        {
            "mesh": 11,
            "weights": [0, 0.5]
        }
    ]
}
```

A skin is instanced within a node using a combination of the node's `mesh` and `skin` properties. The mesh for a skin instance is defined in the `mesh` property. The `skin` property contains the index of the skin to instance.

```json
{
    "skins": [
        {
            "inverseBindMatrices": 29,
            "joints": [1, 2] 
        }
    ],
    "nodes": [
        {
            "name":"Skinned mesh node",
            "mesh": 0,
            "skin": 0
        },
        {
            "name":"Skeleton root joint",
            "children": [2],
            "rotation": [
                0,
                0,
                0.7071067811865475,
                0.7071067811865476
            ],
            "translation": [
                4.61599,
                -2.032e-06,
                -5.08e-08
            ]
        },
        {
            "name":"Head",
            "translation": [
                8.76635,
                0,
                0
            ]
        }
    ]
}
```

## Texture Data

**TODO: describe separation of concerns: textures / images / samplers**

### Textures

**TODO: update to latest revisions**

All textures are stored in the asset's `textures` array. A texture is defined by an image file, denoted by the `source` property; `format` and `internalFormat` specifiers, corresponding to the GL texture format types; a `target` type for the sampler; a sampler identifier (`sampler`), and a `type` property defining the internal data format. Refer to the GL definition of `texImage2D()` for more details.

```json
{
    "textures": [
        {
            "format": 6408,
            "internalFormat": 6408,
            "sampler": 0,
            "source": 2,
            "target": 3553,
            "type": 5121
        }
    ]
}
```

### Images

**TODO: update to latest revisions**

Images referred to by textures are stored in the `images` array of the asset. Each image contains a URI to an external file (or a reference to a bufferView) in one of the supported images formats. Image data may also be stored within the glTF file as base64-encoded data and referenced via data URI. For example:

```json
{
    "images": [
        {
            "uri": "duckCM.png"
        },
        {
            "bufferView": 14,
            "mimeType": "image/jpeg" 
        }
    ]
}
```
> **Implementation Note**: With WebGL API, the first pixel transferred from the `TexImageSource` (i.e., HTML Image object) to the WebGL implementation corresponds to the upper left corner of the source. Non-WebGL runtimes may need to flip Y axis to achieve correct texture rendering.

### Samplers

**TODO: update to latest revisions**

Samplers are stored in the `samplers` array of the asset. Each sampler specifies filter and wrapping options corresponding to the GL types. The following example defines a sampler with linear mag filtering, linear mipmap min filtering, and repeat wrapping in S and T.


```json
{
    "samplers": [
        {
            "magFilter": 9729,
            "minFilter": 9987,
            "wrapS": 10497,
            "wrapT": 10497
        }
    ]
}
```

> **Mipmapping Implementation Note**: When a sampler's minification filter (`minFilter`) uses mipmapping (`NEAREST_MIPMAP_NEAREST`, `NEAREST_MIPMAP_LINEAR`, `LINEAR_MIPMAP_NEAREST`, or `LINEAR_MIPMAP_LINEAR`), any texture referencing the sampler needs to have mipmaps, e.g., by calling GL's `generateMipmap()` function.


> **Non-Power-Of-Two Texture Implementation Note**: glTF does not guarantee that a texture's dimensions are a power-of-two.  At runtime, if a texture's width or height is not a power-of-two, the texture needs to be resized so its dimensions are powers-of-two if the `sampler` the texture references
> * Has a wrapping mode (either `wrapS` or `wrapT`) equal to `REPEAT` or `MIRRORED_REPEAT`, or
> * Has a minification filter (`minFilter`) that uses mipmapping (`NEAREST_MIPMAP_NEAREST`, `NEAREST_MIPMAP_LINEAR`, `LINEAR_MIPMAP_NEAREST`, or `LINEAR_MIPMAP_LINEAR`).

## Materials

glTF defines materials using a common set of parameters that are based on widely used material representations from Physically-Based Rendering (PBR). Specifically, glTF uses the metallic-roughness material model. Using this declarative representation of materials enables a glTF file to be rendered consistently across platforms. 

### Metallic-Roughness Material 

All parameters related to the metallic-roughness material model are defined under the `pbrMetallicRoughness` property of `material` object. The following example shows how a material like gold can be defined using the metallic-roughness parameters: 

```json
{
    "materials": [
        {
            "name": "gold",
            "pbrMetallicRoughness": {
                "baseColorFactor": [ 1.000, 0.766, 0.336, 1.0 ],
                "metallicFactor": 1.0,
                "roughnessFactor": 0.0
            }
        }
    ]
}
```

The metallic-roughness material model is defined by the following properties:
* `baseColor` - The base color of the material
* `metallic` - The metalness of the material
* `roughness` - The roughness of the material

The base color has two different interpretations depending on the value of metalness. When the material is a metal, the base color is the specific measured reflectance value at normal incidence (F0). For a non-metal the base color represents the reflected diffuse color of the material. In this model it is not possible to specify a F0 value for non-metals, and a linear value of 4% (0.04) is used. 

The value for each property (`baseColor`, `metallic`, `roughness`) can be defined using factors or textures. The `metallic` and `roughness` properties are packed together in a single texture called `metallicRoughnessTexture`. If a texture is not given, all respective texture components within this material model are assumed to have a value of `1.0`. If both factors and textures are present the factor value acts as a linear multiplier for the corresponding texture values. Texture content must be converted to linear space before it is used for any lighting computations. 

For example, assume a value of `[0.9, 0.5, 0.3, 1.0]` in linear space is obtained from an RGBA `baseColorTexture`, and assume that `baseColorFactor` is given as `[0.2, 1.0, 0.7, 1.0]`.
Then, the result would be 
```
[0.9 * 0.2, 0.5 * 1.0, 0.3 * 0.7, 1.0 * 1.0] = [0.18, 0.5, 0.21, 1.0]
```

The following equations show how to calculate bidirectional reflectance distribution function (BRDF) inputs (*c<sub>diff</sub>*, *F<sub>0</sub>*, *&alpha;*) from the metallic-roughness material properties. 

`const dielectricSpecular = rgb(0.04, 0.04, 0.04)`
<br>
`const black = rgb(0, 0, 0)`

*c<sub>diff</sub>* = `lerp(baseColor.rgb * (1 - dielectricSpecular.r), black, metallic)`
<br>
*F<sub>0</sub>* = `lerp(dieletricSpecular, baseColor.rgb, metallic)`
<br>
*&alpha;* = `roughness ^ 2`

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix C](#appendix-c-brdf-implementation) for more details on the BRDF calculations.

### Additional Maps

The material definition also provides for additional maps that can also be used with the metallic-roughness material model as well as other material models which could be provided via glTF extensions.

Materials define the following additional maps:
- **normal** : A tangent space normal map.
- **occlusion** : The occlusion map indicating areas of indirect lighting.
- **emissive** : The emissive map controls the color and intensity of the light being emitted by the material.

The following examples shows a material that is defined using `pbrMetallicRoughness` parameters as well as additional texture maps:

```json
{
    "materials": [
        {
            "name": "Material0",
            "pbrMetallicRoughness": {
                "baseColorFactor": [ 0.5, 0.5, 0.5, 1.0 ],
                "baseColorTexture": {
                    "index": 1,
                    "texCoord": 1
                },
                "metallicFactor": 1,
                "roughnessFactor": 1,
                "metallicRoughnessTexture": {
                    "index": 2,
                    "texCoord": 1
                }
            },
            "normalTexture": {
                "scale": 2,
                "index": 3,
                "texCoord": 1
            },
            "emissiveFactor": [ 0.2, 0.1, 0.0 ]
        }
    ]
}
```

>**Implementation Note:** If an implementation is resource-bound and cannot support all the maps defined it should support these additional maps in the following priority order.  Resource-bound implementations should drop maps from the bottom to the top.
>
>| Map       | Rendering impact when map is not supported |
>|-----------|--------------------------------------------|
>| Normal    | Geometry will appear less detailed than authored. |
>| Occlusion | Model will appear brighter in areas that should be darker. |
>| Emissive  | Model with lights will not be lit. For example, the headlights of a car model will be off instead of on. |

### Alpha Coverage

The `alphaMode` property defines how the alpha value of the main factor and texture should be interpreted. The alpha value is defined in the `baseColor` for metallic-roughness material model. 

`alphaMode` can be one of the following values:
* `OPAQUE` - The rendered output is fully opaque and any alpha value is ignored.
* `MASK` - The rendered output is either fully opaque or fully transparent depending on the alpha value and the specified alpha cutoff value. This mode is used to simulate geometry such as tree leaves or wire fences.
* `BLEND` - The rendered output is combined with the background using the normal painting operation (i.e. the Porter and Duff over operator). This mode is used to simulate geometry such as guaze cloth or animal fur. 

 When `alphaMode` is set to `MASK` the `alphaCutoff` property specifies the cutoff threshold. If the alpha value is greater than or equal to the `alphaCutoff` value then it is rendered as fully opaque, otherwise, it is rendered as fully transparent. `alphaCutoff` value is ignored for other modes.

The `doubleSided` property specified whether the material is double sided.

>**Implementation Note for Real-Time Rasterizers:** Real-time rasterizers typically use depth buffers and mesh sorting to support alpha modes. The following describe the expected behavior for these types of renderers.
>* `OPAQUE` - A depth value is written for every pixel and mesh sorting is not required for correct output.
>* `MASK` - A depth value is not written for a pixel that is discarded after the alpha test. A depth value is written for all other pixels. Mesh sorting is not required for correct output.
>* `BLEND` - Support for this mode varies. There is no perfect and fast solution that works for all cases. Implementations should try to achieve the correct blending output for as many situations as possible. Whether depth value is written or whether to sort is up to the implementation. For example, implementations can discard pixels which have zero or close to zero alpha value to avoid sorting issues.

## Cameras

A camera defines the projection matrix that transforms from view to clip coordinates. The projection can be perspective or orthographic. Cameras are contained in nodes and thus can be transformed. Their world-space transformation matrix is used for calculating view-space transformation.

Cameras are stored in the asset's `cameras` array. Each camera defines a `type` property that designates the type of projection (perspective or orthographic), and either a `perspective` or `orthographic` property that defines the details.

Depending on the presence of `zfar` property, perspective cameras could use finite or infinite projection.

The following example defines two perspective cameras with supplied values for Y field of view, aspect ratio, and clipping information.

```json
{
    "cameras": [
        {
            "name": "Finite perspective camera",
            "type": "perspective",
            "perspective": {
                "aspectRatio": 1.5,
                "yfov": 0.660593,
                "zfar": 100,
                "znear": 0.01
            }      
        },
        {
            "name": "Infinite perspective camera",
            "type": "perspective",
            "perspective": {
                "aspectRatio": 1.5,
                "yfov": 0.660593,
                "znear": 0.01
            }
        }
    ]
}
```

### Projection Matrices

Runtimes are expected to use the following projection matrices.

#### Infinite perspective projection
<p><img src="figures/infinite-perspective.png" /></p>
where
- `a` equals `camera.perspective.aspectRatio`;
- `y` equals `camera.perspective.yfov`;
- `n` equals `camera.perspective.znear`.

#### Finite perspective projection
<p><img src="figures/finite-perspective.png" /></p>
where
- `a` equals `camera.perspective.aspectRatio`;
- `y` equals `camera.perspective.yfov`;
- `f` equals `camera.perspective.zfar`;
- `n` equals `camera.perspective.znear`.

#### Orthographic projection
<p><img src="figures/ortho.png" /></p>
where
- `r` equals `camera.orthographic.xmag`;
- `t` equals `camera.orthographic.ymag`;
- `f` equals `camera.orthographic.zfar`;
- `n` equals `camera.orthographic.znear`.

## Animations

glTF supports articulated and skinned animation via key frame animations of nodes' transforms. Key frame data is stored in buffers and referenced in animations using accessors.
glTF 2.0 also supports animation of instantiated Morph Targets in a similar fashion.

> **Note:** glTF 2.0 only supports animating node transforms and Morph Targets weights. A future version of the specification may support animating arbitrary properties, such as material colors and texture transform matrices.

> **Note:** glTF 2.0 defines only animation storage, so this specification doesn't define any particular runtime behavior, such as: order of playing, auto-start, loops, mapping of timelines, etc...

All animations are stored in the `animations` array of the asset. An animation is defined as a set of channels (the `channels` property) and a set of samplers that specify accessors with key frame data and interpolation method (the `samplers` property).

The following examples show expected animations usage.

```json
{
    "animations": [
        {
            "name": "Animate all properties of one node with different samplers",
            "channels": [
                {
                    "sampler": 0,
                    "target": {
                        "node": 1,
                        "path": "rotation"
                    }
                },
                {
                    "sampler": 1,
                    "target": {
                        "node": 1,
                        "path": "scale"
                    }
                },
                {
                    "sampler": 2,
                    "target": {
                        "node": 1,
                        "path": "translation"
                    }
                }
            ],
            "samplers": [
                {
                    "input": 4,
                    "interpolation": "LINEAR",
                    "output": 5
                },
                {
                    "input": 4,
                    "interpolation": "LINEAR",
                    "output": 6
                },
                {
                    "input": 4,
                    "interpolation": "LINEAR",
                    "output": 7
                }
            ]
        },
        {
            "name": "Animate two nodes with different samplers",
            "channels": [
                {
                    "sampler": 0,
                    "target": {
                        "node": 0,
                        "path": "rotation"
                    }
                },
                {
                    "sampler": 1,
                    "target": {
                        "node": 1,
                        "path": "rotation"
                    }
                }
            ],
            "samplers": [
                {
                    "input": 0,
                    "interpolation": "LINEAR",
                    "output": 1
                },
                {
                    "input": 2,
                    "interpolation": "LINEAR",
                    "output": 3
                }
            ]
        },
        {
            "name": "Animate two nodes with the same sampler",
            "channels": [
                {
                    "sampler": 0,
                    "target": {
                        "node": 0,
                        "path": "rotation"
                    }
                },
                {
                    "sampler": 0,
                    "target": {
                        "node": 1,
                        "path": "rotation"
                    }
                }
            ],
            "samplers": [
                {
                    "input": 0,
                    "interpolation": "LINEAR",
                    "output": 1
                }
            ]
        }
    ]
}
```

*Channels* connect the output values of the key frame animation to a specific node in the hierarchy. A channel's `sampler` property contains the index of one of the samplers present in the containing animation's `samplers` array. The `target` property is an object that identifies which node to animate using its `node` property, and which property of the node to animate using `path`. Valid path names are `"translation"`, `"rotation"`, and `"scale"`.

Each of the animation's *samplers* defines the input/output pair: a set of floating point scalar values representing time; and a set of three-component floating-point vectors representing translation or scale, or four-component floating-point vectors representing rotation. All values are stored in a buffer and accessed via accessors. Interpolation between keys is performed using the interpolation formula specified in the `interpolation` property

> Note: glTF 2.0 animation samplers support only discrete and linear interpolation.

glTF animations can be used to drive articulated or skinned animations. Skinned animation is achieved by animating the joints in the skin's joint hierarchy.

**TODO: animating morph weights** 

## Specifying Extensions

glTF defines an extension mechanism that allows the base format to be extended with new capabilities. Any glTF object can have an optional `extensions` property, as in the following example:

```json
{
    "material": [
        {
            "extensions": {
                "KHR_materials_common": {
                    "technique": "LAMBERT"
                }
            }
        }
    ]
}
```

All extensions used in a glTF asset must be listed in the top-level `extensionsUsed` array object, e.g.,

```json
{
    "extensionsUsed": [
        "KHR_materials_common",
        "VENDOR_physics"
    ]
}
```

All glTF extensions required to load and/or render an asset must be listed in the top-level `extensionsRequired` array, e.g.,

```json
{
    "extensionsRequired": [
        "WEB3D_quantized_attributes"
    ]
}
```

For more information on glTF extensions, consult the [extensions registry specification](../extensions/README.md).

# Properties Reference

**TODO: Refer to JSON schemas for now**

# Acknowledgments

* Sarah Chow, Cesium
* Tom Fili, Cesium
* Scott Hunter, Analytical Graphics, Inc.
* Brandon Jones, Google
* Ed Mackey, Analytical Graphics, Inc.
* Matthew McMullan,
* Darryl Gough, 
* Alex Wood,
* Rob Taglang, Cesium
* Marco Hutter,
* Sean Lilley,
* Corentin Wallez,
* Yu Chen Hou

# Appendix A: GLB File Format Specification

See [GLB_FORMAT.md](GLB_FORMAT.md).

# Appendix B: Tangent Space Recalculation

**TODO**

# Appendix C: BRDF Implementation

**TODO**

# Appendix D: Full Khronos Trademark Statement

Copyright (C) 2013-2017 The Khronos Group Inc. All Rights Reserved. This specification is protected by copyright laws and contains material proprietary to the Khronos Group, Inc. It or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast, or otherwise exploited in any manner without the express prior written permission of Khronos Group. You may use this specification for implementing the functionality therein, without altering or removing any trademark, copyright or other notice from the specification, but the receipt or possession of this specification does not convey any rights to reproduce, disclose, or distribute its contents, or to manufacture, use, or sell anything that it may describe, in whole or in part. Khronos Group grants express permission to any current Promoter, Contributor or Adopter member of Khronos to copy and redistribute UNMODIFIED versions of this specification in any fashion, provided that NO CHARGE is made for the specification and the latest available update of the specification for any version of the API is used whenever possible. Such distributed specification may be reformatted AS LONG AS the contents of the specification are not changed in any way. The specification may be incorporated into a product that is sold as long as such product includes significant independent work developed by the seller. A link to the current version of this specification on the Khronos Group website should be included whenever possible with specification distributions. Khronos Group makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this specification, including, without limitation, any implied warranties of merchantability or fitness for a particular purpose or non-infringement of any intellectual property. Khronos Group makes no, and expressly disclaims any, warranties, express or implied, regarding the correctness, accuracy, completeness, timeliness, and reliability of the specification. Under no circumstances will the Khronos Group, or any of its Promoters, Contributors or Members or their respective partners, officers, directors, employees, agents, or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials. Khronos, Vulkan, SYCL, SPIR, WebGL, EGL, COLLADA, StreamInput, OpenVX, OpenKCam, glTF, OpenKODE, OpenVG, OpenWF, OpenSL ES, OpenMAX, OpenMAX AL, OpenMAX IL and OpenMAX DL are trademarks and WebCL is a certification mark of The Khronos Group Inc. OpenCL is a trademark of Apple Inc. and OpenGL and OpenML are registered trademarks and the OpenGL ES and OpenGL SC logos are trademarks of Silicon Graphics International used under license by Khronos. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
