# KHR_implicit_shapes

## Contributors

* Eoin Mcloughlin, Microsoft, <mailto:eomcl@microsoft.com>
* Rory Mullane, Microsoft, <mailto:romul@microsoft.com>
* George Tian, Microsoft, <mailto:geotian@microsoft.com>
* Aaron Franke, Godot Engine, <mailto:arnfranke@yahoo.com>
* Eric Griffith, Meta, <mailto:ericgriffith@meta.com>
* Janine Liu, Cesium, <mailto:janine@cesium.com>
* Sean Lilley, Cesium, <mailto:sean@cesium.com>

## Status

Draft

## Dependencies

Written against glTF 2.0 spec.

## Overview

This extension adds data definitions for implicit shapes. This extension does not mandate any particular behaviour for these objects aside from a description of their geometry. These types are to be used in combination with other extensions which should specify the behavior of these types.

## glTF Schema Updates

### Shapes

This extension provides a set of document-level objects, which can be referenced by objects in a glTF document.

Shapes are defined within a dictionary property in the glTF scene description file, by adding an `extensions` property to the top-level glTF 2.0 object and defining a `KHR_implicit_shapes` property with a `shapes` array inside it.

Each shape defines a mandatory `type` property which designates the type of shape, as well as an additional structure which provides parameterizations specific to that type. The following example defines a sphere.

```javascript
"extensions": {
    "KHR_implicit_shapes" : {
        "shapes": [
            {
                "sphere": { "radius": 0.1 },
                "type": "sphere"
            }
        ]
    }
}
```

To describe the geometry which represents the object, shapes must define at most one of the following properties:

| |Type|Description|
|-|-|-|
|**sphere**|`object`|A sphere centered at the origin in local space.|
|**box**|`object`|An axis-aligned box centered at the origin in local space.|
|**cylinder**|`object`|A cylinder centered at the origin and aligned along the Y axis in local space, with potentially different radii at each end. A cone is a special case of cylinder when one of the radii is zero.|
|**capsule**|`object`|A capsule (cylinder with hemispherical ends) centered at the origin and defined by two "capping" spheres with potentially different radii, aligned along the Y axis in local space.|

When the `type` of a shape refers to a shape type defined by this extension, the shape must not supply a value for a different shape type e.g. a shape whose `type` is "box" must not have a defined `sphere` property. Similarly, if `type` does not reference a shape type declared by `KHR_implicit_shapes`, an extension or other mechanism should supply an alternate shape definition.

Degenerate shapes are prohibited. A sphere must have a positive, non-zero radius. A box shape must have positive non-zero values for each component of `size`. The capsule shape must have a positive, non-zero `height` and both `radiusTop` and `radiusBottom` must be positive. A cylinder shape must have a positive, non-zero `height`, both `radiusTop` and `radiusBottom` must be non-negative, and at least one of `radiusTop` and `radiusBottom` must be non-zero.

### JSON Schema

* **JSON schema**: [glTF.KHR_implicit.schema.json](schema/glTF.KHR_implicit_shapes.schema.json)

### Object Model

With consideration to the glTF 2.0 Asset Object Model Specification document, the following pointer templates represent mutable properties defined in this extension.

| Pointer | Type|
|-|-|
| `/extensions/KHR_implicit_shapes/shapes/{}/box/size` | `float3`|
| `/extensions/KHR_implicit_shapes/shapes/{}/capsule/height` | `float`|
| `/extensions/KHR_implicit_shapes/shapes/{}/capsule/radiusBottom` | `float`|
| `/extensions/KHR_implicit_shapes/shapes/{}/capsule/radiusTop` | `float`|
| `/extensions/KHR_implicit_shapes/shapes/{}/cylinder/height` | `float`|
| `/extensions/KHR_implicit_shapes/shapes/{}/cylinder/radiusBottom` | `float`|
| `/extensions/KHR_implicit_shapes/shapes/{}/cylinder/radiusTop` | `float`|
| `/extensions/KHR_implicit_shapes/shapes/{}/sphere/radius` | `float`|

Additional read-only properties

| Pointer | Type|
|-|-|
| `/extensions/KHR_implicit_shapes/shapes.length` | `int`|

## Appendix: Full Khronos Copyright Statement

Copyright 2021-2023 The Khronos Group Inc.

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
