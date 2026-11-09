# MPEG_audio_spatial

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The `MPEG_audio_spatial` extension adds support for spatial audio and it may be included at top level or attached to any node in the scene. 

The `MPEG_audio_spatial` extension supports three different node types:
* `source`: an audio source that provides input audio data into the scene. Mono object and HOA sources are supported in this version of the document.
    * Type: `Object` or `HOA`
    * `HOA` audio sources shall ignore the parent node's position and be rendered only in 3DoF.
* `reverb`: A reverb effect can be attached to the output of an audio source. Several reverb units can exist and sound sources can feed into one or more of these reverb units. An audio renderer that does not support reverb shall ignore it if the bypass attribute is set to true. If the bypass attribute is set to false, the audio renderer shall return an error message
* `listener`: An audio listener represents the output of audio in the scene. A listener should be attached to a camera node in the scene. By being a child node of the camera, additional transformations can be applied to the audio listener relative to the transformation applied to the parent camera.

## glTF Schema Updates
**`MPEG_audio_spatial` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**sources**|[`MPEG_audio_spatial.source`](#reference-mpeg_audio_spatial-source) `[1-*]`|An array of audio sources.|No|
|**listener**|[`MPEG_audio_spatial.listener`](#reference-mpeg_audio_spatial-listener)||No|
|**reverbs**|[`MPEG_audio_spatial.reverb`](#reference-mpeg_audio_spatial-reverb) `[1-*]`|An array of reverb items|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_audio_spatial.schema.json](./schema/MPEG_audio_spatial.schema.json)

### MPEG_audio_spatial.sources

An array of audio sources.

* **Type**: [`MPEG_audio_spatial.source`](#reference-mpeg_audio_spatial-source) `[1-*]`
* **Required**: No

### MPEG_audio_spatial.listener



* **Type**: [`MPEG_audio_spatial.listener`](#reference-mpeg_audio_spatial-listener)
* **Required**: No

### MPEG_audio_spatial.reverbs

An array of reverb items

* **Type**: [`MPEG_audio_spatial.reverb`](#reference-mpeg_audio_spatial-reverb) `[1-*]`
* **Required**: No

### MPEG_audio_spatial.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_audio_spatial.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_audio_spatial-listener"></a>
## MPEG_audio_spatial.listener



**`MPEG_audio_spatial.listener` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**id**|`integer`|A unique identifier| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_audio_spatial.listener.schema.json](./schema/MPEG_audio_spatial.listener.schema.json)

### MPEG_audio_spatial.listener.id

A unique identifier of the audio listener in the scene.

* **Type**: `integer`
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.listener.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_audio_spatial.listener.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_audio_spatial-reverb"></a>
## MPEG_audio_spatial.reverb



**`MPEG_audio_spatial.reverb` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**id**|[`any`](#reference-any)|A unique identifier| &#10003; Yes|
|**bypass**|`boolean`|Indicates if the reverb unit can be bypassed if the audio renderer does not support it.|No, default: `true`|
|**properties**|[`MPEG_audio_spatial.reverb.property`](#reference-mpeg_audio_spatial-reverb-property) `[1-*]`|An array of property items| &#10003; Yes|
|**predelay**|`number`|Delay of audio source.|No, default: `0`|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_audio_spatial.reverb.schema.json](./schema/MPEG_audio_spatial.reverb.schema.json)

### MPEG_audio_spatial.reverb.id

A unique identifier of the audio reverb unit in the scene.

* **Type**: [`any`](#reference-any)
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.reverb.bypass

Indicates if the reverb unit can be bypassed if the audio renderer does not support it.

* **Type**: `boolean`
* **Required**: No, default: `true`

### MPEG_audio_spatial.reverb.properties

Array of items that contains reverb property objects describing reverb unit specific parameters

* **Type**: [`MPEG_audio_spatial.reverb.property`](#reference-mpeg_audio_spatial-reverb-property) `[1-*]`
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.reverb.predelay

Delay in seconds from onset of source to onset of late reverberation for which DSR is provided.

* **Type**: `number`
* **Required**: No, default: `0`

### MPEG_audio_spatial.reverb.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_audio_spatial.reverb.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_audio_spatial-reverb-property"></a>
## MPEG_audio_spatial.reverb.property



**`MPEG_audio_spatial.reverb.property` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**frequency**|`number`|Frequency for RT60 and DSR values| &#10003; Yes|
|**RT60**|`number`|Specifies RT60 value in seconds for the frequency provided in the `frequency` field| &#10003; Yes|
|**DSR**|`number`|Specifies Diffuse-to-Source Ratio value in dB for the frequency provided in the `frequency` field.| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_audio_spatial.reverb.property.schema.json](./schema/MPEG_audio_spatial.reverb.property.schema.json)

### MPEG_audio_spatial.reverb.property.frequency

Frequency for the provided RT60 and DSR values.

* **Type**: `number`
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.reverb.property.RT60

Specifies RT60 value in seconds for the frequency provided in the `frequency` field

* **Type**: `number`
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.reverb.property.DSR

Specifies Diffuse-to-Source Ratio value in dB for the frequency provided in the `frequency` field. For details see ISO/IEC 23090-14:5.4.1.4.

* **Type**: `number`
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.reverb.property.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_audio_spatial.reverb.property.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_audio_spatial-source"></a>
## MPEG_audio_spatial.source



**`MPEG_audio_spatial.source` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**id**|`integer`|A unique identifier| &#10003; Yes|
|**type**|`string`|A type of the audio source| &#10003; Yes|
|**pregain**|`number`|A level-adjustment of the audio source|No, default: `0`|
|**playbackSpeed**|`number`|Playback speed of the audio source|No, default: `1`|
|**attenuation**|[`any`](#reference-any)|A function used to calculate the attenuation of the audio source.|No, default: `linearDistance`|
|**attenuationParameters**|`number` `[1-*]`|An array of attenuation parameters|No|
|**referenceDistance**|`number`|A distance in meters.|No, default: `1`|
|**accessors**|`integer` `[1-*]`|An array of `accessors` that describe the audio source| &#10003; Yes|
|**reverbFeed**|`integer` `[]`|An array of pointers to reverb units|No|
|**reverbFeedGain**|`number` `[]`|An array of gain values|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_audio_spatial.source.schema.json](./schema/MPEG_audio_spatial.source.schema.json)

### MPEG_audio_spatial.source.id

A unique identifier of the audio source in the scene.

* **Type**: `integer`
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.source.type

Indicates the type of the audio source. `type` value equal to `Object` indicates mono object. `type` value equal to `HOA` indicates HOA object

* **Type**: `string`
* **Required**:  &#10003; Yes
* **Allowed values**:
    * `"Object"`
    * `"HOA"`

### MPEG_audio_spatial.source.pregain

Provides a level-adjustment in dB for the signal associated with the source.

* **Type**: `number`
* **Required**: No, default: `0`
* **Minimum**: ` >= 0`

### MPEG_audio_spatial.source.playbackSpeed

Playback speed of the audio signal. A value of 1.0 corresponds to playback at normal speed. The value shall be between 0.5 and 2.0.

* **Type**: `number`
* **Required**: No, default: `1`
* **Minimum**: ` >= 0.5`
* **Maximum**: ` <= 2`

### MPEG_audio_spatial.source.attenuation

Indicates the function used to calculate the attenuation of the audio signal based on the distance to the source. attenuation value equal to `noAttenuation` indicates no attenuation function should be used. attenuation value equal to `inverseDistance` indicates inverse distance function should be used. attenuation value equal to `linearDistance` indicates linear distance function should be used. attenuation value equal to `exponentialDistance` indicates exponential distance function should be used. attenuation value equal to `custom` indicates custom function should be used. The definition of `custom` function is outside of the scope of ISO/IEC 23090-14. The attenuation functions and their parameters are defined in ISO/IEC 23090-14:Annex D.

* **Type**: [`any`](#reference-any)
* **Required**: No, default: `linearDistance`
* **Allowed values**:
    * `noAttenuation`
    * `inverseDistance`
    * `linearDistance`
    * `exponentialDistance`
    * `custom`

### MPEG_audio_spatial.source.attenuationParameters

An array of parameters that are input to the attenuation function. The semantics of these parameters depend on the attenuation function itself and are defined in ISO/IEC 23090-14

* **Type**: `number` `[1-*]`
* **Required**: No

### MPEG_audio_spatial.source.referenceDistance

Provides the distance in meters for which the distance gain is implicitly included in the source signal after application of pregain. When type equals ‘HOA’ the element shall not be present.

* **Type**: `number`
* **Required**: No, default: `1`
* **Minimum**: ` >= 1`

### MPEG_audio_spatial.source.accessors

An array of `accessor` references, by specifying the `accessors` indices in `accessors array, that describe the buffers where the decoded audio will be made available.

* **Type**: `integer` `[1-*]`
    * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**:  &#10003; Yes

### MPEG_audio_spatial.source.reverbFeed

Provides one or more pointers to reverb units, optionally extended by a floating point scaling factor. A reverb unit represents a reverberation audio processor that is configured by the metadata from a single reverb object. Typically, a reverb object represents reverberation properties of a single room.

* **Type**: `integer` `[]`
* **Required**: No

### MPEG_audio_spatial.source.reverbFeedGain

Provides an array of gain [dB] values to be applied to the source’s signal(s) when feeding it to the corresponding reverbFeed. The array shall have the same  number of elements as the reverbFeed array field.

* **Type**: `number` `[]`
* **Required**: No

### MPEG_audio_spatial.source.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_audio_spatial.source.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


## Interaction with other extensions

The `MPEG_audio_spatial` works with the `MPEG_accessor_timed` to reference audio sources.
## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2022

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.

