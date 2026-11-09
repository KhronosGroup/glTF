# KHR\_materials\_emissive\_strength

## Contributors

- Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)
- Alex Jamerson, Amazon
- Thomas Dideriksen, Amazon
- Alex Wood, AGI, [@abwood](https://github.com/abwood)
- Ed Mackey, AGI, [@emackey](https://github.com/emackey)
- Nicolas Savva, Autodesk, [@nicolassavva-autodesk](https://github.com/nicolassavva-autodesk)
- Henrik Edstrom, Autodesk
- Tobias Haeussler, Dassault Systemes, [@proog128](https://github.com/proog128)
- Bastian Sdorra, Dassault Systemes, [@bsdorra](https://github.com/bsdorra)
- Emmett Lalish, Google, [@elalish](https://github.com/elalish)
- Richard Sahlin, IKEA, [@rsahlin](https://github.com/rsahlin)
- Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)
- Bruce Cherniak, Intel
- Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
- Nicholas Barlow, Microsoft
- Adam Morris, Target [@weegeekps](https://github.com/weegeekps)
- Sandra Voelker, Target
- Ben Houston, Threekit, [@bhouston](https://twitter.com/BenHouston3D)
- Andreas Atteneder, Unity
- Norbert Nopper, UX3D, [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
- Eric Chadwick, Wayfair, [echadwick-wayfair](https://github.com/echadwick-wayfair)

Copyright (C) 2021-2022 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

The core glTF 2.0 material model includes an `emissiveFactor` and an `emissiveTexture` to control the color and
intensity of the light being emitted by the material, clamped to the range [0.0, 1.0]. However, in PBR environments
with high-dynamic range reflections and lighting, stronger emission effects may be desirable.

In this extension, a new `emissiveStrength` scalar factor is supplied, that governs the upper limit of emissive
strength per material.

**Implementation Note**: This strength can be colored and tempered using the core material's `emissiveFactor`
and `emissiveTexture` controls, permitting the strength to vary across the surface of the material.
Supplying values above 1.0 for `emissiveStrength` can have an influence on
reflections, tonemapping, blooming, and more.

### Physical Units

This extension supplies a unitless multiplier to the glTF 2.0 specification's emissive factor and
texture.  Including this multiplier does not alter the physical units defined in glTF 2.0's
[additional textures section](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#additional-textures),
under the **emissive** texture.

## Extending Materials

*This section is non-normative.*

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

| Name                   | Type       | Description                                                                   | Required           |
|------------------------|------------|-------------------------------------------------------------------------------|--------------------|
| **emissiveStrength**   | `number`   | The strength adjustment to be multiplied with the material's emissive value.  | No, default: `1.0` |


## Implementation Notes

*This section is non-normative.*

A typical (pseudocode) implementation might look like the following:

```
color += emissiveFactor.rgb * sRGB_to_Linear(emissiveTexture.rgb) * emissiveStrength;
```

## Schema

- [material.KHR_materials_emissive_strength.schema.json](schema/material.KHR_materials_emissive_strength.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2021-2022 The Khronos Group Inc.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast,
or otherwise exploited in any manner without the express prior written permission
of Khronos.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
https://www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters under the process defined by Khronos for this specification;
see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

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
SPIR™, SPIR&#8209;V™, SYCL™, OpenVG™ and 3D Commerce™ are trademarks of The Khronos Group Inc.
OpenXR™ is a trademark owned by The Khronos Group Inc. and is registered as a trademark in
China, the European Union, Japan and the United Kingdom. OpenCL™ is a trademark of Apple Inc.
and OpenGL® is a registered trademark and the OpenGL ES™ and OpenGL SC™ logos are trademarks
of Hewlett Packard Enterprise used under license by Khronos. ASTC is a trademark of
ARM Holdings PLC. All other product names, trademarks, and/or company names are used solely
for identification and belong to their respective owners.
