<!--
Copyright 2026 Ben Houston
SPDX-License-Identifier: CC-BY-4.0
-->

# MOZ\_lightmap

## Contributors

* Takahiro Aoyagi, Mozilla, [@takahirox](https://github.com/takahirox)
* Ben Houston, [@bhouston](https://github.com/bhouston)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

This extension may be used together with `KHR_texture_transform`, `KHR_materials_unlit`, and `KHR_lights_punctual`.

## Overview

A light map is a texture containing precomputed (baked) diffuse lighting for a surface. Light maps let real-time renderers display global illumination effects such as indirect bounce lighting, color bleeding, and soft shadowing at the cost of a single texture lookup. They are widely used in games, architectural visualization, and web and XR scenes where the scene's static geometry and lighting are known ahead of time.

This extension adds a light map to a glTF material. The light map stores sRGB-encoded RGB baked diffuse lighting, usually in a dedicated, non-overlapping texture coordinate set. The renderer decodes these values to linear RGB before shading. For PBR materials, the renderer interprets the decoded values, scaled by `intensity`, as diffuse irradiance and adds them to the material's diffuse lighting. Unlit materials use the multiplication convention defined in [Unlit Materials](#unlit-materials). Light maps do not affect specular reflections.

The baked irradiance may contain either:

* **indirect diffuse lighting only**, for scenes where direct lighting from some or all lights is still computed at runtime, or
* **total diffuse lighting** (direct and indirect), for fully baked scenes.

Both are diffuse irradiance and are rendered the same way. The content creator chooses which one to bake and which lights to keep in the scene (see [Baked and Dynamic Lights](#baked-and-dynamic-lights)).

<figure>
<img src="./figures/cornell_box.jpg"/>
<figcaption><em>A Cornell box with baked indirect diffuse lighting stored using <code>MOZ_lightmap</code>, combined with a dynamic <code>KHR_lights_punctual</code> point light. Figure by Ben Houston, licensed under <a href="https://creativecommons.org/licenses/by/4.0/">CC BY 4.0</a>.</em></figcaption>
</figure>

## Extending Materials

A light map is added to a material by adding the `MOZ_lightmap` extension to the material's `extensions` property.

```json
"materials": [
    {
        "pbrMetallicRoughness": {
            "baseColorFactor": [ 1.0, 0.0, 0.0, 1.0 ],
            "metallicFactor": 0.0,
            "baseColorTexture": {
                "index": 0
            }
        },
        "extensions": {
            "MOZ_lightmap": {
                "index": 1,
                "texCoord": 1,
                "intensity": 1.0
            }
        }
    }
]
```

The extension object is a [`textureInfo`](../../../../specification/2.0/Specification.adoc#reference-textureinfo) with an extra `intensity` property.

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| **index** | `integer` | The index of the texture that contains the light map. | :white_check_mark: Yes |
| **texCoord** | `integer` | The set index of the texture's `TEXCOORD` attribute used for texture coordinate mapping. | No, default: `1` |
| **intensity** | `number` | A dimensionless linear multiplier applied to the light map's RGB values after sRGB decoding. Must be greater than or equal to `0`. | No, default: `1.0` |

### Texture Coordinates

Light maps usually need a unique, non-overlapping UV layout, which is generally different from the layout used for the material's other textures. For this reason `texCoord` defaults to `1` (`TEXCOORD_1`), **unlike** core `textureInfo`, where it defaults to `0`. This matches existing light-mapped content, which has always used the second texture coordinate set. Exporters **SHOULD** always write `texCoord` explicitly.

The light map `textureInfo` may use `KHR_texture_transform`. One use is placing several materials into one shared light map atlas.

The effective texture coordinate set is `MOZ_lightmap.texCoord` (default `1`), unless a supported `KHR_texture_transform` specifies its own `texCoord`, in which case that value overrides it. Mesh primitives using the light map **MUST** provide the `TEXCOORD_<effective set index>` attribute.

If `KHR_texture_transform` is optional, content creators **MUST** also provide usable fallback texture coordinates for the outer `MOZ_lightmap.texCoord` selection. If the asset relies on the override or transform without a usable fallback, `KHR_texture_transform` **MUST** be listed in `extensionsRequired`.

For example, this material uses `TEXCOORD_2` in clients supporting `KHR_texture_transform`, and `TEXCOORD_1` as its fallback in clients that do not:

```json
{
    "pbrMetallicRoughness": {
        "metallicFactor": 0.0
    },
    "extensions": {
        "MOZ_lightmap": {
            "index": 0,
            "texCoord": 1,
            "extensions": {
                "KHR_texture_transform": {
                    "texCoord": 2,
                    "offset": [ 0.25, 0.0 ],
                    "scale": [ 0.5, 0.5 ]
                }
            }
        }
    }
}
```

Both UV sets are required for this optional-transform fallback. If `KHR_texture_transform` is required instead, only the effective `TEXCOORD_2` set is needed for the light map. All extensions used by an asset, including `MOZ_lightmap` and `KHR_texture_transform` in this example, **MUST** be listed in `extensionsUsed`.

## Light Map Content

The light map texture's RGB values **MUST** be encoded with the sRGB transfer function, using the same encoding convention as the core `emissiveTexture`. Implementations **MUST** decode these values to linear RGB before applying `intensity` or performing shading computations. To achieve correct filtering, the transfer function **SHOULD** be decoded before performing linear interpolation. The alpha channel, if present, **MUST** be ignored.

In the equations below, `lightmapLinear` is the sampled RGB value after sRGB decoding:

```
lightmapLinear = sRGBToLinear(lightmap.rgb)
```

For PBR materials, the diffuse irradiance `E` incident on the surface is:

```
E = intensity * lightmapLinear
```

`E` is expressed in lux (lm/m²). `intensity` is dimensionless; it scales the illuminance represented by the decoded light map values. This is the same photometric convention as `KHR_lights_punctual`, where a directional light's intensity is the illuminance on a surface facing the light. Consistent units and normalization allow baked and runtime diffuse lighting to be combined, but a nondirectional light map cannot reproduce every directional BRDF effect. Exposure, tone mapping, and renderer approximations can also affect the displayed result; this extension does not specify exposure or guarantee identical rendered pixels.

Some baking tools produce values already divided by π (so that "outgoing color = albedo × value"). When exporting for PBR materials, exporters **MUST** convert such values to irradiance by multiplying them, or `intensity`, by π. Unlit materials use the separate normalization defined below.

### Shading

Light map irradiance is diffuse irradiance arriving from the hemisphere above the surface. It **MUST** contribute additively to the material's diffuse reflection through its diffuse BRDF. Runtime lighting, including diffuse image-based lighting, may contribute alongside it. This extension does not prescribe the renderer's image-based lighting calculations. For the core metallic-roughness material, the base Lambertian diffuse contribution, before occlusion and any additional material-layer attenuation, is:

```
f_lightmap = (c_diff / π) * E
c_diff = lerp(baseColor.rgb, black, metallic)
```

Light maps contain no directional information and do not contribute to specular reflection. For PBR materials, the emissive contribution is unchanged. Materials using `KHR_materials_unlit` continue to ignore the core `emissiveFactor` and `emissiveTexture` properties.

The light map contribution **MUST** follow the material's diffuse-reflection lobe attenuation, including reductions caused by `KHR_materials_transmission` or `KHR_materials_diffuse_transmission` when present. It **MUST NOT** be used as an additional source for a transmission BTDF: one RGB irradiance value does not supply separately baked illumination from the opposite hemisphere.

For double-sided materials, both visible sides sample the same light map texel; the extension does not provide separate front- and back-side lighting. Authors needing different bakes for the two sides should use separate geometry and materials. A normal map cannot reconstruct the missing incident-light directions or reorient the baked irradiance to account for normal-mapped detail. Normal maps may still affect runtime lighting.

The core `occlusionTexture` applies to indirect lighting. For this purpose, light map irradiance counts as indirect lighting, so occlusion **MUST** be applied to it, including when this extension is combined with `KHR_materials_unlit`. The occlusion multiplier is:

```
occlusionFactor = 1.0 + occlusionTexture.strength * (occlusionTexture.r - 1.0)
```

The red-channel sample is linear, and `occlusionTexture.strength` defaults to `1.0`. If there is no occlusion texture, `occlusionFactor` is `1.0`. Bakes usually already contain occlusion, so content creators **SHOULD** make sure the occlusion texture does not darken the same regions twice. Either omit it or limit it to detail finer than the light map resolution.

### Unlit Materials

When a material also uses `KHR_materials_unlit`, the light map multiplies the unlit base color. The RGB result, including occlusion, is:

```
baseColor.rgb = baseColorFactor.rgb * baseColorTextureLinear.rgb * vertexColor.rgb
color = baseColor.rgb * lightmapLinear * intensity * occlusionFactor
```

Here `baseColorTextureLinear` is the base color texture after sRGB decoding, and `vertexColor` is the linear `COLOR_0` attribute. An absent texture or vertex color contributes a multiplier of `1.0`, and `baseColorFactor` uses its core default. Base color alpha, including texture and vertex alpha when present, continues to determine coverage through `alphaMode` and `alphaCutoff`. The light map does not modify alpha, and `doubleSided` behavior is unchanged.

In this combination, the light map and its occlusion multiplier are the only changes to the unlit RGB calculation. Runtime lights and IBL do not illuminate the unlit material, and the other core PBR lighting properties, including the emissive slot, remain ignored as specified by `KHR_materials_unlit`.

This normalization preserves the deployed Mozilla Hubs behavior: unlit materials **MUST NOT** divide the light map contribution by π. It differs from the PBR diffuse BRDF normalization above. An unlit light map is a color multiplier rather than a physically normalized Lambertian irradiance input; exporters **MUST NOT** apply the PBR π conversion to it.

This allows fully baked scenes to be rendered with unlit materials at minimal cost. Without this extension, an unlit material displays `baseColor` unchanged.

> **Implementation Note:** Renderers whose unlit material internally divides light maps by π can implement this equation by multiplying their internal light map intensity by π. This is a renderer conversion, not a change to the extension's serialized `intensity`; exporters must reverse it when writing the extension.

### Baked and Dynamic Lights

For PBR materials, the renderer adds light map irradiance on top of whatever lighting it computes at runtime. To avoid counting the same light twice, the content creator decides which lighting is baked and which is dynamic:

* If the light map contains **indirect diffuse lighting only**, the scene's lights (for example, defined with `KHR_lights_punctual`) remain in the asset and supply direct lighting and all specular lighting at runtime.
* If the light map contains **total diffuse lighting**, lights whose direct contribution was baked **SHOULD NOT** also illuminate the light-mapped surfaces at runtime. They can be removed from the asset or kept only for specular highlights or unbaked objects, depending on what the target renderer supports.

`KHR_lights_punctual` does not define specular-only lights or per-surface light exclusions. Keeping fully baked diffuse lighting while using those lights only for live highlights or unbaked objects therefore requires application-specific controls; the authoring guidance above does not encode such controls in the asset. Indirect-only bakes combined with ordinary runtime direct lighting provide a portable baseline for PBR materials.

If the light map already includes diffuse lighting from an environment (such as the sky), adding the same environment's diffuse image-based lighting again will double-count that contribution. Content creators **SHOULD** avoid this duplication. Diffuse image-based lighting whose contribution is not already included in the bake may be added normally. Specular image-based lighting is unaffected.

## Implementation Notes

*This section is non-normative.*

* **Unwrapping and padding.** Light map UV charts should not overlap, and should be padded and dilated so that bilinear filtering and mipmapping don't bleed between charts.
* **Dynamic range.** Irradiance often exceeds `1.0`. With 8-bit formats such as PNG and JPEG, normalize the linear lighting values, encode the normalized values with the sRGB transfer function, and put the linear scale factor in `intensity`. Store dark scenes with care to avoid banding. Any texture extension used for the referenced texture must preserve the sRGB decoding convention defined above.
* **Static content.** Light maps only describe the lighting of the geometry and lights at bake time. They are suited to static geometry. A light map on a moving or deforming mesh stays attached to the surface and will look incorrect.
* **Instancing.** The light map belongs to the material and is addressed through mesh texture coordinates. A mesh referenced by several nodes therefore shares one light map region. In core glTF, materials are bound by mesh primitives, not by nodes. To give nodes unique lighting, use distinct mesh/primitive definitions with separate UV data or material bindings. These definitions may share geometry accessors when only the material binding differs, for example to use different `KHR_texture_transform` atlas offsets. This extension does not add per-node or per-instance light map transforms.

## Fallback

This extension is optional. Assets using it **SHOULD NOT** add `MOZ_lightmap` to `extensionsRequired`. Clients that don't support it render the material without baked lighting, using only the lights and environment available at runtime. Assets that bake total lighting and remove the baked lights will look darker in such clients. Content creators targeting both kinds of client may want to keep an approximate runtime lighting setup.

## glTF Schema Updates

* **JSON schema**: [material.MOZ_lightmap.schema.json](schema/material.MOZ_lightmap.schema.json)

## Known Implementations

* [three.js](https://github.com/mrdoob/three.js/pull/34950): loader and exporter plugins (pull request).
* [Mozilla Hubs](https://github.com/Hubs-Foundation/hubs/blob/master/src/components/gltf-model-plus.js): loader.
* [Hubs Blender Exporter](https://github.com/Hubs-Foundation/hubs-blender-exporter): Blender import and export.
* [Spoke](https://github.com/Hubs-Foundation/Spoke/tree/master/src/editor/gltf/extensions): Hubs scene editor, import and export.
* [hubs-glb-tools](https://github.com/Hubs-Foundation/hubs-glb-tools/blob/master/src/MozLightmap.ts): glTF-Transform extension.
* [iR Engine (formerly Ethereal Engine)](https://github.com/ir-engine/ir-engine): loader and glTF-Transform extension.
* [EE Bridge Unity Exporter](https://github.com/EtherealEngine/EE-Bridge-Unity-Export): Unity export.

## Sample Assets

* [LightmappedCornellBox.glb](https://github.com/bhouston/three.js/blob/94b4693d0f207a8b2ad88a043fe15325eb9676dc/examples/models/gltf/LightmappedCornellBox/LightmappedCornellBox.glb): Cornell box with baked indirect diffuse lighting in a shared light map atlas and a dynamic `KHR_lights_punctual` point light. Created by Ben Houston with three.js, with no external models or textures, and provided under the three.js repository's MIT license. This pinned revision predates the sRGB encoding requirement above; its light map texture requires conversion to sRGB before use as a conforming sample.

## Resources

* [Original draft of `MOZ_lightmap`](https://github.com/takahirox/MOZ_lightmap)
* [`MX_lightmap`](https://thirdroom.io/docs/gltf/mx_lightmap/readme), a related extension that adds per-node atlas transforms and RGBM encoding.
