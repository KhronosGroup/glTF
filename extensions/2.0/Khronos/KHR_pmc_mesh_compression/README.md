# KHR_pmc_mesh_compression

## Contributors

* Igor Vytyaz, Google, <vytyaz@google.com>
* Ondrej Stava, Google, <ostava@google.com>
* Roshan Baliga, Google, <rdb@google.com>
* Frank Galligan, Google, <fgalligan@google.com>

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides the ability to compress mesh primitives using a 3D mesh compression standard called Polygonal Mesh Coding (PMC) developed by the Alliance for Open Media's (AOM) Volumetric Visual Media (VVM) Working Group. PMC compression is designed for efficient transmission and storage of 3D meshes. The `KHR_pmc_mesh_compression` extension enables faster transmission and reduced storage requirements of glTF assets.

## Enabling Mesh Compression

To enable PMC mesh compression for a glTF primitive, add `KHR_pmc_mesh_compression` to the `extensions` object of the `primitives` array item in a `mesh` as shown in the following example:

```json
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
                "KHR_pmc_mesh_compression" : {
                    "bufferView" : 5
                }
            }
        }
    ]
}
```

The compressed mesh data (the bitstream) is stored in a glTF buffer. The extension refers to a `bufferView` at index 5 that describes the location of the mesh data in the buffer. The bitstream is encoded and decoded using an implementation of the PMC compression standard. The `bufferView` is the only extension property, and it is a required extension property (see [extension schema](schema/mesh.primitive.KHR_pmc_mesh_compression.schema.json)).

When using this extension, the `mode` of the primitive must be 4 (`TRIANGLES`).

The use of the extension must be declared by adding the extension name to the `extensionsUsed` array:

```json
"extensionsUsed" : [
    "KHR_pmc_mesh_compression"
]
```

The uncompressed data may be provided along with the compressed version. Loader implementations that do not yet support PMC compression can fall back to using the uncompressed data. If the uncompressed data is not provided in one or more primitives, then the extension name must be added to the `extensionsRequired` array.

```json
"extensionsRequired" : [
    "KHR_pmc_mesh_compression"
]
```

When the uncompressed data is not provided in a primitive, then the `attributes` and `indices` accessors of the primitive must not provide the `bufferView` and `byteOffset` properties, because the loaders must ignore those properties and use the compressed data instead. Other accessor properties like `type`, `count`, etc. should be provided in a manner conforming with the base glTF specification.

The PMC encoder can change the order of faces and vertices for better compression. To avoid any data inconsistencies, the loader must obtain the indices and all attributes either from compressed data only or from the fallback uncompressed data only. For example, decoding `POSITION` from PMC bitstream while reading `NORMAL` from the fallback uncompressed data is not allowed and will lead to data inconsistency.

The PMC bitstream must contain encoded `indices` and all `attributes` specified in the parent primitive. When compressing the attributes, the encoder must specify attribute names in PMC (see `ad_attribute_name_present` in PMC specification), matching the keys in the `attributes` JSON object, such as `POSITIONS`, `JOINTS_0`, etc. Loaders will then identify the decoded attributes by corresponding names.

## Schema

* [mesh.primitive.KHR_pmc_mesh_compression.schema.json](schema/mesh.primitive.KHR_pmc_mesh_compression.schema.json)

## Conformance

The recommended process for loaders that do not support this extension:

* Fail loading the asset if this extension is in `extensionsRequired`.
* Load the asset ignoring this extension if the extension is not in `extensionsRequired`.

The required process for loaders that support this extension:

* Get the bitstream from this extension's `bufferView`, decompress the `indices` and `attributes` data from the bitstream using PMC decoder, and get desired attribute data by attribute names like `NORMAL` specified in the bitstream.
* Process `attributes` and `indices` accessors of the primitive while ignoring accessor `bufferView` and `byteOffset` and use the previously decompressed data directly.

## Implementation note

*This section is non-normative.*

It is recommended to store compressed data and uncompressed fallback data in separate buffers. This allows loaders to request only necessary buffers and prevent transmission of redundant data.

## Known Implementations

* TODO: List of known implementations, with links to each if available.

## Resources

* [PMC reference software](https://gitlab.com/AOMediaVVM/reference-software/aomedia-pmc)
* TODO: Link to PMC standard specification.