<!--
Copyright 2026 Ben Houston
SPDX-License-Identifier: CC-BY-4.0
-->

# KHR_lightmap

## Contributors

* Ben Houston, [@bhouston](https://github.com/bhouston)

## Status

Draft proposal for discussion in the bhouston fork. Not ratified by Khronos. No conformant implementations or rendered sample assets are claimed.

## Dependencies

Written against the [glTF 2.1 draft](../../../../specification/2.1/Specification.adoc), including this branch's **proposed** [core texture transforms](../../../../specification/2.1/Specification.adoc#texture-coordinate-transforms). Upstream `draft-2.1` does not yet contain that promotion. Assets using this extension **MUST** set `asset.version` and `asset.minVersion` to `"2.1"` or later within major version 2.

GPU instances use [EXT_mesh_gpu_instancing](../../../2.0/Vendor/EXT_mesh_gpu_instancing/README.md). Unlit preview materials may use [KHR_materials_unlit](../../../2.0/Khronos/KHR_materials_unlit/README.md).

## Overview

A light map stores precomputed diffuse irradiance on a surface. It can represent indirect bounce lighting, color bleeding, and soft shadows with one texture lookup. This proposal preserves the irradiance, diffuse BRDF, ambient occlusion, and author-controlled baked/runtime split of `MOZ_lightmap`, while adding explicit HDR image support and unique lighting for shared mesh instances. It defines new behavior under a new name; it does not change existing `MOZ_lightmap` assets or implementations.

Bindings may be shared through a material or replaced per primitive on a mesh node. Core texture transforms place each binding in an atlas. Custom `EXT_mesh_gpu_instancing` attributes place individual GPU instances in that atlas. HDR uses a small, explicitly defined KTX2 half-float profile, without RGBM or RGBE shader decoding.

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
                "transform": { "offset": [0, 0], "scale": [0.5, 0.5] }
            },
            "intensity": 1
        }
    }
}
```

| Property | Type | Required | Meaning |
|---|---|---|---|
| `lightmapTexture` | glTF 2.1 `textureInfo` | Yes | Irradiance texture, UV set, and core transform. |
| `intensity` | number ≥ 0 | No, default `1` | Dimensionless linear multiplier, applied after image decoding. |

`texCoord` has the core default **0**. Exporters **SHOULD** write it explicitly; lightmaps commonly use `TEXCOORD_1`. Every affected primitive **MUST** have the selected UV attribute. All numbers **MUST** be finite. Texture and image indices **MUST** reference existing entries in the asset.

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
                        "transform": { "offset": [0.5, 0], "scale": [0.5, 0.5] }
                    }
                }
            ]
        }
    }
}
```

The array **MUST** be nonempty; primitive indices **MUST** exist and **MUST NOT** repeat. An entry completely replaces the material binding for that primitive on this node, including the texture, UV set, transform, and intensity (which defaults to `1` independently). Missing fields are not inherited. A node entry may supply a binding even when the material has none; a primitive without an explicit material uses the core default material. Primitives absent from the array retain their material bindings, if any. Parent node bindings are not inherited by children.

Two nodes referencing the same mesh can therefore use different regions or different textures without duplicating geometry or materials. An entry with `intensity: 0` disables the baked contribution on that primitive. Node spatial transforms do not change lightmap UV transforms.

### GPU instances

For nodes using `EXT_mesh_gpu_instancing`, the following optional attributes are defined in its `attributes` object:

| Attribute | Accessor type | Component type | Default when absent |
|---|---|---|---|
| `_LIGHTMAP_OFFSET` | `VEC2` | `5126` (`FLOAT`) | `[0, 0]` |
| `_LIGHTMAP_SCALE` | `VEC2` | `5126` (`FLOAT`) | `[1, 1]` |
| `_LIGHTMAP_ROTATION` | `SCALAR` | `5126` (`FLOAT`) | `0` radians |

These accessors **MUST NOT** be normalized. Components **MUST** be finite. Counts **MUST** equal those of every other instance attribute on the node. Their values form a per-instance transform using the **same** core texture transform matrix convention. They apply only to effective lightmap bindings, across all primitives of the instanced mesh; they do not affect other material textures. These attributes **MUST NOT** occur as mesh vertex attributes, and the node **MUST** have at least one effective lightmap binding. `KHR_lightmap` **MUST** be listed in `extensionsUsed` when they are used, even when the bindings are supplied only by materials.

Binding selection happens first. Let `B` be the selected binding's core texture transform and `I_j` the transform for GPU instance `j`. Using column vectors:

```text
uv_lightmap = I_j * B * vec3(TEXCOORD_n, 1)
```

Thus the binding's transform is applied **first**, then the instance's transform, then sampler wrapping and filtering. Missing attributes give identity components. No GPU instance attributes means `I_j` is identity. A node replacement is not an extra transform layer. Rotation is fully supported and **MUST NOT** be ignored; exporters may use scale/offset only for easier interoperability. For example, `(0.2, 0.4)` with binding scale `(0.5, 0.5)`, instance scale `(0.5, 1)`, and instance offset `(0.5, 0)` becomes `(0.55, 0.2)`.

Use the usual `EXT_mesh_gpu_instancing` declaration and fallback requirements. Per-instance texture indices, UV set selection, or per-primitive instance attributes are not defined; split batches when those differ. Sparse accessors are interpreted according to core accessor rules.

## Irradiance and HDR

Decoded RGB values represent diffuse irradiance in linear BT.709/sRGB primaries, using the photometric convention of `KHR_lights_punctual`, in lux (lm/m²). Alpha **MUST** be ignored. Values **MUST** be finite and nonnegative. Implementations **MUST** preserve values above `1` through texture sampling and lighting calculations, before display mapping or exposure:

```text
E = intensity * lightmapLinear
```

There are two image paths:

* **PNG/JPEG:** RGB **MUST** be sRGB encoded and decoded to linear before multiplying by intensity. Decode **SHOULD** occur before filtering. HDR can be represented by normalizing linear irradiance to `[0, 1]` and storing the removed scale in `intensity`. This is useful for compatibility, but 8-bit precision may be insufficient for dark regions of high-contrast bakes.
* **KTX2 half-float:** use the HDR texture source below. RGB is already linear and **MUST NOT** undergo sRGB decoding, tone mapping, clamping to `[0, 1]`, or alpha-based unpacking. Store the actual irradiance with intensity `1`, or use any consistent linear normalization and multiplier.

Baking tools that output irradiance divided by π (so that outgoing diffuse color equals albedo times their value) **MUST** multiply those values, or the exported intensity, by π. Do not bake base color, metallic response, or display exposure into irradiance.

### HDR texture source

Add a `KHR_lightmap` object to a **texture**, with `source` identifying an HDR image. This uses the existing alternative-source pattern rather than giving the lightmap binding another texture index:

```json
{
    "asset": { "version": "2.1", "minVersion": "2.1" },
    "extensionsUsed": ["KHR_lightmap"],
    "extensionsRequired": ["KHR_lightmap"],
    "textures": [
        { "extensions": { "KHR_lightmap": { "source": 0 } } }
    ],
    "images": [
        { "uri": "irradiance.ktx2", "mimeType": "image/ktx2" }
    ]
}
```

This is an illustrative fragment; a renderable asset also supplies bindings, geometry, and a scene. Image URIs, data URIs, and GLB buffer views follow core image rules. `mimeType`, when present, **MUST** be `image/ktx2`; it **MUST** be present for a buffer-view image.

An implementation supporting `KHR_lightmap` **MUST** choose its HDR `source` when present. An optional core `texture.source` may reference a PNG/JPEG fallback. With no core fallback, `KHR_lightmap` **MUST** be in `extensionsRequired`. This HDR texture source **MUST NOT** coexist on the same texture with another alternative-source extension. Textures carrying it **MUST** be used exclusively by lightmap bindings, because the linear HDR sampling semantics are specific to this proposal.

The image **MUST** conform to [KTX version 2](https://registry.khronos.org/KTX/specs/2.0/ktxspec.v2.html), with this restricted profile:

| Field or content | Requirement |
|---|---|
| `vkFormat` | `VK_FORMAT_R16G16B16A16_SFLOAT` (`97`) |
| `typeSize` | `2` |
| Dimensions | 2D: positive width and height; `pixelDepth = 0`, `layerCount = 0`, `faceCount = 1` |
| `supercompressionScheme` | `0` (none) or `2` (Zstandard); conformant readers **MUST** support both |
| DFD | Must match RGBA16F; `colorPrimaries = KHR_DF_PRIMARIES_BT709`, `transferFunction = KHR_DF_TRANSFER_LINEAR` |
| RGB | Finite, nonnegative half-float values; no NaN or infinity |
| Alpha | Finite; ignored, never premultiplied |
| `KTXorientation` | `rd` or omitted |
| `KTXswizzle` | `rgba` or omitted |
| Premultiplication | `KHR_DF_FLAG_ALPHA_PREMULTIPLIED` **MUST NOT** be set |
| Mipmaps | `levelCount >= 1`; full pyramid **SHOULD** be included for mipmapped sampling |

Mip levels **MUST** contain filtered linear irradiance. An implementation may convert to a supported GPU storage format, but **MUST** retain HDR range and linear sampling. No particular GPU API format is required. This profile avoids choosing a platform-specific compressed GPU format; optional Zstandard reduces transmission size, not GPU texture memory.

`KHR_texture_basisu` currently defines LDR ETC1S and UASTC, **not** HDR Basis payloads. New HDR encoder support in Basis/KTX tooling is not sufficient to make those payloads conformant to that glTF extension. The draft `KHR_texture_float_*` proposals are related work, not dependencies of this proposal. A later shared HDR image extension could replace this narrow profile after agreement; this draft defines everything a reader needs today.

A PNG/JPEG fallback uses the **same** binding intensity as the HDR source. It **SHOULD** approximate the same decoded values; values beyond its representable range may be clipped. Exporters seeking a closer fallback may normalize both sources with a common intensity, at the cost of fallback precision. There is no source-dependent intensity or hidden exposure adjustment.

## Shading and Ambient Occlusion

Lightmap irradiance **MUST** contribute additively through the material's diffuse reflection BRDF, like diffuse image-based lighting. For the core metallic-roughness material, before additional material-layer attenuation:

```text
c_diff = (1 - metallic) * baseColor.rgb
L_lightmap = occlusionFactor * (c_diff / π) * E
occlusionFactor = 1 + occlusionTexture.strength * (occlusionTexture.r - 1)
```

Use the core linear base color, including its texture, factor, and vertex-color terms. Without an occlusion texture, `occlusionFactor = 1`. The occlusion texture retains its own UV set and transform. The lightmap term **MUST** follow the material's diffuse-reflection lobe attenuation, including transmission and additional layers when present. It **MUST NOT** contribute to specular reflection or be used as illumination for a transmission BTDF. A fully metallic material therefore has no core diffuse lightmap term.

The AO rule applies to **all** lightmap irradiance, even when some direct illumination was baked: the combined bake is treated as indirect lighting for this purpose. AO remains subject to the core rules for other runtime lighting terms. Apply it once to the lightmap contribution, without double multiplying an already occluded sum.

Lightmaps store a nondirectional value per texel. A normal map cannot reorient the baked illumination; both sides of a double-sided primitive receive the same value. Authors **SHOULD** omit AO already captured in the bake, or use AO only for detail finer than the lightmap resolution, to avoid darkening the same features twice.

### Unlit baked preview

When combined with `KHR_materials_unlit`, this extension explicitly replaces the RGB output with:

```text
color.rgb = baseColor.rgb * (E / π) * occlusionFactor
```

Alpha, alpha mode, and other unlit behavior are unchanged. Runtime lights and environment lighting do not apply. Stored values **remain irradiance**, and the division by π gives a Lambertian baked preview; this differs intentionally from MOZ's historical multiplier convention. A migration of a MOZ unlit multiplier must multiply that multiplier by π to preserve appearance. No metallic, specular, or transmission terms are introduced in this unlit mode.

## Baked and Runtime Lighting

Authors may bake indirect diffuse only, total diffuse, or any chosen mixture of direct and indirect diffuse contributions from scene lights and environments. These examples are not exhaustive. The format and shading do not change with that choice, and this extension does not prescribe bake parameters or a particular renderer lighting setup.

Content creators **SHOULD** ensure every baked contribution is not also computed at runtime, including diffuse environment lighting. Runtime lights may still supply direct diffuse and specular when only indirect diffuse was baked, or specular alone when their diffuse contribution was baked. This extension does not add light masks or require disabling an entire runtime light. Coordinating those terms remains an authoring and renderer decision. Lightmaps assume the geometry, visibility, and lighting at bake time; moving or deforming baked geometry may invalidate those assumptions.

UV charts **SHOULD** be non-overlapping and padded/dilated for all used mip levels. Atlas transforms do not provide automatic chart clamping; clamp-to-edge clamps the whole image, not an atlas region. Authors should use appropriate sampler wrapping and padding.

## Fallback and Migration

`KHR_lightmap` **MUST** be in `extensionsUsed`. Assets whose appearance depends on baked lighting **SHOULD** also require it; optional assets render without its baked contribution in unsupported clients. Follow the HDR source and GPU instancing requirements independently. Core texture transforms require glTF 2.1 support via `asset.minVersion`, not a `KHR_texture_transform` declaration.

This proposal does not relabel deployed MOZ data. Migration wraps MOZ's texture info in `lightmapTexture`, writes the effective `texCoord` explicitly (MOZ's default is `1`), moves legacy transforms to core `transform`, preserves the linear irradiance scale for PBR, and converts unlit normalization as above. HDR vendor encodings such as `MOZ_texture_rgbe` or RGBM **MUST** be decoded to irradiance before re-encoding; simply renaming extension objects is insufficient. Existing Hubs implementations are not claimed as KHR implementations.

## Schemas and Examples

* [Material binding](schema/material.KHR_lightmap.schema.json)
* [Node bindings](schema/node.KHR_lightmap.schema.json)
* [Node primitive binding](schema/lightmapBinding.schema.json)
* [HDR texture source](schema/texture.KHR_lightmap.schema.json)
* [Core texture transform proposal](../../../../specification/2.1/schema/textureTransform.schema.json)
* [Ordinary node atlas example](examples/node-atlas.gltf): two nodes share geometry and material but select different atlas regions.
* [GPU atlas example](examples/gpu-atlas.gltf): the same two regions selected through instance attributes.

The examples are self-contained structural fixtures with a two-texel PNG atlas and nearest filtering. Their appearance has not been verified in a KHR_lightmap renderer.

To run the proposal checks, from this directory run `npm ci`, `npm test`, and `npm run lint`. There is no TypeScript implementation to typecheck.

JSON schemas validate structure. Index bounds, distinct primitive indices, HDR contents, allowed image usage, extension declarations, transform conflicts, and instance accessor constraints require semantic validation. The accompanying validation script checks schemas, illustrative examples, and fixture accessor constraints; it is not a complete glTF validator or rendering implementation.

## Feedback Addressed

This proposal follows the complete discussion and inline review of [KhronosGroup/glTF#2672](https://github.com/KhronosGroup/glTF/pull/2672):

* hybridherbst: defined HDR and diffuse BRDF behavior, atlas offset/scale, and an explicit rotation convention; core transforms proposed for 2.1.
* robertlong: ordinary node instances and atlas transforms, informed by Third Room's [MX_lightmap](https://github.com/matrix-org/thirdroom/tree/main/docs/gltf/MX_lightmap).
* takahirox: separate extension, reuse of texture transform math, and reuse of custom GPU instance attributes with types, defaults, and application order.
* Exairnous: preserve MOZ compatibility under its existing name, account for HDR vendor exports during migration, and require integer UV selection in new assets. Hubs remains a separate existing ecosystem.
* aaronfranke: every new schema includes `$id`.
