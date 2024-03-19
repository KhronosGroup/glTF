# KHR_collision_shapes

## Contributors

* Eoin Mcloughlin, Microsoft, <mailto:eomcl@microsoft.com>
* Rory Mullane, Microsoft, <mailto:romul@microsoft.com>
* George Tian, Microsoft, <mailto:geotian@microsoft.com>
* Aaron Franke, Godot Engine, <mailto:arnfranke@yahoo.com>

## Status

Draft

## Dependencies

Written against glTF 2.0 spec.

## Overview

This extension adds the ability to specify collision primitives inside a glTF asset. This extension does not mandate any particular behaviour for those objects aside from their collision geometry. These types are to be used in combination with additional extensions. For example, the KHR\_rigid\_bodies extension adds additional properties to glTF nodes which reference these collision primitives.

## glTF Schema Updates

### Shapes

This extension provides a set of document-level objects, which can be referenced by objects in the scene. The precise usage of these primitives should be specified by the extensions which utilize the shapes. The intended purpose of these these objects are to specify geometry which can be used for collision detection, which has informed the set of shapes which we have defined. Typically, the geometry specified by the shape will be simpler than any render meshes used by the node or it's children, enabling real-time applications to perform queries such as intersection tests.

Each shape defines a mandatory `type` property which designates the type of shape, as well as an additional structure which provides parameterizations specific to that type.

To describe the geometry which represents the object, shapes must define at most one of the following properties:

| |Type|Description|
|-|-|-|
|**sphere**|`object`|A sphere centered at the origin in local space.|
|**box**|`object`|An axis-aligned box centered at the origin in local space.|
|**capsule**|`object`|A capsule centered at the origin and defined by two "capping" spheres with potentially different radii, aligned along the Y axis in local space.|
|**cylinder**|`object`|A cylinder centered at the origin and aligned along the Y axis in local space, with potentially different radii at each end.|
|**convex**|`object`|A convex hull wrapping a `mesh` object.|
|**trimesh**|`object`|A triangulated representation of a `mesh` object.|

The sphere, box, capsule, cylinder and convex types all represent convex objects with a volume, however, the trimesh type always represents an infinitely thin shell or sheet - for example a trimesh created from a `mesh` object in the shape of a box will be represented as a hollow box.

If a shape is required to have an offset from the local space of the node the shape is associated with (for example a sphere _not_ centered at local origin or a rotated box,) a child node should be added with the desired offset applied, and the shape properties should be added to that child.

### JSON Schema

* **JSON schema**: [glTF.KHR_collision_shapes.schema.json](schema/glTF.KHR_collision_shapes.schema.json)

### Object Model

With consideration to the glTF 2.0 Asset Object Model Specification document, the following pointer templates represent mutable properties defined in this extension.

| Pointer | Type|
|-|-|
| `/extensions/KHR_collision_shapes/shapes/{}/box/size` | `float3`|
| `/extensions/KHR_collision_shapes/shapes/{}/capsule/height` | `float`|
| `/extensions/KHR_collision_shapes/shapes/{}/capsule/radiusBottom` | `float`|
| `/extensions/KHR_collision_shapes/shapes/{}/capsule/radiusTop` | `float`|
| `/extensions/KHR_collision_shapes/shapes/{}/cylinder/height` | `float`|
| `/extensions/KHR_collision_shapes/shapes/{}/cylinder/radiusBottom` | `float`|
| `/extensions/KHR_collision_shapes/shapes/{}/cylinder/radiusTop` | `float`|
| `/extensions/KHR_collision_shapes/shapes/{}/sphere/radius` | `float`|

Additional read-only properties

| Pointer | Type|
|-|-|
| `/extensions/KHR_collision_shapes/shapes.length` | `int`|

## Known Implementations

[Blender importer/exporter](https://github.com/eoineoineoin/glTF_Physics_Blender_Exporter)

[Babylon.js importer](https://github.com/eoineoineoin/glTF_Physics_Babylon)

[Godot importer](https://github.com/eoineoineoin/glTF_Physics_Godot_Importer)

## Validator

[glTF validator](https://github.com/eoineoineoin/glTF-Validator)

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
