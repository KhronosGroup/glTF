# KHR_character

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

**Draft** – This extension is not yet ratified by the Khronos Group and is subject to change.

## Dependencies

Written against the glTF 2.0 specification.

Requires the extension: `KHR_xmp_json_ld`

Assets using `KHR_character` MUST also list `KHR_xmp_json_ld` in `extensionsUsed`. Character metadata is attached as JSON-LD blocks using the `KHR_xmp_json_ld` pattern. For background on this approach, see:
[KHR_xmp_json_ld](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_xmp_json_ld)

## Overview

The `KHR_character` extension designates a glTF asset as representing a character. This top-level marker enables tools and runtimes to interpret the asset as containing character-specific content such as rigging, blendshapes, animation retargeting, or metadata.

This extension does not define character features directly but acts as a root declaration that character-related extensions may be present, and that consumers should treat the asset using character-specific logic and pipelines. It's part of the wider set of KHR character extensions that are meant to be building blocks to represent a contract stating functionality and data requirements between a given model and an endpoint.

The extension references the root `node` that represents the character. This glTF 2.0 version of the extension identifies one character per asset. Support for multiple independently addressable characters is deferred for consideration alongside glTF 2.1 support.

Structured character metadata is attached through the required `KHR_xmp_json_ld` mechanism. The presence of individual metadata properties remains optional unless another specification requires them.

## Extension Schema

```json
{
  "extensions": {
    "KHR_character": {
      "rootNode": 0
    }
  }
}
```

### Properties

| Property   | Type    | Description                                                                                                                                                                                  |
| ---------- | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `rootNode` | integer | Index of the glTF `node` representing the root of the character hierarchy. This node SHOULD be a common ancestor of all nodes containing character-related meshes and joints. |

## Non-Normative Metadata Guidance

This section is informative and does not define conformance requirements for `KHR_character`.

Character metadata can be expressed using the `KHR_xmp_json_ld` format, a structured mechanism for attaching JSON-LD metadata blocks to glTF files. In the context of `KHR_character`, this can describe character provenance, licensing, creator, versioning, and intended use, among others.

When used, the `KHR_xmp_json_ld` block is placed according to the rules defined by that extension. This extension does not define a character-specific metadata vocabulary.

The following properties are examples that authoring pipelines may find useful. Their presence is not required by this extension. For these examples, the `khr` prefix uses the proposed registry namespace `https://www.khronos.org/registry/glTF/character/metadata/`. This registry namespace is an example that has not yet been implemented or published; it must be established before these terms can be treated as part of a Khronos-managed vocabulary.

| DC/XMP_JSON_LD Property | Example use                                                                                                                               |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| dc:title                | Human-readable title for the character.                                                                                                   |
| dc:creator              | Person or organization that created the character.                                                                                        |
| dc:license              | License governing use of the character.                                                                                                   |
| dc:rights               | Copyright or other rights information.                                                                                                    |
| dc:created              | Date on which the asset was created.                                                                                                      |
| dc:publisher            | Entity responsible for making the resource available.                                                                                     |
| dc:description          | Context and a summary of the content.                                                                                                     |
| dc:subject              | Content tags or subject classifications.                                                                                                  |
| dc:source               | Source information for provenance and attribution.                                                                                        |
| khr:version             | Character or asset version as a string.                                                                                                   |
| khr:thumbnailImage      | Zero-based index of an image in the top-level glTF `images` array to use as a character thumbnail.                                        |

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
  "images": [
    {
      "name": "Character Thumbnail",
      "uri": "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNk+A8AAQUBAScY42YAAAAASUVORK5CYII="
    }
  ],
  "extensionsUsed": ["KHR_character", "KHR_xmp_json_ld"],
  "extensions": {
    "KHR_character": {
      "rootNode": 0
    },

    "KHR_xmp_json_ld": {
      "packets": [
        {
          "@context": {
            "dc": "http://purl.org/dc/elements/1.1/",
            "khr": "https://www.khronos.org/registry/glTF/character/metadata/"
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
          "dc:source": "https://example.com/characters/example-model",
          "khr:version": "1.0",
          "khr:thumbnailImage": 0
        }
      ]
    }
  }
}
```

## Implementation Notes

- `rootNode` is required, representing the index of the glTF `node` that serves as the root of the character hierarchy. This node SHOULD be a common ancestor of all nodes containing character-related meshes and joints.
- Consumers should use this marker as a signal to search for additional character-related extensions, including skeletal, expression, and other khronos character extensions.
- Consumers of `KHR_character` MUST support its `KHR_xmp_json_ld` dependency.

## Known Implementations

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js viewer and VRM-to-KHR_character converter.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## License

This extension specification is licensed under the Khronos Group Extension License.
See: https://www.khronos.org/registry/gltf/license.html
