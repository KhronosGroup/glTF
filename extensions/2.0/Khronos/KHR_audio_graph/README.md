# KHR_audio_graph

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
- Optional: `KHR_audio_environment`

## Overview

This extension adds graph-based audio processing to `KHR_audio_emitter`. It allows authors to route one or more `KHR_audio_emitter` sources through a directed acyclic graph (DAG) of processors before sending the result to one or more `KHR_audio_emitter` emitters or to the implementation's global audio destination.

`KHR_audio_graph` does not replace `KHR_audio_emitter`. Scene and node attachment, base audio asset references, and emitter placement remain defined by `KHR_audio_emitter`. This extension adds:

- Document-level graph declarations.
- Explicit graph bindings from `KHR_audio_emitter.sources[]` into graph inputs.
- Explicit graph bindings from graph outputs to `KHR_audio_emitter.emitters[]`.
- Graph-only procedural sources through oscillator nodes.
- Optional encoding metadata and extended playback properties on `KHR_audio_emitter` audio and source objects.

### Motivation

Modern audio engines use graph-based routing to mix, filter, spatialize, and procedurally generate sound. A flat `audio -> source -> emitter` model is sufficient for simple playback, but it cannot express:

- Multi-source mixes.
- Shared processing chains.
- Equalization and filtering.
- Delay and distortion effects.
- Explicit channel routing.
- Procedural graph-only sources.

`KHR_audio_graph` preserves the simple `KHR_audio_emitter` workflow while adding those capabilities when needed.

### Design Principles

- Extends `KHR_audio_emitter`; it does not duplicate its document model.
- Keeps scene attachment in `KHR_audio_emitter`, not in `KHR_audio_graph`.
- Uses seconds for timing and radians for angles, matching glTF and `KHR_audio_emitter`.
- Uses non-negative linear gain values, matching `KHR_audio_emitter`.
- Represents routing explicitly through graph nodes and connections.
- Keeps environmental acoustics and listener behavior in `KHR_audio_environment`.

## Extension Declaration

Add `KHR_audio_graph` to `extensionsUsed`. The extension payload is declared under the document root `extensions` object.

```json
{
    "extensionsUsed": ["KHR_audio_emitter", "KHR_audio_graph"],
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [...],
            "sources": [...],
            "emitters": [...]
        },
        "KHR_audio_graph": {
            "graphs": [...]
        }
    }
}
```

## Conventions

- Time values are expressed in seconds.
- Angles are expressed in radians.
- Gain values are linear, unitless multipliers in the range `[0, +inf)`.
- Frequencies are expressed in Hertz.
- Connections use graph-local node indices with optional `input` and `output` port indices. Omitted ports refer to port `0`.
- Graphs are directed acyclic graphs. Cycles are not permitted.

## 1. Graphs

Graphs are the top-level construct of this extension. Each graph contains nodes, edges between nodes, optional bindings from `KHR_audio_emitter` sources, and optional bindings to `KHR_audio_emitter` emitters.

### 1.1 Graph Object

| Property | Type | Description | Required |
|---|---|---|---|
| **name** | `string` | Human-readable name for debugging. | No |
| **nodes** | `node[]` | Array of graph nodes. | Yes |
| **connections** | `connection[]` | Array of edges between node endpoints. | Yes |
| **inputs** | `graphInput[]` | Bindings from `KHR_audio_emitter.sources[]` into graph node inputs. | No |
| **outputs** | `graphOutput[]` | Bindings from graph node outputs into `KHR_audio_emitter.emitters[]`. | No |

A valid graph must have at least one sink. A sink is either:

- A graph output entry in `outputs[]`, or
- A graph with no `outputs[]`, where the implementation routes terminal nodes to the global audio destination.

### 1.2 Graph Inputs

Graph inputs bind an existing `KHR_audio_emitter` source to a graph node input.

| Property | Type | Description | Required |
|---|---|---|---|
| **source** | `integer` | Index into `KHR_audio_emitter.sources[]`. | Yes |
| **node** | `integer` | Target node index in this graph. | Yes |
| **input** | `integer` | Input port on the target node. Default: `0`. | No |

```json
{
    "inputs": [
        { "source": 0, "node": 0 },
        { "source": 1, "node": 0, "input": 1 }
    ]
}
```

### 1.3 Graph Outputs

Graph outputs bind a graph node output to a `KHR_audio_emitter` emitter.

| Property | Type | Description | Required |
|---|---|---|---|
| **node** | `integer` | Source node index in this graph. | Yes |
| **output** | `integer` | Output port on the source node. Default: `0`. | No |
| **emitter** | `integer` | Index into `KHR_audio_emitter.emitters[]`. | Yes |

```json
{
    "outputs": [
        { "node": 2, "emitter": 0 },
        { "node": 3, "output": 1, "emitter": 1 }
    ]
}
```

### 1.4 Connections

Connections describe graph-internal edges.

| Property | Type | Description | Required |
|---|---|---|---|
| **from** | `endpoint` | Upstream endpoint. | Yes |
| **to** | `endpoint` | Downstream endpoint. | Yes |

Endpoint objects use the following properties:

| Property | Type | Description | Required |
|---|---|---|---|
| **node** | `integer` | Graph-local node index. | Yes |
| **output** | `integer` | Output port index. | No |
| **input** | `integer` | Input port index. | No |

```json
{
    "connections": [
        { "from": { "node": 0 }, "to": { "node": 1 } },
        { "from": { "node": 1, "output": 0 }, "to": { "node": 2, "input": 0 } }
    ]
}
```

### 1.5 Complete Graph Example

Two `KHR_audio_emitter` sources are mixed, filtered, and routed into one emitter.

```json
{
    "extensionsUsed": ["KHR_audio_emitter", "KHR_audio_graph"],
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [
                { "uri": "music.mp3" },
                { "uri": "ambience.mp3" }
            ],
            "sources": [
                { "audio": 0, "autoPlay": true, "loop": true, "gain": 1.0 },
                { "audio": 1, "autoPlay": true, "loop": true, "gain": 0.5 }
            ],
            "emitters": [
                { "type": "global", "gain": 1.0, "sources": [] }
            ]
        },
        "KHR_audio_graph": {
            "graphs": [
                {
                    "name": "music-mix",
                    "nodes": [
                        { "kind": "audiomixer", "params": {} },
                        { "kind": "gain", "params": { "gain": 0.8 } },
                        { "kind": "lowpass", "params": { "frequency": 8000.0 } }
                    ],
                    "connections": [
                        { "from": { "node": 0 }, "to": { "node": 1 } },
                        { "from": { "node": 1 }, "to": { "node": 2 } }
                    ],
                    "inputs": [
                        { "source": 0, "node": 0 },
                        { "source": 1, "node": 0 }
                    ],
                    "outputs": [
                        { "node": 2, "emitter": 0 }
                    ]
                }
            ]
        }
    }
}
```

## 2. Node Kinds

Each graph node is defined by a `kind` discriminator and a `params` object.

| Property | Type | Description | Required |
|---|---|---|---|
| **kind** | `string` | Node type identifier. | Yes |
| **params** | `object` | Node-kind-specific parameters. | Yes |
| **label** | `string` | Human-readable label for debugging. | No |
| **bypass** | `boolean` | When `true`, processing and filter nodes are bypassed. Default: `false`. | No |

### Node Kind Summary

| Kind | Category | I/O |
|---|---|---|
| `oscillator` | Source | `0 -> 1` |
| `gain` | Processing | `1 -> 1` |
| `delay` | Processing | `1 -> 1` |
| `waveshaper` | Processing | `1 -> 1` |
| `lowpass` | Filter | `1 -> 1` |
| `highpass` | Filter | `1 -> 1` |
| `bandpass` | Filter | `1 -> 1` |
| `lowshelf` | Filter | `1 -> 1` |
| `highshelf` | Filter | `1 -> 1` |
| `peaking` | Filter | `1 -> 1` |
| `notch` | Filter | `1 -> 1` |
| `allpass` | Filter | `1 -> 1` |
| `splitter` | Channel routing | `1 -> N` |
| `channelmerger` | Channel routing | `N -> 1` |
| `channelmixer` | Channel routing | `1 -> 1` |
| `audiomixer` | Channel routing | `N -> 1` |

## 3. Source Nodes

Source nodes generate audio within the graph. `KHR_audio_graph` defines one graph-only source node: the oscillator.

### 3.1 Oscillator Node

The oscillator node generates a periodic waveform. It does not reference `KHR_audio_emitter.audio[]` or `KHR_audio_emitter.sources[]`.

| Property | Type | Description | Required |
|---|---|---|---|
| **type** | `string` | Waveform: `sine`, `square`, `triangle`, `sawtooth`, `custom`. | Yes |
| **frequency** | `number` | Frequency in Hertz. Default: `440`. | No |
| **detune** | `number` | Detune in cents. Default: `0`. | No |
| **pulseWidth** | `number` | Pulse width for `square` waveforms. Range: `[0, 1]`. Default: `0.5`. | No |

Informative Web Audio mapping: implementations typically map this node to `OscillatorNode`, optionally using `PeriodicWave` when `type` is `square` and `pulseWidth` is not `0.5`.

```json
{
    "name": "test-tone",
    "nodes": [
        { "kind": "oscillator", "params": { "type": "sine", "frequency": 440.0 } },
        { "kind": "gain", "params": { "gain": 0.3 } }
    ],
    "connections": [
        { "from": { "node": 0 }, "to": { "node": 1 } }
    ],
    "outputs": [
        { "node": 1, "emitter": 0 }
    ]
}
```

## 4. Processing Nodes

Processing nodes transform audio. Unless otherwise noted, they have one input and one output. Processing and filter nodes support `bypass`.

### 4.1 Gain Node

| Property | Type | Description | Required |
|---|---|---|---|
| **gain** | `number` | Linear gain multiplier. Default: `1.0`. | No |
| **interpolation** | `string` | Gain transition curve: `linear` or `custom`. Default: `linear`. | No |
| **duration** | `number` | Interpolation duration in seconds. Default: `0`. | No |

### 4.2 Delay Node

| Property | Type | Description | Required |
|---|---|---|---|
| **delayTime** | `number` | Delay time in seconds. Default: `0`. | No |
| **maxDelayTime** | `number` | Maximum supported delay time in seconds. Default: `1.0`. | No |

### 4.3 Waveshaper Node

| Property | Type | Description | Required |
|---|---|---|---|
| **amount** | `number` | Distortion amount in the range `[0, 1]`. | No |
| **oversample** | `string` | Oversampling factor: `none`, `2x`, `4x`. Default: `none`. | No |
| **curve** | `number[]` | Explicit shaping curve in the range `[-1, 1]`. | No |

## 5. Filter Nodes

Filter nodes implement biquad-style filtering. Each has one input and one output.

### 5.1 Lowpass Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Cutoff frequency in Hertz. | Yes |
| **qualityFactor** | `number` | Resonance at the cutoff frequency. Default: `1.0`. | No |

### 5.2 Highpass Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Cutoff frequency in Hertz. | Yes |
| **qualityFactor** | `number` | Resonance at the cutoff frequency. Default: `1.0`. | No |

### 5.3 Bandpass Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Center frequency in Hertz. | Yes |
| **qualityFactor** | `number` | Bandwidth control. Default: `1.0`. | No |

### 5.4 Lowshelf Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Upper limit frequency for the shelf in Hertz. | Yes |
| **gain** | `number` | Shelf gain in decibels. Default: `0`. | No |

### 5.5 Highshelf Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Lower limit frequency for the shelf in Hertz. | Yes |
| **gain** | `number` | Shelf gain in decibels. Default: `0`. | No |

### 5.6 Peaking Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Center frequency in Hertz. | Yes |
| **qualityFactor** | `number` | Bandwidth control. Default: `1.0`. | No |
| **gain** | `number` | Gain in decibels. Default: `0`. | No |

### 5.7 Notch Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Center frequency in Hertz. | Yes |
| **qualityFactor** | `number` | Bandwidth control. Default: `1.0`. | No |

### 5.8 Allpass Filter

| Property | Type | Description | Required |
|---|---|---|---|
| **frequency** | `number` | Center frequency of the phase transition in Hertz. | Yes |
| **qualityFactor** | `number` | Sharpness of the phase transition. Default: `1.0`. | No |

## 6. Channel Routing Nodes

### 6.1 Channel Splitter

Splits a multichannel stream into separate mono outputs.

| Property | Type | Description | Required |
|---|---|---|---|
| **channelInterpretation** | `string` | `speakers` or `discrete`. Default: `discrete`. | No |

### 6.2 Channel Merger

Merges multiple mono inputs into one multichannel output.

| Property | Type | Description | Required |
|---|---|---|---|
| **channelInterpretation** | `string` | `speakers` or `discrete`. Default: `discrete`. | No |

### 6.3 Channel Mixer

Up-mixes or down-mixes channel count.

| Property | Type | Description | Required |
|---|---|---|---|
| **outputChannels** | `integer` | Desired output channel count. | Yes |
| **channelInterpretation** | `string` | `speakers` or `discrete`. Default: `speakers`. | No |

### 6.4 Audio Mixer

Sums multiple inputs with the same channel count into one output.

| Property | Type | Description | Required |
|---|---|---|---|
| **channelInterpretation** | `string` | `speakers` or `discrete`. Default: `speakers`. | No |

## 7. Encoding Properties Extension

When `KHR_audio_graph` is present, entries in `KHR_audio_emitter.audio[]` MAY include encoding metadata through an extension property. This metadata is useful when graph processing depends on channel count, sample rate, or known duration.

```json
{
    "extensions": {
        "KHR_audio_emitter": {
            "audio": [
                {
                    "uri": "music.mp3",
                    "extensions": {
                        "KHR_audio_graph": {
                            "encoding": {
                                "sampleRate": 44100,
                                "channels": 2,
                                "bitsPerSample": 16,
                                "duration": 180.5,
                                "samples": 7960050
                            }
                        }
                    }
                }
            ]
        }
    }
}
```

### Encoding Properties

| Property | Type | Description | Required |
|---|---|---|---|
| **sampleRate** | `number` | Sampling rate in Hertz. | Yes |
| **channels** | `integer` | Number of channels. | Yes |
| **bitsPerSample** | `integer` | Bits per sample. | No |
| **duration** | `number` | Total asset duration in seconds. | No |
| **samples** | `integer` | Total sample count. | No |

## 8. Source Property Extensions

When `KHR_audio_graph` is present, entries in `KHR_audio_emitter.sources[]` MAY include additional playback properties through an extension property.

```json
{
    "extensions": {
        "KHR_audio_emitter": {
            "sources": [
                {
                    "audio": 0,
                    "gain": 1.0,
                    "loop": true,
                    "autoPlay": true,
                    "extensions": {
                        "KHR_audio_graph": {
                            "loopStart": 0.5,
                            "loopEnd": 120.0,
                            "offset": 0.0,
                            "when": 0.0,
                            "duration": 120.0,
                            "playbackRate": 1.0,
                            "priority": 128,
                            "state": "playing",
                            "channelInterpretation": "speakers"
                        }
                    }
                }
            ]
        }
    }
}
```

### Extended Source Properties

| Property | Type | Description | Required |
|---|---|---|---|
| **loopStart** | `number` | Loop start position in seconds. | No |
| **loopEnd** | `number` | Loop end position in seconds. | No |
| **offset** | `number` | Playback start offset in seconds. | No |
| **when** | `number` | Scheduled start time in seconds. | No |
| **duration** | `number` | Requested playback duration in seconds. | No |
| **playbackRate** | `number` | Playback rate multiplier. Default: `1.0`. | No |
| **priority** | `integer` | Playback priority, `0` = highest and `256` = lowest. Default: `128`. | No |
| **state** | `string` | Playback state: `playing`, `paused`, `stopped`. | No |
| **channelInterpretation** | `string` | `speakers` or `discrete`. | No |

## 9. Graph Rules

The following rules are normative:

1. Graphs are directed acyclic graphs. Cycles are not permitted.
2. Each graph input binds one `KHR_audio_emitter` source to one graph node input port.
3. Each graph output binds one graph node output port to one `KHR_audio_emitter` emitter.
4. A `KHR_audio_emitter` source may be bound to multiple graph inputs.
5. A `KHR_audio_emitter` emitter may be targeted by multiple graph outputs. Signals are summed.
6. Multiple graphs may reference the same sources and emitters.
7. Fan-out is allowed: one node output may feed multiple downstream inputs.
8. Fan-in is allowed: one node input may receive multiple upstream outputs. Signals are summed.
9. Splitter output count equals the channel count of its input.
10. Channel merger input count equals the channel count of its output.
11. Audio mixer inputs must have the same channel count.
12. If an emitter is bound through `outputs[]`, that emitter's base `sources` array is ignored.
13. If `outputs[]` is omitted, the implementation routes terminal graph outputs to the global audio destination.

## 10. Bypass Behavior

Processing and filter nodes support the optional `bypass` property.

- `false` or omitted: the node processes audio normally.
- `true`: the node is skipped and audio passes through unchanged.

Implementations may realize bypass either by rewiring the graph or by providing runtime passthrough behavior.

## 11. Web Audio API Mapping

The following mappings are informative.

| Node Kind | Web Audio API | Notes |
|---|---|---|
| `oscillator` | `OscillatorNode` | `pulseWidth` for square waves may be approximated with `PeriodicWave`. |
| `gain` | `GainNode` | `interpolation = linear` maps to `linearRampToValueAtTime`. |
| `delay` | `DelayNode` | |
| `waveshaper` | `WaveShaperNode` | |
| `lowpass` | `BiquadFilterNode` | `type = "lowpass"` |
| `highpass` | `BiquadFilterNode` | `type = "highpass"` |
| `bandpass` | `BiquadFilterNode` | `type = "bandpass"` |
| `lowshelf` | `BiquadFilterNode` | `type = "lowshelf"` |
| `highshelf` | `BiquadFilterNode` | `type = "highshelf"` |
| `peaking` | `BiquadFilterNode` | `type = "peaking"` |
| `notch` | `BiquadFilterNode` | `type = "notch"` |
| `allpass` | `BiquadFilterNode` | `type = "allpass"` |
| `splitter` | `ChannelSplitterNode` | |
| `channelmerger` | `ChannelMergerNode` | |
| `channelmixer` | `GainNode` with explicit channel configuration | |
| `audiomixer` | Connection summing into a shared input node | |

When multiple graph outputs target the same emitter, implementations may realize the emitter as a shared input bus and rely on normal summing behavior before applying emitter gain and spatialization.

## 12. glTF Object Model

The following JSON pointers are defined for mutable properties and may be used with `KHR_animation_pointer` and `KHR_interactivity`.

### Graph Node Parameters

| JSON Pointer | Object Model Type | Description |
|---|---|---|
| `/extensions/KHR_audio_graph/graphs/{}/nodes/{}/params/gain` | `float` | Gain node gain |
| `/extensions/KHR_audio_graph/graphs/{}/nodes/{}/params/delayTime` | `float` | Delay time in seconds |
| `/extensions/KHR_audio_graph/graphs/{}/nodes/{}/params/frequency` | `float` | Filter or oscillator frequency |
| `/extensions/KHR_audio_graph/graphs/{}/nodes/{}/params/qualityFactor` | `float` | Filter Q factor |
| `/extensions/KHR_audio_graph/graphs/{}/nodes/{}/params/detune` | `float` | Oscillator detune |
| `/extensions/KHR_audio_graph/graphs/{}/nodes/{}/bypass` | `bool` | Node bypass state |

### Extended Source Properties

| JSON Pointer | Object Model Type |
|---|---|
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/loopStart` | `float` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/loopEnd` | `float` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/offset` | `float` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/when` | `float` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/duration` | `float` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/playbackRate` | `float` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/priority` | `int` |
| `/extensions/KHR_audio_emitter/sources/{}/extensions/KHR_audio_graph/state` | `string` |

### Read-Only Properties

| JSON Pointer | Object Model Type |
|---|---|
| `/extensions/KHR_audio_graph/graphs.length` | `int` |
| `/extensions/KHR_audio_graph/graphs/{}/nodes.length` | `int` |
| `/extensions/KHR_audio_graph/graphs/{}/connections.length` | `int` |

## 13. JSON Schema Reference

The following schema files are defined in the `schema/` directory:

- `glTF.KHR_audio_graph.schema.json`
- `KHR_audio_graph.graph.schema.json`
- `KHR_audio_graph.node.schema.json`
- `KHR_audio_graph.connection.schema.json`
- `KHR_audio_graph.input.schema.json`
- `KHR_audio_graph.output.schema.json`
- `audio.KHR_audio_graph.schema.json`
- `source.KHR_audio_graph.schema.json`
- `KHR_audio_graph.encoding.schema.json`
- `KHR_audio_graph.oscillator.schema.json`
- `KHR_audio_graph.gain.schema.json`
- `KHR_audio_graph.delay.schema.json`
- `KHR_audio_graph.waveshaper.schema.json`
- `KHR_audio_graph.lowpass.schema.json`
- `KHR_audio_graph.highpass.schema.json`
- `KHR_audio_graph.bandpass.schema.json`
- `KHR_audio_graph.lowshelf.schema.json`
- `KHR_audio_graph.highshelf.schema.json`
- `KHR_audio_graph.peaking.schema.json`
- `KHR_audio_graph.notch.schema.json`
- `KHR_audio_graph.allpass.schema.json`
- `KHR_audio_graph.splitter.schema.json`
- `KHR_audio_graph.channelmerger.schema.json`
- `KHR_audio_graph.channelmixer.schema.json`
- `KHR_audio_graph.audiomixer.schema.json`

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
