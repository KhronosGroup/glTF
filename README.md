## glTF - the runtime asset format for WebGL, OpenGL ES, and OpenGL

_glTF is a draft Specification - may change before Ratification -_  
Everyone is welcomed to contribute to the code and provide feedback about the specification. 

## Specification  

_[glTF Specification](https://github.com/KhronosGroup/glTF/blob/master/specification/README.md) is a work-in-progress from the COLLADA Working Group; it is not an official Khronos-ratified specification yet.  It is incomplete and subject to change.  We've made it available early in the spirit of transparency to receive early community feedback.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback._

_In particular, the definition of materials are in flux, and work on animations and texture and geometry compression are still in the early stages.  We are also initially focusing on WebGL.  OpenGL ES and OpenGL need additional considerations._

## Converter

A [converter](https://github.com/KhronosGroup/glTF/wiki/converter) can be used to produce glTF assets out of COLLADA.

## Viewer

GLTF Viewer based on montagejs can be found (here)[https://github.com/fabrobinet/glTF-webgl-viewer]


Start a web server in `glTF/webgl` which contains an `index.html` page linking to the demos.

#### Workflow

this project provides a reliable toolchain based on OpenCOLLADA.
Starting from exporter plugins in major authoring tools to COLLADA parsing when converting to JSON, 
OpenCOLLADA is where all efforts to import/export COLLADA files are centralized.
