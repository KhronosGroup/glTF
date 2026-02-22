# MPEG_primitive_V3C


## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC 23090-14/DAmd 1](https://www.iso.org/standard/84769.html), available as [WG03_N0795](https://mpeg.expert/live/nextcloud/index.php/s/f9TY6TBDGxXkMjS)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

---------------------------------------
<a name="reference-mpeg_primitive_v3c"></a>

## MPEG_primitive_V3C

glTF extension to specify support for V3C compressed primitives.

**`MPEG_primitive_V3C` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**_MPEG_V3C_CONFIG**|`integer`|| &#10003; Yes|
|**_MPEG_V3C_AD**|[`MPEG_primitive_V3C.atlas`](#reference-mpeg_primitive_v3c-atlas)|| &#10003; Yes|
|**_MPEG_V3C_GVD_MAPS**|`integer` `[1-*]`|an array of references to video texture maps.| &#10003; Yes|
|**_MPEG_V3C_OVD_MAP**|`integer` `[0-*]`|a reference to a video texture that provides the occupancy map|No|
|**_MPEG_V3C_AVD**|[`MPEG_primitive_V3C.attribute`](#reference-mpeg_primitive_v3c-attribute) `[0-*]`||No|
|**_MPEG_V3C_CAD**|[`MPEG_primitive_V3C.CAD`](#reference-mpeg_primitive_v3c-cad)|This object lists different properties described for the Common Atlas Data in ISO/IEC 23090-5.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_primitive_V3C.schema.json](/schema/MPEG_primitive_V3C.schema.json)

### MPEG_primitive_V3C._MPEG_V3C_CONFIG

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: `>= 0`

### MPEG_primitive_V3C._MPEG_V3C_AD

a reference to the accessor that points to the atlas data.

* **Type**: [`MPEG_primitive_V3C.atlas`](#reference-mpeg_primitive_v3c-atlas)
* **Required**:  &#10003; Yes

### MPEG_primitive_V3C._MPEG_V3C_GVD_MAPS

an array of references to video textures that provide the geometry maps.

* **Type**: `integer` `[1-*]`
  * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**:  &#10003; Yes

### MPEG_primitive_V3C._MPEG_V3C_OVD_MAP

a reference to a video texture that provides the occupancy map

* **Type**: `integer` `[0-*]`
  * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**: No

### MPEG_primitive_V3C._MPEG_V3C_AVD

An array of references to the video textures that provide the attribute data

* **Type**: [`MPEG_primitive_V3C.attribute`](#reference-mpeg_primitive_v3c-attribute) `[0-*]`
* **Required**: No

### MPEG_primitive_V3C._MPEG_V3C_CAD

This object lists different properties described for the Common Atlas Data in ISO/IEC 23090-5.

* **Type**: [`MPEG_primitive_V3C.CAD`](#reference-mpeg_primitive_v3c-cad)
* **Required**: No

### MPEG_primitive_V3C.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_primitive_V3C.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No

---------------------------------------
<a name="reference-mpeg_primitive_v3c-atlas"></a>

## MPEG_primitive_V3C.atlas

object that specifies the format of the atlas object

**`MPEG_primitive_V3C.atlas` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**buffer_format**|`string`|string identifier of the atlas data buffer format|No|
|**accessor**|`integer`|index of the accessor to the atlas data.| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_primitive_V3C.atlas.schema.json](/schema/MPEG_primitive_V3C.atlas.schema.json)

### MPEG_primitive_V3C.atlas.buffer_format

identifier of the atlas data buffer format

* **Type**: `string`
* **Required**: No
* **Allowed values**:
  * `"baseline"` Atlas data with common atlas parameters.
  * `"extended"` Atlas data with common atlas parameters and and PROJECTED patch type application-specific data with PLR information, EOM patch type application-specific data, and RAW patch type application-specific data
  * `"miv"` Atlas data with common atlas parameters and PROJECTED patch type application-specific parameters for MIV.

### MPEG_primitive_V3C.atlas.accessor

index of the accessor to the atlas data

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: `>= 0`

### MPEG_primitive_V3C.atlas.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_primitive_V3C.atlas.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No

---------------------------------------
<a name="reference-mpeg_primitive_v3c-attribute"></a>

## MPEG_primitive_V3C.attribute

defines the attribute of a V3C object.

**`MPEG_primitive_V3C.attribute` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**type**|`integer`|provides the type of the attribute.|No|
|**maps**|`integer` `[1-*]`|| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_primitive_V3C.attribute.schema.json](/schema/MPEG_primitive_V3C.attribute.schema.json)

### MPEG_primitive_V3C.attribute.type

provides the type of the attribute.

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`
* **Maximum**: `<= 255`

### MPEG_primitive_V3C.attribute.maps

provides the references to the corresponding video texture maps.

* **Type**: `integer` `[1-*]`
  * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**:  &#10003; Yes

### MPEG_primitive_V3C.attribute.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_primitive_V3C.attribute.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No

---------------------------------------
<a name="reference-mpeg_primitive_v3c-cad"></a>

## MPEG_primitive_V3C.CAD

defines the common atlas data for a v3c object

**`MPEG_primitive_V3C.CAD` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**MIV_view_parameters**|`integer`|indicates the accessor index which is used to refer to the list of MIV view parameters.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_primitive_V3C.CAD.schema.json](/schema/MPEG_primitive_V3C.CAD.schema.json)

### MPEG_primitive_V3C.CAD.MIV_view_parameters

indicates the accessor index which is used to refer to the list of MIV view parameters.

* **Type**: `integer`
* **Required**: No

### MPEG_primitive_V3C.CAD.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_primitive_V3C.CAD.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC 23090-14/DAmd 1](https://www.iso.org/standard/84769.html), Information technology — Coded representation of immersive media — Part 14: Scene description — Amendment 1: Support for immersive media codecs in scene description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2023

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
