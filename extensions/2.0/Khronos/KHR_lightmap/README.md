<!--
Copyright 2026 The Khronos Group Inc.
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR_lightmap

## Contributors

* Ben Houston, [@bhouston](https://github.com/bhouston)
* Felix Herbst, [@hybridherbst](https://github.com/hybridherbst)
* Robert Long, [@robertlong](https://github.com/robertlong)
* Takahiro, [@takahirox](https://github.com/takahirox)
* [@Exairnous](https://github.com/Exairnous)
* Aaron Franke, [@aaronfranke](https://github.com/aaronfranke)

## Status

Draft proposal for discussion. Not ratified by Khronos.

## Dependencies

Written against the [glTF 2.0 specification](../../../../specification/2.0/Specification.adoc). Texture coordinate transforms use [KHR_texture_transform](../../../2.0/Khronos/KHR_texture_transform/README.md). GPU instances use [EXT_mesh_gpu_instancing](../../../2.0/Vendor/EXT_mesh_gpu_instancing/README.md). Unlit preview materials may use [KHR_materials_unlit](../../../2.0/Khronos/KHR_materials_unlit/README.md).

## Overview

A light map stores precomputed diffuse irradiance on a surface, such as indirect bounce lighting, color bleeding, and soft shadows. Bindings may be shared through a material or replaced per primitive on a mesh node, and texture transforms place each binding in an atlas. Custom `EXT_mesh_gpu_instancing` attributes place individual GPU instances in that atlas and scale their intensity.

## Bindings

### Material default

Add `KHR_lightmap` to a material's `extensions` object:

```json
{
    "pbrMetallicRoughness": { "metallicFactor": 0 },
    "extensions": {
        "KHR_lightmap": {
            "lightmapTexture": {
                "index": 0,
                "texCoord": 1,
                "extensions": { "KHR_texture_transform": { "offset": [0, 0], "scale": [0.5, 0.5] } }
            },
            "intensity": 1
        }
    }
}
```

| Property | Type | Required | Meaning |
|---|---|---|---|
| `lightmapTexture` | glTF 2.0 `textureInfo` | Yes | Irradiance texture, UV set, and texture transform. |
| `intensity` | number ≥ 0 | No, default `1` | Dimensionless linear multiplier, applied after image decoding. |

`texCoord` has the core default **0**. Exporters **SHOULD** write it explicitly; lightmaps commonly use `TEXCOORD_1`. The binding's *effective UV set* is its `texCoord` after any `KHR_texture_transform` override, and every affected primitive **MUST** have it.

### Node replacement

For ordinary mesh instances, add `KHR_lightmap` to the node's `extensions` object. The node **MUST** reference a mesh. Each `lightmaps` entry identifies a zero-based index into that mesh's `primitives` array and provides a complete binding:

```json
{
    "mesh": 0,
    "extensions": {
        "KHR_lightmap": {
            "lightmaps": [
                {
                    "primitive": 0,
                    "lightmapTexture": {
                        "index": 0,
                        "texCoord": 1,
                        "extensions": { "KHR_texture_transform": { "offset": [0.5, 0], "scale": [0.5, 0.5] } }
                    }
                }
            ]
        }
    }
}
```

The array **MUST** be nonempty; primitive indices **MUST** exist and **MUST NOT** repeat. An entry completely replaces the material binding for that primitive on this node, including the texture, UV set, transform, and intensity (which defaults to `1` independently). A node entry may supply a binding even when the material has none. Primitives absent from the array retain their material bindings, if any. Parent node bindings are not inherited by children.

> **Implementation Note**: Node bindings are keyed by primitive index; tools that reorder, merge, split, or remove primitives must update them.

### GPU instances

For nodes using `EXT_mesh_gpu_instancing`, the following optional attributes are defined in its `attributes` object:

| Attribute | Accessor type | Component type | Default when absent |
|---|---|---|---|
| `_LIGHTMAP_OFFSET` | `VEC2` | `5126` (`FLOAT`) | `[0, 0]` |
| `_LIGHTMAP_SCALE` | `VEC2` | `5126` (`FLOAT`) | `[1, 1]` |
| `_LIGHTMAP_ROTATION` | `SCALAR` | `5126` (`FLOAT`) | `0` radians |
| `_LIGHTMAP_INTENSITY` | `SCALAR` | `5126` (`FLOAT`) | `1` |

These accessors **MUST NOT** be normalized or contain infinite or NaN values, and `_LIGHTMAP_INTENSITY` values **MUST** be nonnegative. Counts **MUST** equal those of every other instance attribute on the node. Offset, scale, and rotation form a per-instance transform using the `KHR_texture_transform` matrix convention. These attributes apply only to effective lightmap bindings, across all primitives of the instanced mesh. They **MUST NOT** occur as mesh vertex attributes, and the node **MUST** have at least one effective lightmap binding.

Binding selection happens first. Let `B` be the selected binding's `KHR_texture_transform` and `I_j` the transform for GPU instance `j`, with identity components for missing attributes. Using column vectors:

```text
uv_lightmap = I_j * B * vec3(TEXCOORD_n, 1)
```

`TEXCOORD_n` is the binding's effective UV set. Sampler wrapping and filtering apply to the result.

Per-instance texture indices, UV set selection, or per-primitive instance attributes are not defined; split batches when those differ.

## Irradiance and Image Formats

Decoded RGB values represent diffuse irradiance in linear BT.709/sRGB primaries, using the photometric convention of `KHR_lights_punctual`, in lux (lm/m²). Values **MUST** be finite and nonnegative. Implementations **MUST** preserve values above `1` through texture sampling and lighting calculations, before display mapping or exposure:

```text
E = intensity * instanceIntensity * lightmapLinear
```

`instanceIntensity` is the GPU instance's `_LIGHTMAP_INTENSITY`, or `1` when absent. Both multipliers are unitless; `E` is in lux.

Without an image-format extension, images **MUST** be PNG or JPEG, and their RGB **MUST** contain linear irradiance; implementations **MUST NOT** apply sRGB decoding. Image-format extensions **MUST** decode to linear RGB irradiance. Alpha **MUST** be ignored for lighting. Authors may normalize irradiance to `[0, 1]` and store the removed scale in `intensity`.

> **Implementation Note**: 8-bit linear storage bands visibly in dark regions; prefer an image-format extension with higher precision or HDR.

Baking tools that output irradiance divided by π (so that outgoing diffuse color equals albedo times their value) **MUST** multiply those values, or the exported intensity, by π. Do not bake base color, metallic response, or display exposure into irradiance.

## Shading and Ambient Occlusion

Lightmap irradiance **MUST** contribute additively through the material's diffuse reflection BRDF, like diffuse image-based lighting. For the core metallic-roughness material, before additional material-layer attenuation:

```text
c_diff = (1 - metallic) * baseColor.rgb
L_lightmap = occlusionFactor * (c_diff / π) * E
occlusionFactor = 1 + occlusionTexture.strength * (occlusionTexture.r - 1)
```

Without an occlusion texture, `occlusionFactor = 1`. The occlusion texture retains its own UV set and transform. The lightmap term **MUST** follow the material's diffuse-reflection lobe attenuation, including transmission and additional layers when present. It **MUST NOT** contribute to specular reflection or be used as illumination for a transmission BTDF.

All lightmap irradiance is treated as indirect lighting for occlusion, even when direct lighting was baked. This deliberately deviates from the core rule that direct lighting is not occluded, because a lightmap cannot separate the two. Apply occlusion once to the lightmap contribution.

Bakers **SHOULD NOT** multiply ambient occlusion into the lightmap; `occlusionTexture` is already applied at runtime, so it would darken twice.

Lightmaps store a nondirectional value per texel. A normal map cannot reorient the baked illumination; both sides of a double-sided primitive receive the same value.

### Unlit baked preview

When combined with `KHR_materials_unlit`, this extension replaces the RGB output with:

```text
color.rgb = baseColor.rgb * (E / π) * occlusionFactor
```

Alpha and alpha mode are unchanged.

## Baked and Runtime Lighting

Authors may bake any mixture of direct and indirect diffuse contributions; the format and shading are the same. Content creators **SHOULD** ensure every baked contribution, including diffuse environment lighting, is not also computed at runtime. Lightmaps assume the geometry, visibility, and lighting at bake time.

UV charts **SHOULD** be non-overlapping and padded for all used mip levels.

## Extension Declarations

`KHR_lightmap` **MUST** be in `extensionsUsed`. Assets whose appearance depends on baked lighting **SHOULD** also require it.

## Schemas and Examples

* [Material binding](schema/material.KHR_lightmap.schema.json)
* [Node bindings](schema/node.KHR_lightmap.schema.json)
* [Node primitive binding](schema/lightmapBinding.schema.json)
* [Ordinary node atlas example](examples/node-atlas.gltf): two nodes share geometry and material but select different atlas regions.
* [GPU atlas example](examples/gpu-atlas.gltf): the same two regions selected through instance attributes, with the second instance at half intensity.
