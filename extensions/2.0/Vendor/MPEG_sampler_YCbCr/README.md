# MPEG_sampler_YCbCr 

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14/AMD 1](https://www.iso.org/standard/84769.html)

## Dependencies

Written against the glTF 2.0 specification and ISO/IEC FDIS 23090-14.

## Overview

A sampler-level extension is described to sample a video texture natively in parallel processing devices such as GPUs. This extension shall be present if the format of the referencing video texture is set to YCbCr. 
A texture object in the textures array may use a sampler with the “MPEG_sampler_YCbCr” sampler extension to provide information to the Presentation Engine to sample the video texture when the texture format is a chroma format such as YCbCr.

The YCbCr sampler extension is identified by `MPEG_sampler_YCbCr`. 

## glTF Schema Updates

The format of the `MPEG_MPEG_sampler_YCbCr` glTF extension is as follows:


**`MPEG_MPEG_sampler_YCbCr` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**ycbcrModel**|`integer`|Describes the color matrix for conversion between color models. The supported formats are defined by the VkSamplerYcbcrModelConversion enumeration in clause 13.1 of Vulkan 1.3.|Yes|
|**ycbcrRange**|[`integer`]|Describes whether the encoded values have headroom and foot room, or whether the encoding uses the full numerical range.|Yes|
|**chromaFilter**|`integer` `[1-*]`|Describes the filter for chroma reconstruction.|Yes|
|**components**|`integer` `[1-*]`|Applies a swizzle to the [r,g,b,a] components based on VkComponentSwizzle enums prior to range expansion and color model conversion. If present, the array shall include 4 values, each of which corresponding to the r,g,b,a components in order of appearance. |No|
|**xChromnaOffset**|[`integer`] |Describes the sample location associated with downsampled chroma components in the x dimension. xChromaOffset has no effect for formats in which chroma components are not downsampled horizontally.|No|
|**yChromnaOffset**|[`integer`] |Describes the sample location associated with downsampled chroma components in the y dimension. yChromaOffset has no effect for formats in which the chroma components are not downsampled vertically.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_MPEG_sampler_YCbCr.schema.json](/Extensions/MPEG_MPEG_sampler_YCbCr/schema/MPEG_MPEG_sampler_YCbCr.schema.json)

### MPEG_MPEG_sampler_YCbCr.ycbcrModel

Describes the color matrix for conversion between color models. The supported formats are defined by the VkSamplerYcbcrModelConversion enumeration in clause 13.1 of Vulkan 1.3.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: ` >= 0`

### MPEG_MPEG_sampler_YCbCr.ycbcrRange

Describes whether the encoded values have headroom and foot room, or whether the encoding uses the full numerical range.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_MPEG_sampler_YCbCr.chromaFilter

Describes the filter for chroma reconstruction.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`


### MPEG_MPEG_sampler_YCbCr.components

Applies a swizzle to the [r,g,b,a] components based on VkComponentSwizzle enums prior to range expansion and color model conversion. If present, the array shall include 4 values, each of which corresponding to the r,g,b,a components in order of appearance. 

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`


### MPEG_MPEG_sampler_YCbCr.xChromaOffset

Describes the sample location associated with downsampled chroma components in the x dimension. xChromaOffset has no effect for formats in which chroma components are not downsampled horizontally.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`


### MPEG_MPEG_sampler_YCbCr.yChromaOffset

Describes the sample location associated with downsampled chroma components in the y dimension. yChromaOffset has no effect for formats in which the chroma components are not downsampled vertically.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`
