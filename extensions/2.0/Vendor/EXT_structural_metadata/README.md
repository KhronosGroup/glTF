# EXT_structural_metadata

## Contributors

* Peter Gagliardi, Cesium
* Sean Lilley, Cesium
* Sam Suhag, Cesium
* Don McCurdy, Independent
* Marco Hutter, Cesium
* Bao Tran, Cesium
* Samuel Vargas, Cesium
* Patrick Cozzi, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.0 specification.

## Overview

* **Version 0.0.0** February 2022

  Split the `EXT_mesh_features` extension into one that only defines the concept of feature IDs, and a separate `EXT_structural_metadata` extension that allows associating the features with metadata, including the following changes:

  * Updates to class properties
    * `type` is required and must be one of the following: `SCALAR`, `VEC2`, `VEC3`, `VEC4`, `MAT2`, `MAT3`, `MAT4`, `STRING`, `BOOLEAN`, `ENUM`
    * `componentType` is required for scalar, vector, and matrix types and must be one of the following: `INT8`, `UINT8`, `INT16`, `UINT16`, `INT32`, `UINT32`, `INT64`, `UINT64`, `FLOAT32`, `FLOAT64`
    * Arrays are now distinct from the type system
      * Removed `ARRAY` type. Removed `componentCount`. Added `count` and `hasFixedCount`.
      * To indicate that a property is a single element, `count` must be 1 and `hasFixedCount` must be true. Since these are the default values `count` and `hasFixedCount` may be omitted.
      * To indicate that a property is a fixed-length array, `count` must be greater than 1 and `hasFixedCount` must be true. Since true is the default value `hasFixedCount` may be omitted.
      * To indicate that a property is a variable-length array, `hasFixedCount` must be false and `count` may be omitted.
      * Arrays of vectors and matrices are now supported
    * Added `offset` and `scale` which are used to transform property values into a different range
    * Added back `default`
  * Updates to property tables
    * Renamed `bufferView` to `values`. `values` now points to an accessor.
      * Property values may be padded or interleaved via the accessor's bufferView's `byteStride`
    * Renamed `stringOffsetBufferView` to `stringOffsets`. `stringOffsets` now points to an accessor. Removed `stringOffsetType`.
    * Renamed `arrayOffsetBufferView` to `arrayOffsets`. `arrayOffsets` now points to an accessor. Removed `arrayOffsetType`.
    * Added `offset` and `scale` which are used to transform property values into a different range. When present, these override the class property's `offset` and `scale`.
  * Updates to property textures
    * Properties are no longer required to be packed into the same texture
    * Each item in the `properties` dictionary is now a `textureInfo` object
    * Added `offset` and `scale` which are used to transform property values into a different range. When present, these override the class property's `offset` and `scale`.
  * Added property mappings
    * TODO