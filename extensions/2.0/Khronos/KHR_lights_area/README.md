# KHR\_lights\_area

## Contributors

* Ben Houston, Threekit, <https://twitter.com/BenHouston3D>

Copyright (C) 2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Proposed

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines common area lights for use with glTF 2.0.

Many 3D tools and engines support built-in implementations of area shapes. Using this extension, tools can export and engines can import these lights.

This extension defines three area light shapes: `sphere`, `disk` and `rect`.

These lights are referenced by nodes and inherit the transform of that node.

A conforming implementation of this extension must be able to load light data defined in the asset and has to render the asset using those lights.

## Defining Rect Area Lights

Lights are defined within a dictionary property in the glTF scene description file, by adding an `extensions` property to the top-level glTF 2.0 object and defining a `KHR_lights_rect_area` property with a `lights` array inside it.

If the area lights are flat, then it is assumed that light is only emitted on the surface facing in the -Z direction.

The following example defines 3 area lights:

```javascript
"extensions": {
    "KHR_lights_area" : {
        "lights": [
            {
                "intensity": 1.0,
                "color": [
                    1.0,
                    1.0,
                    1.0
                ],
                "shape": "sphere",
                "radius": 1.0,
            },
            {
                "intensity": 1.0,
                "color": [
                    1.0,
                    1.0,
                    1.0
                ],
                "shape": "disk",
                "radius": 1.0,
            },
            {
                "intensity": 1.0,
                "color": [
                    1.0,
                    1.0,
                    1.0
                ],
                "shape": "rect",
                "width": 1.0,
                "height": 1.0,
            },
         ]
    }
}
```

## Adding Light Instances to Nodes

Lights must be attached to a node by defining the `extensions.KHR_lights_area` property and, within that, an index into the `lights` array using the `light` property.

```javascript
"nodes" : [
    {
        "extensions" : {
            "KHR_lights_area" : {
                "light" : 0
            }
        }
    }
]
```

The light will inherit the transform of the node.

## Rect Area Properties

| Property | Description | Required |
|:-----------------------|:------------------------------------------| :--------------------------|
| `name` | Name of the light. | No, Default: `""` |
| `color` | RGB value for light's color in linear space. | No, Default: `[1.0, 1.0, 1.0]` |
| `intensity` | Brightness of light in. The units that this is defined in depend on the shape of light in terms of nits (1 nit = 1 lm/sr/m^2) | No, Default: `1.0` |
| `shape` | Declares the shape of the light. | :white_check_mark: Yes |
| `width` | The centered width of a rect light. Supported only for `rect` lights. | No, Default: `1.0` |
| `height` | The centered height of a rect light. Supported only for `rect` lights. | No, Default: `1.0` |
| `radius` | The radius of a disk or sphere light. Supported only for `disk` and `sphere` lights. | No, Default: `1.0` |

QUESTION: If an area light is scaled down by parent transforms, does it emit less light because it has less world space area?  Currently this would be the behavior given the current definition.  This seems intuitively like it makes sense.

## Appendix: Full Khronos Copyright Statement

Copyright 2017-2018 The Khronos Group Inc.

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

Vulkan is a registered trademark and Khronos, OpenXR, SPIR, SPIR-V, SYCL, WebGL,
WebCL, OpenVX, OpenVG, EGL, COLLADA, glTF, NNEF, OpenKODE, OpenKCAM, StreamInput,
OpenWF, OpenSL ES, OpenMAX, OpenMAX AL, OpenMAX IL, OpenMAX DL, OpenML and DevU are
trademarks of The Khronos Group Inc. ASTC is a trademark of ARM Holdings PLC,
OpenCL is a trademark of Apple Inc. and OpenGL and OpenML are registered trademarks
and the OpenGL ES and OpenGL SC logos are trademarks of Silicon Graphics
International used under license by Khronos. All other product names, trademarks,
and/or company names are used solely for identification and belong to their
respective owners.
