<!--
SPDX-FileCopyrightText: 2026 Bentley Systems, Incorporated

SPDX-License-Identifier: CC-BY-4.0
-->

# 3DTILES\_tileset\_voxels

## Contributors

- Sean Lilley, Cesium
- Ian Lilley, Cesium
- Janine Liu, Cesium
- Jeshurun Hembd, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.1 spec.

Depends on [3DTILES_tileset](../3DTILES_tileset/README.md).
Depends on [EXT_structural_metadata](../../../2.0/Vendor/EXT_structural_metadata).
Depends on [EXT_voxels](../EXT_voxels/README.md).

## Required

This extension is required, meaning it **MUST** be placed in both `extensionsRequired` and `extensionsUsed`.

## Overview

This extension defines a structure for volumetric data (voxels) in 3D Tiles.

- The tileset's root tile **MUST** use [`3DTILES_implicit_tiling`](../3DTILES_implicit_tiling/README.md).
- Each tile content **MUST** have a single node with the [`EXT_voxels`](../EXT_voxels/README.md) extension.
- The shape defined in each tile's `EXT_voxels` extension **MUST** be equivalent to the implicitly derived bounding volume.
- Each tile's `EXT_voxels` extension **MUST** associate its attribute data with metadata definitions using [`EXT_structural_metadata`](../../../2.0/Vendor/EXT_structural_metadata).

### Dimensions

The `dimensions` property of the extension specifies the voxel grid's dimensions along each axis. The allowed values, their order, and meaning are as defined in [EXT_voxels](../EXT_voxels/README.md#dimensions).

The value of the `dimensions` property **MUST** match the value of `dimensions` defined in the `EXT_voxels` extension for each tile.

### Padding

The `padding` property specifies how many rows of voxel data in each dimension are copied from neighboring tiles. The allowed values, their order, and meaning are as defined in [EXT_voxels](../EXT_voxels/README.md#padding).

The value of the `padding` property **MUST** match the value of `padding` defined in the `EXT_voxels` extension for each tile.

### Class

The `class` property refers to a class ID in the metadata schema associated with the tileset, as defined in the [`EXT_structural_metadata`](../../../2.0/Vendor/EXT_structural_metadata) extension. The class describes which properties exist in the voxel grid. In the example below, each voxel may have a `temperature` value and/or a `salinity` value. When a property value equals the `noData` value it indicates that no data exists for that voxel.

The `class` **MUST** match the `class` used to classify the voxels in the `EXT_structural_metadata` extension, inside `EXT_voxels` for each tile.

## Example

_This section is non-normative._

The following example describes a voxel tileset containing two metadata values in each voxel grid.

```json
{
  "extensionsUsed": ["3DTILES_tileset", "3DTILES_tileset_voxels", "EXT_structural_metadata"],
  "extensionsRequired": ["3DTILES_tileset", "3DTILES_tileset_voxels", "EXT_structural_metadata"],
  "asset": {
    "version": "2.1"
  },
  "shapes": [
    {
      "type": "box",
      "box": {
        "size": [1.0, 1.0, 1.0]
      }
    }
  ],
  "extensions": {
    "EXT_structural_metadata": {
      "schema": {
        "classes": {
          "voxel": {
            "properties": {
              "temperature": {
                "type": "SCALAR",
                "componentType": "FLOAT32",
                "noData": 999.9
              },
              "salinity": {
                "type": "SCALAR",
                "componentType": "UINT8",
                "normalized": true,
                "noData": 255
              }
            }
          }
        }
      }
    },
    "3DTILES_tileset": {
      "geometricError": 240
    },
    "3DTILES_tileset_voxels": {
      "shape": 0,
      "dimensions": [8, 8, 8],
      "padding": {
        "before": [1, 1, 1],
        "after": [1, 1, 1]
      },
      "class": "voxel"
    }
  },
  "nodes": [
    {
      "extensions": {
        "3DTILES_tileset": {
          "geometricError": 70.0,
          "refine": "REPLACE"
        },
        "3DTILES_implicit_tiling": {
          "contentUri": "content/{level}/{right}/{forward}/{up}.glb",
          "subtreeUri": "subtrees/{level}/{right}/{forward}/{up}.subtree.glb",
          "subdivisionScheme": "OCTREE",
          "availableLevels": 9,
          "subtreeLevels": 3
        }
      },
      "boundingVolume": {
        "shape": 0
      }
    }
  ]
}
```

## Schema

- [glTF.3DTILES_tileset_voxels.schema.json](schema/glTF.3DTILES_tileset_voxels.schema.json)
- [padding.schema.json](schema/padding.schema.json)
