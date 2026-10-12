# KHR_audio_environment

## Contributors

- Chintan Shah, Meta
- Alexey Medvedev, Meta

Copyright 2024-2026 The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

- **Required**: `KHR_audio_emitter`
- **Optional**: `KHR_audio_graph` (for the listener-bus processing hook, section 3.5)

## Overview

This extension defines how audio in a glTF scene is *heard*: the listener, the spatialization model, and the acoustic environment (reverberation, distance-dependent filtering, Doppler). It is the third layer of the layered glTF audio architecture:

1. `KHR_audio_emitter` — what plays, and where it is placed (sources, emitters, attachment)
2. `KHR_audio_graph` — how source signals are processed (routing/mixing/filter DAG)
3. `KHR_audio_environment` — how the result is heard (listener, spatialization, acoustics)

Without this extension, `KHR_audio_emitter` relies on implementations to implicitly determine the listener (typically the active camera) and the spatialization algorithm. `KHR_audio_environment` makes these explicit and adds environmental acoustics.

### Motivation

Spatial audio quality depends on three factors beyond source/emitter placement:

1. **Listener**: Where is the listener? What are its properties? (head size, HRTF profile)
2. **Spatialization model**: How is 3D positioning computed? (HRTF, equal-power, custom)
3. **Environment**: What does the space sound like? (reverberation, distance filtering, Doppler)

Modern XR and game runtimes provide all three (X3D 4.0 Sound component Level 3, MPEG-I Immersive Audio, and the audio subsystems of the major engines converge on the same feature set). This extension brings them to glTF in a form that degrades gracefully: a file using this extension remains fully playable by an implementation that only supports `KHR_audio_emitter`.

### Design Principles

- Extends `KHR_audio_emitter` — does not replace it.
- Follows `KHR_audio_emitter` conventions: seconds, radians, Hz, linear gain `[0, +∞)`.
- Listener and its lifecycle are explicit and normative, with a defined implicit fallback.
- Spatialization model is selectable per listener and overridable per emitter.
- Environmental acoustics use an **abstract, engine-neutral parameter vocabulary** (decay, gains, delays, diffusion, density) plus **named presets**. Detailed or proprietary acoustic models (geometric acoustics, baked wave data, full legacy parameter sets) attach through the `extensions` object of the relevant environment objects rather than expanding the core vocabulary.
- Environment **zones** have normative shapes and selection rules, so the same asset sounds the same everywhere.
- Works with or without `KHR_audio_graph`.

---

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

---

## 1. Listener

The listener represents the point in the scene from which spatial audio is heard.

### 1.1 Listener Object

Listeners are defined at the document level in the `listeners[]` array.

| Property | Type | Description | Required |
|---|---|---|---|
| **name** | `string` | Human-readable name. | No |
| **gain** | `number` | Master gain applied to the final mix heard by this listener. Range `[0, +∞)`. Default: 1.0. | No |
| **spatializationModel** | `string` | Spatialization algorithm. See 1.4. | No |
| **hrtf** | `object` | HRTF configuration. See 1.5. | No |
| **interauralDistance** | `number` | Distance between ears in meters. Range `[0, +∞)`. Default: 0.17. | No |
| **graph** | `integer` | Index into `KHR_audio_graph.graphs[]`; listener-bus processing hook. See 3.5. | No |
| **extensions** | `object` | Extension-specific objects. | No |
| **extras** | `any` | Application-specific data. | No |

### 1.2 Listener Binding

Listeners are bound to glTF nodes (typically cameras) via a node extension. The listener inherits **position and orientation** from the node; these are not duplicated on the listener object. The listener faces the node's **-Z** axis, matching glTF cameras.

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

| Property | Type | Description | Required |
|---|---|---|---|
| **listener** | `integer` | Index into `KHR_audio_environment.listeners[]`. | Yes |

### 1.3 Listener Lifecycle (normative)

Exactly **one listener is active** at any time. The active listener is determined in this order:

1. If the scene extension declares `activeListener` (see 2.2), the listener with that index is active, heard from the node it is bound to.
2. Otherwise, if a listener is bound to the node of the currently active camera, that listener is active.
3. Otherwise, the first listener binding encountered in `scene.nodes` order (depth-first) is active.
4. Otherwise (no listener declared anywhere), the **implicit listener** is active: the viewer pose (active camera), with `spatializationModel: "equalpower"` and all listener defaults.

Rule 4 makes this extension strictly additive: a `KHR_audio_emitter`-only implementation and a `KHR_audio_environment` implementation render an emitter-only asset identically.

If the node binding the active listener is removed or disabled at runtime, implementations fall back through the same order.

### 1.4 Spatialization Model

The `spatializationModel` property defines how positional audio emitters are spatialized relative to the listener.

| Value | Description |
|---|---|
| `equalpower` | Equal-power panning. Simple and efficient. Produces a stereo image. **Default.** |
| `HRTF` | Head-Related Transfer Function. Renders binaural audio using measured or modeled impulse responses. Higher-quality spatial positioning than equal-power. |
| `custom` | User-defined spatialization, described by the `hrtf` property or by application-specific extensions. |

**Fallback (normative)**: an implementation that does not support the requested model MUST fall back to `equalpower` and MUST NOT fail to load the asset.

### 1.5 HRTF Configuration

When `spatializationModel` is `HRTF` or `custom`, optional HRTF configuration may be provided.

| Property | Type | Description | Required |
|---|---|---|---|
| **audio** | `integer` | Index into `KHR_audio_emitter.audio[]` referencing an HRTF impulse-response dataset. | No |
| **profile** | `string` | Named HRTF profile hint: `generic`, `small`, `medium`, `large`. Implementations may use this to select a built-in HRTF dataset. Default: `generic`. | No |

```json
{
    "listeners": [
        {
            "name": "Player Listener",
            "gain": 1.0,
            "spatializationModel": "HRTF",
            "hrtf": { "profile": "medium" }
        }
    ]
}
```

> **Future work**: per-direction HRTF datasets are expected to reference the AES69 (SOFA) container through a dedicated extension. The `audio` reference above is suitable only for single-IR approximations.

---

## 2. Environment

Environments define the acoustic characteristics of a space: reverberation and Doppler propagation. An environment applies to emitters according to the zone rules in 2.3.

### 2.1 Environment Object

Environments are defined at the document level in the `environments[]` array.

| Property | Type | Description | Required |
|---|---|---|---|
| **name** | `string` | Human-readable name. | No |
| **reverb** | `object` | Reverberation properties. See 2.4. | No |
| **doppler** | `object` | Doppler properties. See 2.5. | No |
| **extensions** | `object` | Extension-specific objects. Detailed acoustic models (geometric acoustics, baked acoustic data, legacy parameter sets) attach here. | No |
| **extras** | `any` | Application-specific data. | No |

### 2.2 Environment and Listener Binding on Scenes

The scene-level extension declares the **default environment** (the fallback when the listener is inside no zone) and optionally pins the active listener:

```json
{
    "scenes": [
        {
            "name": "Cathedral",
            "extensions": {
                "KHR_audio_environment": {
                    "environment": 0,
                    "activeListener": 0
                }
            }
        }
    ]
}
```

| Property | Type | Description | Required |
|---|---|---|---|
| **environment** | `integer` | Index into `environments[]`; the scene's default environment. | No |
| **activeListener** | `integer` | Index into `listeners[]`; pins the active listener (see 1.3). | No |

### 2.3 Environment Zones (normative)

A node-level extension binds an environment to a **spatial zone**:

```json
{
    "nodes": [
        {
            "name": "TunnelZone",
            "translation": [0, 0, -20],
            "extensions": {
                "KHR_audio_environment": {
                    "environment": 1,
                    "shape": { "type": "box", "size": [4.0, 3.0, 30.0] },
                    "blendDistance": 1.0,
                    "priority": 1
                }
            }
        }
    ]
}
```

| Property | Type | Description | Required |
|---|---|---|---|
| **environment** | `integer` | Index into `environments[]`. | Yes |
| **shape** | `object` | Zone volume in node-local space. See below. | Yes |
| **blendDistance** | `number` | Width in meters of the crossfade band, measured inward from the zone boundary. Default: 0.0 (hard switch). | No |
| **priority** | `integer` | Zone precedence when zones overlap; higher wins. Default: 0. | No |

**Shape object**:

| Property | Type | Description | Required |
|---|---|---|---|
| **type** | `string` | `"box"` or `"sphere"`. Additional shapes (e.g., mesh volumes) may be defined by extensions. | Yes |
| **size** | `number[3]` | Box only: full extents along local X/Y/Z, centered at the node origin. | When `box` |
| **radius** | `number` | Sphere only: radius, centered at the node origin. | When `sphere` |

The shape is defined in the node's local space and transformed by the node's global transform. Non-uniform scale applies to boxes; for spheres, the maximum scale component applies to the radius.

**Zone selection (normative)**:

1. The **listener's world position** determines the active zone. (Environmental acoustics follow the listener, matching engine convention; emitters may be forced to a specific environment via the emitter override in 3.1.)
2. If the listener is inside multiple zones, the zone with the highest `priority` is active. Among equal priorities, the zone with the smallest volume is active.
3. If the listener is inside no zone, the scene's default environment (2.2) is active. If the scene declares none, no environmental processing is applied.
4. Within `blendDistance` of the active zone's boundary (inside it), implementations SHOULD crossfade the reverb send levels between the zone's environment and the environment that would be active just outside the boundary.

### 2.4 Reverb Properties

Reverberation uses an abstract parametric vocabulary shared by standard parametric reverberators (the parameter set is aligned with the I3DL2 listener-property model, expressed in glTF units), or a measured impulse response.

| Property | Type | Description | Required |
|---|---|---|---|
| **type** | `string` | `parametric` or `impulseResponse`. Default: `parametric`. | No |
| **preset** | `string` | Named starting point for parametric reverb. See 2.4.1. | No |
| **mix** | `number` | Return level of the environment's reverb bus into the final mix. 0 = reverb inaudible, 1 = full level. Default: 0.5. | No |
| **decayTime** | `number` | RT60 decay time in seconds, mid frequencies. Range `(0, +∞)`. Default: 1.5. | No |
| **decayHFRatio** | `number` | Ratio of high-frequency decay time to `decayTime`. Range `(0, 2]`. Values below 1.0 model air/material absorption of highs. Default: 0.83. | No |
| **reflectionsGain** | `number` | Level of early reflections, linear. Range `[0, +∞)`. Default: 1.0. | No |
| **reflectionsDelay** | `number` | Delay of the first reflection relative to the direct sound, in seconds. Default: 0.02. | No |
| **reverbGain** | `number` | Level of the late diffuse tail, linear. Range `[0, +∞)`. Default: 1.0. | No |
| **reverbDelay** | `number` | Onset of the late tail relative to the first reflection, in seconds. Default: 0.04. | No |
| **diffusion** | `number` | Echo density of the late tail. Range `[0, 1]`. Default: 1.0. | No |
| **density** | `number` | Modal density of the late tail. Range `[0, 1]`. Default: 1.0. | No |
| **audio** | `integer` | Index into `KHR_audio_emitter.audio[]` for impulse-response data. Required when `type` is `impulseResponse`. | No |
| **normalize** | `boolean` | Impulse response only: apply equal-power normalization to the IR. Default: true. | No |
| **extensions** | `object` | Extension-specific objects. Richer models attach here without changing the core vocabulary. | No |

When both `preset` and explicit parameters are present, explicit parameters override the preset's values. When `type` is `impulseResponse`, the parametric properties other than `mix` are ignored.

#### 2.4.1 Presets

Presets give compact files and cross-engine recognizability; the vocabulary follows the I3DL2/EAX lineage that survives in today's engines. Implementations MUST treat a preset as a full assignment of the parametric properties.

`generic`, `smallRoom`, `mediumRoom`, `largeRoom`, `bathroom`, `concertHall`, `cathedral`, `cave`, `arena`, `hangar`, `corridor`, `forest`, `underwater`

Informative reference values (final values to be normative in the ratified spec; derived from the I3DL2 preset table, converted to glTF units):

| Preset | decayTime | decayHFRatio | reflectionsDelay | reverbDelay |
|---|---|---|---|---|
| generic | 1.49 | 0.83 | 0.007 | 0.011 |
| smallRoom | 0.40 | 0.83 | 0.002 | 0.003 |
| mediumRoom | 1.10 | 0.83 | 0.010 | 0.011 |
| largeRoom | 4.32 | 0.59 | 0.020 | 0.030 |
| bathroom | 1.49 | 0.54 | 0.007 | 0.011 |
| concertHall | 3.92 | 0.70 | 0.020 | 0.029 |
| cathedral | 5.50 | 0.60 | 0.025 | 0.040 |
| cave | 2.91 | 1.30 | 0.015 | 0.022 |
| arena | 7.24 | 0.33 | 0.020 | 0.030 |
| hangar | 10.05 | 0.23 | 0.020 | 0.030 |
| corridor | 1.79 | 0.59 | 0.007 | 0.011 |
| forest | 1.49 | 0.54 | 0.162 | 0.088 |
| underwater | 1.49 | 0.10 | 0.007 | 0.011 |

#### Parametric Reverb Example

```json
{
    "environments": [
        {
            "name": "Large Cathedral",
            "reverb": {
                "type": "parametric",
                "preset": "cathedral",
                "mix": 0.6,
                "decayTime": 4.0
            }
        }
    ]
}
```

#### Impulse Response Reverb Example

```json
{
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [
                { "uri": "music.mp3" },
                { "uri": "cathedral_ir.wav" }
            ],
            "sources": ["..."],
            "emitters": ["..."]
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

**Web Audio Mapping (informative)**: each active environment is one shared reverb bus. Emitters feed the bus via their `reverbLevel` send gain (3.1); the bus output is scaled by `mix` and summed into the destination. Impulse-response reverb maps to `ConvolverNode` (1, 2, or 4 channel IRs, per Web Audio conventions) with `normalize` as specified.

**Parametric realization (recommendation)**: implementations SHOULD realize parametric reverb by *generating an impulse response* from the parametric values (early-reflection taps after `reflectionsDelay`, followed by an exponentially decaying noise tail with RT60 = `decayTime`, low-passed per `decayHFRatio`) and rendering it by convolution. Convolution has no feedback path and is unconditionally stable. Algorithmic (feedback-network) realizations are permitted but MUST remain stable under sustained wide-band input; naive single-comb topologies are known to accumulate energy and are discouraged. (This recommendation comes directly from reference-implementation experience.)

### 2.5 Doppler Properties

Doppler shift is declared per environment and disabled by default (matching the Web Audio API, which provides no Doppler facility; X3D 4.0 Level 3 and MPEG-I include it).

| Property | Type | Description | Required |
|---|---|---|---|
| **enabled** | `boolean` | Enable Doppler shift for positional emitters in this environment. Default: false. | No |
| **scale** | `number` | Exaggeration factor applied to relative velocities. Range `[0, +∞)`. Default: 1.0. | No |
| **speedOfSound** | `number` | Propagation speed in meters/second. Default: 343.0. | No |

**Model (normative)**: for each positional emitter, the rendered pitch factor is

```
pitch = (c + s·vL) / (c − s·vS)
```

where `c` is `speedOfSound`, `s` is `scale`, `vL` is the radial velocity of the listener toward the emitter, and `vS` is the radial velocity of the emitter toward the listener (both derived from node transforms; positive = approaching, so approach raises pitch from either side). Implementations MUST clamp the denominator to remain positive (and the numerator non-negative) and SHOULD smooth velocity estimates across frames. Individual emitters may opt out via `dopplerEnabled: false` (3.2).

**Interaction with `KHR_audio_graph` (normative)**: Doppler is a per-emitter effect, but pitch-based realizations act on source playback. When an emitter is fed by a graph output whose graph consumes sources shared with other emitters (no 1:1 source-to-emitter path), implementations MAY omit Doppler for that emitter. Authors requiring Doppler SHOULD keep Doppler-critical emitters on direct (non-graph) source paths or give them dedicated graph sources.

---

## 3. Emitter Integration

This extension adds properties to `KHR_audio_emitter` emitters through the standard `extensions` mechanism, at two levels: on the **emitter object** (routing into the environment) and on its **`positional` object** (spatial rendering detail).

### 3.1 Emitter-Level Properties: Direct and Reverb Sends

```json
{
    "emitters": [
        {
            "type": "positional",
            "gain": 0.8,
            "sources": [0],
            "positional": { "...": "..." },
            "extensions": {
                "KHR_audio_environment": {
                    "directLevel": 1.0,
                    "reverbLevel": 0.7
                }
            }
        }
    ]
}
```

| Property | Type | Description | Required |
|---|---|---|---|
| **directLevel** | `number` | Gain of the emitter's direct (dry) path. Range `[0, +∞)`. Default: 1.0. | No |
| **reverbLevel** | `number` | Gain of the emitter's send into the active environment's reverb bus. Range `[0, +∞)`. Default: 1.0 for `positional` emitters, 0.0 for `global` emitters. | No |
| **environment** | `integer` | Force this emitter's reverb send to a specific environment, regardless of the listener's active zone. | No |

Per-emitter sends are the standard engine model (aux sends; direct vs reverberant level) and enable "distant but dry" / "close but wet" authoring that a single global wet/dry mix cannot express. Distance-dependent send shaping can be layered by animating `reverbLevel` or via future extensions.

### 3.2 Positional Properties

Declared under `positional.extensions.KHR_audio_environment`:

| Property | Type | Description | Required |
|---|---|---|---|
| **spatializationModel** | `string` | Per-emitter override of the listener's model: `equalpower`, `HRTF`, `custom`. | No |
| **distanceCurve** | `number[]` | Custom distance attenuation. See 3.3. | No |
| **airAbsorption** | `object` | Distance-dependent low-pass filtering. See 3.4. | No |
| **coneOuterCutoff** | `number` | Low-pass cutoff in Hz applied at and outside the cone's outer angle. See 3.4. | No |
| **dopplerEnabled** | `boolean` | Per-emitter Doppler opt-out/in within the environment's Doppler settings. Default: true (follows environment). | No |

### 3.3 Custom Distance Models

`KHR_audio_emitter` defines three distance models: `linear`, `inverse`, `exponential`. When the base emitter sets `distanceModel: "custom"`, the `distanceCurve` in this extension defines attenuation:

| Property | Type | Description | Required |
|---|---|---|---|
| **distanceCurve** | `number[]` | Gain values in `[0, 1]`, sampled uniformly from `refDistance` (index 0) to `maxDistance` (last index), linearly interpolated. At least 2 values. | Yes, when `distanceModel` is `custom` |

`maxDistance` MUST be greater than `refDistance` when a `distanceCurve` is used. Custom curves replace the three fixed models' single-knob rolloff with authored attenuation — the mechanism engines use (attenuation curves) expressed declaratively.

```json
{
    "positional": {
        "distanceModel": "custom",
        "refDistance": 1.0,
        "maxDistance": 50.0,
        "extensions": {
            "KHR_audio_environment": {
                "distanceCurve": [1.0, 0.8, 0.5, 0.3, 0.1, 0.0]
            }
        }
    }
}
```

### 3.4 Distance and Directivity Filtering

Volume-only attenuation is the most audible difference between a basic renderer and a modern engine. Two optional filters close that gap:

**Air absorption** — a low-pass filter whose cutoff falls with distance:

| Property | Type | Description | Required |
|---|---|---|---|
| **enabled** | `boolean` | Default: false. | No |
| **cutoffAtMaxDistance** | `number` | Cutoff frequency in Hz when the listener is at `maxDistance`. Default: 5000.0. | No |

The cutoff is 20000 Hz at `refDistance`, interpolated to `cutoffAtMaxDistance` at `maxDistance`, linearly in log-frequency over the distance fraction. Requires `maxDistance > refDistance`; when the base emitter has `maxDistance: 0` (no maximum), air absorption MUST be ignored.

**Cone low-pass** (`coneOuterCutoff`) — directional emitters sound *darker*, not just quieter, off-axis. No filtering at or inside `coneInnerAngle`; at and outside `coneOuterAngle` a low-pass at `coneOuterCutoff` Hz applies; between the angles the cutoff is interpolated linearly in log-frequency. Applies only when the base emitter's `shapeType` is `cone`. When absent, no directional filtering occurs.

**Web Audio Mapping (informative)**: both filters map to a per-emitter `BiquadFilterNode` (`lowpass`) whose `frequency` is updated from listener–emitter distance and angle each frame; the two cutoffs multiply in the log domain (i.e., the lower effective cutoff wins with soft combination).

### 3.5 Listener-Bus Processing Hook

When `KHR_audio_graph` is present, a listener may reference a graph applied to the **final mix** (after spatialization and reverb return, before output):

```json
{
    "listeners": [
        { "name": "Player", "spatializationModel": "HRTF", "graph": 2 }
    ]
}
```

The referenced graph MUST NOT declare `inputs[]` or `outputs[]` bindings. The implementation routes the final mix into every entry node (nodes with no incoming connection) and takes the result from terminal nodes (nodes with no outgoing connection). This provides the master-bus insert point (mastering EQ, dynamics) that `KHR_audio_graph` deliberately leaves out of the pre-spatialization graph. Implementations that do not support `KHR_audio_graph` MUST skip the hook and play the unprocessed mix.

---

## 4. Complete Example

A scene with a listener, a default environment, a tunnel zone, and a spatially-processed emitter:

```json
{
    "asset": { "version": "2.0" },
    "extensionsUsed": ["KHR_audio_emitter", "KHR_audio_environment"],
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [{ "uri": "footsteps.mp3" }],
            "sources": [{ "audio": 0, "autoplay": true, "loop": true, "gain": 1.0 }],
            "emitters": [
                {
                    "type": "positional",
                    "gain": 0.8,
                    "sources": [0],
                    "positional": {
                        "shapeType": "cone",
                        "coneInnerAngle": 1.0472,
                        "coneOuterAngle": 2.0944,
                        "coneOuterGain": 0.25,
                        "distanceModel": "inverse",
                        "refDistance": 1.0,
                        "maxDistance": 50.0,
                        "rolloffFactor": 1.0,
                        "extensions": {
                            "KHR_audio_environment": {
                                "airAbsorption": { "enabled": true, "cutoffAtMaxDistance": 4000.0 },
                                "coneOuterCutoff": 3000.0
                            }
                        }
                    },
                    "extensions": {
                        "KHR_audio_environment": {
                            "directLevel": 1.0,
                            "reverbLevel": 0.7
                        }
                    }
                }
            ]
        },
        "KHR_audio_environment": {
            "listeners": [
                {
                    "name": "Player",
                    "gain": 1.0,
                    "spatializationModel": "HRTF",
                    "interauralDistance": 0.17
                }
            ],
            "environments": [
                {
                    "name": "Courtyard",
                    "reverb": { "preset": "generic", "mix": 0.3 },
                    "doppler": { "enabled": true, "scale": 1.0 }
                },
                {
                    "name": "Tunnel",
                    "reverb": { "preset": "corridor", "mix": 0.6, "decayTime": 2.5 }
                }
            ]
        }
    },
    "scenes": [
        {
            "name": "Main",
            "nodes": [0, 1, 2],
            "extensions": {
                "KHR_audio_environment": { "environment": 0, "activeListener": 0 }
            }
        }
    ],
    "nodes": [
        {
            "name": "PlayerCamera",
            "camera": 0,
            "extensions": { "KHR_audio_environment": { "listener": 0 } }
        },
        {
            "name": "Character",
            "translation": [5.0, 0.0, 3.0],
            "extensions": { "KHR_audio_emitter": { "emitters": [0] } }
        },
        {
            "name": "TunnelZone",
            "translation": [0.0, 0.0, -20.0],
            "extensions": {
                "KHR_audio_environment": {
                    "environment": 1,
                    "shape": { "type": "box", "size": [4.0, 3.0, 30.0] },
                    "blendDistance": 1.0,
                    "priority": 1
                }
            }
        }
    ]
}
```

---

## 5. glTF Object Model

### Mutable Properties

| JSON Pointer | Object Model Type | Description |
|---|---|---|
| `/extensions/KHR_audio_environment/listeners/{}/gain` | `float` | Listener master gain |
| `/extensions/KHR_audio_environment/listeners/{}/interauralDistance` | `float` | Inter-aural distance |
| `/extensions/KHR_audio_environment/environments/{}/reverb/mix` | `float` | Reverb return level |
| `/extensions/KHR_audio_environment/environments/{}/reverb/decayTime` | `float` | RT60 decay time |
| `/extensions/KHR_audio_environment/environments/{}/reverb/decayHFRatio` | `float` | HF decay ratio |
| `/extensions/KHR_audio_environment/environments/{}/reverb/reflectionsGain` | `float` | Early reflections gain |
| `/extensions/KHR_audio_environment/environments/{}/reverb/reflectionsDelay` | `float` | First reflection delay |
| `/extensions/KHR_audio_environment/environments/{}/reverb/reverbGain` | `float` | Late reverb gain |
| `/extensions/KHR_audio_environment/environments/{}/reverb/reverbDelay` | `float` | Late reverb delay |
| `/extensions/KHR_audio_environment/environments/{}/reverb/diffusion` | `float` | Late tail echo density |
| `/extensions/KHR_audio_environment/environments/{}/reverb/density` | `float` | Late tail modal density |
| `/extensions/KHR_audio_environment/environments/{}/doppler/scale` | `float` | Doppler exaggeration |
| `/extensions/KHR_audio_environment/environments/{}/doppler/speedOfSound` | `float` | Propagation speed |
| `/extensions/KHR_audio_emitter/emitters/{}/extensions/KHR_audio_environment/directLevel` | `float` | Emitter dry level |
| `/extensions/KHR_audio_emitter/emitters/{}/extensions/KHR_audio_environment/reverbLevel` | `float` | Emitter reverb send |
| `/extensions/KHR_audio_emitter/emitters/{}/positional/extensions/KHR_audio_environment/coneOuterCutoff` | `float` | Cone low-pass cutoff |
| `/extensions/KHR_audio_emitter/emitters/{}/positional/extensions/KHR_audio_environment/airAbsorption/cutoffAtMaxDistance` | `float` | Air absorption cutoff |

Note: `spatializationModel` and `preset` are intentionally not animatable (string discriminators, matching the base spec's treatment of `distanceModel`/`shapeType`).

### Read-Only Properties

| JSON Pointer | Object Model Type |
|---|---|
| `/extensions/KHR_audio_environment/listeners.length` | `int` |
| `/extensions/KHR_audio_environment/environments.length` | `int` |

---

## 6. Future Work (informative)

Named here so reviewers can see what was considered and deliberately layered out:

- **Ambient beds**: orientation-locked, position-free multichannel/ambisonic environment sources (audio skyboxes) with ambisonics metadata (order, AmbiX/FuMa ordering, SN3D/N3D normalization). Blocked on multichannel codec support in the base layer.
- **Acoustic materials**: frequency-banded absorption/scattering/transmission on glTF materials (X3D `AcousticProperties` analog), as a separate `KHR_materials_acoustic` extension composing with this one.
- **Rooms, portals, occlusion, baked acoustics**: geometric and wave-based acoustics hooks; expected to attach via `environment.extensions`.
- **Voice management**: max-voice limits and normative priority ordering, coordinated with the `priority` property discussion in the base layers.
- **Mesh zone shapes**: `shape.type` values beyond `box`/`sphere` via extensions.
- **SOFA (AES69) HRTF datasets**; **multiple simultaneous listeners** (virtual microphones, X3D `ListenerPointSource` analog).
- **AR acoustics**: implementations MAY substitute estimated real-room acoustics for the declared environment in passthrough/AR contexts; a future extension may make this authorable.

---

## 7. JSON Schema Reference

Schema files (in `schema/`):

- `glTF.KHR_audio_environment.schema.json` — Document-level extension
- `listener.schema.json` — Listener object
- `hrtf.schema.json` — HRTF configuration
- `environment.schema.json` — Environment object
- `reverb.schema.json` — Reverb properties
- `doppler.schema.json` — Doppler properties
- `zoneShape.schema.json` — Zone shape (box/sphere)
- `node.KHR_audio_environment.schema.json` — Node-level extension (listener binding or environment zone)
- `scene.KHR_audio_environment.schema.json` — Scene-level extension (default environment, active listener)
- `emitter.KHR_audio_environment.schema.json` — Emitter-level extension (directLevel, reverbLevel, environment override)
- `positional.KHR_audio_environment.schema.json` — Positional extension (spatialization override, distance curve, air absorption, cone cutoff, doppler opt-out)
- `airAbsorption.schema.json` — Air absorption properties

---

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
