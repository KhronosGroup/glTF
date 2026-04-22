# KHR_audio_environment

## Contributors

- Chintan Shah, Meta
- Alexey Medvedev, Meta

Copyright 2024 The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

- Required: `KHR_audio_emitter`
- Optional: `KHR_audio_graph`

## Overview

This extension adds listener, spatialization, and environmental acoustics to `KHR_audio_emitter`. It makes the listener explicit, allows the spatialization algorithm to be selected, and adds room-level acoustic properties such as reverb and custom distance attenuation.

Without this extension, `KHR_audio_emitter` leaves listener behavior and environmental acoustics to the implementation. `KHR_audio_environment` makes those details portable.

### Motivation

High-quality spatial audio depends on more than emitter placement:

1. Listener properties determine where the scene is heard from.
2. Spatialization settings determine how emitters are rendered relative to the listener.
3. Environmental acoustics determine how sound reflects, decays, and fills the scene.

This extension captures those concerns without changing the core `KHR_audio_emitter` source and emitter model.

### Design Principles

- Extends `KHR_audio_emitter`; it does not replace it.
- Keeps listener transforms on glTF nodes, not duplicated on listener objects.
- Makes spatialization model selection explicit.
- Supports both parametric and impulse-response reverb.
- Works with or without `KHR_audio_graph`.

## Extension Declaration

```json
{
    "extensionsUsed": ["KHR_audio_emitter", "KHR_audio_environment"],
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [...],
            "sources": [...],
            "emitters": [...]
        },
        "KHR_audio_environment": {
            "listeners": [...],
            "environments": [...]
        }
    }
}
```

## 1. Listener

The listener represents the point in the scene from which spatial audio is heard.

### 1.1 Listener Object

Listeners are declared at the document level.

| Property | Type | Description | Required |
|---|---|---|---|
| **name** | `string` | Human-readable name. | No |
| **spatializationModel** | `string` | Spatialization algorithm. Default: `equalpower`. | No |
| **hrtf** | `object` | Optional HRTF configuration. | No |
| **interauralDistance** | `number` | Distance between ears in meters. Default: `0.17`. | No |
| **extensions** | `object` | Extension-specific objects. | No |
| **extras** | `any` | Application-specific data. | No |

### 1.2 Listener Binding

Listeners are bound to glTF nodes, typically nodes containing cameras.

```json
{
    "nodes": [
        {
            "name": "MainCamera",
            "camera": 0,
            "extensions": {
                "KHR_audio_environment": {
                    "listener": 0
                }
            }
        }
    ]
}
```

At runtime a scene should have at most one active listener. If multiple nodes reference listeners, implementations should prefer the listener attached to the active camera. Otherwise, they may fall back to the first referenced listener.

The listener inherits position and orientation from the glTF node. Those transforms are not repeated on the listener object.

### 1.3 Spatialization Model

The `spatializationModel` property selects the algorithm used to spatialize positional emitters relative to the listener.

| Value | Description |
|---|---|
| `equalpower` | Equal-power panning. Simple, efficient, stereo-oriented. Default. |
| `HRTF` | Head-related transfer function rendering for higher quality binaural positioning. |
| `custom` | User-defined spatialization determined by application logic or extensions. |

```json
{
    "listeners": [
        {
            "name": "Player Listener",
            "spatializationModel": "HRTF"
        }
    ]
}
```

### 1.4 HRTF Configuration

When `spatializationModel` is `HRTF` or `custom`, optional HRTF configuration may be provided.

| Property | Type | Description | Required |
|---|---|---|---|
| **audio** | `integer` | Index into `KHR_audio_emitter.audio[]` referencing an HRTF impulse-response asset. | No |
| **profile** | `string` | HRTF profile hint: `generic`, `small`, `medium`, `large`. Default: `generic`. | No |

Built-in profile example:

```json
{
    "listeners": [
        {
            "name": "Profiled Listener",
            "spatializationModel": "HRTF",
            "hrtf": {
                "profile": "medium"
            }
        }
    ]
}
```

Impulse-response example:

```json
{
    "listeners": [
        {
            "name": "IR Listener",
            "spatializationModel": "custom",
            "hrtf": {
                "audio": 5
            }
        }
    ]
}
```

## 2. Environment

Environments define acoustic characteristics that apply to a scene or to a localized node subtree.

### 2.1 Environment Object

Environments are declared at the document level.

| Property | Type | Description | Required |
|---|---|---|---|
| **name** | `string` | Human-readable name. | No |
| **reverb** | `object` | Reverb properties. | No |
| **extensions** | `object` | Extension-specific objects. | No |
| **extras** | `any` | Application-specific data. | No |

### 2.2 Environment Binding

Environments may be bound globally on scenes or locally on nodes.

Scene-level binding:

```json
{
    "scenes": [
        {
            "name": "Cathedral",
            "extensions": {
                "KHR_audio_environment": {
                    "environment": 0
                }
            }
        }
    ]
}
```

Node-level binding:

```json
{
    "nodes": [
        {
            "name": "TunnelZone",
            "extensions": {
                "KHR_audio_environment": {
                    "environment": 1
                }
            }
        }
    ]
}
```

Scene-level environments provide a default. Node-level environments apply to emitters within the bound node's subtree or other implementation-defined local scope.

### 2.3 Reverb

Reverb simulates room acoustics.

| Property | Type | Description | Required |
|---|---|---|---|
| **type** | `string` | `parametric` or `impulseResponse`. Default: `parametric`. | No |
| **mix** | `number` | Wet/dry blend in the range `[0, 1]`. Default: `0.5`. | No |
| **roomSize** | `number` | Approximate room size in meters. Default: `10.0`. | No |
| **reflectivity** | `number` | Surface reflectivity in the range `[0, 1]`. Default: `0.5`. | No |
| **reflectivityHigh** | `number` | High-frequency reflectivity in the range `[0, 1]`. | No |
| **reflectivityLow** | `number` | Low-frequency reflectivity in the range `[0, 1]`. | No |
| **earlyReflections** | `integer` | Number of early reflections. Range: `[0, 32]`. Default: `8`. | No |
| **earlyReflectionsGain** | `number` | Gain applied to early reflections. Default: `1.0`. | No |
| **diffusionGain** | `number` | Gain applied to late diffuse reverb. Default: `1.0`. | No |
| **reflectionDelay** | `number` | Initial reflection delay in seconds. Default: `0.02`. | No |
| **reverbDelay** | `number` | Late reverb onset delay in seconds. Default: `0.04`. | No |
| **decayTime** | `number` | Reverb decay time in seconds. Default: `1.5`. | No |
| **audio** | `integer` | Index into `KHR_audio_emitter.audio[]` for an impulse-response asset. Required when `type` is `impulseResponse`. | No |

Parametric reverb example:

```json
{
    "environments": [
        {
            "name": "Large Cathedral",
            "reverb": {
                "type": "parametric",
                "mix": 0.6,
                "roomSize": 40.0,
                "reflectivity": 0.8,
                "reflectivityHigh": 0.6,
                "reflectivityLow": 0.9,
                "earlyReflections": 16,
                "earlyReflectionsGain": 1.2,
                "diffusionGain": 0.8,
                "reflectionDelay": 0.03,
                "reverbDelay": 0.06,
                "decayTime": 4.0
            }
        }
    ]
}
```

Impulse-response example:

```json
{
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [
                { "uri": "music.mp3" },
                { "uri": "cathedral_ir.wav" }
            ],
            "sources": [...],
            "emitters": [...]
        },
        "KHR_audio_environment": {
            "environments": [
                {
                    "name": "Cathedral IR",
                    "reverb": {
                        "type": "impulseResponse",
                        "mix": 0.5,
                        "audio": 1
                    }
                }
            ]
        }
    }
}
```

## 3. Custom Distance Models

`KHR_audio_emitter` defines `linear`, `inverse`, and `exponential` distance models. This extension adds a `custom` mode driven by a sampled attenuation curve.

Custom distance attenuation is declared as an extension on `KHR_audio_emitter.emitters[].positional`:

```json
{
    "extensions": {
        "KHR_audio_emitter": {
            "emitters": [
                {
                    "type": "positional",
                    "gain": 1.0,
                    "sources": [0],
                    "positional": {
                        "distanceModel": "custom",
                        "refDistance": 1.0,
                        "maxDistance": 50.0,
                        "rolloffFactor": 1.0,
                        "extensions": {
                            "KHR_audio_environment": {
                                "distanceCurve": [1.0, 0.8, 0.5, 0.3, 0.1, 0.0]
                            }
                        }
                    }
                }
            ]
        }
    }
}
```

### Custom Distance Properties

| Property | Type | Description | Required |
|---|---|---|---|
| **distanceCurve** | `number[]` | Gain samples, uniformly distributed from `refDistance` to `maxDistance`, with values in `[0, 1]`. | Required when `distanceModel` is `custom` |

## 4. Emitter Spatialization Override

Listeners define the default spatialization model, but individual emitters may override it on their `positional` object.

```json
{
    "extensions": {
        "KHR_audio_emitter": {
            "emitters": [
                {
                    "type": "positional",
                    "gain": 1.0,
                    "sources": [0],
                    "positional": {
                        "distanceModel": "inverse",
                        "refDistance": 1.0,
                        "maxDistance": 100.0,
                        "rolloffFactor": 1.0,
                        "extensions": {
                            "KHR_audio_environment": {
                                "spatializationModel": "HRTF"
                            }
                        }
                    }
                }
            ]
        }
    }
}
```

| Property | Type | Description | Required |
|---|---|---|---|
| **spatializationModel** | `string` | Override: `equalpower`, `HRTF`, or `custom`. | No |

## 5. Complete Example

```json
{
    "asset": { "version": "2.0" },
    "extensionsUsed": ["KHR_audio_emitter", "KHR_audio_environment"],
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [
                { "uri": "footsteps.mp3" },
                { "uri": "cathedral_ir.wav" }
            ],
            "sources": [
                { "audio": 0, "autoPlay": true, "loop": true, "gain": 1.0 }
            ],
            "emitters": [
                {
                    "type": "positional",
                    "gain": 0.8,
                    "sources": [0],
                    "positional": {
                        "distanceModel": "inverse",
                        "refDistance": 1.0,
                        "maxDistance": 50.0,
                        "rolloffFactor": 1.0
                    }
                }
            ]
        },
        "KHR_audio_environment": {
            "listeners": [
                {
                    "name": "Player",
                    "spatializationModel": "HRTF",
                    "interauralDistance": 0.17
                }
            ],
            "environments": [
                {
                    "name": "Cathedral",
                    "reverb": {
                        "type": "parametric",
                        "mix": 0.4,
                        "roomSize": 30.0,
                        "reflectivity": 0.75,
                        "decayTime": 3.5,
                        "earlyReflections": 12
                    }
                }
            ]
        }
    },
    "scenes": [
        {
            "name": "Main",
            "nodes": [0, 1],
            "extensions": {
                "KHR_audio_environment": {
                    "environment": 0
                }
            }
        }
    ],
    "nodes": [
        {
            "name": "PlayerCamera",
            "camera": 0,
            "extensions": {
                "KHR_audio_environment": {
                    "listener": 0
                }
            }
        },
        {
            "name": "Character",
            "translation": [5.0, 0.0, 3.0],
            "extensions": {
                "KHR_audio_emitter": {
                    "emitters": [0]
                }
            }
        }
    ]
}
```

## 6. glTF Object Model

### Mutable Properties

| JSON Pointer | Object Model Type | Description |
|---|---|---|
| `/extensions/KHR_audio_environment/listeners/{}/spatializationModel` | `string` | Listener spatialization model |
| `/extensions/KHR_audio_environment/listeners/{}/interauralDistance` | `float` | Listener interaural distance |
| `/extensions/KHR_audio_environment/environments/{}/reverb/mix` | `float` | Reverb wet/dry mix |
| `/extensions/KHR_audio_environment/environments/{}/reverb/roomSize` | `float` | Reverb room size |
| `/extensions/KHR_audio_environment/environments/{}/reverb/reflectivity` | `float` | Surface reflectivity |
| `/extensions/KHR_audio_environment/environments/{}/reverb/decayTime` | `float` | Reverb decay time |
| `/extensions/KHR_audio_environment/environments/{}/reverb/earlyReflectionsGain` | `float` | Early reflections gain |
| `/extensions/KHR_audio_environment/environments/{}/reverb/diffusionGain` | `float` | Late reverb gain |

### Read-Only Properties

| JSON Pointer | Object Model Type |
|---|---|
| `/extensions/KHR_audio_environment/listeners.length` | `int` |
| `/extensions/KHR_audio_environment/environments.length` | `int` |

## 7. JSON Schema Reference

The following schema files are defined in the `schema/` directory:

- `glTF.KHR_audio_environment.schema.json`
- `node.KHR_audio_environment.schema.json`
- `scene.KHR_audio_environment.schema.json`
- `emitter.positional.KHR_audio_environment.schema.json`
- `KHR_audio_environment.listener.schema.json`
- `KHR_audio_environment.hrtf.schema.json`
- `KHR_audio_environment.environment.schema.json`
- `KHR_audio_environment.reverb.schema.json`
- `KHR_audio_environment.distancecurve.schema.json`

## Appendix: Full Khronos Copyright Statement

Copyright 2013-2017 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.
