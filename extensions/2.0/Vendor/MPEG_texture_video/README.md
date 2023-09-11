# MPEG_texture_video

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

`MPEG_texture_video` extension provides the possibility to link a `texture` 
object defined in glTF 2.0 to media and its respective track, listed by 
an `MPEG_media`. `MPEG_texture_video` extension also provides a reference 
to the timed `accessor`, i.e. `accessor` with `MPEG_accessor_timed` extension, 
where the decoded timed texture will be made available.

When the `MPEG_texture_video` extension is not supported, a texture buffer 
should filled by data described by the standard glTF source object.

## glTF Schema Updates

**`MPEG_texture_video` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**accessor**|`integer`|Provides the accessor's index in accessors array| &#10003; Yes|
|**width**|`number`|Provides the maximum width of the texture.| &#10003; Yes|
|**height**|`number`|Provides the maximum height of the texture.| &#10003; Yes|
|**format**|`string`|Indicates the format of the pixel data for this video texture.|No, default: `"RGB"`|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_texture_video.schema.json](./schema/MPEG_texture_video.schema.json)

### MPEG_texture_video.accessor

Provides a reference to the `accessor`, by specifying the accessor's index in accessors array, that describes the buffer where the decoded timed texture will be made available. The `accessor` shall have the `MPEG_accessor_timed` extension. The type, componentType, and count of the accessor depend on the width, height, and format.

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_texture_video.width

Provides the maximum width of the texture.

* **Type**: `number`
* **Required**:  &#10003; Yes

### MPEG_texture_video.height

Provides the maximum height of the texture.

* **Type**: `number`
* **Required**:  &#10003; Yes

### MPEG_texture_video.format

Indicates the format of the pixel data for this video texture. The semantics of these values are defined in Table 8.3 of OpenGL 4.6 specification. The number of components shall match the type indicated by the referenced `accessor`. Normalization of the pixel data shall be indicated by the normalized attribute of the accessor.

* **Type**: `string`
* **Required**: No, default: `"RGB"`
* **Allowed values**:
    * `"RED"`
    * `"GREEN"`
    * `"BLUE"`
    * `"RG"`
    * `"RGB"`
    * `"RGBA"`
    * `"BGR"`
    * `"BGRA"`
    * `"DEPTH_COMPONENT"`

### MPEG_texture_video.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_texture_video.extras

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

