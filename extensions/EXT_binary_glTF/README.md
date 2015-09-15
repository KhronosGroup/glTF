# EXT_binary_glTF

## Contributors

* Patrick Cozzi, Cesium, [@pjcozzi](https://twitter.com/pjcozzi)
* Tom Fili, Cesium, [@CesiumFili](https://twitter.com/CesiumFili)
* Kai Ninomiya, Cesium, [@kainino0x](https://twitter.com/kainino0x)
* Max Limper, Fraunhofer IGD, [@mlimper_cg](https://twitter.com/mlimper_cg)
* Maik Thöner, Fraunhofer IGD, [@mthoener](https://twitter.com/mthoener)

## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

glTF provides two delivery options that can be also be used together:

* glTF JSON points to external binary data (geometry, key frames, skins), images, and shaders.
* glTF JSON embeds base64-encoded binary data, images, and shaders inline using data URIs.

For these data, glTF requires either separate requests or extra space due to base64-encoding.  Base64-encoding requires extra processing to decode and increases the file size (by ~33% for encoded resources).  While gzip mitigates the file size increase, decompression and decoding still add significant loading time.

To solve this, this extension introduces a container format, _Binary glTF_.
In Binary glTF, glTF resources (JSON data, .bin, images, and shaders) can be stored in a binary blob accessed in JavaScript as an `ArrayBuffer`. 
This binary blob (which can be a file, for example) is always divided into three different, subsequent parts:

* A 20-byte preamble, entitled the `header`
* The structured glTF scene description (also denoted as `scene`)
* The binary `body`

The scene part can then refer to external resources (as usual), but also to resources stored within the binary file body.
Informally, this is like embedding the JSON, images, and shaders in an .bin file.

_TODO: this is browser implementation stuff
In a browser, the `TextDecoder` JavaScript API can be used to extract the glTF scene from the arraybuffer. 
A scene encoded as JSON can be parsed with `JSON.parse` as usual, and then the arraybuffer is treated as a glTF `buffer`.

## Binary glTF Layout

Binary glTF is little endian. Figure 1 shows an overview of the three parts of a Binary glTF file.

**Figure 1**: Binary glTF layout.

![](figures/layout.png)

The following sections show the structure of the three different parts more in detail.

### Header

The 20-byte header always consists of the following five 4-byte entries:

* `magic` is the ASCII string `'glTF'`, and can be used to identify the arraybuffer as Binary glTF.

* `version` is an `uint32` that indicates the version of the Binary glTF container format, which is `1` for this version of the extension. Examples of currently available versions are shown in Table 1.
_TODO: should there be a more sophisticated way of specifying the version, such as major-minor-maintenance, or major-minor? Maybe major-minor as two 16 bit values?

* `length` is the total length of the Binary glTF, including header, scene and body, in bytes.

* `sceneLength` is the length, in bytes, of the structured glTF `scene`.

* `sceneFormat` specifies the format of the structured glTF `scene`. A list of all valid  values currently available is provided within Table 2.

**Table 1**: Example values for 'version'

| Decimal | Hex        | Short Description | Comment                    |
|--------:|-----------:|------------------:|---------------------------:|
| 1       | 0x00000001 | Version 1         |                            |


**Table 2**: Valid values for 'sceneFormat'

| Decimal | Hex        | Short Description | Comment                    |
|--------:|-----------:|------------------:|---------------------------:|
| 0       | 0x00000000 | JSON              | JavaScript Object Notation |


### Scene

The `scene` part holds the structured glTF scene description, as it would be provided within a .gltf file in a non-binary version of glTF.
By reading the `scene` first, an implementation is able to progressively retrieve resources from the binary body.
This way, it is also possible to read only a selected subset of resources from a binary glTF file (for instance, the coarsest LOD of a mesh).

By referring to a special buffer entitled `binary_glTF`, elements of the scene can refer to binary data within the body.
For more details, see section [glTF Schema Updates](#gltf-schema-updates).

Binary glTF still supports external resources.
For example, an application that wants to download textures on demand may embed everything except images in the Binary glTF.
Embedded base64-encoded resources are also still supported, but it would be inefficient to use them.
An advantage of Binary glTF over glTF is that resources can be embedded without paying the size and client-side decoding costs of base64-encoding.

### Body

The binary body carries the actual payload of the Binary glTF file.
Strings inside this binary body, i.e., JSON or shaders, are encoded using UTF-8.
The special buffer entitled `binary_glTF` can be used to address the content of the binary body.
An offset of zero, for example, means that the start of the binary body is addressed.



## glTF Schema Updates

This extension introduces an explicitly named `buffer` called `binary_glTF`.
This buffer is an implicit reference to the binary body of the Binary glTF file.
It only has one property, `"type": "arraybuffer"`. 
When a runtime encounters this, it should use the Binary glTF body as the buffer.
`bufferViews` that reference this `buffer` work as usual.

To support embedded shaders and images, `shader` and `image` glTF properties have new `binary_glTF` extension properties that should be used insted of the `uri` property.
See Listings 2 and 3.

**Listing 2**: A `shader` referencing a `bufferview` to access an embedded shader source.
```javascript
"extensionsUsed" : [
    "EXT_binary_glTF"
]
...
"a_shader" : {
    "extensions" : {
        "EXT_binary_glTF" : {
            "bufferview" : ...
        }
    }
}
```

**Listing 3**: An `image` referencing a `bufferview` and with metadata useful for loading the image from the arrayBuffer.  In JavaScript, `Blob` can be used as the source for an `Image` to extract an image from the arraybuffer (for example, see Cesium's [`loadImageFromTypedArray`](https://github.com/AnalyticalGraphicsInc/cesium/blob/1.13/Source/Core/loadImageFromTypedArray.js) helper function).
```javascript
"extensionsUsed" : [
    "EXT_binary_glTF"
]
...
"an_image" : {
    "extensions" : {
        "EXT_binary_glTF" : {
            "bufferview" : ...,
            "mimeType" : "image/png",
            "height" : 256,
            "width" : 512
        }
    }
}
```

## Schema

TODO

## File Extension

`.bgltf` or `.bgl`or `.bgt`

## MIME Type

Use `model/vnd.gltf.binary`.

## Known Implementations

### Runtime

* Cesium ([code](https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Scene/Model.js))

### Tools

* Cesium COLLADA-to-glTF Converter ([app](http://cesiumjs.org/convertmodel.html))
* colladaToBglTFConverter ([code](https://github.com/virtualcitySYSTEMS/colladaToBglTFConverter))
* SRC writer source code ([code](http://x3dom.org/src/files/src_writer_source.zip))

## Resources

* Discussion - [#357](https://github.com/KhronosGroup/glTF/issues/357) and [#400](https://github.com/KhronosGroup/glTF/pull/400)
* base64-encoded data in glTF - [#68](https://github.com/KhronosGroup/glTF/issues/68)
* [Faster 3D Models with Binary glTF](http://cesiumjs.org/2015/06/01/Binary-glTF/) article on CESIUM_binary_glTF the Cesium blog
* SRC project page (paper, background, basic writer) - [http://x3dom.org/src/](http://x3dom.org/src/)

## Performance Results

_This section is non-normative._

Based on experimentation (below & [[1]](#BenchData)) using CESIUM_binary_glTF and Cesium's glTF loader (only trivial differences to EXT_binary_glTF), different configurations are recommended for different scenarios.

* To minimize file size and number of files, use Binary glTF (gzipped), and external compressed image files (PNG, JPEG, etc.) to avoid significant decompression overhead.
* If a single file is desired, use Binary glTF (gzipped) with all files embedded.
* For files with very little non-texture data, the difference in loading time is minimal, but Binary glTF can reduce the number of requests without overhead.

The following observations are made from file size and benchmark data:

* JSON, mesh data, and animation data are highly compressible.
* Already-compressed textures (PNG, JPEG) are not very compressible.  Adding compression (e.g. by embedding them into a Binary glTF file which will be gzipped) adds significant CPU overhead with little size benefit.

Using the Cesium [aircraft model](https://github.com/AnalyticalGraphicsInc/cesium/tree/master/Apps/SampleData/models/CesiumAir), which contains 5,984 triangles with two texture atlases and a simple animation without skinning, statistics and results for the common glTF setups are:

| Cesium Air                            | # files |     size | size (gzip\*) |  load time |
| :------------------------------------ | ------: | -------: | ------------: | ---------: |
| COLLADA                               |     3   |  922 KiB |     591 KiB   |            |
| glTF                                  |     8   |  608 KiB |     538 KiB   |   0.32 s   |
| glTF, base64-encoded bin/jpg/png/glsl |   **1** |  808 KiB |     540 KiB   |   0.41 s   |
| Binary glTF, embedded textures        |   **1** |  609 KiB |   **513 KiB** | **0.29 s** |
| Binary glTF, separate textures        |     3   |  609 KiB |     538 KiB   | **0.30 s** |

![](BenchData/thumb/Cesium_Air.jpg)

Using the 1200 12th Ave model (thanks to [Cube Cities](http://cubecities.com/)), which contians 30,235 triangles with 21 textures and no animations, statistics and results for the common glTF setups are:

| 1200 12th Ave                         | # files |     size | size (gzip\*) |  load time | 
| :------------------------------------ | ------: | -------: | ------------: | ---------: | 
| COLLADA                               |    22   | 5.93 MiB |    1.36 MiB   |            |
| glTF                                  |    31   | 2.99 MiB |    1.25 MiB   | **0.85 s** |
| glTF, base64-encoded bin/jpg/png/glsl |   **1** | 3.64 MiB |    1.30 MiB   |   1.25 s   |
| Binary glTF, embedded textures        |   **1** | 2.99 MiB |  **1.23 MiB** |   1.03 s   |
| Binary glTF, separate textures        |    22   | 2.99 MiB |    1.25 MiB   | **0.84 s** |

![](BenchData/thumb/1200_12th.jpg)

\* All files gzipped except for stand-alone images.

<a name="BenchData">
* [1] Raw data for benchmarks using compression available in [BenchData](BenchData/README.md) supplemental.
