<!--
Copyright 2013-2017 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR_draco_mesh_compression

## Contributors

- Fan Zhang, Google, <mailto:zhafang@google.com>
- Ondrej Stava, Google, <mailto:ostava@google.com>
- Frank Galligan, Google, <mailto:fgalligan@google.com>
- Kai Ninomiya, Google, <mailto:kainino@google.com>
- Patrick Cozzi, Cesium, [@pjcozzi](https://twitter.com/pjcozzi)

Copyright 2013-2017 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.
## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a schema to use [Draco geometry compression (non-normative)](https://github.com/google/draco) libraries in glTF format. This allows glTF to support streaming compressed geometry data instead of the raw data. This extension specification is based on [Draco bitstream version 2.2 (the entirety of this specification is normative and included in Scope)](https://google.github.io/draco/spec/).

The [conformance](#conformance) section specifies what an implementation must do when encountering this extension, and how the extension interacts with the attributes defined in the base specification.

## glTF Schema Updates

If a `primitive` contains an `extension` property and the `extension` property defines its `KHR_draco_mesh_compression` property, then the Draco geometry compression must be used.

The following picture shows the structure of the schema update.

**Figure 1**: Structure of geometry compression extension.
![](figures/structure.png)

The Draco extension points to the `bufferView` that contains the compressed data.
If the uncompressed version of the asset is not provided, then `KHR_draco_mesh_compression` must be added to `extensionsRequired`.
```javascript
"extensionsRequired" : [
    "KHR_draco_mesh_compression"
]

```
If the `KHR_draco_mesh_compression` property is set in `extensionsRequired` then the primitive must only contain the Draco compressed data.
If a Draco compressed version of the asset is provided then `KHR_draco_mesh_compression` must be added to `extensionsUsed`.

Usage of the extension must be listed in the `extensionsUsed`.

```javascript
"extensionsUsed" : [
    "KHR_draco_mesh_compression"
]

```

Below is an example of what part of a glTF file will look like if the Draco extension is set. Note that all other nodes stay the same except `primitives`:

```javascript

"mesh" : {
    "primitives" : [
        {
            "attributes" : {
                "POSITION" : 11,
                "NORMAL" : 12,
                "TEXCOORD_0" : 13,
                "WEIGHTS_0" : 14,
                "JOINTS_0" : 15
            },
            "indices" : 10,
            "mode" : 4,
            "extensions" : {
                "KHR_draco_mesh_compression" : {
                    "bufferView" : 5,
                    "attributes" : {
                        "POSITION" : 0,
                        "NORMAL" : 1,
                        "TEXCOORD_0" : 2,
                        "WEIGHTS_0" : 3,
                        "JOINTS_0" : 4
                    }
                }
            }
        }
    ]
}

"bufferViews" : [
    // ...
    // bufferView of Id 5
    {
        "buffer" : 10,
        "byteOffset" : 1024,
        "byteLength" : 10000
    }
    // ...
}

```
#### bufferView
The `bufferView` property points to the buffer containing compressed data. The data must be passed to a mesh decoder and decompressed to a mesh.

### attributes
`attributes` defines the attributes stored in the decompressed geometry. e.g., in the example above, `POSITION`, `NORMAL`, `TEXCOORD_0`, `WEIGHTS_0` and `JOINTS_0`. Each attribute is associated with an attribute id which is its unique id in the compressed data. The `attributes` defined in the extension must be a subset of the attributes of the primitive. To request an attribute, loaders must be able to use the correspondent attribute id specified in the `attributes` to get the attribute from the compressed data.

### accessors
The `accessors` properties corresponding to the `attributes` and `indices` of the `primitives` must match the decompressed data.

#### Restrictions on geometry type
When using this extension, the `mode` of `primitive` must be either `TRIANGLES` or `TRIANGLE_STRIP` and the mesh data will be decoded accordingly.

### JSON Schema

For full details on the `KHR_draco_mesh_compression` extension properties, see the schema:

- [extension property (the entirety of this specification is normative and included in Scope)](schema/mesh.primitive.KHR_draco_mesh_compression.schema.json)

## Conformance

Below is the recommended process when a loader encounters a glTF asset with the Draco extension set:

- If `KHR_draco_mesh_compression` is in `extensionsRequired` and the loader does not support the Draco extension, then the loader must fail loading the asset.
- If the loader does not support the Draco extension and `KHR_draco_mesh_compression` is not in `extensionsRequired`, then load the glTF asset ignoring `KHR_draco_mesh_compression` in `primitive`.
- If the loader does support the Draco extension, but will not process `KHR_draco_mesh_compression`, then the loader must load the glTF asset ignoring `KHR_draco_mesh_compression` in `primitive`.
- If the loader does support the Draco extension, and will process `KHR_draco_mesh_compression` then:
    - The loader must process `KHR_draco_mesh_compression` first. The loader must get the data from `KHR_draco_mesh_compression`'s `bufferView` property and decompress the data using a Draco decoder to a Draco geometry.
    - Then the loader must process `attributes` and `indices` properties of the `primitive`. When loading each `accessor`, you must ignore the `bufferView` and `byteOffset` of the `accessor` and go to the previously decoded Draco geometry in the `primitive` to get the data of indices and attributes. A loader must use the decompressed data to fill the `accessors` or render the decompressed Draco geometry directly.
    - If additional attributes are defined in `primitive`'s `attributes`, but not defined in `KHR_draco_mesh_compression`'s `attributes`, then the loader must process the additional attributes as usual.

## Implementation note

*This section is non-normative.*

To prevent transmission of redundant data, exporters should generally write compressed Draco data into a separate buffer from the uncompressed fallback, and shared data into a third buffer. Loaders may then optimize to request only the necessary buffers.

Draco compression may change the order and number of vertices in a mesh. To satisfy the requirement that `accessors` properties be correct for both compressed and uncompressed data, generators should create uncompressed `attributes` and `indices` using data that has been decompressed from the Draco buffer, rather than the original source data.

## Resources

*This section is non-normative.*

- [Draco Open Source Library](https://github.com/google/draco)
- three.js [GLTFLoader.js](https://github.com/mrdoob/three.js/blob/r149/examples/jsm/loaders/GLTFLoader.js) and [DracoLoader.js](https://github.com/mrdoob/three.js/blob/r149/examples/jsm/loaders/DRACOLoader.js)
- [Khronos glTF 2.0 Sample Viewer Draco decoder](https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/v.1.0.9/source/gltf/primitive.js)
- [glTF Transform Draco decoder](https://github.com/donmccurdy/glTF-Transform/blob/v2.5.1/packages/extensions/src/khr-draco-mesh-compression/draco-mesh-compression.ts#L165-L223)

## Appendix: Full Khronos Copyright Statement

Copyright 2013-2017 The Khronos Group Inc.

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
