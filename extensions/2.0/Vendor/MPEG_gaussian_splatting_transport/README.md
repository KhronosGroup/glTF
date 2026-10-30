# MPEG_gaussian_splatting_transport

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on ISO/IEC 23090-14 2nd edition FDAM 1

## Dependencies

Written against the glTF 2.0 spec.

Requires `KHR_gaussian_splatting` extension.

## Overview

This extension adds transport and streaming capabilities to the `KHR_gaussian_splatting` base extension. It enables MPEG-specific features for efficient delivery and progressive refinement of 3D Gaussian splat data, including alternative spherical harmonics (SH) coefficient layouts optimized for streaming, progressive download signaling, timed media support for dynamic sequences, and a stitching mechanism to attach Gaussian splats to triangular mesh primitives.

The `MPEG_gaussian_splatting_transport` object shall be carried inside the nested `extensions` object of `KHR_gaussian_splatting` on a mesh primitive. The mesh primitive `mode` shall be `0` (POINTS).

A receiver that does not implement this extension can still render a baseline POINTS preview using standard glTF attributes `POSITION` and `COLOR_0` as defined by `KHR_gaussian_splatting`.

## glTF Schema Updates

The `MPEG_gaussian_splatting_transport` extension provides:

* Accessor bindings for MPEG SH layouts and DC reconstruction policy.
* An optional progressive ordering over accessors for incremental download. Any referenced accessor is treated as timed if and only if it carries `MPEG_accessor_timed`.
* An optional stitching mechanism to attach Gaussian splats to a 3D triangular mesh primitive.

**`MPEG_gaussian_splatting_transport` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**shEncoding**|[`MPEG_gaussian_splatting_transport.shEncoding`](#reference-mpeg_gaussian_splatting_transport-shencoding)|Signals how SH coefficients are encoded for this splat primitive.| &#10003; Yes|
|**stitching**|[`MPEG_gaussian_splatting_transport.stitching`](#reference-mpeg_gaussian_splatting_transport-stitching)|Parameters to attach Gaussian splats to a 3D triangular mesh primitive.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.schema.json](./schema/MPEG_gaussian_splatting_transport.schema.json)

### MPEG_gaussian_splatting_transport.shEncoding

Signals how SH coefficients are encoded for this splat primitive. The object provides the receiver with the information needed to locate, decode, and reconstruct SH data regardless of the chosen layout.

* **Type**: [`MPEG_gaussian_splatting_transport.shEncoding`](#reference-mpeg_gaussian_splatting_transport-shencoding)
* **Required**: &#10003; Yes

### MPEG_gaussian_splatting_transport.stitching

Parameters to attach Gaussian splats to a 3D triangular mesh primitive. When present, the base `POSITION` attribute of each Gaussian splat is overridden by the computed position on the target mesh surface.

* **Type**: [`MPEG_gaussian_splatting_transport.stitching`](#reference-mpeg_gaussian_splatting_transport-stitching)
* **Required**: No

### MPEG_gaussian_splatting_transport.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


---------------------------------------
<a name="reference-mpeg_gaussian_splatting_transport-shencoding"></a>
## MPEG_gaussian_splatting_transport.shEncoding

Signals how SH coefficients are encoded for this splat primitive.

Three SH encoding modes are supported:

* **khrPacked**: SH coefficients are stored directly using the `KHR_gaussian_splatting:SH_DEGREE_l_COEF_n` VEC3 attributes. This mode is fully compatible with renderers that only support the base `KHR_gaussian_splatting` extension.
* **mpegProgressive**: SH coefficients are stored in SCALAR accessors organized by degree groups. The DC term is not stored in these accessors.
* **mpegPerChannel**: SH coefficients are stored in SCALAR accessors organized by color channel. The DC term is not stored in these accessors.

In both MPEG layouts, the DC (degree 0) term is not stored in the MPEG coefficient accessors. When `dcFromColor0` is true, the receiver shall reconstruct degree 0 from `COLOR_0.rgb` using rules consistent with the declared `KHR_gaussian_splatting` `colorSpace`. When `dcFromColor0` is false, degree 0 is carried explicitly using `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0`.

**`MPEG_gaussian_splatting_transport.shEncoding` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**layout**|`string`|Indicates the encoding mode of the SH coefficients.| &#10003; Yes|
|**maxDegree**|`integer`|Maximum SH degree available to the receiver.| &#10003; Yes|
|**dcFromColor0**|`boolean`|If true, receivers reconstruct the degree-0 (DC) term from `COLOR_0.rgb`.| &#10003; Yes|
|**mpegProgressive**|[`MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive`](#reference-mpeg_gaussian_splatting_transport-shencoding-mpegprogressive)|Degree-group SH coefficient accessors.|No|
|**mpegPerChannel**|[`MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel`](#reference-mpeg_gaussian_splatting_transport-shencoding-mpegperchannel)|Per-channel SH coefficient accessors.|No|
|**progressive**|[`MPEG_gaussian_splatting_transport.shEncoding.progressive`](#reference-mpeg_gaussian_splatting_transport-shencoding-progressive)|Progressive ordering of accessors for incremental download.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.shEncoding.schema.json](./schema/MPEG_gaussian_splatting_transport.shEncoding.schema.json)

### MPEG_gaussian_splatting_transport.shEncoding.layout

Indicates the encoding mode of the SH coefficients. When `layout` is `"mpegProgressive"`, the `mpegProgressive` object shall be present. When `layout` is `"mpegPerChannel"`, the `mpegPerChannel` object shall be present.

* **Type**: `string`
* **Required**: &#10003; Yes
* **Allowed values**:
    * `"khrPacked"`
    * `"mpegProgressive"`
    * `"mpegPerChannel"`

### MPEG_gaussian_splatting_transport.shEncoding.maxDegree

Maximum SH degree available to the receiver. Valid values are 0, 1, 2, or 3. When `layout` is `"mpegProgressive"` and `maxDegree` is greater than or equal to 2, the `mpegProgressive.second` accessor shall be present.

* **Type**: `integer`
* **Required**: &#10003; Yes
* **Minimum**: ` >= 0`
* **Maximum**: ` <= 3`

### MPEG_gaussian_splatting_transport.shEncoding.dcFromColor0

If true, receivers reconstruct the degree-0 (DC) term from `COLOR_0.rgb` using rules consistent with the declared `KHR_gaussian_splatting` `colorSpace`. If false, degree 0 is carried explicitly via `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0`.

* **Type**: `boolean`
* **Required**: &#10003; Yes

### MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive

Degree-group SH coefficient accessors. Required when `layout` is `"mpegProgressive"`.

* **Type**: [`MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive`](#reference-mpeg_gaussian_splatting_transport-shencoding-mpegprogressive)
* **Required**: No

### MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel

Per-channel SH coefficient accessors. Required when `layout` is `"mpegPerChannel"`.

* **Type**: [`MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel`](#reference-mpeg_gaussian_splatting_transport-shencoding-mpegperchannel)
* **Required**: No

### MPEG_gaussian_splatting_transport.shEncoding.progressive

Progressive ordering of accessors for incremental download. Stages are ordered from lower to higher fidelity. A receiver may initially fetch only the first stage and render using whatever attributes are available. As additional stages become available, the receiver progressively refines the rendering without re-decoding previously available data.

* **Type**: [`MPEG_gaussian_splatting_transport.shEncoding.progressive`](#reference-mpeg_gaussian_splatting_transport-shencoding-progressive)
* **Required**: No

### MPEG_gaussian_splatting_transport.shEncoding.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.shEncoding.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


---------------------------------------
<a name="reference-mpeg_gaussian_splatting_transport-shencoding-mpegprogressive"></a>
## MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive

SH coefficients stored as degree-group SCALAR accessors. The DC term is excluded. Each accessor stores interleaved RGB triplets for all coefficients of a given degree. Per splat, `first` contains 9 values (3 RGB triplets for degree 1), `second` contains 15 values (5 RGB triplets for degree 2), and `third` contains 21 values (7 RGB triplets for degree 3).

**`MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**first**|`integer`|Accessor index for degree 1 coefficients (9 scalar values per splat).| &#10003; Yes|
|**second**|`integer`|Accessor index for degree 2 coefficients (15 scalar values per splat).|No|
|**third**|`integer`|Accessor index for degree 3 coefficients (21 scalar values per splat).|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.schema.json](./schema/MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.schema.json)

### MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.first

The index of the accessor that contains degree 1 coefficients as 3 RGB triplets (coef 0..2), giving 9 scalar values per splat. The DC term is excluded.

* **Type**: `integer`
* **Required**: &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.second

The index of the accessor that contains degree 2 coefficients as 5 RGB triplets (coef 0..4), giving 15 scalar values per splat. The DC term is excluded. Required when `shEncoding.maxDegree` is greater than or equal to 2.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.third

The index of the accessor that contains degree 3 coefficients as 7 RGB triplets (coef 0..6), giving 21 scalar values per splat. The DC term is excluded.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.shEncoding.mpegProgressive.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


---------------------------------------
<a name="reference-mpeg_gaussian_splatting_transport-shencoding-mpegperchannel"></a>
## MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel

SH coefficients stored as per-channel SCALAR accessors. The DC term is excluded. Each channel accessor contains 15 values per splat for degrees 1 to 3, in order: degree 1 (3 values), degree 2 (5 values), degree 3 (7 values).

**`MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**r**|`integer`|Accessor index for red channel coefficients (15 scalar values per splat).| &#10003; Yes|
|**g**|`integer`|Accessor index for green channel coefficients (15 scalar values per splat).| &#10003; Yes|
|**b**|`integer`|Accessor index for blue channel coefficients (15 scalar values per splat).| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.schema.json](./schema/MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.schema.json)

### MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.r

The index of the accessor that contains red channel coefficients (15 scalar values per splat) for degrees 1 to 3, in order: degree 1 (3), degree 2 (5), degree 3 (7). The DC term is excluded.

* **Type**: `integer`
* **Required**: &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.g

The index of the accessor that contains green channel coefficients (15 scalar values per splat) for degrees 1 to 3, in order: degree 1 (3), degree 2 (5), degree 3 (7). The DC term is excluded.

* **Type**: `integer`
* **Required**: &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.b

The index of the accessor that contains blue channel coefficients (15 scalar values per splat) for degrees 1 to 3, in order: degree 1 (3), degree 2 (5), degree 3 (7). The DC term is excluded.

* **Type**: `integer`
* **Required**: &#10003; Yes
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.shEncoding.mpegPerChannel.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


---------------------------------------
<a name="reference-mpeg_gaussian_splatting_transport-shencoding-progressive"></a>
## MPEG_gaussian_splatting_transport.shEncoding.progressive

Progressive ordering of accessors for incremental download. To avoid partial states, a stage should correspond to complete attributes or complete SH degrees.

**`MPEG_gaussian_splatting_transport.shEncoding.progressive` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**stages**|[`MPEG_gaussian_splatting_transport.shEncoding.progressive.stage`](#reference-mpeg_gaussian_splatting_transport-shencoding-progressive-stage) `[1-*]`|Array of progressive stages ordered from lower to higher fidelity.| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.shEncoding.progressive.schema.json](./schema/MPEG_gaussian_splatting_transport.shEncoding.progressive.schema.json)

### MPEG_gaussian_splatting_transport.shEncoding.progressive.stages

Array of progressive stages ordered from lower to higher fidelity. A receiver may initially fetch only the first stage and begin rendering. As additional stages become available, the receiver progressively refines the rendering without re-decoding previously available data.

* **Type**: [`MPEG_gaussian_splatting_transport.shEncoding.progressive.stage`](#reference-mpeg_gaussian_splatting_transport-shencoding-progressive-stage) `[1-*]`
* **Required**: &#10003; Yes

### MPEG_gaussian_splatting_transport.shEncoding.progressive.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.shEncoding.progressive.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


---------------------------------------
<a name="reference-mpeg_gaussian_splatting_transport-shencoding-progressive-stage"></a>
## MPEG_gaussian_splatting_transport.shEncoding.progressive.stage

A progressive stage listing accessors that should be fetched together.

**`MPEG_gaussian_splatting_transport.shEncoding.progressive.stage` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**accessors**|`integer` `[1-*]`|Array of accessor indices to fetch for this stage.| &#10003; Yes|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.shEncoding.progressive.stage.schema.json](./schema/MPEG_gaussian_splatting_transport.shEncoding.progressive.stage.schema.json)

### MPEG_gaussian_splatting_transport.shEncoding.progressive.stage.accessors

Array of accessor indices to fetch for this stage. An accessor is timed if and only if it carries `MPEG_accessor_timed`. Static and timed accessors may be mixed within the same stage.

* **Type**: `integer` `[1-*]`
    * Each element in the array **MUST** be greater than or equal to `0`.
* **Required**: &#10003; Yes

### MPEG_gaussian_splatting_transport.shEncoding.progressive.stage.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.shEncoding.progressive.stage.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


---------------------------------------
<a name="reference-mpeg_gaussian_splatting_transport-stitching"></a>
## MPEG_gaussian_splatting_transport.stitching

Parameters to attach Gaussian splats to a 3D triangular mesh primitive. This enables hybrid representations where splats are anchored to an underlying mesh geometry. The stitching mechanism is compatible with all SH encoding modes.

Two stitching modes are supported, which are mutually exclusive:

* **Vertex-based**: When `vertices` is present, each Gaussian splat is located at a vertex of the referenced mesh. The `faces` and `weights` properties shall not be present.
* **Face-based**: When `faces` is present, each Gaussian splat is located on a triangular face of the referenced mesh. The `weights` property is mandatory and `vertices` shall not be present.

**`MPEG_gaussian_splatting_transport.stitching` Properties**

|   |Type|Description|Required|
|---|---|---|---|
|**mesh**|`integer`|Index of the mesh in the glTF `meshes` array to which the Gaussian splats are stitched.|No|
|**primitive**|`integer`|Index of the mesh primitive to which the Gaussian splats are stitched.|No|
|**vertices**|`integer`|Accessor index containing vertex indices defining splat locations on mesh vertices.|No|
|**faces**|`integer`|Accessor index containing face indices defining splat locations on triangular faces.|No|
|**weights**|`integer`|Accessor index containing barycentric weight triplets (VEC3).|No|
|**distances**|`integer`|Accessor index containing scalar distance values along the vertex or face normal.|No|
|**displacement**|`integer`|Accessor index containing 3D displacement vectors (VEC3) from the mesh surface.|No|
|**extensions**|`object`|JSON object with extension-specific objects.|No|
|**extras**|[`any`](#reference-any)|Application-specific data.|No|

Additional properties are allowed.

* **JSON schema**: [MPEG_gaussian_splatting_transport.stitching.schema.json](./schema/MPEG_gaussian_splatting_transport.stitching.schema.json)

### MPEG_gaussian_splatting_transport.stitching.mesh

Index of the mesh in the glTF `meshes` array to which the Gaussian splats are stitched. If `mesh` is present and `primitive` is not present, the referenced mesh shall have a single primitive, and that primitive is used as the stitching target. If both `mesh` and `primitive` are present, the splats are attached to `meshes[mesh].primitives[primitive]`.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.primitive

Index of the mesh primitive to which the Gaussian splats are stitched. If `mesh` is not present, `primitive` references a primitive within the same mesh object that contains the `KHR_gaussian_splatting` extension. If `mesh` is present, `primitive` references a primitive of the referenced mesh.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.vertices

Accessor index containing vertex indices that define the location of each Gaussian splat on a mesh vertex. When present, `faces` and `weights` shall not be present.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.faces

Accessor index containing face indices that define the location of each Gaussian splat on a triangular face. When present, `vertices` shall not be present and `weights` is mandatory.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.weights

Accessor index containing barycentric weight triplets (VEC3) that define the position of each Gaussian splat within a triangular face. Required when `faces` is present.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.distances

Accessor index containing scalar distance values. Each value represents the distance from the mesh surface along the normal of the referenced vertex or face.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.displacement

Accessor index containing displacement vectors (VEC3). Each vector represents the 3D displacement from the mesh surface for a Gaussian splat.

* **Type**: `integer`
* **Required**: No
* **Minimum**: ` >= 0`

### MPEG_gaussian_splatting_transport.stitching.extensions

JSON object with extension-specific objects.

* **Type**: `object`
* **Required**: No
* **Type of each property**: Extension

### MPEG_gaussian_splatting_transport.stitching.extras

Application-specific data.

* **Type**: [`any`](#reference-any)
* **Required**: No


## Example

The following example shows a POINTS primitive with `KHR_gaussian_splatting` base data and the `MPEG_gaussian_splatting_transport` extension using the `mpegProgressive` SH layout with progressive download stages and stitching to a mesh.

```json
{
  "asset": {
    "version": "2.0"
  },
  "extensionsUsed": [
    "KHR_gaussian_splatting",
    "MPEG_gaussian_splatting_transport"
  ],
  "extensionsRequired": [
    "KHR_gaussian_splatting",
    "MPEG_gaussian_splatting_transport"
  ],
  "meshes": [
    {
      "primitives": [
        {
          "attributes": {
            "POSITION": 0,
            "COLOR_0": 1,
            "KHR_gaussian_splatting:ROTATION": 2,
            "KHR_gaussian_splatting:SCALE": 3,
            "KHR_gaussian_splatting:OPACITY": 4,
            "KHR_gaussian_splatting:SH_DEGREE_0_COEF_0": 5
          },
          "mode": 0,
          "extensions": {
            "KHR_gaussian_splatting": {
              "kernel": "ellipse",
              "colorSpace": "srgb_rec709_display",
              "sortingMethod": "cameraDistance",
              "projection": "perspective",
              "extensions": {
                "MPEG_gaussian_splatting_transport": {
                  "shEncoding": {
                    "layout": "mpegProgressive",
                    "maxDegree": 3,
                    "dcFromColor0": false,
                    "mpegProgressive": {
                      "first": 6,
                      "second": 7,
                      "third": 8
                    },
                    "progressive": {
                      "stages": [
                        { "accessors": [0, 1, 2, 3, 4, 5] },
                        { "accessors": [6] },
                        { "accessors": [7] },
                        { "accessors": [8] }
                      ]
                    }
                  },
                  "stitching": {
                    "mesh": 1,
                    "primitive": 0,
                    "faces": 9,
                    "weights": 10,
                    "distances": 11
                  }
                }
              }
            }
          }
        }
      ]
    }
  ]
}
```

**SH Encoding.** The `mpegProgressive` layout packs all coefficients of the same SH degree into a single flat SCALAR accessor with interleaved RGB values. Accessor 6 (`first`) carries 9 scalars per splat for degree 1, accessor 7 (`second`) carries 15 for degree 2, and accessor 8 (`third`) carries 21 for degree 3. Each degree is a self-contained fetch unit. With `dcFromColor0` set to false, the degree-0 (DC) term is stored explicitly in accessor 5 (`SH_DEGREE_0_COEF_0`) rather than being derived from `COLOR_0`.

**Progressive Download.** The four stages define the incremental fetch order. Stage 0 delivers geometry and DC color (accessors 0-5), enabling immediate rendering. Stages 1-3 each deliver one SH degree, progressively adding view-dependent lighting detail. No previously fetched data needs to be re-decoded. The degree-group packing ensures each stage boundary aligns with a complete SH degree, avoiding partial states.

**Stitching.** The splats are anchored to the triangle mesh at `meshes[1].primitives[0]`. Accessor 9 (`faces`) identifies which triangle each splat sits on, accessor 10 (`weights`) provides barycentric coordinates within that triangle, and accessor 11 (`distances`) offsets each splat along the face normal. At render time, each splat's position is computed from the face vertices and barycentric weights, overriding the base `POSITION` attribute. If the target mesh is animated, the splats move with it.

## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC 23090-14](https://www.iso.org/standard/80900.html), Information technology — Coded representation of immersive media — Part 14: Scene Description
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media
* [KHR_gaussian_splatting](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_gaussian_splatting) - Base extension for 3D Gaussian splats in glTF
* [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) - Original 3DGS paper

## License

Copyright ISO/IEC 2022

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
