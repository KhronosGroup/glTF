# KHR\_animation\_pointer

## Contributors

- Alexey Knyazev
- Ben Houston, Threekit
- Bryce Hutchings, Microsoft [@brycehutchings](https://twitter.com/brycehutchings)
- Don McCurdy
- Ed Mackey, AGI [@emackey](https://github.com/emackey)
- Felix Herbst, prefrontal cortex [@hybridherbst](https://twitter.com/hybridherbst)
- Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
- Jamie Marconi, Microsoft [@najadojo](https://twitter.com/najadojo)
- Joe Herdman, [@jbherdman](https://github.com/jbherdman)
- Lewis Weaver, Microsoft
- Marco Hutter
- Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

Copyright 2024 The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Complete, Ratified by the Khronos Group

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides a standardized way of animating arbitrary glTF properties according to the [glTF 2.0 Asset Object Model](../../../../specification/2.0/ObjectModel.adoc).

### Motivation

The base glTF 2.0 specification supports animating only node transformation or morph target weights. With this extension, an animation can target any mutable property in a glTF asset, such as material factors or camera field of view.

The **glTF 2.0 Asset Object Model** defines JSON pointers and data types of the mutable glTF properties.

### Operation

Each animation target specified by the JSON Pointer is resolved to the corresponding glTF asset property and the value of the latter is dynamically updated using values provided by the animation channel output accessor. The property being animated **MUST** be mutable as defined by the **glTF 2.0 Asset Object Model**.

The JSON Pointer **MUST** point to a property defined in the asset. A property is considered defined if it is present in the asset explicitly or if it has a default value and its enclosing object is present.

In the following example, both the `/materials/0/pbrMetallicRoughness/baseColorFactor` and `/materials/0/pbrMetallicRoughness/metallicFactor` are valid pointers: the base color factor is defined explicitly while the metallic factor has a spec-defined default value and the `pbrMetallicRoughness` object is defined.

```json
{
    "asset": {
        "version": "2.0"
    },
    "materials": [
        {
            "pbrMetallicRoughness": {
                "baseColorFactor": [ 1.0, 0.0, 0.0, 1.0 ],
            }
        }
    ]
}
```

Pointers to the asset properties that do not have a spec-defined default value, such as `/cameras/0/perspective/zfar`, are invalid if the property is not defined in the asset explicitly.

The same property **MUST NOT** be targeted more than once in one animation, i.e., different channels of the same animation **MUST NOT**:

- have identical pointers;
- target an array property and individual elements of the same array;
- target the same node property with a pointer and with an unextended animation target object.

The output accessor **MUST** be compatible with the animated property data type (see the table below) and the values provided by it **MUST** be valid for the property being animated.

| Asset Object Model Data Type | Output Accessor Type |
|------------|---|
| `bool`     | SCALAR |
| `float`    | SCALAR |
| `float[]`  | SCALAR |
| `float2`   | VEC2 |
| `float3`   | VEC3 |
| `float4`   | VEC4 |
| `float2x2` | MAT2 |
| `float3x3` | MAT3 |
| `float4x4` | MAT4 |
| `int`      | SCALAR |

> [!NOTE]
> There are currently no mutable properties of matrix types in the ratified glTF 2.0 specification or extensions.

#### Output Accessor Component Types

If the Object Model Data Type is one of the `float*` types, the output accessor values are converted based on the accessor's component type as follows:

- float accessor values are used as-is;
- non-normalized integer accessor values are converted to the equal floating-point values, e.g., `1` to `1.0`;
- normalized integer accessor values are converted using the equations from the [Animations section of the glTF 2.0 specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#animations).

Keyframe interpolation **MUST** happen after type conversions, using floating-point values.

If the Object Model Data Type is `int`, the output accessor component type **MUST** be one of the non-normalized integer types and its values are used as-is.

If the Object Model Data Type is `bool`, the output accessor component type **MUST** be _unsigned byte_; `0` is converted to `false`, any other value is converted to `true`.

Animation samplers used with `int` or `bool` Object Model Data Types **MUST** use `STEP` interpolation.

#### `extras`

Properties located in `extras` objects **MAY** be targeted as well, but validity and interpretation of the animated values is entirely application specific.

## Extension Usage

To use this extension, the animation channel target path **MUST** be set to `"pointer"` and the actual JSON pointer value is provided in the extension object's `pointer` property. The animation channel `node` property **MUST NOT** be set.

> [!NOTE]
> Implementations not supporting this extension will ignore animation channels with undefined `node` properties according to the base glTF 2.0 specification.

## glTF Schema Updates

- **JSON schema**: [animation.channel.target.KHR_animation_pointer.schema.json](schema/animation.channel.target.KHR_animation_pointer.schema.json)

## Examples

### Node Rotation

The following two snippets represent the same animation expressed with an unextended animation target and with the animation pointer extension.

```json
"animations": [
    {
        "channels": [
            {
                "sampler": 0,
                "target": {
                    "node": 0,
                    "path": "rotation"
                }
            }
        ],
        "samplers": [
            {
                "input": 0,
                "interpolation": "LINEAR",
                "output": 1
            }
        ]
    }
]
```

```json
"animations": [
    {
        "channels": [
            {
                "sampler": 0,
                "target": {
                    "path": "pointer",
                    "extensions": {
                        "KHR_animation_pointer": {
                            "pointer": "/nodes/0/rotation"
                        }
                    }
                }
            }
        ],
        "samplers": [
            {
                "input": 0,
                "interpolation": "LINEAR",
                "output": 1
            }
        ]
    }
]
```

### Base Color

The following animation uses the animation pointer extension to target a base color factor.

```json
"animations": [
    {
        "channels": [
            {
                "sampler": 0,
                "target": {
                    "path": "pointer",
                    "extensions": {
                        "KHR_animation_pointer": {
                            "pointer": "/materials/0/pbrMetallicRoughness/baseColorFactor"
                        }
                    }
                }
            }
        ],
        "samplers": [
            {
                "input": 0,
                "interpolation": "LINEAR",
                "output": 1
            }
        ]
    }
]
```

## References

- [glTF 2.0 Asset Object Model](../../../../specification/2.0/ObjectModel.adoc)

- [JavaScript Object Notation (JSON) Pointer, RFC 6901](https://www.rfc-editor.org/info/rfc6901)

## Appendix: Full Khronos Copyright Statement

Copyright 2024 The Khronos Group Inc.

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
