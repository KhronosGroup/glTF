# MPEG_accessor_timed 

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

An `accessor` in glTF defines the types and layout of the data as stored in a buffer that is viewed through a `bufferView`. When timed data is read from a buffer, the data in the buffer could change dynamically with time. 

A scene that contains timed media and/or metadata shall utilize the timed accessor extension to describe access to the dynamically changing data. The `MPEG_accessor_timed` is an extension to regular accessors to indicate that the underlying data buffer is dynamic. Note that a timed `accessor` has two `bufferViews`, one inherited from the containing `accessor` and a second in the `MPEG_accessor_timed` extension. The former shall be used to reference the timed media data. The latter may be used to point to a dynamic buffer header, which may or may not be present. When present, both `bufferViews` shall point into the same circular buffer. 

The `accessor.bufferView` field, in an `accessor` that has the `MPEG_accessor_timed` extension, as well as the timed accessor information header fields apply to the data of each frame within the circular buffer.

The timed accessor extension is identified by `MPEG_accessor_timed`. 

## glTF Schema Updates

glTF extension to specify timed accessor format formats


**`MPEG_accessor_timed` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**immutable**|`boolean`|This flag indicates if the accessor information may change over time|No, default: `true`|
|**bufferView**|`integer`|This property provides the index in the `bufferViews` array to a `bufferView` element that points to the timed accessor information header. |No|
|**suggestedUpdateRate**|`number`|This property provides the frequency at which the renderer is recommended to poll the underlying buffer for new data.|No, default: `25`|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_accessor_timed.schema.json](/Extensions/MPEG_accessor_timed/schema/MPEG_accessor_timed.schema.json)

### MPEG_accessor_timed.immutable

This flag equal to false indicates the accessor information `componentType`, `type`, and `normalized` may change over time. The changing values of `componentType`, `type` and `normalized` are provided through accessor information header. This flag equal to true indicates the accessor information `componentType`, `type`, and `normalized` do not change over time and are not present in the accessor information header. Note that count, max, min, and byteOffset are expected to change and are always included as part of the timed accessor information header.

* **Type**: `boolean`
* **Required**: No, default: `true`

### MPEG_accessor_timed.bufferView

This property provides the index in the `bufferViews` array to a `bufferView` element that points to the timed accessor information header as described in ISO/IEC 23090-14:Table 8. byteLength field of the `bufferView` element indicates the size of the timed accessor information header. The buffer properties in the `bufferView` element shall point to the same buffer as the `bufferView` in the containing accessor object. In the absence of the `bufferView` attribute, it shall be assumed that the buffer has no dynamic header. In that case, the immutable flag shall be present and shall be set to true.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_accessor_timed.suggestedUpdateRate

The suggestedUpdateRate provides the frequency at which the renderer is recommended to poll the underlying buffer for new data. The rate is provided in number of changes per second.

* **Type**: `number`
* **Required**: No, default: `25`
* **Minimum**: ` >= 0`

### MPEG_accessor_timed.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_accessor_timed.extras

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
