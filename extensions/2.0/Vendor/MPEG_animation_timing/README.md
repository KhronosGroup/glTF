# MPEG_animation_timing 

## Contributors
* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)
## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

`MPEG_animation_timing` enables alignment between media timelines and animation timeline defined by glTF 2.0. Using the extension narrated stories could be created. The animation timing metadata could allow simultaneous pausing and other manipulation of animations defined in glTF 2.0 and media.

glTF extension to specify timing information that allow to synchronized animation with media


**`MPEG_animation_timing` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**accessor**|`integer`|Provides a reference to `accessor`| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_animation_timing.schema.json](./schema/MPEG_animation_timing.schema.json)

### MPEG_animation_timing.accessor

Provides a reference to `accessor`, by specifying the `accessor`'s index in `accessors` array, that describes the buffer where the animation timing data will be made available. The sample format shall be as defined in ISO/IEC 23090-14:7.6.3. The componentType of the referenced `accessor` shall be `BYTE` and the type shall be `SCALAR`.

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_animation_timing.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_animation_timing.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


## Interaction with other extensions

`accessor` pointed by `MPEG_animation_timing.accessor` shall contain [`MPEG_accessor_timed`](../MPEG_accessor_timed/README.md) extension and point to a a glTF 2.0 buffer that contain [`MPEG_buffer_circular`](../MPEG_buffer_circular/README.md). The [`MPEG_buffer_circular`](../MPEG_buffer_circular/README.md) extensions links to media that are provided by   [`MPEG_media`](../MPEG_Media/README.md).

## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2022

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
