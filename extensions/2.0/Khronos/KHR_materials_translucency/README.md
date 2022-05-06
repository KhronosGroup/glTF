# KHR\_materials\_translucency

## Contributors

* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
* Ed Mackey, AGI [@emackey](https://twitter.com/emackey)

## Acknowledgments

TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds a Lambertian diffuse transmission BSDF to the metallic-roughness material. Thin, dielectric objects like leaves or paper transmit some of the incoming light to the opposite side of the surface. For thick media (volumes) with short scattering distances and therefore dense scattering behavior, a diffuse transmission lobe is a phenomenological plausible and cheap approximation.

<p float="left">
<img src="figures/leaves.jpg" height="350" /> <img src="figures/thin_translucent.jpg" height="350" />
<p><em>(Thin) translucent leaves</em></p>
</p>

## Extending Materials

The effect is activated by adding the `KHR_materials_translucency` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_translucency": {
                    "translucencyFactor": 0.7,
                    "translucencyTexture": 0,
                    "translucencyColorFactor": [
                      1.0,
                      0.0,
                      0.0
                    ],
                    "translucencyColorTexture": {
                      "index": 0
                    }
                }
            }
        }
    ]
}
```

The strength of the effect is controlled by `translucencyFactor` and `translucencyTexture`, combined via multiplication to describe a single value.
The color of the effect is controlled by `translucencyColorFactor` and `translucencyColorTexture`, combined via multiplication to describe a single color.

|                              | Type                                                                | Description                                                                                                                                                                      | Required                 |
| ---------------------------- | ------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------ |
| **translucencyFactor**       | `number`                                                            | The percentage of non-specularly reflected light that is transmitted through the surface via the Lambertian diffuse transmission, i.e., the strength of the translucency effect. | No, default: `0`         |
| **translucencyTexture**      | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A greyscale texture that defines the strength of the translucency effect, stored in the alpha (A) channel. Will be multiplied by the translucencyFactor                          | No                       |
| **translucencyColorFactor**  | `number[3]`                                                         | The tint of the transmitted light                                                                                                                                                | No, default: `[1, 1, 1]` |
| **translucencyColorTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A texture that defines the color of the transmitted light, stored in the RGB channels and encoded in sRGB. This texture will be multiplied by translucencyColorFactor.           | No                       |

Increasing the strength of the translucency effect using the `translucency` parameter takes away energy from the diffuse reflection BSDF and passes it to the diffuse transmission BSDF. The specular reflection BSDF and Fresnel weighting are not affected. The `translucencyColorFactor` effectively replaces the usage of the `baseColor` for modulation of the transmitted light as specified by [KHR_materials_transmission](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_transmission#tinting).

## Examples

All examples use `translucencyColorTexture` from `baseColorTexture`
|                No translucency                 | <sub>`translucencyFactor: : 0.25`</sub> | <sub>`translucencyFactor: 0.25`<br>`translucencyColorFactor: [1.0,0.9,0.85]`</sub> |
| :--------------------------------------------: | :-------------------------------------: | :--------------------------------------------------------------------------------: |
| ![](figures/teatime_backlit_no_trans.png) <br> |    ![](figures/teatime_backlit.png)     |                      ![](figures/teatime_backlit_colored.png)                      |
| ![](figures/teatime_side_no_trans.png) <br> |    ![](figures/teatime_side.png)     |                      ![](figures/teatime_side_colored.png)                      |

[Tea Set](https://polyhaven.com/a/tea_set_01) by [James Ray Cock](https://www.artstation.com/jamesray), Jurita Burger and [Rico Cilliers](https://www.artstation.com/ricocilliers) on [PolyHaven](https://polyhaven.com)

|         No translucency         | <sub>`translucencyFactor: : 0.5`<br>`translucencyColorFactor: [1,0,0]`</sub> | <sub>`translucencyFactor: : 1.0`<br>`translucencyColorFactor: [1,0,0]`</sub> |
| :-----------------------------: | :------------------------------------: | :------------------------------------: |
| ![](figures/plant_no_trans.png) |       ![](figures/plant_0.5.png)       |       ![](figures/plant_1.0.png)       |

[Potted Plant](https://polyhaven.com/a/potted_plant_02) by [Rico Cilliers](https://www.artstation.com/ricocilliers) on [PolyHaven](https://polyhaven.com)


## Implementation

*This section is non-normative.*

The extension changes the diffuse term defined in [Appendix B](/specification/2.0/README.md#appendix-b-brdf-implementation) to the following:

```
translucency = translucencyFactor * translucencyTexture.a
color_refl = baseColorFactor * baseColorTexture
color_trans = translucencyColorFactor * translucencyColorTexture
diffuse = lerp(color_refl * diffuseBRDF, color_trans * diffuseBTDF, translucency) / pi
```

The `diffuseBRDF` is 1 if view and light direction point into the same hemisphere wrt. the normal direction, and 0 otherwise. The `diffuseBTDF` is 1 if view and light direction point into different hemispheres wrt. the normal direction, and 0 otherwise.

```
diffuseBRDF = dot(N, V) * dot(N, L) >= 0 ? 1 : 0
diffuseBTDF = dot(N, V) * dot(N, L) <  0 ? 1 : 0
```

<figure style="text-align:center">
<img src="./figures/bsdf.svg"/>
<sub><figcaption><em>Left: Diffuse BRDF. Right: Diffuse BTDF (translucency).</em></figcaption></sub>
</figure>

## Combining Translucency with other Extensions
### KHR_materials_transmission
 Both `KHR_materials_translucency` and  `KHR_materials_transmission` replace the diffuse BRDF with a BTDF that transmits light onto the opposite side of the surface. In case of `KHR_materials_transmission`, this is a microfacet BTDF that shares its roughness with the microfacet BRDF. In case of `KHR_materials_translucency`, on the other hand, this is a diffuse BTDF. Since the diffuse BTDF does not have controls for roughness, the roughness parameter acts only on the reflective part of the surface. By decoupling the reflection and transmission parts. By decoupling the reflection and transmission parts it is possible to configure materials which have a smooth reflection and a diffuse transmission, as shown in image below (right).

<figure style="text-align:center">
 <img src="./figures/transmission-translucency_thin.jpg" width="40%" />
 <img src="./figures/transmission-translucency_thin_angle.jpg" width="39.5%" />
<sub><figcaption><em>Emissive sphere behind material sample. Left: Opaque diffuse. Middle: Rough transmission. Right: Translucency.</em></figcaption></sub>
</figure><br>

If `KHR_materials_transmission` is used in combination with `KHR_materials_translucency`, the transmission effect overrides the translucency effect. The calculation given in `KHR_materials_translucency` still holds, but uses the modified diffuse term defined above: *f*<sub>*diffuse*</sub> = (1 - *F*) * (1 - *T*) * *diffuse*.

<figure style="text-align:center">
 <img src="./figures/star2.jpg" width="500" />
<sub><figcaption><em>95% transmissive, 5% translucent</em></figcaption></sub>
</figure>


### KHR_materials_volume
If `KHR_materials_translucency` is combined with `KHR_materials_volume`, a diffuse transmission BTDF describes the transmission of light through the volume boundary. The object becomes translucent. The roughness parameter only affects the reflection. Scattering and absorption inside the volume are computed as defined in `KHR_materials_volume` and `KHR_materials_sss`, so the random walk through the volume is not affected by the surface BSDF.

<figure style="text-align:center">
<img src="./figures/translucent-roughness.png"/>
<sub><figcaption><em>Translucent sphere with varying roughness. From left to right: 0.0, 0.2, 0.4.</em></figcaption></sub>
</figure>

For comparison, below is the result for the same scene with `KHR_materials_transmission` instead of `KHR_materials_translucency`. Please note, how the refractive microfacet BTDF describes the transmission of light through the volume boundary. The refraction occurs on microfacets, and thus the roughness parameter affects both reflection and transmission.

<figure style="text-align:center">
<img src="./figures/transmissive-roughness.png"/>
<sub><figcaption><em>Transmissive sphere with varying roughness. From left to right: 0.0, 0.2, 0.4.</em></figcaption></sub>
</figure>

### KHR_materials_sss

If the medium exhibits strong subsurface scattering (large values for the scattering coefficient σ<sub>s</sub>) it is recommended to use `KHR_materials_translucency` instead of `KHR_materials_transmission`. Examples for such dense scattering materials are skin or wax. The visual difference between translucency and transmission is small in this case, as the path a light travels is dominated by volume scattering. The scattering interaction at the volume boundary has only a small effect on the final result.

The benefit of using translucency is that it signals the renderer that a material is dense, without the need to analyze geometry and scattering distance. Typically, the size of the volume in relation to the scattering coefficient determines the density of the object. A tiny object with low scattering coefficient may appear transparent, but increasing the size of the object will make it appear denser, although the scattering coefficient stays the same. If translucency is being used instead of highly glossy transmission, the material appears to be translucent independent of its size.

Consequently, renderers may use translucency as a cue to switch to diffusion approximation instead of random walk subsurface scattering. Diffusion approximation gives results that are close to ground-truth for dense materials, but can be much faster. This is crucial for real-time implementations (which cannot do random walk), but also beneficial for offline rendering. [Christensen and Burley (2015)](#ChristensenBurley2015) show how to map the physical parameters for attenuation and subsurface scattering to an appropriate reflectance profile for diffusion approximation and compare results between approximation and ground-truth random walk. [Jimenez et al. (2015)](#Jimenez2015) present a method to render reflectance profiles in real-time by approximating the profile with a separable kernel.

<figure style="text-align:center">
<img src="./figures/transmission-translucency.png"/>
<figcaption><em>Comparison of combining subsurface scattering with either transmission or translucency. Left: Rough transmission and subsurface scattering. Middle: Translucency and subsurface scattering. Right: Translucency without subsurface scattering using a thin-walled material. Colors are adjusted manually so that they look similar in the three configurations. This adjustment is needed in order to account for differences in distances and to minimize the impact of energy loss from the rough microfacet BTDF.</em></figcaption>
</figure>


### Summary
|                                  |                                                                                                                                                                                    KHR_materials_transmission                                                                                                                                                                                    |                                                                                                                                                                                KHR_materials_translucency                                                                                                                                                                                |
| :------------------------------: | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| KHR_materials_volume <br>(thin)  | <img src="./figures/nayyara-shabbir-RUZSejG7xig-unsplash.jpg" height="250"/><br/><sub><em><span>Photo by <a href="https://unsplash.com/@nayyaranoor?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Nayyara Shabbir</a> on <a href="https://unsplash.com/?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Unsplash</a></span></em></sub> |       <img src="./figures/jotaka-ospuTF_nBho-unsplash.jpg" height="250"/><br/><sub><em><span>Photo by <a href="https://unsplash.com/@pragmart?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">jötâkå</a> on <a href="https://unsplash.com/?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Unsplash</a></span></em></sub>        |
| KHR_materials_volume <br>(thick) |       <img src="./figures/nate-reagan-ZXlAB98N5aQ-unsplash.jpg" height="250"/><br/><sub><em><span>Photo by <a href="https://unsplash.com/@nreagan?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Nate Reagan</a> on <a href="https://unsplash.com/?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Unsplash</a></span></em></sub>       | <img src="./figures/carolyn-v-bdVmIkx_gIs-unsplash.jpg" height="250"/><br/><sub><em><span>Photo by <a href="https://unsplash.com/@sixteenmilesout?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Carolyn V</a> on <a href="https://unsplash.com/?utm_source=unsplash&amp;utm_medium=referral&amp;utm_content=creditCopyText">Unsplash</a></span></em></sub> |

## Schema

- [glTF.KHR_materials_translucency.schema.json](schema/glTF.KHR_materials_translucency.schema.json)

