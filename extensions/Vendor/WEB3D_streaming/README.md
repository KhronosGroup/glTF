# WEB3D_streaming


## Contributors

* Max Limper, Fraunhofer IGD, [@mlimper_cg](https://twitter.com/mlimper_cg)
* Maik Thöner, Fraunhofer IGD, [@mthoener](https://twitter.com/mthoener)


## Dependencies

Written against the glTF draft 1.0 spec.

Required extensions:
* EXT_binary_glTF


## Overview

The glTF format is able to be used for progressive streaming of mesh data.
The reason for this is that, with today's browser technology, basically any buffer can be progressively downloaded, using XHR and streaming APIs.
However, in practice this only makes sense for non-indexed mesh data.
A buffer of vertex positions, for example, can be progressively retrieved, and partial results can already be displayed.
If the corresponding buffers for vertex normals, for example, should also be progressively retrieved, a second download, performed in parallel, is necessary.
Moreover, to make use of a partial result, the intermediate results of both downloads need to match with each other.
This drawback of having to parallelize (and align) downloads for a progressive retrieval of mesh data also potentially has an impact on application performance, since it is not possible to send all data in a single file (as it can be done with binary_glTF, for example).

This extension introduces the concept of buffer `chunks`. By interleaving content for transmission, buffers can be retrieved progressively, using a limited amount of steps for progression.

**Figure 1**: Binary glTF layout.

![](img/streaming.jpg)

TODO


## glTF Schema Updates

TODO

 
### JSON Schema

TODO


## Known Implementations

The [SRC](http://x3dom.org/src/) implementation by Fraunhofer IGD has served as a basis for this proposal.


## Resources
* [SRC project](http://x3dom.org/src/) (paper, background, basic writer)
* SRC writer ([code](http://x3dom.org/src/files/src_writer_source.zip))
