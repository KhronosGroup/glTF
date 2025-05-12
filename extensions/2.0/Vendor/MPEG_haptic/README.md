# MPEG_haptics

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC DIS 23090-14 2nd Edition](https://www.iso.org/standard/90191.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The MPEG_haptic and MPEG_haptic material extensions provide the ability to define the integration of haptics in a glTF scene .

The MPEG_haptic extension allows the support of haptic data as defined in ISO/IEC 23090-31. It is an independent haptic media like an audio content or an image. This extension is attached at the the glTF file level and stores the haptic data contained in the scene.

The MPEG_haptic extension contains an array of haptic objects. The data for each element of the array contains a list of accessors to medias in the MPEG_media extension. 

The data referenced by the accessors is stored in dedicated Haptic buffers. The Haptic buffer formats corresponds to the HMPG binary format detailed in ISO/IEC 23090-31.


---------------------------------------
<a name="reference-mpeg_haptic"></a>
### MPEG_haptic

Haptic data referencing MPEG medias.

**`MPEG_haptic` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**hapticObjects**|[`MPEG_haptic.hapticObjects`](#mpeg_haptichapticobjects-items) `[]`|Provides a list of haptic elements to enable haptic support|No|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|`extras`|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_haptic.schema.json](./schema/MPEG_haptic.schema.json)

#### MPEG_haptic.hapticObjects

* **Type**: [`MPEG_haptic.hapticObjects`](#mpeg_haptichapticobjects-items) `[]`
* **Required**: No

#### MPEG_haptic.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

#### MPEG_haptic.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

#### MPEG_haptic.extras

Application-specific data.

* **Type**: `extras`
* **Required**: No

---------------------------------------
<a name="reference-mpeg_haptic"></a>
### MPEG_haptic.hapticObjects items

**`MPEG_haptic.hapticObjects` items properties**

|   |Type|Description|Required|
|---|---|---|---|
|**accessors**|`array` `[integer]`|Indices of accessors, in the accessors array, to one or more media sources in MPEG_media.media array containing haptic media files.|No|


## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC DIS 23090-14 2nd Edition](https://www.iso.org/standard/90191.html), Information technology — Coded representation of immersive media — Part 14: Scene Description
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2025

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
