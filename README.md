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

## glTF Tools

### Converters

* [COLLADA2GLTF](https://github.com/KhronosGroup/glTF/wiki/converter) - Command-line tool to convert COLLADA to glTF
* [COLLADA to glTF](http://cesiumjs.org/convertmodel.html) - online drag and drop converter
* [Assimp](http://www.assimp.org/) - General-purpose conversion pipeline with a glTF importer/exporter pipeline. See the [list of supported import formats](https://github.com/assimp/assimp#supported-file-formats).
* [OBJ2GLTF](https://github.com/AnalyticalGraphicsInc/OBJ2GLTF) - Command-line tool to convert OBJ to glTF
* [binary-gltf-utils](https://github.com/Qantas94Heavy/binary-gltf-utils) - Node.js tool to convert glTF to Binary glTF.
* [FBX-glTF](https://github.com/cyrillef/FBX-glTF) - FBX glTF importer/exporter plug-in and converter (glTF 0.8)
* [collada2gltf-web-service](https://github.com/AnalyticalGraphicsInc/collada2gltf-web-service) - Simple Node.js web service built on COLLADA2GLTF
* [collada2gltf](https://www.npmjs.com/package/collada2gltf) - Node.js wrapper around COLLADA2GLTF
* [colladaToBglTFConverter](https://github.com/virtualcitySYSTEMS/colladaToBglTFConverter) - Groovy/JavaScript tool to convert COLLADA to Binary glTF (0.8 / CESIUM_binary_glTF)

### Loaders and Viewers

#### WebGL Engines

* [glTF loader](https://github.com/mrdoob/three.js/tree/master/examples/js/loaders/gltf) in [Three.js](http://threejs.org/) (glTF 1.0 in progress)
* [glTF loader](https://github.com/BabylonJS/Babylon.js/tree/master/loaders/glTF) in [BabylonJS](http://babylonjs.com/)
* [glTF loader](https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Scene/Model.js) in [Cesium](http://cesiumjs.org/)
   * The [COLLADA to glTF](http://cesiumjs.org/convertmodel.html) converter also supports drag-and-drop viewing of glTF assets

#### JavaScript / Node.js

* [gltf-walker](https://github.com/ksons/gltf-walker) - onvenience library for processing glTF
* [rest3d](https://github.com/amd/rest3d) - serves glTF and other 3D assets via a REST API (glTF 0.8)
* [gltf-viewer-element](https://www.npmjs.com/package/gltf-viewer-element) - Node.js package for loading and rendering glTF (0.8)
* [Amobee 3D](http://amobee3d.s3.amazonaws.com/ads/Amobee3D_AdList.html) (0.8)

#### C++

* [Tiny glTF loader](https://github.com/syoyo/tinygltfloader) - Header only C++ glTF parsing library
* [LibreOffice](http://zolnaitamas.blogspot.com/2014/08/3d-models-in-impress-libreoffice-43.html) (glTF 0.8)

#### C# #

* [C# glTF loader](https://github.com/KhronosGroup/glTF/tree/master/loaders/CSharp) - C# reference loader for glTF

#### Haxe

* [haxe-gltf](https://github.com/FuzzyWuzzie/haxe-gltf) - A Haxe library for reading glTF

### Utilities

* [gltf-utilities](https://github.com/AnalyticalGraphicsInc/gltf-utilities) - JavaScript utility library to help load glTF
* [gltf-statistics](https://github.com/AnalyticalGraphicsInc/gltf-statistics) - JavaScript and Node.js library and command-line tool to display statistics for glTF models (glTF 0.8)

## Presentations and Articles

* [An Introduction to glTF 1.0](https://www.khronos.org/assets/uploads/developers/library/overview/glTF-1.0-Introduction-Oct15.pdf). October 2015
* [Khronos Group glTF Webinar](https://www.youtube.com/watch?v=YXPeh2hy6Tc). Neil Trevett, Virtual AR Community meeting. October 2015
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
