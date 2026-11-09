<!--
Copyright 2017-2018 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR\_materials\_unlit

## Contributors

- Don McCurdy, Google, [@donrmccurdy](https://twitter.com/donrmccurdy)
- Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
- Gary Hsu, Microsoft, [@bghgary](https://twitter.com/bghgary)
- Scott Nagy, Microsoft, [@visageofscott](https://twitter.com/visageofscott)
- Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
- Michael Feldstein, Facebook, [@msfeldstein](https://twitter.com/msfeldstein)
- Robert Long, Mozilla, [@arobertlong](https://twitter.com/arobertlong)

Copyright 2017-2018 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines an unlit shading model for use in glTF 2.0
materials, as an alternative to the Physically Based Rendering (PBR) shading
models provided by the core specification. Three motivating uses cases for
unlit materials include:

- Mobile devices with limited resources, where unlit materials offer a
performant alternative to higher-quality shading models.
- Photogrammetry, in which lighting information is already present and
additional lighting should not be applied.
- Stylized materials (such as "anime" or "hand-drawn" looks) in which lighting is
undesirable for aesthetic reasons.

These use cases are not mutually exclusive: artists may pick an unlit material
for performance reasons, and make aesthetic decisions to complement that
choice. As a result, client implementations capable of rendering PBR should not
automatically "upgrade" to fully-shaded PBR. Any core PBR properties specified
on an unlit material (except `baseColor`) are meant only as fallbacks for
clients that do not support the `KHR_materials_unlit` extension. The extension,
whether required or optional within the asset, indicates preference for an
unlit visual style.

## Extending Materials

The common Unlit material is defined by adding the
`KHR_materials_unlit` extension to any glTF material. When present, the
extension indicates that a material should be unlit and use available
`baseColor` values, alpha values, and vertex colors while ignoring all
properties of the default PBR model related to lighting or color. Alpha
coverage and doubleSided still apply to unlit materials.

```json
{
    "materials": [
        {
            "name": "MyUnlitMaterial",
            "pbrMetallicRoughness": {
                "baseColorFactor": [ 0.5, 0.8, 0.0, 1.0 ]
            },
            "extensions": {
                "KHR_materials_unlit": {}
            }
        }
    ]
}
```

### Definition

The Unlit material model describes a constantly shaded surface that is
independent of lighting. The material is defined only by properties already
present in the [glTF 2.0 material specification](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#materials).
No new properties are added by this extension — it is effectively a Boolean
flag indicating use of an unlit shading model. Additional properties on the
extension object are allowed, but may lead to undefined behavior in conforming
viewers.

Color is calculated as:

```
color = <baseColorTerm>
```

`<baseColorTerm>` is the product of `baseColorFactor`, `baseColorTexture`, and vertex color (if any), as defined by the [core glTF material specification](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material).

### Example

The following example defines an unlit material with a constant red color.

```json
"materials": [
    {
        "name": "red_unlit_material",
        "pbrMetallicRoughness": {
            "baseColorFactor": [ 1.0, 0.0, 0.0, 1.0 ]
        },
        "extensions": {
            "KHR_materials_unlit": {}
        }
    }
]
```

### Extension compatibility and fallback materials

When possible, authoring tools should provide a fallback material definition
and mark the `KHR_materials_unlit` extension as optional. Models
including the extension optionally will still render in all clients that
support the core glTF 2.0 specification, falling back to a PBR metal/rough
material. In clients that do not support the extension, direct lighting will
be applied and performance may be degraded.

Defining a fallback to the default PBR shading model is achieved by setting
appropriate parameters on the default material.

```json
"materials": [
    {
        "name": "unlit_with_fallback",
        "pbrMetallicRoughness": {
            "baseColorFactor": [ 1.0, 1.0, 1.0, 1.0 ],
            "baseColorTexture": { "index": 0 },
            "roughnessFactor": 0.9,
            "metallicFactor": 0.0
        },
        "extensions": {
            "KHR_materials_unlit": {}
        }
    }
]
```

> **Implementation Note:** For best fallback behavior in clients that do not
> implement the `KHR_materials_unlit` extension, authoring tools may use:
>
> * `metallicFactor` is `0` and `emissiveFactor` is `[0, 0, 0]`.
> * `roughnessFactor` is greater than `0.5`.
> * Omit `metallicRoughnessTexture`, `occlusionTexture`, `emissiveTexture`,
> and `normalTexture`.

> **Implementation Note:** When `KHR_materials_unlit` is included with another
> extension specifying a shading model on the same material, the result is
> undefined.

## Schema

- [material.KHR_materials_unlit.schema.json](schema/material.KHR_materials_unlit.schema.json)

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
