# KHR_materials_variants

## Contributors

- Mike Bond, Adobe, [@Miibond](https://github.com/MiiBond)
- Aurélien Vaysset, Emersya, [@aurelienvaysset](https://github.com/aurelienvaysset)
- Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
- Renee Rashid, Facebook, [@debuggrl](https://github.com/debuggrl)
- Jeremy Cytryn, Facebook, [@jercytrn](https://github.com/jercytryn)
- Edgar Simson, OLV, [@Edzis](https://github.com/edzis)
- Daniel Beauchamp, Shopify, [@pushmatrix](http://github.com/pushmatrix)
- Stephan Leroux, Shopify, [@sleroux](http://github.com/sleroux)
- Eric Chadwick, Wayfair, [@echadwick-wayfair](https://github.com/echadwick-wayfair)

Copyright (C) 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple material variants of an asset, structured to allow low-latency switching at runtime.

A typical use case is digital commerce, where a user might be presented with e.g. a pair of sneakers and the ability to switch between different colours.

![pair of sneakers](examples/shoes/photo.png)

## Design Goals

_This section is non-normative._

This extension aims to give asset creators and consumers a compact way to define multiple materials for a common set of geometry nodes in a glTF document. We explicitly optimize for the case of representing material variants which are:

* **finite** - the list of variants is bounded; multi-dimensional variants (e.g. color + texture) are possible but must ultimately be representable as a flattened list
* **premade** - the variants are produced at asset authoring time
* **holistic** - while composable, the variants are defined at the granularity of the glTF asset; sub-variants or hierarchical variants are not considered

In digital commerce, each variant may correspond to a distinct SKU or stocked product code, which naturally meet these requirements.  In games or movies, each variant may represent a fixed set of curated asset skins which can be represented, moved across the wire, and loaded efficiently.

A non-goal of this extension is to serve _configuration authorship_ use cases (i.e. configuration builders or "configurators"). These have a separate set of concerns, are more subject to complex application-specific business logic, and may benefit from dynamic and continuous configurability with complex builder rules.

While not designed around this use case, we note however that this extension does implicitly communicate at the primitive level which materials are available for that primitive, for any such applications which may be interested in leveraging this information.

## Material Variants

A _material_variant_ is a set of mappings that determine which material can be applied to a given primitive. Consider a model of a shoe where three meshes are defined along with two materials. These meshes contain an assortment of primitives that each have an associated material attached.

Imagine a sneaker with shoelace holes that are made from materials that depend on the overall shoe colour in non-obvious ways:

| Meshes to Skin                     | Available Materials             |
| ---------------------------------- | ------------------------------- |
| `laces`                            | `white_matte`                   |
| `body`                             | `red_matte`                     |
| `sole`                             |                                 |

In one variant of the shoe, the primitives associated with each mesh listed below would have the following materials assigned to them:

| Meshes                             | Material                        |
| ---------------------------------- | ------------------------------- |
| `laces`                            | `white_matte`                   |
| `body`                             | `red_matte`                     |
| `sole`                             | `white_matte`                   |

In this case, the laces and sole of the shoe would be white while the body of the shoe would be red.

This material variant may have an associated SKU or product name in digital commerce. The shoe, however, also comes in an alternative color scheme that is represented by an alternative set of primitive/material mappings:

| Meshes                             | Material                        |
| ---------------------------------- | ------------------------------- |
| `laces`                            | `red_matte`                     |
| `body`                             | `white_matte`                   |
| `sole`                             | `white_matte`                   |

In the second case, the shoe would have a white body and sole but red laces. In both cases, the meshes and their associated primitives remain the same with the exception of which material is used. Looking at the laces mesh, the KHR_materials_variants extension would define this mapping as follows:

```javascript
"meshes": [
    {
        "name": "laces",
        "primitives": [
            {
                ...,
                "extensions": {
                    "KHR_materials_variants" : {
                        "mappings": [
                            {
                                "variants": [0],
                                "material": 1,
                            },
                            {
                                "variants": [1],
                                "material": 2,
                            },
                        ],
                    }
                }
            },
            // ... more primitives ...
        ]
    },
    // ... more meshes ...
]
```

Viewer-specific logic defines, for a given instance of a glTF asset, up to one single active variant. Each `mappings` item gives the material that a compliant viewer should apply for that primitive when any of its `variants` indices is the active one.  If there are no matching variants found within `mappings`, or there is no active variant, fall back on vanilla glTF behaviour. Across the entire `mappings` array, each variant index and each material must be used no more than one time.

For each mapping, each index of the `variants` property refers to the index of a material variant located at the glTF root node:

```javascript
{
  "asset": {"version": "2.0", "generator": "Fancy 3D Tool" },
  "extensions": {
      "KHR_materials_variants": {
        "variants": [
          {"name": "Red Shoe with White Laces" },
          {"name": "White Shoe with Red Laces" },
        ]
      }
  }
}
```

## Interaction with existing glTF functionality

_This section is non-normative._

The primary purpose of this extension is to simply formalise the idea of static configurability, so that members of the ecosystem can meaningfully communicate: those who produce assets and those who ingest them, and all the tooling in between.

As a secondary effect, material variants allow multiple assets — with shared geometry but different materials — to be stored more compactly, particularly relevant for the self-contained binary format GLB. When using external URIs as references to textures, applications may (optionally) process geometry only once and lazily request texture assets only when needed for a particular variant.

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2020 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements necessary for compliance and so are outside the Scope of this Specification. These parts of the Specification are marked as being non-normative, or identified as Implementation Notes.

Where this Specification includes normative references to external documents, only the specifically identified sections and functionality of those external documents are in Scope. Requirements defined by external documents not created by Khronos may contain contributions from non-members of Khronos not covered by the Khronos Intellectual Property Rights Policy.

This specification is protected by copyright laws and contains material proprietary to Khronos. Except as described by these terms, it or any components may not be reproduced, republished, distributed, transmitted, displayed, broadcast or otherwise exploited in any manner without the express prior written permission of Khronos.

This specification has been created under the Khronos Intellectual Property Rights Policy, which is Attachment A of the Khronos Group Membership Agreement available at www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional copyright license to use and reproduce the unmodified specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent, trademark or other intellectual property rights are granted under these terms. Parties desiring to implement the specification and make use of Khronos trademarks in relation to that implementation, and receive reciprocal patent license protection under the Khronos IP Policy must become Adopters and confirm the implementation as conformant under the process defined by Khronos for this specification; see https://www.khronos.org/adopters.

Khronos makes no, and expressly disclaims any, representations or warranties, express or implied, regarding this specification, including, without limitation: merchantability, fitness for a particular purpose, non-infringement of any intellectual property, correctness, accuracy, completeness, timeliness, and reliability. Under no circumstances will Khronos, or any of its Promoters, Contributors or Members, or their respective partners, officers, directors, employees, agents or representatives be liable for any damages, whether direct, indirect, special or consequential damages for lost revenues, lost profits, or otherwise, arising from or in connection with these materials.

Khronos® and Vulkan® are registered trademarks, and ANARI™, WebGL™, glTF™, NNEF™, OpenVX™, SPIR™, SPIR-V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc. OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc. and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely for identification and belong to their respective owners.
