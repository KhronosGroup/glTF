# KHR\_quantized\_geometry

## Contributors

* Arseny Kapoulkine, [@zeuxcg](https://twitter.com/zeuxcg)
* Alexey Knyazev, [@lexaknyazev](https://github.com/lexaknyazev)

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec. Depends on `KHR_texture_transform` for texture coordinate dequantization.

## Overview

Vertex attributes are usually stored using `FLOAT` component type. However, this can result in excess precision and increased memory consumption and transmission size, as well as reduced rendering performance.

This extension expands the set of allowed component types for geometry storage to provide a memory/precision tradeoff - depending on the application needs, 16-bit or 8-bit storage can be sufficient.

Using 16-bit or 8-bit storage typically requires transforming the original floating point values to fit a uniform 3D or 2D grid; the process is commonly known as quantization.

To simplify implementation requirements, the extension relies on existing ways to specify geometry transformation instead of adding special dequantization transforms to the schema.

As an example, a static PBR-ready mesh typically requires `POSITION` (12 bytes), `TEXCOORD` (8 bytes), `NORMAL` (12 bytes) and `TANGENT` (16 bytes) for each vertex, for a total of 48 bytes. With this extension, it is possible to use `SHORT` to store position and texture coordinate data (8 and 4 bytes, respectively) and `BYTE` to store normal and tangent data (4 bytes each), for a total of 20 bytes per vertex with often negligible quality impact.

Because the extension does not provide a way to specify both `FLOAT` and quantized versions of the data, files that use the extension should specify it in `extensionsRequired` list - the extension is not optional.

## Extending Mesh Attributes

When `KHR_quantized_geometry` extension is supported, the set of types used for storing geometry attributes is expanded according to the table below.

|Name|Accessor Type(s)|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`POSITION`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|XYZ vertex positions|
|`NORMAL`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized|Normalized XYZ vertex normals|
|`TANGENT`|`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized|XYZW vertex tangents where the *w* component is a sign value (-1 or +1) indicating handedness of the tangent basis|
|`TEXCOORD_n`|`"VEC2"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5121`&nbsp;(UNSIGNED_BYTE)<br>`5121`&nbsp;(UNSIGNED_BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)<br>`5122`&nbsp;(SHORT)&nbsp;normalized<br>`5123`&nbsp;(UNSIGNED_SHORT)<br>`5123`&nbsp;(UNSIGNED_SHORT)&nbsp;normalized|UV texture coordinates for set #n|

Note that to comply with alignment rules for accessors, each element needs to be aligned to 4-byte boundaries; for example, a `BYTE` normal is expected to have a stride of 4, not 3.

For `POSITION` and `TEXCOORD` attributes, the application is free to choose normalized or unnormalized storage, as well as signed or unsigned. When normalized storage is used, often the data doesn't have to be dequantized (which eliminates the need for a dequantization transform); however, if the data is not in `[0..1]` or `[-1..1]` range, using integer storage can reduce precision loss as standard glTF normalization factors such as `1/255` and `1/65535` are not representable exactly as floating-point numbers.

> **Implementation Note:** As quantization may introduce a non-negligible error, quantized normal and tangent vectors are typically not exactly unit length. Applications are expected to normalize the vectors before using them in lighting equations; this typically can be done after transforming them using the normal matrix. Even if quantization is not used, normal matrix can contain scale/skew so normalization is typically required anyway.

## Extending Morph Target Attributes

When `KHR_quantized_geometry` extension is supported, the set of types used for storing morph target attributes is expanded according to the table below.

|Name|Accessor Type(s)|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`POSITION`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)<br>`5122`&nbsp;(SHORT)&nbsp;normalized|XYZ vertex position displacements|
|`NORMAL`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized|XYZ vertex normal displacements|
|`TANGENT`|`"VEC3"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized|XYZ vertex tangent displacements|

Note that to comply with alignment rules for accessors, `"VEC3"` accessors need to be aligned to 4-byte boundaries; for example, a `BYTE` normal is expected to have a stride of 4, not 3.

For simplicity the specification assumes that morph target displacements are signed; for `NORMAL` and `TANGENT`, models with very large displacements that do not fit into `[-1..1]` range need to use `FLOAT` storage.

## Decoding Quantized Data

Depending on the attribute values and component types used, the attributes may need to be transformed into their original range for display. Instead of relying on separate dequantization transform, this extension relies on existing methods to specify transformation.

For `POSITION` attribute, the transform can be specified in one of two ways:

- For non-skinned meshes, the dequantization transform (which typically consists of scale and offset) can be encoded into `node` transformation - this can require adding an extra leaf `node` so that animations that affect the mesh transformation are not disturbed.

- For skinned meshes, the `node` hierarchy does not affect the transform of the `mesh` directly - however, dequantization transform can be encoded into `inverseBindMatrices` for the `skin` used for the mesh.

In both cases, to preserve the direction of normal/tangent vectors, it is recommended that the quantization scale specified in the transform is uniform across X/Y/Z axes.

For `TEXCOORD` attribute, the transform can be specified using the `offset`/`scale` supplied by `KHR_texture_transform` extension.

For morph target data, it's expected that deltas are quantized using the same transform as the base positions. Extra care must be taken to ensure that the quantization accommodates for large displacements, if present.

## Encoding Quantized Data

It is up to the encoder to determine the optimal range and format, along with the quantization transform, to maximize the output quality. If necessary, different meshes can use different component types for different attributes - this extension does not require that a single component type is used consistently for all attributes of the same type.

> **Implementation Note:** It is possible to determine the range of the attribute for each mesh individually, and pick the 8-bit or 16-bit storage format accordingly; this minimizes the error for each individual mesh, but may prevent sharing for some scene elements (such as reusing a material across multiple meshes with different texture coordinate ranges) and may result in the same vertex being quantized differently, causing visible gaps.

> **Implementation Note:** It is also possible to determine the range of all attributes of all meshes in the scene and use a shared dequantization transform; this can eliminate the gaps between different meshes but can increase the quantization error.

Implementations should assume following equations are used to get corresponding floating-point value `f` from a normalized integer `c` and should use the specified equations to encode floating-point values to integers after range normalization:

|`accessor.componentType`|int-to-float|float-to-int|
|-----------------------------|--------|----------------|
| `5120`&nbsp;(BYTE)          |`f = max(c / 127.0, -1.0)`|`c = round(f * 127.0)`|
| `5121`&nbsp;(UNSIGNED_BYTE) |`f = c / 255.0`|`c = round(f * 255.0)`|
| `5122`&nbsp;(SHORT)         |`f = max(c / 32767.0, -1.0)`|`c = round(f * 32767.0)`|
| `5123`&nbsp;(UNSIGNED_SHORT)|`f = c / 65535.0`|`c = round(f * 65535.0)`|

> **Implementation Note:** Due to OpenGL ES 2.0 / WebGL 1.0 restrictions, some implementations may decode signed normalized integers to floating-point values differently. While the difference is unlikely to be significant for normal/tangent data, implementations may want to use unsigned normalized or signed non-normalized formats to avoid the discrepancy in position data.
