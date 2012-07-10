##COLLADA2JSON


COLLADA2JSON introduces a JSON format that aims at filling the gap between COLLADA and WebGL.


#### JSON Format

The key aspect of this JSON format is to be driven by WebGL.
It is a final format which extends the workflow from COLLADA to WebGL.
COLLADA (https://www.khronos.org/collada/) is an intermediate format which allows to get scene informations out the authoring tools.

This JSON format is actually an hybrid format that:
- describes the scene graph (and its entities).
- relies on a separate binary blob to host all heavy data (indices/vertices).

To satisfy WebGL constraints, the provided tool named COLLADA2JSON performs the following transformations:
- unify index buffers with unique indices for all vertex attributes. (i.e referred as deindexing)
- split meshes into indexes buffer that do not exceed 65535 indexes to satisfy UNSIGNED_SHORT contraint for WebGL
- generate GLES 2.0 shaders from COLLADA materials.
- convert axis up from scenes to allow seamless merging.
- ...

This design based on an external binary blob also provides the following opportunities:
 
- within the core of the JSON format refers to buffers as typed arrays.
- add compression support.

note: some transformations are either a work in progress or about to be started soon, these are tracked in GitHub issues.

#### What's in there

- viewer: a webgl viewer that loads a model asynchronously (loads the scene graph at once but all the vertex buffers and indices progressivly)
- converter: source code for COLLADA2JSON based on OpenCOLLADA
- importers: A Three.js importer. To be extended with support for other popular WebGL frameworks.

#### Project status

The project is released in a very preliminary stage to encourage early feedback from the community.
So far it has been tested only on OSX, but all the components involved are cross platforms, testing will be extended to all major platforms.

#### Workflow

This JSON file is produced by a tool named COLLADA2JSON based on OpenCOLLADA(https://github.com/KhronosGroup/OpenCOLLADA).
OpenCOLLADA provides a reliable toolchain from end to end by 

and complete the tool from authoring to final 


