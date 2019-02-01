# KHR_XMP

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


## XMP Properties
XMP properties are grouped in namespace. 
This extension focuses on the properties defined in the [DCMI (Dublic Core Metadata Initative) namespace](https://github.com/adobe/xmp-docs/blob/master/XMPNamespaces/dc.md).
The properties below have been adapted from XMP (that is currently based on XML) to fit glTF json manifest.

| Property | DCMI definition | Description  | Type |
| --------- | --------------- |-------------|-------|
| `dc:contributor` | An entity responsible for making contributions to the resource. | Can be a Name, an email address, or both in the form `Name<email@email.com>` |  Ordered array of text |
| `dc:coverage`   | The spatial or temporal topic of the resource, the spatial applicability of the resource, or the jurisdiction under which the resource is relevant. | Open ended | Text |
| `dc:creator`   | An entity primarily responsible for making the resource | Can be a Name, an email address, or both in the form `Name<email@email.com>`| Ordered array of text |
| `dc:date` | A point or period of time associated with an event in the life cycle of the resource | date-time specified as ISO 8601 | Text ISO 8601  |
| `dc:description` | An account of the resource | Textual descriptions of the content of the resource. | Language Alternative |
| `dc:format` | The file format, physical medium, or dimensions of the resource. | Prefer using a mediatype when available. | Text mediatype |
| `dc:identifier` | An unambiguous reference to the resource within a given context | Can be in the form of URN or text. | URN or Text |
| `dc:language` | A language of the resource | RFC 3066  language code | Array of RFC 3066 codes |
| `dc:publisher` | An entity responsible for making the resource available | A list of Names or urls.| Unordered array of text|
| `dc:relation` | A related resource. Recommended best practice is to identify the related resource by means of a string conforming to a formal identification system | A list of URLS | Unordered array of text |
| `dc:rights`|Information about rights held in and over the resource| We recommend using an [SPDX](https://spdx.org/licenses/) identifier when appropriate, alternative can be a list of informal right statements or URLs | Language Alternative *|
| `dc:source`|A related resource from which the described resource is derived. |When possible prefer a URL. |Text|
| `dc:subject`|The topic of the resource|A list of descriptive phrases or keywords that specify the content of the resource| Unordered array of text |
|`dc:title`| A name given to the resource. | The asset title. | Language Alternative |
|`dc:type`| The nature or genre of the resource | Refer to the [DCMITYPE](http://dublincore.org/documents/dcmi-type-vocabulary/) vocabulary when possible | Unordered array of text|

### Language Alternative
A language alternative is a dictionary mapping a language (RFC 3066 language string) to text. Example:
```
"dc:title" : { 
    "en-us" : "MyModel",
    "it-IT" : "Mio Modello",
},
```

The `dc:rights` Language Alternative was extended to accept the "SPDX" key in order to include the machine-readable [SPDX](https://spdx.org/) tag.



## Example
The following example shows a glTF Mesh with embedded XMP metadata.
```
{
    "meshes": [
      {
        extensions : {
            "KHR_XMP": {
                "dc:contributor" : [ "Creator1Name", "Creator2Email@email.com", "Creator3Name<Email@email.com>"],
                "dc:coverage" : "Bay Area, California, United States",
                "dc:creator" : [ "CreatorName", "CreatorEmail@email.com"],
                "dc:date" : "1997-07-16T19:20:30+01:00",
                "dc:description" : { "en-us": "text"},
                "dc:format" : "model/gltf-binary",
                "dc:identifier" : "urn:stock-id:292930",
                "dc:language" : ["en"],
                "dc:publisher" : ["Company"],
                "dc:relation" : ["https://www.khronos.org/"],
                "dc:rights" : {"en-us": "BSD", "SPDX": "BSD-Protection"},
                "dc:source" : "http://related_resource.org/derived_from_this.gltf",
                "dc:subject" : ["architecture"],
                "dc:title" : { "en-us" : "MyModel"},
                "dc:type" : ["Physical Object"],
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
