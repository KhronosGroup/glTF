# KHR\_materials\_iridescence

## Khronos 3D Formats Working Group

* Pascal Schoen, adidas
* Ben Houston, threekit
* Mathias Kanzler, UX3D
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

Copyright (C) 2018-2021 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

Iridescence describes an effect where hue varies depending on the viewing angle and illumination angle: A thin-film of a semi-transparent layer results in inter-reflections and due to thin-film interference, certain wavelengths get absorbed or amplified. Iridescence can be seen on soap bubbles, oil films, or on the wings of many insects.
With this extension, thickness and index of refraction (IOR) of the thin-film can be specified, enabling iridescent materials.

## Extending Materials

The iridescence materials are defined by adding the `KHR_materials_iridescence` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_iridescence": {
                    "iridescenceFactor": 1.0,
                    "iridescenceIOR": 1.3,
                    "iridescenceThicknessMaximum": 1200.0
                }
            }
        }
    ]
}
```

### Iridescence

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                       | Type                                                                                                | Description                                                                                                                           | Required              |
| ------------------------------------- | --------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------- | --------------------- |
| **iridescenceFactor**           | `number`                                                                                          | The iridescence intensity.                                                                                                            | No, default:`0.0`   |
| **iridescenceTexture**          | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The iridescence intensity stored in the red channel of the texture.                                                                   | No                    |
| **iridescenceIOR**              | `number`                                                                                          | The index of refraction of the dielectric thin-film layer.                                                                            | No, default:`1.3`   |
| **iridescenceThicknessMinimum** | `number`                                                                                          | The minimum thickness of the thin-film layer given in nanometers (nm).                                                                | No, default:`100.0` |
| **iridescenceThicknessMaximum** | `number`                                                                                          | The maximum thickness of the thin-film layer given in nanometers (nm).                                                                | No, default:`400.0` |
| **iridescenceThicknessTexture** | [`textureInfo`](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The green channel of this texture defines the thickness of the thin-film layer by blending between the minimum and maximum thickness. | No                    |

The values for iridescence intensity can be defined using a factor, a texture, or both.
`iridescenceFactor` is multiplied with the red channel of `iridescenceTexture` to control the overall strength of the iridescence effect. If the texture is not set, a value of 1.0 is assumed for the texture.

```glsl
iridescence = iridescenceFactor * iridescenceTexture.r
```

If `iridescenceFactor` is zero (default), the iridescence extension has no effect on the material.
All textures in this extension use a single channel in linear space.
The thickness of the thin-film is set to `iridescenceThicknessMaximum` if `iridescenceThicknessTexture` is not given.
If `iridescenceThicknessTexture` is set, the thickness of the thin-film varies between  `iridescenceThicknessMinimum` and `iridescenceThicknessMaximum` as follows:

```glsl
thickness = mix(iridescenceThicknessMinimum, iridescenceThicknessMaximum, iridescenceThicknessTexture.g)
```

Aside from light direction and IOR, the thickness of the thin-film defines the variation in hue.
This effect is the result of constructive and destructive interferences of certain wavelengths.
If the the optical path difference between the ray reflected at the thin-film and the ray reflected at the base material is half the wavelength (λ), the resulting 180 degree phase shift is cancelling out the reflected light:

<figure>
<img width=70% src="./figures/interference.png"/>
<figcaption><em> </em></figcaption>
</figure>

With a thin-film thickness near half the wavelength of visible light (380 nm - 750 nm), the effect is most visible.
By increasing the thin-film thickness, multiples of wavelengths are still causing wave interferences, however, as the optical path distance increases, different rays are mixed in.
This leads to more pastel colored patterns for increased thickness:

<figure>
<img src="./figures/thickness-comparison.jpg"/>
<figcaption><em>Comparison of different iridescence thickness ranges for a constant iridescence IOR value of 1.8. </em></figcaption>
</figure>

The thin-film layer can have a different IOR than the underlying material. With `iridescenceIOR` one can set an IOR value for the thin-film layer independently.
This also has an effect on the optical path difference:

<figure>
<img src="./figures/ior-comparison.jpg"/>
<figcaption><em>Comparison of different iridescence IOR values for a thin-film thickness range between 200 nm (bottom) and 800 nm (top).</em></figcaption>
</figure>

The iridescence effect is modeled via a microfacet BRDF with a modified Fresnel reflectance term that accounts for inter-reflections as shown in [Laurent Belcour and Pascal Barla (2017)](https://belcour.github.io/blog/research/publication/2017/05/01/brdf-thin-film.html).

### Clearcoat

Considering `KHR_materials_clearcoat` extension, the clearcoat layer is evaluated before the thin-film layer.
Light that is passing through the clearcoat is then processed as described taking into account differences in IOR.

<figure>
<img width=40% src="./figures/layering.png"/>
<figcaption><em>When adding clearcoat ontop of the material, the interface between the "outside" and the thin-film changes and the IOR of the clearcoat (usually 1.5) has to be used for the outside IOR.</em></figcaption>
</figure>

## Implementation Notes

*This section is non-normative.*

For the calculation of `f_specular`, the original Fresnel term `F_schlick` (see [B.3.4. Fresnel](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#fresnel) in the glTF 2.0 specification) is exchanged with a newly introduced `F_iridescence` term.
To artistically control the strength of the iridescence effect, the two Fresnel terms are mixed by using the `iridescenceFactor`:

```glsl
F = mix(F_schlick, F_iridescence, iridescenceFactor)
```

The calculation of `F_iridescence` is described in the following sections. For glTF an approximation of the original model (defined in the Mitsuba code example from [Belcour/Barla](https://belcour.github.io/blog/research/publication/2017/05/01/brdf-thin-film.html) is used.

### Material Interfaces

The iridescence model defined by Barla and Belour models two material interfaces - one from the outside to the thin-film layer and another one from the thin-film to the base material. These two interfaces are defined as follows:

```glsl
// First interface
float R0 = IorToFresnel0(iridescenceIOR, outsideIOR);
float R12 = F_Schlick(R0, cosTheta1);
float R21 = R12;
float T121 = 1.0 - R12;

// Second interface
vec3 baseIOR = Fresnel0ToIor(baseF0 + 0.0001); // guard against 1.0
vec3 R1 = IorToFresnel0(baseIOR, iridescenceIOR);
vec3 R23 = F_Schlick(R1, cosTheta2);
```

`iridescenceIOR` is the index of refraction of the thin-film layer, `outsideIOR` the IOR outside the thin-film (e.g. air or the clearcoat material) and `baseIOR` the IOR of the base material. The latter is calculated from its F0 value using the inverse of the special normal incidence case of Fresnel's equations:

```glsl
// Assume air interface for top
vec3 Fresnel0ToIor(vec3 F0) {
    vec3 sqrtF0 = sqrt(F0);
    return (vec3(1.0) + sqrtF0) / (vec3(1.0) - sqrtF0);
}
```

This simple formula is used for both dielectrics and metals. While it is physically correct for dielectric materials, it's only an approximation for metals, which are usually described using a complex IOR with an additional extinction factor. Such a value cannot be accurately inferred from the F0 value and is thus assumed to be `0.0`. 

To calculate the reflectance factors at normal incidence of both interfaces (`R0` and `R1`), the special case is used as is:

<img src="https://render.githubusercontent.com/render/math?math=\large\color{gray}\displaystyle%20R%20=%20\left|\frac{n_1%20-%20n_2%20}{n_1%20%2B%20n_2%20}\right|^2">

```glsl
float IorToFresnel0(float transmittedIor, float incidentIor) {
    return pow((transmittedIor - incidentIor) / (transmittedIor + incidentIor), 2.0);
}
```

To calculate the reflectances `R12` and `R23` at the viewing angles `theta1` (angle hitting the thin-film layer) and `theta2` (angle after refraction in the thin-film) Schlick Fresnel is again used. This approximation allows to eliminate the split into S and P polarization for the exact Fresnel equations. 

`theta2` can be calculated using Snell's law like:

```glsl
float sinTheta2Sq = pow(outsideIOR / iridescenceIOR, 2.0) * (1.0 - pow(cosTheta1, 2.0));
float cosTheta2 = sqrt(1.0 - sinTheta2Sq);
```

### Phase Shift

The phase shift is as follows:

<img src="https://render.githubusercontent.com/render/math?math=\large\color{gray}\displaystyle%20\Delta%20\phi%20=%202%20\pi%20\lambda^{-1}\mathcal{D}">

and depends on the first-order optical path difference <img src="https://render.githubusercontent.com/render/math?math=\color{gray}\displaystyle%20\mathcal{D}"> (or `OPD`):

```glsl
OPD = 2.0 * iridescenceIOR * iridescenceThickness * cosTheta1;
```

`phi12` and `phi23` define the base phases per interface and are approximated with `0.0` if the IOR of the hit material (`iridescenceIOR` or `baseIOR`) is higher than the IOR of the previous material (`outsideIOR` or `iridescenceIOR`) and `M_PI` otherwise. Also here, polarization is ignored.

```glsl
// First interface
float phi12 = 0.0;
if (iridescenceIOR < outsideIOR) phi12 = M_PI;
float phi21 = M_PI - phi12;

// Second interface
vec3 phi23 = vec3(0.0);
if (baseIOR[0] < iridescenceIOR) phi23[0] = M_PI;
if (baseIOR[1] < iridescenceIOR) phi23[1] = M_PI;
if (baseIOR[2] < iridescenceIOR) phi23[2] = M_PI;

// Phase shift
vec3 phi = vec3(phi21) + phi23;
```

### Analytic Spectral Integration

Spectral integration is performed in the Fourier space. 

```glsl
// Compound terms
vec3 R123 = clamp(R12 * R23, 1e-5, 0.9999);
vec3 r123 = sqrt(R123);
vec3 Rs = sq(T121) * R23 / (vec3(1.0) - R123);

// Reflectance term for m = 0 (DC term amplitude)
vec3 C0 = R12 + Rs;
I = C0;

// Reflectance term for m > 0 (pairs of diracs)
vec3 Cm = Rs - T121;
for (int m = 1; m <= 2; ++m)
{
    Cm *= r123;
    vec3 Sm = 2.0 * evalSensitivity(float(m) * OPD, float(m) * phi);
    I += Cm * Sm;
}

F_iridescence = max(I, vec3(0.0));
```

With the sensitivity evaluation function being:

```glsl
vec3 evalSensitivity(float OPD, vec3 shift) {
    float phase = 2.0 * M_PI * OPD * 1.0e-9;
    vec3 val = vec3(5.4856e-13, 4.4201e-13, 5.2481e-13);
    vec3 pos = vec3(1.6810e+06, 1.7953e+06, 2.2084e+06);
    vec3 var = vec3(4.3278e+09, 9.3046e+09, 6.6121e+09);

    vec3 xyz = val * sqrt(2.0 * M_PI * var) * cos(pos * phase + shift) * exp(-sq(phase) * var);
    xyz.x += 9.7470e-14 * sqrt(2.0 * M_PI * 4.5282e+09) * cos(2.2399e+06 * phase + shift[0]) * exp(-4.5282e+09 * sq(phase));
    xyz /= 1.0685e-7;

    vec3 rgb = XYZ_TO_REC709 * xyz;
    return rgb;
}
```

and the color space transformation matrix to convert from XYZ to RGB:

```glsl
const mat3 XYZ_TO_REC709 = mat3(
     3.2404542, -0.9692660,  0.0556434,
    -1.5371385,  1.8760108, -0.2040259,
    -0.4985314,  0.0415560,  1.0572252
);
```

The full derivation of the fast analytical spectral integration is described in the original publication in section 4 (Analytical Spectral Integration) and will not be described in detail here.

## Reference

### Theory, Documentation and Implementations

[Belcour, L. and Barla, P. (2017): A Practical Extension to Microfacet Theory for the Modeling of Varying Iridescence](https://belcour.github.io/blog/research/publication/2017/05/01/brdf-thin-film.html)

[Autodesk: Arnold for Maya user guide - Thin Film](https://docs.arnoldrenderer.com/display/A5AFMUG/Thin+Film)

[Drobot, M. and Micciulla, A. (2017): Practical Multilayered Materials in Call of Duty: Infinite Warfare](https://blog.selfshadow.com/publications/s2017-shading-course/drobot/s2017_pbs_multilayered.pdf)

[Kneiphof, T., Golla, T. and Klein, R. (2019): Real-time Image-based Lighting of Microfacet BRDFs with Varying Iridescence](https://cg.cs.uni-bonn.de/publication/kneiphof-2019-real-time)

[Akenine-Möller, T., Haines, E., Hoffman, N., Pesce, A., Iwanicki, M., and Hillaire, S. (2018): Real-Time Rendering, Fourth Edition; page 361ff](https://www.realtimerendering.com/)

[Sussenbach, M. (2013): Rendering Iridescent Objects in Real-time](http://dspace.library.uu.nl/handle/1874/287110)

## Schema

[glTF.KHR_materials_iridescence.schema.json](schema/glTF.KHR_materials_iridescence.schema.json)

## Appendix: Full Khronos Copyright Statement

Copyright 2018-2021 The Khronos Group Inc.

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
