<p align="center">
<img src="specification/figures/gltf.png" />
</p>

[![Join the chat at https://gitter.im/KhronosGroup/glTF](https://badges.gitter.im/KhronosGroup/glTF.svg)](https://gitter.im/KhronosGroup/glTF)

glTF™ (GL Transmission Format) is a royalty-free specification for the efficient transmission and loading of 3D scenes and models by applications. glTF minimizes both the size of 3D assets, and the runtime processing needed to unpack and use those assets. glTF defines an extensible, common publishing format for 3D content tools and services that streamlines authoring workflows and enables interoperable use of content across the industry.

## Specification

* [glTF Specification, 2.0](specification/2.0/README.md) (or [all specification versions](specification/README.md))
* [glTF Extension Registry](extensions/README.md)

Please provide spec feedback and community updates by submitting [issues](https://github.com/KhronosGroup/glTF/issues).  For quick questions, use [gitter](https://gitter.im/KhronosGroup/glTF).

## Quickstart

### Overview

<p align="center">
<a href="specification/2.0/figures/gltfOverview-2.0.0.png"><img src="specification/2.0/figures/gltfOverview-2.0.0-small.png" /></a>
</p>

### For developers

* [Sample models](https://github.com/KhronosGroup/glTF-Sample-Models) for testing runtime engines and content pipeline tools.
   * [100,000+ models](https://sketchfab.com/models?features=downloadable&sort_by=-likeCount) under Creative Commons license on Sketchfab (check license for individual models).
* [Drag-and-drop validator](http://github.khronos.org/glTF-Validator/) for verifying correctness of existing glTF files.

### For artists

[Blender exporter](https://github.com/KhronosGroup/glTF-Blender-Exporter) for creating and converting models with glTF 2.0.

Preview tools:
  - [Sketchfab](https://sketchfab.com/)
  - [BabylonJS Sandbox](https://www.babylonjs.com/sandbox/)
  - [Drag-and-drop viewer](https://gltf-viewer.donmccurdy.com/)

## Contents

* [glTF Tools](#gltf-tools)
  * [Converters and Exporters](#converters-and-exporters)
  * [Optimizers](#optimizers)
  * [Validators](#validators)
  * [Editors and Modeling Tools](#editors-and-modeling-tools)
  * [Applications](#applications)
  * [Loaders and Viewers](#loaders-and-viewers)
    * [WebGL Engines](#webgl-engines)
    * [WebGL Sample Code](#webgl-sample-code)
    * [Game Engines](#game-engines)
  * [Languages](#languages)
  * [Utilities](#utilities)
* [Stack Overflow](#stack-overflow)
* [Presentations and Articles](#presentations-and-articles)
  * [Intros](#intros)
  * [All Presentations and Articles](#all-presentations-and-articles)

## glTF Tools

### Converters and Exporters

| Tool | Input | Output | Description |
|------|-------|--------|-------------|
| [Khronos Group Blender Exporter](https://github.com/KhronosGroup/glTF-Blender-Exporter) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Official exporter for Blender |
| [Unity Exporter (Sketchfab fork)](https://github.com/sketchfab/Unity-glTF-Exporter) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Unity editor wizard that exports to glTF Format |
| Unity COLLADA & glTF Exporter [Unity 5](https://www.assetstore.unity3d.com/#!/content/40946) & [Unity 2017](https://www.assetstore.unity3d.com/#!/content/99793) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Commercial unity asset store editor extension to export COLLADA and glTF via collada2gltf |
| Sketchup [export](http://extensions.sketchup.com/en/content/gltf-exporter) and [import](http://extensions.sketchup.com/en/content/gltf-import) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Separate extensions for export and import |
| [obj2gltf](https://github.com/AnalyticalGraphicsInc/OBJ2GLTF)  | `OBJ` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Command-line converter |
| [blendergltf](https://github.com/Kupoman/blendergltf) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF exporter for Blender aimed at streaming scenes to other applications |
| [Assimp](http://www.assimp.org/) | [Multiple](https://github.com/assimp/assimp#supported-file-formats) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | General-purpose online conversion pipeline |
| [PiXYZ Studio](https://www.pixyz-software.com/studio/) | [Multiple](https://docs.google.com/spreadsheets/d/1aWdeBOvZsbnwHfbyhuVS4vBDwevsT2Bz65hEhc0CtnU/pubhtml?gid=786960864) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Interactively prepare & optimize any CAD data |
| [COLLADA2GLTF](https://github.com/KhronosGroup/COLLADA2GLTF/) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Official command-line converter, with updates for glTF 2.0 in progress |
| [Cesium drag-and-drop converter](http://cesiumjs.org/convertmodel.html) | `OBJ`, `COLLADA` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Online drag and drop converter |
| [batchgltf](https://github.com/feiss/batchgltf) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Batch converter with GUI |
| [FBX-glTF](https://github.com/cyrillef/FBX-glTF) | `FBX` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Plug-in and command-line converter |
| [jgltf-obj](https://github.com/javagl/JglTF/tree/master/jgltf-obj) | `OBJ` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Java programmatic and command-line converters |
| [Docker collada2gltf](https://hub.docker.com/r/winsent/collada2gltf/) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Docker container for Python web service, built on COLLADA2GLTF |
| [collada2gltf-web-service](https://github.com/AnalyticalGraphicsInc/collada2gltf-web-service) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Node.js web service, built on COLLADA2GLTF |
| [Node.js collada2gltf](https://www.npmjs.com/package/collada2gltf) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Node.js programmatic wrapper for COLLADA2GLTF |
| [osm2cesium](https://github.com/stirringhalo/osm2cesium) | `OSM` | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Extracts buildings in OpenStreetMap and converts them to glTF |

### Optimizers

| Tool | Status | Description |
|------|--------|-------------|
| [glTF Pipeline](https://github.com/AnalyticalGraphicsInc/gltf-pipeline) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Official Node.js command-line tool for optimizing glTF assets |
| [gltf2glb](https://github.com/Geopipe/gltf2glb) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Python tool to convert glTF to Binary glTF |
| [binary-gltf-utils](https://github.com/Qantas94Heavy/binary-gltf-utils) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Node.js tool to convert glTF to Binary glTF |

### Validators

| Tool | Status | Description |
|------|--------|-------------|
| [glTF Validator](https://github.com/KhronosGroup/glTF-Validator) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Official command-line and drag-and-top tool to validate glTF assets against the specification |

### Editors and Modeling Tools

| Tool | Status | Description |
|------|--------|-------------|
| [glTF VSCode Extension](https://marketplace.visualstudio.com/items?itemName=cesium.gltf-vscode) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cross-platform JSON schema validation and previews for glTF 2.0 and 1.0 |
| [Blender Importer](https://github.com/ksons/gltf-blender-importer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Blender importer for glTF 2.0 (alpha) |

### Applications

| Application | Status | Description |
|-------------|--------|-------------|
| [Paint 3D](https://www.microsoft.com/en-us/store/p/paint-3d/9nblggh5fv99) by Microsoft | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | 3D creation tool |
| [Remix 3D](https://www.remix3d.com) by Microsoft | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Share and print 3D creations |
| [Sketchfab](https://blog.sketchfab.com/sketchfab-now-supports-gltf/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Platform to publish, share & discover 3D online and in VR |
| [Wordpress](https://wordpress.org/plugins/gltf-media-type/) (plugin) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Inline glTF renderer for WordPress |
| [Archilogic](https://spaces.archilogic.com/blog/gltf-import-export) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Web-based 3D platform for architecture and interiors |
| [Aspose.3D](https://www.aspose.com/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | File Format APIs for creating, editing, and converting over 100 file formats |

### Loaders and Viewers

#### WebGL Engines

To compare WebGL-based glTF loaders, see [gltf-test](https://github.com/cx20/gltf-test).

| Engine | Status | Scope | Related |
|--------|--------|-------|-------------|
| [three.js](https://threejs.org/) ([loader](https://threejs.org/docs/index.html#examples/loaders/GLTF2Loader)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Drag-and-drop viewer](https://gltf-viewer.donmccurdy.com/), [`<gltf-model/>` component](https://github.com/mrdoob/model-tag) |
| [BabylonJS](http://babylonjs.com/) ([loader](https://github.com/BabylonJS/Babylon.js/tree/master/loaders/src/glTF)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Sandbox / viewer](https://www.babylonjs.com/sandbox/) |
| [Cesium](http://cesiumjs.org/) ([loader](https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Scene/Model.js)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Drag-and-drop viewer](https://www.virtualgis.io/gltfviewer/), [tutorial](https://cesiumjs.org/tutorials/3D-Models-Tutorial/) |
| [OSG.JS](http://osgjs.org/) ([loader](https://github.com/cedricpinson/osgjs/blob/master/sources/osgPlugins/ReaderWriterGLTF.js)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | |
| [Grimoire.js](https://github.com/GrimoireGL/GrimoireJS) ([loader](https://github.com/GrimoireGL/grimoirejs-gltf)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Geometry and materials, partial animation | |
| [A-Frame](https://aframe.io/) ([loader](https://aframe.io/docs/0.6.0/components/gltf-model.html)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | All | [Experimental glTF 2.0 support](https://github.com/donmccurdy/aframe-extras/tree/master/src/loaders) |
| [PEX](http://vorg.github.io/pex/) ([loader](https://github.com/pex-gl/pex-gltf)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Geometry and materials| |
| [GLBoost](https://github.com/emadurandal/GLBoost) ([loader](https://github.com/emadurandal/GLBoost/blob/master/src/js/middle_level/loader/GLTFLoader.js)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | ? | [Examples](https://gitcdn.xyz/repo/emadurandal/GLBoost/master/examples/index.html) |
| [xml3d.js](http://xml3d.org) ([loader](https://github.com/xml3d/xml3d-gltf-plugin)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Geometry and materials | |
| [X3DOM](http://x3dom.org/) ([loader](https://github.com/x3dom/x3dom/blob/master/src/util/glTF/glTFLoader.js)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Geometry and materials | |
| [xeogl](http://xeogl.org/) ([loader](https://github.com/xeolabs/xeogl/tree/master/src/importing/gltf)) | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Geometry and materials | [Tutorial](https://github.com/xeolabs/xeogl/wiki/Importing-glTF) |

#### WebGL Sample Code

| Sample | Status | Description |
|--------|--------|-------------|
| [Sample glTF loader with WebGL PBR shader](http://github.khronos.org/glTF-WebGL-PBR/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Engine-agnostic reference implementation for PBR shading with WebGL in glTF 2.0 ([source code](https://github.com/KhronosGroup/glTF-WebGL-PBR)) |

#### Game Engines

| Tool | Status | Description |
|------|--------|-------------|
| [Unity Loader](https://github.com/AltspaceVR/UnityGLTF) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Unity3D library for exporting, loading, parsing, and rendering glTF assets |
| [Godot Game Engine](https://godotengine.org/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Godot 3.0 fully supports glTF import |

### Languages

#### JavaScript / Node.js

| Tool | Status | Description |
|------|--------|-------------|
| [gltf-viewer](https://github.com/avgp/gltf-viewer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Web component to display glTF models on a website |
| [minimal-gltf-loader](https://github.com/shrekshao/minimal-gltf-loader) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A minimal, engine-agnostic JavaScript glTF Loader, with a raw WebGL 2 renderer example |
| [gltf-walker](https://github.com/ksons/gltf-walker) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Convenience library for processing glTF |
| [RedCube.js](https://github.com/Reon90/redcube) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | glTF viewer without dependencies |

#### C++

| Tool | Status | Description |
|------|--------|-------------|
| [Tiny glTF loader](https://github.com/syoyo/tinygltf) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat)  | Header only C++ glTF parsing library, with [Alembic→glTF](https://github.com/syoyo/tinygltfloader/tree/master/examples/alembic_to_gltf) and [CyHair->glTF](https://github.com/syoyo/tinygltfloader/tree/master/examples/cyhair_to_gltf) converters |
| [yocto-gltf](https://github.com/xelatihy/yocto-gl) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Single file, header only, C++ glTF loading/writing automatically generated from the spec |
| [glTF2-loader](https://github.com/Lugdunum3D/glTF2-loader/tree/dev) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | C++14 glTF (2.0 only) parsing library, by Lugdunum3D |
| [gltfpp](https://github.com/mmha/gltfpp) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF 2.0 loader for C++14 |
| [TGen](https://github.com/mlimper/tgen/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Simple C++ Tangent Generator |
| [Cinder](http://discourse.libcinder.org/t/gltf-initial-release-wip/212) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Work-in-progress glTF importer |
| [nvpro-pipeline](https://github.com/nvpro-pipeline/pipeline) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Research rendering pipeline by NVIDIA for minimizing CPU cost |
| [AssetKit](https://github.com/recp/assetkit) | ? | 3D asset importer/exporter/util library |

#### C++ and Vulkan

| Tool | Status | Description |
|------|--------|-------------|
| [Laugh Engine](https://github.com/jian-ru/laugh_engine#laugh-engine) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Vulkan PBR and IBL renderer |
| [Lugdunum3D](https://github.com/Lugdunum3D/Lugdunum) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A modern cross-platform 3D rendering engine built with Vulkan and modern C++14 |
| [UX3D Engine](http://www.ux3d.io/ux3d-engine/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cross-platform multi-threaded Vulkan 3D Engine with glTF 2.0 import and export |

#### C# #

| Tool | Status | Description |
|------|--------|-------------|
| [C# glTF loader](https://github.com/KhronosGroup/glTF-CSharp-Loader) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | C# reference loader for glTF |
| [Aspose.3D for .NET](http://www.aspose.com/products/3d/net) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat)| Import, export, and convert glTF |

#### Go

| Tool | Status | Description |
|------|--------|-------------|
| [gltf](https://github.com/sturfeeinc/glTF) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | go library for marshaling and unmarshaling glTF |

#### Rust

| Tool | Status | Description |
|------|--------|-------------|
| [Loader](https://github.com/Alteous/gltf) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A crate for loading glTF 2.0 |
| [Viewer](https://github.com/bwasty/gltf-viewer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF 2.0 Viewer written in Rust |

#### Haxe

| Tool | Status | Description |
|------|--------|-------------|
| [haxe-gltf](https://github.com/FuzzyWuzzie/haxe-gltf) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | A Haxe library for reading glTF |

#### Java

* [JglTF](https://github.com/javagl/JglTF) - Java libraries for glTF
  * [jgltf-impl](https://github.com/javagl/JglTF/tree/master/jgltf-impl) - Classes representing a glTF model, auto-generated from the schema
  * [jgltf-model](https://github.com/javagl/JglTF/tree/master/jgltf-model) - Classes for reading, processing, converting and writing glTF
  * [jgltf-obj](https://github.com/javagl/JglTF/tree/master/jgltf-obj) - A library for converting OBJ to glTF
  * [jgltf-viewer](https://github.com/javagl/JglTF/tree/master/jgltf-viewer) - A viewer for glTF, with different implementations:
      * [jgltf-viewer-jogl](https://github.com/javagl/JglTF/tree/master/jgltf-viewer-jogl) - A glTF viewer based on [JOGL](http://jogamp.org/jogl/www/)
      * [jgltf-viewer-lwjgl](https://github.com/javagl/JglTF/tree/master/jgltf-viewer-lwjgl) - A glTF viewer based on [LWJGL version 2](http://legacy.lwjgl.org/)
  * [jgltf-browser](https://github.com/javagl/JglTF/tree/master/jgltf-browser) - An application combining the above libraries

#### Ada

| Tool | Status | Description |
|------|--------|-------------|
| [Orka](https://github.com/onox/orka) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | The OpenGL 4.5 Rendering Kernel in Ada |

#### TypeScript

| Tool | Status | Description |
|------|--------|-------------|
| [gltf-typescript-generator](https://github.com/robertlong/gltf-typescript-generator) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | CLI to generate a TypeScript interface for glTF |

### Utilities

* [gltf-utilities](https://github.com/AnalyticalGraphicsInc/gltf-utilities) - JavaScript utility library to help load glTF
* [wetzel](https://github.com/AnalyticalGraphicsInc/wetzel) - Generate Markdown documentation from JSON Schema

## Stack Overflow

* [glTF tagged](http://stackoverflow.com/questions/tagged/gltf) questions

## Presentations and Articles

### Intros

* [glTF 2.0 Launch](https://www.khronos.org/assets/uploads/developers/library/2017-web3d/glTF-2.0-Launch_Jun17.pdf) by Neil Trevett. June 2017
* **glTF Webinar** ([video](https://www.youtube.com/watch?v=KALedPvtFHY), [slides](https://www.khronos.org/assets/uploads/developers/library/2017-glTF-webinar/glTF-Webinar_Feb17.pdf)) by Marco Hutter. February 2017
* [glTF Brief](https://docs.google.com/presentation/d/1BRdEGqJFIWk3QOehOxJqM9dIE4kIBNQhIm7UeBaVse0/edit#slide=id.g185e245559_2_28) by Tony Parisi, FormVR and Amanda Watson, Oculus. October 2016

### Tutorials

* [glTF Tutorials](https://github.com/KhronosGroup/glTF-Tutorials)
* [Physically-Based Rendering: From Theory to glTF](https://github.com/moneimne/glTF-Tutorials/tree/master/PBR)

### All Presentations and Articles

* [State of glTF 2.0 for WebVR Devs](https://gist.github.com/donmccurdy/de7ff6c44ecd76fddf1ecad170a114a8) by Don McCurdy, Google. September 2017
* [glTF Exporter in three.js and A-Frame](https://blog.mozvr.com/gltf-exporter-in-three-js-and-a-frame/) by Fernando Serrano. August 2017
* SIGGRAPH 2017 glTF BOF [video](https://www.youtube.com/watch?v=UMN6rh-5nPg). August 2017
   * glTF 2.0 and Community Update: Tony Parisi, Unity, and Patrick Cozzi, Cesium
   * Microsoft update - Paint 3D, View 3D, 3D in Office and one more thing…: Saurabh Bhatia and Gary Hsu, Microsoft
   * glTF VSCode editor: Ed Mackey, AGI
   * Autodesk Forge and glTF: Nop Jiarathanakul, Autodesk
   * Three.js and A-Frame update: Don McCurdy, Google and Ricardo Cabello, Google
   * Introduction to geometry compression on the web with Draco: Michael Hemmer and Jamieson Brettle, Google
   * WebGL PBR reference implementation: Scott Nagy, Microsoft, and Mohamad Moneimne, Cesium
   * Sketchfab update: Aurelien Chatelain, Sketchfab
   * [glTF 2.0 Export in InstantUV](http://max-limper.de/publications/GLTF2017/GLTF2017.pdf): Max Limper, Fraunhofer
* [Physically-Based Rendering in Cesium](https://cesium.com/blog/2017/08/08/Physically-Based-Rendering-in-Cesium/) by Mohamad Moneimne. August 2017
* [Why we should all support glTF 2.0 as THE standard asset exchange format for game engines](https://godotengine.org/article/we-should-all-use-gltf-20-export-3d-assets-game-engines) by Juan Linietsky. August 2017
* [Exporting glTF 2.0 from Maya LT](https://www.donmccurdy.com/2017/06/27/exporting-gltf-2-0-from-maya-lt-2/) by Don McCurdy. June 2017
* [glTF 2.0: PBR Materials](https://www.khronos.org/assets/uploads/developers/library/2017-gtc/glTF-2.0-and-PBR-GTC_May17.pdf) by Saurabh Bhatia. May 2017
* [glTF Workflow for a Saturday Night](https://blog.mozvr.com/a-saturday-night-gltf-workflow/) by Diego F. Goberna. April 2017
* [2017 GDC WebGL/WebVR/glTF Meetup](https://www.khronos.org/developers/library/2017-gdc-webgl-webvr-gltf-meetup) YouTube recording. March 2017
* [Reach the Largest Gaming Platform of All: The Web. WebGL, WebVR and glTF](https://www.youtube.com/watch?v=jLNtjujPhzY). March 2017
* [PBR-ready glTF in instant3Dhub / instantUV](http://max-limper.de/publications/PBR2017/PBR2017.pdf) by Max Limper. March 2017
* [Call for feedback on glTF 2.0](https://www.khronos.org/blog/call-for-feedback-on-gltf-2.0) by Neil Trevett. February 2017
* [Improve expressiveness of WebGL with the topic 3D file format glTF now! (in Japanese)](http://qiita.com/emadurandal/items/1a034c4addd7ff8b5184) by Yuki Shimada(@emadurandal), WebGL advent calendar 2016 at Qiita. December 2016
* [A new 3D horizon: glTF import and export with Archilogic](https://spaces.archilogic.com/blog/gltf-import-export) by Martin Splitt. December 2016
* [Bringing 3D to everyone through open standards](https://blogs.windows.com/buildingapps/2016/10/28/bringing-3d-to-everyone-through-open-standards/) by Forest W. Gouin and Jean Paoli. October 2016
* [Using Quantization with 3D Models](http://cesiumjs.org/2016/08/08/Cesium-web3d-quantized-attributes/) by Rob Taglang. August 2016
* [glTF and Mobile VR: Inclusive standards for a 3D world](https://www.khronos.org/assets/uploads/developers/library/2016-siggraph/glTF-MobileVR-Oculus-BOF-Update-SIGGRAPH_Jul16.pdf). Amanda Watson, Oculus, WebGL + glTF BOF. July 2016
* [glTF Update and Roadmap](https://www.khronos.org/assets/uploads/developers/library/2016-siggraph/glTF-BOF-Update-SIGGRAPH_Jul16.pdf). Tony Parisi, WebGL + glTF BOF. July 2016
* [PBR in glTF: Current State](https://www.khronos.org/webgl/wiki_1_15/images/2016-07-28_WebGL_BOF_PBR.pdf). Max Limper, Johannes Behr, and Timo Sturm, WebGL + glTF BOF. July 2016
* [glTF: The Runtime Asset Format for GL-based Applications](https://www.khronos.org/assets/uploads/developers/library/2016-siggraph/glTF-Background-Briefing_Jul16.pdf). July 2016
* [glTF working group updates](https://www.khronos.org/assets/uploads/developers/library/2016-gdc/glTF-GDC_Mar16.pdf) ([slides](https://www.khronos.org/assets/uploads/developers/library/2016-gdc/glTF-GDC_Mar16.pdf), [video](https://www.youtube.com/watch?v=qAjWiVfR5Nw&t=43m40s)). Patrick Cozzi and Tony Parisi, WebGL + glTF BOF. March 2016
* [FBX to/from glTF](https://www.khronos.org/assets/uploads/developers/library/2016-gdc/Autodesk-FBX-glTF-WebGL_Mar16.pdf) ([slides](https://www.khronos.org/assets/uploads/developers/library/2016-gdc/Autodesk-FBX-glTF-WebGL_Mar16.pdf), [video](https://www.youtube.com/watch?v=qAjWiVfR5Nw&t=59m08s)). Cyrille Fauvel, WebGL + glTF BOF. March 2016
* [Khronos Group glTF Webinar](https://www.youtube.com/watch?v=YXPeh2hy6Tc). Neil Trevett, Virtual AR Community meeting. October 2015
* [An Introduction to glTF 1.0](https://www.khronos.org/assets/uploads/developers/library/overview/glTF-1.0-Introduction-Oct15.pdf). October 2015
* [The state of WebGL and glTF](https://www.khronos.org/assets/uploads/developers/library/2015-graphical-web/WebGL-and-glTF-Graphical-Web_Sep15.pdf). Patrick Cozzi, The Graphical Web. September 2015
* [glTF ecosystem and mesh compression update](https://www.khronos.org/webgl/wiki_1_15/images/GlTF_Update_SIGGRAPH_Aug15.pdf). Khronos 3D Formats Working Group, SIGGRAPH 2015. August 2015
* [glTF and the WebGL Art Pipeline](http://www.slideshare.net/auradeluxe/gltf-and-the-webgl-art-pipeline-march-2015). Tony Parisi, WebGL Meetup. March 2015
* [Writing an FBX importer / Exporter plug-in](http://around-the-corner.typepad.com/adn/2015/01/writing-an-fbx-importer-exporter-plug-in.html). Cyrille Fauvel. January 2015
* [glTF Tips for Artists](http://cesiumjs.org/2014/12/15/glTF-Tips-for-Artists/). Branden Coker. December 2014
* [3D for the Modern Web: Declarative 3D and glTF](http://mason.gmu.edu/~bcoughl2/cs752/). Brian Coughlin. Summer 2014
* [glTF: Designing an Open-Standard Runtime Asset Format](https://books.google.com/books?id=uIDSBQAAQBAJ&pg=PA375&lpg=PA375&dq=%22Designing+an+Open-Standard+Runtime+Asset+Format%22&source=bl&ots=XLLQ_9piKe&sig=rwLmjPbxN3p5LMYBzf-LGoAJtgs&hl=en&sa=X&ved=0CCkQ6AEwAmoVChMI5bTKlJ3MyAIVBqMeCh012ggk). Fabrice Robinet et al, GPU Pro 5. May 2014
* [Building a WebGL Santa with Cesium and glTF](http://cesiumjs.org/2013/12/23/Building-A-WebGL-Santa-with-Cesium-and-glTF/). Patrick Cozzi. December 2013
* [glTF update](http://www.slideshare.net/auradeluxe/gltf-update-with-tony-parisi). Tony Parisi. August 2013
* [How I got involved in glTF and Khronos](http://blog.virtualglobebook.com/2013/03/how-i-got-involved-in-gltf-and-khronos.html). Patrick Cozzi, WebGL Meetup. March 2013

---

We believe the true usefulness of glTF goes beyond the spec itself; it is an ecosystem of tools, documentation, and extensions contributed by the community.  You are encouraged to [get involved](https://github.com/KhronosGroup/glTF/issues/456)!
