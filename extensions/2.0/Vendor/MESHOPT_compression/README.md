# MESHOPT\_compression

## Contributors

* Arseny Kapoulkine, [@zeuxcg](https://twitter.com/zeuxcg)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

glTF files come with a variety of binary data - vertex attribute data, index data, morph target deltas, animation inputs/outputs - that can be a substantial fraction of the overall transmission size. To optimize for delivery size, general-purpose compression such as gzip can be used - however, it often doesn't capture some common types of redundancy in glTF binary data.

To achieve optimal compression rates for mesh geometry data, glTF supports Draco mesh compression through KHR\_draco\_mesh\_compression extension. However, this comes at a cost of decoding performance (Draco decoding takes an appreciable time which can start offsetting the loading time for faster connections) and inability to directly upload buffer view data to GPU storage or, for most implementations, use quantized storage in the output. Additionally, animation and morph target data isn't supported.

This extension provides a different option for compressing binary data. Similarly to supercompressed textures (see KHR\_image\_ktx2), this extension assumes that the buffer view data is optimized for GPU efficiency - using quantization and using optimal data order for GPU rendering - and provides a compression layer on top of bufferView data. Each bufferView is compressed in isolation which allows the loaders to maximally efficiently decompress the data directly into GPU storage.

The compressed format is designed to have two properties beyond optimizing compression ratio - very fast decoding (using WebAssembly SIMD, the decoders run at \~1 GB/sec on modern desktop hardware), and byte-wise storage compatible with general-purpose compression. That is, instead of reducing the encoded size as much as possible, the bitstream is constructed in such a way that general-purpose compressor can compress it further.

This is beneficial for typical Web delivery scenarios, where all files are usually using gzip compression - instead of completely replacing it, the codecs here augment it, while still reducing the size (which is valuable to optimize delivery size when gzip compression isn't available, and additionally reduces the performance impact of gzip decompression which is typically *much slower* than decoders proposed here).

## Specifying compressed views

As explained in the overview, this extension operates on bufferViews. This allows the loaders to directly decompress data into GPU memory and minimizes the JSON size impact of specifying compressed data. To specify the compressed representation, `MESHOPT_compression` extension section overrides the source buffer index as well as specifying the buffer parameters and a compression mode/filter (detailed later in the specification):

```json
{
	"buffer": 1,
	"byteOffset": 0,
	"byteLength": 2368,
	"byteStride": 16,
	"target": 34962,
	"extensions": {
		"MESHOPT_compression": {
			"buffer": 0,
			"byteOffset": 1024,
			"byteLength": 347,
			"byteStride": 16,
			"mode": 0,
			"count": 148
		}
	}
}
```

In this example, the uncompressed buffer contents is stored in buffer 1 (this can be used by loaders that don't implement this extension). The compressed data is stored in a separate buffer, specifying a separate byte range (with compressed data). Note that for compressors to work, they need to know the compression `mode`, `filter` (for `mode 0`), and additionally the layout of the encoded data - `count` elements with `byteStride` bytes each. This data is specified in the extension JSON; while in some cases `byteStride` is available on the parent `bufferView` declaration, JSON schema prohibits specifying this for some types of storage such as index data.

## JSON schema updates

Each `bufferView` can contain an extension object with the following properties:

| Property | Type | Description | Required |
|:---------|:--------------|:------------------------------------------| :--------------------------|
| `buffer` | `integer` | The index of the buffer with compressed data. | :white_check_mark: Yes |
| `byteOffset` | `integer` | The offset into the buffer in bytes. | No, default: `0` |
| `byteLength` | `integer` | The length of the compressed data in bytes. | :white_check_mark: Yes |
| `byteStride` | `integer` | The stride, in bytes. | :white_check_mark: Yes |
| `count` | `integer` | The number of elements. | :white_check_mark: Yes |
| `mode` | `integer` | The compression mode. | :white_check_mark: Yes |
| `filter` | `integer` | The compression filter. | No, default: `0` |

For the extension object to be valid, the following must hold:

- When parent `bufferView` has `byteStride` defined, it matches `byteStride` in the extension JSON
- Buffer view length is equal to `byteStride` times `count`
- When `mode` is 0 (attributes), `byteStride` must be divisible by 4
- When `mode` is 1 (triangles), `count` must be divisible by 3
- When `mode` is 1 (triangles) or 2 (indices), `byteStride` must be equal to 2 or 4
- When `mode` is 1 (triangles) or 2 (indices), `filter` must be equal to `0` or omitted
- When `filter` is 1 (octahedral), `byteStride` must be equal to 4 or 8
- When `filter` is 2 (quaternion), `byteStride` must be equal to 8

The type of compressed data must match the bitstream specification (note that each `mode` specifies a different bitstream format).

## Compression modes and filters

Compression mode specifies the bitstream layout and the algorithm used to decompress the data, and can be one of:

- Mode 0: attributes. Suitable for storing sequences of values of arbitrary size, relies on exploiting similarity between bytes of consecutive elements to reduce the size.
- Mode 1: triangles. Suitable for storing indices that represent triangle lists, relies on exploiting topological redundancy of consecutive triangles.
- Mode 2: indices. Suitable for storing indices that don't represent triangle lists, relies on exploiting similarity between consecutive elements.

In all three modes, the resulting compressed byte sequence is typically noticably smaller than the buffer view length, *and* can be additionally compressed by using a general purpose compression algorithm such as Deflate for the resulting glTF file (.glb/.bin).

The format of the bitstream is specified in Appendix A (Bitstream).

When using attribute encoding, for some types of data exploiting the redundancy between consecutive elements is not enough to achieve good compression ratio; quantization can help but isn't always sufficient either. To that end, when using mode 0, this extension allows a further use of a compression filter, that transforms each element stored in the buffer view to make it more compressible with the attribute codec and often allows to trade precision for compressed size. Filters don't change the size of the output data, they merely improve the compressed size by reducing entropy; note that the use of a compression filter restricts `byteStride` which effectively prohibits storing interleaved data.

Filter specifies the algorithm used to transform the data after decompression, and can be one of:

- Filter 0: none. Attribute data is used as is.
- Filter 1: octahedral. Suitable for storing unit length vectors (normals/tangents) as 4-byte or 8-byte values with variable precision octahedral encoding.
- Filter 2: quaternion. Suitable for storing rotation data for animations or instancing as 8-byte values with variable precision max-component encoding.
- Filter 3: exponential. Suitable for storing floating point data as 4-byte values with variable mantissa precision.

The filters are detailed further in Appendix B (Filters).

When using filters, the expectation is that the filter is applied after the attribute decoder on the contents of the resulting bufferView; the resulting data can then be used according to the referencing accessors without further modifications.

**Non-normative** To decompress the data, [meshoptimizer](https://github.com/zeux/meshoptimizer) library may be used; it supports efficient decompression using C++ and/or WebAssembly, including fast SIMD implementation for attribute decoding.

## Fallback buffers

While the extension JSON specifies a separate buffer to source compressed data from, the parent `bufferView` must also have a valid `buffer` reference as per glTF 2.0 spec requirement. To produce glTF files that *require* support for this extension and don't have uncompressed data, the referenced buffer can contain no URI as follows:

```json
{ "byteLength": 1432878 }
```

When stored in a GLB file, the dummy buffer should have index 1 or above, to avoid conflicts with GLB binary buffer.

This extension allows buffers to be tagged as fallback by using the `fallback` attribute as follows:

```json
{
	"byteLength": 1432878,
	"extensions": {
		"MESHOPT_compression": {
			"fallback": true
		}
	}
}
```

This is useful to avoid confusion, and can also be used by loaders that support the extension to skip loading of these buffers.

When a buffer is marked as a fallback buffer, the following must hold:

- All references to the buffer must come from `bufferView`s that have a `MESHOPT_compression` extension specified
- No references to the buffer may come from `MESHOPT_compression` extension JSON

If a fallback buffer doesn't have a URI and doesn't refer to the GLB binary chunk, it follows that `MESHOPT_compression` must be a required extension.

## Compressing geometry data

The codecs used by this extension can represent geometry exactly, replicating both vertex and index data without changes in contents or order. However, to get optimal compression, it's necessary to pre-process the data.

To get optimal compression, encoders should optimize vertex and index data for locality of reference. Specifically:

- Triangle order should be optimized to maximize the recency of previously encountered vertices; this is similar to optimizing meshes for vertex reuse aka post-transform cache in GPU hardware.
- Vertex order should be linearized in the order that vertices appear in the index stream to get optimal index compression

When index data is not available (e.g. point data sets) or represents topology with a lot of seams (e.g. each triangle has unique vertex indices because it specifies flat-shaded normal), encoders could additionally optimize vertex data for spatial locality, so that vertices close together in the vertex stream are close together in space.

Vertex data should be quantized using the appropriate representation; this extension cleanly interacts with KHR\_mesh\_quantization by compressing already quantized data.

Morph targets can be treated identically to other vertex attributes, as long as vertex order optimization is performed on all target streams at the same time. It is recommended to use quantized storage for morph target deltas, possibly with a narrower type than that used for baseline values.

When storing vertex data, mode 0 (attributes) should be used; for index data, mode 1 (triangles) or mode 2 (indices) should be used instead. Mode 1 only supports triangle list storage; indices of other topology types can be stored using mode 2. The use of triangle strip topology is not recommended since it's more efficient to store triangle lists using mode 1.

Using filter 1 (octahedral) for normal/tangent data may improve compression ratio further.

## Compressing animation data

To minimize the size of animation data, it is important to reduce the number of stored keyframes and reduce the size of each keyframe.

To reduce the number of keyframes, encoders can either selectively remove keyframes that don't contribute to the resulting movement, resulting in sparse input/output data, or resample the keyframes uniformly, resulting in uniformly dense data. Resampling can be beneficial since it means that all animation channels in the same animation can share the same input accessor, and provides a convenient quality vs size tradeoff, but it's up to the encoder to pick the optimal strategy.

Additionally it's important to identify tracks with the same output value and use a single keyframe for these.

To reduce the size of each keyframe, rotation data should be quantized using 16-bit normalized components; for additional compression, the use of filter 2 (quaternion) is recommended. Translation/scale data can be compressed using filter 3 (exponential) with the same exponent used for all three vector components.

After pre-processing, both input and output data should be stored using mode 0 (attributes).

# Appendix A: Bitstream

The following sections specify the format of the bitstream for compressed data for various modes.

## Mode 0: attributes

Attribute compression exploits similarity between consecutive elements of the buffer by encoding deltas. The deltas are stored for each separate byte which makes the codec more versatile since it can work with components of various sizes. Additionally, the elements are stored with bytes deinterleaved, which means that sequences of deltas are more easily compressible by some general purpose compressors that may run on the resulting data.

To facilitate efficient decompression, deinterleaving and delta encoding are performed on attribute blocks instead of on the entire buffer; within each block, elements are processed in groups of 16.

The encoded stream structure is as follows:

- Header byte, which must be equal to `0xa0`
- One or more attribute blocks, detailed below
- Tail block, which consists of a baseline element stored verbatim, padded to 32 bytes

Each attribute block stores a sequence of deltas, with the first element in the first block using the deltas from the baseline element stored in the tail block, and each subsequent element using the deltas from the previous element. The attribute block always stores an integer number of elements, with that number computed as follows:

```
count = min((8192 / byteStride) & ~15, 256)
```

The attribute block structure consists of `byteStride` blocks (one for each byte of the element) with the following structure:

- Header bits, 2 bits for each group of 16 elements (count/16 2-bit values), padded to a byte
- Delta blocks, with variable number of bytes stored for each group of 16 elements

Each group always contains 16 elements; when the number of elements that needs to be encoded isn't divisible by 16, it gets rounded up and the remaining elements are ignored after decoding.

Header bits are stored from least significant to most significant bit - header bits for 4 consecutive groups of 16 elements are packed in a byte together as follows:

```
bits0 | (bits1 << 2) | (bits2 << 4) | (bits3 << 6)
```

The header bits establish the delta encoding mode (0-3) for each group of 16 elements that follows:

- bits 0: All 16 byte deltas are 0; the size of the encoded block is 0 bytes
- bits 1: Deltas are using 2-bit sentinel encoding; the size of the encoded block is [4..20] bytes
- bits 2: Deltas are using 4-bit sentinel encoding; the size of the encoded block is [8..24] bytes
- bits 3: All 16 byte deltas are stored verbatim; the size of the encoded block is 16 bytes

Byte deltas are stored as zigzag-encoded differences between the byte values of the element and the byte values of the previous element in the same position; the zigzag encoding scheme works as follows:

```
encode(uint8_t v) = ((v & 0x80) != 0) ? ~(v << 1) : (v << 1)
decode(uint8_t v) = ((v & 1) != 0) ? ~(v >> 1) : (v >> 1)
```

When using the sentinel encoding, each delta is stored as a 2-bit or 4-bit value in a single 4-byte or 8-byte block, with deltas stored from most significant to least significant bit inside the byte (e.g. two 4-bit deltas are stored as `0xAB` where 0xA is the zigzag encoding of the first delta, and 0xB is the zigzag encoding of the second delta). The encoded value of the delta that has all bits set to 1 (corresponds to `3` for 2-bit deltas and `15` for 4-bit deltas, which, when decoded, represents `-2` and `-8` respectively) indicates that the real delta value is outside of the 2-bit or 4-bit range and is stored as a full byte after the bit deltas for this group. For example, for 4-bit deltas, the following byte sequence:

```
0x17 0x5F 0xF0 0xBC 0x77 0xA9 0x21 0x00 0x34 0xB5
```

Encodes 16 deltas, where the first 8 bytes of the sequence specifies the 4-bit deltas, and the last 2 bytes of the sequence specify the explicit delta values encoded for elements 3 and 4 in the sequence, so the decoded deltas (after zigzag decoding) look like

```
-1 -4 -3 26 -91 0 -6 6 -4 -4 5 -5 1 -1 0 0
```

Finally, note that the deltas are computed in 8-bit integer space with wrap-around two-complement arithmetic; for example, if the values of the first byte of two consecutive elements are 0x00 and 0xFF, the byte delta that is stored is `-1` (`1` after zigzag encoding).

## Mode 1: triangles

Triangle compression compresses triangle list indices by exploiting similarity between consecutive triangles. Given a triangle stream that has been optimized for locality, very often subsequent triangles share an edge with the recently encoded triangle. The encoder uses a few other techniques to try to encode most triangles in optimized triangle lists into a single byte.

The encoded stream structure is as follows:

- Header byte, which must be equal to `0xe1`
- Triangle codes, referred to as `code` below, with a single byte for each triangle
- Extra data which is necessary to decode triangles that don't fit into a single byte, referred to as `data` below
- Tail block, which consists of a 16-byte lookup table, referred to as `codeaux` below

There are two limitations on the structure of the 16-byte lookup table:

- The last two bytes must be 0
- The remaining bytes must not contain any nibbles equal to `0xf`.

During the decoding process, decoder maintains four variables:

- `next`: an integer referring to the expected next index (also known as high-watermark), starts at 0
- `last`: an integer referring to the last encoded index, starts at 0
- `edgefifo`: a 16-entry FIFO with two vertex indices in each entry; initial contents is undefined
- `vertexfifo`: a 16-entry FIFO with a vertex index in each entry; initial contents is undefined

To decode each triangle, the decoder needs to analyze the `code` byte, read additional bytes from `data` as necessary, and update the internal state correctly. The `code` byte encoding is optimized to reach a single byte per triangle in most common cases; the resulting data can often be compressed by a general purpose compressor running on the resulting .bin/.glb file.

When extra data is necessary to decode a triangle and it represents an index value, the decoder uses varint-7 encoding, which encodes an integer as one or more bytes, with the byte with the 0 most significant bit terminating the sequence:

```
0x7F => 0x7F
0x81 0x04 => 0x201
0xFF 0xA0 0x05 => 0x1FD005
```

Instead of using the raw index value, a zigzag-encoded 32-bit delta from `last` is used:

```
uint32_t decodeIndex(uint32_t v) {
	int32_t delta = (v & 1) != 0 ? ~(v >> 1) : (v >> 1);

	last += delta;
	return last;
}
```

The encoding for `code` is split into various cases, some of which are self-sufficient and some need to read extra data. The encoding is as follows:

- `0xX0`, where `X < 0xf`: Encodes a recently encountered edge and a `next` vertex.
The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is equal to `next`.
Vertex c is pushed to vertex FIFO; edges (c, b) and (a, c) are pushed to edge FIFO (in this order).
`next` is incremented, and the triangle (a, b, c) is emitted to the output.

- `0xXY`, where `X < 0xf` and `0 < Y < 0xd`: Encodes a recently encountered edge and a recently encountered vertex.
The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is read from the vertex FIFO at index Y (where 0 is the most recently added vertex; note that 0 is never actually read here, since `Y > 0`).
Edges (c, b) and (a, c) are pushed to edge FIFO (in this order).
The triangle (a, b, c) is emitted to the output.

- `0xXd` or `0xXe`, where `X < 0xf`: Encodes a recently encountered edge and a vertex that's adjacent to `last`.
The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is equal to `last-1` for `0xXd` and `last+1` for `0xXe`.
Vertex c is pushed to vertex FIFO; edges (c, b) and (a, c) are pushed to edge FIFO (in this order).
`last` is set to `c` (effectively decrementing or incrementing it accordingly).
The triangle (a, b, c) is emitted to the output.

- `0xXf`, where `X < 0xf`: Encodes a recently encountered edge and a free-standing vertex encoded explicitly.
The edge (a, b) is read from the edge FIFO at index X (where 0 is the most recently added edge).
The third index, `c`, is decoded using `decodeIndex` by reading extra bytes from `data` (and also updates `last`).
Vertex c is pushed to vertex FIFO; edges (c, b) and (a, c) are pushed to edge FIFO (in this order).
The triangle (a, b, c) is emitted to the output.

- `0xfY`, where `Y < 0xe`: Encodes three indices using `codeaux` table lookup and vertex FIFO.
The table `codeaux` is used to read the element Y; let's assume that results in `0xZW`.
The first index, `a`, is equal to `next`; `next` is incremented to decode b/c correctly.
The second index, `b`, is read from vertex FIFO at index `Z-1` (where 0 is the most recently added vertex) if `Z > 0`.
If `Z == 0`, the second index is equal to `next` and `next` is incremented to decode c correctly.
The third index, `c`, is read from vertex FIFO at index `W-1` (where 0 is the most recently added vertex) if `W > 0`.
If `W == 0`, the third index is equal to `next` and `next` is incremented.
Note that in the process `next` is incremented from 1 to 3 times depending on values of Z/W.
Edges (b, a), (c, b) and (a, c) are pushed to edge FIFO (in this order).
Vertex a is pushed to vertex FIFO.
Vertex b is pushed to vertex FIFO if `Z == 0`.
Vertex c is pushed to vertex FIFO if `W == 0`.
The triangle (a, b, c) is emitted to the output.

- `0xfe` or `0xff`: Encodes three indices explicitly.
This requires an extra triangle code that is read from `data`; let's assume that results in `0xZW`.
If `0xZW` == `0x00`, then `next` is reset to 0. This is a special mechanism used to restart the `next` sequence which is useful for concatenating independent triangle streams. This must be done before further processing.
The first index, `a`, is equal to `next` for `0xfe` encoding (`next` is then incremented), or is read using `decodeIndex` by reading extra bytes from `data` (and also updates `last`).
The second index, `b`, is equal to `next` if `Z == 0` (`next` is then incremented), is read from vertex FIFO at index `Z-1` (where 0 is the most recently added vertex) if `Z < 0xf`, or is read using `decodeIndex` by reading extra bytes from `data` (and also updates `last`) if `Z == 0xf`.
The third index, `c`, is equal to `next` if `W == 0` (`next` is then incremented), is read from vertex FIFO at index `W-1` (where 0 is the most recently added vertex) if `W < 0xf`, or is read using `decodeIndex` by reading extra bytes from `data` (and also updates `last`) if `W == 0xf`.
Edges (b, a), (c, b) and (a, c) are pushed to edge FIFO (in this order).
Vertex a is pushed to vertex FIFO.
Vertex b is pushed to vertex FIFO if `Z == 0` or `Z == 0xf`.
Vertex c is pushed to vertex FIFO if `W == 0` or `W == 0xf`.
The triangle (a, b, c) is emitted to the output.

At the end of the decoding, `data` is expected to be fully read by all the triangle codes and not contain any extra bytes.

## Mode 2: indices

Index compression exploits similarity between consecutive indices. Note that, unlike the triangle index compression (mode 1), this mode doesn't assume a specific topology and as such is less efficient in terms of the resulting size. However, unlike mode 1, this mode can be used to compress triangle strips, line lists and other types of mesh index data, and can additionally be used to compress non-mesh index data such as sparse indices for accessors.

The encoded stream structure is as follows:

- Header byte, which must be equal to `0xd1`
- A sequence of index deltas, with encoding specified below
- Tail block, which consists of 4 bytes that are reserved and should be set to 0

Instead of simply encoding deltas vs the previous index, the decoder tracks *two* baseline index values, that start at 0. Each delta is specified in relation to one of these values and updates it so that the next delta that references the same baseline uses the encoded index value as a reference. This encoding is more efficient at handling some types of bimodal sequences where two independent monotonic sequences are spliced together, which can occur for some common cases of triangle strips or line lists.

To specify the index delta, the varint-7 encoding scheme is used, which encodes an integer as one or more bytes, with the byte with the 0 most significant bit terminating the sequence:

```
0x7F => 0x7F
0x81 0x04 => 0x201
0xFF 0xA0 0x05 => 0x1FD005
```

When decoding the deltas, the 32-bit value is read using the varint encoding. The least significant bit of the value indicates one of the baseline values; the remaining bits specify a zigzag-encoded signed delta and can be decoded as follows:

```
uint32_t decode(uint32_t v) {
	int32_t baseline = v & 1;
	int32_t delta = (v & 2) != 0 ? ~(v >> 2) : (v >> 2);

	last[baseline] += delta;
	return last[baseline];
}
```

It's up to the encoder to determine the optimal selection of the baseline for each index; this encoding scheme can be used to do basic delta encoding (with baseline bit always set to 0) as well as more complex bimodal encodings.

Note that the zigzag-encoded delta must fit in a 31-bit integer; as such, deltas are limited to [-2^30..2^30-1].

# Appendix B: Filters

Filters are functions that transform each encoded attribute. For each filter, this document specifies the transformation used for decoding the data; it's up to the encoder to pick the parameters of the encoding for each element to balance quality and precision.

For performance reasons the results of the decoding process are specified to one unit in last place (ULP) in terms of the decoded data, e.g. if a filter results in a 16-bit signed normalized integer, decoding may produce results within 1/32767 of specifed value.

## Filter 1: octahedral

Octahedral filter allows to encode unit length 3D vectors (normals/tangents) using octahedral encoding, which results in a more optimal quality vs precision tradeoff compared to storing raw components.

The input to the filter is four 8-bit or 16-bit components, where the first two specify the X and Y components in octahedral encoding encoded as signed normalized K-bit integers, the third component explicitly encodes 1.0 as a K-bit integer, and the last component may contain arbitrary data (which is useful for tangents).

The output of the filter is three decoded unit vector components, stored as 8-bit or 16-bit normalized integers, and the last input component verbatim.

```
void decode(intN_t input[4], intN_t output[4]) {
	// input[2] encodes a K-bit representation of 1.0
	float32_t one = input[3];

	float32_t x = input[0] / one;
	float32_t y = input[1] / one;
	float32_t z = 1.0 - abs(x) - abs(y);

	// octahedral fixup for negative hemisphere
	float32_t t = min(z, 0.0);

	x += copysign(t, x);
	y += copysign(t, y);

	// renormalize (x, y, z)
	float32_t l = sqrt(x * x + y * y + z * z);

	x /= l;
	y /= l;
	z /= l;

	output[0] = round(x * INTN_MAX);
	output[1] = round(y * INTN_MAX);
	output[2] = round(z * INTN_MAX);
	output[3] = input[3];
}
```

## Filter 2: quaternion

Quaternion filter allows to encode unit length quaternions using normalized 16-bit integers for all components, but allows control over the precision used for the components and provides better quality compared to naively encoding each component one by one.

The input to the filter is three quaternion components, excluding the component with the largest magnitude, encoded as signed normalized K-bit integers (4 <= K <= 16), and an index of the largest component that is omitted in the encoding. The largest component is assumed to always be positive (which is possible due to quaternion double-cover). To allow per-element control over K, the last input element explicitly encodes 1.0 as a K-bit integer, except for the least significant 2 bits that store the index of the maximum component.

The output of the filter is four decoded quaternion components, stored as 16-bit normalized integers.

After eliminating the maximum component, the maximum magnitude of the remaining components is 1/sqrt(2). Because of this the input components store the original component value scaled by sqrt(2.0) to increase precision.

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

Exponential filter allows to encode floating point values with a range close to the full range of a 32-bit floating point value, but allows more control over the exponent/mantissa to trade quality for precision, and has a bit structure that is more optimally aligned to the byte boundary.

The input to the filter is a sequence of 32-bit little endian integers, with the most significant 8 bits specifying a (signed) exponent value, and the remaining 24 bits specifying a (signed) mantissa value. The integers are stored in two-complement format.

The result of the filter is 2^e * m:

```
float32_t decode(int32_t input) {
	int32_t e = input >> 24;
	int32_t m = (input << 8) >> 8;
	return pow(2.0, e) * m;
}
```

The valid range of `e` is [-100, +100], which facilitates performant implementations.
