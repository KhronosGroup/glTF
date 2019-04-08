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

* [extension property (the entirety of this specification is normative and included in Scope)](schema/node.DRACO_animation_compression.schema.json)

## Conformance

Below is the recommended process when a loader encounters a glTF asset with the Draco animation extension set:

* If `Draco_animation_compression` is in `extensionsRequired` and the loader does not support the Draco animation extension, then the loader must fail loading the asset.
* If the loader does not support the Draco animation extension and `Draco_animation_compression` is not in `extensionsRequired`, then load the glTF asset ignoring `Draco_animation_compression` in `extensions`.
* If the loader does support the Draco animation extension, but will not process `Draco_animation_compression`, then the loader must load the glTF asset ignoring `Draco_animation_compression` in `extensions`.
* If the loader does support the Draco animation extension, and will process `Draco_animation_compression` then:
    * The loader must process `Draco_animation_compression` first. The loader must get the data from `Draco_animation_compression`'s `bufferView` property and decompress the data using a Draco animation decoder to a Draco animation.
    * Then the loader must process `samplers` properties of the `animations`. When loading each element of `samplers`, you must ignore the `bufferView` and go to the previously decoded Draco animation from `compressedAnimations` to get the data of `input` and `outputs`. A loader must use the decompressed animation data to fill the `accessors` (e.g. [ThreeJS (non-normative)](https://github.com/FrankGalligan/three.js/blob/add_draco_animation_compression/examples/js/loaders/DRACOLoader.js)).

## Implementation note

*This section is non-normative.*

To prevent transmission of redundant data, exporters should generally write compressed Draco animation data into a separate buffer from the uncompressed fallback, and shared data into a third buffer. Loaders may then optimize to request only the necessary buffers.

## Resources

*This section is non-normative.*

* [Draco Open Source Library](https://github.com/google/draco)
* [Creation
  gltf-pipeline](https://github.com/FrankGalligan/gltf-pipeline/tree/add_draco_animation_compression)
* [ThreeJS
  Loader](https://github.com/FrankGalligan/three.js/blob/add_draco_animation_compression/examples/js/loaders/DRACOLoader.js)
