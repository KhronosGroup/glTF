# MPEG_haptics

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC DIS 23090-14 2nd Edition](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The MPEG_haptic and MPEG_haptic material extensions provide the ability to define the integration of haptics in a glTF scene .

At the glTF file level: The MPEG_haptic extension contains an array defining every haptic object.

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

* [ISO/IEC 23090-14/CD Amd 2](https://www.iso.org/standard/86439.html), Information technology — Coded representation of immersive media — Part 14: Scene description — Amendment 2: Support for haptics, augmented reality, avatars, Interactivity, MPEG-I audio, and lighting 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2023

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
