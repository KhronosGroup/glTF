# EXT\_materials\_bump

## Contributors

- Ben Houston, Threekit [@bhouston](https://github.com/bhouston)

Copyright 2023 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension adds one parameters to the metallic-roughness material: `bump`.

`bump` allows users to perturb the surface normal via a bump map. The bump map is a grayscale texture, where the value of each texel represents the height of the surface at that texel.  The height is then used to perturb the normal direction.

Bump maps were introduced into computer graphics in 1978 by Blinn in "Simulation of Wrinkled Surfaces, while normal maps were introduced in 1989 by Cohen et al, "Appearance-Preserving Simplification."

Benefits of bump maps over normal maps are primarily these two:

1. Bump maps are much more compact that normal maps and lead to smaller glTF files as well as less GPU memory requirements.  This is because the normal map has to store a full 3D vector per texel with floating point precision, while the bump map only needs to store a single scalar value per texel.  Thus bump maps require 3 times less data than normal maps for transmission and GPU storage.  Normal maps are also very sensitive to texture compressions methods, while bump maps are less sensitive.

2. Bump maps are useful for capturing small details in combination with a normal map for large scale structures.  For example, when rendering human faces, there are usually two layers of surface pertubation detail, the unwrapped normal map is used for representing wrinkles, and then a repeating bump map is used for skin pore details.  This is also the case for many other materials such as wood, metal, and stone, where there is both macro and micro details.

Normal maps are more resistant to artifacts when undergoing going magnification as compared to bump maps, thus they are better suited for large scale smooth detail.  This limitation of bump maps is not significant though when bump maps are being used for detail textures where the magnification is small and they align roughly with the same texel resolution as the color map.

Additionally, maybe WebGL rendering engines, including both ThreeJS and Sketchfab, already support simultaneous bump maps and normal maps in their default materials.  Without this extension, these engines can not save or load this information via glTF files.

## Extending Materials

The `KHR_materials_bump` extension can be specified in the `extensions` property of the material:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_bump": {
                    "bumpFactor": 0.5
                }
            }
        }
    ]
}
```

Factor and texture are combined by multiplication to describe a single value.

| |Type|Description|Required|
|-|----|-----------|--------|
| **bumpFactor** | `number` | The strength of the specular reflection. | No, default: `1.0`|
| **bumpTexture** | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | A texture that defines the strength of the specular reflection, stored in the red (`R`) channel. This will be multiplied by bumpFactor. | No |

The `bump` parameters affect the normal used in `dielectric_brdf` of the glTF 2.0 metallic-roughness material.

## Implementation

*This section is non-normative.*

The `bump` parameter is used to perturb the normal direction. The normal direction is perturbed by the value of the bump map at the texel corresponding to the fragment. The perturbed normal is then used in the `dielectric_brdf` function of the glTF 2.0 metallic-roughness material.

### Blending Normal and Bump Perturbations

In order to properly blend the bump map with a normal map, please follow the recommendation of https://blog.selfshadow.com/publications/blending-in-detail/.

```glsl
vec3 t = originalNormal.xyz * vec3( 2,  2, 2 ) + vec3( -1, -1, 0 );
vec3 u = bumpNormal.xyz * vec3( -2, -2, 2 ) + vec3( 1,  1, -1 );
vec3 r = normalize( t * dot(t, u) / t.z - u ); // not required if t and u are pre-normalized
vec3 blendedNormal = r * 0.5 + 0.5;
```

## Schema

- [material.EXT_materials_bump.schema.json](schema/material.EXT_materials_bump.schema.json)

## References

[Blinn, J.F., "Simulation of Wrinkled Surfaces" (1978)](https://www.microsoft.com/en-us/research/wp-content/uploads/1978/01/p286-blinn.pdf)

[Cohen, J. et al. "Appearance-Preserving Simplification" (1989)](http://www.cs.unc.edu/~geom/APS/APS.pdf)

[Waveren, J.M.P. et al. "Real-Time Normal Map DXT Compression" (2008)](https://developer.download.nvidia.com/whitepapers/2008/real-time-normal-map-dxt-compression.pdf)

## Appendix: Full Khronos Copyright Statement

Copyright 2023 The Khronos Group Inc.

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
