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

TODO

## Mode 1: triangles

TODO

## Mode 2: indices

TODO

# Appendix B: Filters

Filters are functions that transform each encoded attribute. For each filter, this document specifies the transformation used for decoding the data; it's up to the encoder to pick the parameters of the encoding for each element to balance quality and precision.

## Filter 1: octahedral

TODO

## Filter 2: quaternion

TODO

## Filter 3: exponential

TODO
