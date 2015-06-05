# CESIUM_binary_glTF

## Contributors

* Patrick Cozzi, [@pjcozzi](https://twitter.com/pjcozzi)
* Tom Fili, [@CesiumFili](https://twitter.com/CesiumFili)
* Kai Ninomiya, [@kainino0x](https://twitter.com/kainino0x)

## Status

Experimental

## Dependencies

Written against the glTF 0.8 spec.

## Overview

glTF provides two delivery options that can be also be used together:
* glTF JSON points to external binary data (geometry, key frames, skins), images, and shaders.
* glTF JSON has base64-encoded binary data, images, and shaders using data uris.

Base64-encoding increases the file size and requires extra decoding.  glTF is commonly criticized for requiring separate requests or extra space due to base64-encoding.

To solve this, this extension introduces a container format, _Binary glTF_.  In Binary glTF, glTF resources (JSON, .bin, images, and shaders) are stored in a binary blob accessed in JavaScript as an `ArrayBuffer`.  The `TextDecoder` JavaScript API can be used to extract the glTF JSON from the arraybuffer.  The JSON can be parses with `JSON.parse` as usual, and then the arraybuffer is treated as a glTF `buffer`. Informally, this is like embedding the JSON, images, and shaders in the .bin file.

## Binary glTF Layout

Binary glTF is little endian.  It has a 20-byte header followed by the glTF resources, including the JSON:

**Figure 1**: Binary glTF layout.

![](layout.png)

`magic` is the ANSI string `'glTF'`, and can be used to identify the arraybuffer as Binary glTF.  `version` is an `uint32` that indicates the version of the Binary glTF container format, which is currently `1`.  `length` is the total length of the Binary glTF, including the header, in bytes.  `jsonOffset` is the offset, in bytes, from the start of the arraybuffer to the start of the glTF JSON.  `jsonLength` is the length of the glTF JSON in bytes.

`jsonOffset` and `jsonLength` are used to access the JSON.  This extension does not define where the JSON is in the arraybuffer nor does it define where the JSON is stored relative to the embedded data.  Figure 1 illustrates that the embedded data may come before or after the JSON (or both).  Pragmatically, exporter implementations will find it easier to write the JSON after the embedded data to make computing `byteOffset` and `byteLength` for bufferviews straightforward.

The start of the embedded data is 4-byte aligned to ease its use with JavaScript Typed Arrays.

Given an `arrayBuffer` with Binary glTF, Listing 1 shows how to parse the header and access the JSON.

_TODO: the code below is out-of-sync with Cesium_

**Listing 1**: Parsing Binary glTF.  This uses a `TextDecoder` wrapper in Cesium, [`getStringFromTypedArray`](https://github.com/AnalyticalGraphicsInc/cesium/blob/bgltf/Source/Core/getStringFromTypedArray.js).
```javascript
var sizeOfUint32 = Uint32Array.BYTES_PER_ELEMENT;

var magic = getStringFromTypedArray(arrayBuffer, 0, 4);
if (magic !== 'glTF') {
    // Not Binary glTF
}

var view = new DataView(arrayBuffer);
var byteOffset = sizeOfUint32;  // Skip magic number

var version = view.getUint32(byteOffset, true);
byteOffset += sizeOfUint32;
if (version !== 1) {
    // This only handles version 1.
}

byteOffset += sizeOfUint32;  // Skip length

var jsonOffset = view.getUint32(byteOffset, true);
byteOffset += sizeOfUint32;

var jsonLength = view.getUint32(byteOffset, true);
byteOffset += sizeOfUint32;

var jsonString = getStringFromTypedArray(arrayBuffer, jsonOffset, jsonLength);
var json = JSON.parse(jsonString)
```

Strings in the Binary glTF, i.e., JSON or shaders, are UTF-8.

Binary glTF still supports external resources.  For example, an application that wants to download textures on demand may embed everything except images in the Binary glTF.

Binary glTF also supported embedded base64-encoded resources, but it would be inefficient to use them.  An advantage of Binary glTF over glTF is that resources can be embedded without paying the size and client-side decoding costs of base64-encoding.

## glTF Schema Updates

This extension introduces an explicitly named `buffer` called `CESIUM_binary_glTF`.  This buffer is an implicit reference to the arraybuffer that is the Binary glTF.  It only has one property, `"type": "arraybuffer"`.  When a runtime encounters this, it should use the already loaded Binary glTF arrayBuffer as the buffer.  `bufferViews` that reference this `buffer` work as usual.

To support embedded shaders and images, `shader` and `image` glTF properties have new `CESIUM_binary_glTF` extension properties and no longer require the `uri` property.  See Listings 2 and 3.

**Listing 2**: A `shader` referencing a `bufferview` to access an embedded shader source.
```json
"a_shader" : {
    "extensions" : {
        "CESIUM_binary_glTF" : {
            "bufferview" : ...
        }
    }
}
```

**Listing 3**: An `image` referencing a `bufferview` and with metadata useful for loading the image from the arrayBuffer.  In JavaScript, `Blob` can be used as the source for an `Image` to extract an image from the arraybuffer.  See Cesium's [`loadImageFromTypedArray`](https://github.com/AnalyticalGraphicsInc/cesium/blob/bgltf/Source/Core/loadImageFromTypedArray.js) helper function.
```json
"an_image" : {
    "extensions" : {
        "CESIUM_binary_glTF" : {
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

## MIME Type

Use `model/vnd.gltf.binary`.

## Experimental Results

Using the Cesium [aircraft model](https://github.com/AnalyticalGraphicsInc/cesium/tree/master/Apps/SampleData/models/CesiumAir), which contains 5,984 triangles, two texture atlases, a simple animation, and no skins, sizes for the common glTF setups are:

            | dae               | glTF              | glTF (base64-encoded bin/png/glsl) | Binary glTF
------------|-------------------|-------------------|------------------------------------|------------
size        | 1.07 MB (3 files) | 802 KB (8 files)  | 1.03 MB                            |  806 KB
size (gzip) | 728 KB            | 677 KB            | 706 KB                             |  707 KB

TODO: another model and decode speeds

## Known Implementations

### Runtime

* Cesium ([code](https://github.com/AnalyticalGraphicsInc/cesium/blob/bgltf/Source/Scene/Model.js))

### Tools

* Cesium COLLADA-to-glTF Converter ([app](http://cesiumjs.org/convertmodel.html))
* colladaToBglTFConverter ([code](https://github.com/virtualcitySYSTEMS/colladaToBglTFConverter))

## Resources

* Discussion - [#357](https://github.com/KhronosGroup/glTF/issues/357)
* base64-encoded data in glTF - [#68](https://github.com/KhronosGroup/glTF/issues/68)
