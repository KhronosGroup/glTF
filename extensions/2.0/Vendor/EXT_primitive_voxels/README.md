# EXT_primitive_voxels

<p align="center">
  <img src="figures/voxels.png">
</p>
<p align="center"><i>Geothermal temperature data surrounding Lake Thunseree, Switzerland, visualized in CesiumJS using <code>EXT_primitive_voxels</code>. Data courtesy of Swisstopo.</i></p>

## Contributors

- Janine Liu, Cesium
- Daniel Krupka, Cesium
- Ian Lilley, Cesium
- Sean Lilley, Cesium
- Jeshurun Hembd, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.0 specification. Depends on the [`KHR_implicit_shapes`](https://github.com/eoineoineoin/glTF/tree/refs/heads/collisionShapeMerge/extensions/2.0/Khronos/KHR_implicit_shapes) extension.

## Overview

This extension allows mesh primitives to represent volumetric (voxel) data via application-specific attribute semantics. Primitives that use this extension must set their `mode` to the constant `0x7FFFFFFF` (`2147483647`) used to indicate voxels.

Typical mesh primitives make use of the `POSITION` attribute to store positional mesh data. However, `POSITION` is neither required nor used by `EXT_primitive_voxels`. Instead, this extension relies on the `KHR_implicit_shapes` extension to describe the shape of the voxel grid.

```json
{
  "extensions": { 
    "KHR_implicit_shapes": {
      "shapes": [
        {
          "type": "box",
          "box": {
            "size": [2, 2, 2]
          }
        }
      ]
    }
  },
  "meshes": [
    {
      "primitives": [
        {
          "attributes": {
            "_TEMPERATURE": 0
          },
          "mode": 2147483647,
          "extensions": {
            "EXT_primitive_voxels": {
                "shape": 0,
                "dimensions": [8, 8, 8]
            }
          }
        }
      ]
    }
  ]
}
```

Though voxels are commonly associated with cubic geometry on a box-based grid, this extension also allows voxels to be based on other shapes, including cylinder-based regions specified by [`EXT_implicit_cylinder_region`](../EXT_implicit_cylinder_region/) and ellipsoid-based regions specified by [`EXT_implicit_ellipsoid_region`](../EXT_implicit_ellipsoid_region/). The supported shapes are visualized below.

|Box|Cylinder|Ellipsoid|
| ------------- | ------------- | ------------- |
|![Box Voxel Grid](figures/box.png)|![Cylindrical Voxel Grid](figures/cylinder.png)|![Ellipsoid Voxel Grid](figures/sphere.png)|

Voxels exist inside a bounding volume that conforms to the shape of the grid. The `dimensions` property refers to the number of subdivisions _within_ this bounding volume. Each value of `dimensions` must be a positive integer. The way that `dimensions` is interpreted depends on the grid geometry, as explained below.

> [!NOTE]
> The following examples use small voxel `dimensions` for illustrative purposes. In practice, voxel primitives will use much larger values for their `dimensions`.

### Box Grid

A **box** grid is a Cartesian grid defined by `x`, `y`, and `z` axes with equally-sized boxes. The `dimensions` correspond to the subdivisions of the box along the `x`, `y`, and `z` axes respectively.

Elements are laid out in memory where the `x` data is contiguous in strides along the `y` axis, and each group of `y` strides represents a `z` slice.

![Uniform box grid](figures/uniform-box.png)
<p align="center"><i>A uniform box grid that is subdivided into two cells along each axis.</i></p>

![Non-uniform box grid](figures/non-uniform-box.png)
<p align="center"><i>A box grid that is non-uniformly scaled and also non-uniformly subdivided.</i></p>

### Cylinder Region Grid

A **cylinder** region grid is subdivided along the radius, height, and angle ranges of the region. The `dimensions` correspond to the subdivisions of those ranges, respectively.

![Cylinder subdivisions](figures/cylinder-subdivisions.png)

The cylinder is aligned with the `y`-axis in the primitive's local space. Its height is subdivided along that local `y`-axis from bottom to top. Subdivisions along the radius are concentric, centered around the `y`-axis and extending outwards. Finally, the angular bounds are subdivided counter-clockwise around the circumference of the cylinder.

Elements are laid out in memory where the radial data is contiguous in strides along the cylinder's height, as if stacked in a column. Each group of height strides represents an angular slice on the cylinder.

![Whole cylinder grid](figures/whole-cylinder.png)
<p align="center"><i>A cylinder that is subdivided into two cells along each axis.</i></p>

![Non-uniform cylinder grid](figures/non-uniform-cylinder.png)
<p align="center"><i>A smaller cylinder region with radial and angular bounds that is non-uniformly subdivided.</i></p>

### Ellipsoid Region Grid

An **ellipsoid** region grid is subdivided along the longitude, latitude, and height ranges of the region. The `dimensions` correspond to the subdivisions of those ranges, respectively.

Elements are laid out in memory where the longitude data is contiguous in strides along the region's latitude. Each group of latitude strides represents a height slice on the region.

![Region grid](figures/part-ellipsoid.png)
<p align="center"><i>An ellipsoid region that is subdivided into two cells along each axis.</i></p>

![Non-uniform region grid](figures/non-uniform-part-ellipsoid.png)

<p align="center"><i>An ellipsoid region that is non-uniformly subdivided.</i></p>

![Whole ellipsoid grid](figures/whole-ellipsoid.png)
<p align="center"><i>A hollow ellipsoid region that covers the entire ellipsoid, subdivided into two cells along each axis.</i></p>

### Padding

The `padding` property specifies how many rows of attribute data in each dimension come from neighboring grids. This is useful in situations where the primitive represents a single tile in a larger grid, and data from neighboring tiles is needed for non-local effects e.g. trilinear interpolation, blurring, or antialiasing.

`padding.before` and `padding.after` specify the number of rows before and after the grid in each dimension, e.g. a `padding.before` of 1 and a `padding.after` of 2 in the `y` dimension mean that each series of values in a given `y`-slice is preceded by one value and followed by two.

Padding data must be included with the rest of the voxel data. In other words, given `dimensions` of `[d1, d2, d3]`, `padding.before` of `[b1, b2, b3]`, and `padding.after` of `[a1, a2, a3]`, the voxel primitive's attributes must contain `(d1 + a1 + b1)*(d2 + a2 + b2)*(d3 + a3 + b3)` elements. In the following example, the attributes on this voxel primitive must supply `(8 + 1 + 1)*(8 + 1 + 1)*(8 + 1 + 1) = 1000` elements.

```json
"extensions": {
  "EXT_primitive_voxels": {
    "shape": 0,
    "dimensions": [8, 8, 8],
    "padding": {
      "before": [1, 1, 1],
      "after": [1, 1, 1]
    }
  }
}
```

### No Data Values

Voxel primitives may optionally specify a "No Data" value (or "sentinel value") for its attributes to indicate where property values do not exist. This "No Data" value may be provided for any type of attribute, but must be defined according to the type of its `accessor`. For `normalized` accessors, the `noData` value should be specified as the raw data value *before* normalization.

The "No Data" values for attributes must be defined in the `noData` object. Any key in `noData` must match an existing key in the primitive's `attributes` object. However, not all `attributes` are required to provide a `noData` value.

For instance, if a voxel primitive references the following accessors...

```jsonc
"accessors": [
  {
    "type": "SCALAR",
    "componentType": 5122, // SHORT
    "normalized": true
  },
  {
    "type": "VEC3",
    "componentType": 5126, // FLOAT
  },
  // ....
]
```

...then it may define `noData` values for its corresponding attributes like so:

```json
"meshes": [
  {
    "primitives": [
      {
        "attributes": {
          "_TEMPERATURE": 0,
          "_DIRECTION": 1,
          "_DATA_CONFIDENCE": 2
        },
        "mode": 2147483647,
        "extensions": {
          "EXT_primitive_voxels": {
              "shape": 0,
              "dimensions": [8, 8, 8],
              "noData": {
                "_TEMPERATURE": [-32768],
                "_DIRECTION": [-999.99, -999.99, -999.99]
              }
          }
        }
      }
    ]
  }
]
```

Note that `_DATA_CONFIDENCE` intentionally does not specify a `noData` value. The attribute is expected to contain a valid value for every voxel cell.

### Metadata

This extension may be paired with the `EXT_structural_metadata` extension to convey more semantic information about the voxel attributes.

```json
{
  "extensions": {
    "EXT_structural_metadata": {
      "schema": {
        "classes": {
          "voxels": {
            "properties": {
              "temperature": {
                "type": "SCALAR",
                "componentType": "UINT32",
                "normalized": true,
                "offset": 32.0,
                "scale": 1.8
              }
            }
          }
        }
      },
      "propertyAttributes": [
        {
          "class": "voxels",
          "properties": {
            "temperature":{
              "attribute": "_TEMPERATURE"
            }
          }
        }
      ]
    }
  },
  "meshes": [
    {
      "primitives": [
        {
          "attributes": {
            "_TEMPERATURE": 0
          },
          "extensions": {
            "EXT_primitive_voxels": {
              "dimensions": [8, 8, 8],
              "padding": {
                "before": [1, 1, 1],
                "after": [1, 1, 1]
              }
            },
            "EXT_structural_metadata": {
              "propertyAttributes": [0]
            }
          }
        }
      ]
    }
  ]
}
```

`EXT_structural_metadata` may also specify a `noData` value for a property attribute property. If `EXT_primitive_voxels` contains an entry in `noData` for the same attribute, the values **SHOULD** match betwen the two extensions.

## Optional vs. Required
This extension is required, meaning it should be placed in both the `extensionsUsed` list and `extensionsRequired` list.
