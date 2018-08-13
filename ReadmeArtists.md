# Artists

[Blender exporter](https://github.com/KhronosGroup/glTF-Blender-Exporter) for creating and converting models with glTF 2.0.

Preview tools:
  - [Sketchfab](https://sketchfab.com/)
  - [BabylonJS Sandbox](https://www.babylonjs.com/sandbox/)
  - [Drag-and-drop viewer](https://gltf-viewer.donmccurdy.com/)

## Contents

* [glTF Tools](#gltf-tools)
  * [Converters and Exporters](#converters-and-exporters)
  * [Editors and Modeling Tools](#editors-and-modeling-tools)
  * [Viewers and Debugging](#viewers-and-debugging)
  * [Applications](#applications)
  * [Loaders and Viewers](#loaders-and-viewers)
    * [WebGL Engines](#webgl-engines)
    * [Game Engines](#game-engines)
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

### Editors and Modeling Tools

| Tool | Status | Description |
|------|--------|-------------|
| [glTF VSCode Extension](https://marketplace.visualstudio.com/items?itemName=cesium.gltf-vscode) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Cross-platform JSON schema validation and previews for glTF 2.0 and 1.0 |
| [Blender Importer](https://github.com/ksons/gltf-blender-importer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Blender importer for glTF 2.0 (alpha) |
| [Blender Importer](https://github.com/julienduroure/gltf2-blender-importer) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | glTF 2.0 Blender Importer |

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

### Applications

| Application | Status | Description |
|-------------|--------|-------------|
| [Paint 3D](https://www.microsoft.com/en-us/store/p/paint-3d/9nblggh5fv99) by Microsoft | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | 3D creation tool |
| [Remix 3D](https://www.remix3d.com) by Microsoft | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Share and print 3D creations |
| [Facebook](https://developers.facebook.com/docs/sharing/3d-posts) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Use .glb files to create 3D Posts |
| [Sketchfab](https://blog.sketchfab.com/sketchfab-now-supports-gltf/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Platform to publish, share & discover 3D online and in VR |
| [Wordpress](https://wordpress.org/plugins/gltf-media-type/) (plugin) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Inline glTF renderer for WordPress |
| [Modo](https://www.foundry.com/products/modo) by Foundry | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | 3D modeling, texturing & rendering tools |
| [Substance Painter](https://www.allegorithmic.com/products/substance-painter) by Allegorithmic | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | 3D painting software with glTF 2.0 export |
| [Marmoset Toolbag](https://www.marmoset.co/toolbag/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | 3D real-time rendering, animation, and baking suite |
| [Aspose.3D](https://www.aspose.com/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | File Format APIs for creating, editing, and converting over 100 file formats |
| [3D Slash](http://www.3dslash.net) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Web-based and app-based 3D modeling imitating a stonecutter |
| [Archilogic](https://spaces.archilogic.com/blog/gltf-import-export) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Web-based 3D platform for architecture and interiors |
| [Plex.Earth](https://plexearth.com/) | ![status](https://img.shields.io/badge/glTF-1%2E0-yellow.svg?style=flat) | Connects AutoCAD with Google Earth and the Real World. |

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

#### Game Engines

| Tool | Status | Description |
|------|--------|-------------|
| [UnityGLTF](https://github.com/KhronosGroup/UnityGLTF) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Unity3D library for exporting, loading, parsing, and rendering glTF assets |
| [Godot Game Engine](https://godotengine.org/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | Godot 3.0 fully supports glTF import |
| [jMonkeyEngine](http://jmonkeyengine.org/) | ![status](https://img.shields.io/badge/glTF-2%2E0-green.svg?style=flat) | jME 3.2 supports glTF 2.0 |

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

We believe the true usefulness of glTF goes beyond the spec itself; it is an ecosystem of tools, documentation, and extensions contributed by the community.  You are encouraged to [get involved](https://github.com/KhronosGroup/glTF/issues/456)!
