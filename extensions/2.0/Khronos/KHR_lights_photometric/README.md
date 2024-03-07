# KHR_lights_photometric

## Contributors

* Bruce Cherniak, Intel, bruce.cherniak@intel.com
* Timothy Blut, NVIDIA, tblut@nvidia.com
* Lutz Kettner, NVIDIA, lkettner@nvidia.com
* ... (many more)

Copyright (C) 2018-2022 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft
<!--TODO: Draft or Stable-->

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension enables the use of photometric light profiles as light sources within a scene.  Photometric light sources model the distribution of measured light intensity or of relative illuminating power.  Two popular formats for expressing photometric lights are IES and EULUMDAT.  Intensity distributions may also be generated procedurally.

This extension creates a glTF json schema that conveys the lighting distribution information, agnostic of the the file format; and provides sample code to create simple file importers and converters.


(EULUMDAT is a data file format used for specification of photometric data especially intensity distributions from light sources such as lamps and luminaries. The file extension is .ldt. The format was proposed by Axel Stockmar (Light Consult Inc., Berlin) in 1990. The format is the European equivalent to the IES file format specified in IESNA LM-63.)<sup>1</sup>

(EULUMDAT (also known as LDT, after its file extension) is the de-facto industry standard photometric data file in Europe. Without a recognised standards organization to define and maintain it, EULUMDAT has remained largely unchanged since its introduction in 1990. Unfortunately, it also means that there is no publication available which officially documents the file format.)<sup>2</sup>


The light profiles are referenced by nodes and inherit their transform. This also allows changing the light profile's orientation.

A conforming implementation of this extension must be able to load the light profiles defined in the asset and render the asset using those lights.

## Defining Light Profiles

Light profiles are defined within the array `lights` which is defined in the `extensions.KHR_lights_photometric` property located in the top-level object of the asset.

Each light profile defines XXX <!--TODO: -->.

```javascript
"extensions" : {
  "KHR_lights_photometric" : {
    "lights": [
      {
        // luminous intensity distribution (float values, 2D for asymetric illumination, column-major)
        "intensityDistribution": {
          "dimensions": [1, 13],
          "data": [ 1.0, .95, .94, .90, .88, .87, .98, .87, .88, .90, .94, .95, 1.0 ]
        },
        "extras": {
          "test": "Test ABC",
          "testlab": "Lab ABC",
          "manufac": "Manufacturer ABC"
        }
      }
    ]
  }
}
```

### Light Profile Properties

XXX
<!--TODO: Draft or Stable-->

## Adding Light Profile Instances to Nodes

Light profiles can be attached to a node by defining the `extensions.KHR_lights_photometric` property and, within that, the `light` property which defines an index into the `lights` array. Optionally, the `multiplier` and `orientation` properties can be defined to scale the light's intensity and change it's orientation, respectively.

```javascript
"nodes": [
    {
        "extensions": {
            "KHR_lights_photometric" : {
                "light": 0
            }
        }
    }
]
```

The light will inherit the transform of the node. The light's `orientation` is to be applied before the node's transform.

### Light Profile Instance Properties

| Property | Description | Required |
|:---------|:------------|:---------|
| `light` | Index of the light profile. | :white_check_mark: Yes |
| `multiplier` | Non-negative factor to scale the light distribution intensity. | No, Default: `1.0` |
| `orientation` | Unit quaternion in the order (x, y, z, w) that defines the orientation of the light distribution. | No, Default: `[0, 0, 0, 1]` |

## glTF Schema Updates

### JSON Schema

* [glTF.KHR_lights_photometric.schema.json](schema/glTF.KHR_lights_photometric.schema.json)
* [node.KHR_lights_photometric.schema.json](schema/node.KHR_lights_photometric.schema.json)
* [light.schema.json](schema/light.schema.json)

## Known Implementations

* [Intel OSPRay Studio](https://www.ospray.org/ospray_studio/)

## Resources

1. https://en.wikipedia.org/wiki/EULUMDAT
2. http://www.photometrictesting.co.uk/File/understanding_photometric_data_files.php

* [IES Standard File Format for Electronic Transfer of Photometric Data and Related Information, IES Computer Committee](http://www.iesna.org)

Permissively (MIT) licensed header-only IES and LDT file loaders
* https://github.com/fknfilewalker/tinyies
* https://github.com/fknfilewalker/tinyldt


## Appendix: Full Khronos Copyright Statement

Copyright 2018-2022 The Khronos Group Inc.

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
