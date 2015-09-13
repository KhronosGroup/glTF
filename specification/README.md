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
  * [The glTF Ecosystem](#glTF-ecosystem)
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

glTF solves these problems by providing a compact runtime format that can be rendered with minimal processing. The format combines an easily parseable JSON scene description with one or more binary files representing geometry, animations and other rich data. Binary data is stored in such a way that it can be loaded directly into GL buffers without additional processing. Using this approach, glTF is able to faithfully preserve full hierarchial scenes with nodes, meshes, cameras, lights, materials and animations, while enabling efficient delivery and fast loading.

<a name="glTFbasics"></a>

## glTF Basics

glTF assets are JSON files plus supporting external data. Specifically, a glTF asset is represented by:

* A JSON-formatted file (`.gltf`) containing a full scene description: node hierarchy, materials, lights, cameras, as well as descriptor information for meshes, shaders, animations and other constructs
* Binary files (`.bin`) containing geometry and animation data, and other buffer-based data
* Image files (`.jpg`, `.png`, etc.) for textures
* GLSL text files (`.glsl`) for GLSL shader source code

Assets defined in other formats, such as images and GLSL shader source code, may be stored in external files referenced via URI or embedded directly into the JSON using  [data URIs](https://developer.mozilla.org/en/data_URIs).

<mark>*Todo: Patrick where did we land on allow binary data to be encoded as data URIs?*</mark>

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
* *glTF is not a container format.* Supporting assets can be encoded within the JSON file as data URIs, allowing for the delivery of the entire asset in a single file. However there is no provision for embedding multiple independent files within the same archive/package.
* *glTF is not intended to be human-readable,* though by virtue of being represented in JSON, it is developer-friendly.

<a name="mimetypes"></a>
## File Extensions and MIME Types

<mark>*why vnd.gltf? gltf is not vendor-specific*</mark>

* `*.gltf` files use `model/vnd.gltf+json`
* `*.bin` files use `application/octet-stream`
* `*.glsl` files use `text/plain`
* Texture files use the official `image/*` type based on the specific image format.


<a name="glTF-ecosystem"></a>
## The glTF Ecosystem

glTF can be combined with freely available open-source tools, enabling an open ecosystem for the development of content and applications.

### Loaders, Importers and Playback Engines

<mark>*list here*</mark>

### File Converters and Exporters

**COLLADA2GLTF**

The Khronos Group is maintaining [COLLADA2GLTF](https://github.com/KhronosGroup/glTF/tree/master/converter/COLLADA2GLTF), a converter from the COLLADA 3D interchange standard [ reference here ] to glTF. The COLLADA2GLTF pipeline is depicted in the figure below.

![](figures/COLLADA2GLTF.png)


For a simple example, see the converted [COLLADA duck model](https://github.com/KhronosGroup/glTF/tree/master/model/duck).

> glTF is not part of the COLLADA standard; it is being developed as its own independent specification. While glTF has some designs borrowed from COLLADA (in greatly simplified form), there are many other new concepts not found in COLLADA, motivated by the unique requirements for fast transmission and loading.

**FBX-glTF**

Autodesk has developed an initial implementation of a [converter](https://github.com/cyrillef/FBX-glTF) from its FBX format to glTF.
 
<a name="geometry-compression"></a>
### Extensions

There is already work underway to define compression extensions and/or work with outside organizations to define compression techniques suitable for use with glTF. These will be defined as glTF extensions.


> The 3D Formats Working Group is developing partnerships to define the codec options for geometry compression.  glTF defines the node hierarchy, materials, animations, and geometry, and will reference the external compression specs.



<a name="concepts"></a>
# Concepts

*This section is non-normative.*

<a name="file-structure"></a>
## File Structure


<a name="scene-graph"></a>
## The Scene Graph

### Nodes and Hierarchy

Mention tree vs. DAG here and discuss how we're simplifying for V1.

### Transforms

<a name="accessing-binary-data"></a>
## Accessing Binary Data


### Buffers and Buffer Views

A *buffer* is data stored as binary blobs in a file. The buffer can contain a combination of geometry, animation, and skins. Buffer data is little endian.

Binary blobs allow efficient creation of GL buffers andtextures since they require no additional parsing, except perhaps decompression. An asset can have any number of buffer files for flexibility for a wide array of applications.

A *bufferView* represents a subset of data in a buffer, defined by an integer offset into the buffer, and an integer data length. The bufferView also defines a target data type (ARRAY_BUFFER,ELEMENT_BUFFER, animation/skin) so that the implementation can readily create and populate buffers in memory.

buffers and bufferViews do not contain type information. They simply define the raw data for retrieval from the file. Objects within the glTF file (meshes, skins, animations) never access buffers or bufferViews directly, but rather via *accessors*.

### Accessors

All large data for meshes, skins and animations is stored in buffers and retrieved via accessors.

An *accessor* defines a method for retrieving data as typed arrays from within a bufferView. The accessor specifies a component type (e.g. FLOAT) and a data type (e.g. VEC3), which when combined define the complete data type for each array. The accessor also specifies the location and size of the data within the bufferView using the properties `byteOffset` and `count`. count specifies the number of attributes within the bufferView, *not* the number of bytes.

<a name="geometry-and-meshes"></a>

## Geometry and Meshes

Any node can contain one or more meshes, defined in its `meshes` property. Any node can contain one skin, defined in its `instanceSkin` property.

### Meshes

In glTF, meshes are defined as arrays of *primitives*. Primitives correspond to the data required for calls to glDrawElements.

The following example defines a mesh containing one triangle set primitive:

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

                    
                    
### Skins

*TBD*


<a name="materials-and-shading"></a>
## Materials and Shading
### Techniques
### Passes
### Programs
### Shader Semantics
### Render States
### Common Techniques
### Textures

<a name="cameras"></a>
## Cameras

<a name="lights"></a>
## Lights

<a name="animations"></a>
## Animations

<a name="metadata"></a>
## Metadata

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
# Acknowledgments

* Brandon Jones, for the first version of Three.js loader and all his support in the early days of this project.
* Tom Fili, Analytical Graphics, Inc.
* Scott Hunter, Analytical Graphics, Inc.
* Ed Mackey, Analytical Graphics, Inc.

<a name="references"></a>
# References

* [WebGL 1.0.2 spec](https://www.khronos.org/registry/webgl/specs/1.0/)
* [COLLADA 1.5 spec](http://www.khronos.org/files/collada_spec_1_5.pdf)
