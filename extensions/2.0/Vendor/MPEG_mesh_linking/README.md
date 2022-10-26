# MPEG_mesh_linking

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

`MPEG_mesh_linking` extension provides the possibility to link a mesh to another 
mesh in a glTF asset. The shadow mesh corresponds to regular mesh data as 
defined by glTF 2.0 without the MPEG_mesh_linking extension. The dependent 
mesh can be transformed/animated by relying on the shadow mesh. The 
`MPEG_mesh_linking` extension included into the dependent mesh links the 
dependent mesh and the shadow mesh and provides with the data and 
information which is used to achieve the ability to animate the dependent 
mesh. Hence, the shadow mesh is present in the glTF assets to assist in 
achieving the ability to apply transformation onto the dependent mesh.



**`MPEG_mesh_linking` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**correspondence**|`integer`|Provides a reference to the `accessor`| &#10003; Yes|
|**mesh**|`integer`|Provides a reference to the shadow `mesh`| &#10003; Yes|
|**pose**|`integer`|Provides a reference to the `accessor`| &#10003; Yes|
|**weights**|`integer`|Provides a reference to the `accessor`|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_mesh_linking.schema.json](/Extensions/MPEG_mesh_linking/schema/MPEG_mesh_linking.schema.json)

### MPEG_mesh_linking.correspondence

Provides a reference to the `accessor`, by specifying the accessor's index in accessors array, that describe the buffer where the correspondence values between the dependent mesh and its associated shadow mesh  will be made available. The componentType of the referenced accessor shall be as indicated in ISO/IEC 23090-14:7.4 and the type shall be `SCALAR`.

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_mesh_linking.mesh

Provides a reference to the shadow `mesh`, by specifying the mesh index in meshes array, associated to the dependent mesh for which the correspondence values are established.

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_mesh_linking.pose

Provides a reference to the `accessor`, by specifying the accessor's index in accessors array, that describe the buffer where the transformation of the nodes associated to the dependent mesh will be made available. The componentType of the referenced accessor shall be `FLOAT` and the type shall be `MAT4`.

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_mesh_linking.weights

Provides a reference to the `accessor`, by specifying the accessor's index in accessors array, that describe the buffer where the “weights” to be applied to the morph targets of the shadow mesh associated to the dependent mesh will be made available. The componentType of the referenced accessor shall be `FLOAT` and the type shall be `SCALAR`.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_mesh_linking.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_mesh_linking.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

The copyright in this software is being made available under the BSD License, included below. This software may be subject to other third party and contributor rights, including patentrights, and no such rights are granted under this license.

Copyright (c) 2010-2021, ISO/IEC
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the ISO/IEC nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
