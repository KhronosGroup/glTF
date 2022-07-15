# KHR\_behavior

## Contributors

* Ben Houston, Threekit
* Jan Hermes, Continental
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

Copyright 2018-2022 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is adding logic to glTF for enabling behavior.

### Motivation

glTF is one of the four technology enablers for the metaverse (see "Value creation in the metaverse").  
A common feature request from metaverse companies is adding logic and behavior to glTF. Furthermore, this request also comes from the automotive, e-commerce and games industry.  
  
Goal is to have a glTF extension which is providing bevavior to glTF with following requirements:  

* 100% portable with the major game engines from day one
* Easy to implement on major platforms
* Safe execution

From these requirements, the main features can be deducted:

### Main features

#### Portability

* Use the intersection set of Visual Scripting features from Unity and the Unreal Engine

#### Implementation

* Minimum meaningful and extensible feature set

#### Safety

* No loops
* No recursions
* No access outside glTF
* No spawning of glTF objects

## New glTF object

`scriptNode`  
A script node is a node both available in the Visual Scripting system from Unity and the Unreal Engine. To not conflict with the `node` in glTF, these nodes are called `scriptNode`.

### JSON Schemas

[schema](schema/)

### Examples

#### Group `event`

```json
{
    "extensions": {
        "KHR_behavior": {
            "scriptNodes": [
                {
                    "name": "Event is triggered at start",
                    "group": "event",
                    "event": {
                        "type": "OnStart",
                        "linkedScriptNodes": [ 
                            0
                        ]
                    }
                },
                {
                    "name": "Event is triggered before every frame update",
                    "group": "event",
                    "event": {
                        "type": "OnUpdate",
                        "linkedScriptNodes": [ 
                            1
                        ]
                    }
                },
                {
                    "name": "Event is triggered after the value is changed",
                    "group": "event",
                    "event": {
                        "type": "OnValueChanged",
                        "linkedScriptNodes": [
                            2
                        ],
                        "OnValueChanged": {
                            "pointer": "/nodes/0/translation"
                        }
                    }
                },
                {
                    "name": "Event is triggered by the engine/viewer on demand",
                    "group": "event",
                    "event": {
                        "type": "OnDemand",
                        "linkedScriptNodes": [
                            3
                        ]
                    }
                },
                {
                    "name": "Event is triggered after user interacted within a bounding sphere",
                    "group": "event",
                    "event": {
                        "type": "OnInteraction",
                        "linkedScriptNodes": [
                            4
                        ],
                        "OnInteraction": {
                            "node": 0,
                            "boundingSphere": 10.0
                        }
                },
                {
                    "name": "Event is triggered after user interacted within a bounding box",
                    "group": "event",
                    "event": {
                        "type": "OnInteraction",
                        "linkedScriptNodes": [
                            5
                        ],
                        "OnInteraction": {
                            "node": 1,
                            "boundingBox": [
                                1.0,
                                2.0,
                                1.0
                            ]
                        }
                    }
                }
            ]
        }
    }
}
```

#### Group `get`

```json
{
    "extensions": {
        "KHR_behavior": {
            "scriptNodes": [
                {
                    "name": "Setting the translation of a node",
                    "group": "get",
                    "get": {
                        "pointer" : "/nodes/1/translation"
                    }
                }
            ]
        }
    }
}
```

#### Group `branch`

```json
{
    "extensions": {
        "KHR_behavior": {
            "scriptNodes": [
                {
                    "name": "Setting the translation of a node",
                    "group": "branch",
                    "branch": {
                        "condition" : "if",
                        "if": {
                            "ToDo":"ToDo"
                        }
                    }
                }
            ]
        }
    }
}
```

#### Group `set`

```json
{
    "extensions": {
        "KHR_behavior": {
            "scriptNodes": [
                {
                    "name": "Setting the translation of a node",
                    "group": "set",
                    "set" : {
                        "pointer": "/nodes/1/translation"
                    }
                }
            ]
        }
    }
}
```

## References

* [McKinsey & Company, Value creation in the metaverse, June 2022, page 49](https://www.mckinsey.com/business-functions/growth-marketing-and-sales/our-insights/value-creation-in-the-metaverse)
* [Unity Visual Scripting](https://docs.unity3d.com/Packages/com.unity.visualscripting@1.7/manual/index.html)
* [Unreal Engine Blueprint Visual Scripting](https://docs.unrealengine.com/5.0/en-US/blueprints-visual-scripting-in-unreal-engine/)

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2022 The Khronos Group Inc.

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
