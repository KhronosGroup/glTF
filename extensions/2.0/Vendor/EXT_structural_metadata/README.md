<!-- omit in toc -->
# EXT_mesh_features

<!-- omit in toc -->
## Contributors

* Peter Gagliardi, Cesium
* Sean Lilley, Cesium
* Sam Suhag, Cesium
* Don McCurdy, Independent
* Marco Hutter, Cesium
* Bao Tran, Cesium
* Samuel Vargas, Cesium
* Patrick Cozzi, Cesium

<!-- omit in toc -->
## Status

Draft

<!-- omit in toc -->
## Dependencies

Written against the glTF 2.0 specification.

Optionally, this extension may be used in conjunction with [`EXT_mesh_gpu_instancing`](../EXT_mesh_gpu_instancing). When used together, certain GPU instance attributes defined by `EXT_mesh_gpu_instancing` are used as [instance feature IDs](#feature-id-by-gpu-instance).

<!-- omit in toc -->
## Table of Contents

- [Overview](#overview)
- [Feature Properties](#feature-properties)
  - [Overview](#overview-2)
  - [Schema Definitions](#schema-definitions)
    - [Overview](#overview-3)
    - [Schema](#schema)
    - [Class](#class)
    - [Class Property](#class-property)
    - [Enum](#enum)
    - [Enum Value](#enum-value)
  - [Property Tables](#property-tables)
  - [Property Textures](#property-textures)
- [Binary Data Storage](#binary-data-storage)
- [Optional vs. Required](#optional-vs-required)
- [Schema](#schema-1)
- [Specifying Feature IDs](#specifying-feature-ids)
  - [Referencing Property Tables with Feature IDs](#referencing-property-tables-with-feature-ids)
  - [Referencing External Resources with Feature IDs](#referencing-external-resources-with-feature-ids)
- [Examples](#examples)
- [Revision History](#revision-history)

## Overview

This extension defines a means of storing structured metadata associated with features within a glTF 2.0 asset. A feature a conceptual object or element of a glTF asset that and be uniquely identified. The identification of features can happen on different levels of granularity. A feature might be one instance of a rendered mesh, when using the [`EXT_mesh_gpu_instancing`](../EXT_mesh_gpu_instancing) extension, or a subset of geometry or an area on the surface of a mesh that is identified using the concepts that are introduced in the [`EXT_mesh_fatures`](../EXT_mesh_gpu_instancing) extension.

Concepts and terminology used throughout this document refer to the [3D Metadata Specification](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata), which should be considered a normative reference for definitions and requirements. This document provides inline definitions of terms where appropriate.

See [Examples](#examples) for a more detailed list of use cases for this extension.

> **Disambiguation:** glTF has other methods of storing details that could similarly be described as metadata or properties, including [`KHR_xmp_json_ld`](../../Khronos/KHR_xmp_json_ld), Extras, and Extensions. While those methods associate data with discrete objects in a glTF asset — nodes, materials, etc. — `EXT_mesh_features` is uniquely suited for properties of more granular conceptual features in subregions composed of vertices or texels.







## Feature Properties

### Overview

Feature properties describe attributes or characteristics of a feature. Classes, defined by a schema, are templates describing the data types and meanings of properties, where each feature is a single instance of that template with specific values. Property values may be associated with features in one of two ways:

- **Property Tables** store property values as numeric arrays in a parallel, column-based binary layout. Property tables are indexed by Feature IDs, used as the index of a given feature into each property array.
- **Property Textures** store property values in channels of a texture, suitable for very high-frequency data mapped to less-detailed 3D surfaces. Property textures are indexed by texture coordinates, and do not have associated Feature IDs.

Both storage formats are appropriate for efficiently transmitting large quantities of property values.

### Schema Definitions

#### Overview

Data types and meanings of properties are provided by a schema, as defined in the [3D Metadata Specification](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata/) and summarized below.

#### Schema

*Defined in [schema.schema.json](./schema/schema.schema.json).*

Top-level definitions for the structure and data types of properties. The schema provides a set of [classes](#class) and [enums](#enum) the asset can reference.

A schema may be embedded in the extension directly or referenced externally with the `schemaUri` property. Multiple glTF assets may refer to the same external schema to avoid duplication. A schema is defined by an `EXT_mesh_features` extension attached to the glTF root object.

> **Example:** A simple schema defining enums and classes.
>
> ```jsonc
> {
>   "extensions": {
>     "EXT_mesh_features": {
>       "schema": {
>         "name": "Schema 001",
>         "description": "An example schema.",
>         "version": "3.5.1",
>         "enums": { ... },
>         "classes": { ... }
>       }
>     }
>   }
> }
> ```

#### Class

*Defined in [class.schema.json](./schema/class.schema.json).*

Template for features. Classes provide a list of property definitions. Every feature must be associated with a class, and the feature's properties must conform to the class's property definitions. Features whose properties conform to a class are considered instances of that class.

Classes are defined as entries in the `schema.classes` dictionary, indexed by an alphanumeric class ID.

> **Example:** A "Tree" class, which might describe a table of tree measurements taken in a park. Property definitions are abbreviated here, and introduced in the next section.
>
> ```jsonc
> {
>   "extensions": {
>     "EXT_mesh_features": {
>       "schema": {
>         "classes": {
>           "tree": {
>             "name": "Tree",
>             "description": "Woody, perennial plant.",
>             "properties": {
>               "species": { ... },
>               "height": { ... },
>               "diameter": { ... }
>             }
>           }
>         }
>       }
>     }
>   }
> }
> ```

#### Class Property

*Defined in [class.property.schema.json](./schema/class.property.schema.json).*

Properties are defined abstractly in a class, and are instantiated in a feature with specific values conforming to that definition. Properties support a richer variety of data types than glTF accessors or GPU shading languages allow, defined by `property.componentType`.

Allowed values for `componentType`:

- `"BOOLEAN"`
- `"STRING"`
- `"ENUM"`
- `"INT8"`, `"INT16"`, `"INT32"`, `"INT64"`
- `"UINT8"`, `"UINT16"`, `"UINT32"`, `"UINT64"`
- `"FLOAT32"`, `"FLOAT64"`

A property may compose multiple components into higher-level types (vector, matrix, and array), defined by `property.type`.

Allowed values for `type`:

- `"SINGLE"` (default)
- `"ARRAY"`
- `"VEC2"`, `"VEC3"`, `"VEC4"`
- `"MAT2"`, `"MAT3"`, `"MAT4"`

`"SINGLE"` and `"ARRAY"` types may contain any component type; `"VECN"` and `"MATN"` must contain only numeric component types.

Class properties are defined as entries in the `class.properties` dictionary, indexed by an alphanumeric property ID.

> **Example:** A "Tree" class, which might describe a table of tree measurements taken in a park. Properties include species, height, and diameter of each tree, as well as the number of birds observed in its branches.
>
> ```jsonc
> {
>   "extensions": {
>     "EXT_mesh_features": {
>       "schema": {
>         "classes": {
>           "tree": {
>             "name": "Tree",
>             "description": "Woody, perennial plant.",
>             "properties": {
>               "species": {
>                 "description": "Type of tree.",
>                 "componentType": "ENUM",
>                 "enumType": "speciesEnum",
>                 "required": true
>               },
>               "birdCount": {
>                 "description": "Number of birds perching on the tree",
>                 "componentType": "UINT8",
>                 "required": true
>               },
>               "height": {
>                 "description": "Height of tree measured from ground level, in meters.",
>                 "componentType": "FLOAT32"
>               },
>               "diameter": {
>                 "description": "Diameter at trunk base, in meters.",
>                 "componentType": "FLOAT32"
>               }
>             }
>           }
>         }
>       }
>     }
>   }
> }
> ```

#### Enum

*Defined in [enum.schema.json](./schema/enum.schema.json).*

Set of categorical types, defined as `(name, value)` pairs. Enum properties use an enum as their component type.

Enums are defined as entries in the `schema.enums` dictionary, indexed by an alphanumeric enum ID.

> **Example:** A "Species" enum defining types of trees. An "Unspecified" enum value is optional, but when provided as the `noData` value for a property (see: [3D Metadata → No Data Values](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata#required-properties-and-no-data-values)) may be helpful to identify missing data.
>
> ```jsonc
> {
>   "extensions": {
>     "EXT_mesh_features": {
>       "schema": {
>         "enums": {
>           "speciesEnum": {
>             "name": "Species",
>             "description": "An example enum for tree species.",
>             "values": [
>               {"name": "Unspecified", "value": 0},
>               {"name": "Oak", "value": 1},
>               {"name": "Pine", "value": 2},
>               {"name": "Walnut", "value": 3}
>             ]
>           }
>         }
>       }
>     }
>   }
> }
> ```

#### Enum Value

*Defined in [enum.value.schema.json](./schema/enum.value.schema.json).*

Pairs of `(name, value)` entries representing possible values of an enum property.

Enum values are defined as entries in the `enum.values` array. Duplicate names or duplicate integer values are not allowed.

### Property Tables

*Defined in [propertyTable.schema.json](./schema/propertyTable.schema.json).*

Each property table defines a specified number (`count`) of features conforming to a particular class (`class`), with property values stored as parallel arrays in a column-based binary layout. Property tables support a richer variety of data types than glTF accessors or GPU shading languages allow, and are suitable for datasets that can be expressed in a tabular layout.

Property tables are defined as entries in the `propertyTables` array of the root-level `EXT_mesh_features` extension, and may be referenced by extensions on primitive or node objects.

The property table may provide value arrays for only a subset of the properties of its class, but class properties marked `required: true` must not be omitted. Each property value array given by the property table must be defined by a class property with the same alphanumeric property ID, with values matching the data type of the class property.

> **Example:** A `tree_survey_2021-09-29` property table, implementing the `tree` class defined in earlier examples. The table contains observations for 10 trees, with each property defined by a buffer view containing 10 values.
>
> ```jsonc
> {
>   "extensions": {
>     "EXT_mesh_features": {
>       "schema": { ... },
>       "propertyTables": [{
>         "name": "tree_survey_2021-09-29",
>         "class": "tree",
>         "count": 10,
>         "properties": {
>           "species": {
>             "bufferView": 2,
>             "stringOffsetBufferView": 3
>           },
>           "birdCount": {
>             "bufferView": 1
>           },
>           "height": {
>             "bufferView": 0
>           },
>           // "diameter" is not required and has been omitted from this table.
>         }
>       }]
>     }
>   }
> }
> ```

Property arrays are stored in glTF buffer views and use the binary encoding defined in the [Binary Table Format](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata#binary-table-format) section of the [3D Metadata Specification](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata).

As in the core glTF specification, values of NaN, +Infinity, and -Infinity are never allowed.

Each buffer view `byteOffset` must be aligned to a multiple of its component size.

> **Implementation note:** Authoring tools may choose to align all buffer views to 8-byte boundaries for consistency, but client implementations should only depend on 8-byte alignment for buffer views containing 64-bit component types.

### Property Textures

*Defined in [propertyTexture.schema.json](./schema/propertyTexture.schema.json).*

Property textures use texture channels to store property values conforming to a particular class (identified by ID `class`), with those values accessed directly by texture coordinates. Property textures do not require feature IDs, and are especially useful when texture mapping high frequency data to less detailed 3D surfaces. Unlike textures used in glTF materials, property textures are not necessarily visible in a rendered scene. Like property tables, property textures are implementations of the [3D Metadata Specification](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata).

Property textures are defined as entries in the `propertyTextures` array of the root-level `EXT_mesh_features` extension, and may be referenced by extensions on primitive objects. Property textures do not provide per-instance values with `EXT_mesh_gpu_instancing`, and must not be used by extensions on node objects.

A property texture may provide channels for only a subset of the properties of its class, but class properties marked `required: true` must not be omitted.

Several constraints are imposed to ensure compatibility of texture storage with various property types:

* A scalar property cannot be encoded into multiple channels. For example, it is not possible to encode a `UINT32` property in an `RGBA8` texture.
* Components of array and vector properties must be separate channels within a texture.
* Variable-length arrays are not supported.
* Data type and bit depth of the image must be compatible with the property type.

For example, an 8-bit per pixel RGB image may only contain `SINGLE`, fixed-length `ARRAY` (length ≤3), `VEC2`, or `VEC3` values composed of 8-bit component types.

Enum values may be encoded in images, as integer values according to their enum value type (see [Enum](#enum)).

> **Implementation note:** Use of floating-point properties in a property texture would require a floating point-compatible image format like KTX2, provided by an additional extension.

> **Example:** Property texture implementing a "wall" class, with property values stored in a glTF texture at index 0 and indexed by `TEXCOORD_0`.
>
> <img src="figures/feature-texture.png"  alt="Property Texture" width="500">
>
> _Class and property texture_
>
> ```jsonc
> {
>   "extensions": {
>     "EXT_mesh_features": {
>       "schema": {
>         "classes": {
>           "wall": {
>             "name": "Wall Temperature vs. Insulation",
>             "properties": {
>               "insideTemperature": {
>                 "name": "Inside Temperature",
>                 "componentType": "UINT8"
>               },
>               "outsideTemperature": {
>                 "name": "Outside Temperature",
>                 "componentType": "UINT8"
>               },
>               "insulation": {
>                 "name": "Insulation Thickness",
>                 "componentType": "UINT8",
>                 "normalized": true
>               }
>             }
>           }
>         }
>       },
>       "propertyTextures": [
>         {
>           "class": "wall",
>           "index": 0,
>           "texCoord": 0,
>           "properties": {
>             "insideTemperature": {
>               "channels": [0]
>             },
>             "outsideTemperature": {
>               "channels": [1]
>             },
>             "insulation": {
>               "channels": [2]
>             }
>           }
>         }
>       ]
>     }
>   }
> }
> ```
>
> _Primitive_
>
> ```jsonc
> {
>   "primitives": [
>     {
>       "attributes": {
>         "POSITION": 0,
>         "TEXCOORD_0": 1
>       },
>       "indices": 2,
>       "material": 0,
>       "extensions": {
>         "EXT_mesh_features": {
>           "propertyTextures": [0]
>         }
>       }
>     }
>   ]
> }
> ```


A `propertyTexture` object extends the glTF [`textureInfo`](../../../../specification/2.0/schema/textureInfo.schema.json) object. `texCoord` specifies a texture coordinate set in the referring primitive.

The `properties` map specifies the texture channels providing data for available properties. An array of integer index values identifies channels, where multiple channels may be used only for fixed-length arrays of 2, 3, or 4 components. Channels of an `RGBA` texture are numbered 0–3 respectively, although specialized texture formats may allow additional channels. All values are stored in linear space.

> **Example:** A property texture for wind velocity samples. The "speed" property values are stored in the red channel, and "direction" property values are stored as a unit-length vector, with X/Y components in the green and blue channels. Both properties are indexed by UV coordinates in a `TEXCOORD_0` attribute.
>
> ```jsonc
> // Root EXT_mesh_features extension:
> {
>   "propertyTextures": [
>     {
>       "class": "wind",
>       "index": 0,
>       "texCoord": 0,
>       "properties": {
>         "speed": {
>           "channels": [0]
>         },
>         "direction": {
>           "channels": [1, 2]
>         }
>       }
>     }
>   ]
> }

Texture filtering must be `9728` (NEAREST), `9729` (LINEAR), or undefined, for any texture object referenced as a property texture.

## Binary Data Storage

Feature properties are stored in a compact binary tabular format described in the [3D Metadata Specification](https://github.com/CesiumGS/3d-tiles/tree/main/specification/Metadata), with each property table array occupying a glTF buffer view. `EXT_mesh_features` imposes 8-byte binary data alignment requirements on an asset, allowing support for 64-bit data types while remaining compatible with the 4-byte alignments in the core glTF specification:

- GLB-stored `JSON` chunk must be padded with trailing `Space` characters (`0x20`) to 8-byte boundary.
- GLB-stored `BIN` chunk must be padded with trailing zeroes (`0x00`) to 8-byte boundary.

As a result, byte length of the `BIN` chunk may be up to 7 bytes larger than JSON-defined `buffer.byteLength` to satisfy alignment requirements.

## Optional vs. Required

This extension is optional, meaning it should be placed in the `extensionsUsed` list, but not in the `extensionsRequired` list.

## Schema

* [glTF.EXT_mesh_features.schema.json](./schema/glTF.EXT_mesh_features.schema.json)
* [mesh.primitive.EXT_mesh_features.schema.json](./schema/mesh.primitive.EXT_mesh_features.schema.json)
* [node.EXT_mesh_features.schema.json](./schema/node.EXT_mesh_features.schema.json)












### Specifying Feature IDs

A primitive or node may specify multiple feature IDs using one or more of the methods above. With multiple feature IDs, an asset might (for example) identify features at different levels of abstraction: per-vertex feature IDs for individual buildings, and per-texel feature IDs for parts of each building, with each level of abstraction having its own properties.

Each feature ID definition may include only a single source, so the following are mutually exclusive:

- `featureId.attribute` (for a Feature ID Attribute)
- `featureId.offset` and `featureId.repeat` (for a Feature ID Attribute)
- `featureId.index` (for a Feature ID Texture)

Primitives may contain `featureIds` entries for vertex attribute and texture-based feature IDs, and Nodes may contain `featureIds` entries for GPU instance attributes.

#### Referencing Property Tables with Feature IDs

When feature IDs are associated with property tables, then every `propertyTables` index must have an associated `featureIds` definition. The `propertyTables` entry at index `i` corresponds to the `featureIds` entry at the same index. As a result, the length of the `featureIds` array must be greater than or equal to the length of the `propertyTables` array. Each (`featureId`, `propertyTable`) pair must be unique, but individual feature IDs and property tables may be repeated within a primitive or node.

Empty feature IDs (e.g. `{}`) are disallowed — a feature ID must explicitly set at least one property.

> **Example:** Multiple property tables and feature IDs may be defined on a single primitive.
>
> ```jsonc
> // Primitive:
> "extensions": {
>   "EXT_mesh_features": {
>     "propertyTables": [0, 1, 2],
>     "featureIds": [
>       {"attribute": 0},
>       {"offset": 1},
>       {"index": 0, "texCoord": 0, "channel": 0},
>     ]
>   }
> }
> ```

#### Referencing External Resources with Feature IDs

Feature IDs do not have to be associated with a property table. Without a property table, IDs may identify features for use in other extensions or in custom applications. Use cases for these IDs extend beyond the scope of this extension, but could include identifying features for styling or picking, or looking up metadata externally in a REST API or database.

> <img src="figures/feature-id-lookup.png"  alt="Feature ID lookup" width="800">






































## Examples

_This section is non-normative_

The examples below shows the breadth of possible use cases for this extension.

Example|Description|Image
--|--|--
Triangle mesh|Feature IDs are assigned to each vertex to distinguish components of a building.|![Building Components](figures/building-components.png)
Per-vertex properties<img width=700/>|An implicit feature ID is assigned to each vertex. The property table stores `FLOAT64` accuracy values. |![Per-vertex properties](figures/per-vertex-metadata.png)
Per-triangle properties|An implicit feature ID is assigned to each set of three vertices. The property table stores `FLOAT64` area values.|![Per-triangle properties](figures/per-triangle-metadata.png)
Per-point properties|An implicit feature ID is assigned to each point. The property table stores `FLOAT64` , `STRING`, and `ENUM` properties, which are not possible through glTF vertex attribute accessors alone.|![Point features](figures/point-features.png)
Per-node properties|Vertices in node 0 and node 1, not batched together, are assigned different `offset` feature IDs.|![Per-node properties](figures/per-node-metadata.png)
Multi-point features|A point cloud with two property tables, one storing properties for groups of points and the other storing properties for individual points.|![Multi-point features](figures/point-cloud-layers.png)
Multi-instance features|Instanced tree meshes, where trees are assigned to groups with a per-GPU-instance feature ID attribute. One property table stores per-group properties and the other stores per-tree properties.|![Multi-instance features](figures/multi-instance-metadata.png)
Material classification|A textured mesh using a property texture to store both material enums and normalized `UINT8` insulation values.|![Material Classification](figures/material-classification.png)
Composite|A glTF containing a 3D mesh (house), a point cloud (tree), and instanced meshes (fencing) with three property tables.|![Composite Example](figures/composite-example.png)

## Revision History

The revision history of this extension can be found in the [common revision history of the 3D Tiles Next extensions](https://github.com/CesiumGS/3d-tiles/blob/extension-revisions/next/REVISION_HISTORY.md).
