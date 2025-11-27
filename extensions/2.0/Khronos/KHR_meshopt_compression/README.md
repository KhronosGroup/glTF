# KHR\_meshopt\_compression

## Contributors

* Arseny Kapoulkine, [@zeux](https://github.com/zeux)
* Jasper St. Pierre, [@magcius](https://github.com/magcius)
* Alexey Knyazev, [@lexaknyazev](https://github.com/lexaknyazev)
* Don McCurdy, [@donmccurdy](https://github.com/donmccurdy)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a buffer view that also uses `EXT_meshopt_compression`.
- This extension must not be used on a buffer that also uses `EXT_meshopt_compression` (see "Fallback buffers").

## Overview

glTF files come with a variety of binary data - vertex attribute data, index data, morph target deltas, animation inputs/outputs - that can be a substantial fraction of the overall transmission size. To optimize for delivery size, general-purpose compression such as gzip can be used - however, it often doesn't capture some common types of redundancy in glTF binary data.

This extension provides a generic option for compressing binary data that is tailored to the common types of data seen in glTF buffers. The extension works on a bufferView level and as such is agnostic of how the data is used, supporting geometry (vertex and index data, including morph targets), animation (keyframe time and values) and other data, such as instance transforms for `EXT_mesh_gpu_instancing`.

Similarly to supercompressed textures (see `KHR_texture_basisu`), this extension assumes that the buffer view data is optimized for GPU efficiency - using quantization and using optimal data order for GPU rendering - and provides a compression layer on top of bufferView data. Each bufferView is compressed in isolation which allows the loaders to maximally efficiently decompress the data directly into GPU storage.

The compressed format is designed to have two properties beyond optimizing compression ratio - very fast decoding (using WebAssembly SIMD, the decoders run at \~1 GB/sec on modern desktop hardware), and byte-wise storage compatible with general-purpose compression. That is, instead of reducing the encoded size as much as possible, the bitstream is constructed in such a way that general-purpose compressor can compress it further.

This is beneficial for typical Web delivery scenarios, where all files are usually using lossess general-purpose compression (gzip, Brotli, Zstandard) - instead of completely replacing it, the codecs here augment it, while still reducing the size (which is valuable to optimize delivery size when general-purpose compression isn't available, and additionally reduces the performance impact of general-purpose decompression which is typically *much slower* than decoders proposed here).

## Specifying compressed views

As explained in the overview, this extension operates on bufferViews. This allows the loaders to directly decompress data into GPU memory and minimizes the JSON size impact of specifying compressed data. To specify the compressed representation, `KHR_meshopt_compression` extension section overrides the source buffer index as well as specifying the buffer parameters and a compression mode/filter (detailed later in the specification):

```json
{
	"buffer": 1,
	"byteOffset": 0,
	"byteLength": 2368,
	"byteStride": 16,
	"target": 34962,
	"extensions": {
		"KHR_meshopt_compression": {
			"buffer": 0,
			"byteOffset": 1024,
			"byteLength": 347,
			"byteStride": 16,
			"mode": "ATTRIBUTES",
			"count": 148
		}
	}
}
```

In this example, the uncompressed buffer contents is stored in buffer 1 (this can be used by loaders that don't implement this extension). The compressed data is stored in a separate buffer, specifying a separate byte range (with compressed data). Note that for compressors to work, they need to know the compression `mode`, `filter` (for `"ATTRIBUTES"` mode), and additionally the layout of the encoded data - `count` elements with `byteStride` bytes each. This data is specified in the extension JSON; while in some cases `byteStride` is available on the parent `bufferView` declaration, JSON schema prohibits specifying this for some types of storage such as index data.

## JSON schema updates

Each `bufferView` can contain an extension object with the following properties:

| Property | Type | Description | Required |
|:---------|:--------------|:------------------------------------------| :--------------------------|
| `buffer` | `integer` | The index of the buffer with compressed data. | :white_check_mark: Yes |
| `byteOffset` | `integer` | The offset into the buffer in bytes. | No, default: `0` |
| `byteLength` | `integer` | The length of the compressed data in bytes. | :white_check_mark: Yes |
| `byteStride` | `integer` | The stride, in bytes. | :white_check_mark: Yes |
| `count` | `integer` | The number of elements. | :white_check_mark: Yes |
| `mode` | `string` | The compression mode. | :white_check_mark: Yes |
| `filter` | `string` | The compression filter. | No, default: `"NONE"` |

`mode` represents the compression mode using an enumerated value that must be one of `"ATTRIBUTES"`, `"TRIANGLES"`, `"INDICES"`.

`filter` represents the post-decompression filter using an enumerated value that must be one of `"NONE"`, `"OCTAHEDRAL"`, `"QUATERNION"`, `"EXPONENTIAL"`, `"COLOR"`.

For the extension object to be valid, the following must hold:

- The parent `bufferView.byteLength` is equal to `byteStride` times `count`
- When `mode` is `"ATTRIBUTES"`, `byteStride` must be divisible by 4 and must be >= 4 and <= 256.
- When `mode` is `"TRIANGLES"`, `count` must be divisible by 3
- When `mode` is `"TRIANGLES"` or `"INDICES"`, `byteStride` must be equal to 2 or 4
- When `mode` is `"TRIANGLES"` or `"INDICES"`, `filter` must be equal to `"NONE"` or omitted
- When `filter` is `"OCTAHEDRAL"`, `byteStride` must be equal to 4 or 8
- When `filter` is `"QUATERNION"`, `byteStride` must be equal to 8
- When `filter` is `"EXPONENTIAL"`, `byteStride` must be divisible by 4
- When `filter` is `"COLOR"`, `byteStride` must be equal to 4 or 8

The compressed bitstream format is defined by the value of the `mode` property.

The parent `bufferView` properties define a layout which can hold the data decompressed from the extension object.

## Compression modes and filters

Compression mode specifies the bitstream layout and the algorithm used to decompress the data, and can be one of:

- Mode 0: attributes. Suitable for storing sequences of values of arbitrary size, relies on exploiting similarity between bytes of consecutive elements to reduce the size.
- Mode 1: triangles. Suitable for storing indices that represent triangle lists, relies on exploiting topological redundancy of consecutive triangles.
- Mode 2: indices. Suitable for storing indices that don't represent triangle lists, relies on exploiting similarity between consecutive elements.

In all three modes, the resulting compressed byte sequence is typically noticeably smaller than the buffer view length, *and* can be additionally compressed by using a general purpose compression algorithm such as Deflate for the resulting glTF file (.glb/.bin).

The format of the bitstream is specified in [Appendix A (Bitstream)](#appendix-a-bitstream).

When using attribute encoding, for some types of data exploiting the redundancy between consecutive elements is not enough to achieve good compression ratio; quantization can help but isn't always sufficient either. To that end, when using mode 0, this extension allows a further use of a compression filter, that transforms each element stored in the buffer view to make it more compressible with the attribute codec and often allows to trade precision for compressed size. Filters don't change the size of the output data, they merely improve the compressed size by reducing entropy; note that the use of a compression filter restricts `byteStride` which effectively prohibits storing interleaved data.

Filter specifies the algorithm used to transform the data after decompression, and can be one of:

- Filter 0: none. Attribute data is used as is.
- Filter 1: octahedral. Suitable for storing unit length vectors (normals/tangents) as 4-byte or 8-byte values with variable precision octahedral encoding.
- Filter 2: quaternion. Suitable for storing rotation data for animations or instancing as 8-byte values with variable precision max-component encoding.
- Filter 3: exponential. Suitable for storing floating point data as 4-byte values with variable mantissa precision.
- Filter 4: color. Suitable for storing color data as 4-byte or 8-byte values using variable precision YCoCg color model.

The filters are detailed further in [Appendix B (Filters)](#appendix-b-filters).

When using filters, the expectation is that the filter is applied after the attribute decoder on the contents of the resulting bufferView; the resulting data can then be used according to the referencing accessors without further modifications.

**Non-normative** To decompress the data, [meshoptimizer](https://github.com/zeux/meshoptimizer) library may be used; it supports efficient decompression using C++ and/or WebAssembly, including fast SIMD implementation for attribute decoding.

## Fallback buffers

While the extension JSON specifies a separate buffer to source compressed data from, the parent `bufferView` must also have a valid `buffer` reference as per glTF 2.0 spec requirement. To produce glTF files that *require* support for this extension and don't have uncompressed data, the referenced buffer can contain no URI as follows:

```json
{ "byteLength": 1432878 }
```

The `byteLength` property of such a placeholder buffer **MUST** be sufficiently large to contain all uncompressed buffer views referencing it.

When stored in a GLB file, the placeholder buffer should have index 1 or above, to avoid conflicts with GLB binary buffer.

This extension allows buffers to be optionally tagged as fallback by using the `fallback` attribute as follows:

```json
{
	"byteLength": 1432878,
	"extensions": {
		"KHR_meshopt_compression": {
			"fallback": true
		}
	}
}
```

This is useful to avoid confusion, and may also be used by loaders that support the extension to skip loading of these buffers.

When a buffer is marked as a fallback buffer, the following must hold:

- All references to the buffer must come from `bufferView`s that have a `KHR_meshopt_compression` extension specified
- No references to the buffer may come from `KHR_meshopt_compression` extension JSON

If a fallback buffer doesn't have a URI and doesn't refer to the GLB binary chunk, it follows that `KHR_meshopt_compression` must be a required extension.

## Compressing geometry data

> This section is non-normative.

The codecs used by this extension can represent geometry exactly, replicating both vertex and index data without changes in contents or order. However, to get optimal compression, it's necessary to pre-process the data.

To get optimal compression, encoders should optimize vertex and index data for locality of reference. Specifically:

- Triangle order should be optimized to maximize the recency of previously encountered vertices; this is similar to optimizing meshes for vertex reuse aka post-transform cache in GPU hardware.
- Vertex order should be linearized in the order that vertices appear in the index stream to get optimal index compression

When index data is not available (e.g. point data sets) or represents topology with a lot of seams (e.g. each triangle has unique vertex indices because it specifies flat-shaded normal), encoders could additionally optimize vertex data for spatial locality, so that vertices close together in the vertex stream are close together in space.

Vertex data should be quantized using the appropriate representation; this extension cleanly interacts with `KHR_mesh_quantization` by compressing already quantized data.

Morph targets can be treated identically to other vertex attributes, as long as vertex order optimization is performed on all target streams at the same time. It is recommended to use quantized storage for morph target deltas, possibly with a narrower type than that used for baseline values.

When storing vertex data, mode 0 (attributes) should be used; for index data, mode 1 (triangles) or mode 2 (indices) should be used instead. Mode 1 only supports triangle list storage; indices of other topology types can be stored using mode 2. The use of triangle strip topology is not recommended since it's more efficient to store triangle lists using mode 1.

Using filter 1 (octahedral) for normal/tangent data, and filter 4 (color) for color data, may improve compression ratio further.

While using quantized attributes is recommended for optimal compression, it's also possible to use non-quantized floating point attributes. To increase compression ratio in that case, filter 3 (exponential) is recommended - advanced encoders can additionally constraint the exponent to be the same for all components of a vector, or for all values of the same component across the entire mesh, which can further improve compression ratio.

## Compressing animation data

> This section is non-normative.

To minimize the size of animation data, it is important to reduce the number of stored keyframes and reduce the size of each keyframe.

To reduce the number of keyframes, encoders can either selectively remove keyframes that don't contribute to the resulting movement, resulting in sparse input/output data, or resample the keyframes uniformly, resulting in uniformly dense data. Resampling can be beneficial since it means that all animation channels in the same animation can share the same input accessor, and provides a convenient quality vs size tradeoff, but it's up to the encoder to pick the optimal strategy.

Additionally it's important to identify tracks with the same output value and use a single keyframe for these.

To reduce the size of each keyframe, rotation data should be quantized using 16-bit normalized components; for additional compression, the use of filter 2 (quaternion) is recommended. Translation/scale data can be compressed using filter 3 (exponential); for scale data, using the same exponent for all three vector components can enhance compression ratio further.

Note that animation inputs that specify time values require enough precision to avoid animation distortion. It's recommended to either not use any filters for animation inputs to avoid any precision loss (attribute encoder can still be efficient at reducing the size of animation input track even without filters when the inputs are uniformly spaced), or use filter 3 (exponential) with maximum mantissa bit count (23).

After pre-processing, both input and output data should be stored using mode 0 (attributes).

When `EXT_mesh_gpu_instancing` extension is used, the instance transform data can also be compressed with the same techniques as animation data, using mode 0 (attributes) with filter 3 (exponential) for position and scale, and filter 2 (quaternion) for rotation.

## Content guidelines

> This section is non-normative.

This extension expands the compression available by the existing extension `EXT_meshopt_compression`. Since existing tools and pipelines already support that extension, and existing assets already use it, the following guidelines are recommended for content creators and tool authors:

- Tools that already support `EXT_meshopt_compression` extension should keep supporting it alongside this extension to be able to read pre-existing assets.
- For maximum compabitility, DCC tools should give users a choice to use either variant when exporting assets. The default option should be eventually switched to the KHR variant once most loaders support it.
- Existing assets that use the EXT variant can be losslessly converted to KHR, if needed, by changing the extension strings inside glTF JSON.
- When producing assets that target loaders supporting both extensions, using this extension with v1 format should be preferred since it provides better compression ratio at no additional runtime cost.

# Appendix A: Bitstream

The following sections specify the format of the bitstream for compressed data for various modes.

## Mode 0: attributes

Attribute compression exploits similarity between consecutive elements of the buffer by encoding deltas. The deltas are stored for each separate byte which makes the codec more versatile since it can work with components of various sizes. Additionally, the elements are stored with bytes deinterleaved, which means that sequences of deltas are more easily compressible by some general purpose compressors that may run on the resulting data.

To facilitate efficient decompression, deinterleaving and delta encoding are performed on attribute blocks instead of on the entire buffer; within each block, elements are processed in groups of 16.

The encoded stream structure is as follows:

- Header byte, which must be equal to `0xa1` (version 1) or `0xa0` (version 0)
- One or more attribute blocks, detailed below
- Tail padding, which pads the size of the subsequent tail block with zero bytes to a minimum of 24 bytes for version 1 or 32 bytes for version 0 (required for efficient decoding)
- Tail block, which consists of a baseline element stored verbatim (`byteStride` bytes), followed by channel modes (`byteStride / 4` bytes, only in version 1)

**Non-normative** While using version 1 is preferred for better compression, version 0 is provided for binary compatibility with `EXT_meshopt_compression`. When using version 0, the bitstream is identical to that defined in `EXT_meshopt_compression`.

Note that there is no way to calculate the length of a stream; instead, the input stream must be correctly sized (using `byteLength`) so that the tail block element can be found. If the decoding procedure reaches the end of stream too early, or any unprocessed bytes remain after decoding and before tail, the stream is invalid.

Each attribute block encodes a sequence of deltas, with the first element in the first block using the deltas from the baseline element stored in the tail block, and each subsequent element using the deltas from the previous element. The attribute block always stores an integer number of elements, with that number computed as follows:

```
maxBlockElements = min((8192 / byteStride) & ~15, 256)
blockElements = min(remainingElements, maxBlockElements)
```

Where `remainingElements` is the number of elements that have yet to be decoded (with the initial value of `count` extension property).

Each attribute block consists of:
- Control header (only in version 1): `byteStride / 4` bytes specifying a packed 2-bit control mode for each byte position of the element
- `byteStride` "data blocks" (one for each byte of the element), each containing deltas stored for groups of elements

Each group always contains 16 elements; when the number of elements that needs to be encoded isn't divisible by 16, it gets rounded up and the remaining elements are ignored after decoding. In other terms:

```
groupCount = ceil(blockElements / 16)
```

For example, a stream with a `byteStride` of 64 containing 200 elements would be broken up into two attribute blocks: one containing 128 elements, and the other containing 72 elements. And these blocks would have 8 and 5 groups, respectively.

The control header (only present in version 1) contains 2 bits for each byte position, packed into bytes as follows:

```
controlByte = (controlForByte0 << 0) | (controlForByte1 << 2) | (controlForByte2 << 4) | (controlForByte3 << 6)
```

The control bits specify the control mode for each byte:

- control mode 0: Use bit lengths `{0, 1, 2, 4}` for encoding
- control mode 1: Use bit lengths `{1, 2, 4, 8}` for encoding
- control mode 2: All delta bytes are 0; no data is stored for this byte
- control mode 3: Literal encoding; delta bytes are stored uncompressed with no header bits

The structure of each "data block" (when using control mode 0 or 1, or when using version 0) breaks down as follows:
- Header bits, with 2 bits for each group, aligned to the byte boundary if groupCount is not divisible by 4
- Delta blocks, with variable number of bytes stored for each group

Header bits are stored from least significant to most significant bit - header bits for 4 consecutive groups are packed in a byte together as follows:

```
(headerBitsForGroup0 << 0) | (headerBitsForGroup1 << 2) | (headerBitsForGroup2 << 4) | (headerBitsForGroup3 << 6)
```

The header bits establish the delta encoding mode for each group of 16 elements:

For control mode 0 (version 1):
- delta encoding mode 0: All 16 byte deltas are 0; the size of the encoded block is 0 bytes
- delta encoding mode 1: Deltas are stored in 1-bit sentinel encoding; the size of the encoded block is [2..18] bytes
- delta encoding mode 2: Deltas are stored in 2-bit sentinel encoding; the size of the encoded block is [4..20] bytes
- delta encoding mode 3: Deltas are stored in 4-bit sentinel encoding; the size of the encoded block is [8..24] bytes

For control mode 1 (version 1):
- delta encoding mode 0: Deltas are stored in 1-bit sentinel encoding; the size of the encoded block is [2..18] bytes
- delta encoding mode 1: Deltas are stored in 2-bit sentinel encoding; the size of the encoded block is [4..20] bytes
- delta encoding mode 2: Deltas are stored in 4-bit sentinel encoding; the size of the encoded block is [8..24] bytes
- delta encoding mode 3: All 16 byte deltas are stored as bytes; the size of the encoded block is 16 bytes

For version 0:
- delta encoding mode 0: All 16 byte deltas are 0; the size of the encoded block is 0 bytes
- delta encoding mode 1: Deltas are stored in 2-bit sentinel encoding; the size of the encoded block is [4..20] bytes
- delta encoding mode 2: Deltas are stored in 4-bit sentinel encoding; the size of the encoded block is [8..24] bytes
- delta encoding mode 3: All 16 byte deltas are stored as bytes; the size of the encoded block is 16 bytes

When using the sentinel encoding, each delta is stored as a 1-bit, 2-bit, or 4-bit value in packed bytes. For 2-bit and 4-bit encodings, deltas are stored from most significant to least significant bit inside the byte. For 1-bit encoding, deltas are stored from least significant to most significant bit to facilitate better reuse of lookup tables in efficient implementations. The 1-bit encoding is packed as follows with 8 deltas per byte:

```
(delta0 << 0) | (delta1 << 1) | (delta2 << 2) | (delta3 << 3) | (delta4 << 4) | (delta5 << 5) | (delta6 << 6) | (delta7 << 7)
```

The 2-bit encoding is packed as follows with 4 deltas per byte:

```
(delta3 << 0) | (delta2 << 2) | (delta1 << 4) | (delta0 << 6)
```

And the 4-bit encoding is packed as follows with 2 deltas per byte:

```
(delta1 << 0) | (delta0 << 4)
```

A delta that has all bits set to 1 (corresponds to `1` for 1-bit encoding, `3` for 2-bit encoding, and `15` for 4-bit encoding, otherwise known as "sentinel") indicates that the real delta value is outside of the bit range, and is stored as a full byte after the bit deltas for this group.

To decode deltas into original values, the channel modes (specified in the tail block for version 1) are used. When using version 0, the channel mode is assumed to be 0 (byte deltas); other modes can only be present in version 1. The encoded stride is split into `byteStride / 4` channels, and each channel specifies the mode in a single byte in the tail block, with the low 4 bits of the byte specifying the mode:

**Channel mode 0 (byte deltas)**: Byte deltas are stored as zigzag-encoded differences between the byte values of the element and the byte values of the previous element in the same position; the zigzag encoding scheme works as follows:

```
encode(uint8_t v) = ((v & 0x80) != 0) ? ~(v << 1) : (v << 1)
decode(uint8_t v) = ((v & 1) != 0) ? ~(v >> 1) : (v >> 1)
```

For a complete example, assuming 4-bit sentinel coding, the following byte sequence:

```
0x17 0x5f 0xf0 0xbc 0x77 0xa9 0x21 0x00 0x34 0xb5
```

Encodes 16 deltas, where the first 8 bytes of the sequence specify 16 4-bit deltas, and the last 2 bytes of the sequence specify the explicit delta code values encoded for elements 3 and 4 in the sequence. After de-zigzagging, the decoded deltas look like:

```
-1 -4 -3 26 -91 0 -6 6 -4 -4 5 -5 1 -1 0 0
```

Finally, note that the deltas are computed in 8-bit integer space with wraparound two's complement arithmetic; for example, if the values of the first byte of two consecutive elements are `0x00` and `0xff`, the byte delta that is stored is `-1` (`1` after zigzag encoding).

**Channel mode 1 (2-byte deltas)**: 2-byte deltas are computed as zigzag-encoded differences between 16-bit values of the element and the previous element in the same position; the zigzag encoding scheme works as follows:

```
encode(uint16_t v) = ((v & 0x8000) != 0) ? ~(v << 1) : (v << 1)
decode(uint16_t v) = ((v & 1) != 0) ? ~(v >> 1) : (v >> 1)
```

The deltas are computed in 16-bit integer space with wraparound two's complement arithmetic. Values are assumed to be little-endian, so the least significant byte is encoded before the most significant byte.

**Channel mode 2 (4-byte XOR deltas)**: 4-byte deltas are computed as XOR between 32-bit values of the element and the previous element in the same position, with an additional rotation `r` applied based on the high 4 bits of the channel mode byte:

```
rotate(uint32_t v, uint r) = (v << r) | (v >> ((32 - r) & 31))
```

The deltas are computed in 32-bit integer space. Values are assumed to be little-endian, so the least significant byte is encoded before the most significant byte.

Because the channel mode defines encoding for 4 bytes at once, it's impossible to mix modes 0 and 1 within the same channel: if the first 2-byte group of an aligned 4-byte group uses 2-byte deltas, the second 2-byte group must use 2-byte deltas as well.

Streams that use channel mode 3 or above, as well as streams that use channel mode 0 or 1 with high 4 bits of the channel mode byte not equal to 0, are invalid.

## Mode 1: triangles

Triangle compression compresses triangle list indices by exploiting similarity between consecutive triangles. Given a triangle stream that has been optimized for locality, very often subsequent triangles share an edge with the recently encoded triangle. The encoder uses a few other techniques to try to encode most triangles in optimized triangle lists into a single byte.

The encoded stream structure is as follows:

- Header byte, which must be equal to `0xe1`
- Triangle codes, referred to as `code` below, with a single byte for each triangle (for a total of `count` extension property divided by 3, since `count` counts index values)
- Extra data which is necessary to decode triangles that don't fit into a single byte, referred to as `data` below
- Tail block, which consists of a 16-byte lookup table (containing 16 one-byte values), referred to as `codeaux` below

Note that there is no way to calculate the length of a stream; instead, the input stream must be correctly sized (using `byteLength`) so that the tail block element can be found. If the decoding procedure reaches the end of stream too early, or any unprocessed bytes remain after decoding and before tail, the stream is invalid.

There are two limitations on the structure of the 16-byte lookup table:

- The last two bytes must be 0
- Neither high four bits nor low four bits of any of 16 bytes can be equal to `0xf`.

During the decoding process, decoder maintains five variables:

- current offset into `data` section
- `next`: an integer referring to the expected next unique index (also known as high-watermark), starts at 0
- `last`: an integer referring to the last encoded index, starts at 0
- `edgefifo`: a 16-entry FIFO with two `uint32_t` vertex indices in each entry; initial contents is undefined
- `vertexfifo`: a 16-entry FIFO with a `uint32_t` vertex index in each entry; initial contents is undefined

To decode each triangle, the decoder needs to analyze the `code` byte, read additional bytes from `data` as necessary, and update the internal state correctly. The `code` byte encoding is optimized to reach a single byte per triangle in most common cases; the resulting data can often be compressed by a general purpose compressor running on the resulting .bin/.glb file.

When extra data is necessary to decode a triangle and it represents an index value, the decoder uses varint-7 encoding (also known as [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128#Unsigned_LEB128)) with up to 5 bytes, which encodes an integer as one or more bytes, with the byte with the 0 most significant bit terminating the sequence:

```
0x7f => 0x7f
0x81 0x04 => 0x201
0xff 0xa0 0x05 => 0x1fd005
```

Instead of using the raw index value, a zigzag-encoded 32-bit delta from `last` is used:

```
uint32_t decodeIndex(uint32_t v) {
	int32_t delta = (v & 1) != 0 ? ~(v >> 1) : (v >> 1);

	last += delta;
	return last;
}
```

The encoding for `code` is split into various cases, some of which are self-sufficient and some need to read extra data. The encoding is detailed below; after either path the triangle (a, b, c) is emitted to the output.

Any streams that require the decoder to read an edge or vertex FIFO entry that was not previously written are invalid.

- `0xX0`, where `X < 0xf`: Encodes a recently encountered edge and a `next` vertex.

The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is equal to `next` (which is then incremented).

Edge (c, b) is pushed to the edge FIFO.
Edge (a, c) is pushed to the edge FIFO.
Vertex c is pushed to the vertex FIFO.

- `0xXY`, where `X < 0xf` and `0 < Y < 0xd`: Encodes a recently encountered edge and a recently encountered vertex.

The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is read from the vertex FIFO at index Y (where 0 is the most recently added vertex; note that 0 is never actually read here, since `Y > 0`).

Edge (c, b) is pushed to the edge FIFO.
Edge (a, c) is pushed to the edge FIFO.

- `0xXd` or `0xXe`, where `X < 0xf`: Encodes a recently encountered edge and a vertex that's adjacent to `last`.

The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is equal to `last-1` for `0xXd` and `last+1` for `0xXe`.

`last` is set to `c` (effectively decrementing or incrementing it accordingly).

Edge (c, b) is pushed to the edge FIFO.
Edge (a, c) is pushed to the edge FIFO.
Vertex c is pushed to the vertex FIFO.

- `0xXf`, where `X < 0xf`: Encodes a recently encountered edge and a free-standing vertex encoded explicitly.

The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is decoded using `decodeIndex` by reading extra bytes from `data` (and also updates `last`).

Edge (c, b) is pushed to edge FIFO.
Edge (a, c) is pushed to edge FIFO.
Vertex c is pushed to the vertex FIFO.

- `0xfY`, where `Y < 0xe`: Encodes three indices using `codeaux` table lookup and vertex FIFO.

The table `codeaux` is used to read the element Y; let's assume that results in `0xZW`.

The triangle indices are set as follows:

Set first index, `a`, to `next`.
Increment `next`.
If `Z == 0`:
	Set second index, `b`, to `next`.
	Increment `next`.
Else read second index, `b`, from vertex FIFO at index `Z-1` (where 0 is the most recently added vertex).
If `W == 0`:
	Set third index, `c`, to `next`.
	Increment `next`.
Else read third index, `c`, from vertex FIFO at index `W-1` (where 0 is the most recently added vertex).

Note that in the process `next` is incremented from 1 to 3 times depending on values of Z/W.

Edge (b, a) is pushed to the edge FIFO.
Edge (c, b) is pushed to the edge FIFO.
Edge (a, c) is pushed to the edge FIFO.
Vertex a is pushed to the vertex FIFO.
Vertex b is pushed to the vertex FIFO if `Z == 0`.
Vertex c is pushed to the vertex FIFO if `W == 0`.

- `0xfe` or `0xff`: Encodes three indices explicitly.

Read one byte from `data` as-is, without using LEB128 decoding; let's assume that results in `0xZW`.

If `0xZW` == `0x00`, then `next` is reset to 0. This is a special mechanism used to restart the `next` sequence which is useful for concatenating independent triangle streams. This must be done before further processing.

The triangle indices are set as follows:

If using `0xfe` encoding:
	Set first index, `a`, to `next`.
	Increment `next`.
Else read first index, `a`, using `decodeIndex` by reading extra bytes from `data` (note, this also updates `last`).

If `Z == 0`:
	Set second index, `b`, to `next`.
	Increment `next`.
Else if `Z < 0xf`:
	Read second index, `b`, from vertex FIFO at index `Z-1` (where 0 is the most recently added vertex).
Else read second index, `b`, using `decodeIndex` by reading extra bytes from `data` (note, this also updates `last`).

If `W == 0`:
	Set third index, `c`, to `next`.
	Increment `next`.
Else if `W < 0xf`:
	Read third index, `c`, from vertex FIFO at index `W-1` (where 0 is the most recently added vertex).
Else read third index, `c`, using `decodeIndex` by reading extra bytes from `data` (note, this also updates `last`).

Edge (b, a) is pushed to the edge FIFO.
Edge (c, b) is pushed to the edge FIFO.
Edge (a, c) is pushed to the edge FIFO.
Vertex a is pushed to the vertex FIFO.
Vertex b is pushed to the vertex FIFO if `Z == 0` or `Z == 0xf`.
Vertex c is pushed to the vertex FIFO if `W == 0` or `W == 0xf`.

At the end of the decoding, `data` is expected to be fully read by all the triangle codes and not contain any extra bytes.

## Mode 2: indices

Index compression exploits similarity between consecutive indices. Note that, unlike the triangle index compression (mode 1), this mode doesn't assume a specific topology and as such is less efficient in terms of the resulting size. However, unlike mode 1, this mode can be used to compress triangle strips, line lists and other types of mesh index data, and can additionally be used to compress non-mesh index data such as sparse indices for accessors.

The encoded stream structure is as follows:

- Header byte, which must be equal to `0xd1`
- A sequence of index deltas (with number of elements equal to `count` extension property), with encoding specified below
- Tail block, which consists of 4 bytes that are reserved and should be set to 0

Note that there is no way to calculate the length of a stream; instead, the input stream must be correctly sized (using `byteLength`) so that the tail block element can be found. If the decoding procedure reaches the end of stream too early, or any unprocessed bytes remain after decoding and before tail, the stream is invalid.

Instead of simply encoding deltas vs the previous index, the decoder tracks *two* baseline index values, that start at 0. Each delta is specified in relation to one of these values and updates it so that the next delta that references the same baseline uses the encoded index value as a reference. This encoding is more efficient at handling some types of bimodal sequences where two independent monotonic sequences are spliced together, which can occur for some common cases of triangle strips or line lists.

To specify the index delta, the varint-7 encoding scheme (also known as [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128#Unsigned_LEB128)) with up to 5 bytes is used, which encodes an integer as one or more bytes, with the byte with the 0 most significant bit terminating the sequence:

```
0x7f => 0x7f
0x81 0x04 => 0x201
0xff 0xa0 0x05 => 0x1fd005
```

When decoding the deltas, the 32-bit value is read using the varint-7 encoding. The least significant bit of the value indicates one of the baseline values; the remaining bits specify a zigzag-encoded signed delta and can be decoded as follows:

```
uint32_t decode(uint32_t v) {
	int32_t baseline = v & 1;
	int32_t delta = (v & 2) != 0 ? ~(v >> 2) : (v >> 2);

	last[baseline] += delta;
	return last[baseline];
}
```

It's up to the encoder to determine the optimal selection of the baseline for each index; this encoding scheme can be used to do basic delta encoding (with baseline bit always set to 0) as well as more complex bimodal encodings. Since zigzag-encoded delta uses a 31-bit integer, the deltas are limited to [-2^30..2^30-1].

# Appendix B: Filters

Filters are functions that transform each encoded attribute. For each filter, this document specifies the transformation used for decoding the data; it's up to the encoder to pick the parameters of the encoding for each element to balance quality and precision.

For performance reasons the results of the decoding process are specified to one unit in last place (ULP) in terms of the decoded data, e.g. if a filter results in a 16-bit signed normalized integer, decoding may produce results within 1/32767 of specified value.

## Filter 1: octahedral

Octahedral filter allows to encode unit length 3D vectors (normals/tangents) using octahedral encoding, which results in a more optimal quality vs precision tradeoff compared to storing raw components.

This filter is only valid if `byteStride` is 4 or 8. When `byteStride` is 4, then the input and output of this filter are four 8-bit signed components, and when `byteStride` is 8, the input and output of this filter are four 16-bit signed components.

The input to the filter is four 8-bit or 16-bit components, where the first two specify the X and Y components in octahedral encoding encoded as signed normalized K-bit integers (2 <= K <= 16, integers are stored in two's complement format), the third component explicitly encodes 1.0 as a signed normalized K-bit integer. The last component may contain arbitrary data which is passed through unfiltered (this can be useful for tangents).

The encoding of the third component allows to compute K for each vector independently from the bit representation, and must encode 1.0 precisely which is equivalent to `(1 << (K - 1)) - 1` as an integer; values of the third component that aren't equal to `(1 << (K - 1)) - 1` for a valid `K` are invalid and the result of decoding such vectors is unspecified.

When storing a K-bit integer in a 8-bit of 16-bit component when K is not 8 or 16, the remaining bits (e.g. top 6 bits in case of K=10) must be equal to the sign bit; the valid range of the resulting integer is from `-max` to `max` where `max = (1 << (K - 1)) - 1`. The behavior of decoding values outside of that range is unspecified.

The output of the filter is three decoded unit vector components, stored as 8-bit or 16-bit normalized integers, and the last input component verbatim.

```
void decode(intN_t input[4], intN_t output[4]) {
	// input[2] encodes a K-bit representation of 1.0
	float32_t one = input[2];

	float32_t x = input[0] / one;
	float32_t y = input[1] / one;
	float32_t z = 1.0 - abs(x) - abs(y);

	// octahedral fixup for negative hemisphere
	float32_t t = min(z, 0.0);

	x -= copysign(t, x);
	y -= copysign(t, y);

	// renormalize (x, y, z)
	float32_t len = sqrt(x * x + y * y + z * z);

	x /= len;
	y /= len;
	z /= len;

	output[0] = round(x * INTN_MAX);
	output[1] = round(y * INTN_MAX);
	output[2] = round(z * INTN_MAX);
	output[3] = input[3];
}
```

`INTN_MAX` is equal to 127 when using 8-bit components (N is 8) and equal to 32767 when using 16-bit components (N is 16).

`copysign` returns the value with the magnitude of the first argument and the sign of the second argument.

`round` returns the nearest integer value, rounding halfway cases away from zero.

## Filter 2: quaternion

Quaternion filter allows to encode unit length quaternions using normalized 16-bit integers for all components, but allows control over the precision used for the components and provides better quality compared to naively encoding each component one by one.

This filter is only valid if `byteStride` is 8.

The input to the filter is three quaternion components, excluding the component with the largest magnitude, encoded as signed normalized K-bit integers (4 <= K <= 16, integers are stored in two's complement format), and an index of the largest component that is omitted in the encoding. The largest component is assumed to always be positive (which is possible due to quaternion double-cover). To allow per-element control over K, the last input element must explicitly encode 1.0 as a signed normalized K-bit integer, except for the least significant 2 bits that store the index of the maximum component.

When storing a K-bit integer in a 16-bit component when K is not 16, the remaining bits (e.g. top 6 bits in case of K=10) must be equal to the sign bit; the valid range of the resulting integer is from `-max` to `max` where `max = (1 << (K - 1)) - 1`. The behavior of decoding values outside of that range is unspecified.

The output of the filter is four decoded quaternion components, stored as 16-bit normalized integers.

After eliminating the maximum component, the maximum magnitude of the remaining components is `1.0/sqrt(2.0)`. Because of this the input components store the original component value scaled by `sqrt(2.0)` to increase precision.

```
void decode(int16_t input[4], int16_t output[4]) {
	float32_t range = 1.0 / sqrt(2.0);

	// input[3] encodes a K-bit representation of 1.0 except for bottom two bits
	float32_t one = input[3] | 3;

	float32_t x = input[0] / one * range;
	float32_t y = input[1] / one * range;
	float32_t z = input[2] / one * range;

	float32_t w = sqrt(max(0.0, 1.0 - x * x - y * y - z * z));

	int maxcomp = input[3] & 3;

	// maxcomp specifies a cyclic rotation of the quaternion components
	output[(maxcomp + 1) % 4] = round(x * 32767.0);
	output[(maxcomp + 2) % 4] = round(y * 32767.0);
	output[(maxcomp + 3) % 4] = round(z * 32767.0);
	output[(maxcomp + 0) % 4] = round(w * 32767.0);
}
```

## Filter 3: exponential

Exponential filter allows to encode floating point values with a range close to the full range of a 32-bit floating point value, but allows more control over the exponent/mantissa to trade quality for precision, and has a bit structure that is more optimally aligned to the byte boundary to facilitate better compression.

This filter is only valid if `byteStride` is a multiple of 4.

The input to the filter is a sequence of 32-bit little endian integers, with the most significant 8 bits specifying a (signed) exponent value, and the remaining 24 bits specifying a (signed) mantissa value. The integers are stored in two's complement format.

The result of the filter is 2^e * m:

```
float32_t decode(int32_t input) {
	int32_t e = input >> 24;
	int32_t m = (input << 8) >> 8;
	return pow(2.0, e) * m;
}
```

The valid range of `e` is [-100, +100], which facilitates performant implementations. Decoding out of range values results in unspecified behavior, and encoders are expected to clamp `e` to the valid range.

## Filter 4: color

Color filter allows to encode color data using YCoCg color model, which results in better compression for typical color data by exploiting correlation between color channels.

This filter is only valid if `byteStride` is 4 or 8. When `byteStride` is 4, then the input and output of this filter are four 8-bit components, and when `byteStride` is 8, the input and output of this filter are four 16-bit components.

The input to the filter is four 8-bit or 16-bit components, where the first component stores the Y (luma) value as a K-bit unsigned integer, the second and third components store Co/Cg (chrominance) values as K-bit signed integers, and the fourth component stores the alpha value as a K-1-bit unsigned integer with the bit K set to 1. 1 <= K <= 16, signed integers are stored in two's complement format.

The transformation uses YCoCg encoding; reconstruction of RGB values can be performed in integer space or in floating point space, depending on the implementation. The encoder must guarantee that original RGB values can be reconstructed using K-bit integer math without overflow or underflow.

The alpha component uses K-1 bits for the alpha value with the high bit set to 1; note that K can be smaller than the bit depth. This allows decoder to recover K and decode the color and alpha values correctly.

The output of the filter is four decoded color components (R, G, B, A), stored as 8-bit or 16-bit normalized integers.

```
void decode(intN_t input[4], intN_t output[4]) {
	// recover scale from alpha high bit
	int as = (1 << (findMSB(input[3]) + 1)) - 1;

	// convert to RGB in fixed point
	int y = input[0], co = input[1], cg = input[2];

	int r = y + co - cg;
	int g = y + cg;
	int b = y - co - cg;

	// expand alpha by one bit to match other components, replicating last bit
	int a = input[3] & (as >> 1);
	a = (a << 1) | (a & 1);

	// compute scaling factor
	float ss = INTN_MAX / float(as);

	output[0] = round(float(r) * ss);
	output[1] = round(float(g) * ss);
	output[2] = round(float(b) * ss);
	output[3] = round(float(a) * ss);
}
```

`INTN_MAX` is equal to 255 when using 8-bit components (N is 8) and equal to 65535 when using 16-bit components (N is 16).

# Appendix C: Differences from EXT_meshopt_compression

This extension is derived from `EXT_meshopt_compression` with the following changes:

- Vertex data supports an upgraded v1 format which provides more granular bit packing (via control modes) and enhanced delta encoding (via channel modes) to compress data better
- For compatibility, the v0 format (identical to `EXT_meshopt_compression` format) is still supported; however, use of v1 format is preferred
- New `COLOR` filter supports lossy color compression at smaller compression ratios using YCoCg encoding

These improvements achieve better compression ratios for typical glTF content while maintaining the same fast decompression performance.
