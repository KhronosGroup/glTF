# KHR_materials_sss

## Contributors

* TODO: Name, affiliation, and contact info for each contributor

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Needs to be combined with `KHR_materials_volume`

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Table of Contents

- [Overview](#overview)
- [Extending Materials](#extending-materials)
- [Properties](#properties)
- [Scattering](#scattering)
- [Phase Function](#phase-function)
- [Scattering vs Translucency](#scattering-vs-translucency)
- [Reference](#reference)
- [Appendix: Full Khronos Copyright Statement](#appendix-full-khronos-copyright-statement)

## Overview

`KHR_materials_volume` explains how we turn surfaces into interfaces between volumes. It also gives us the necessary tools to define the effect of absorption inside homogeneous volumes. What it lacks, is the definition of scattering. Scattering is the wavelength-dependent change of direction which occurs when light interacts with particles. Whereas absorption is the wavelength-dependent reduction of light energy along a path. This extension can be considered as an augmentation of the definitions given in KHR_materials_volume. 

<figure style="text-align:center">
<img src="./figures/volume.svg"/>
<figcaption><em>Interaction of light rays inside the volume and at the boundaries. The volume is homogeneous and has an index of refraction of 1.5.</figcaption>
</em></figure>

TODO: better graphics of absorption vs scattering

## Extending Materials

TODO: Clarify location of scattering extensions. volume or material
TODO: Clarify subsurfaceColor or scattering Color + scatteringDistance
The scattering properties are defined by adding the `KHR_materials_sss` extension to any glTF material.

```json
materials: [
    {
        "extensions": {
            "KHR_materials_sss": {
                "subsurfaceColor": [ 0.572, 0.227, 0.075 ]
            }
        }
    }
]
```

## Properties

The extension defines the following parameters to describe the scattering behavior.

| | Type | Description | Required |
|-|------|-------------|----------|
| **subsurfaceColor** | `number[3]` | The overall color perceived at the surface due to subsurface multi-scattering. | No, default: `[0, 0, 0]` |
<br>


## Scattering

In literature, scattering is usually defined by the scattering coefficient σ<sub>s</sub>. The scattering coefficient is a probability density with the unit 1/m<sup>2</sup> and values in the range [0, inf]. The sum of absorption and scattering coefficients is commonly referred to as the attenuation (or extinction) coefficient

 σ<sub>t</sub> = σ<sub>a</sub> + σ<sub>s</sub>

Based on the attenuation coefficient, we can define the transmittance T. It corresponds to a change in radiance along a path as light travels through a medium with absorbing and scattering particles.

T(x) = e<sup>-σ<sub>t</sub>x</sup>

The single-scattering albedo is the color of a single scattering interaction in the medium. Light that is scattered by a particle will be tinted with this color. An albedo of 0 (black) disables scattering, resulting in a medium that only absorbs.

 ρ<sub>ss</sub> =  σ<sub>s</sub> / σ<sub>t</sub>

 Light that is scattered by a particle will be tinted with this color. An albedo of 0 (black) disables scattering, resulting in a medium that only absorbs.

Please note, light likely scatters many times in the medium until it leaves the volume. Depending on the number of bounces, the overall color sensation of the medium differ drastically from the single-scatter albedo. We decided to replace the single-scatter albedo with the multi-scatter albedo as defined by [Kulla and Conty (2017)](#KullaConty2017). It is a more reasonable approximation for the perceived color in multi bounce scenarios and easier to control.

ρ<sub>ss</sub> = 1 - (4.09712 + 4.20863 ρ<sub>ms</sub> - sqrt(9.59217 + 41.6808 ρ<sub>ms</sub> + 17.7126 ρ<sub>ms</sub><sup>2</sup>))<sup>2</sup>

Now that we have computed σ<sub>t</sub> and ρ<sub>ss</sub>, we can finally derive σ<sub>a</sub> and σ<sub>s</sub> as follows:

σ<sub>a</sub> = σ<sub>t</sub> (1 - ρ<sub>ss</sub>)

σ<sub>s</sub> = σ<sub>t</sub> ρ<sub>ss</sub> = σ<sub>t</sub> - σ<sub>a</sub>

<figure style="text-align:center">
<img src="./figures/diffuse-sss.png"/>
<figcaption><em>A simple, diffuse-only material (left) and a material that makes use of subsurface scattering (right). The base color of the diffuse material is set to the same color as the subsurface color of the subsurface scattering material. Due to the albedo mapping the final color of the object is very similar.</em></figcaption>
</figure>

## Phase Function

The phase function p used for scattering inside the medium is isotropic. For any pair of incident and outgoing directions k<sub>1</sub> and k<sub>2</sub>, p(k<sub>1</sub>, k<sub>2</sub>) = 1 / (4π).

## Scattering vs. Translucency 
(TODO)
The volume extension needs one of `KHR_materials_transmission` and `KHR_materials_translucency` to allow light rays to pass through the surface into the volume. Once the volume is entered however, the simulation of absorption and subsurface scattering inside the medium will be independent of the surface properties.

If the extension is combined with `KHR_materials_transmission`, the refraction occurs at the microfacets. That means that the thin microfacet BTDF is replaced by a microfacet BTDF that takes refraction into account. The roughness parameter affects both reflection and transmission.

<figure style="text-align:center">
<img src="./figures/transmissive-roughness.png"/>
<figcaption><em>Transmissive sphere with varying roughness. From left to right: 0.0, 0.2, 0.4.</em></figcaption>
</figure>

If the extension is combined with `KHR_materials_translucency`, the translucent BTDF remains unchanged.

<figure style="text-align:center">
<img src="./figures/translucent-roughness.png"/>
<figcaption><em>Translucent sphere with varying roughness. From left to right: 0.0, 0.2, 0.4.</em></figcaption>
</figure>

For best results, we recommend using `KHR_materials_translucency` instead of `KHR_materials_transmission` in case the medium exhibits strong subsurface scattering (small scattering distance, high subsurface color). Examples for these dense materials are skin or candle wax. The visual difference between translucency and transmission is small in this case, as the path a light travels is dominated by volume scattering. The scattering interaction at the volume boundary has only a small effect on the final result. 

The benefit of using translucency is that it signals the renderer that a material is dense, without the need to analyze geometry and attenuation distance. Typically, the size of the volume in relation to the scattering coefficient determines the density of the object. A tiny object with low scattering coefficient may appear transparent, but increasing the size of the object will make it appear denser, although the scattering coefficient stays the same. If translucency is used instead of highly glossy transmission, the material appears to be translucent independent of its size.

Consequently, renderers may use translucency as a cue to switch to diffusion approximation instead of random walk subsurface scattering. Diffusion approximation gives results that are very close to ground-truth for dense materials, but can be much faster. This is crucial for real-time implementations (which cannot do random walk), but also beneficial for offline rendering. [Christensen and Burley (2015)](#ChristensenBurley2015) show how to map the physical parameters for attenuation and subsurface scattering to an appropriate reflectance profile for diffusion approximation and compare results between approximation and ground-truth random walk. [Jimenez et al. (2015)](#Jimenez2015) present a method to render reflectance profiles in real-time by approximating the profile with a separable kernel.

<figure style="text-align:center">
<img src="./figures/transmission-translucency.png"/>
<figcaption><em>Comparison of combining subsurface scattering with either transmission or translucency. Left: Rough transmission and subsurface scattering. Middle: Translucency and subsurface scattering. Right: Translucency without subsurface scattering using a thin-walled material. Colors are adjusted manually so that they look similar in the three configurations. This adjustment is needed in order to account for differences in distances and to minimize the impact of energy loss from the rough microfacet BTDF.</em></figcaption>
</figure>


## Schema

- [glTF.KHR_materials_sss.schema.json](schema/glTF.KHR_materials_sss.schema.json)

## References

* [Christensen, P. and B. Burley (2015): Approximate Reflectance Profiles for Efficient Subsurface Scattering](https://graphics.pixar.com/library/ApproxBSSRDF/paper.pdf)<a name="ChristensenBurley2015"></a>
* [Jimenez J., K. Zsolnai, A. Jarabo, C. Freude, T. Auzinger, X.-C. Wu, J. Pahlen, M. Wimmer and D. Gutierrez (2015): Separable Subsurface Scattering](http://www.iryoku.com/separable-sss/)<a name="Jimenez2015"></a>
* [Kulla C., Conty A. (2017): Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf)<a name="KullaConty2017"></a>


## Appendix: Full Khronos Copyright Statement

Copyright 2018-2020 The Khronos Group Inc.

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
