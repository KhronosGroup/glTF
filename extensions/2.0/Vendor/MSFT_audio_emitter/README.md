# MSFT\_audio\_emitter

## Contributors

* Cedric Caillaud, Microsoft
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Jaclyn Shumate, Microsoft
* Jamie Marconi, Microsoft [@najadojo](https://twitter.com/najadojo)
* Jason Syltebo, Microsoft
* Jeremy Kersey, Microsoft
* Joe Kelly, Microsoft
* Matthew Johnston, Microsoft
* Mike Chemistruck, Microsoft

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds a specification for spatialized and ambient audio with clip randomization. Audio data is defined by adding an `extension` property to the top-level glTF 2.0 object and defining a `MSFT_audio_emitter` property with a [`clips`](#reference-clip) array and an [`emitters`](#reference-emitter) array within.

---------------------------------------
<a name="reference-clip"></a>
### Clip

Audio clip data used to create an audio emitter. [`Clip`](#reference-clip) can be referenced by `uri` or `bufferView` index. `mimeType` is required in the latter case.

```json
  "extensions": {
    "MSFT_audio_emitter": {
      "clips": [
        {
          "bufferView": 8,
          "mimeType": "audio/wav"
        },
        {
          "uri": "clip1.wav"
        }
      ]
    }
  }
```

Supported audio formats are

* audio/wav

Each clip contains one of

* a URI to an external file in one of the supported audio formats, or
* a URI with embedded base64-encoded data, or
* a reference to a bufferView; in that case mimeType must be defined.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**uri**|`string`|The uri of the audio source.|No|
|**mimeType**|`string`|The audio file's MIME type.|No|
|**bufferView**|`integer`|The index of the bufferView that contains the audio data. Use this instead of the clip's uri property.|No|

---------------------------------------
<a name="reference-emitter"></a>
### Emitter

Audio Emitters refer to one or more clips and define rendering properties.

```json
  "extensions": {
    "MSFT_audio_emitter": {
      "emitters": [
        {
            "name": "right foot emitter",
            "innerAngle": 1.04719755,
            "outerAngle": 1.57079633,
            "maxDistance": 10,
            "distanceModel": "linear",
            "loop": true,
            "volume": 1.0,
            "clips": [{
              "clip": 0,
              "weight": 0.5
            },
            {
              "clip": 1,
              "weight": 0.5
            }
          ]
        }
      ]
    }
  },
```

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**distanceModel**|`string`|Attenuation function to use on the audio source as it moves away from the listener.|No, default: `exponential`|
|**refDistance**|`number`|A reference distance for reducing volume as source moves further from the listener.|No, default: `1`|
|**maxDistance**|`number`|The maximum distance between source and listener outside of which there will be no sound.|No, default: `256`|
|**rolloffFactor**|`number`|Factor at which attenuation occurs as the source moves further from the listener.|No, default: `1`|
|**innerAngle**|`number`|Size of cone in radians for a directional sound in which there will be no attenuation.|No, default: `3.141592653589793`|
|**outerAngle**|`number`|Size of cone in radians for a directional sound outside of which there will be no sound. Listener angles between innerAngle and outerAngle will falloff linearly.|No, default: `3.141592653589793`|
|**loop**|`boolean`|Selected source must be played in loops.|No, default: `false`|
|**volume**|`number`|Playback volume.|No, default: `1`|
|**clips**|[`Emitter Clip`](#reference-emitter-clip) `[1-*]`|| :white_check_mark: Yes|
|**name**|`string`|The user-defined name of this object.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

#### emitter.distanceModel

Attenuation function to use on the audio source as it moves away from the listener. The attenuation calculation is done in emitter space.
> Example: Emitter refDistance is set to 1 and the emitter's node is scaled by a factor of 10 the refDistance will express as if it was set to 10, rather than unchanged at 1.

* **Type**: `string`
* **Required**: No, default: `exponential`
* **Allowed values**:
  * `linear` expressed by: `1 - rolloffFactor * (distance - refDistance) / (maxDistance - refDistance)`
  * `inverse` expressed by: `refDistance / (refDistance + rolloffFactor * (distance - refDistance))`
  * `exponential` expressed by: `pow(distance / refDistance, -rolloffFactor)`

#### emitter.refDistance

A reference distance for reducing volume as source moves further from the listener.

* **Type**: `number`
* **Required**: No, default: `1`
* **Minimum**: `> 0`

### emitter.maxDistance

The maximum distance between source and listener outside of which there will be no sound. Must be larger than refDistance and only applies to linear distanceModel.

* **Type**: `number`
* **Required**: No, default: `256`
* **Minimum**: `> 0`

#### emitter.rolloffFactor

Factor at which attenuation occurs as the source moves further from the listener. Value must be <= 1 if distanceModel is linear, unbounded otherwise.

* **Type**: `number`
* **Required**: No, default: `1`
* **Minimum**: `>= 0`

#### emitter.innerAngle

The cone will be symmetrical and face the forward direction (Z+). The outerAngle value must be greater than or equal to the innerAngle value. If neither innerAngle or outerAngle are defined the emitter will behave as an omnidirectional source.

* **Type**: `number`
* **Required**: No, default: `3.141592653589793`
* **Minimum**: `>= 0`
* **Maximum**: `<= 3.141592653589793`

#### emitter.outerAngle

The cone will be symmetrical and face the forward direction (Z+). The outerAngle value must be greater than or equal to the innerAngle value. If neither innerAngle or outerAngle are defined the emitter will behave as an omnidirectional source.

* **Type**: `number`
* **Required**: No, default: `3.141592653589793`
* **Minimum**: `>= 0`
* **Maximum**: `<= 3.141592653589793`

#### emitter.loop

Selected source must be played in loops.

* **Type**: `boolean`
* **Required**: No, default: `false`

#### emitter.volume

Playback volume.

* **Type**: `number`
* **Required**: No, default: `1`
* **Minimum**: `>= 0`
* **Maximum**: `<= 1`

#### emitter.clips :white_check_mark:

* **Type**: [`Emitter Clip`](#reference-emitter-clip) `[1-*]`
* **Required**: Yes

---------------------------------------
<a name="reference-emitter-clip"></a>
### Emitter Clip

Multiple clips maybe defined with a probability of selection when triggered. This is helps to create delightful effects like footsteps.

|   |Type|Description|Required|
|---|----|-----------|--------|
|**clip**|`integer`|The index of the clip used by this emitter.| :white_check_mark: Yes|
|**weight**|`number`|Random selection weight.|No, default: `1`|

#### emitter.clip.clip :white_check_mark: 

The index of the clip used by this emitter.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: `>= 0`

#### emitter.clip.weight

Random selection weight. Sum of all weights in the clip array must equal 1.

* **Type**: `number`
* **Required**: No, default: `1`
* **Minimum**: `>= 0`
* **Maximum**: `<= 1`

## Adding Emitter Instances to Nodes

Nodes can have emitters attached to them, just like any other objects that have a transform. These emitters are attached to a node by defining the `extensions.MSFT_audio_emitter` property and, within that, an array of indexes into the `emitters` array using the `emitters` property.

```json
  "nodes": [
    {
      "mesh": 0,
      "extensions": {
        "MSFT_audio_emitter": {
          "emitters": [ 0 ]
        }
      }
    }
  ]

 ```

## Adding Emitter Instances to Scenes

Emitters that have no transform information are global and belong to a scene rather than a node. These Emitters must not define directional or distance properties. These emitters are attached to the scene by defining the `extensions.MSFT_audio_emitter` property and, within that, an array indexes into the `emitters` array using the `emitters` property.

```json
  "scenes": [
    {
      "nodes": [ 0 ],
      "extensions": {
        "MSFT_audio_emitter": {
          "emitters": [ 0 ]
        }
      }
    }
  ]

 ```

## Animations

Audio emitters can play or stop by using an animation event.

```json
"animations": [
  {
    "extensions": {
      "MSFT_audio_emitter": {
        "events": [
          {
            "time": 0.5,
            "action": "play",
            "emitter": 0
          }
        ]
      }
    }
  }
]
 ```

 [`events`](#reference-event) is an array of animation events that trigger an action (play, or stop) at a target time.
 > **Implementation Note:** During animation playback if frames are missed or skipped past the event time the event should be executed as soon as possible. Additionally the event offset for play actions should include the missed amount of time so that the audio clip is still synchronized with the animation. When an animation completes (including stopped or paused) audio playback should cease.

<a name="reference-event"></a>
### Event

Animation events that trigger an action.

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**action**|`string`|Action to take on the target emitter.| :white_check_mark: Yes|
|**emitter**|`integer`|The index of the emitter to target.| :white_check_mark: Yes|
|**time**|`number`|Linear time in seconds that the event should occur.| :white_check_mark: Yes|
|**startOffset**|`number`|Position the clip head at a specific time in seconds.|No|
|**extensions**|`object`|Dictionary object with extension-specific objects.|No|
|**extras**|`any`|Application-specific data.|No|

Additional properties are not allowed.

### event.action :white_check_mark:

Action to take on the target emitter.

* **Type**: `string`
* **Required**: Yes
* **Allowed values**:
  * `play`
  * `pause`
  * `stop`

#### event.emitter :white_check_mark:

The index of the emitter to target.

* **Type**: `integer`
* **Required**: Yes
* **Minimum**: `>= 0`

#### event.time :white_check_mark:

Linear time in seconds that the event should occur.

* **Type**: `number`
* **Required**: Yes
* **Minimum**: `>= 0`

#### event.startOffset

Position the clip head at a specific time in seconds. Only used when action is play and emitter was not most recently paused.

* **Type**: `number`
* **Required**: No
* **Minimum**: `>= 0`

### Property Animations

Audio emitter's properties can be animated along with the reset of the scene using the `EXT_property_animation` extension.

#### Valid EXT_property_animation target templates

|`channel.path`|Accessor Type|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`"/extensions/MSFT_audio_emitter/emitters/{}/volume"`|`"SCALAR"`|`5126`&nbsp;(FLOAT)|volume of playback|

## Extension compatibility and fallback behavior

Clients that do not support the extension will fallback to a muted state. A client can support this extension without `EXT_property_animation` implemented. If the client doesn't implement `EXT_property_animation` these values will be ignored, as such assets should be authored with acceptable initial values.

## glTF Schema Updates

* **JSON schema**: [MSFT_audio_emitter.schema.json](schema/)

## Known Implementations

* [Babylon.js](https://github.com/BabylonJS/Babylon.js/compare/master...najadojo:MSFT_audio_emitter.only)
* [Mixed Reality Viewer](https://www.microsoft.com/store/productId/9NBLGGH42THS)
* Windows Mixed Reality