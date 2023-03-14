# MPEG_viewport_recommended

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

`MPEG_viewport_recommended` provides the link from a `camera` object defined in 
glTF 2.0 to recommended viewport information by referencing to the timed
`accessor`, where the sample of recommended viewport information will be 
made available.

The recommended viewport information provides dynamically changing 
information which includes translation and rotation of the node which 
includes the camera object, as well as the intrinsic camera parameter of the 
camera object. The client renders viewport according to the dynamically 
changed information.

> **_NOTE:_** Another approach to achieve recommended viewport is to define 
> an animation for a node with attached camera. The approach, however, does 
> not support dynamically changing intrinsic camera and must be defined 
> during the creation of glTF object.


**`MPEG_viewport_recommended` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**viewports**|[`MPEG_viewport_recommended.viewport`](#reference-mpeg_viewport_recommended-viewport) `[1-*]`|| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_viewport_recommended.schema.json](./schema/MPEG_viewport_recommended.schema.json)

### MPEG_viewport_recommended.viewports

* **Type**: [`MPEG_viewport_recommended.viewport`](#reference-mpeg_viewport_recommended-viewport) `[1-*]`
* **Required**:  &#10003; Yes

### MPEG_viewport_recommended.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_viewport_recommended.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_viewport_recommended-viewport"></a>
## MPEG_viewport_recommended.viewport

specifying a recommended viewport.

**`MPEG_viewport_recommended.viewport` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**name**|`string`|The user-defined name of this object.|No|
|**translation**|`integer`|Provides a reference to `accessor`|No|
|**rotation**|`integer`|Provides a reference to `accessor`|No|
|**type**|`string`|provides the type of camera.|No, default: `"perspective"`|
|**parameters**|`integer`|Provides a reference to `accessor`|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_viewport_recommended.viewport.schema.json](./schema/MPEG_viewport_recommended.viewport.schema.json)

### MPEG_viewport_recommended.viewport.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### MPEG_viewport_recommended.viewport.translation

Provides a reference to `accessor` where the timed data for the translation of camera object will be made available. The componentType of the referenced accessor shall be `FLOAT` and the type shall be `VEC3`, (x, y, z).

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_viewport_recommended.viewport.rotation

Provides a reference to `accessor` where the timed data for  the rotation of camera object will be made available. The componentType of the referenced accessor shall be `FLOAT` and the type shall be `VEC4`, as a unit quaternion, (x, y, z, w).

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_viewport_recommended.viewport.type

provides the type of camera.

* **Type**: `string`
* **Required**: No, default: `"perspective"`
* **Allowed values**:
    * `"perspective"`
    * `"orthographic"`

### MPEG_viewport_recommended.viewport.parameters

Provides a reference to `accessor` where the timed data for the perspective or orthographic camera parameters will be made available. The componentType of the referenced accessor shall be `FLOAT` and the type shall be `VEC4`. When the type of the camera object which includes this extension is perspective, `FLOAT_VEC4` means (aspectRatio, yfov,  zfar, znear). When orthographic type, `FLOAT_VEC4` means (xmag, ymag, zfar, znear). The requirements on the camera parameters from ISO/IEC 12113:2021  shall apply.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_viewport_recommended.viewport.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_viewport_recommended.viewport.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No

## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2022

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.

