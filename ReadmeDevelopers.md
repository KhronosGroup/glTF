# Developers

* [Sample models](https://github.com/KhronosGroup/glTF-Sample-Models) for testing runtime engines and content pipeline tools.
   * [glTF-Asset-Generator](https://github.com/bghgary/glTF-Asset-Generator) provides assets for robust importer validation.
   * [100,000+ models](https://sketchfab.com/models?features=downloadable&sort_by=-likeCount) under Creative Commons license on Sketchfab (check license for individual models).
* [Drag-and-drop validator](http://github.khronos.org/glTF-Validator/) for verifying correctness of existing glTF files.
* [glTF Tutorial Series](https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/README.md)

## Contents

* [glTF Tools](#gltf-tools)
  * [Converters and Exporters](#converters-and-exporters)
  * [Optimizers](#optimizers)
  * [Validators](#validators)
  * [Viewers and Debugging](#viewers-and-debugging)
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
| [3DS Max Exporter](http://doc.babylonjs.com/resources/3dsmax_to_gltf) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Export glTF files using [BabylonJS plugin](http://doc.babylonjs.com/resources/3dsmax#how-to-install-the-3ds-max-plugin) for 3DS Max 2015 or later |
| [Unity Exporter (Sketchfab fork)](https://github.com/sketchfab/Unity-glTF-Exporter) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Unity editor wizard that exports to glTF Format |
| Unity COLLADA & glTF Exporter [Unity 5](https://www.assetstore.unity3d.com/#!/content/40946) & [Unity 2017](https://www.assetstore.unity3d.com/#!/content/99793) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Commercial unity asset store editor extension to export COLLADA and glTF via collada2gltf |
| Sketchup [export](http://extensions.sketchup.com/en/content/gltf-exporter) and [import](http://extensions.sketchup.com/en/content/gltf-import) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Separate extensions for export and import |
| [PBR extension for SketchUp](https://github.com/SamuelTS/SketchUp-PBR-Plugin) | `SKP` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | PBR Material Editor, glTF Exporter, PBR Viewport |
| [obj2gltf](https://github.com/AnalyticalGraphicsInc/OBJ2GLTF)  | `OBJ` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Command-line converter |
| [blendergltf](https://github.com/Kupoman/blendergltf) | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF exporter for Blender aimed at streaming scenes to other applications |
| [BlackThread.io converter](https://blackthread.io/gltf-converter/) | [Multiple](https://blackthread.io/gltf-converter/#supported-formats) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Online drag and drop converter powered by three.js |
| [Assimp](http://www.assimp.org/) | [Multiple](https://github.com/assimp/assimp#supported-file-formats) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | General-purpose online conversion pipeline |
| [PiXYZ Studio](https://www.pixyz-software.com/studio/) | [Multiple](https://docs.google.com/spreadsheets/d/1aWdeBOvZsbnwHfbyhuVS4vBDwevsT2Bz65hEhc0CtnU/pubhtml?gid=786960864) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Interactively prepare & optimize any CAD data |
| [FBX2glTF (Facebook)](https://github.com/facebookincubator/FBX2glTF)  | `FBX` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Command-line converter |
| [FBX2glTF (ClayGL)](https://github.com/pissang/clay-viewer#converter)  | `FBX`, `COLLADA`, `OBJ` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Python tool to convert models to glTF2.0 format, model is parsed by FBX SDK. |
| [glTF API](https://github.com/headjack-dev/gltf-api)  | `FBX`, `COLLADA`, `OBJ` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Web API to convert OBJ, FBX and COLLADA files to glTF or GLB |
| [Simplygon Cloud](https://azuremarketplace.microsoft.com/en-us/marketplace/apps/simplygon.simplygon)  | [Multiple](https://simplygonclouddoc.simplygon.com/tutorials/firstprocessing.html#asset) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cloud based asset optimization pipeline |
| [Verge3D](http://www.soft8soft.com/verge3d/)  | — | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF 2.0 exporters for Blender and 3ds Max |
| [COLLADA2GLTF](https://github.com/KhronosGroup/COLLADA2GLTF/) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Official command-line converter, with updates for glTF 2.0 in progress |
| [Cesium drag-and-drop converter](http://cesiumjs.org/convertmodel.html) | `OBJ`, `COLLADA` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Online drag and drop converter |
| [batchgltf](https://github.com/feiss/batchgltf) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Batch converter with GUI |
| [FBX-glTF (Cyrillef)](https://github.com/cyrillef/FBX-glTF) | `FBX` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Plug-in and command-line converter |
| [jgltf-obj](https://github.com/javagl/JglTF/tree/master/jgltf-obj) | `OBJ` | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Java programmatic and command-line converters |
| [Docker collada2gltf](https://hub.docker.com/r/winsent/collada2gltf/) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Docker container for Python web service, built on COLLADA2GLTF |
| [collada2gltf-web-service](https://github.com/AnalyticalGraphicsInc/collada2gltf-web-service) | `COLLADA` | ![status](https://img.shields.io/badge/glTF-%3F-lightgrey.svg?style=flat) | Node.js web service, built on COLLADA2GLTF |
| [cqparts](https://github.com/fragmuffin/cqparts) | `STEP` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Python CAD for programmers ([import / export doc](https://fragmuffin.github.io/cqparts/doc/cqparts/import-export.html)) ([GLTFExporter](https://fragmuffin.github.io/cqparts/doc/api/cqparts.codec.html#cqparts.codec.gltf.GLTFExporter)) | |
[gltf-to-usdz](https://github.com/TimvanScherpenzeel/gltf-to-usdz) | `USDZ` | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF to USDZ Converter |

### Optimizers

| Tool | Status | Description |
|------|--------|-------------|
| [gltf-import-export](https://github.com/najadojo/gltf-import-export) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | NPM package to convert between glb and gltf files |
| [makeglb](https://github.com/sbtron/makeglb) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Drag and drop browser tool to convert gltf to self contained binary glb |
| [gltf-toolkit](https://github.com/microsoft/gltf-toolkit) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Collection of tools and libraries to modify and optimize glTF assets for Windows Mixed Reality home |
| [glTF Pipeline](https://github.com/AnalyticalGraphicsInc/gltf-pipeline) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Official Node.js command-line tool for optimizing glTF assets |
| [gltf2glb](https://github.com/Geopipe/gltf2glb) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Python tool to convert glTF to Binary glTF |
| [binary-gltf-utils](https://github.com/Qantas94Heavy/binary-gltf-utils) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Node.js tool to convert glTF to Binary glTF |


### Validators

| Tool | Status | Description |
|------|--------|-------------|
| [glTF Validator](https://github.com/KhronosGroup/glTF-Validator) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Official command-line and drag-and-top tool to validate glTF assets against the specification |

### Editors

| Tool | Status | Description |
|------|--------|-------------|
| [glTF VSCode Extension](https://marketplace.visualstudio.com/items?itemName=cesium.gltf-vscode) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cross-platform JSON schema validation and previews for glTF 2.0 and 1.0 ||

### Viewers and Debugging

| Tool | Status | Description |
|------|--------|-------------|
| [BabylonJS Sandbox](https://www.babylonjs.com/sandbox/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Drag-and-drop online viewer for model preview and debugging, using BabylonJS |
| [three.js glTF Viewer](https://gltf-viewer.donmccurdy.com/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Drag-and-drop viewer (web and [desktop](https://github.com/donmccurdy/three-gltf-viewer#gltf-viewer)) for model preview and debugging, using three.js |
| [three.js Editor](http://threejs.org/editor) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Editor with drag-and-drop support |
| [Clay Viewer](https://pissang.github.io/clay-viewer/editor/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Drag-and-drop glTF2.0 viewer with high quality rendering and PBR material editing. |
| [glTF Animation Visualizer](https://bengfarrell.github.io/animation-visualizer/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Frame-by-frame debugging for glTF animations. |
| [VirtualGIS Viewer](https://www.virtualgis.io/gltfviewer/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cesium-based glTF viewer. |
| [GLTFQuickLook](https://github.com/magicien/GLTFQuickLook) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | macOS QuickLook plugin for glTF files |
| [glTF Viewer](https://www.8thwall.com/gltf/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Android and iOS apps for glTF 2.0 viewing and link sharing: supports embedded glTF 2.0 files and links |
| [DirectX glTF Viewer](https://github.com/Microsoft/glTF-DXViewer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | DirectX11, C++ desktop app for loading and rendering glTF files |

### Loaders and Viewers

#### WebGL Engines

To compare WebGL-based glTF loaders, see [gltf-test](https://github.com/cx20/gltf-test).

| Engine | Status | Scope | Related |
|--------|--------|-------|-------------|
| [three.js](https://threejs.org/) ([loader](https://threejs.org/docs/index.html#examples/loaders/GLTFLoader)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [`<gltf-model/>` component](https://github.com/mrdoob/model-tag) |
| [BabylonJS](https://www.babylonjs.com/) ([loader](https://github.com/BabylonJS/Babylon.js/tree/master/loaders/src/glTF)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | |
| [Cesium](http://cesiumjs.org/) ([loader](https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Scene/Model.js)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Drag-and-drop viewer](https://www.virtualgis.io/gltfviewer/), [tutorial](https://cesiumjs.org/tutorials/3D-Models-Tutorial/) |
| [OSG.JS](http://osgjs.org/) ([loader](https://github.com/cedricpinson/osgjs/blob/master/sources/osgPlugins/ReaderWriterGLTF.js)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | |
| [Grimoire.js](https://github.com/GrimoireGL/GrimoireJS) ([loader](https://github.com/GrimoireGL/grimoirejs-gltf)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Geometry and materials, partial animation | |
| [Hilo3d](https://github.com/hiloteam/Hilo3d) ([loader](https://github.com/hiloteam/Hilo3d/blob/master/src/loader/GLTFLoader.js)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Doc](https://hiloteam.github.io/Hilo3d/docs/) |
| [A-Frame](https://aframe.io/) ([loader](https://aframe.io/docs/0.6.0/components/gltf-model.html)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Doc](https://aframe.io/docs/0.7.0/components/gltf-model.html) |
| [xeogl](http://xeogl.org/) ([loader](http://xeogl.org/examples/#importing_gltf_BoomBox)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Geometry and materials | [Tutorial](http://xeogl.org/docs/classes/GLTFModel.html) |
| [ClayGL](http://claygl.xyz/) ([loader](http://docs.claygl.xyz/api/clay.application.App3D.html#loadModel)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Examples](http://examples.claygl.xyz/) |
| [Verge3D](http://www.soft8soft.com/verge3d/) ([loader](https://www.soft8soft.com/docs/examples/loaders/GLTFLoader.html)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | |
| [ReactVR](https://facebook.github.io/react-vr/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | |
| [CZPG](https://github.com/PrincessGod/CraZyPG) ([loader](https://github.com/PrincessGod/CraZyPG/blob/master/src/loaders/GLTFLoader.js)) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | All | [Examples](http://princessgod.github.io/CraZyPG) |
| [PEX](http://vorg.github.io/pex/) ([loader](https://github.com/pex-gl/pex-gltf)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Geometry and materials| |
| [GLBoost](https://github.com/emadurandal/GLBoost) ([loader](https://github.com/emadurandal/GLBoost/blob/master/src/js/middle_level/loader/GLTFLoader.js)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | ? | [Examples](https://gitcdn.xyz/repo/emadurandal/GLBoost/master/examples/index.html) |
| [xml3d.js](http://xml3d.org) ([loader](https://github.com/xml3d/xml3d-gltf-plugin)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Geometry and materials | |
| [X3DOM](http://x3dom.org/) ([loader](https://github.com/x3dom/x3dom/blob/master/src/util/glTF/glTFLoader.js)) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Geometry and materials | |

#### WebGL Sample Code

| Sample | Status | Description |
|--------|--------|-------------|
| [Sample glTF loader with WebGL PBR shader](http://github.khronos.org/glTF-WebGL-PBR/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Engine-agnostic reference implementation for PBR shading with WebGL in glTF 2.0 ([source code](https://github.com/KhronosGroup/glTF-WebGL-PBR)) |


### Languages

#### JavaScript / Node.js

| Tool | Status | Description |
|------|--------|-------------|
| [gltf-viewer](https://github.com/avgp/gltf-viewer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Web component to display glTF models on a website |
| [minimal-gltf-loader](https://github.com/shrekshao/minimal-gltf-loader) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A minimal, engine-agnostic JavaScript glTF Loader, with a raw WebGL 2 renderer example |
| [THREE.GLTFLoader](https://threejs.org/docs/#examples/loaders/GLTFLoader) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Loads glTF assets into a three.js scene. |
| [THREE.GLTFExporter](https://threejs.org/docs/#examples/exporters/GLTFExporter) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Exports three.js scenes to a glTF asset |
| [gltf-walker](https://github.com/ksons/gltf-walker) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Convenience library for processing glTF |
| [RedCube.js](https://github.com/Reon90/redcube) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | glTF viewer without dependencies |
| [gltf-bounding-box](https://github.com/wanadev/gltf-bounding-box) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Computes the global bounding box of a gltf model |

#### C++

| Tool | Status | Description |
|------|--------|-------------|
| [Tiny glTF loader](https://github.com/syoyo/tinygltf) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat)  | Header only C++ glTF parsing library, with [Alembic→glTF](https://github.com/syoyo/tinygltfloader/tree/master/examples/alembic_to_gltf) and [CyHair->glTF](https://github.com/syoyo/tinygltfloader/tree/master/examples/cyhair_to_gltf) converters |
| [yocto-gltf](https://github.com/xelatihy/yocto-gl) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Single file, header only, C++ glTF loading/writing automatically generated from the spec |
| [glTF2-loader](https://github.com/Lugdunum3D/glTF2-loader/tree/dev) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | C++14 glTF (2.0 only) parsing library, by Lugdunum3D |
| [gltfpp](https://github.com/mmha/gltfpp) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF 2.0 loader for C++14 |
| [fx-gltf](https://github.com/jessey-git/fx-gltf) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A C++14/C++17 header-only library for simple, efficient, and robust serialization/deserialization of glTF 2.0 |
| [TGen](https://github.com/mlimper/tgen/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Simple C++ Tangent Generator |
| [Microsoft.glTF.CPP](https://www.nuget.org/packages/Microsoft.glTF.CPP/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) |  A C++ library for serializing and deserializing gltf/glb files. | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | |
| [Qt 3D](http://doc.qt.io/qt-5.6/qt3d-gltf-example.html) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Qt 3D provides functionality for near-realtime simulation systems |
| [Laugh Engine](https://github.com/jian-ru/laugh_engine#laugh-engine) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Vulkan PBR and IBL renderer |
| [Lugdunum3D](https://github.com/Lugdunum3D/Lugdunum) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A modern cross-platform 3D rendering engine built with Vulkan and modern C++14 |
| [UX3D Engine](http://www.ux3d.io/ux3d-engine/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cross-platform multi-threaded Vulkan 3D Engine with glTF 2.0 import and export |
| [Cinder](http://discourse.libcinder.org/t/gltf-initial-release-wip/212) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Work-in-progress glTF importer |
| [nvpro-pipeline](https://github.com/nvpro-pipeline/pipeline) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Research rendering pipeline by NVIDIA for minimizing CPU cost |
| [AssetKit](https://github.com/recp/assetkit) | ? | 3D asset importer/exporter/util library |
| [libgltf](https://github.com/code4game/libgltf) | ![status](https://img.shields.io/badge/glTF-2%2E0-yellow.svg?style=flat) | glTF 2.0 parser for C++11 |
| [libgltf_ue4](https://github.com/code4game/libgltf_ue4) | ![status](https://img.shields.io/badge/glTF-2%2E0-yellow.svg?style=flat) | A third-party library for UE4.10+ to load glTF files |
| [glTF for UE4](https://github.com/code4game/glTFForUE4) | ![status](https://img.shields.io/badge/glTF-2%2E0-yellow.svg?style=flat) | A plugin for UE4.10+ to import glTF files |

#### Objective-C

| Tool | Status | Description |
|------|--------|-------------|
| [GLTFKit](https://github.com/warrenm/GLTFKit) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | An Objective-C glTF 2.0 loader and Metal-based renderer |

#### C#

| Tool | Status | Description |
|------|--------|-------------|
| [C# glTF loader](https://github.com/KhronosGroup/glTF-CSharp-Loader) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | C# reference loader for glTF |
| [Aspose.3D for .NET](http://www.aspose.com/products/3d/net) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat)| Import, export, and convert glTF |
| [MixedRealityToolkit-Unity](https://github.com/Microsoft/MixedRealityToolkit-Unity) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A collection of scripts and components intended to accelerate development of applications targeting Microsoft HoloLens and Windows Mixed Reality headsets. |

#### C

| Tool | Status | Description |
|------|--------|-------------|
| [AssetKit](https://github.com/recp/assetkit) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | 3D asset importer, exporter library. |

#### Go

| Tool | Status | Description |
|------|--------|-------------|
| [gltf](https://github.com/sturfeeinc/glTF) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | go library for marshaling and unmarshaling glTF |

#### Rust

| Tool | Status | Description |
|------|--------|-------------|
| [Loader](https://github.com/gltf-rs/gltf) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | A crate for loading glTF 2.0 |
| [Viewer](https://github.com/bwasty/gltf-viewer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF 2.0 Viewer written in Rust |

#### Haxe

| Tool | Status | Description |
|------|--------|-------------|
| [haxe-gltf](https://github.com/FuzzyWuzzie/haxe-gltf) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | A Haxe library for reading glTF |

#### Java

* [JglTF](https://github.com/javagl/JglTF) - Java libraries for glTF
  * jgltf-impl - Classes representing a glTF model, auto-generated from the glTF JSON schema
    * [jgltf-impl-v1](https://github.com/javagl/JglTF/tree/master/jgltf-impl-v1) - glTF 1.0
    * [jgltf-impl-v2](https://github.com/javagl/JglTF/tree/master/jgltf-impl-v2) - glTF 2.0
    * [jgltf-impl-v2-technique-webgl](https://github.com/javagl/JglTF/tree/master/jgltf-impl-v2-technique-webgl) - KHR_technique_webgl extension
  * [jgltf-model](https://github.com/javagl/JglTF/tree/master/jgltf-model) - Classes for reading, processing, converting and writing glTF
  * [jgltf-obj](https://github.com/javagl/JglTF/tree/master/jgltf-obj) - Library for converting OBJ to glTF
  * [jgltf-viewer](https://github.com/javagl/JglTF/tree/master/jgltf-viewer) - Viewer for glTF, with different implementations:
      * [jgltf-viewer-jogl](https://github.com/javagl/JglTF/tree/master/jgltf-viewer-jogl) - glTF viewer based on [JOGL](http://jogamp.org/jogl/www/)
      * [jgltf-viewer-lwjgl](https://github.com/javagl/JglTF/tree/master/jgltf-viewer-lwjgl) - glTF viewer based on [LWJGL version 2](http://legacy.lwjgl.org/)
  * [jgltf-browser](https://github.com/javagl/JglTF/tree/master/jgltf-browser) - Application combining the above libraries
  * [jgltf-validator](https://github.com/javagl/JglTF/tree/master/jgltf-validator) - Simple glTF validator, only intended for internal use

#### Ada

| Tool | Status | Description |
|------|--------|-------------|
| [Orka](https://github.com/onox/orka) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | The OpenGL 4.5 Rendering Kernel in Ada |

#### TypeScript

| Tool | Status | Description |
|------|--------|-------------|
| [gltf-typescript-generator](https://github.com/robertlong/gltf-typescript-generator) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | CLI to generate a TypeScript interface for glTF |

#### Swift

| Tool | Status | Description |
|------|--------|-------------|
| [GLTFSceneKit](https://github.com/magicien/GLTFSceneKit) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF loader for SceneKit |

### Utilities

* [gltf-utilities](https://github.com/AnalyticalGraphicsInc/gltf-utilities) - JavaScript utility library to help load glTF
* [wetzel](https://github.com/AnalyticalGraphicsInc/wetzel) - Generate Markdown documentation from JSON Schema
* [gltf-enum](http://gltf-enum.com/) - Simple site to help work with all the different enums in glTF spec
* [gltf-SDK](https://github.com/Microsoft/glTF-SDK/) -  A C++ Deserializer/Serializer for glTF compatible with Windows, macOS and Android

## Stack Overflow

* [glTF tagged](http://stackoverflow.com/questions/tagged/gltf) questions
---

We believe the true usefulness of glTF goes beyond the spec itself; it is an ecosystem of tools, documentation, and extensions contributed by the community.  You are encouraged to [get involved](https://github.com/KhronosGroup/glTF/issues/456)!
