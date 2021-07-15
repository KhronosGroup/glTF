# KHR_materials_physicalScale

## Contributors

* Ben Houston, Threekit, <https://twitter.com/BenHouston3D>

Copyright (C) 2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.

See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright

## Status

Proposed

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Reuse of models has been greatly aided by requiring 3D models to be created on a real-world scale.  This requirement has not been pushed onto materials.  As such materials are not as easily reused without applying arbitrary scales.  The solution to this is to allow for materials to specify how they can be mapped to real-world scale.  This extension proposed a simple solution to this - merely materials can specify that they support real world scale by specifying their scale factors relative to 1m in each direction.

## Scenarios Enabled

### Floor and Wall AR Experiences

There are AR experiences that can be enabled with glTF-based physical scale materials that do not involve any glTF-transmitted 3D models.  For example, one could preview a new flooring material (carpet, hardwood) or wall covering (tiles, wallpaper, or paint) using ARKit or ARCore to identify the walls or floors planes and then 3D extents in the real world.  Then it could create this geometry and apply the physically scaled material to it.  Thus allowing you to preview these home improvements.

### Reusable Material Libraries

Material-only glTFs are already supported.  This can enable the transfer around of materials by themselves, enabling material libraries.  But for a material which represents a real-world material to look correct on an arbitrary model, it needs to be applied such that its real-world scale is preserved.  Right now there is no concept of a real-world material scale in materials themselves, rather the apparent scale of materials at the moment is a combination of the real world based on combining the physical scale of the models, along with the arbitrary UV scale on the models combined with the arbitrary UV scale of the materials.  This extension enables one to eliminate the two arbitrary scales used currently that determine the physical scale of the materials so that there is an explicit physical scale on materials.

## Extending Materials

Materials that support real-world scale declare this support by adding the `KHR_materials_physicalScale` extension to any glTF material. 

For example, the following defines a material whose UV's space maps onto a 1 meter by 1 meter square.

```json
{
    "materials": [
        {
            "name": "velvet",
            "extensions": {
                "KHR_materials_physicalScale": {
                    "physicalScale": [1.0, 1.0]
                }
            }
        }
    ]
}
```

### Physical Scale

The following parameters are contributed by the `KHR_materials_physicalScale` extension:

|                                  | Type                                                                            | Description                            | Required                       |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|--------------------------------|
|**physicalScale**                   | `array`                                                                         | The ratio of material UVs to 1 meter square        | No, default: `[1.0, 1.0]` |


## Implementation Notes

### Suggested Orientations of Orientable Materials

In order to ensure easy reuse, many textures have orientable physical materials.  The following list makes suggestions for orientations of these materials:

* Fabric: U is along the grain, while V is cross grain.
* Wood Grain: U is along the grain, while V is cross gain.
* Wall Coverings: U is along the horizontal, while V is along the vertical.
* Hardwood Tilings: U is the long side (e.g. along the lengths planks, or tiles), while V is the short side.
* Tiles: U is the long side, while V is the short side.

In all cases, it is assumed that U dimension is from left to right, and V dimension goes from top to bottom.

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
