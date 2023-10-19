# KHR_audio

## Contributors

- Robert Long, Element Inc.
- Anthony Burchell, Individual Contributor
- K. S. Ernest (iFire) Lee, Individual Contributor
- Michael Nisbet, Individual Contributor
- humbletim, Individual Contributor
- Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
- Aaron Franke, The Mirror Megaverse Inc.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for the addition of spatialized and non-spatialized audio to glTF scenes.

Audio emitter objects may be added to 3D nodes for positional audio or to the scene for environmental or ambient audio such as background music.

### Example:

```json
{
    "extensions": {
        "KHR_audio": {
            "emitters": [
                {
                    "name": "Positional Emitter",
                    "type": "positional",
                    "gain": 0.8,
                    "sources": [0, 1],
                    "positional": {
                        "coneInnerAngle": 6.283185307179586,
                        "coneOuterAngle": 6.283185307179586,
                        "coneOuterGain": 0.0,
                        "distanceModel": "inverse",
                        "maxDistance": 10.0,
                        "refDistance": 1.0,
                        "rolloffFactor": 0.8
                    }
                },
                {
                    "name": "Global Emitter",
                    "type": "global",
                    "gain": 0.8,
                    "sources": [1]
                }
            ],
            "sources": [
                {
                    "name": "Clip 1",
                    "gain": 0.6,
                    "autoPlay": true,
                    "loop": true,
                    "audio": 0
                },
                {
                    "name": "Clip 2",
                    "gain": 0.6,
                    "autoPlay": true,
                    "loop": true,
                    "audio": 1
                }
            ],
            "audio": [
                {
                    "uri": "audio1.mp3"
                },
                {
                    "bufferView": 0,
                    "mimeType": "audio/mpeg"
                }
            ]
        }
    },
    "scenes": [
        {
            "name": "Default Scene",
            "extensions": {
                "KHR_audio": {
                    "emitters": [1]
                }
            }
        }
    ],
    "nodes": [
        {
            "name": "Duck",
            "translation": [1.0, 2.0, 3.0],
            "extensions": {
                "KHR_audio": {
                    "emitter": 0
                }
            }
        }
    ]
}
```

## glTF Schema Updates

This extension consists of three primary data structures: Audio Data, Audio Sources, and Audio Emitters. Data, sources and emitters are defined on an `KHR_audio` object added to the `extensions` object on the document root.

The extension must be added to the file's `extensionsUsed` array and because it is optional, it does not need to be added to the `extensionsRequired` array.

#### Example:

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "KHR_audio"
    ],
    "scenes": [...],
    "nodes": [...],
    "extensions": {
        "KHR_audio": {
            "audio": [...],
            "sources": [...],
            "emitters": [...]
        }
    }
}
```

### Audio Data

Audio data objects define where audio data is located. Data is either accessed via a bufferView or uri.

When storing audio data in a buffer view, the `mimeType` field must be specified. The base specification supports `audio/mpeg` and `audio/wav` MIME types. These were chosen with consideration for the wide support for these types acrosss 3D engines and common use cases. Other supported audio formats may be added via extensions.

Note that in tools that process glTF files, but do not implement the KHR_audio extension, external files referenced via the `uri` field may not be properly copied to their final destination or baked into the final binary glTF file. In these cases, using the `bufferView` property may be a better choice assuming the referenced `bufferView` index is not changed by the tool. The `uri` field might be a better choice when you want to be able to quickly change the referenced audio asset.

#### `bufferView`

The index of the bufferView that contains the audio data. Use this instead of the audio source's uri property.

#### `mimeType`

The audio's MIME type. Required if `bufferView` is defined. Unless specified by another extension, the only supported mimeType is `audio/mpeg`.

#### `uri`

The uri of the audio file. Relative paths are relative to the .gltf file.

### Audio Sources

Audio sources define the playing state for a given audio data. They connect one audio data to zero to many audio emitters.

#### `gain`

Unitless linear multiplier against original audio file volume used for determining audio source loudness.

#### `loop`

Whether or not to loop the specified audio when finished.

#### `autoPlay`

Whether or not to play the specified audio when the glTF is loaded.

#### `audio`

The index of the audio data assigned to this clip.

### Audio Emitter

Positional or global sinks for playing back audio sources.

#### `type`

Specifies the audio emitter type.

- `positional` Positional audio emitters. Using sound cones, the orientation is `-Z` having the same emission direction as [`KHR_lights_punctual`](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_lights_punctual) and [glTF cameras](https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_016_Cameras.md).
- `global` Global audio emitters are not affected by the position of audio listeners. `coneInnerAngle`, `coneOuterAngle`, `coneOuterGain`, `distanceModel`, `maxDistance`, `refDistance`, and `rolloffFactor` should all be ignored when set.

#### `gain`

Unitless multiplier against original source volume for determining emitter loudness.

#### `loop`

Whether or not to loop the specified audio clip when finished.

#### `playing`

Whether or not the specified audio clip is playing. Setting this property `true` will set the audio clip to play on load (autoplay).

#### `sources`

An array of audio source indices used by the audio emitter. This array may be empty.

#### `positional`

An object containing the positional audio emitter properties. This may only be defined if `type` is set to `positional`.

### Positional Audio Emitter Properties

#### `coneInnerAngle`

The angle, in radians, of a cone inside of which there will be no volume reduction.

#### `coneOuterAngle`

The angle, in radians, of a cone outside of which the volume will be reduced to a constant value of`coneOuterGain`.

#### `coneOuterGain`

The gain of the audio emitter set when outside the cone defined by the `coneOuterAngle` property. It is a linear value (not dB).

#### `distanceModel`

Specifies the distance model for the audio emitter.

- `linear` A linear distance model calculating the gain induced by the distance according to:
    `1.0 - rolloffFactor * (distance - refDistance) / (maxDistance - refDistance)`
- `inverse` (default) An inverse distance model calculating the gain induced by the distance according to:
    `refDistance / (refDistance + rolloffFactor * (Math.max(distance, refDistance) - refDistance))`
- `exponential` An exponential distance model calculating the gain induced by the distance according to:
    `pow((Math.max(distance, refDistance) / refDistance, -rolloffFactor))`

#### `maxDistance`

The maximum distance between the emitter and listener, after which the volume will not be reduced any further. `maximumDistance` may only be applied when the distanceModel is set to linear. Otherwise, it should be ignored.

#### `refDistance`

A reference distance for reducing volume as the emitter moves further from the listener. For distances less than this, the volume is not reduced.

#### `rolloffFactor`

Describes how quickly the volume is reduced as the emitter moves away from listener. When distanceModel is set to linear, the maximum value is 1 otherwise there is no upper limit.

### Using Audio Emitters

Audio emitters of type `global` may be added to scenes using the following syntax:

```json
{
    "scenes": [
        {
            "extensions": {
                "KHR_audio": {
                    "emitters": [1, 3]
                }
            }
        }
    ]
}
```

Audio emitters of type `global` may be added to nodes using the following syntax:

```json
{
    "nodes": [
        {
            "extensions": {
                "KHR_audio": {
                    "emitter": 1
                }
            }
        }
    ]
}
```

Audio emitters of type `positional` may be added to nodes using the following syntax:

```json
{
    "nodes": [
        {
            "extensions": {
                "KHR_audio": {
                    "emitter": 0
                }
            }
        }
    ]
}
```

Note that multiple global audio emitters are allowed on the scene, but only a single audio emitter may be added to a node.

### Audio Rolloff Formula

The Audio Rolloff range is `(0.0, +∞)`. The default is `1.0`.

The rolloff formula is dependant on the distance model defined. The available distance models are `linear`, `inverse`, and `exponential`.

- linear formula: `1.0 - rolloffFactor * (distance - refDistance) / (maxDistance - refDistance)`
- inverse formula: `refDistance / (refDistance + rolloffFactor * (Math.max(distance, refDistance) - refDistance))`
- exponential formula: `pow((Math.max(distance, refDistance) / refDistance, -rolloffFactor))`

### Audio Gain Units

The gain unit range is `(0.0, +∞)`. The default is `1.0`.

- gain formula: `originalVolume * gain`

### Audio Cone Vizualized

<img alt="Audio cone showing how cone parameters impact volume based on relative distance to the source." src="./figures/cone-diagram.svg" width="500px" />

Figure 1. A modified graphic based on the <a href="https://webaudio.github.io/web-audio-api/#Spatialization-sound-cones" target="_blank">W3C Web Audio API Audio cone Figure</a>

The cone properties relate to the `PannerNode` interface and determine the amount of volume relative to a listeners position within the defined cone area.

The gain relative to cone properties is determined in a similar way as described in the web audio api with the difference that this audio emitter extension uses radians in place of degrees. [Cone Gain Algorithm Example](https://webaudio.github.io/web-audio-api/#Spatialization-sound-cones)

### Units for Rotations

Radians are used for rotations matching glTF2.

### JSON Schema

[glTF.KHR_audio.schema.json](/extensions/2.0/KHR_audio/schema/glTF.KHR_audio.schema.json)

## Known Implementations

- Third Room - https://github.com/thirdroom/thirdroom
- Three Object Viewer (WordPress Plugin) - https://wordpress.org/plugins/three-object-viewer/
- UX3D Experimental C++ implementation - https://github.com/ux3d/OMI

## Resources

Prior Art:

- [W3C Web Audio API](https://www.w3.org/TR/webaudio/)
- [MSFT_audio_emitter](https://github.com/KhronosGroup/glTF/pull/1400)
- [MOZ_hubs_components Audio](https://github.com/MozillaReality/hubs-blender-exporter/blob/04fc1d1/default-config.json#L298-L324)
