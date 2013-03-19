##OVERVIEW

**glTF is a work in progress and not an official Khronos-ratified specification** 
As an initiative from the COLLADA Working Group, this project stands in Khronos repository.  
Everyone is welcomed to contribute and provide feedback. 

## Specification

Can be found [here](https://github.com/KhronosGroup/glTF/blob/master/specification/README.md)
_This is a work-in-progress from the COLLADA Working Group; it is not an official Khronos-ratified specification yet.  It is incomplete and subject to change.  We've made it available early in the spirit of transparency to receive early community feedback.  Please create [issues](https://github.com/KhronosGroup/glTF/issues) with your feedback._

_In particular, the definition of materials are in flux, and work on animations and texture and geometry compression are still in the early stages.  We are also initially focusing on WebGL, OpenGL and OpenGL ES need additional considerations.  The related open-source tools are also not 100% compatible with this spec yet._

## Converter

The JSON format is actually a hybrid format consisting of two files:
- A JSON document describing the scene graph and its entities.
- A separate binary blob containing all large data arrays (indices/vertices).

To satisfy WebGL constraints, the provided tool named COLLADA2JSON performs the following transformations:
(Most of these are work in progress)
- Unify index buffers with unique indices for all vertex attributes. (Known as "deindexing")
- Split meshes into buffers that do not exceed 65535 indices to satisfy the WebGL's maximum index size of UNSIGNED_SHORT 
- Generate default GLSL shaders from the COLLADA materials.
- Normalize the "Up" axis of all scenes to allow easy resource sharing.

Using an external binary blob has several advantages:
 
- The JSON format references the buffers as opaque typed arrays.
- The binary format provides opportunities for better data compression.

More details about the format in the [wiki](https://github.com/KhronosGroup/collada2json/wiki/WebGLTF)  

Watch the GitHub issues tracker for updates on the status of these features.

#### What's provided in this repository:

- viewer: A sample WebGL viewer (Chrome canary recommanded) that loads and displays a model asynchronously (vertex and index buffers loaded progressivly).
The example model contains thousands of independant meshes and buffers (to show progressive loading).
- converter: Source code for COLLADA2JSON converter based on OpenCOLLADA
- loaders: Loader utilities for 3rd party WebGL libraries. Currently contains a Three.js loader, with support planned for other popular WebGL frameworks.

#### Workflow

COLLADA2JSON provides a reliable toolchain based on OpenCOLLADA.
Starting from exporter plugins in major authoring tools to COLLADA parsing when converting to JSON, 
OpenCOLLADA is where all efforts to import/export COLLADA files are centralized.

#### Common setup 
```
git clone https://github.com/KhronosGroup/glTF.git
cd glTF
git submodule update --init --recursive
```

Start a web server in `glTF/webgl` which contains an `index.html` page linking to the demos.

## Converter build instructions

##### Windows

* Install [CMake](http://cmake.org/cmake/resources/software.html)
* `cd glTF/converter/dae2json`
* `cmake CMakeLists.txt`

open `COLLADA2JSON.sln` and build.

##### Mac OSX

* some packages are required.They can be easily installed using a terminal with [brew](http://mxcl.github.com/homebrew/) type:
 * ruby <(curl -fsSkL raw.github.com/mxcl/homebrew/go)
 * brew install cmake
 * brew install pkgconfig
 * brew install pcre
 * brew install libpng
* `cd glTF/converter/dae2json`
* `cmake -G Xcode`

open `COLLADA2JSON.xcodeproj` and build, or if you installed xcode command line tools you can also build in the terminal: `xcodebuild -target collada2json`
