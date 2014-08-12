<p align="center">
<img src="specification/figures/glTF_300.jpg" />
</p>

glTF is the runtime asset format for WebGL, OpenGL ES, and OpenGL.

glTF is a draft specification, it may change before ratification.  Everyone is encouraged to provide feedback on the specification and contribute to the open-source converter.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback.

## Specification  

[glTF Specification 0.8](https://github.com/KhronosGroup/glTF/blob/master/specification/README.md)

## Converter

An open-source command-line [converter](https://github.com/KhronosGroup/glTF/wiki/converter) produces glTF assets from COLLADA using [OpenCOLLADA](https://github.com/khronosGroup/OpenCOLLADA/).

Download the [binary](https://github.com/KhronosGroup/glTF/wiki/Converter-builds) for Mac or Windows.

## Cloning the repo

Since this repo has git submodules, clone with:

```
git clone --recurse-submodules https://github.com/KhronosGroup/glTF.git
```

## Viewers

* [glTF viewer](https://github.com/fabrobinet/glTF-webgl-viewer) based on [montagejs](https://github.com/montagejs/montage)
* [glTF loader](https://github.com/mrdoob/three.js/tree/master/examples/js/loaders/gltf) in [Three.js](http://threejs.org/)
* [glTF loader](http://cesiumjs.org/2014/03/03/Cesium-3D-Models-Tutorial/) in the [Cesium](http://cesiumjs.org/) virtual globe engine
* [rest3d](https://github.com/amd/rest3d) - serves glTF and other 3D assets via a REST API