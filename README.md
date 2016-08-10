<p align="center">
<img src="specification/figures/gltf.png" />
</p>

The GL Transmission Format (glTF) is a runtime asset delivery format for GL APIs: WebGL, OpenGL ES, and OpenGL.  glTF bridges the gap between 3D content creation tools and modern GL applications by providing an efficient, extensible, interoperable format for the transmission and loading of 3D content.

## Specification

* [glTF Specification, 1.0](specification/README.md)
* [glTF Extension Registry](extensions/README.md)

Please provide feedback by submitting [issues](https://github.com/KhronosGroup/glTF/issues).

## Sample Models

[Sample models](sampleModels/README.md) for learning glTF, and testing runtime engines and content pipeline tools.

## Overview

<p align="center">
<a href="https://raw.githubusercontent.com/KhronosGroup/glTF/master/specification/figures/gltfOverview-0.2.0.png"><img src="specification/figures/gltfOverview-0.2.0-small.png" /></a>
</p>
Diagram by [Marco Hutter](http://marco-hutter.de/) ([repo](https://github.com/javagl/gltfOverview)).

## glTF Tools

### Converters

* [COLLADA2GLTF](https://github.com/KhronosGroup/glTF/wiki/converter) - Command-line tool to convert COLLADA to glTF
* [COLLADA to glTF](http://cesiumjs.org/convertmodel.html) - online drag and drop converter
* [Assimp](http://www.assimp.org/) - General-purpose conversion pipeline with a glTF importer/exporter pipeline. See the [list of supported import formats](https://github.com/assimp/assimp#supported-file-formats).
* [obj2gltf](https://github.com/AnalyticalGraphicsInc/OBJ2GLTF) - Command-line tool to convert OBJ to glTF
* [FBX-glTF](https://github.com/cyrillef/FBX-glTF) - FBX glTF importer/exporter plug-in and converter
* [osm2cesium](https://github.com/stirringhalo/osm2cesium) - Extracts buildings in OpenStreetMap and converts them to glTF
* [Docker collada2gltf](https://hub.docker.com/r/winsent/collada2gltf/) - Docker container of Python web service to convert COLLADA to glTF
* [binary-gltf-utils](https://github.com/Qantas94Heavy/binary-gltf-utils) - Node.js tool to convert glTF to Binary glTF
* [collada2gltf-web-service](https://github.com/AnalyticalGraphicsInc/collada2gltf-web-service) - Simple Node.js web service built on COLLADA2GLTF
* [collada2gltf](https://www.npmjs.com/package/collada2gltf) - Node.js wrapper around COLLADA2GLTF
* [colladaToBglTFConverter](https://github.com/virtualcitySYSTEMS/colladaToBglTFConverter) - Groovy/JavaScript tool to convert COLLADA to Binary glTF (0.8 / CESIUM_binary_glTF)

### Validators

* [glTF Validator](https://github.com/KhronosGroup/glTF-Validator) - open-source command-line and drag-and-top tool to validate glTF assets against the spec

### Exporters

* [Blender](https://github.com/Kupoman/blendergltf) - work in progress

### Loaders and Viewers

#### WebGL Engines

* [glTF loader](https://github.com/mrdoob/three.js/tree/master/examples/js/loaders/gltf) in [Three.js](http://threejs.org/)
* [glTF loader](https://github.com/BabylonJS/Babylon.js/tree/master/loaders/glTF) in [BabylonJS](http://babylonjs.com/)
* [glTF loader](https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Scene/Model.js) in [Cesium](http://cesiumjs.org/)
   * The [COLLADA to glTF](http://cesiumjs.org/convertmodel.html) converter also supports drag-and-drop viewing of glTF assets
   * [3D Models](https://cesiumjs.org/tutorials/3D-Models-Tutorial/) tutorial
* Initial [glTF loader](https://github.com/pex-gl/pex-gltf) in [PEX](http://vorg.github.io/pex/) (geometry and materials)
* Initial [glTF loader](https://github.com/xeolabs/xeoengine/tree/master/src/importing/gltf) in [xeoEngine](http://xeoengine.org/) (geometry and materials)
   * [Importing glTF](https://github.com/xeolabs/xeoengine/wiki/Importing-glTF) tutorial
* [aframe-gltf](https://github.com/aframevr/aframe) - loader for A-Frame, a framework for creating virtual reality web experiences

#### JavaScript / Node.js

* [gltf-walker](https://github.com/ksons/gltf-walker) - convenience library for processing glTF
* [rest3d](https://github.com/amd/rest3d) - serves glTF and other 3D assets via a REST API (glTF 0.8)
* [gltf-viewer-element](https://www.npmjs.com/package/gltf-viewer-element) - Node.js package for loading and rendering glTF (0.8)
* [Amobee 3D](http://amobee3d.s3.amazonaws.com/ads/Amobee3D_AdList.html) (0.8)

#### C++

* [nvpro-pipeline](https://github.com/nvpro-pipeline/pipeline) - research rendering pipeline by NVIDIA for minimizing CPU cost 
* [Tiny glTF loader](https://github.com/syoyo/tinygltfloader) - Header only C++ glTF parsing library
* [Cinder](http://discourse.libcinder.org/t/gltf-initial-release-wip/212) - Work-in-progress glTF importer
* [LibreOffice](http://zolnaitamas.blogspot.com/2014/08/3d-models-in-impress-libreoffice-43.html) (glTF 0.8)

#### C# #

* [C# glTF loader](https://github.com/KhronosGroup/glTF/tree/master/loaders/CSharp) - C# reference loader for glTF

#### Haxe

* [haxe-gltf](https://github.com/FuzzyWuzzie/haxe-gltf) - A Haxe library for reading glTF

### Utilities

* [gltf-utilities](https://github.com/AnalyticalGraphicsInc/gltf-utilities) - JavaScript utility library to help load glTF
* [gltf-statistics](https://github.com/AnalyticalGraphicsInc/gltf-statistics) - JavaScript and Node.js library and command-line tool to display statistics for glTF models (glTF 0.8)

## Presentations and Articles

* [glTF: The Runtime Asset Format for GL-based Applications](https://www.khronos.org/assets/uploads/developers/library/2016-siggraph/glTF-Background-Briefing_Jul16.pdf). July 2016
* [Using Quantization with 3D Models](http://cesiumjs.org/2016/08/08/Cesium-web3d-quantized-attributes/) by Rob Taglang. August 2016
* [glTF and Mobile VR: Inclusive standards for a 3D world](https://www.khronos.org/assets/uploads/developers/library/2016-siggraph/glTF-MobileVR-Oculus-BOF-Update-SIGGRAPH_Jul16.pdf). Amanda Watson, Oculus, WebGL + glTF BOF. July 2016
* [glTF Update and Roadmap](https://www.khronos.org/assets/uploads/developers/library/2016-siggraph/glTF-BOF-Update-SIGGRAPH_Jul16.pdf). Tony Parisi, WebGL + glTF BOF. July 2016
* [PBR in glTF: Current State](https://www.khronos.org/webgl/wiki_1_15/images/2016-07-28_WebGL_BOF_PBR.pdf). Max Limper, Johannes Behr, and Timo Sturm, WebGL + glTF BOF. July 2016
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
