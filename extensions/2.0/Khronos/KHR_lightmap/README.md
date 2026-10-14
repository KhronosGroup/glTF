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

Draft proposal for discussion. Not ratified by Khronos. No conformant implementations or rendered sample assets are claimed.

## Dependencies

Written against the [glTF 2.0 specification](../../../../specification/2.0/Specification.adoc). Texture coordinate transforms use [KHR_texture_transform](../../../2.0/Khronos/KHR_texture_transform/README.md). GPU instances use [EXT_mesh_gpu_instancing](../../../2.0/Vendor/EXT_mesh_gpu_instancing/README.md). Unlit preview materials may use [KHR_materials_unlit](../../../2.0/Khronos/KHR_materials_unlit/README.md).

## Overview

A light map stores precomputed diffuse irradiance on a surface. It can represent indirect bounce lighting, color bleeding, and soft shadows with one texture lookup. This extension defines irradiance bindings, diffuse BRDF integration, ambient occlusion, and unique lighting for shared mesh instances. Authors control which lighting contributions are baked and which are computed at runtime.

Bindings may be shared through a material or replaced per primitive on a mesh node. Texture transforms place each binding in an atlas. Custom `EXT_mesh_gpu_instancing` attributes place individual GPU instances in that atlas and scale their intensity. Lightmap textures support glTF image-format extensions.

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

`texCoord` has the core default **0**. Exporters **SHOULD** write it explicitly; lightmaps commonly use `TEXCOORD_1`. When the binding's `KHR_texture_transform` object supplies its own `texCoord`, that value overrides `textureInfo.texCoord` as defined by `KHR_texture_transform`. The resulting UV set is the binding's *effective UV set*, and every affected primitive **MUST** have it. All numbers **MUST** be finite. Texture and image indices **MUST** reference existing entries in the asset.

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

The array **MUST** be nonempty; primitive indices **MUST** exist and **MUST NOT** repeat. An entry completely replaces the material binding for that primitive on this node, including the texture, UV set, transform, and intensity (which defaults to `1` independently). Missing fields are not inherited. A node entry may supply a binding even when the material has none; a primitive without an explicit material uses the core default material. Primitives absent from the array retain their material bindings, if any. Parent node bindings are not inherited by children.

Two nodes referencing the same mesh can therefore use different regions or different textures without duplicating geometry or materials. An entry with `intensity: 0` disables the baked contribution on that primitive. Node spatial transforms do not change lightmap UV transforms.

> **Implementation Note**: Node bindings are keyed by primitive index. Tools that reorder, merge, split, or remove a mesh's primitives (for example, optimizers that join or deduplicate primitives) change those indices. Such tools should understand `KHR_lightmap` and update or remove the affected node bindings. A tool unaware of this extension can silently attach a binding to the wrong primitive.

### GPU instances

For nodes using `EXT_mesh_gpu_instancing`, the following optional attributes are defined in its `attributes` object:

| Attribute | Accessor type | Component type | Default when absent |
|---|---|---|---|
| `_LIGHTMAP_OFFSET` | `VEC2` | `5126` (`FLOAT`) | `[0, 0]` |
| `_LIGHTMAP_SCALE` | `VEC2` | `5126` (`FLOAT`) | `[1, 1]` |
| `_LIGHTMAP_ROTATION` | `SCALAR` | `5126` (`FLOAT`) | `0` radians |
| `_LIGHTMAP_INTENSITY` | `SCALAR` | `5126` (`FLOAT`) | `1` |

These accessors **MUST NOT** be normalized. Components **MUST** be finite, and `_LIGHTMAP_INTENSITY` values **MUST** be nonnegative. Counts **MUST** equal those of every other instance attribute on the node. Offset, scale, and rotation form a per-instance transform using the **same** `KHR_texture_transform` matrix convention. `_LIGHTMAP_INTENSITY` is a dimensionless per-instance multiplier applied in addition to the binding's `intensity`; it lets instances in one batch use differently normalized bakes. These attributes apply only to effective lightmap bindings, across all primitives of the instanced mesh; they do not affect other material textures. These attributes **MUST NOT** occur as mesh vertex attributes, and the node **MUST** have at least one effective lightmap binding. `KHR_lightmap` **MUST** be listed in `extensionsUsed` when they are used, even when the bindings are supplied only by materials.

Binding selection happens first. Let `B` be the selected binding's `KHR_texture_transform` and `I_j` the transform for GPU instance `j`. Using column vectors:

```text
uv_lightmap = I_j * B * vec3(TEXCOORD_n, 1)
```

`TEXCOORD_n` is the binding's effective UV set. Thus the binding's transform is applied **first**, then the instance's transform, then sampler wrapping and filtering. Missing attributes give identity components. No GPU instance attributes means `I_j` is identity. A node replacement is not an extra transform layer. Rotation is fully supported and **MUST NOT** be ignored; exporters may use scale/offset only for easier interoperability. For example, `(0.2, 0.4)` with binding scale `(0.5, 0.5)`, instance scale `(0.5, 1)`, and instance offset `(0.5, 0)` becomes `(0.55, 0.2)`.

Use the usual `EXT_mesh_gpu_instancing` declaration and fallback requirements. Per-instance texture indices, UV set selection, or per-primitive instance attributes are not defined; split batches when those differ.

## Irradiance and Image Formats

Decoded RGB values represent diffuse irradiance in linear BT.709/sRGB primaries, using the photometric convention of `KHR_lights_punctual`, in lux (lm/m²). Values **MUST** be finite and nonnegative. Implementations **MUST** preserve values above `1` through texture sampling and lighting calculations, before display mapping or exposure:

```text
E = intensity * instanceIntensity * lightmapLinear
```

`instanceIntensity` is the GPU instance's `_LIGHTMAP_INTENSITY` value, or `1` when absent or when the node is not GPU instanced.

`lightmapTexture.index` references an ordinary glTF texture, whose image is selected using core `texture.source` or an applicable texture/image extension. Without a separate image-format extension, images **MUST** use formats supported by glTF 2.0 (PNG or JPEG).

For core PNG/JPEG images, RGB **MUST** contain linear irradiance samples; implementations **MUST NOT** apply sRGB decoding. Image color-space metadata is ignored according to core glTF rules. Alpha **MUST** be ignored for lighting. Authors may normalize irradiance to `[0, 1]` and store the removed scale in `intensity`.

> **Implementation Note**: 8-bit PNG and JPEG store linear values with too little precision for most lighting: dark regions band visibly, and high-contrast bakes lose detail. This matches the core requirement that occlusion textures be linear, but it limits quality. Authors who need better quality should use an image-format extension that provides higher precision or HDR storage.

Baking tools that output irradiance divided by π (so that outgoing diffuse color equals albedo times their value) **MUST** multiply those values, or the exported intensity, by π. Do not bake base color, metallic response, or display exposure into irradiance.

### Image Format Extensions

`KHR_lightmap` supports glTF image-format extensions for additional image formats.

The image-format extension's source-selection and decoding rules **MUST** be followed. Decoded samples **MUST** provide linear RGB irradiance before filtering and multiplication by `intensity`. Alpha may be used during image decoding but **MUST** be ignored for lighting.

## Shading and Ambient Occlusion

Lightmap irradiance **MUST** contribute additively through the material's diffuse reflection BRDF, like diffuse image-based lighting. For the core metallic-roughness material, before additional material-layer attenuation:

```text
c_diff = (1 - metallic) * baseColor.rgb
L_lightmap = occlusionFactor * (c_diff / π) * E
occlusionFactor = 1 + occlusionTexture.strength * (occlusionTexture.r - 1)
```

Use the core linear base color, including its texture, factor, and vertex-color terms. Without an occlusion texture, `occlusionFactor = 1`. The occlusion texture retains its own UV set and transform. The lightmap term **MUST** follow the material's diffuse-reflection lobe attenuation, including transmission and additional layers when present. It **MUST NOT** contribute to specular reflection or be used as illumination for a transmission BTDF. A fully metallic material therefore has no core diffuse lightmap term.

The core specification defines the occlusion texture as indicating "areas that would receive less indirect lighting from ambient sources" and states that "direct lighting is not affected." A lightmap stores the sum of whatever contributions were baked and cannot separate direct from indirect diffuse lighting. This extension therefore treats **all** lightmap irradiance as indirect lighting for the purpose of occlusion, even when some direct illumination was baked. This is an intentional deviation from the core definition: it is not exact for baked direct lighting, but it is the only consistent rule available for a combined value. Occlusion remains subject to the core rules for runtime lighting terms. Apply it once to the lightmap contribution, without double multiplying an already occluded sum.

Bakers **SHOULD NOT** multiply an ambient occlusion term into the lightmap. The material's `occlusionTexture` is applied to the lightmap at runtime, and in many implementations it also attenuates runtime ambient lighting, including specular image-based lighting. Baking occlusion into the lightmap as well darkens the same features twice. Removing the occlusion texture to compensate loses occlusion for those runtime terms. Occlusion that the light transport simulation itself resolves at lightmap resolution is inherent to the bake; the occlusion texture then mainly adds detail finer than the lightmap resolution.

Lightmaps store a nondirectional value per texel. A normal map cannot reorient the baked illumination; both sides of a double-sided primitive receive the same value.

### Unlit baked preview

When combined with `KHR_materials_unlit`, this extension explicitly replaces the RGB output with:

```text
color.rgb = baseColor.rgb * (E / π) * occlusionFactor
```

Alpha, alpha mode, and other unlit behavior are unchanged. Runtime lights and environment lighting do not apply. Stored values **remain irradiance**, and the division by π gives a Lambertian baked preview. No metallic, specular, or transmission terms are introduced in this unlit mode.

## Baked and Runtime Lighting

Authors may bake indirect diffuse only, total diffuse, or any chosen mixture of direct and indirect diffuse contributions from scene lights and environments. These examples are not exhaustive. The format and shading do not change with that choice, and this extension does not prescribe bake parameters or a particular renderer lighting setup.

Content creators **SHOULD** ensure every baked contribution is not also computed at runtime, including diffuse environment lighting. Runtime lights may still supply direct diffuse and specular when only indirect diffuse was baked, or specular alone when their diffuse contribution was baked. This extension does not add light masks or require disabling an entire runtime light. Coordinating those terms remains an authoring and renderer decision. Lightmaps assume the geometry, visibility, and lighting at bake time; moving or deforming baked geometry may invalidate those assumptions.

UV charts **SHOULD** be non-overlapping and padded/dilated for all used mip levels. Atlas transforms do not provide automatic chart clamping; clamp-to-edge clamps the whole image, not an atlas region. Authors should use appropriate sampler wrapping and padding.

## Extension Declarations

`KHR_lightmap` **MUST** be in `extensionsUsed`. Assets whose appearance depends on baked lighting **SHOULD** also require it; optional assets render without its baked contribution in unsupported clients. Follow image-format extension and GPU instancing requirements independently. Declare `KHR_texture_transform` according to its extension requirements when texture transforms are used.

## Schemas and Examples

* [Material binding](schema/material.KHR_lightmap.schema.json)
* [Node bindings](schema/node.KHR_lightmap.schema.json)
* [Node primitive binding](schema/lightmapBinding.schema.json)
* [Ordinary node atlas example](examples/node-atlas.gltf): two nodes share geometry and material but select different atlas regions.
* [GPU atlas example](examples/gpu-atlas.gltf): the same two regions selected through instance attributes.

The examples are self-contained structural fixtures with a two-texel PNG atlas and nearest filtering. Their appearance has not been verified in a KHR_lightmap renderer.

JSON schemas validate structure. Index bounds, distinct primitive indices, decoded irradiance values, image-format compatibility, extension declarations, and instance accessor constraints require semantic validation.
