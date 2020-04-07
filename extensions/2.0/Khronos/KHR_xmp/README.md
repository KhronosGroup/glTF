# KHR_xmp

## Contributors

* Emiliano Gambaretto, Adobe, <mailto:emiliano@adobe.com>
* Mike Bond, Adobe, [@MiiBond](https://twitter.com/MiiBond)
* Robert Long, Mozilla, [@arobertlong](https://twitter.com/arobertlong)
* Don McCurdy, Google, [@donmccurdy](https://twitter.com/donrmccurdy)
* Andreas Plesch, Individual Contributor,[@andreasplesch](https://github.com/andreasplesch)
* Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)
* Ed Mackey, Analytical Graphics, Inc.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds support for [XMP (Extensible Metadata Platform)](https://github.com/adobe/xmp-docs) metadata to glTF.
XMP is a technology for embedding metadata into documents and has been [an ISO standard since 2012](https://www.iso.org/news/2012/03/Ref1525.html).
XMP metadata is embedded in a top-level glTF extension as an array of metadata packets.
Any JSON object in the glTF can reference metadata packets with the exception of the root object.
XMP metadata referenced by the glTF top level property `asset` applies to the entire glTF file.
XMP metadata is organized in namespaces. This extension enables any XMP metadata namespace to be embedded in a glTF asset.
We refer to the [XMP namespaces documentation](https://github.com/adobe/xmp-docs/tree/master/XMPNamespaces) for a detailed description of XMP properties.

## XMP data types

This section describes how [XMP data types](https://github.com/adobe/xmp-docs/tree/master/XMPNamespaces/XMPDataTypes) shall be encoded in a glTF JSON.
XMP supports three classes of data types: simple, structure and array. XMP structures are encoded via JSON objects. XMP arrays are encoded via JSON arrays.
XMP simple values are generally encoded via JSON strings with the exception of `Boolean`, `Integer` and `Real` that are encoded as described below.

### XMP Core Properties

The following table describes how [XMP Core Properties](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md) are encoded in the glTF manifest.

| XMP Property Type | JSON Type |
| ----------------- | --------- |
| [Boolean](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#boolean)              | Boolean   |
| [Date](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#date)                    | String formatted as described in the XMP documentation |
| [Integer](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#integer)              | Number    |
| [Real](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#real)                    | Number    |
| [Text](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#text)                    | String |
| [Agent Name](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#agent-name)        | String formatted as described in the XMP documentation |
| [Choice](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#choice)                | String    |
| [Language Alternative](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#language-alternative) | A language alternative is a dictionary mapping a language ([BCP-47](https://tools.ietf.org/html/bcp47) language code) to text. See property `dc:title` in the example below|
| [GUID](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#guid)                    | String formatted as described in the XMP documentation |
| [Locale](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#locale)                | String formatted as described in the XMP documentation |
| [MIMEType](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#mimetype)            | String formatted as described in the XMP documentation |
| [ProperName](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#propername)        | String formatted as described in the XMP documentation |
| [RenditionClass](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#renditionclass)    | String formatted as described in the XMP documentation |
| [URI](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#uri)                      | String formatted as described in the XMP documentation |
| [ResourceRef](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#resourceref)      | Object, we refer to the XMP documentation for a description of its properties |
| [URL](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#url)                      | String formatted as described in the XMP documentation |
| [Rational](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#rational)            | String formatted as described in the XMP documentation |
| [FrameRate](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#framerate)          | String formatted as described in the XMP documentation |
| [FrameCount](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#framecount)        | String formatted as described in the XMP documentation |
| [Part](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md#part)                    | String formatted as described in the XMP documentation |

### Additional Properties

| XMP Property Type | JSON Type |
| ----------------- | --------- |
| [ResourceEvent](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/ResourceEvent.md)| Object, we refer to the XMP documentation for a description of its properties |

## Defining XMP Metadata

An indirection level is introduced to avoid replicating the same XMP metadata in multiple glTF objects.
XMP metadatas are defined within a dictionary property in the glTF scene description file by adding an `extensions` property to the top-level glTF 2.0 object and defining a `KHR_xmp` object. The `KHR_xmp` object defines two properties:

* `@context` : a dictionary mapping XMP [namespaces](https://github.com/adobe/xmp-docs/blob/master/Namespaces.md) to the URI where they are defined.
* `packets`: an array of metadata packets referencing the namespaces defined in the `namespaces` dictionary.

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

#### Overriding Rule

A glTF scene might reference resources already containing XMP metadata.
A relevant example would be an image object referencing a PNG file with embedded XMP metadata.
When this happens the XMP metadata specified in the glTF manifest takes precedence over the metadata embedded in the referenced resource.

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

The [xmpMM](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/xmpMM.md) namespaces introduces mechanisms such as Pantry-Ingredient to handle the case when a glTF file is generated via derivation or composition of one or more source documents.
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
