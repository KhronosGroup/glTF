# EXT_quantized_attributes 

## Contributors

* Max Limper, Fraunhofer IGD, [@mlimper_cg](https://twitter.com/mlimper_cg)
* Miguel Sousa, Fraunhofer IGD, [@mfportela](https://twitter.com/mfportela)
* Maik Thöner, Fraunhofer IGD, [@mthoener](https://twitter.com/mthoener)


## Dependencies

Written against the glTF draft 1.0 spec.

## Overview

Vertex Attributes, as they are used to render 3D meshes, usually come with 32 bit floating-point precision.
However, for many applications, this precision is much more than what would actually be required.
Therefore, a common optimization to save bandwidth and memory is to use vertex data in a less precise fixed-point format, for example using 16 bits.
In many cases, this approach does not introduce any visible loss of quality in the rendered image, but it significantly speeds up loading time.
Moreover, since less memory is needed, it becomes possible to use the same amount of memory for even larger models.

This extension provides a straightforward method for compression of vertex attributes, using a fixed-point data format with reduced precision.
The process of resampling floating-point data to a grid with fixed spacing is commonly referred to as _quantization_, which is why this extension is entitled EXT_quantized_attributes.

### Quantization Scheme and Decode Matrix

TODO

## glTF Schema Updates

TODO

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.

## Known Implementations

The [SRC](http://x3dom.org/src/) implementation by Fraunhofer IGD features a similar technology, which has served as a basis for this proposal.
Note, however, that SRC uses two attributes for decoding (_decodeOffset_ and _decodeScale_), instead of using a matrix.

## Resources

* [SRC project](http://x3dom.org/src/) (paper, background, basic writer)
* SRC writer ([code](http://x3dom.org/src/files/src_writer_source.zip))
