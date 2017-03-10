# KHR_mesh_compression

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

This extension defines an schema to use mesh compression libraries, e.g. Google Draco Project, in glTF format. This allows glTF to support streaming compressed mesh data instead of the raw data.

The [conformance](#conformance) section specifies what an implementation must to do when encountering this extension, and how the extension interacts with the attributes defined in the base specification.

## Primitive Extension

A compression library could be used for `primitive` by adding an `extension` property to a primitive, and defining its `KHR_mesh_compression` property.

Usage of the extension must be listed in the `extensionUsed` and `extensionsRequired`. 

```javascript
"extensionsUsed" : [
    "KHR_mesh_compression"
]

"extensionsRequired" : [
    "KHR_mesh_compression"
]

```

The extension then could used like:

```javascript

"mesh" : {
    "primitives" : [
        {
            "attributes" : {
                "POSITION" : 11,
                "NORMAL" : 12,
                "TEXCOORD_0" : 13,
            },
            "indices" : 10,
            "mode" : 4
            "targets" : [
                {
                    "NORMAL" : 14,
                    "POSITION" : 15,
                },
                {
                    "NORMAL" : 16,
                    "POSITION" : 17,
                }
            ],
            "extensions" : {
                "KHR_mesh_compression" : {
                    "bufferView" : 

                }
            }
        },
    ]
}

```
To note that the extension currently don't support morph targets.
In glTF 2.0, `bufferView` is not required in `accessor`. But if it is not present or the id is 0, it will be used with `sparse` field to act as an sparse accessor. 
The correspondent accessors will be modified:

```javascript
{
    "accessors" : [
        // ...
        // Skip first 10 accessors that's not used in the example.
        // ...

        // This is the accessor referred by id 10
        {
            "bufferView" : 
            "componentType" :
            "count" :
            "type" :
            "max" :
            "min" :
        },
        {
        },

```

## glTF Schema Updates

As mentioned above, Binary glTF introduces a `buffer` with id equal to `"binary_glTF"`.
This buffer is an implicit reference to the binary `body` of the Binary glTF asset. 
Its `type` property is `"arraybuffer"`, and a runtime can ignore the `uri` property since the buffer refers to the Binary glTF `body` section, not an external resource.
When a runtime encounters this buffer, it should use the Binary glTF `body` as the buffer.
`bufferViews` that reference this `buffer` work as usual.

To support embedded shaders and images, `shader` and `image` glTF properties have new `KHR_binary_glTF` extension properties that should be used instead of the `uri` property.
See Listings 2 and 3.

**Listing 2**: A `shader` referencing a `bufferView` to access an embedded shader source.
```javascript
// ...
"a_shader" : {
    "extensions" : {
        "KHR_binary_glTF" : {
            "bufferView" : // ...
        }
    }
}
```

### JSON Schema

For full details on the `KHR_binary_glTF` extension properties, see the schema:

* [image](schema/image.KHR_binary_glTF.schema.json) `KHR_binary_glTF` extensions object
* [shader](schema/shader.KHR_binary_glTF.schema.json) `KHR_binary_glTF` extensions object

## Format of Decompressed Mesh

## Conformance

To process this extension, there are some changes need to be made in loading
glTF.
* First, need to decompressed the data frome a `buffer` pointed by the `bufferView` of extension
  and store the data as a new buffer
* Then, when handling `accessor` of attributes, insteading of getting data from its
  own `bufferView --> buffer`, we get the data from the `bufferView (in
  extension) --> 

It is pretty straigtforward for top-down loading of glTF file, e.g. only
decompress the mesh data when a `primitive` is met the first time. However, for
bottom-up loading, e.g. loading `accessor` before `primitive`, we need to
clarify some details.


