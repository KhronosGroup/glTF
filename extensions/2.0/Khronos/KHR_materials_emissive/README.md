# KHR\_materials\_emissive\_strength

## Contributors

* Ben Houston, Threekit, [@bhouston](https://twitter.com/BenHouston3D)
* Ed Mackey, Analytical Graphics, [@emackey](https://twitter.com/emackey)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
* Alex Wood, Analytical Graphics, [@abwood](https://twitter.com/abwood)

Copyright (C) 2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

The core glTF 2.0 material model includes an `emissiveFactor` and an `emissiveTexture` to control the color and
intensity of the light being emitted by the material, clamped to the range [0.0, 1.0]. However, in PBR environments
with high-dynamic range reflections and lighting, stronger emission effects may be desirable.

In this extension, a new `emissiveStrength` scalar factor is supplied, that governs the upper limit of emissive
strength per material. This strength can be colored and tempered using the core `emissiveFactor`
and `emissiveTexture` controls, permitting the strength to vary across the surface of the material.
Supplying values above 1.0 for `emissiveStrength` can have an influence on reflections, tonemapping, blooming, and more.

For systems where a physical light unit is needed, the units for `emissiveStrength` are kilonits (thousands of
nits, where a nit is candela per square meter: **1000 cd/m<sup>2</sup>**).  This is intended to be roughly
equivalent to a very bright display screen.

Specifying the default value of `1.0` for `emissiveStrength` should not produce any change from glTF's core emissive
material parameters.  [Unless the implementation can do something with the physical units defined?]

[Alternative: we could default emissiveStrength to 1000 and have it specified in terms of nits.]

## Extending Materials

Any material with an `emissiveFactor` (and optionally an `emissiveTexture`) can have its strength modulated
or amplified by the inclusion of this extension.  For example:

```json
{
    "materials": [
        {
            "emissiveFactor": [
                1.0,
                1.0,
                1.0
            ],
            "emissiveTexture": {
                "index": 3
            },
            "extensions": {
                "KHR_materials_emissive_strength": {
                    "emissiveStrength": 5.0
                }
            }
        }
    ]
}
```

In the above example, the `emissiveFactor` has been set to its maximum value, to enable the `emissiveTexture`.
The `emissiveStrength` has been set to 5.0, making the texture five times brighter than it otherwise
would have been.  

### Parameters

The following parameters are contributed by the `KHR_materials_emissive_strength` extension:

| Name                   | Type       | Description                                    | Required           |
|------------------------|------------|------------------------------------------------|--------------------|
| **emissiveStrength**   | `number`   | The maximum strength of the emissive texture.  | No, default: `1.0` |


## Implementation Notes

*This section is non-normative.*


## Appendix: Full Khronos Copyright Statement

Copyright 2021 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

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
