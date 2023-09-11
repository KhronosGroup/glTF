<!--
Copyright 2018-2020 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_xmp

## Contributors

* Emiliano Gambaretto, Adobe, [@emilian0](https://github.com/emilian0)
* Mike Bond, Adobe, [@MiiBond](https://twitter.com/MiiBond)
* Leonard Rosenthol, Adobe, [@lrosenthol](https://github.com/lrosenthol)
* Ed Mackey, Analytical Graphics, Inc.
* Don McCurdy, Google, [@donrmccurdy](https://twitter.com/donrmccurdy)
* Gary Hsu, Microsoft, [@bghgary](https://twitter.com/bghgary)
* Robert Long, Mozilla, [@arobertlong](https://twitter.com/arobertlong)
* Adam Morris, Target, [@weegeekps](https://github.com/weegeekps)
* Andreas Plesch, Individual Contributor,[@andreasplesch](https://github.com/andreasplesch)
* Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)

Copyright 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Archived, Ratified by the Khronos Group

Superseded by [KHR_xmp_json_ld](../../Khronos/KHR_xmp_json_ld/README.md)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds support for [XMP (Extensible Metadata Platform) (ISO 16684-1)](https://www.iso.org/standard/75163.html) metadata to glTF.
Metadata is used to transmit information (such as attribution, licensing, creation date) about the glTF asset. Metadata has no normative effect on the glTF asset appearance and rendering.
XMP is a technology for embedding metadata into documents and has been [an ISO standard since 2012](https://www.iso.org/files/live/sites/isoorg/files/news/magazine/ISO%20Focus+%20(2010-2013)/en/2012/ISO%20Focus+,%20May%202012.pdf).
An instance of the XMP data model is called an XMP packet (ISO 16684-1$6.1).
XMP metadata is embedded in a top-level glTF extension as an array of metadata packets.
XMP metadata packets can then be referenced from glTF objects of type: `asset`, `scene`, `node`, `mesh`, `material`, `image`, `animation`.
XMP metadata referenced by the glTF top level object `asset` applies to the entire glTF asset.
XMP metadata is organized in namespaces (ISO 16684-1$6.2). This extension enables any XMP metadata namespace to be embedded in a glTF asset.

## XMP data types

This section describes how [XMP data types (ISO 16684-1$6.3)](https://github.com/adobe/xmp-docs/tree/master/XMPNamespaces/XMPDataTypes) shall be encoded in a glTF JSON.
XMP supports three classes of data types: simple, structure and array. XMP structures are encoded via JSON objects. XMP arrays are encoded via JSON arrays.
XMP simple values are generally encoded via JSON strings with the exception of `Boolean`, `Integer` and `Real` that are encoded as described below.

### XMP Core Properties

The following table describes how [XMP Core Properties (ISO 16684-1$8.2)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md) are encoded in the glTF manifest.

| XMP Property Type | JSON Type |
| ----------------- | --------- |
| [Boolean (ISO 16684-1$8.2.1.1)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#boolean)              | Boolean   |
| [Date (ISO 16684-1$8.2.1.2)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#date)                    | String formatted as described in the XMP documentation |
| [Integer (ISO 16684-1$8.2.1.3)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#integer)              | Number    |
| [Real (ISO 16684-1$8.2.1.4)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#real)                    | Number    |
| [Text (ISO 16684-1$8.2.1.5](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#text)                    | String |
| [Agent Name (ISO 16684-1$8.2.2.1)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#agent-name)        | String formatted as described in the XMP documentation |
| [Choice (ISO 16684-1$8.2.2.2)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#choice)                | A value chosen from a vocabulary of values of a specific type. |
| [GUID (ISO 16684-1$8.2.2.3)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#guid)                    | String formatted as described in the XMP documentation |
| [Language Alternative (ISO 16684-1$8.2.2.4)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#language-alternative) | A language alternative is a dictionary that maps a language code, specified via a lower-case [BCP-47](https://tools.ietf.org/html/bcp47) string, to text. See property `dc:title` in the example below|
| [Locale (ISO 16684-1$8.2.2.5)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#locale)                | String formatted as described in the XMP documentation |
| [MIMEType (ISO 16684-1$8.2.2.6)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#mimetype)            | String formatted as described in the XMP documentation |
| [Rational (ISO 16684-1$8.2.2.7)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#rational)            | String formatted as described in the XMP documentation |
| [RenditionClass (ISO 16684-1$8.2.2.8)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#renditionclass)    | String formatted as described in the XMP documentation |
| [ResourceRef  (ISO 16684-1$8.2.2.9)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#resourceref)      | Object, we refer to the XMP documentation for a description of its properties |
| [URI (ISO 16684-1$8.2.2.10)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#uri)                      | String formatted as described in the XMP documentation |
| [URL (ISO 16684-1$8.2.2.11)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#url)                      | String formatted as described in the XMP documentation |

## Defining XMP Metadata

An indirection level is introduced to avoid replicating the same XMP metadata in multiple glTF objects.
XMP metadatas are defined within a dictionary property in the glTF scene description file by adding an `extensions` property to the top-level glTF 2.0 object and defining a `KHR_xmp` object. The `KHR_xmp` object defines two properties:

* `@context` : a dictionary mapping XMP namespaces to the URI where they are defined.
Please note that the context definition conforms with [JSON-LD](https://www.w3.org/2018/jsonld-cg-reports/json-ld/#the-context).
* `packets`: an array of metadata packets referencing the namespaces defined in the `@context` dictionary.

The following example defines a glTF scene with a sample XMP metadata.

```json
"extensions": {
    "KHR_xmp" : {
        "@context" : {
            "dc" : "http://purl.org/dc/elements/1.1/"
        },
        "packets" : [
            {
                "dc:contributor" : [ "Creator1Name", "Creator2Email@email.com", "Creator3Name<Email@email.com>"],
                "dc:coverage" : "Bay Area, California, United States",
                "dc:creator" : [ "CreatorName", "CreatorEmail@email.com"],
                "dc:date" : ["1997-07-16T19:20:30+01:00"],
                "dc:description" : { "en-us": "text"},
                "dc:format" : "model/gltf-binary",
                "dc:identifier" : "urn:stock-id:292930",
                "dc:language" : ["en"],
                "dc:publisher" : ["Company"],
                "dc:relation" : ["https://www.khronos.org/"],
                "dc:rights" : {"en-us": "BSD"},
                "dc:source" : "http://related_resource.org/derived_from_this.gltf",
                "dc:subject" : ["architecture"],
                "dc:title" : { "en-us" : "MyModel", "it-it" : "Mio Modello"},
                "dc:type" : ["Physical Object"],
            }
        ]
    }
}
```

## Instantiating XMP metadata

Metadata can be instantiated in a gltf object by defining the `extensions.KHR_xmp` property and, within that, an index into the `packets` array using the `packet` property.
The following example shows a glTF Mesh instantiating the XMP metadata at index `0`.

```json
{
    "meshes": [
      {
        "extensions" : {
            "KHR_xmp": {
                "packet" : 0,
            }
        },
        "primitives": [
          {
            "attributes": {
                "NORMAL": 23,
                "POSITION": 22,
                "TANGENT": 24,
                "TEXCOORD_0": 25
            },
            "indices": 21,
            "material": 3,
            "mode": 4
          }
        ]
      }
    ]
}
```

### Notes:

#### Precedence

Metadata applied to JSON objects in the top level arrays ( `scenes`, `nodes`, `meshes`, `materials`, `images`, `animations`) has precedence over the metadata specified in the `asset` property of a glTF.
A glTF might reference resources already containing XMP metadata. A relevant example would be an image object referencing a PNG/JPG file with embedded XMP metadata. Note that glTF clients and viewers may ignore the metadata embedded in the referenced resources (for instance PNG/JPG images).

#### Transferring and merging metadata

When a glTF object (for example a Mesh) is copied from a glTF file to another, the associated `KHR_xmp` metadata should be copied as well.
Copying metadata requires adding the source object's metadata packets to the destination glTF's `KHR_xmp.packets` array.
In the following example two glTFs containing a mesh with `KHR_xmp` metadata are copied into a third glTF.

glTF containing the first mesh:

```json
{
    "meshes": [
      {
        "extensions" : { "KHR_xmp": { "packet" : 0 } },
        ...
      }
    ],
    "extensions": {
        "KHR_xmp" : {
            "@context" : { "dc" : "http://purl.org/dc/elements/1.1/" },
            "packets" : [{
                "dc:title" : { "en-us" : "My first mesh"},
                ...
            }]
        }
    }
}
```

glTF containing the second mesh:

```json
{
    "meshes": [
      {
        "extensions" : { "KHR_xmp": { "packet" : 0 } },
        ...
      }
    ],
    "extensions": {
        "KHR_xmp" : {
            "@context" : { "dc" : "http://purl.org/dc/elements/1.1/" },
            "packets" : [{
                "dc:title" : { "en-us" : "My second mesh"},
                ...
            }]
        }
    }
}
```

glTF containing copies of both meshes:

```json
{
    "meshes": [
      {
        "extensions" : { "KHR_xmp": { "packet" : 0 } },
        ...
      },
      {
        "extensions" : { "KHR_xmp": { "packet" : 1 } },
        ...
      }
    ],
    "extensions": {
        "KHR_xmp" : {
            "@context" : { "dc" : "http://purl.org/dc/elements/1.1/" },
            "packets" : [
              {
                "dc:title" : { "en-us" : "My first mesh"},
                ...
              },
              {
                "dc:title" : { "en-us" : "My second mesh"},
                ...
              }
            ]
        }
    }
}
```

The [xmpMM (ISO 16684-1$8.6)](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/xmpMM.md) namespaces introduces mechanisms such as Pantry-Ingredient to handle the case when a glTF file is generated via derivation or composition of one or more source documents.
The following example illustrates how `KHR_xmp` metadata can be computed in the case of two glTFs, both containing `asset` metadata, that are processed together to obtain a new composited glTF document.

First glTF document:

```json
{
    "asset": {
        "extensions" : {
            "KHR_xmp": {
                "packet" : 0,
            }
        },
        ...
    },
    "extensions": {
        "KHR_xmp" : {
            "@context" : { "dc" : "http://purl.org/dc/elements/1.1/" },
            "packets" : [
              {
                "dc:title" : { "en-us" : "My first glTF"},
                ...
              }
            ]
        }
    }
}
```

Second glTF document:

```json
{
    "asset": {
        "extensions" : {
            "KHR_xmp": {
                "packet" : 0,
            }
        },
        ...
    },
    "extensions": {
        "KHR_xmp" : {
            "@context" : { "dc" : "http://purl.org/dc/elements/1.1/" },
            "packets" : [
              {
                "dc:title" : { "en-us" : "My second glTF"},
                ...
              }
            ]
        }
    }
}
```

Derived glTF document metadata:

```json
{
    "asset": {
        "extensions" : {
            "KHR_xmp": {
                "packet" : 0,
            }
        },
        ...
    },
    "extensions": {
        "KHR_xmp" : {
            "@context" :
              {
                "dc" : "http://purl.org/dc/elements/1.1/",
                "xmpMM" : "http://ns.adobe.com/xap/1.0/mm/",
              },
            "packets" : [
              {
                "dc:title" : { "en-us" : "My composed glTF."},
                "xmpMM:Pantry" : [
                  {
                    "xmpMM:DocumentID" : "62bc2623-968e-4b09-8174-02dc53d6b856",
                    "xmpMM:InstanceID" : "1a33a91f-7351-471e-9b6c-24bca3213d2a",
                    "dc:title" : { "en-us" : "My first glTF"},
                    ...
                  },
                  {
                    "xmpMM:DocumentID" : "6d70a25e-129e-42d8-9d63-93bd9eb0298b",
                    "xmpMM:InstanceID" : "235b5571-e6a9-48fd-8d6b-b88276f37ee3",
                    "dc:title" : { "en-us" : "My second glTF"},
                    ...
                  }
                ],
                "xmpMM:Ingredients" : [
                  {
                    "xmpMM:DocumentID" : "62bc2623-968e-4b09-8174-02dc53d6b856",
                    "xmpMM:InstanceID" : "1a33a91f-7351-471e-9b6c-24bca3213d2a"
                  },
                  {
                    "xmpMM:DocumentID" : "6d70a25e-129e-42d8-9d63-93bd9eb0298b",
                    "xmpMM:InstanceID" : "235b5571-e6a9-48fd-8d6b-b88276f37ee3"
                  }
                ]
              }
            ]
        }
    }
}
```

### Implementation notes

*This section is non-normative.*

glTF viewers are free to chose which metadata in the glTF to display.
Our recommendation for viewers that choose to display XMP metadata is to include the metadata in the glTF `asset` object.

XMP is an extensible metadata format by design. In order to achieve better readability and interoperability across the industry, we recommend glTF creators to use the following preferred list of namespace prefixes and URIs.

| Namespace prefix  | Namespace URI                         |
|:----------------- |:------------------------------------- |
| `dc`              | `http://purl.org/dc/elements/1.1/`    |
| `xmp`             | `http://ns.adobe.com/xap/1.0/`        |
| `xmpMM`           | `http://ns.adobe.com/xap/1.0/mm/`     |
| `xmpRights`       | `http://ns.adobe.com/xap/1.0/rights/` |

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2020 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

In particular, the referenced paragraphs of ISO 16684-1:2019 are normative and so are
INCLUDED in the Scope of this Specification and may contain contributions from non-members
of Khronos that are not covered by the Khronos Intellectual Property Rights Policy.
The links to https://github.com/adobe/xmp-docs are purely informative and so are EXCLUDED
from the Scope of this Specification, but are provided for convenience.
The references to ISO 16684-1:2019 shall remain normative if there are differences to
information provided at any informative links.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/adopters.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

Khronos® and Vulkan® are registered trademarks, and ANARI™, WebGL™, glTF™, NNEF™, OpenVX™,
SPIR™, SPIR-V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc.
OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in
China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc.
and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks
of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of
ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely
for identification and belong to their respective owners.
