<p align="center">
<img src="specification/figures/glTF_300.jpg" />
</p>

glTF is the runtime asset format for WebGL, OpenGL ES, and OpenGL.

glTF is a draft specification, it may change before ratification.  Everyone is welcomed to contribute to the code and provide feedback about the specification.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback.

## Specification  

[glTF Specification 0.6](https://github.com/KhronosGroup/glTF/blob/schema/specification/README.md)

#### Cloning the repo
```
git clone --recurse-submodules https://github.com/KhronosGroup/glTF.git
```

## Converter

A [converter](https://github.com/KhronosGroup/glTF/wiki/converter) can be used to produce glTF assets out of COLLADA.

You can also find binary [builds](https://github.com/KhronosGroup/glTF/wiki/Converter-builds).

## Viewer

glTF Viewer based on [montagejs](https://github.com/montagejs/montage) can be found [here](https://github.com/fabrobinet/glTF-webgl-viewer)

#### Workflow

this project provides a reliable toolchain based on OpenCOLLADA.
Starting from exporter plugins in major authoring tools to COLLADA parsing when converting to JSON, 
OpenCOLLADA is where all efforts to import/export COLLADA files are centralized.
