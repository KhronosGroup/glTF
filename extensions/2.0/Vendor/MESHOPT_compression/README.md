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

This is beneficial for typical Web delivery scenarios, where all files are usually using gzip compression - instead of completely replacing it, the codecs here augment it, while still reducing the size (which is valuable to optimize delivery size when gzip compression isn't available, and additionally reduces the performance impact of gzip decompression which is typically *slower* than decoders proposed here).

## Specifying compressed views

As explained in the overview, this extension operates on bufferViews. This allows the loaders to directly decompress data into GPU memory and minimizes the JSON size impact of specifying compressed data. To specify the compressed representation, `MESHOPT_compression` extension section overrides the source buffer index as well as specifying the buffer parameters and a compression mode (detailed later in the specification):

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

In this example, the uncompressed buffer contents is stored in buffer 1 (this can be used by loaders that don't implement this extension). The compressed data is stored in a separate buffer, specifying a separate byte range (with compressed data). Note that for compressors to work, they need to know the compression `mode`, and additionally the layout of the encoded data - `count` elements with `byteStride` bytes each. This data is specified in the extension JSON; while in some cases `byteStride` is available on the parent `bufferView` declaration, JSON schema prohibits specifying this for some types of storage such as index data.

For the extension JSON to be valid, the following must hold:

- When parent `bufferView` has `byteStride` defined, it matches `byteStride` in the extension JSON
- Buffer view length is equal to `byteStride` times `count`
- When `mode` is 0 (attributes), `byteStride` must be divisible by 4
- When `mode` is 1 (indices), `byteStride` must be equal to 2 or 4

The type of compressed data must match the bitstream specification (note that each `mode` specifies a different bitstream format).

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

When index data is not available (e.g. point data sets) or represents topology with a lot of seams (e.g. each triangle has unique vertex indices because it specifies flat-shaded normal), encoders should additionally optimize vertex data for spatial locality, so that vertices close together in the vertex stream are close together in space.

Vertex data should be quantized using the appropriate representation; this extension cleanly interacts with KHR\_quantized\_geometry by compressing already quantized data.

Morph targets can be treated identically to other vertex attributes, as long as vertex order optimization is performed on all target streams at the same time. It is recommended to use quantized storage for morph target deltas, possibly with a narrower type than that used for baseline values.

When storing vertex data, mode 0 (attributes) should be used; for index data, mode 1 (indices) should be used instead. Mode 1 only supports triangle list storage; indices of other topology types can be stored uncompressed or using mode 0 (attributes). The use of triangle strip topology is not recommended since it's more efficient to store triangle lists using mode 1.

TODO: normal & tangent unit filters

## Compressing animation data

To minimize the size of animation data, it is recommended that encoders resample animations before compression. This serves two purposes:

- After resampling, all animation channels can share the same input accessor that carries the time data once
- Varying sample frequency allows one to trade off quality of animations for size

After resampling, rotation data should be additionally quantized for maximum efficiency - using 16-bit components should provide enough precision to store most rotations.

After pre-processing, both input and output data should be stored using mode 0 (attributes).

TODO: quaternion unit filter

## Bitstream mode 0 - attributes

TODO

## Bitstream mode 1 - indices

TODO
