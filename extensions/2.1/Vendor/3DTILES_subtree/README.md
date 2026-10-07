<!--
SPDX-FileCopyrightText: 2026 Bentley Systems, Incorporated

SPDX-License-Identifier: CC-BY-4.0
-->

# 3DTILES\_subtree

## Contributors

- Sean Lilley, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.1 spec.

Assets with the `3DTILES_subtree` extension are referenced by [3DTILES_implicit_tiling](../3DTILES_implicit_tiling/README.md).

## Required

This extension is required, meaning it **MUST** be placed in both `extensionsRequired` and `extensionsUsed`.

## Overview

This extension specifies a subset of glTF 2.1 for representing a subtree in [3DTILES_implicit_tiling](../3DTILES_implicit_tiling/README.md). A subtree stores tile, content, and child subtree availability, as well as attributes and application-specific properties for available tiles and contents.

A subtree is not intended for rendering and **SHOULD NOT** have any scenes.

## File Extensions

Assets that use the `3DTILES_subtree` extension **SHOULD** use the `.subtree.gltf` or `.subtree.glb` file extensions. This convention helps differentiate subtree files from tileset and content files.

## Availability

Tile availability (`tileAvailability`) and child subtree availability (`childSubtreeAvailability`) **MUST** always be provided for a subtree.

Content availability (`contentAvailability`) **MUST** be provided if the implicit root tile has a `contentUri` property, otherwise it **MUST** be omitted.

Availability may be represented either as a bitstream or a constant value. `bitstream` is an integer index that identifies the buffer view containing the availability bitstream. `constant` is an integer indicating whether all of the elements are available (`1`) or all are unavailable (`0`). `availableCount` is an integer indicating how many `1` bits exist in the availability bitstream.

For a bitstream with `N` values, the buffer view that stores these availability values will consist of `ceil(N / 8)` bytes. When `N` is not divisible by `8`, then the unused bits of the last byte of this buffer **MUST** be set to 0.

> [!NOTE]
>
> Example accessing availability for element `i`
>
> ```javascript
> byteIndex = floor(i / 8)
> bitIndex = i % 8
> bitValue = (buffer[byteIndex] >> bitIndex) & 1
> available = bitValue == 1
> ```

> **Example:** A subtree where each tile is available, but not all tiles have content, and not all child subtrees are available:
>
> ```json
> {
>   "buffers": [
>     {
>       "byteLength": 48
>     }
>   ],
>   "bufferViews": [
>     {
>       "buffer": 0,
>       "byteOffset": 0,
>       "byteLength": 11
>     },
>     {
>       "buffer": 0,
>       "byteOffset": 16,
>       "byteLength": 32
>     }
>   ],
>   "extensions": {
>     "3DTILES_subtree": {
>       "tileAvailability": {
>         "constant": 1
>       },
>       "contentAvailability": {
>         "bitstream": 0,
>         "availableCount": 60
>       },
>       "childSubtreeAvailability": {
>         "bitstream": 1
>       }
>     }
>   }
> }
> ```

## Attributes

A subtree may store attribute values for available tiles and contents. This may include, for example, tighter fitting bounding volumes than those computed automatically based on [Subdivision Rules](../3DTILES_implicit_tiling/README.md#subdivision-rules).

Each attribute is defined as a property of an attributes object. The name of the property corresponds to a semantic identifying the attribute. The value of the property is the index of an accessor that contains the data. The number of elements in the accessor **MUST** equal the number of available elements.

Tile attributes are provided in the `tileAttributes` object and content attributes are provided in the `contentAttributes` object.

If more than one bounding volume attribute is provided, clients may select the most appropriate option based on use case and performance requirements.

> **Example:** A subtree with tile and content attributes:
>
> ```json
> {
>   "buffers": [
>     {
>       "name": "Availability Buffer",
>       "uri": "availability.bin",
>       "byteLength": 48
>     },
>     {
>       "name": "Attributes Buffer",
>       "uri": "attributes.bin",
>       "byteLength": 19240
>     }
>   ],
>   "bufferViews": [
>     { "buffer": 0, "byteOffset": 0, "byteLength": 11 },
>     { "buffer": 0, "byteOffset": 16, "byteLength": 32 },
>     { "buffer": 1, "byteOffset": 0, "byteLength": 10880 },
>     { "buffer": 1, "byteOffset": 10880, "byteLength": 680 },
>     { "buffer": 1, "byteOffset": 11560, "byteLength": 7680 }
>   ],
>   "accessors": [
>     {
>       "type": "MAT4",
>       "componentType": 5130,
>       "bufferView": 2,
>       "count": 85
>     },
>     {
>       "type": "SCALAR",
>       "componentType": 5130,
>       "bufferView": 3,
>       "count": 85
>     },
>     {
>       "type": "MAT4",
>       "componentType": 5130,
>       "bufferView": 4,
>       "count": 60
>     }
>   ],
>   "extensions": {
>     "3DTILES_subtree": {
>       "tileAvailability": {
>         "constant": 1
>       },
>       "contentAvailability": {
>         "bitstream": 0,
>         "availableCount": 60
>       },
>       "childSubtreeAvailability": {
>         "bitstream": 1
>       },
>       "tileAttributes": {
>         "TILE_BOUNDING_BOX": 0,
>         "TILE_GEOMETRIC_ERROR": 1
>       },
>       "contentAttributes": {
>         "CONTENT_BOUNDING_BOX": 2
>       }
>     }
>   }
> }
> ```

### Tile Attributes

Below is the list of tile attribute semantics. Additional semantics may be defined by extensions.

|Attribute Semantic|Accessor Type|Component Type|Description|
|---|---|---|---|
|`"TILE_BOUNDING_BOX"`|`"MAT4"`|`5130` (DOUBLE)|The bounding box of the tile, expressed as a column-major transformation matrix applied to a unit cube where the top-left 3x3 matrix encodes the box's rotation and scale and the first three elements of the fourth column encodes the box's translation. See [Bounding Box](../3DTILES_tileset/README.md#bounding-box).|
|`"TILE_BOUNDING_SPHERE"`|`"VEC4"`|`5130` (DOUBLE)|The bounding sphere of the tile, where the first three elements encode the x, y, and z values for the center of the sphere and the the last element encodes the radius. See [Bounding Sphere](../3DTILES_tileset/README.md#bounding-sphere).|
|`"TILE_GEOMETRIC_ERROR"`|`"SCALAR"`|`5130` (DOUBLE)|The geometric error of the tile. See [Geometric Error](../3DTILES_tileset/README.md#geometric-error).|
|`"TILE_REFINE"`|`"SCALAR"`|`5121` (UNSIGNED_BYTE)|The tile refinement. Elements in the accessor **MUST** be either `0` (`"ADD"`) or `1` (`"REPLACE"`). See [Refinement](../3DTILES_tileset/README.md#refinement).|
|`"TILE_TRANSFORM"`|`"MAT4"`|`5130` (DOUBLE)|The tile transform. See [Transforms](../3DTILES_tileset/README.md#transforms).|

### Content Attributes

Below is the list of content attribute semantics. Additional semantics may be defined by extensions.

|Attribute Semantic|Accessor Type|Component Type|Description|
|---|---|---|---|
|`"CONTENT_BOUNDING_BOX"`|`"MAT4"`|`5130` (DOUBLE)|The bounding box of the content, expressed as a column-major transformation matrix applied to a unit cube where the top-left 3x3 matrix encodes the box's rotation and scale and the first three elements of the fourth column encodes the box's translation. See [Bounding Box](../3DTILES_tileset/README.md#bounding-box).|
|`"CONTENT_BOUNDING_SPHERE"`|`"VEC4"`|`5130` (DOUBLE)|The bounding sphere of the content, where the first three elements encode the x, y, and z values for the center of the sphere and the the last element encodes the radius. See [Bounding Sphere](../3DTILES_tileset/README.md#bounding-sphere).|

## Properties

Subtrees may store application-specific properties for available tiles and contents. Property values are stored in [property tables](../../../2.0/Vendor/EXT_structural_metadata#property-tables) with [EXT_structural_metadata](../../../2.0/Vendor/EXT_structural_metadata). The binary representation is particularly efficient for larger datasets with many tiles.

`tileProperties` is the index of a property table containing property values for available tiles.

`contentProperties` is the index of a property table containing property values for available content. If the implicit root tile does not have a `contentUri` property, then none of the implicit tiles have content, and `contentProperties` property **MUST** be omitted.

> **Example:** A subtree with tile and content properties:
>
> ```json
> {
>   "extensionsUsed": ["3DTILES_subtree", "EXT_structural_metadata"],
>   "extensionsRequired": ["3DTILES_subtree", "EXT_structural_metadata"],
>   "asset": {
>     "version": "2.1"
>   },
>   "buffers": [
>     {
>       "name": "Availability Buffer",
>       "uri": "availability.bin",
>       "byteLength": 48
>     },
>     {
>       "name": "Properties Buffer",
>       "uri": "properties.bin",
>       "byteLength": 3138
>     }
>   ],
>   "bufferViews": [
>     { "buffer": 0, "byteOffset": 0, "byteLength": 11 },
>     { "buffer": 0, "byteOffset": 16, "byteLength": 32 },
>     { "buffer": 1, "byteOffset": 0, "byteLength": 1530 },
>     { "buffer": 1, "byteOffset": 1530, "byteLength": 344 },
>     { "buffer": 1, "byteOffset": 1874, "byteLength": 1024 },
>     { "buffer": 1, "byteOffset": 2898, "byteLength": 240 }
>   ],
>   "extensions": {
>     "3DTILES_subtree": {
>       "tileAvailability": {
>         "constant": 1
>       },
>       "contentAvailability": {
>         "bitstream": 0,
>         "availableCount": 60
>       },
>       "childSubtreeAvailability": {
>         "bitstream": 1
>       },
>       "tileProperties": 0,
>       "contentProperties": 1
>     },
>     "EXT_structural_metadata": {
>       "schema": {
>         "classes": {
>           "tile": {
>             "properties": {
>               "countries": {
>                 "description": "Countries a tile intersects",
>                 "type": "STRING",
>                 "array": true
>               }
>             }
>           },
>           "content": {
>             "properties": {
>               "triangleCount": {
>                 "type": "SCALAR",
>                 "componentType": "UINT32"
>               }
>             }
>           }
>         }
>       },
>       "propertyTables": [
>         {
>           "class": "tile",
>           "count": 85,
>           "properties": {
>             "countries": {
>               "values": 2,
>               "arrayOffsets": 3,
>               "stringOffsets": 4,
>               "arrayOffsetType": "UINT32",
>               "stringOffsetType": "UINT32"
>             }
>           }
>         },
>         {
>           "class": "content",
>           "count": 60,
>           "properties": {
>             "triangleCount": {
>               "values": 5
>             }
>           }
>         }
>       ]
>     }
>   }
> }
> ```

## Schema

- [3DTILES_subtree.availability.schema.json](schema/3DTILES_subtree.availability.schema.json)
- [glTF.3DTILES_subtree.schema.json](schema/glTF.3DTILES_subtree.schema.json)
