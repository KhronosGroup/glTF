# KHR_character

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

Requires the extensions: `KHR_xmp_json_ld`

This extension also leverages the `KHR_xmp_json_ld` pattern for attaching extensible metadata as JSON-LD blocks within glTF assets. For background on this approach, see:  
[KHR_xmp_json_ld](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_xmp_json_ld)

## Overview

The `KHR_character` extension designates a glTF asset as representing an character. This top-level marker enables tools and runtimes to interpret the asset as containing character-specific content such as rigging, blendshapes, animation retargeting, or metadata.

This extension does not define character features directly but acts as a root declaration that character-related extensions may be present, and that consumers should treat the asset using character-specific logic and pipelines. It's part of the wider set of KHR character extensions that are meant to be building blocks to represent a contract stating functionality and data requirements between a given model and an endpoint.

The extension supports referencing the source `scene` that represents the character and optionally includes structured metadata through the `KHR_xmp_json_ld` mechanism.

## Extension Schema

```json
{
  "extensions": {
    "KHR_character": {
      "sceneIndex": 0
    }
  }
}
```

### Properties

| Property     | Type    | Description                                                                                                              |
| ------------ | ------- | ------------------------------------------------------------------------------------------------------------------------ |
| `sceneIndex` | integer | Index of the glTF `scene` representing the character. Used to distinguish the character root when multiple scenes exist. |

## Metadata Attachment: KHR_xmp_json_ld

character metadata should be expressed using the `KHR_xmp_json_ld` format, a structured mechanism for attaching JSON-LD metadata blocks to glTF files. In the context of `KHR_character`, this allows consistent expression of character provenance, licensing, creator, versioning, and intended use, among others.

The `KHR_xmp_json_ld` block is placed at the root level of the glTF asset as part of the defined extension usage. Metadata keys and structures are defined in the shared Khronos character Metadata schema (TBD).

| DC/XMP_JSON_LD Property | Why                                                                                                                                       | Required |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- | -------- |
| dc:title                |                                                                                                                                           | Yes      |
| dc:creator              |                                                                                                                                           | Yes      |
| dc:license              |                                                                                                                                           | No       |
| dc:rights               |                                                                                                                                           | No       |
| dc:created              | Date on which the asset was created                                                                                                       | No       |
| dc:publisher            | Identifies the entity responsible for making the resource available; important for understanding the source and authority of the content. | No       |
| dc:description          | Context and a summary of the content                                                                                                      | No       |
| dc:subject              | Can potentially be used for content tagging/association                                                                                   | No       |
| dc:source               | Important for tracing the provenance and ensuring proper attribution.                                                                     | Yes      |
| khr:version             |                                                                                                                                           | No       |
| khr:thumbnailImage      |                                                                                                                                           | No       |

## Example

```json
{
  "asset": {
    "version": "2.0",
    "extensions": {
      "KHR_xmp_json_ld": {
        "packet": 0
      }
    }
  },
  "scene": 0,
  "scenes": [
    {
      "nodes": [0]
    }
  ],
  "nodes": [
    {
      "name": "characterRoot"
    }
  ],
  "extensionsUsed": ["KHR_character", "KHR_xmp_json_ld"],
  "extensions": {
    "KHR_character": {
      "sceneIndex": 0
    },

    "KHR_xmp_json_ld": {
      "packets": [
        {
          "@context": {
            "dc": "http://purl.org/dc/elements/1.1/",
            "vrm": "https://github.com/vrm-c/vrm-specification/blob/master/specification/VRMC_vrm-1.0/meta.md"
          },
          "dc:title": "Example Model",
          "dc:creator": {
            "@list": [
              "Author1",
              "AuthorEmail1@email.com",
              "Author2",
              "AuthorEmail2@email.com"
            ]
          },
          "dc:license": {
            "@list": [
              "https://vrm.dev/licenses/1.0/",
              "https://example.com/third-party-license"
            ]
          },
          "dc:created": "2023-05-05",
          "dc:rights": "Copyright information about the model",
          "dc:publisher": "Imaginary Corporation A, LLC",
          "dc:description": "A sentence, or paragraph describing the character at hand",
          "dc:subject": {
            "@list": ["Example trait", "Another example trait"]
          },
          "dc:source": "imaginaryCompany.com/characterl",
          "khr:version": "1.0",
          "khr:thumbnailImage": 0
        }
      ]
    }
  }
}
```

## Implementation Notes

- `sceneIndex` is required, representing the index of the glTF `scene` corresponding to the character. Used to distinguish the character root when multiple scenes exist.
- Consumers should use this marker as a signal to search for additional character-related extensions, including skeletal, expression, and other khronos character extensions.
- Support for `KHR_xmp_json_ld` is encouraged to ensure interoperable metadata across tools and runtimes.

## Known Implementations

## License

This extension specification is licensed under the Khronos Group Extension License.  
See: https://www.khronos.org/registry/gltf/license.html
