# MPEG_buffer_circular

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## glTF Schema Updates

In order to support timed data access, the buffer element is extended to provide functionality of a circular buffer. The [`MPEG_buffer_circular`](#reference-mpeg_buffer_circular) may be included as part of the `buffer`. `buffer` that provides access to timed data shall include the `MPEG_buffer_circular` extension.


**`MPEG_buffer_circular` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**count**|`integer`|This provides the recommended number of sequential buffer frames.|No, default: `2`|
|**media**|`integer`|The index of the media entry in the `MPEG_media` extension| &#10003; Yes|
|**tracks**|`integer` `[1-*]`|The array of indices of tracks the `MPEG_media` entry.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_buffer_circular.schema.json](./schema/MPEG_buffer_circular.schema.json)

### MPEG_buffer_circular.count

The count field provides the recommended number of sequential buffer frames to be offered by a circular buffer to the presentation engine. This information may be used by the application to setup the circular buffer towards the presentation engine.

* **Type**: `integer`
* **Required**: No, default: `2`
* **Minimum**: ` >= 2`

### MPEG_buffer_circular.media

Index of the media entry in the `MPEG_media` extension, which is used as the source for the input data to the buffer.

* **Type**: `integer`
* **Required**:  &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_buffer_circular.tracks

The array of indices of a track of a media entry, indicated by media and listed by `MPEG_media` extension, used as the source for the input data to this buffer. When tracks element is not present, the media pipeline should perform the necessary processing of all tracks of the `MPEG_media` entry, referenced by the media property, to generate the requested data format of the buffer. When tracks array contains multiple tracks, the media pipeline should perform the necessary processing of all referenced tracks to generate the requested data format of the buffer. If the track attribute is present and there are multiple `alternatives` (i.e. indicating equivalent content) in the referenced media, then the selected track shall be present in all alternatives. When more than one track is listed by tracks element, the corresponding buffer is in active state and the application is informed that the corresponding tracks are needed as source for the input buffer, then the application can optimize the delivery of multiple tracks.

* **Type**: `integer` `[1-*]`
    * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**: No

### MPEG_buffer_circular.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_buffer_circular.extras

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

