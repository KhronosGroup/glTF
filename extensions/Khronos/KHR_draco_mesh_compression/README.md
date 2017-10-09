# KHR_draco_mesh_compression

## Contributors

* Fan Zhang, Google, <mailto:zhafang@google.com>
* Ondrej Stava, Google, <mailto:ostava@google.com>
* Frank Galligan, Google, <mailto:fgalligan@google.com>
* Kai Ninomiya, Google, <mailto:kainino@google.com>
* Patrick Cozzi, Cesium, [@pjcozzi](https://twitter.com/pjcozzi)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines a schema to use [Draco geometry compression](https://github.com/google/draco) libraries in glTF format. This allows glTF to support streaming compressed geometry data instead of the raw data. This extension specification is based on [Draco bitestream version 2.0](https://google.github.io/draco/spec/).

The [conformance](#conformance) section specifies what an implementation must do when encountering this extension, and how the extension interacts with the attributes defined in the base specification.

## glTF Schema Updates

Draco geometry compression library could be used for `primitive` by adding an `extension` property to a primitive, and defining its `KHR_draco_mesh_compression` property.

The following picture shows the structure of the schema update. 

**Figure 1**: Structure of geometry compression extension.
![](figures/structure.png)

In general, we will use the extension to point to the buffer that contains the compressed data. One of the requirements is that a loader/engine should be able to load the glTF assets no matter it supports the extension or not. To achieve that, all the existing components of the glTF specification stays the same when the extension exists, so that a loader doesn't support decoding compressed assets could just ignore the extension.

Usage of the extension must be listed in the `extensionUsed`. 

```javascript
"extensionsUsed" : [
    "KHR_draco_mesh_compression"
]

```

The extension then could be used like the following, note that all other nodes stay the same
except `primitives`:

```javascript

"mesh" : {
    "primitives" : [
        {
            "attributes" : {
                "POSITION" : 11,
                "NORMAL" : 12,
                "TEXCOORD_0" : 13,
                "WEIGHTS_0" : 14,
                "JOINTS_0" : 15,
            },
            "indices" : 10,
            "mode" : 4
            "extensions" : {
                "KHR_draco_mesh_compression" : {
                    "bufferView" : 5,
                    "attributes" : {
                        "POSITION" : 0,
                        "NORMAL" : 1,
                        "TEXCOORD_0" : 2,
                        "WEIGHTS_0" : 3,
                        "JOINTS_0" : 4
                    },
                }
            }
        },
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
We will explain each of the property in the following sections.
#### bufferView
The `bufferView` property points to the buffer containing compressed data. The data should be passed to a mesh decoder and decompressed to a
mesh.

### attributes
`attributes` defines the attributes stored in the decompressed geometry. E.g, in the example above, `POSITION`, `NORMAL`, `TEXCOORD_0`, `WEIGHTS_0` and `JOINTS_0`. Each attribute is associated with an attribute id which is its unique id in the compressed data. The `attributes` defined in the extension must be a subset of the attributes of the primitive. To request an attribute, loaders should be able to use the correspondent attribute id specified in the `attributes` to get the attribute from the compressed data.

#### Restrictions on geometry type
When using this extension, the `mode` of `primitive` could only be one of `TRIANGLES` and `TRIANGLE_STRIP` and the mesh data will be decoded accordingly.

### JSON Schema

For full details on the `KHR_draco_mesh_compression` extension properties, see the schema:

* [extension property](schema/node.KHR_draco_mesh_compression.schema.json) `KHR_draco_mesh_compression` extensions object.

## Conformance

To process this extension, there are some changes need to be made in loading a glTF asset.
* If `KHR_draco_mesh_compression` is in `extensionsRequired` then the loader must support the extension or it will fail loading the assets.
* If `KHR_draco_mesh_compression` is in `extensionsUsed` but not `extensionsRequired`:
    * Check if the loader supports the extension. If not, then load the glTF asset ignoring the compression extension in `primitive`.
    * If the loader supports the extension, then process the extension and ignore the attributes and indices of the primitive that are contained in the extension.
* When encountering a `primitive` with the extension the first time, you must process the extension first. Get the data from the pointed `bufferView` in the extension and decompress the data to a geometry of a specific format, e.g. Draco geometry.
* Then, process `attributes` and `indices` properties of the `primitive`. When loading each `accessor`, go to the previously decoded geometry in the `primitive` to get indices and attributes data. A loader could use the decompressed data to overwrite `accessors` or render the decompressed geometry directly (e.g. ThreeJS).

It is pretty straightforward for top-down loading of a glTF asset, e.g. only
decompress the geometry data when a `primitive` is met for the first time. However, for
bottom-up loading, loading `accessor` before `primitive` will not get the data. It could only be handled when processing its parent `primitive`. This is based on the consideration that it will rarely happen that
loading an `accessor` without knowing its parent `primitive`. And it should be
easy enough to change the loader to ignore `accessor` without `bufferView` in glTF 2.0. But we are
definitely open to change this if there actually are some use cases that require
loading `accessor` independently. 

The extension provides compressed alternatives to one or more of a primitive's uncompressed attributes. A loader may choose to use the uncompressed attributes instead — when the extension is not supported, or for other reasons. When using compressed Draco data, the corresponding uncompressed attributes defined by the primitive should be ignored. If additional uncompressed attributes are defined by the primitive, but are not provided by the Draco extension, the loader must proceed to use these additional attributes as usual.

* Implementation note: To prevent transmission of redundant data, exporters should generally write compressed Draco data into a separate buffer from the uncompressed fallback, and shared data into a third buffer. Loaders may then optimize to request only the necessary buffers.

If the uncompressed version of asset is not provided for the fallback described above, then the extension must be added to `extensionsRequired` so that the loaders/engines don't support the extension could report failure.

## Resources

* [Draco Open Source Library](https://github.com/google/draco)
* [ThreeJS
  Loader](https://github.com/mrdoob/three.js/blob/dev/examples/js/loaders/DRACOLoader.js)
  and
  [example](https://github.com/mrdoob/three.js/blob/dev/examples/webgl_loader_draco.html)

