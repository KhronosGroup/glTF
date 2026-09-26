# MPEG_node_avatar

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14/CD Amd.2](https://www.iso.org/standard/86439.html), available as [WG03_N0797 (https://mpeg.expert/live/nextcloud/index.php/s/f6prqzxWMdDM3r2)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

---------------------------------------
<a name="reference-mpeg_node_avatar"></a>

## MPEG_node_avatar

glTF extension to specify support for avatars

**`MPEG_node_avatar` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**isAvatar**|`boolean`|An avatar node.|No, default: `false`|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_node_avatar.schema.json](/schema/MPEG_node_avatar.schema.json)

### MPEG_node_avatar.isAvatar

A node that is the root of an avatar.

* **Type**: `boolean`
* **Required**: No, default: `false`

### MPEG_node_avatar.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_node_avatar.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC 23090-14/CD Amd 2](https://www.iso.org/standard/86439.html), Information technology — Coded representation of immersive media — Part 14: Scene description — Amendment 2: Support for haptics, augmented reality, avatars, Interactivity, MPEG-I audio, and lighting 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2023

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
