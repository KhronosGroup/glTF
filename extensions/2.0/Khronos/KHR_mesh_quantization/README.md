<!--
Copyright 2018-2020 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_mesh\_quantization

## Contributors

- Arseny Kapoulkine, [@zeuxcg](https://twitter.com/zeuxcg)
- Alexey Knyazev, [@lexaknyazev](https://github.com/lexaknyazev)

Copyright 2018-2020 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec. Depends on `KHR_texture_transform` for texture coordinate dequantization.

## Overview

Vertex attributes are usually stored using `FLOAT` component type. However, this can result in excess precision and increased memory consumption and transmission size, as well as reduced rendering performance.

This extension expands the set of allowed component types for mesh attribute storage to provide a memory/precision tradeoff - depending on the application needs, 16-bit or 8-bit storage can be sufficient.

Using 16-bit or 8-bit storage typically requires transforming the original floating point values to fit a uniform 3D or 2D grid; the process is commonly known as quantization.

To simplify implementation requirements, the extension relies on existing ways to specify geometry transformation instead of adding special dequantization transforms to the schema.

As an example, a static PBR-ready mesh typically requires `POSITION` (12 bytes), `TEXCOORD` (8 bytes), `NORMAL` (12 bytes) and `TANGENT` (16 bytes) for each vertex, for a total of 48 bytes. With this extension, it is possible to use `SHORT` to store position and texture coordinate data (8 and 4 bytes, respectively) and `BYTE` to store normal and tangent data (4 bytes each), for a total of 20 bytes per vertex with often negligible quality impact.

Because the extension does not provide a way to specify both `FLOAT` and quantized versions of the data, files that use the extension must specify it in `extensionsRequired` array - the extension is not optional.

## Extending Mesh Attributes

When `KHR_mesh_quantization` extension is supported, the following **extra** types are allowed for storing mesh attributes in addition to the types defined in [Section 3.7.2.1](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#meshes-overview).

|Name|Accessor Type(s)|Component Type(s)|
|----|----------------|-----------------|
|`POSITION`|VEC3|_byte_<br>_byte_&nbsp;normalized<br>_unsigned byte_<br>_unsigned byte_&nbsp;normalized<br>_short_<br>_short_&nbsp;normalized<br>_unsigned short_<br>_unsigned short_&nbsp;normalized|
|`NORMAL`|VEC3|_byte_&nbsp;normalized<br>_short_&nbsp;normalized|
|`TANGENT`|VEC4|_byte_&nbsp;normalized<br>_short_&nbsp;normalized|
|`TEXCOORD_n`|VEC2|_byte_<br>_byte_&nbsp;normalized<br>_unsigned byte_<br>_short_<br>_short_&nbsp;normalized<br>_unsigned short_|

Note that to comply with alignment rules for accessors, each element needs to be aligned to 4-byte boundaries; for example, a `BYTE` normal is expected to have a stride of 4, not 3.

For `POSITION` and `TEXCOORD` attributes, the application is free to choose normalized or unnormalized storage, as well as signed or unsigned. When normalized storage is used, often the data doesn't have to be dequantized (which eliminates the need for a dequantization transform); however, if the data is not in `[0..1]` or `[-1..1]` range, using unnormalized integers instead of normalized integers can slightly improve precision as standard glTF normalization factors such as `1/255` and `1/65535` are not representable exactly as floating-point numbers. Note that using unnormalized integers does not change semantics of the stored values - for example, unnormalized integer `2` corresponds to `2.0` in UV space when encoding a texture coordinate - and as such, in this case it's usually necessary to specify a transformation for decoding (see Decoding Quantized Data).

> **Implementation Note:** As quantization may introduce a non-negligible error, quantized normal and tangent vectors are typically not exactly unit length. Applications are expected to normalize the vectors before using them in lighting equations; this typically can be done after transforming them using the normal matrix. Even if quantization is not used, normal matrix can contain scale/skew so normalization is typically required anyway.

> **Implementation Note:** For quantized data, `accessor.min` and `accessor.max` properties also contain quantized values. Applications that use these properties, e.g. for bounding box computations, will need to dequantize them first.

## Extending Morph Target Attributes

When `KHR_mesh_quantization` extension is supported, the following **extra** types are allowed for storing morph target attributes in addition to the types defined in [Section 3.7.2.2](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#morph-targets).

|Name|Accessor Type(s)|Component Type(s)|
|----|----------------|-----------------|
|`POSITION`|VEC3|_byte_<br>_byte_&nbsp;normalized<br>_short_<br>_short_&nbsp;normalized|
|`NORMAL`|VEC3|_byte_&nbsp;normalized<br>_short_&nbsp;normalized|
|`TANGENT`|VEC3|_byte_&nbsp;normalized<br>_short_&nbsp;normalized|
|`TEXCOORD_n`|VEC2|_byte_<br>_short_|

Note that to comply with alignment rules for accessors, `"VEC3"` accessors need to be aligned to 4-byte boundaries; for example, a `BYTE` normal is expected to have a stride of 4, not 3.

For simplicity the specification assumes that morph target displacements are signed; for `NORMAL` and `TANGENT`, models with very large displacements that do not fit into `[-1..1]` range need to use `FLOAT` storage.

## Decoding Quantized Data

Depending on the attribute values and component types used, the attributes may need to be transformed into their original range for display. Instead of relying on separate dequantization transform, this extension relies on existing methods to specify transformation.

For `POSITION` attribute, the transform can be specified in one of two ways:

- For non-skinned meshes, the dequantization transform (which typically consists of scale and offset) can be encoded into `node` transformation - this can require adding an extra leaf `node` so that animations that affect the mesh transformation are not disturbed.

- For skinned meshes, the `node` hierarchy does not affect the transform of the `mesh` directly - however, dequantization transform can be encoded into `inverseBindMatrices` for the `skin` used for the mesh.

In both cases, to preserve the direction of normal/tangent vectors, it is recommended that the quantization scale specified in the transform is uniform across X/Y/Z axes.

For `TEXCOORD` attribute, the transform can be specified using the `offset`/`scale` supplied by `KHR_texture_transform` extension. Note that this requires merging existing `offset`/`scale` values, if present, with the dequantization transform.

For morph target data, it's expected that deltas are quantized using the same transform as the base positions. Extra care must be taken to ensure that the quantization accommodates for large displacements, if present.

## Encoding Quantized Data

It is up to the encoder to determine the optimal range and format, along with the quantization transform, to maximize the output quality. If necessary, different meshes can use different component types for different attributes - this extension does not require that a single component type is used consistently for all attributes of the same type.

> **Implementation Note:** It is possible to determine the range of the attribute for each mesh individually, and pick the 8-bit or 16-bit storage format accordingly; this minimizes the error for each individual mesh, but may prevent sharing for some scene elements (such as reusing a material across multiple meshes with different texture coordinate ranges) and may result in the same vertex being quantized differently, causing visible gaps.

> **Implementation Note:** It is also possible to determine the range of all attributes of all meshes in the scene and use a shared dequantization transform; this can eliminate the gaps between different meshes but can increase the quantization error.

Implementations should assume following equations are used to get corresponding floating-point value `f` from a normalized integer `c` and should use the specified equations to encode floating-point values to integers after range normalization:

|`accessor.componentType`|int-to-float|float-to-int|
|-----------------------------|--------|----------------|
| `5120`&nbsp;(BYTE)          |`f = max(c / 127.0, -1.0)`|`c = round(f * 127.0)`|
| `5121`&nbsp;(UNSIGNED_BYTE) |`f = c / 255.0`|`c = round(f * 255.0)`|
| `5122`&nbsp;(SHORT)         |`f = max(c / 32767.0, -1.0)`|`c = round(f * 32767.0)`|
| `5123`&nbsp;(UNSIGNED_SHORT)|`f = c / 65535.0`|`c = round(f * 65535.0)`|

> **Implementation Note:** Due to OpenGL ES 2.0 / WebGL 1.0 platform differences, some implementations may decode signed normalized integers to floating-point values differently. While the difference is unlikely to be significant for normal/tangent data, implementations may want to use unsigned normalized or signed non-normalized formats to avoid the discrepancy in position data.

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

Vulkan is a registered trademark and Khronos, OpenXR, SPIR, SPIR-V, SYCL, WebGL,
WebCL, OpenVX, OpenVG, EGL, COLLADA, glTF, NNEF, OpenKODE, OpenKCAM, StreamInput,
OpenWF, OpenSL ES, OpenMAX, OpenMAX AL, OpenMAX IL, OpenMAX DL, OpenML and DevU are
trademarks of The Khronos Group Inc. ASTC is a trademark of ARM Holdings PLC,
OpenCL is a trademark of Apple Inc. and OpenGL and OpenML are registered trademarks
and the OpenGL ES and OpenGL SC logos are trademarks of Silicon Graphics
International used under license by Khronos. All other product names, trademarks,
and/or company names are used solely for identification and belong to their
respective owners.
