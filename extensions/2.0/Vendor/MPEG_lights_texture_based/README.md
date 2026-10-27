# MPEG_lights_texture_based


## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14/CD Amd.2](https://www.iso.org/standard/86439.html), available as [WG03_N0797](https://mpeg.expert/live/nextcloud/index.php/s/f6prqzxWMdDM3r2)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

---------------------------------------
<a name="reference-mpeg_lights_texture_based-light"></a>

## MPEG_lights_texture_based.light

Extension to support texture-based lights

**`MPEG_lights_texture_based.light` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**nature**|`string`|indicates whether the lighting information corresponds to physical light, virtual light, both, or unknown.|No|
|**position**|`number` `[3]`|position of both light and reflection probes associated with the lighting information|No, default: `[0,0,0]`|
|**projection**|`string`|provides the projection type of the specular images|No|
|**rotation_accessor**|`integer`|provides a reference to the accessor giving a sequence of quaternions|No|
|**intensity_accessor**|`integer`|provides a reference to the accessor giving a sequence of intensity values|No|
|**irradiance_accessor**|`integer`|provides a reference to the accessor giving a sequence of irradiance coefficient samples| &#10003; Yes|
|**specular_images**|`integer` `[1-6]`|provides a list of references to textures providing specular images (environment maps)| &#10003; Yes|
|**name**|[`any`](#reference-any)||No|
|**extensions**|[`any`](#reference-any)||No|
|**extras**|[`any`](#reference-any)||No|

Additional properties are allowed.

* **JSON schema**: [MPEG_lights_texture_based.light.schema.json](/schema/MPEG_lights_texture_based.light.schema.json)

### MPEG_lights_texture_based.light.nature

indicates whether the lighting information corresponds to physical light, virtual light, both, or unknown.

* **Type**: `string`
* **Required**: No
* **Allowed values**:
  * `"physical"`
  * `"virtual"`
  * `"both"`
  * `"unknown"`

### MPEG_lights_texture_based.light.position

position of both light and reflection probes associated with the lighting information

* **Type**: `number` `[3]`
* **Required**: No, default: `[0,0,0]`

### MPEG_lights_texture_based.light.projection

provides the projection type of the specular images

* **Type**: `string`
* **Required**: No
* **Allowed values**:
  * `"equirectangular"`
  * `"cubemap"`

### MPEG_lights_texture_based.light.rotation_accessor

provides a reference to the accessor giving a sequence of quaternions

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### MPEG_lights_texture_based.light.intensity_accessor

provides a reference to the accessor giving a sequence of intensity values

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### MPEG_lights_texture_based.light.irradiance_accessor

provides a reference to the accessor giving a sequence of irradiance coefficient samples

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: `>= 0`

### MPEG_lights_texture_based.light.specular_images

provides a list of references to textures providing specular images (environment maps)

* **Type**: `integer` `[1-6]`
  * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**:  &#10003; Yes

### MPEG_lights_texture_based.light.name

* **Type**: [`any`](#reference-any)
* **Required**: No

### MPEG_lights_texture_based.light.extensions

* **Type**: [`any`](#reference-any)
* **Required**: No

### MPEG_lights_texture_based.light.extras

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
