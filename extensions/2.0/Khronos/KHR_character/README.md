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

## Conventions

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **NOT RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in [BCP 14](https://www.rfc-editor.org/info/bcp14) when, and only when, they appear in all capitals, as shown here. Sections labeled (Informative), their subsections, and all examples are informative; all other sections are normative.

## Dependencies

Written against the glTF 2.0 specification.

This extension has no extension dependencies.

## Overview (Informative)

The `KHR_character` extension designates a glTF asset as containing a character and provides a common discovery point for independently defined character-related extensions.

The extension does not define which nodes, materials, animations, or other objects belong to the character. It does not define rigging, expressions, retargeting, metadata, traversal, rendering, or other runtime behavior. Those capabilities are defined, when present, by their respective extensions.

## Extension usage (Normative)

The `KHR_character` object **MUST** be attached to the top-level glTF object. The asset **MUST** list `KHR_character` in `extensionsUsed` and **MAY** list it in `extensionsRequired` according to the rules of the glTF 2.0 specification.

A consumer claiming support for this extension **MUST** recognize the `KHR_character` object and make the character designation available to host logic. Support does not require the consumer to traverse, render, animate, or otherwise operate on the character. A consumer that cannot provide this minimum behavior **MUST** treat an asset listing `KHR_character` in `extensionsRequired` as unsupported, according to the glTF 2.0 extension rules.

### Multiple characters

The `KHR_character` designation covers at most one character in a glTF asset. It neither prohibits unrelated scene content nor identifies which nodes or other objects belong to that character. Representation of multiple characters within a single asset is outside the scope of this extension.

## Extension structure (Normative)

```json
{
  "asset": {
    "version": "2.0"
  },
  "extensionsUsed": [
    "KHR_character"
  ],
  "extensions": {
    "KHR_character": {}
  }
}
```

### Properties

The `KHR_character` object defines no properties.

## Validation boundary (Normative)

A validator claiming support for this extension **MUST** validate the `KHR_character` object against its schema and **MUST** verify its placement on the top-level glTF object. Ordinary glTF validation governs the `extensionsUsed` and `extensionsRequired` declarations.

## Metadata guidance (Informative)

This section is informative and does not define conformance requirements for `KHR_character`.

Asset metadata can be expressed using the `KHR_xmp_json_ld` format, a structured mechanism for attaching JSON-LD metadata blocks to glTF files. For an asset that uses `KHR_character`, this can describe provenance, licensing, and creators, among others.

When used, the `KHR_xmp_json_ld` block is placed according to the rules defined by that extension. A packet referenced from `asset` describes the whole glTF asset, as `KHR_xmp_json_ld` defines, not only the character. This extension does not define a character-specific metadata vocabulary.

The following properties are examples that authoring pipelines may find useful. Their presence is not required by this extension.

These examples use two Dublin Core vocabularies. The `dc` prefix is bound to the Dublin Core Metadata Element Set, Version 1.1 (`http://purl.org/dc/elements/1.1/`), which defines fifteen elements and no others: `contributor`, `coverage`, `creator`, `date`, `description`, `format`, `identifier`, `language`, `publisher`, `relation`, `rights`, `source`, `subject`, `title` and `type`. `created` and `license` are not among them. Both are DCMI Metadata Terms, so they take the `dcterms` prefix bound to `http://purl.org/dc/terms/`. Writing them under `dc` yields IRIs that DCMI does not define.

The value shape column gives the encoding these examples use for each term. `KHR_xmp_json_ld` defines the list, set and language alternative forms, and states the `rdf` prefix that a language alternative requires.

`KHR_xmp_json_ld` lists the namespace prefixes it recommends for interoperable use. `dcterms` is not among them; the closest XMP-native properties in those namespaces are `xmp:CreateDate` for a creation date and `xmpRights` properties, such as `xmpRights:WebStatement`, for license information. An author who needs a term that no published vocabulary supplies can bind a prefix to a namespace the author controls, and consumers read such terms as author-specific.

| Property             | Value shape                                          | Example use                                                                                        |
| -------------------- | ---------------------------------------------------- | -------------------------------------------------------------------------------------------------- |
| `dc:title`           | XMP Language Alternative (`rdf:Alt`).                | Human-readable title for the asset.                                                                |
| `dc:creator`         | Ordered `@list` of strings, one per creator.         | Person or organization that created the asset.                                                     |
| `dcterms:license`    | Ordered `@list` of URI strings.                      | License governing use of the asset.                                                                |
| `dc:rights`          | XMP Language Alternative (`rdf:Alt`).                | Copyright or other rights information.                                                             |
| `dcterms:created`    | Single date string in the XMP Date format.           | Date on which the asset was created.                                                               |
| `dc:publisher`       | Unordered `@set` of strings.                         | Entity responsible for making the resource available.                                              |
| `dc:description`     | XMP Language Alternative (`rdf:Alt`).                | Context and a summary of the content.                                                              |
| `dc:subject`         | Unordered `@set` of strings.                         | Content tags or subject classifications.                                                           |
| `dc:source`          | Single URI string.                                   | Source information for provenance and attribution.                                                 |

This extension does not define how multiple `dcterms:license` entries combine.

An XMP packet describes, for metadata consumers, the glTF objects that reference it. It does not carry references to glTF objects. `KHR_xmp_json_ld` gives no meaning to a glTF array index written inside a packet, so the validity of such a reference, its update after an asset edit, and the image formats it can name are all undefined, and no validator checks them. A reference to a glTF object, such as an image used as a character thumbnail, belongs in a typed extension property. This extension does not define one.

### Optional metadata example

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
  "extensionsUsed": ["KHR_character", "KHR_xmp_json_ld"],
  "extensions": {
    "KHR_character": {},

    "KHR_xmp_json_ld": {
      "packets": [
        {
          "@context": {
            "dc": "http://purl.org/dc/elements/1.1/",
            "dcterms": "http://purl.org/dc/terms/",
            "rdf": "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
          },
          "@id": "",
          "dc:title": {
            "@type": "rdf:Alt",
            "rdf:_1": {
              "@language": "x-default",
              "@value": "Example Model"
            }
          },
          "dc:creator": {
            "@list": [
              "Author1 <AuthorEmail1@email.com>",
              "Author2 <AuthorEmail2@email.com>"
            ]
          },
          "dcterms:license": {
            "@list": [
              "https://vrm.dev/licenses/1.0/",
              "https://example.com/third-party-license"
            ]
          },
          "dcterms:created": "2023-05-05",
          "dc:rights": {
            "@type": "rdf:Alt",
            "rdf:_1": {
              "@language": "x-default",
              "@value": "Copyright information about the model"
            }
          },
          "dc:publisher": {
            "@set": ["Imaginary Corporation A, LLC"]
          },
          "dc:description": {
            "@type": "rdf:Alt",
            "rdf:_1": {
              "@language": "x-default",
              "@value": "A sentence or paragraph describing the asset"
            }
          },
          "dc:subject": {
            "@set": ["Example trait", "Another example trait"]
          },
          "dc:source": "https://example.com/characters/example-model"
        }
      ]
    }
  }
}
```

## Implementation notes (Informative)

- Consumers cannot infer character ownership or relationships unless another extension defines them.
- Consumers can use the character designation as a signal to inspect `extensionsUsed` for independently defined character-related extensions.

## Known implementations (Informative)

- [0b5vr/khr-character-testbed](https://github.com/0b5vr/khr-character-testbed) - Three.js viewer and VRM-to-KHR_character converter.
- [Kjakubzak/khr_character_testbed](https://github.com/Kjakubzak/khr_character_testbed) - UnityGLTF importer, exporter, sample assets, and Unity demos.

## Schema

- [glTF.KHR_character.schema.json](./schema/glTF.KHR_character.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this Specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

This Specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at https://www.khronos.org/files/member_agreement.pdf. Parties desiring to implement the Specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos Intellectual Property Rights Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this Specification; see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those specifically identified sections define normative functionality. The Khronos Intellectual Property Rights Policy excludes external references to materials and associated enabling technology not created by Khronos from the Scope of this Specification, and any licenses that may be required to implement such referenced materials and associated technologies must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
