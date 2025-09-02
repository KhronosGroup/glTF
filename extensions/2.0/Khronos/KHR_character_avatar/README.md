# KHR_character_avatar

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

Requires the extension(s): `KHR_xmp_json_ld`, `KHR_character`

## Overview

The `KHR_character_avatar` extension designates a glTF asset as representing an avatar. This top-level marker enables tools and runtimes to interpret the asset as containing avatar-specific content such as rigging, blendshapes, animation retargeting, or metadata.

This extension does not define avatar features directly but acts as a root declaration that avatar-related extensions may be present, and that consumers should treat the asset using avatar-specific logic and pipelines. It's part of the wider set of KHR character and avatar extensions that are meant to be building blocks to represent a contract stating functionality and data requirements between a given model and an endpoint.

This extension is dependent on the KHR_character extension; which defines related functionality pertaining to more generalized characters.

## Extension Schema

```json
{
  "extensions": {
    "KHR_character_avatar": {
      "sceneIndex": 0
    }
  }
}
```

### Properties

| Property     | Type    | Description                                                                                                        |
| ------------ | ------- | ------------------------------------------------------------------------------------------------------------------ |
| `sceneIndex` | integer | Index of the glTF `scene` representing the avatar. Used to distinguish the avatar root when multiple scenes exist. |

## Implementation Notes

- `sceneIndex` is required, representing the index of the glTF `scene` corresponding to the avatar. Used to distinguish the avatar root when multiple scenes exist.
- Consumers should use this marker as a signal to search for additional avatar-related extensions, including skeletal, expression, and other khronos avatar extensions.
- Support for `KHR_xmp_json_ld` is encouraged to ensure interoperable metadata across tools and runtimes.

## Known Implementations

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
