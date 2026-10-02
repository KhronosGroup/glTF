<!--
SPDX-License-Identifier: LicenseRef-KhronosSpecCopyright
-->

# KHR_materials_fuzz

## Contributors

- Mike Bond, Adobe [@MiiBond](https://github.com/MiiBond)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines a fuzz layer that can be layered on top of an existing glTF material definition. A fuzz layer is commonly used in Physically-Based Rendering to represent materials with fine surface fibers or hair-like structures, such as peach skin, velvet, or even dust. The fuzz layer sits at the very top of the material stack, above all other layers.

## Extending Materials

The fuzz material is defined by adding the `KHR_materials_fuzz` extension to any compatible glTF material. For example, the following defines a material with black fuzz:

```json
{
  "materials": [
    {
      "name": "black_velvet",
      "extensions": {
        "KHR_materials_fuzz": {
          "fuzzFactor": 1.0,
          "fuzzColorFactor": [0.0, 0.0, 0.0],
          "fuzzRoughnessFactor": 0.8
        }
      }
    }
  ]
}
```

## Interaction with KHR_materials_sheen

This extension is intended to replace `KHR_materials_sheen` as it contains all the functionality of that extension but has a few key differences. Firstly, it sits on top of the coating layer, rather than underneath it. Secondly, while `sheenColor` acted as an intensity for the sheen layer, `fuzzColor` defines purely the color of the fuzz layer, which allows dark or even black fuzz. If both extensions are defined on a material, it is expected that `KHR_materials_fuzz` will take precedence and `KHR_materials_sheen` only exists as a fallback for loaders that do not support the newer extension.

## Parameters

The following parameters are contributed by the `KHR_materials_fuzz` extension:

|                              | Type                                                                            | Description                            | Required                       |
|------------------------------|---------------------------------------------------------------------------------|----------------------------------------|--------------------------------|
|**fuzzFactor**                | `number`                                                                        | The intensity of the fuzz layer        | No, default: `0.0`             |
|**fuzzTexture**               | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The fuzz intensity texture             | No                             |
|**fuzzColorFactor**           | `number[3]`                                                                     | The fuzz color in linear space         | No, default: `[1.0, 1.0, 1.0]` |
|**fuzzColorTexture**          | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The fuzz color (RGB) texture           | No                             |
|**fuzzRoughnessFactor**       | `number`                                                                        | The fuzz roughness                     | No, default: `0.5`             |
|**fuzzRoughnessTexture**      | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The fuzz roughness texture             | No                             |

### Parameter Combinations

The fuzz layer is applied when `fuzzFactor` is greater than zero. If `fuzzFactor` is zero, the entire fuzz layer is disabled.

If textures are defined:

- The fuzz intensity is computed as: `fuzzIntensity = fuzzFactor * fuzzTexture.r`
- The fuzz color is computed as: `fuzzColor = fuzzColorFactor * sRGBToLinear(fuzzColorTexture.rgb)`
- The fuzz roughness is computed as: `fuzzRoughness = fuzzRoughnessFactor * fuzzRoughnessTexture.a`

Otherwise:
- `fuzzIntensity = fuzzFactor`
- `fuzzColor = fuzzColorFactor` 
- `fuzzRoughness = fuzzRoughnessFactor`

### Key Differences from Sheen

1. **Blend Factor**: The addition of `fuzzFactor` and `fuzzTexture` allows complete control over the layer opacity, enabling black fuzz colors (which wasn't possible with sheen since a black sheen color would disable the layer entirely).

2. **Layer Ordering**: Fuzz sits at the very top of the material stack, above all other layers including clearcoat:
   ```
   Fuzz (topmost)
   ↓
   Clearcoat (if present)
   ↓
   Base Material (metallic-roughness + other extensions)
   ```

3. **Color Default**: The default `fuzzColorFactor` is white `[1.0, 1.0, 1.0]` rather than black, making the blend factor the primary control for layer intensity.

## Shading Model

The fuzz BRDF simulates the back-scattering of materials with fine surface fibers that are opaque and oriented mostly perpendicular to the surface. At low `fuzzRoughness`, the specular response can be modeled as reflection from cylindrical micro-fibers oriented in the normal direction. At high `fuzzRoughness`, the shape of the fibers becomes spherical, ideal for simulating fine, dust particles.
The fuzz layer is purely scattering and does not absorb energy. Therefore, any energy not multiple scattered by the fuzz layer will be transmitted on to layers below. This energy is always greyscale so the underlying layers are never tinted by the fuzz.

The BRDF is assumed to be derived from an anisotropic microflake volume model with a fiber-like distribution. We recommend a specific model below for increased consistency between renderers.

## Shading Normal

The fuzz follows the shape of the surface immediately below it. This might be the coat layer or the base layer, depending on the intensity of the coat. This can be expressed as an interpolation between the normal of the base layer and coat layer using the coat's weight:

`fuzz_normal = mix(base_normal, coat_normal, coat_weight)`

#### Recommended BRDF

*This section is non-normative.*

We recommend the [fuzz model](https://github.com/tizian/ltc-sheen) defined by Tizian Zeltner, Brent Burley, and Matt Jen-Yuan Chiang, called Practical Multiple-Scattering Sheen Using Linearly Transformed Cosines from ACM SIGGRAPH Talks (2022). It models fuzz as a homogeneous volume with a fiber-like SGGX microflake phase function. It uses Linearly Transformed Cosines (LTC) to approximate the reflectance of the volume.
They provide an RGB LUT that contains reflectance data and lobe shape (for anisotropic sampling) that can be used for rendering of fuzz.

### Fuzz Layering

The fuzz layer is combined with the underlying material using the fuzz weight and the reflectance. The reflectance depends on the fuzz roughness and cosine of the angle between the incoming and outgoing light rays. This value can be stored in a lookup table (LUT) to avoid expensive calculations at runtime.

```glsl
fuzz = fuzz_color * fuzz_reflectance * fuzz_weight + base_material * mix(1.0, 1.0 - fuzz_reflectance, fuzz_weight)
```

## Schema

- [material.KHR_materials_fuzz.schema.json](schema/material.KHR_materials_fuzz.schema.json)