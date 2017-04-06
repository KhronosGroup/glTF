# Appendix A: GLB File Format Specification

*Version 2.0*

## Contents

* [Overview](#overview)
  * [File Extension](#file-extension)
  * [MIME Type](#mime-type)
* [Binary glTF Layout](#binary-gltf-layout)
  * [Header](#header)
  * [Chunks](#chunks)
    * [Structured JSON Content](#structured-json-content)
    * [Binary Buffer](#binary-buffer)

## Overview

glTF provides two delivery options that can also be used together:

* glTF JSON points to external binary data (geometry, key frames, skins), and images.
* glTF JSON embeds base64-encoded binary data, and images inline using data URIs.

For these resources, glTF requires either separate requests or extra space due to base64-encoding. Base64-encoding requires extra processing to decode and increases the file size (by ~33% for encoded resources). While gzip mitigates the file size increase, decompression and decoding still add significant loading time.

To solve this, a container format, _Binary glTF_ is introduced. In Binary glTF, a glTF asset (JSON, .bin, and images) can be stored in a binary blob. 

This binary blob (which can be a file, for example) has the following structure:
* A 12-byte preamble, entitled the `header`.
* One or more `chunks` that contains JSON content and binary data.

The `chunk` containing JSON can refer to external resources as usual, and can also reference resources stored within other `chunks`.

For example, an application that wants to download textures on demand may embed everything except images in the Binary glTF. Embedded base64-encoded resources are also still supported, but it would be inefficient to use them.

### File Extension

The file extension to be used with Binary glTF is `.glb`.

### MIME Type

Use `model/gltf.binary`.

## Binary glTF Layout

Binary glTF is little endian. Figure 1 shows an example of a Binary glTF asset.

**Figure 1**: Binary glTF layout.

![](figures/glb2.png)

The following sections describe the structure more in detail.

### Header

The 12-byte header consists of three 4-byte entries:

```
uint32 magic
uint32 version
uint32 length
```

* `magic` equals `0x46546C67`. It is ASCII string `glTF`, and can be used to identify data as Binary glTF.

* `version` indicates the version of the Binary glTF container format. This specification defines version 2.

* `length` is the total length of the Binary glTF, including Header and all Chunks, in bytes.

> **Implementation Note:** GLB loaders should also check for the [asset version properties](readme.md#asset) in the JSON chunk, as the version specified in the GLB header only refers to the GLB container version.

### Chunks

Each chunk has the following structure:
```
uint32 chunkLength
uint32 chunkType
ubyte[] chunkData
```

* `chunkLength` is the length of `chunkData`, in bytes.

* `chunkType` indicates the type of chunk. See Table 1 for details.

* `chunkData` is a binary payload of chunk.

The start and the end of each chunk must be aligned to 4-byte boundary. See chunks definitions for padding schemes. Chunks must appear in exactly the order given in the Table 1.

**Table 1**: Chunk types

|  | Chunk Type | ASCII | Description | Occurrences |
|----|------------|-------|-------------------------|-------------|
| 1. | 0x4E4F534A | JSON | Structured JSON content | 1 |
| 2. | 0x004E4942 | BIN | Binary buffer | 0 or 1 |

 Clients must ignore chunks with unknown types.
 
#### Structured JSON Content

This chunk holds the structured glTF content description, as it would be provided within a .gltf file.

> **Implementation Note:** In a JavaScript implementation, the `TextDecoder` API can be used to extract the glTF content from the ArrayBuffer, and then the JSON can be parsed with `JSON.parse` as usual.

This chunk must be the very first chunk of Binary glTF asset. By reading this chunk first, an implementation is able to progressively retrieve resources from subsequent chunks. This way, it is also possible to read only a selected subset of resources from a Binary glTF asset (for instance, the coarsest LOD of a mesh).

This chunk must be padded with trailing `Space` chars (`0x20`) to satisfy alignment requirements.  

#### Binary buffer

This chunk contains the binary payload for geometry, animation key frames, skins, and images. See glTF specification for details on referencing this chunk from JSON.

This chunk must be padded with trailing zeros (`0x00`) to satisfy alignment requirements.
