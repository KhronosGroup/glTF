# WEB3D_quantized_attributes 


## Contributors

* Max Limper, Fraunhofer IGD, [@mlimper_cg](https://twitter.com/mlimper_cg)
* Miguel Sousa, Fraunhofer IGD, [@mfportela](https://twitter.com/mfportela)
* Maik Thöner, Fraunhofer IGD, [@mthoener](https://twitter.com/mthoener)
* Robert Taglang, Cesium, [@lasalvavida](https://github.com/lasalvavida)


## Dependencies

Written against the glTF draft 1.0 spec.


## Overview

Vertex attributes, as they are used to render 3D meshes, usually come with 32-bit floating-point precision.
However, for many applications, this precision is much more than is required.
Therefore, a common optimization to save bandwidth and memory is to use vertex data in a less precise fixed-point format, for example, using 16 bits.
In many cases, this approach does not introduce any visible quality loss in the rendered image, but it significantly speeds up loading time.
Moreover, since less memory is needed, it becomes possible to use the same amount of memory for even larger models.

This extension provides a straightforward method for compression of vertex attributes, using a fixed-point data format with reduced precision.
The process of resampling floating-point data to a grid with fixed spacing is commonly referred to as _quantization_, which is why this extension is entitled WEB3D_quantized_attributes.


### Quantization Scheme and Decode Matrix

Within the following, the typical process of quantizing mesh attribute data is shown.
It is not the only possible way to perform data quantization, but it should illustrate how the method works in general, and why decoding can finally be executed using a matrix.
The [paper](#lee-10-compression) "Mesh Geometry Compression for Mobile Graphics" by Lee et al. describes this method more in detail, and it also provides a practical analysis of results.


#### <a name="choosing-a-range-for-normalization"></a>Choosing a Range for Normalization

To quantize a given attribute, the first step is to compute a range.
This step is needed for normalization on the encoder side, and later for de-normalization on the decoder side.
There are various possibilities to pick reference values for normalization.
For 3D vertex positions, for example, good quality is achieved by using the 3D bounding box of the model.
In the case of glTF writers, this information is often already available, since glTF supports the optional `min` and `max` accessor properties.

Another way, for example, is to use the same bounding box, tiled across the scene, to the quantize data of different meshes.
Although this means that more information gets lost than for tight fitting, it allows to efficiently hide cracks that could otherwise occur at borders between different meshes.
For reasons like this, it is important to note at this point that the bounding box of the model is not necessarily always identical to one used for quantization of the vertex positions.


#### Encoding Attribute Values

Using the normalization method of choice, mapping all values to be compressed to a floating-point unit range `[0, 1]^n` is the first step of the compression algorithm.
Here, `n` is the number of components (for example, 3 for 3D vertex positions).
This mapping can be achieved by first subtracting by the minimum normalization value from a given value and then dividing by the range of the respective component.

From the floating-point unit range, values are then linearly mapped to the unsigned integer range `[0, 2^p - 1]`, where p is the precision in bits.
For example, for 16-bit precision, we will map all values to the integer range `[0, 65535]`.
This is just a multiplication operation, with an additional conversion from floating-point to integer format.
During this last step, the crucial loss of precision takes place.


#### Decoding Attribute Values

Uncompressing values from the quantized unsigned integer range back to their original range is straightforward.
All that has to be done is to perform the inverse operations of the encoding algorithm:
First, values are linearly mapped from the integer range back to the floating-point range `[0, 1]`.
This can be achieved by converting from integer format to floating point and then performing a division operation.
Second, the resulting values are multiplied by the range used for normalization.
Finally, the minimum normalization value is added to the result, in order to achieve the correct offset.

Since this whole decode operation consists of a division, a multiplication and an addition, it can be expressed as a multiplication with a homogeneous matrix.
For example, when decoding 3D vertex positions, a `4x4` matrix can be multiplied with a quantized 3D vector to obtain the reconstructed, uncompressed vertex position.
In glTF, attributes can be `SCALAR`, `VEC2`, `VEC3`, or `VEC4`, making the possible matrix sizes `2x2`, `3x3`, `4x4`, and `5x5`.

To represent the decode matrix, this extension introduces one additional property of the `accessor`, which is entitled `decodeMatrix`.
This property must be an array of `4`, `9`, `16`, or `25` values, with a size matching the homogeneous decode matrix for the respective accessor.
For example, quantized 3D vertex positions will require a `4x4` matrix with `16 `entries, while quantized 2D texture coordinates will require a `3x3` matrix with `9` entries.
(Note that for `VEC4` attributes there is no `mat5` data type in GLSL, requiring the implementation to split it up into a `mat4` scale and `vec4` translate.)

A great advantage of this method is that runtime engines can use the GPU to efficiently perform decoding of each vertex during rendering.
One way to do this would be to pass the matrix to a vertex shader and perform the decoding before multiplying with a model-view matrix.
However, it is even possible to multiply the decode matrix with the model-view matrix in advance.
This way, no special adaptions are necessary inside the shader in order to render compressed data.


## glTF Schema Updates

The new `decodeMatrix` property introduced by this extension belongs to the accessor of the respective attribute.

As already discussed in section [Choosing a Range for Normalization](#choosing-a-range-for-normalization), the range chosen for normalization and de-normalization of vertex positions, for example, must not be identical with their bounding box.
To obtain information such as bounding boxes, glTF accessors already contain the optional `min` and `max` values.
However, this values might not have been provided, and even if they are provided, they apply to the encoded, compressed data.
One could obtain original values by multiplying the decode matrix with given `min` and `max` values, but it is desirable to save the overhead of matrix multiplication.
Therefore, this extension introduces two properties, `decodedMin` and `decodedMax`, which contain the extreme values for the decoded accessor.
This can help simplify the client implementation where `min` and `max` are used to compute bounding volumes.

The following example illustrates two different, valid accessors:

```javascript
"extensionsUsed" : [
    "WEB3D_quantized_attributes"
]
// ...
"an_accessor" : {
    // ... standard glTF properties
    "extensions" : {
        "WEB3D_quantized_attributes" : {
            decodeMatrix : [1, 0, 0, 0, ...],
            decodedMin : [1,2,3],
            decodedMax : [3,4,5]
        }
    }
},
"another_accessor" : {
    // ... standard glTF properties
}
```

The first accessor from the example makes use of the extension, having the mandatory `decodeMatrix`, `decodeMin`, and `decodeMax` properties, which can be used to decompress the quantized data.
The second one does not make use of the extension, meaning that it uses its data directly from the `bufferView`, as usual, without decompressing it.


## JSON Schema

### WEB3D_quantized_attributes accessor extension

**Properties**

|   |Type|Description|Required|
|---|----|-----------|--------|
|**decodeMatrix**|`number[4,9,16,25]`|A homogenous floating-point transformation matrix stored in column-major order. This matrix is used to decode the compressed, quantized data of this accessor.| :white_check_mark: Yes|
|**decodedMax**|`number[1-4]`|Maximum decoded value of each component in this attribute.|Yes|
|**decodedMin**|`number[1-4]`|Minimum decoded value of each component in this attribute.|Yes|

Additional properties are not allowed.

#### WEB3D_quantized_attributes accessor extension.decodeMatrix :white_check_mark: 

A homogenous floating-point transformation matrix stored in column-major order. This matrix is used to decode the compressed, quantized data of this accessor.

* **Type**: `number[4,9,16,25]`
* **Required**: Yes

#### WEB3D_quantized_attributes accessor extension.decodedMax

Maximum decoded value of each component in this attribute.

* **Type**: `number[1-4]`
* **Required**: Yes

#### WEB3D_quantized_attributes accessor extension.decodedMin

Minimum decoded value of each component in this attribute.

* **Type**: `number[1-4]`
* **Required**: Yes

Also, see the schema file:

* [accessor](schema/WEB3D_quantized_attributes.accessor.schema.json) `WEB3D_quantized_attributes` extensions object

## Known Implementations

* The [SRC](http://x3dom.org/src/) implementation by Fraunhofer IGD features a similar technology, which has served as a basis for this proposal.
Note, however, that SRC uses two attributes for decoding (`decodeOffset` and `decodeScale`), instead of using a matrix.
* Cesium ([code](https://github.com/AnalyticalGraphicsInc/cesium/blob/master/Source/Scene/Model.js))
* gltf-pipeline ([code](https://github.com/AnalyticalGraphicsInc/gltf-pipeline/blob/master/lib/quantizeAttributes.js))

## Resources
* <a name="lee-10-compression"></a>Lee, J., Choe, S., and Lee, S. 2010. _Mesh geometry compression for mobile graphics._ In _Proc. CCNC_, 301–305 [paper](http://cg.postech.ac.kr/research/mesh_comp_mobile/mesh_comp_mobile_conference.pdf)
* [SRC project](http://x3dom.org/src/) (paper, background, basic writer)
* SRC writer ([code](http://x3dom.org/src/files/src_writer_source.zip))
* [Using Quantization with 3D Models](http://cesiumjs.org/2016/08/08/Cesium-web3d-quantized-attributes/) by Rob Taglang. August 2016
* [mesh-quantization-example](https://github.com/tsherif/mesh-quantization-example) by Tarek Sherif. A minimal example of vertex quantization.

# Acknowledgments
* Alexey Knyazev, [@lexaknyazev](https://github.com/lexaknyazev)
