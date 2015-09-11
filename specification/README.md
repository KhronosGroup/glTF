<p align="center">
<img src="figures/glTF_300.jpg" />
</p>

*Draft, Version 1.0*

The GL Transmission Format (glTF) is a runtime asset delivery format for GL APIs: WebGL, OpenGL ES, and OpenGL.  glTF bridges the gap between 3D content creation tools and modern GL applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.


> 
_This is a draft specification; it is incomplete and may change before ratification.  We have made it available early in the spirit of transparency to receive early community feedback.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback._

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
  * [The glTF Ecosystem](#glTFecosystem)
* [Concepts](#concepts)
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

Traditional 3D modeling formats have been designed to store data for offline use, primarily to support authoring workflows on desktop systems. Industry-standard 3D interchange formats allow for sharing assets between different modeling tools, and within the content pipeline in general. However, neither of these types of formats has been optimized for download speed or fast loading at runtime. Files tend to grow very large, and applications need to do a significant amount of processing to load such assets into GL-based applications.

Applications seeking high performance rarely load modeling formats directly; instead, they process models offline as part of a custom content pipeline, converting the assets to a proprietary format optimized for their runtime application.  This has led to a fragmented market of incompatible proprietary runtime formats, and duplicated efforts in the content creation pipeline. 3D assets exported for one application can not be reused in another application without going back to the original modeling tool-specific source and performing another proprietary export step.

With the advent of mobile- and web-based 3D computing, new classes of 3D applications have emerged that require fast, dynamic loading of standardized 3D assets. Digital marketing solutions, e-commerce product visualizations and online model-sharing sites are just a few of the connected 3D applications being built today using WebGL or OpenGL ES. Beyond the need for efficient delivery, many of these online 3D applications can benefit from a standard, interoperable format to enable sharing and reuse of assets between users, between applications, and within heterogeneous, distributed content pipelines.

glTF solves these problems by providing a compact runtime format that can be rendered with minimal processing. The format combines an easily parseable JSON scene description with one or more binary files representing geometry, animations and other rich data. Binary data is stored in such a way that it can be loaded directly into GL buffers without additional processing. Using this approach, glTF is able to faithfully preserve full hierarchial scenes with nodes, meshes, cameras, lights, materials and animations, while enabling efficient delivery and fast loading.

3D is the last media type without a standard codec.  Audio has mp3.  Video has H.264.  Images have png and jpg.  Why is there no analogous format for 3D?  The variety of use cases and complexity of 3D asset types, and the heretofore lack of market demand have historically left 3D without a standard codec. But now the time has come, and the need is there. glTF has the potential to become a cross-vendor standard allowing for portable, reusable content, unified asset repositories and archives, and optimized codec implementations in hardware and software.

<a name="glTFbasics"></a>

## glTF Basics

glTF assets are JSON files plus supporting external data. Specifically, a glTF asset is represented by:

* A JSON-formatted file (`.gltf`) containing a full scene description: node hierarchy, materials, lights, cameras, as well as descriptor information for meshes, shaders, animations and other constructs
* Binary files (`.bin`) containing geometry and animation data, and other buffer-based data
* Image files (`.jpg`, `.png`, etc.) for textures
* GLSL text files (`.glsl`) for GLSL shader source code

Assets defined in other formats, such as images and GLSL shader source code, may be stored in external files referenced via URI or embedded directly into the JSON using  [data URIs](https://developer.mozilla.org/en/data_URIs).


<mark>*diagram here*</mark>

<mark>*sample fragment here*</mark>

<a name="designgoals"></a>

## Design Goals

glTF has been designed to meet the following goals:

* *Compact file sizes.* While web developers like to work with clear text as much as possible, clear text encoding is simply not practical for transmitting 3D data due to sheer size. The glTF JSON file itself is clear text, but it is compact and rapid to parse. All large data such as geometry and animations are stored in binary files that are much smaller than equivalent text representations.
* *Fast loading.* glTF data structures have been designed to mirror the GL API data as closely as possible, both in the JSON and binary files, to reduce load times. For example, binary data for meshes can be loaded directly into WebGL typed arrays with a simple data copy; no parsing or further processing is required.
* *Runtime-independence.* glTF makes no assumptions about the target application or 3D engine. glTF specifies no runtime behaviors other than rendering and animation.
* *Complete 3D scene representation.* Exporting single objects from a modeling package is not sufficient for many applications. Often, authors want to use load entire scenes, including nodes, transformations, transform hierarchy, meshes, materials, lights, cameras, animations, into their applications. glTF strives to preserve all of this information for use in the downstream application.
* *Extensibility.* While the initial base specification supports a rich feature set, there will be many opportunities for growth and improvement. glTF defines a mechanism that allows the addition of vendor-specific and experimental extensions.

The design of glTF takes a pragmatic approach. The format is meant to mirror the GL APIs as closely as possible, but if it did only that, there would be no lights, cameras, animations and other features typically found in both modeling tools and runtime systems, and much semantic information would be lost in the translation. By supporting these common constructs, glTF content can not only load and render, but it can be immediately usable in a wider range of applications and require less duplication of effort in the content pipeline.

glTF is **not** intended to be human-readable, though by virtue of being represented in JSON, it is developer-friendly.

<a name="mimetypes"></a>
## File Extensions and MIME Types

* `*.gltf` files use `model/vnd.gltf+json`
* `*.bin` files use `application/octet-stream`
* `*.glsl` files use `text/plain`
* Texture files use the official `image/*` type based on the specific image format.

<a name="glTFecosystem"></a>
## The glTF Ecosystem

<mark>*This all needs to get updated*</mark>

Combined with open-source pipeline tools, that, while not part of the glTF specification, provide the ecosystem of freely-available tools necessary to facilitate adoption of glTF.  In particular, [COLLADA2GLTF](https://github.com/KhronosGroup/glTF/tree/master/converter/COLLADA2GLTF) converts COLLADA assets to glTF.

![](figures/COLLADA2GLTF.png)

 

> The 3D Formats Working Group is developing partnerships to define the codec options for geometry compression.  glTF defines the node hierarchy, materials, animations, and geometry, and will reference the external compression specs. 

For a simple example, see the converted [COLLADA duck model](https://github.com/KhronosGroup/glTF/tree/master/model/duck).

Finally, glTF is not part of COLLADA, that is, it is not a COLLADA profile.  It is its own specification with many designs borrowed from COLLADA and simplified.

<a name="concepts"></a>
# Concepts

*This section is non-normative.*


<a name="scene-graph"></a>
## The Scene Graph

### Nodes and Hierarchy

Mention tree vs. DAG here and discuss how we're simplifying for V1.

### Transforms

<a name="accessing-binary-data"></a>
## Accessing Binary Data
### Buffers and Buffer Views
### Accessors

<a name="geometry-and-meshes"></a>
## Geometry and Meshes
### Primitives
### Meshes
### Skins

<a name="materials-and-shading"></a>
## Materials and Shading
### Techniques
### Passes
### Programs
### Shader Semantics
### Render States
### Common Techniques

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

   * <a href="accessor.schema.json">`accessor`</a>
   * <a href="animation.schema.json">`animation`</a>
   * <a href="animationChannel.schema.json">`animation/channel`</a>
   * <a href="animationChannelTarget.schema.json">`animation/channel/target`</a>
   * <a href="animationParameter.schema.json">`animation/parameter`</a>
   * <a href="animationSampler.schema.json">`animation/sampler`</a>
   * <a href="asset.schema.json">`asset`</a>
   * <a href="buffer.schema.json">`buffer`</a>
   * <a href="bufferView.schema.json">`bufferView`</a>
   * <a href="camera.schema.json">`camera`</a>
   * <a href="cameraOrthographic.schema.json">`camera/orthographic`</a>
   * <a href="cameraPerspective.schema.json">`camera/perspective`</a>
   * <a href="extension.schema.json">`extension`</a>
   * <a href="extras.schema.json">`extras`</a>
   * <a href="glTF.schema.json">`glTF`</a> (root property for model)
   * <a href="image.schema.json">`image`</a>
   * <a href="light.schema.json">`light`</a>
   * <a href="lightAmbient.schema.json">`light/ambient`</a>
   * <a href="lightDirectional.schema.json">`light/directional`</a>
   * <a href="lightPoint.schema.json">`light/point`</a>
   * <a href="lightSpot.schema.json">`light/spot`</a>
   * <a href="material.schema.json">`material`</a>
   * <a href="materialInstanceTechnique.schema.json">`material/instanceTechnique`</a>
   * <a href="materialInstanceTechniqueValues.schema.json">`material/instanceTechnique/values`</a>
   * <a href="mesh.schema.json">`mesh`</a>
   * <a href="meshPrimitive.schema.json">`mesh/primitive`</a>
   * <a href="meshPrimitiveAttribute.schema.json">`mesh/primitive/attribute`</a>
   * <a href="node.schema.json">`node`</a>
   * <a href="nodeInstanceSkin.schema.json">`node/instanceSkin`</a>
   * <a href="program.schema.json">`program`</a>
   * <a href="sampler.schema.json">`sampler`</a>
   * <a href="scene.schema.json">`scene`</a>
   * <a href="shader.schema.json">`shader`</a>
   * <a href="skin.schema.json">`skin`</a>
   * <a href="technique.schema.json">`technique`</a>
   * <a href="techniqueParameters.schema.json">`technique/parameters`</a>
   * <a href="techniquePass.schema.json">`technique/pass`</a>
   * <a href="techniquePassDetails.schema.json">`technique/pass/details`</a>
   * <a href="techniquePassDetailsCommonProfile.schema.json">`technique/pass/details/commonProfile`</a>
   * <a href="techniquePassDetailsCommonProfileTexcoordBindings.schema.json">`technique/pass/details/commonProfile/texcoordBindings`</a>
   * <a href="techniquePassInstanceProgram.schema.json">`technique/pass/instanceProgram`</a>
   * <a href="techniquePassInstanceProgramAttribute.schema.json">`technique/pass/instanceProgram/attribute`</a>
   * <a href="techniquePassInstanceProgramUniform.schema.json">`technique/pass/instanceProgram/uniform`</a>
   * <a href="techniquePassStates.schema.json">`technique/pass/states`</a>
   * <a href="texture.schema.json">`texture`</a>

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
