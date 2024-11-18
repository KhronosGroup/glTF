# KHR\_materials\_diffuse_transmission

## Contributors

- Tobias Haeussler, Dassault Systèmes [@proog128](https://github.com/proog128)
- Bastian Sdorra, Dassault Systèmes [@bsdorra](https://github.com/bsdorra)
- Mike Bond, Adobe [@miibond](https://github.com/MiiBond)
- Eric Chadwick, DGG, [@echadwick-artist](https://github.com/echadwick-artist)
- Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)
- Emmett Lalish, Google [@elalish](https://github.com/elalish)
- Alex Wood, AGI [@abwood](https://github.com/abwood)
- Ed Mackey, AGI [@emackey](https://github.com/emackey)

Copyright 2024 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Release Candidate

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension models the physical phenomenon of light being diffusely transmitted through an infinitely thin material. Thin dielectric objects like leaves or paper diffusely transmit light due to dense volumetric scattering within the object. In 3D graphics, it is common to approximate thin volumetric objects as non-volumetric surfaces. The KHR_materials_diffuse_transmission extension models the diffuse transmission of light through such infinitely thin surfaces. For optically thick media (volumes) with short scattering distances and dense scattering behavior, i.e. candles, KHR_materials_diffuse_transmission provides a phenomenologically plausible and cost-effective approximation.

<div align="center">
<figure style="text-align:center">
<p float="left">
<img src="figures/leaves.jpg" height="350" />
<img src="figures/candle_0.5.jpg" height="350" />
</p>
<figcaption>
<p style="font-size:0.7em;">(Left: <a href="https://github.com/KhronosGroup/glTF-Sample-Assets/tree/main/Models/DiffuseTransmissionPlant">Diffuse Transmission Plant</a> by Eric Chadwick CC-BY 4.0 and Rico Cilliers CC0 1.0, showing a small firefly emitting light that transmits diffusely through the back of a leaf.)<p>
<p style="font-size:0.7em;">(Right: Candle model by <a href="https://sketchfab.com/lucatorcigliani">lucatorcigliani</a> with modifications by @emackey, showing diffuse transmission through the surface of wax paired with volumetric attenuation of light within the wax. Original <a href="https://sketchfab.com/3d-models/candle-in-a-glass-ec6cf358c4dd4cbda7be67f7846cbc9e">source</a> CC-BY 4.0)<p>
</figcaption>
</figure>
</div>


## Extending Materials

The effect is activated by adding the `KHR_materials_diffuse_transmission` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_diffuse_transmission": {
                    "diffuseTransmissionFactor": 0.25,
                    "diffuseTransmissionTexture": {
                      "index": 0
                    },
                    "diffuseTransmissionColorFactor": [
                      1.0,
                      0.9,
                      0.85
                    ]
                }
            }
        }
    ]
}
```

## Properties

|                                     | Type                                                                | Description                                                                                                                                                                     | Required                 |
|-------------------------------------|---------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------|
| **diffuseTransmissionFactor**       | `number`                                                            | The percentage of non-specularly reflected light that is diffusely transmitted through the surface.                      | No, default: `0`         |
| **diffuseTransmissionTexture**      | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A texture that defines the percentage of non-specularly reflected light that is diffusely transmitted through the surface. Stored in the alpha (`A`) channel. Will be multiplied by the diffuseTransmissionFactor.                 | No                       |
| **diffuseTransmissionColorFactor**  | `number[3]`                                                         | The color that modulates the transmitted light.                                                                                                                                 | No, default: `[1, 1, 1]` |
| **diffuseTransmissionColorTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A texture that defines the color that modulates the diffusely transmitted light, stored in the `RGB` channels and encoded in sRGB. This texture will be multiplied by diffuseTransmissionColorFactor. | No                       |

### diffuseTransmissionFactor

The proportion of light that is diffusely transmitted through a surface, rather than being diffusely re-emitted. This is expressed as a percentage of the light that penetrates the surface (i.e., not specularly reflected), rather than a percentage of the total light incident on the surface. A value of 1.0 indicates that 100% of the light that penetrates the surface is transmitted through it.

<table>
  <tr>
    <td><img src="figures/factor_0.0.jpg"/></td>
    <td><img src="figures/factor_0.25.jpg"/></td>
    <td><img src="figures/factor_0.5.jpg"/></td>
    <td><img src="figures/factor_0.75.jpg"/></td>
    <td><img src="figures/factor_1.0.jpg"/></td>
  </tr>
  <tr>
    <td align="center">0.0</td>
    <td align="center">0.25</td>
    <td align="center">0.5</td>
    <td align="center">0.75</td>
    <td align="center">1.0</td>
  </tr>
  <tr>
    <td colspan="5" align="center">
      <em>Backlit, occluded plane with blue <code>baseColorFactor</code> for varying <code>diffuseTransmissionFactor</code>.</em>
    </td>
  </tr>
</table>

### diffuseTransmissionColorFactor

The proportion of light at each color channel that is not attenuated by the surface transmission. Attenuation is usually defined as an amount of light at each frequency that is reduced over a given distance through a medium by absorption and scattering interactions. However, since this extension deals exclusively with infinitely thin surfaces, attenuation is constant and equal to 1.0 - `diffuseTransmissionColorFactor`.

<table>
  <tr>
    <td><img src="figures/color_0.0.jpg"/></td>
    <td><img src="figures/color_0.25.jpg"/></td>
    <td><img src="figures/color_0.5.jpg"/></td>
    <td><img src="figures/color_0.75.jpg"/></td>
    <td><img src="figures/color_1.0.jpg"/></td>
  </tr>
 <tr>
    <td align="center">0.0</td>
    <td align="center">0.25</td>
    <td align="center">0.5</td>
    <td align="center">0.75</td>
    <td align="center">1.0</td>
  </tr>
  <tr>
    <td colspan="5" align="center">
      <em>Backlit, occluded plane with blue <code>baseColorFactor</code> and red <code>diffuseTransmissionColorFactor</code> for varying <code>diffuseTransmissionFactor</code>.</em>
    </td>
  </tr>
</table>

### diffuseTransmissionTexture

The `A` channel of this texture defines proportion of light that is diffusely transmitted through a surface, rather than being diffusely re-emitted. This is expressed as a percentage of the light that penetrates the surface (i.e., not specularly reflected), rather than a percentage of the total light incident on the surface. A value of 1.0 indicates that 100% of the light that penetrates the surface is transmitted through it.

The value is linear and is multiplied by the `diffuseTransmissionFactor` to determine the total diffuse transmission value.

```
diffuseTransmission = diffuseTransmissionFactor * diffuseTransmissionTexture.a
```

<table>
 <tr>
    <td><img src="./figures/factor_tex_inlay.jpg"/></td>
  </tr>
  <tr>
    <td align="center">
      <em>Backlit, occluded plane with blue <code>baseColorFactor</code> and a striped <code>diffuseTransmissionTexture</code>.<br>(Input texture shown in the top-left).</em>
    </td>
  </tr>
</table>


### diffuseTransmissionColorTexture

The `RGB` channels of this texture define the proportion of light at each color channel that is not attenuated by the surface transmission.
The values are multiplied by the `diffuseTransmissionColorFactor` to determine the total diffuse transmission color.
```
diffuseTransmissionColor = diffuseTransmissionColorFactor * diffuseTransmissionColorTexture.rgb
```

<table>
  <tr>
    <td><img src="figures/color_tex_0.0.jpg"/></td>
    <td><img src="figures/color_tex_0.25.jpg"/></td>
    <td><img src="figures/color_tex_0.5.jpg"/></td>
    <td><img src="figures/color_tex_0.75.jpg"/></td>
    <td><img src="figures/color_tex_1.0.jpg"/></td>
  </tr>
  <tr>
    <td align="center">0.0</td>
    <td align="center">0.25</td>
    <td align="center">0.5</td>
    <td align="center">0.75</td>
    <td align="center">1.0</td>
  </tr>
  <tr>
    <td colspan="5" align="center">
      <em>
        Single-sided plane in a symmetric light setup. <code>baseColorTexture</code> and <code>diffuseTransmissionColorTexture</code> use textures that represent the different sides of the one-dollar bill. Series shows the setup at varying values of <code>diffuseTransmissionFactor</code>.<br>
        <p style="font-size:0.7em;">(dollar bill texture from <a href="https://resourceboy.com/textures/dollar-textures/">ResourceBoy.com</a> )
        <p>
      </em>
    </td>
  </tr>
</table>

## Material Structure Updates

*This section is normative.*

This extension changes the `dielectric_brdf` defined in [Appendix B](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#material-structure)
```
dielectric_brdf =
  fresnel_mix(
    ior = 1.5,
    base = diffuse_brdf(color = baseColor),
    layer = specular_brdf(α = roughness ^ 2)
  )
```
 to the following:
```
dielectric_brdf =
  fresnel_mix(
    ior = 1.5,
    base = mix(
      diffuse_brdf(color = baseColor),
      diffuse_btdf(color = diffuseTransmissionColor),
      diffuseTransmission),
    layer = specular_brdf(α = roughness ^ 2)
  )
```
Increasing the strength of the diffuse transmission effect using the `diffuseTransmissionFactor` parameter takes away energy from the diffuse reflection BSDF and passes it to the diffuse transmission BSDF. The specular reflection BSDF and Fresnel weighting are not affected.

## Implementation
*This section is non-normative.*

With a simple Lambert BRDF model, `diffuse_brdf` and `diffuse_btdf` may be implemented as follows
```
function diffuse_brdf(color) {
  if (view and light on same hemisphere) {
    return (1/pi) * color
  } else {
    return 0
  }
}

function diffuse_btdf(color) {
  if (view and light on opposite hemispheres) {
    return (1/pi) * color
  } else {
    return 0
  }
}

function mix(bsdf0, bsdf1, factor) {
  return (1-factor) * bsdf0 + factor * bsdf1
}
```

<div align="center">
<table>
  <tr>
    <td colspan="2"><img src="./figures/bsdf.jpg"/></td>
  </tr>
  <tr>
    <td colspan="1" align="center">
      <em>Diffuse BRDF</em>
    </td>
    <td colspan="1" align="center">
      <em>Diffuse BTDF</em>
    </td>
  </tr>
</table>
</div>

## Combining Diffuse Transmission with other Extensions
### KHR_materials_transmission
Both `KHR_materials_diffuse_transmission` and `KHR_materials_transmission` replace the diffuse BRDF with a mix of diffuse BRDF and a BTDF that transmits light onto the opposite side of the surface. In case of `KHR_materials_transmission`, this is a microfacet BTDF that shares its roughness with the microfacet BRDF. In case of `KHR_materials_diffuse_transmission`, on the other hand, this is a diffuse BTDF.

Let's recall the `dielectric_brdf` for `KHR_materials_diffuse_transmission` as defined above
```
dielectric_brdf =
  fresnel_mix(
    ior = 1.5,
    base = mix(
      diffuse_brdf(color = baseColor),
      diffuse_btdf(color = diffuseTransmissionColor),
      diffuseTransmission,
    layer = specular_brdf(α = roughness ^ 2)
  )
```

and compare it to the `dielectric_brdf` defined in `KHR_materials_transmission`
```
dielectric_brdf =
  fresnel_mix(
    ior = 1.5,
    base = mix(
      diffuse_brdf(baseColor),
      specular_btdf(α = roughness^2) * baseColor,
      transmission),
    layer = specular_brdf(α = roughness^2)
  )
```

Since the diffuse BTDF does not have controls for roughness, the roughness parameter acts only on the reflective part of the surface. By decoupling the reflection and transmission parts it is possible to configure materials which have a smooth reflection and a diffuse transmission, as shown in images below.

<table>
  <tr>
    <td><img src="./figures/transmission-translucency_thin.jpg"/></td>
    <td><img src="./figures/transmission-translucency_thin_angle.jpg"/></td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <em>Emissive sphere behind material sample.<br>Left: Opaque diffuse. Middle: Rough transmission. Right: Diffuse transmission.</em>
    </td>
  </tr>
</table>

<table>
  <tr>
    <td colspan="3"><img src="./figures/translucent-roughness.png"/></td>
  </tr>
  <tr>
    <td align="center">0.0</td>
    <td align="center">0.2</td>
    <td align="center">0.4</td>
  </tr>
  <tr>
    <td colspan="3" align="center">
      <em>Translucent sphere with varying roughness.</em>
    </td>
  </tr>
</table>

<table>
 <tr>
    <td colspan="3"><img src="./figures/transmissive-roughness.png"/></td>
  </tr>
  <tr>
    <td align="center">0.0</td>
    <td align="center">0.2</td>
    <td align="center">0.4</td>
  </tr>
  <tr>
    <td colspan="3" align="center">
      <em>Transmissive sphere with varying roughness.</em>
    </td>
  </tr>
</table>

If `KHR_materials_transmission` is used in combination with `KHR_materials_diffuse_transmission`, the transmission effect overrides the diffuse transmission effect.

We can formalize this behavior by combining the two cases from above
```
dielectric_brdf =
  fresnel_mix(
    ior = 1.5,
    base = mix(
      diffuse_bsdf,
      specular_btdf(α = roughness^2) * baseColor,
      transmission),
    layer = specular_brdf(α = roughness^2)
  )

diffuse_bsdf = mix(
    diffuse_brdf(color = baseColor),
    diffuse_btdf(color = diffuseTransmissionColor),
    diffuseTransmission)
```
<table>
  <tr>
    <td><img src="figures/dt_transmission_1.0.jpg"/></td>
    <td><img src="figures/dt_transmission_0.75.jpg"/></td>
    <td><img src="figures/dt_transmission_0.5.jpg"/></td>
    <td><img src="figures/dt_transmission_0.25.jpg"/></td>
    <td><img src="figures/dt_transmission_0.0.jpg"/></td>
  </tr>
  <tr>
    <td align="center">1.0</td>
    <td align="center">0.75</td>
    <td align="center">0.5</td>
    <td align="center">0.25</td>
    <td align="center">0.0</td>
  </tr>
  <tr>
    <td colspan="5" align="center">
      <em>Dragon with fixed <code>diffuseTransmissionFactor</code> of 1.0 and varying <code>transmissionFactor</code>.<br>
        <p style="font-size:0.7em;">
        (dragon model from <a href="https://graphics.stanford.edu/data/3Dscanrep/">Stanford 3D Scanning Repository</a>)
      </em>
    </td>
  </tr>
</table>

### KHR_materials_volume
When `KHR_materials_diffuse_transmission` is combined with `KHR_materials_volume`, a diffuse transmission BTDF describes the transmission of light through the volume boundary. The object becomes translucent. The light transport inside the volume is solely handled by `KHR_materials_volume` and is not affected by the surface BSDF.

<table>
  <tr>
    <td><img src="figures/attenuation_dt_0.0.jpg"/></td>
    <td><img src="figures/attenuation_dt_0.5.jpg"/></td>
    <td><img src="figures/attenuation_dt_1.0.jpg"/></td>
  </tr>
  <tr>
    <td align="center">0.0</td>
    <td align="center">0.5</td>
    <td align="center">1.0</td>
  </tr>
  <tr>
    <td colspan="3" align="center">
      <em>
      Dragon with white base color, colored volume attenuation and varying <code>diffuseTransmissionFactor</code>.<br>
      </em>
    </td>
  </tr>
</table>

<table>
  <tr>
    <td><img src="figures/candle_0.0.jpg"/></td>
    <td><img src="figures/candle_0.25.jpg"/></td>
    <td><img src="figures/candle_0.5.jpg"/></td>
  </tr>
  <tr>
    <td align="center">0.0</td>
    <td align="center">0.25</td>
    <td align="center">0.5</td>
  </tr>
  <tr>
    <td colspan="3" align="center">
      <em>
      Candle with off-white base color, colored volume attenuation and varying <code>diffuseTransmissionFactor</code>.<br>
      <p style="font-size:0.7em;">(candle model by <a href="https://sketchfab.com/lucatorcigliani">lucatorcigliani</a> with modifications by @emackey. Original <a href="https://sketchfab.com/3d-models/candle-in-a-glass-ec6cf358c4dd4cbda7be67f7846cbc9e">source</a> CC-BY 4.0)<p>
      </em>
    </td>
  </tr>
</table>


## Schema

- [glTF.KHR_materials_diffuse_transmission.schema.json](schema/glTF.KHR_materials_diffuse_transmission.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2024 The Khronos Group Inc.

Some parts of this Specification are purely informative and do not define requirements
necessary for compliance and so are outside the Scope of this Specification. These
parts of the Specification are marked as being non-normative, or identified as
**Implementation Notes**.

Where this Specification includes normative references to external documents, only the
specifically identified sections and functionality of those external documents are in
Scope. Requirements defined by external documents not created by Khronos may contain
contributions from non-members of Khronos not covered by the Khronos Intellectual
Property Rights Policy.

This specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/adopters.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

Vulkan is a registered trademark and Khronos, OpenXR, SPIR, SPIR-V, SYCL, WebGL,
WebCL, OpenVX, OpenVG, EGL, COLLADA, glTF, NNEF, OpenKODE, OpenKCAM, StreamInput,
OpenWF, OpenSL ES, OpenMAX, OpenMAX AL, OpenMAX IL, OpenMAX DL, OpenML and DevU are
trademarks of The Khronos Group Inc. ASTC is a trademark of ARM Holdings PLC,
OpenCL is a trademark of Apple Inc. and OpenGL and OpenML are registered trademarks
and the OpenGL ES and OpenGL SC logos are trademarks of Silicon Graphics
International used under license by Khronos. All other product names, trademarks,
and/or company names are used solely for identification and belong to their
respective owners.
