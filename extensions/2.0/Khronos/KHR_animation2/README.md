# KHR\_animation2

## Contributors

* Alexey Knyazev
* Ben Houston, Threekit
* Bryce Hutchings, Microsoft [@brycehutchings](https://twitter.com/brycehutchings)
* Don McCurdy
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Jamie Marconi, Microsoft [@najadojo](https://twitter.com/najadojo)
* Joe Herdman, [@jbherdman](https://github.com/jbherdman)
* Lewis Weaver, Microsoft
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Felix Herbst, prefrontal cortex [@hybridherbst](https://twitter.com/hybridherbst)

Copyright (C) 2018-2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is based on the animation features of glTF 2.0. The structure of the schemas stay the same.  
  
The only major addition is, that the output values are mapped using a [JSON Pointer](https://datatracker.ietf.org/doc/html/rfc6901).
  
### Motivation

At this point of time, one can only target the transformation or weight data of a node.  
With this extension, one can technically target any value (`scalar`, `vec2`, `vec3`, `vec4` and scalars in an `array`) in glTF e.g.

* Color factors in materials
* Camera field of view

Even using a JSON pointer, the targets and their expected behavior are explictly defined.

Current and future extensions have to write against this specification to allow to animate specific values.

#### Current and future restrictions

The calculated values can be out of range of the minimum and maximum value, however the values have to be clamped using the properties min and max value before setting the value.

##### Not animatable properties

`glTFid`

It is not allowed to animate a glTFid property, as it does change the structure of the glTF in general.

### Valid target templates

|`path`                                                |Accessor Type|Component Type(s)  |Description                                                   |
|------------------------------------------------------|-------------|-------------------|--------------------------------------------------------------|
|`"/nodes/{}/matrix"`                                  |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Matrix elements                                               |
|`"/nodes/{}/rotation"`                                |`"VEC4"`     |`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XYZW rotation quaternion|
|`"/nodes/{}/scale"`                                   |`"VEC3"`     |`5126`&nbsp;(FLOAT)|XYZ scale vector                                              |
|`"/nodes/{}/translation"`                             |`"VEC3"`     |`5126`&nbsp;(FLOAT)|XYZ translation vector                                        |
|`"/nodes/{}/weights"`                                 |`"SCALAR"`   |`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|Morph target weights                                          |
|`"/cameras/{}/orthographic/xmag"`                     |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Horizontal magnification of the view                          |
|`"/cameras/{}/orthographic/ymag"`                     |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Vertical magnification of the view                            |
|`"/cameras/{}/orthographic/zfar"`                     |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Distance to the far clipping plane                            |
|`"/cameras/{}/orthographic/znear"`                    |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Distance to the near clipping plane                           |
|`"/cameras/{}/perspective/aspectRatio"`               |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Aspect ratio of the field of view                             |
|`"/cameras/{}/perspective/yfov"`                      |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Vertical field of view in radians                             |
|`"/cameras/{}/perspective/zfar"`                      |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Distance to the far clipping plane                            |
|`"/cameras/{}/perspective/znear"`                     |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Distance to the near clipping plane                           |
|`"/materials/{}/pbrMetallicRoughness/baseColorFactor"`|`"VEC4"`     |`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|The material's base color factor|
|`"/materials/{}/pbrMetallicRoughness/metallicFactor"` |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|The metalness of the material                                 |
|`"/materials/{}/pbrMetallicRoughness/roughnessFactor"`|`"SCALAR"`   |`5126`&nbsp;(FLOAT)|The roughness of the material                                 |
|`"/materials/{}/alphaCutoff"`                         |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|The alpha cutoff value of the material                        |
|`"/materials/{}/emissiveFactor"`                      |`"VEC3"`     |`5126`&nbsp;(FLOAT)|The emissive color of the material                            |
|`"/materials/{}/normalTexture/scale"`                 |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Multiplier applied to each normal vector of the normal texture|
|`"/materials/{}/occlusionTexture/strength"`           |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|Multiplier controlling the amount of occlusion applied        |

##### `extras`

A channel may target `extras` at any allowed JSON Pointer value but
interpretation of the animated values is entirely application specific.

## Extension compatibility and fallback behavior

If this extension is used, the `animation.channel.target.node` **must not** be set.
Because the node isn’t defined, the channel is ignored and not animated due to the current specification.  
If this extension is used, the `animation.channel.target.path` **must** contain any of constant values. Even this value is ignored, the unused glTF part is still valid.

### Animating properties of extensions

Extensions created after KHR_animation2 should state which properties can be animated and which cannot.  
The following extensions have been created before KHR_animation2 and are thus listed here with their animatable properties:  

#### [KHR_draco_mesh_compression](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_draco_mesh_compression)
❌ No animatable properties.

#### [KHR_lights_punctual](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_lights_punctual)
| Property | Animatable |
|:-----|:------|
| `name` | ❌ |
| `color` | ✅ |
| `intensity` | ✅ |
| `type` | ❌ |
| `range` | ✅ |
| `spot.innerConeAngle` | ✅ |
| `spot.outerConeAngle` | ✅ |

#### [KHR_materials_clearcoat](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_clearcoat)
| Property | Animatable |
|:----|:-------|
| `clearcoatFactor` | ✅ |
| `clearcoatTexture` | ❌ |
| `clearcoatRoughnessFactor` | ✅ |
| `clearcoatRoughnessTexture` | ❌ |
| `clearcoatNormalTexture` | ❌ |

#### [KHR_materials_ior](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_ior)
| Property | Animatable |
|:-----|:------------|
| `ior` | ✅ |

#### [KHR_materials_sheen](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_sheen)
| Property | Animatable |
|:-----|:------------|
| `sheenColorFactor` | ✅ |
| `sheenColorTexture` | ❌ |
| `sheenRoughnessFactor` | ✅ |
| `sheenRoughnessTexture` | ❌ |

#### [KHR_materials_specular](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_specular)
| Property | Animatable |
|:--------|:----------|
| `specularFactor` | ✅ |
| `specularTexture` | ❌ |
| `specularColorFactor` | ✅ |
| `specularColorTexture` | ❌ |

#### [KHR_materials_transmission](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_transmission)
| Property | Animatable |
|:--------|:----------|
| `transmissionFactor` | ✅ |
| `transmissionTexture` | ❌ |

#### [KHR_materials_unlit](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_unlit)
❌ No animatable properties.

#### [KHR_materials_variants](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_variants)
❌ No animatable properties.

#### [KHR_materials_volume](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_volume)
| Property | Animatable |
|:--------|:----------|
| `thicknessFactor` | ✅ |
| `thicknessTexture` | ❌ |
| `attenuationDistance` | ✅ |
| `attenuationColor` | ✅ |

#### [KHR_mesh_quantization](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_mesh_quantization)
❌ No animatable properties.

#### [KHR_texture_basisu](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_basisu)
❌ No animatable properties.

#### [KHR_texture_transform](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_transform)
| Property | Animatable |
|:--------|:----------|
| `offset` | ✅ |
| `rotation` | ✅ |
| `scale` | ✅ |
| `texCoord` | ❌ |

#### [KHR_xmp_json_ld](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_xmp_json_ld)
❌ No animatable properties.

### Example target templates for extensions

|`path`                                                |Accessor Type|Component Type(s)  |Description                                                   |
|------------------------------------------------------|-------------|-------------------|--------------------------------------------------------------|
|`"/materials/{}/pbrMetallicRoughness/baseColorTexture/extensions/KHR_texture_transform/offset"`                                  |`"SCALAR"`   |`5126`&nbsp;(FLOAT)|XY offset vector                                               |

## glTF Schema Updates

* **JSON schema**: [glTF.KHR_animation2.schema.json](schema/glTF.KHR_animation2.schema.json)

## Notes and clarifications

### JSON Pointer

For a `scalar` value, the JSON Pointer targets the glTF property.  
This means, the property is replaced by the interpolated value.
  
For any other case, the JSON Pointer targets the glTF property as well but must be an `array` property.  
This means, that the two or more elements do replace the values in the array.  

## Examples

The following snippet shows the changes for [`animations`](https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/AnimatedCube/glTF/AnimatedCube.gltf#L117) from the [AnimatedCube](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube) asset.

```javascript
"animations" : [
    {
        "channels" : [
            {
                "name" : "Targeting x, y, z, w for a rotation of node at index 0."
                "sampler" : 0,
                "target" : {
                    "path" : "rotation"
                    "extensions": {
                        "KHR_animation2" : {
                            "path" : "/nodes/0/rotation"
                        }
                    }
               }
            }
        ],
        "samplers" : [
            {
                "input" : 0,
                "interpolation" : "LINEAR",
                "output" : 1
            }
        ]
    }
]
```

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2021 The Khronos Group Inc.

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
