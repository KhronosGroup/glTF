# Draco_animation_compression

## Contributors

* Fan Zhang, Google, <mailto:zhafang@google.com>
* Michael Hemmer, <mailto:hemmer@google.com>
* Ondrej Stava, Google, <mailto:ostava@google.com>
* Frank Galligan, Google, <mailto:fgalligan@google.com>

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a schema to use [Draco compression library (non-normative)](https://github.com/google/draco) to compress `animations` in glTF format. This allows glTF to support streaming compressed animation data instead of the raw data.

The [conformance](#conformance) section specifies what an implementation must do when encountering this extension, and how the extension interacts with the attributes defined in the base specification.

## glTF Schema Updates

If a Draco compressed version of one or more `animations` is provided then `Draco_animation_compression` must be added to `extensionsUsed`.

```javascript
"extensionsUsed" : [
    "Draco_animation_compression"
]
```

If the uncompressed version of one or more `animations` are not provided, then `Draco_animation_compression` must be added to `extensionsRequired`.
```javascript
"extensionsRequired" : [
    "Draco_animation_compression"
]
```

If the root object of a glTF asset contains `extensions` property and the `extensions` property defines its `Draco_animation_compression` property, then the Draco animation compression must be used. If there are any compressed animations then the `Draco_animation_compression` must define the `compressedAnimations` property. The `compressedAnimations` property consists of an array of compressed animation objects. Each element in `compressedAnimations` must have the following properties:

#### input
The `input` property defines a single `input` of a compressed animation. The
`input` corresponds to a `samplers` element's `input`.

### outputs
The `outputs` property defines an array of `output` of sharing the same `input`.
Each element of `outputs` corresponds to a `samplers` element's `output`.

### attributeId
The `attributeId` defines the unique id of each element in `outputs`. The unique id is used to locate the correspondent data in the compressed data.

#### bufferView
The `bufferView` contains the compressed data.


Below is an example of what a part of a glTF file my look like if the Draco extension is set:

```javascript
{
    "extensionsRequired" : [
        "Draco_animation_compression"
    ],
    "extensionsUsed" : [
        "Draco_animation_compression"
    ],
    "extensions" : {
        "Draco_animation_compression" : {
            "compressedAnimations" : [
                {
                    "input" : 0,
                    "outputs" : [ 1, 2, 3 ],
                    "attributeId" : [ 1, 2, 3 ],
                    "bufferView" : 0
                },
                {
                    "input" : 4,
                    "outputs" : [ 5 ],
                    "attributeId" : [ 1 ],
                    "bufferView" : 1
                }
            ],
        }
    },
    "nodes" : [
      ...
    ],
    "animations" : [
        {
            "channels" : [
                {
                    "sampler" : 0,
                    "target" : {
                        "node" : 0,
                        "path" : "translation"
                    }
                },
                {
                    "sampler" : 1,
                    "target" : {
                        "node" : 1,
                        "path" : "rotation"
                    }
                },
            ],
            "samplers" : [
                {
                    "input" : 0
                    "interpolation" : "LINEAR",
                    "output" : 1
                },
                {
                    "input" : 0
                    "interpolation" : "LINEAR",
                    "output" : 2
                }
            ]
        },
        {
            "channels" : [
                {
                    "sampler" : 0,
                    "target" : {
                        "node" : 2,
                        "path" : "rotation"
                    }
                },
                {
                    "sampler" : 1,
                    "target" : {
                        "node" : 3,
                        "path" : "translation"
                    }
                },
            ],
            "samplers" : [
                {
                    "input" : 0
                    "interpolation" : "LINEAR",
                    "output" : 3
                },
                {
                    "input" : 4
                    "interpolation" : "LINEAR",
                    "output" : 5
                }
            ]
        },
    ],
    "accessors" : [
        // Accessors of input/output of animations that use the compression extension will omit "bufferView" property.
        {
            "componentType" : 5123,
            "count" : 100,
            "type" : "SCALAR"
        },
        {
            "componentType" : 5123,
            "count" : 100,
            "type" : "VEC3"
        },
        {
            "componentType" : 5123,
            "count" : 100,
            "type" : "VEC4"
        },
        {
            "componentType" : 5123,
            "count" : 100,
            "type" : "VEC4"
        },
        {
            "componentType" : 5123,
            "count" : 100,
            "type" : "SCALAR"
        },
        {
            "componentType" : 5123,
            "count" : 100,
            "type" : "VEC3"
        },
        ...
    ],
    "bufferViews" : [
        // Contains data for the first compressed animation.
        {
            "buffer" : 10,
            "byteOffset" : 0,
            "byteLength" : 10000
        },
        // Contains data for the second compressed animation.
        {
            "buffer" : 10,
            "byteOffset" : 10000,
            "byteLength" : 6000
        },
        ...
    ]
}


```

### JSON Schema

For full details on the `Draco_animation_compression` extension properties, see the schema:

* [extension property (the entirety of this specification is normative and included in Scope)](schema/node.Draco_animation_compression.schema.json)

## Conformance

## Implementation note

*This section is non-normative.*

## Resources

*This section is non-normative.*

* [Draco Open Source Library](https://github.com/google/draco)
* [ThreeJS
  Loader](https://github.com/mrdoob/three.js/blob/dev/examples/js/loaders/draco/DRACOLoader.js)
  and
  [example](https://github.com/mrdoob/three.js/blob/dev/examples/webgl_loader_draco.html)
