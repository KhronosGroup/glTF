# EXT\_accessor\_additional\_types

## Contributors

- Sean, [@spnda](https://github.com/spnda)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Similarly to the `KHR_mesh_quantization` extension, this extension offers additional accessor data types for use-cases where the traditional values either offer too much precision, and are therefore wasting memory, or offer too little precision.

This extension expands the list of component types to additionally support 16-bit half-precision floats, which are commonly used for UV texture coordinates, and for larger data types, such as 64-bit double precision floats, and 64-bit integers.

This extension adds an exhaustive list of all currently supportable types, so that other extensions can make use of them for different use-cases. Additionally, this extension also allows 16-bit half-precision floats as an additional type for UV texture coordinates, as they are commonly used to optimise memory usage, since they often have next to no effect on quality, and is supported by most modern GPUs and graphics APIs.

Assets that make use of this extension must mark it as required, as it cannot be optional, since there is no method of switching component types based on supported extensions.

## Extending Accessor Data Types

When the `EXT_accessor_additional_types` extension is supported, the following **extra** accessor data types are allowed for usage in accessors in addition to the types defined in [Section 3.6.2.2](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types).

|componentType|Data type|Signed|Bits|
|-------------|---------|------|----|
|5124|signed int|Signed, two’s complement|32|
|5130|double|Signed|64|
|5131|half float|Signed|16|
|5135|unsigned long|Unsigned|64|

The new floating point accessor data types must use IEEE754 double-precision format (binary64) and half-precision format (binary16), respectively.

## Extending Mesh Attributes

When `EXT_accessor_additional_types` extension is supported, the following **extra** types are allowed for storing mesh attributes in addition to the types defined in [Section 3.7.2.1](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#meshes-overview).

|Name|Accessor Type(s)|Component Type(s)|
|----|----------------|-----------------|
|`TEXCOORD_n`|VEC2|_half float_|

## Extending Morph Target Attributes

When `EXT_accessor_additional_types` extension is supported, the following **extra** types are allowed for storing morph target attributes in addition to the types defined in [Section 3.7.2.2](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#morph-targets).

|Name|Accessor Type(s)|Component Type(s)|
|----|----------------|-----------------|
|`TEXCOORD_n`|VEC2|_half float_|
