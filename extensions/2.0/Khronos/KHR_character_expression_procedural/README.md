# KHR_character_expression_procedural

## Contributors

- Ken Jakubzak, Meta
- Hideaki Eguchi / VirtualCast, Inc.
- K. S. Ernest (iFire) Lee, Independent Contributor / https://github.com/fire
- Shinnosuke Iwaki / VirtualCast, Inc.
- 0b5vr / pixiv Inc.
- Leonard Daly, Independent Contributor
- Nick Burkard, Meta
- Sarah Cooney, Microsoft XGTG
- Aaron Franke, Independent Contributor

## Status

### TODO - Iterate on this to address some of the concerns around this current implementation, as well as incorporate it into the refactored extension hierarchy with KHR_character_expression

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.
Requires the extension(s): `KHR_character`,`KHR_character_expression`
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
    "KHR_character_expression": {
      "expressions": [
        {
          "expression": "blink_left",
          "animation": 0,
          "extensions": {
            "KHR_character_expression_procedural": {
              "conflictResolution": "none"
            }
          }
        },
        {
          "expression": "mouth_purse",
          "animation": 1,
          "extensions": {
            "KHR_character_expression_procedural": {
              "conflictResolution": "blend"
            }
          }
        }
      ]
    }
  }
}
```

### Properties

| Property             | Type   | Description                                                                                                                                                  |
| -------------------- | ------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `conflictResolution` | string | Conflict Resolution route used to determine the resolution strategy when multiple sources (procedural and non-procedural) want to control a given expression |

## Expression Control Behavior

To help define how procedural expressions interact with animation systems, this extension introduces an additional field called `conflictResolution`, inspired by the [VRM 1.0 specification](https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/expressions.md#lip-sync-procedural).

Each procedural expression may declare a resolution strategy for combining procedural output with baked animation data.

### `conflictResolution` Enum

| Value   | Meaning                                                                                  |
| ------- | ---------------------------------------------------------------------------------------- |
| `none`  | No procedural influence. The expression is controlled exclusively by animation or input. |
| `block` | Procedural output _overrides_ animation data while active.                               |
| `blend` | Procedural and animation data may be blended or accumulated together.                    |


This allows developers to distinguish expressions meant to supplement animation (e.g., micro-blinks over emotive acting) versus those that must fully override input (e.g., forced gaze).

## Implementation Notes

- This extension is schema-only and does not contain behavior logic or animation data.
- Runtimes may use this to suppress baked animation for procedural targets.

## License

This extension is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
