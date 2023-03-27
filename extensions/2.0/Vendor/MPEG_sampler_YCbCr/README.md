# MPEG_sampler_YCbCr

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC 23090-14/DAmd 1](https://www.iso.org/standard/84769.html), available as [WG03_N0795](https://mpeg.expert/live/nextcloud/index.php/s/f9TY6TBDGxXkMjS)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

defines an extension to support YCbCr formatted textures.

**Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**ycbcrModel**|`integer`|color matrix conversion model|No|
|**ycbcrRange**|`integer`|range used for the color encoding|No|
|**chromaFilter**|`integer`|filter for chroma reconstruction|No|
|**components**|`integer` `[1-*]`|swizzle applied to the RGBA components prior to range expansion and color model conversion|No|
|**xChromaOffset**|`integer`|sample location associated with downsampled chroma in the x dimension|No|
|**yChromaOffset**|`integer`|sample location associated with downsampled chroma in the y dimension|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_sampler_YCbCr.schema.json](/schema/MPEG_sampler_YCbCr.schema.json)

### .ycbcrModel

color matrix conversion model

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### .ycbcrRange

range used for the color encoding

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### .chromaFilter

filter for chroma reconstruction

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### .components

swizzle applied to the RGBA components prior to range expansion and color model conversion

* **Type**: `integer` `[1-*]`
* **Required**: No

### .xChromaOffset

sample location associated with downsampled chroma in the x dimension

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### .yChromaOffset

sample location associated with downsampled chroma in the y dimension

* **Type**: `integer`
* **Required**: No
* **Minimum**: `>= 0`

### .extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### .extras

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
