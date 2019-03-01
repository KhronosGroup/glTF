# KHR_xmp

## Contributors

TODO


## Status

Draft


## Dependencies

Written against the glTF 2.0 spec.


## Overview
This extension adds support for [XMP (Extensible Metadata Platform)](https://github.com/adobe/xmp-docs) metadata to glTF.
XMP is a technology for embedding metadata into documents and [is an ISO standard since 2012](https://www.iso.org/news/2012/03/Ref1525.html).
XMP applies to several glTF concepts: `asset`, `scene`, `mesh`, `material`, `image`, `texture`.
XMP metadata is organized in namespaces.
We refer to the [XMP namespaces documentation](https://github.com/adobe/xmp-docs/tree/master/XMPNamespaces) for a detailed description of XMP properties.
We define below the XMP - `glTF` namespace.


## XMP data types
This section describes how [XMP data types](https://github.com/adobe/xmp-docs/tree/master/XMPNamespaces/XMPDataTypes) shall be encoded in a glTF JSON.

### XMP Core Properties
The following table describes how [XMP Core Properties](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/CoreProperties.md) are encoded in the glTF manifest.

| XMP Property Type | JSON Type |
| ----------------- | --------- |
| Boolean           | Boolean   |
| Date              | String formatted as described in the XMP documentation |
| Integer           | Number    |
| Real              | Number    |
| Agent Name        | String formatted as described in the XMP documentation |
| Choice            | String    |
| Language Alternative | A language alternative is a dictionary mapping a language (RFC 3066 language code) to text. See property `dc:title` in the example below|
| GUID              | String formatted as described in the XMP documentation |
| Locale            | String formatted as described in the XMP documentation |
| MIMEType          | String formatted as described in the XMP documentation |
| ProperName        | String formatted as described in the XMP documentation |
| RenditionClass    | String formatted as described in the XMP documentation |
| URI               | String formatted as described in the XMP documentation |
| ResourceRef       | Object, we refer to the XMP documentation for a description of its properties |
| URL               | String formatted as described in the XMP documentation |
| Rational          | String formatted as described in the XMP documentation |
| FrameRate         | String formatted as described in the XMP documentation |
| FrameCount        | String formatted as described in the XMP documentation |
| Part              | String formatted as described in the XMP documentation |

### Additional Properties

| XMP Property Type | JSON Type |
| ----------------- | --------- |
| Array             | Array     |
| [ResourceEvent](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/XMPDataTypes/ResourceEvent.md)| Object, we refer to the XMP documentation for a description of its properties |


## The XMP gltf Namespace
The `gltf` namespace is used to extend `XMP` to include metadata of interest to the glTF working group.
The table below describes the properties in the `gltf` namespace.

| Property Name  | Desrcription | JSON Type    |
| -------------- | ------------ | ------------ |
| spdx           | A [SPDX](https://spdx.org/licenses/) license identifier   | string |


## Defining XMP Metadata
An indirection level is introduced to avoid replicating the same XMP metadata in multiple glTF nodes.
XMP metadatas are defined within a dictionary property in the glTF scene description file, by adding an extensions property to the top-level glTF 2.0 object and defining a KHR_xmp property with a `xmps` array inside it.
The following example defines a glTF scene with a sample XMP metadata.

```
"extensions": {
    "KHR_xmp" : {
        "xmps" : [
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
                "dc:title" : { "en-us" : "MyModel", "it-IT" : "Mio Modello"},
                "dc:type" : ["Physical Object"],
                "gltf:spdx" : "BSD-Protection",
            }
        ]
    }
}
```


## Instantiating XMP metadata
Metadata can be instantiate in a node by defining the extensions.KHR_xmp property and, within that, an index into the `xmps` array using the `xmp` property.
The following example shows a glTF Mesh instantiating the XMP metadata at index `0`.

```
{
    "meshes": [
      {
        extensions : {
            "KHR_xmp": {
                "xmp" : 0,
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

####Overriding Rule
A glTF scene might reference assets already containing XMP metadata.
A relevant example would be an image node referencing a `png` file with embedded XMP metadata.
When this happens the XMP metadata specified in the glTF manifest takes precedence over the metadata embedded in the referenced resource.
