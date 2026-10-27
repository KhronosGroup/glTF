# KHR\_behavior

NOTE: Currently we are merging this proposal with the Adobe behavior extension.

## Contributors

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Moritz Becher, UX3D [@mobecher](https://twitter.com/mobecher)
* Ben Houston, Threekit
* Jan Hermes, Continental
* Dwight Rogers, Adobe
* Emmet Lalish, Google
* Ken Xu, NVIDIA

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
The following value types are known to the behavior extension:

* integer (TODO: Specify precision, i32 or i64, and what happens at overflow)
* boolean
* float/scalar (TODO: Specify precision, fp32 or fp64, and what happens at overflow)
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

Automatic casting may not take place for types that differ in their dimensionalities, such as `vec2`, `vec3` and `vec4`. 

## Behavior Graph Nodes

Nodes are JSON objects with properties `type`, `parameters`, `flow` and `name`. Based on the `type` the node must have a specific set of properties in `parameters` and `flow`. 

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

### Parameter Input Sockets

`parameters` is an object containing the value inputs of the node. 

Each node defines which parameters it expects, specified by a *name* and a [type](#types).

Parameters can be passed a [JSON value](#json-value), a [reference to another node's output socket](#output-socket-references) or a [reference to a variable](#variable-references). 

All values have to be *ready* at the time of execution of the node, i.e. if the parameter is a reference to another node's output socket, this node has to already have been executed.

Automatic type conversions according to the rules in [Automatic casting](#automatic-casting) take place when connecting compatible types to the parameter as output socket reference or variable reference. Automatic casting does not apply to JSON values, these must be of the same type as the node's parameter.

#### JSON Value

Constant JSON values can directly be passed to parameters. It is invalid to pass a value that doesn't correspond to the type of the parameter, e.g passing a `1.0` number literal to a boolean parameter is not allowed.

```json
{
    "condition": true
}
```

#### [Output Socket References](schema/behavior.types.reference.outputSocket.schema.json)

Each node type also implicitly defines a set of output sockets, where each output is referenced with a string key. For example, the "condition" parameter of the `logic/branch` node above could be connected to the output value of a previous node like in the following example.

```json
{
    "condition": {
        "$node": 0,
        "socket": "result"
    }
}
```

If the referenced node only provides one output socket, it is allowed to omit the "socket" property in the output socket reference.

#### [Variable References](schema/behavior.types.reference.variable.schema.json)

Variables that are defined in the behavior can be referenced in a parameter with the *Variable Reference* object literal. 
- The Reference is an index into the `variables` array of the behavior
- Only variables defined in the same behavior can be referenced

```json
{
    "condition": {
        "$variable": 0
    }
}

```

### Flow Output Sockets

Nodes may define a `flow` property containing references to other nodes in the behavior's *nodes* array that should succeed the current node's execution in certain conditions. 

If the node doesn't define a `flow` property or if its value is an empty object literal, the behavior **terminates** at the node.

The node can define which of the paths in the `flow` property are followed during the execution of the behavior e.g. based on the evaluation of an *input parameter*.

### Value Output Sockets

Nodes define a set of output values that become available after the execution of the node. Subsequent nodes can therefore reference these output values via the node's so-called *output sockets* (see [Output Socket References](#output-socket-references)).

The output sockets are defined in the node schema with a type and a socket name.

### Event Nodes

Event nodes serve a special purpose, as they can initiate the execution of a behavior.

#### Lifecycle

* start/load : Triggered on the start of execution for the object
* update/tick : Triggered on a per-frame update for the object, may not fire on every frame if there isn't sufficient resources (not available in level 0)
* on<*Name*>Event: generic event specified by a name. The implementation is responsible for determining when the event should fire. This can for example be used to react to user input

#### Mouse/Touch

* hover : Start and end events for hoving on a node, compatible with at least VR and mouse interactions
* select : The equivalent of tap, click for mouse, touch or VR.

#### Camera

* proximity - In and Out In/Out
* view frustum - In/Out


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

* combine2 : Combine 2 separate input values into a vec2  
* combine3 : Combine 3 separate input values into a vec3
* combine4 : Combine 4 separate input values into a vec4
* extract2 : Extracts vec2 into 2 separate output values  
* extract3 : Extracts vec3 into 3 separate output values  
* extract4 : Extracts vec4 into 4 separate output values  

### Flow Nodes

Flow nodes can be used to define a more complex control flow inside the node graph. 

* branch : Branch the control flow based on a condition
* gate : allow for multiple trigger events to be combined.
* delay : after the trigger, it will fire the output after the user specified delay.  it will not respond to any other triggers during that delay.   it is not a queue.
* debounce : a delay combined with a gate, such that if there is a duration delay between the trigger and the cancel, it will fire the output
* flipflop : each time it is triggered it will fire either its first or second output subsequently
* multi-gate : each time there is a new trigger, it will fire a subsequent output, it can optionally loop and it can be reset to start against
* do-once : it will trigger its output the first time it is triggered, but subsequent triggers will do nothing until it is reset

### Query Nodes

* set : Set a value of glTF object properties or of one of the behavior's variables, relies upon the KHR_animation_pointer extension, with the caveat below for additional paths.

### Action Nodes

* get : Get a value from glTF object properties or from one of the behavior's variables, relies upon the KHR_animation_pointer extension, with the caveat below for additional paths.
* animation play/cancel : start, stop animations
* sound play/cancel : start, stop sounds, relies upon the KHR_sound extension
* interpolate to: takes the current value and target value along with delta time, and easing function and executes that in the background, relies upon the KHR_animation_pointer extension.
* setVariable: Set the value of a variable

TODO: Determine how we can manipulate node visibility, currently this isn't supported in glTF.  There is the extension KHR_nodes_disable.


### KHR_Animation_Pointer caveats

Much of the queries and action capabilities of this extension are mediated through the KHR_Animation_Pointer extension.
* For layer 0, we will not allow for queries using KHR_animation_pointer.
* For layer 1, we will support all capoabilities of KHT_animation_pointer for both queries and actions.
* We will support a namespace extension to the KHR_animation_pointer for querying and setting the viewport camera.  The format for this is "/viewer/camera/" The supported camera properties are are all of those on the camera.schema.json along with the properties on either orthpgraphic or perspective schema.json dependent on the camera type.  We also allow you to query "cameraIndex" which if this is a camera specified by the original glTF file, it will be a non-negative index to that camera, otherwise it will be a negative number.  We also support accessing the current location and orientation of the camera via the path /globalTransform/{rotation|scale|position}."

* For disabling and enabling nodes and node trees, we will rely upon the KHT_nodes_disable extension.

### Examples

#### Events

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
                                "next": 1
                            }
                        },
                    ]
                }
            ]
        }
    }
}
```

#### Setting a nodes' translation


```json
{
    "nodes": [
        {
            "name": "Event triggered each frame",
            "type": "event/onUpdate",
            "flow": {
                "next": 1
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
```

#### Defining variables

Behaviors can contain variables like the following, e.g. to store state of the behavior or to parameterize the behavior

```json
{
    "nodes": [
        ...
    ],
    "variables": [
        {
            "name": "Offset",
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
                "next": 1
            }
        },
        {
            "name": "Getting the translation of the second node",
            "type": "action/get",
            "parameters": {
                "source": "/nodes/1/translation"
            },
            "flow": {
                "next": 2
            }
        },
        {
            "name": "Setting the translation of the first node",
            "type": "action/set",
            "parameters": {
                "target": "/nodes/0/translation",
                "value": { "$operation": 1 }
            }
        }
    ]
}
```

#### Conditional flow


```json
{
    "nodes": [
        {
            "name": "Event triggered each frame",
            "type": "event/onUpdate",
            "flow": {
                "next": 1
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
                "next": 2
            }
        },
        {
            "name": "Basic if condition",
            "type": "flow/branch",
            "parameters": {
                "condition": { "$operation": 1 }
            },
            "flow": {
                "true": 3,
                "false": 4
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
