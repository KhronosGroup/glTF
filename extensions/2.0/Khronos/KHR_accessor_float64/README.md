<!--
Copyright 2025 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_accessor\_float64

## Contributors

- Alexey Knyazev, [@lexaknyazev](https://github.com/lexaknyazev)
- Sean, [@spnda](https://github.com/spnda)

Copyright 2025 The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 Specification.

## Overview

The base glTF 2.0 Specification supports only one floating-point component type—IEEE-754 _binary32_ (also known as single-precision). Although this is sufficient for most applications, there are use cases that benefit from increased range and precision provided by the 64-bit floating-point data type (also known as double-precision).

This extension expands the set of recognized accessor component types with a new enum value `5130` that represents IEEE-754 _binary64_.

This extension allows usage of such accessors in all places where the single-precision floating-point component type is accepted by the base glTF 2.0 Specification or extensions.

Because this extension does not provide a way to specify alternative data representations, assets that use this extension **MUST** specify it in the `extensionsRequired` array.

## Extending Accessors

When the extension is supported, the following data type is valid for accessors in addition to the types defined in the [Accessor Data Types](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types) section of the glTF 2.0 Specification.

| `componentType` | Data Type | Signed | Bits |
|-----------------|-----------|--------|------|
| `5130`          | _double_  | Signed |  64  |

The `normalized` property of such accessors **MUST NOT** be set to true.

When a _double_ accessor refers to a buffer view that refers to a GLB-stored buffer, the start of the buffer **SHOULD** be aligned to an 8-byte boundary by adding necessary trailing padding bytes to the previous chunks as defined in the [GLB File Format Specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#chunks).

## Extending Accessor Usage

When the extension is supported, it is valid to use _double_ component type accessors in **all** places where _float_ component type accessors are valid.

The following sections are not exhaustive and cover only the base glTF 2.0 Specification and some ratified extensions.

### Mesh Attributes

When the extension is supported, _double_ component type accessors are valid for all mesh and morph target attributes that support _float_ component type accessors as defined in the [Meshes](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#meshes-overview) and [Morph Targets](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#morph-targets) sections of the glTF 2.0 Specification respectively.

#### Interaction with `EXT_mesh_gpu_instancing`

When the extension is supported, _double_ component type accessors are valid for all instanced attribute semantics defined in the `EXT_mesh_gpu_instancing` extension.

### Skins

When the extension is supported, _double_ component type accessors are valid for inverse-bind matrices defined in the [Skins](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#skins-overview) section of the glTF 2.0 Specification.

### Animation Samplers

When the extension is supported, _double_ component type accessors are valid for animation sampler inputs and outputs defined in the [Animations](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#animations) section of the glTF 2.0 Specification.

Interpolation of double-precision keyframe data **MUST** be performed with double-precision arithmetic.

#### Interaction with `KHR_animation_pointer`

When the extension is supported, _double_ component type accessors are valid for keyframe data of all Object Model properties of `float*` data types.

## Compatibility

Since not all graphics environments support double-precision floating-point data, implementations **MAY** reject assets using double-precision data for mesh attributes or any other purpose that would require native hardware support.

If an implementation converts double-precision data to single-precision due to platform limitations, it **SHOULD** warn users that the asset's rendering can be incorrect.

Given transmission size and portability implications, _double_ component type accessors **SHOULD NOT** be used unless absolutely necessary.

## Appendix: Full Khronos Copyright Statement

Copyright 2025 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified
Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent,
trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this Specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
https://www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those
specifically identified sections define normative functionality. The Khronos Intellectual
Property Rights Policy excludes external references to materials and associated enabling
technology not created by Khronos from the Scope of this Specification, and any licenses
that may be required to implement such referenced materials and associated technologies
must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All
other product names, trademarks, and/or company names are used solely for identification
and belong to their respective owners.
