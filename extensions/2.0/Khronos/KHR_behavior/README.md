# KHR\_behavior

## Contributors

* Ben Houston, Threekit
* Jan Hermes, Continental
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Moritz Becher, UX3D [@mobecher](https://twitter.com/mobecher)

Copyright 2018-2022 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Early Draft

ToDo:
- Condition can reference another condition instead of a variable for having a complex condition.
- Math node e.g. `+` to combine a variable result.

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
* One input execution flow socket

#### Safety

* No loops
* No recursions
* No access outside glTF
* No spawning of glTF objects

## New glTF object

`behaviorNode`  
A behavior node is a node both available in the Visual Scripting system from Unity and the Unreal Engine. To not conflict with the `node` in glTF, these nodes are called `behaviorNode`.

### JSON Schemas

[schema](schema/)

### Types
Only the following types are allowed to read and write to.

* integer
* boolean
* float (scalar)
* vec2
* vec3
* vec4

### Automatic casting
For simplicity, behavior nodes can be connected, even if they do have different input and output types. Following list provides the rules to cast a type using C/c++ style notation:

|From boolean b|to integer i  |
|--------------|--------------|
|b = true      |i = 1         |
|b = false     |i = 0         |

|From integer i|to boolean b  |
|--------------|--------------|
|i != 0	       |b = true      |
|i == 0        |b = false     |

|From integer i|to float f    |
|--------------|--------------|
|i	           |f = (float)i  |

|From float f  |to integer i  |
|--------------|--------------|
|f	           |i = (int)f    |

#### Dimensionality

Automatic casting may not take place for types that differ in dimensionality, such as `vec2`, `vec3` and `vec4`. 

## Behavior Graph Nodes

Nodes are json objects with properties `type`, `parameters`, `flow` and `name`. Based on the `type` the node must have a specific set of properties in `parameters` and `flow`. 

```json
{
    "name": "Some Node",
    "type": "logic/branch",
    "parameters": {
        "condition": true
    },
    "flow": {
        "true": 1,
        "false": 2
    }
}
```
*Example of a `logic/branch` node with a condition parameter and two flow outputs*

The parameters and flow outputs corresponding to a specific node type can be found in the schema. 

### Flow

Nodes may define a `flow` property containing references to other nodes in the behavior's `nodes` array that should follow the current node in certain conditions. 

If the node doesn't define a `flow` property or if it's value is an empty object literal, the behavior **terminates** at the node.

The node can define which of the paths in the `flow` property are followed during execution of the behavior based on some rules defined in the node's specification. 

### Parameters

Parameters can be passed a [Type](#types) compatible json value, a reference to another node's output socket or a reference to a variable.

In addition to specifying the required parameter names, nodes also define the type of the parameter. Automatic type conversions according to the rules in [Automatic casting](#automatic-casting) take place when connecting compatible types to the parameter as output socket reference or variable reference. Automatic casting does not apply to json values, these must be of the same type as the node's parameter.

#### Json Value

Constant json values can directly be passed to parameters. Passing a value that doesn't correspond to the parameters type is invalid, e.g passing a `1.0` number literal to a boolean parameter is not allowed.

```json
{
    "condition": true
}
```

#### Output Socket References

Each node type also implicitly defines a set of output sockets, where each output is referenced with a string key. For example the "condition" parameter of the `logic/branch` node above could be connected to the output value of a previous node like in the following example.

```json
{
    "condition": {
        "$node": 0,
        "socket": "result"
    }
}
```

#### Variable References

Variables that are defined in the behavior can be referenced in a parameter with the *Variable Reference* object literal 

```json
{
    "condition": {
        "$variable": 0
    }
}

```


### Math Nodes
The elements and the wording are inspired by MaterialX (see "MaterialX Specification"):  
  
* add : add two values  
* subtract : subtract two values  
* multiply : multiply two values  
* divide : divide two values  
* modulo : the remaining fraction after dividing a value by another value  
* absval : the per-channel absolute value  
* floor : the per-channel nearest integer value less than or equal to the incoming value  
* ceil : the per-channel nearest integer value greater than or equal to the incoming value  
* round : round each channel of the incoming values to the nearest integer value  
* power : raise incoming values to the specified exponent  
* sin : the sine of the incoming value, which is expected to be expressed in radians  
* cos : the cosine of the incoming value, which is expected to be expressed in radians  
* tan : the tangent of the incoming value, which is expected to be expressed in radians  
* asin : the arcsine of the incoming value; the output will be expressed in radians  
* acos : the arccosine of the incoming value; the output will be expressed in radians  
* atan2 : the arctangent of the expression (iny/inx); the output will be expressed in radians  
* sqrt : the square root of the incoming value  
* ln : the natural log of the incoming value  
* exp : "e" to the power of the incoming value  
* clamp : clamp incoming values  
* min : select the minimum of the two incoming values  
* max : select the maximum of the two incoming values  
* normalize : output the normalized vector N from the incoming vector N  
* magnitude : output the float magnitude (vector length) of the incoming vector N  
* dotproduct : output the (float) dot product of two incoming vector N  
* crossproduct : output the (vector3) cross product of two incoming vector3  
* rotate2d : rotate a vector2 value about the origin in 2D  
* rotate3d : rotate a vector3 value about a specified unit axis vector  
* less : Compare two numbers and return a boolean with the result of the comparison

### Channel Nodes
The elements and the wording are inspired by MaterialX (see "MaterialX Specification"):  

* combine2 : Combine 2 separate input values into one array  
* combine3 : Combine 3 separate input values into one array  
* combine4 : Combine 4 separate input values into one array  
* extract2 : Extracts array into 2 separate output values  
* extract3 : Extracts array into 3 separate output values  
* extract4 : Extracts array into 4 separate output values  

### Flow Nodes

Flow nodes can be used to define a more complex control flow inside the node graph. 

* branch : Branch the control flow based on a condition

### Action Nodes

* set : Set a value of glTF object properties or of one of the behavior's variables
* get : Get a value from glTF object properties or from one of the behavior's variables



### Examples

#### Setting a nodes' translation


```json
{
    "extensions": {
        "KHR_behavior": {
            "behaviors": [
                {
                    "nodes": [
                        {
                            "name": "Event triggered each frame",
                            "type": "event/onUpdate",
                            "flow": {
                                "next": 0
                            }
                        },
                        {
                            "name": "Setting the translation of the first node",
                            "type": "action/set",
                            "parameters": {
                                "target": "/nodes/0/translation",
                                "value": [1.0, 2.0, 3.0]
                            },
                        },
                    ]
                }
            ]

        }
    }
}
```

The same example can also be implemented with a constant variable

```json
{
    "nodes": [
        {
            "name": "Event triggered each frame",
            "type": "event/onUpdate",
            "flow": {
                "next": 0
            }
        },
        {
            "name": "Setting the translation of the first node",
            "type": "action/set",
            "parameters": {
                "target": "/nodes/0/translation",
                "value": { "$variable": 0 }
            },
        },
    ],
    "variables": [
        {
            "name": "Constant values",
            "type": "vec3",
            "initialValue": [
                1.0,
                2.0,
                3.0
            ]
        }
    ]
}
```

#### Getting the translation value from a node

Translation from the second node is written each frame to the translation of the first node:
```json
{
    "nodes": [
        {
            "name": "Event triggered each frame",
            "type": "event/onUpdate",
            "flow": {
                "next": 0
            }
        },
        {
            "name": "Getting the translation of the second node",
            "type": "action/get",
            "parameters": {
                "source": "/nodes/1/translation"
            },
            "flow": {
                "next": 1
            }
        },
        {
            "name": "Setting the translation of the first node",
            "type": "action/set",
            "parameters": {
                "target": "/nodes/0/translation",
                "value": { "$operation": 0 }
            }
        }
    ]
}
```

#### Conditional flow

Translation is written each frame to the first or second node depending on a condition:
```json
{
    "nodes": [
        {
            "name": "Event triggered each frame",
            "type": "event/onUpdate",
            "flow": {
                "next": 0
            }
        },
        {
            "name": "Comparing two values",
            "type": "math/less",
            "parameters": {
                "first": 1,
                "second": 2
            },
            "flow": {
                "next": 1
            }
        },
        {
            "name": "Basic if condition",
            "type": "flow/branch",
            "parameters": {
                "condition": { "$operation": 1 }
            },
            "flow": {
                "true": 2,
                "false": 3
            }
        },
        {
            "name": "Setting the translation of the first node from the true condition case",
            "type": "action/set",
            "parameters": {
                "target": "/nodes/0/translation",
                "value": [1.0, 1.0, 1.0 ]
            }
        },
        {
            "name": "Setting the translation of the first node from the false condition case",
            "type": "action/set",
            "parameters": {
                "target": "/nodes/0/translation",
                "value": [0.0, 0.0, 0.0 ]
            }
        }
    ]
}
```


## References

* [MaterialX Specification](https://www.materialx.org/Specification.html)
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
