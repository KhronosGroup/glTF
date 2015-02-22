<p align="center">
<img src="specification/figures/glTF_300.jpg" />
</p>

glTF is the runtime asset format for WebGL, OpenGL ES, and OpenGL.

glTF is a draft specification, it may change before ratification.  Everyone is encouraged to provide feedback on the specification and contribute to the open-source converter.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback.

## Specification  

[glTF Specification 0.8](https://github.com/KhronosGroup/glTF/blob/master/specification/README.md)

## Converter

COLLADA2GLTF is an open-source command-line pipeline tool that converts COLLADA to glTF.

* Download the [binary](https://github.com/KhronosGroup/glTF/wiki/Converter-builds) for Mac or Windows
* Get the [source & instructions](https://github.com/KhronosGroup/glTF/wiki/converter)

There is also an online drag-and-drop [COLLADA-to-glTF converter](http://cesiumjs.org/convertmodel.html) hosted by the [Cesium](http://cesiumjs.org/) virtual globe engine


## Cloning the repo

Since this repo has git submodules, clone with:

```
git clone --recurse-submodules https://github.com/KhronosGroup/glTF.git
```

# glTF Tools

## Viewers

* [glTF loader](https://github.com/mrdoob/three.js/tree/master/examples/js/loaders/gltf) in [Three.js](http://threejs.org/)
* [glTF loader](http://cesiumjs.org/2014/03/03/Cesium-3D-Models-Tutorial/) in [Cesium](http://cesiumjs.org/)
* [glTF viewer](https://github.com/fabrobinet/glTF-webgl-viewer) based on [montagejs](https://github.com/montagejs/montage)
* [rest3d](https://github.com/amd/rest3d) - serves glTF and other 3D assets via a REST API
* [gltf-viewer-element](https://www.npmjs.com/package/gltf-viewer-element) - Node.js package for loading and rendering glTF
* [Amobee 3D](http://amobee3d.s3.amazonaws.com/ads/Amobee3D_AdList.html)
* [LibreOffice](http://zolnaitamas.blogspot.com/2014/08/3d-models-in-impress-libreoffice-43.html)

## Converters

* `COLLADA2GLTF` - C++ command-line tool to converter COLLADA to glTF (mentioned above)
* [COLLADA to glTF](http://cesiumjs.org/convertmodel.html) drag and drop converter and viewer from Cesium (mentioned above)
* [collada2gltf](https://www.npmjs.com/package/collada2gltf) - Node.js wrapper around COLLADA2GLTF
* [FBX-glTF](https://github.com/cyrillef/FBX-glTF) - FBX glTF importer/exporter plug-in and converter

## Utilities

* [gltf-utilities](https://github.com/AnalyticalGraphicsInc/gltf-utilities) - JavaScript utility library to help load glTF
* [gltf-statistics](https://github.com/AnalyticalGraphicsInc/gltf-statistics) - JavaScript and Node.js library and command-line tool to display statistics for glTF models

## Tutorials and Articles

* [Writing an FBX importer / Exporter plug-in](http://around-the-corner.typepad.com/adn/2015/01/writing-an-fbx-importer-exporter-plug-in.html), January 2015
* [glTF Tips for Artists](http://cesiumjs.org/2014/12/15/glTF-Tips-for-Artists/), December 2014
* [3D for the Modern Web: Declarative 3D and glTF](http://mason.gmu.edu/~bcoughl2/cs752/), Summer 2014
* [Building a WebGL Santa with Cesium and glTF](http://cesiumjs.org/2013/12/23/Building-A-WebGL-Santa-with-Cesium-and-glTF/), December 2013
* [glTF update](http://www.slideshare.net/auradeluxe/gltf-update-with-tony-parisi), August 2013
