# KHR_materials_retroreflection

## Contributors

* Martin-Karl Lefrançois, NVIDIA, [@mklefrancois](https://github.com/mklefrancois)
* Nia Bickford, NVIDIA, [@NBickford-NV](https://github.com/NBickford-NV)
* Bastian Sdorra, Dassault Systèmes, [@bsdorra](https://github.com/bsdorra)
* Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions
- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension adds a physically-plausible retroreflective response to the metallic and dielectric BRDFs of the glTF metallic-roughness material model.
It is based on the Minimal Retroreflective Microfacet (MRM) model of [Portsmouth et al. (JCGT 15(1), 2026)](http://jcgt.org/published/0015/01/04/),
which turns a BRDF into a retroreflective one by substituting the outgoing view direction $V$ with its reflection about the surface normal, $V_\text{retro} = 2 N (V \cdot N) - V$, before evaluation and sampling.
This redirects the specular peak into the back-scatter direction, producing the bright
retroreflective highlight seen on high-visibility clothing,
glass-bead road markings, many safety signs, and other materials.
In addition, MRM preserves reciprocity and energy conservation under reflection-symmetric NDFs (including GGX, Beckmann, and Phong).

A *retroreflection weight* then linearly blends between the regular metallic and dielectric BRDFs (at a factor of 0) and their retroreflective variants (at a factor of 1). This matches the OpenPBR `specular_retroreflectivity` parameter.

<figure>
<img src="./figures/retroreflection.png"/>
<figcaption><em>Left: A traffic cone with <code>KHR_materials_retroreflection</code> on both sleeves (using a <code>retroreflectionFactor</code> of 1, masked by <code>retroreflectionTexture</code>). The lower sleeve appears brighter than the top sleeve because the light and camera are more closely aligned there. Right: The same traffic cone model and material without the extension. Rendered in <a href="https://github.com/nvpro-samples/vk_gltf_renderer">vk_gltf_renderer</a>.</em></figcaption>
</figure>

## Extending Materials

`KHR_materials_retroreflection` can be added to a material's `extensions`, like this:

```json
{
  "materials": [
    {
      "name": "TrafficConeBaseMat",
      "pbrMetallicRoughness": {
        "baseColorTexture": { "index": 0 },
        "metallicFactor": 0.0,
        "roughnessFactor": 0.35
      },
      "extensions": {
        "KHR_materials_retroreflection": {
          "retroreflectionFactor": 1.0,
          "retroreflectionTexture": { "index": 2 }
        }
      }
    }
  ],
  "extensionsUsed": [ "KHR_materials_retroreflection" ]
}
```

The extension object contains the following properties:

| | Type | Description | Required |
| -------------------------- | -------- | ----------- | ------------------ |
| **retroreflectionFactor** | `number` | Linear blend between forward microfacet (0.0) and retroreflective microfacet (1.0). Range [0, 1]. | No, default: `0.0` |
| **retroreflectionTexture** | [`textureInfo`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | Per-texel multiplier for `retroreflectionFactor`, sampled from the **R** channel. | No |

The final per-shading-point retroreflection weight $w$ is:

```
retroreflectionFactor * retroreflectionTexture.r
```

if `retroreflectionTexture` is present and

```
retroreflectionFactor
```

otherwise.

## Retroreflection

If this extension is present on a material, then it replaces the metallic BRDF and dielectric BRDF of the glTF metallic-roughness material, as defined in [Appendix B](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation), with blends between the original BRDFs and retroreflective variants.

More specifically, let `metal_brdf_retro` and `dielectric_brdf_retro` be retroreflective variants of `metal_brdf` and `dielectric_brdf`, respectively, which have an increased response in the back-scatter direction relative to their normal variants. Then the blends between the original BRDFs are

```
metal_brdf_blended      = mix(metal_brdf,      metal_brdf_retro,      w)

dielectric_brdf_blended = mix(dielectric_brdf, dielectric_brdf_retro, w)
```

where `w` is the per-shading-point weight from the [Extending Materials](#extending-materials) section.

The final material is evaluated as defined in Appendix B, with each BRDF replaced by its blended variant:

```
material = mix(dielectric_brdf_blended, metal_brdf_blended, metallic)
```

## Implementation

*This section is non-normative.*

The Minimal Retroreflective Microfacet (MRM) model of [Portsmouth et al. (JCGT 15(1), 2026)](http://jcgt.org/published/0015/01/04/) provides a straightforward method for producing the retroreflective variants. Using the notation from Appendix B, MRM uses the reflected view direction `V_retro` instead of `V` when evaluating or sampling either affected BRDF:

`V_retro = reflect(-V, N) = 2 * N * dot(V, N) - V`.

All intermediate quantities derived from `V`, including the half vector `H` and Fresnel terms, are evaluated using `V_retro` for the retroreflective variants. In other words, if the regular BRDF is a function `f(L, V)` taking the light direction `L` and reflected view direction `V` at a point, then the MRM BRDF is `f(L, reflect(-V, N))`.

The same `V` to `V_retro` substitution is used when sampling the retroreflective BRDF and evaluating its PDF; see Listing 1 of Portsmouth et al. 2026.

## Excluding Other Lobes

*This section is non-normative.*

While it is possible to apply the MRM model transformation to any BSDF, this extension intentionally does not apply retroreflectivity to the additional BSDFs introduced by other extensions. This matches the behavior of retroreflection within OpenPBR and MaterialX's implementations.

This extension's choice can also be motivated by physical and visual reasons. For transmission, Section 5 of Portsmouth et al. 2026 describes how applying the MRM model transformation to transmissive lobes produces nonintuitive results, especially when the index of refraction $\eta$ is near 1. Although KHR_materials_clearcoat and KHR_materials_sheen add BRDFs and not BTDFs to the glTF material model, clearcoats of retroreflective objects are typically not themselves retroreflective, and applying the MRM model transformation to the sheen BRDF has little effect as its scattering is nearly symmetric in the view and reflected view directions.

## Optional vs. Required

This extension should not be listed in the `extensionsRequired` list, as retroreflectivity is generally not significant enough to justify blocking the entire scene from loading. We use "should not" instead of "must not" here, though, because there are some cases where retroreflectivity is so important to an object that it justifies listing this extension under `extensionsRequired`.

## Schema

* [material.KHR_materials_retroreflection.schema.json](schema/material.KHR_materials_retroreflection.schema.json)

## Known Implementations

* [NVIDIA DesignWorks Samples' vk_gltf_renderer](https://github.com/nvpro-samples/vk_gltf_renderer)

## Resources

* [Jamie Portsmouth, Matthias Raab, Laurent Belcour, and Francis Liu, The Minimal Retroreflective Microfacet Model, *Journal of Computer Graphics Techniques (JCGT)*, vol. 15, no. 1, 60-75, 2026](http://jcgt.org/published/0015/01/04/)
* [Retroreflection in MaterialX](https://github.com/AcademySoftwareFoundation/MaterialX/pull/2783)
* [Retroreflection in the OpenPBR 1.2 draft](https://github.com/AcademySoftwareFoundation/OpenPBR/blob/475bd90d5211d1a4bfb3227d147c692cbd9bc958/index.html#retroreflectivity)
