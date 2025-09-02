# KHR_character_expression_procedural

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Individual Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Individual Contributor
- Nick Burkard, Meta

## Status

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.  
Dependent on: `KHR_character`
Works alongside: `KHR_character_expression_mapping`, `KHR_character_expression_joint`, `KHR_character_expression_texture`, `KHR_character_expression_morphtargets`.

## Overview

The `KHR_character_expression_procedural` extension introduces a metadata layer that defines which expressions are expected to be procedurally generated or controlled at runtime. This includes expressions such as:

- **Timed blinking**
- **Eye darting**
- **Idle mouth movement**
- **Microexpressions**
- **Randomized twitches or gestures**

Expressions in this context describe face-localized animations used to drive small and/or larger movements across the face and/or down-chain meshes needed for reasonable conveyance of emotion/intent.

By labeling expressions as procedural, this extension enables runtime systems (e.g., animation controllers, live-tracking systems, or AI characters) to drive those expressions without conflict with animation data or user inputs.

This metadata is **descriptive only**: it does not contain animation or behavior logic itself. Instead, it signals to tools and engines that the following expressions are _expected to be dynamically generated_.

## Use Cases

- Define which expressions are not statically animated but generated procedurally
- Avoid conflict between procedural and baked expression systems
- Inform rig exporters or runtime systems which expressions are reserved for real-time generation

## Schema

```json
{
  "extensions": {
    "KHR_character_expression_procedural": {
      "proceduralExpressions": [
        {
          "expression": "blinkLeft",
          "mode": "timed"
        },
        {
          "expression": "mouthTwitch",
          "mode": "random"
        }
      ]
    }
  }
}
```

### Properties

| Property                | Type   | Description                                                                             |
| ----------------------- | ------ | --------------------------------------------------------------------------------------- |
| `proceduralExpressions` | array  | List of expression metadata entries that are intended to be procedurally controlled     |
| `expression`            | string | Corresponding expression name                                                           |
| `mode`                  | string | Enum: `"timed"`, `"random"`, `"live"`, `"scripted"` — describes the procedural strategy |

## Procedural Modes

TODO: Need to discuss the items below; there is concern around these particular fields and assertions that end applications should decide how to control these

| Mode       | Meaning                                                                        |
| ---------- | ------------------------------------------------------------------------------ |
| `timed`    | The expression occurs in regular intervals (e.g., blinking every 5s)           |
| `random`   | The expression fires with stochastic timing or weights                         |
| `live`     | Driven by live-captured inputs (e.g., webcam, eye tracking)                    |
| `scripted` | Driven by external dialogue, AI logic, or gameplay sequences                   |
| `mixed`    | Combines two or more procedural sources, such as live tracking + scripted cues |

## Expression Control Behavior

To help define how procedural expressions interact with animation systems, this extension introduces an additional field called `conflictResolution`, inspired by the [VRM 1.0 specification](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/expressions.md#lip-sync-procedural).

Each procedural expression may declare a resolution strategy for combining procedural output with baked animation data.

### `conflictResolution` Enum

| Value   | Meaning                                                                                  |
| ------- | ---------------------------------------------------------------------------------------- |
| `none`  | No procedural influence. The expression is controlled exclusively by animation or input. |
| `block` | Procedural output _overrides_ animation data while active.                               |
| `blend` | Procedural and animation data may be blended or accumulated together.                    |

### Example with Resolution Mode

```json
{
  "expression": "blinkLeft",
  "mode": "timed",
  "conflictResolution": "blend"
}
```

This allows developers to distinguish expressions meant to supplement animation (e.g., micro-blinks over emotive acting) versus those that must fully override input (e.g., forced gaze).

## Implementation Notes

- This extension is schema-only and does not contain behavior logic or animation data.
- Runtimes may use this to suppress baked animation for procedural targets.
- The expression names should align with the shared vocabulary defined in the `KHR_character_expression_mapping`.

## License

This extension is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
