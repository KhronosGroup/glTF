<!--
Copyright 2018-2020 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

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
- Mikko Haapoja, [@mikkoh](http://github.com/mikkoh)
- Eric Chadwick, Wayfair, [@echadwick-wayfair](https://github.com/echadwick-wayfair)

Copyright 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple material variants of an asset, structured to allow low-latency switching at runtime.

A typical use case is digital commerce, where a user might be presented with e.g. a pair of sneakers and the ability to switch between different colours.

![pair of sneakers](sneakers.png)

## Design Goals

_This section is non-normative._

This extension aims to give asset creators and consumers a compact way to define multiple materials for a common set of geometry nodes in a glTF document. We explicitly optimize for the case of representing material variants which are:

- **finite** - the list of variants is bounded; multi-dimensional variants (e.g. color + texture) are possible but must ultimately be representable as a flattened list
- **premade** - the variants are produced at asset authoring time
- **holistic** - while composable, the variants are defined at the granularity of the glTF asset; sub-variants or hierarchical variants are not considered

In digital commerce, each variant may correspond to a distinct SKU or stocked product code, which naturally meet these requirements.  In games or movies, each variant may represent a fixed set of curated asset skins which can be represented, moved across the wire, and loaded efficiently.

A non-goal of this extension is to serve _configuration authorship_ use cases (i.e. configuration builders or "configurators"). These have a separate set of concerns, are more subject to complex application-specific business logic, and may benefit from dynamic and continuous configurability with complex builder rules.

While not designed around this use case, we note however that this extension does implicitly communicate at the primitive level which materials are available for that primitive, for any such applications which may be interested in leveraging this information.

## Properties

The `KHR_materials_variants` extension defines two properties — `variants` on the root node and `mappings` on any number of primitive nodes.

## Variants

For a glTF asset, a material `variant` represents a combination of materials that can be applied in unison to a set of primitives based on _mappings_.

Available _variants_ are defined at the glTF root node as an array of objects each with a name property.

Viewer-specific logic defines, for a given instance of a glTF asset, up to one single active variant at a time.

## Mappings

For a given primitive, each mapping item represents a material that should be applied to the primitive when one of its _variants_ is active. Available _mappings_ are defined on the primitive node as an array of objects. Each object specifies a material by its index in the root level `materials` array and an array of variants each by their respective indices in the root level `variants` array, defined above.

Across the entire _mappings_ array, each variant index must be used no more than one time.

When the active variant is referenced in a mapping, a compliant viewer will apply its material to the primitive.

When no mapping contains the active variant, or there is no active variant, a compliant viewer will fall back on vanilla glTF behaviour for the primitive.

## Example

_This section is non-normative._

By way of illustration, imagine a sneaker with four differently colored variants, each of which may have an associated SKU or product name in digital commerce.

| Variants         |
| ---------------- |
| `Yellow Sneaker` |
| `Red Sneaker`    |
| `Black Sneaker`  |
| `Orange Sneaker` |

At the root level, this can be described as follows

```javascript
{
  "asset": {"version": "2.0", "generator": "Fancy 3D Tool" },
  "extensions": {
    "KHR_materials_variants": {
      "variants": [
        {"name": "Yellow Sneaker" },
        {"name": "Red Sneaker"    },
        {"name": "Black Sneaker"  },
        {"name": "Orange Sneaker" },
      ]
    }
  }
}
```

Now consider that the sneaker's shoelaces are made from materials which depend on the overall shoe colour in non-obvious ways:

| Variants                           | Shoelace Material           | Material Index |
| ---------------------------------- | --------------------------- | -------------- |
| `Yellow Sneaker`, `Orange Sneaker` | `Brown Shoelaces`           | 2              |
| `Red Sneaker`                      | `Purple Shoelaces`          | 4              |
| `Black Sneaker`                    | `Yellow Shoelaces`          | 5              |

Looking at the shoelaces mesh, the KHR_materials_variants extension would define this mapping as follows:

```javascript
"meshes": [
  {
    "name": "shoelaces",
    "primitives": [
      {
        ...,
        "extensions": {
          "KHR_materials_variants" : {
            "mappings": [
              {
                "material": 2,
                "variants": [0, 3],
              },
              {
                "material": 4,
                "variants": [1],
              },
              {
                "material": 5,
                "variants": [2],
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
