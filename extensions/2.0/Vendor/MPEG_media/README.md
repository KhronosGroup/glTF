# MPEG_media 

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

MPEG_media provides an array of MPEG media items referenced by other 
extensions in glTF document. 


**`MPEG_media` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**media**|[`MPEG_media.media`](#reference-mpeg_media-media) `[1-*]`|An array of media. A media contains data referred by other object in a scene| &#10003; Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_media.schema.json](./schema/MPEG_media.schema.json)

### MPEG_media.media

An array of media. A media contains data referred by other object in a scene

* **Type**: [`MPEG_media.media`](#reference-mpeg_media-media) `[1-*]`
* **Required**:  &#10003; Yes

### MPEG_media.name

The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name.

* **Type**: `string`
* **Required**: No

### MPEG_media.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_media.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_media-media"></a>
## MPEG_media.media.

Media used to create a texture, audio source, or any other media type.

**`MPEG_media.media.` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**name**|`string`|User-defined name of the media. |No|
|**startTime**|`number`|The startTime gives the time at which the rendering of the timed media will be in seconds. |No, default: `0`|
|**startTimeOffset**|`number`|The startTimeOffset indicates the time offset into the source, starting from which the timed media is generated.|No, default: `0`|
|**endTimeOffset**|`number`|The endTimeOffset indicates the time offset into the source, up to which the timed media is generated. |No|
|**autoplay**|`boolean`|Specifies that the media start playing as soon as it is ready.|No, default: `true`|
|**autoplayGroup**|`integer`|Specifies that playback starts simultaneously for all media sources with the autoplay flag set to true.|No|
|**loop**|`boolean`|Specifies that the media start over again, every time it is finished.|No, default: `false`|
|**controls**|`boolean`|Specifies that media controls should be exposed to end user|No, default: `false`|
|**alternatives**|[`MPEG_media.media.alternative`](#reference-mpeg_media-media-alternative) `[1-*]`|An array of alternatives of the same media (e.g. different codecs used)| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_media.media.schema.json](./schema/MPEG_media.media.schema.json)

### MPEG_media.media.name

User-defined name of the media. 

* **Type**: `string`
* **Required**: No

### MPEG_media.media.startTime

The startTime gives the time at which the rendering of the timed media will begin. The value is provided in seconds. In the case of timed textures, the static image should be rendered as a texture until the startTime is reached. A startTime of 0 means the presentation time of the current scene. Either startTime or autoplay shall be present in glTF description.

* **Type**: `number`
* **Required**: No, default: `0`
* **Minimum**: ` >= 0`

### MPEG_media.media.startTimeOffset

The startTimeOffset indicates the time offset into the source, starting from which the timed media shall be generated. The value is provided in seconds, where 0 corresponds to the start of the source.

* **Type**: `number`
* **Required**: No, default: `0`
* **Minimum**: ` >= 0`

### MPEG_media.media.endTimeOffset

The endTimeOffset indicates the end time offset into the source, up to which the timed media shall be generated. The value is provided in seconds. If not present, the endTimeOffset corresponds to the end of the source media.

* **Type**: `number`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_media.media.autoplay

Specifies that the media will start playing as soon as it is ready. Either startTime or autoplay shall be present for a media item description. Rendering of all media for which the autoplay flag is set to true should happen simultaneously.

* **Type**: `boolean`
* **Required**: No, default: `true`

### MPEG_media.media.autoplayGroup

All media that have the same autoplayGroup identifier shall start playing synchronously as soon as all autoplayGroup media are ready. autoplayGroup is only allowed if autoplay is set to true.

* **Type**: `integer`
* **Required**: No

### MPEG_media.media.loop

Specifies that the media will start over again, every time it is finished. The timestamp in the buffer shall be continuously increasing when the media source loops, i.e. the playback duration prior to looping shall be added to the media time after looping.

* **Type**: `boolean`
* **Required**: No, default: `false`

### MPEG_media.media.controls

Specifies that media controls should be exposed to end user (such as a play/pause button etc).

* **Type**: `boolean`
* **Required**: No, default: `false`

### MPEG_media.media.alternatives

An array of items that indicate alternatives of the same media (e.g. different video codecs used). The client could select items (i.e. uri and track) included in alternatives depending on the client’s capability.

* **Type**: [`MPEG_media.media.alternative`](#reference-mpeg_media-media-alternative) `[1-*]`
* **Required**:  &#10003; Yes

### MPEG_media.media.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_media.media.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_media-media-alternative"></a>
## MPEG_media.media.alternative

**`MPEG_media.media.alternative` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**uri**|`string`|The uri of the media.| &#10003; Yes|
|**mimeType**|`string`|The media's MIME type.| &#10003; Yes|
|**tracks**|[`MPEG_media.media.alternative.track`](#reference-mpeg_media-media-alternative-track) `[1-*]`|An array of items that lists the components of the referenced media source that are to be used.|No|
|**extraParams**|`object`|An object that may contain any additional media-specific parameters.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_media.media.alternative.schema.json](./schema/MPEG_media.media.alternative.schema.json)

### MPEG_media.media.alternative.uri

The uri of the media. Relative paths are relative to the .gltf file. If the reference media is a real-time media stream, then the uri may follow the referencing scheme defined in ISO/IEC 23090-14:Annex C. If the tracks element is present, the last part of the URI (i.e. the stream identifier such as the mid) is provided by the tracks information.

* **Type**: `string`
* **Required**:  &#10003; Yes
* **Format**: iri-reference

### MPEG_media.media.alternative.mimeType

The media's MIME type. The profiles parameter, as defined in IETF RFC6381, may be included as a part of the mimeType to specify the profile of the media container. (e.g. the profiles parameter indicates the DASH profile when the uri specifies a DASH manifest)

* **Type**: `string`
* **Required**:  &#10003; Yes
* **Allowed values**:
    * `"video/mp4"`

### MPEG_media.media.alternative.tracks

An array of items that lists the components of the referenced media source that are to be used. These can e.g. be a track number of an ISOBMFF, a DASH/CMAF SwitchingSet identifier, or a media id of an RTP stream.

* **Type**: [`MPEG_media.media.alternative.track`](#reference-mpeg_media-media-alternative-track) `[1-*]`
* **Required**: No

### MPEG_media.media.alternative.extraParams

An object that may contain any additional media-specific parameters.

* **Type**: `object`
* **Required**: No

### MPEG_media.media.alternative.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_media.media.alternative.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No




---------------------------------------
<a name="reference-mpeg_media-media-alternative-track"></a>
## MPEG_media.media.alternative.track

**`MPEG_media.media.alternative.track` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**track**|`string`|URL fragment to access the track within the media alternative.| &#10003; Yes|
|**codecs**|`string`|The codecs parameter, as defined in IETF RFC 6381, of the media included in the track.| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_media.media.alternative.track.schema.json](./schema/MPEG_media.media.alternative.track.schema.json)

### MPEG_media.media.alternative.track.track

URL fragment to access the track within the media alternative.  The URL structure is defined for the following formats: DASH: Using MPD Anchors (URL fragments) as defined in ISO/IEC 23009-1:2019:Annex C (Table C.1); ISOBMFF: URL fragments as specified in ISO/IEC 14496-12:2020:Annex C. SDP: stream identifier of the media stream as defined in ISO/IEC 20390-14:Annex C. When V3C data is referenced in the scene description document as in item in MPEG_media.alternative.tracks and the referenced item corresponds to an ISBOBMFF track, the following applies: for single-track encapsulated V3C data, the referenced track in MPEG_media shall be the V3C bitstream track. for multi-track encapsulated V3C data, the referenced track in MPEG_media shall be the V3C atlas track.

* **Type**: `string`
* **Required**:  &#10003; Yes

### MPEG_media.media.alternative.track.codecs

The codecs parameter, as defined in IETF RFC 6381, of the media included in the track.  When the track includes different types of codecs (e.g. the AdaptationSet includes Representations with different codecs), the codecs parameter may be signaled by comma-separated list of values of the codecs.

* **Type**: `string`
* **Required**:  &#10003; Yes

### MPEG_media.media.alternative.track.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_media.media.alternative.track.extras

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

