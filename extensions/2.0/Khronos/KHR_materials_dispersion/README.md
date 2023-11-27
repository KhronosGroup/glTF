# KHR\_materials\_dispersion

## Contributors

- Ben Houston, Threekit [@bhouston](https://github.com/bhouston)

Copyright 2023 The Khronos Group Inc. All Rights Reserved. glTF is a trademark of The Khronos Group Inc.
See [Appendix](#appendix-full-khronos-copyright-statement) for full Khronos Copyright Statement.

## Status

Draft

## Dependencies

- Written against the glTF 2.0 spec.
- The `KHR_materials_volume` extension as it builds upon its volumetric effect.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension adds one parameter to the metallic-roughness material: `dispersion`.

`dispersion` enables configuring the strength of the angular separation of colors (chromatic aberration) transmitting through a relatively clear volume.  It is an enhancement to the default ```KHR_materials_volume``` transmission model which assumes no dispersion.

Optical dispersion is represented in terms of the Abbe number parameterization \( $V$ \).  The dispersion effect is a result of the wavelength-dependent index of refraction of a material.  Dispersion is a widely adopted parameter in modern PBR models.  It is present in both OpenPBR (as *transmission_dispersion_abbe_number*) and the Dassault Enterprise PBR Shading Model (as $V_d$).

The Abbe number \( $V$ \) is computed from the index of refraction at three wavelengths of visible light: 486.1 nm (short wavelength blue, $n_F$), 587.6 nm (central wavelength, $n_d$), and 656.3 nm (long wavelength red, $n_C$).  The Abbe number makes the simplifying assumption that the index of refraction variance is linear:

$$V = \frac{n_d - 1}{n_F - n_C}$$

To calculate the index of refraction at a specific wavelength \( $\lambda$ \), given an Abbe number \( $V$ \) and the central index of refraction as specified by the ```KHR_materials_ior``` extension (assumed to be at the central wavelength, \( $N_d$ \)):

$$
B = \frac{n_d - 1}{V \left( {\lambda_F^{-2}} - {\lambda_C^{-2}} \right)}
$$

And then:

$$
A = n_d - \frac{B}{\lambda_d^2}
$$

And finally:

$$
n(\lambda) = A + \frac{B}{\lambda^2}
$$

![Dispersion on a Gem](./figures/Dispersion.jpg)

In this extension, we store a transformed dispersion instead of the Abbe number directly.  Specifically we store $20/V$ so that a value of 1.0 is equivalent to $V=20$, which is about the lowest Abbe number for normal materials. Values over 1.0 are still valid for artists that want to exaggerate the effect. Decreasing values lower the amount of dispersion down to 0.0.  Aside from being more intuitive for artists, this mapping also has the added benefit of being more easily defined with a texture.  This is the same transform used by both Adobe Standard Material and OpenPBR.

## Extending Materials

The dispersion, defined in terms of Abbe number, is defined by adding the `KHR_materials_dispersion` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_dispersion": {
                    "dispersion": 0.1
                }
            }
        }
    ]
}
```

| |Type|Description|Required|
|-|----|-----------|--------|
| **dispersion** | `number` | The strength of the dispersion effect, specified as 20/Abbe number. | No, default: `0`|

The default value of 0 has a special meaning in that no dispersion should be used.  This is the default value for backwards compatibility.  Any value zero or larger is considered to be a valid dispersion value, although the range between [0 , 1] is the range of realistic values.

Here is a table of some material dispersion Abbe numbers, including the outlier of Rutile which is a very high dispersion material:

| Material | Abbe Number (V) | Dispersion (20/V) |
| -------- | ----------- | -------------- |
| Rutile | 9.8 | 2.04 |
| Polycarbonate | 32 | 0.625 |
| Diamond | 55 | 0.36 |
| Water | 55 | 0.36 |
| Crown Glass | 59 | 0.33 |

## Implementation

*This section is non-normative.*

One real-time method for rendering dispersion effects is to trace volume transmission separately for each of color channel accounting for the per channel IOR as determined by the Abbe number.  The resulting composite image will show color separation between the channels as a result.

## Schema

- [material.KHR_materials_dispersion.schema.json](schema/material.KHR_materials_dispersion.schema.json)

## Reference

### Theory, Documentation and Implementations

[Abbe Number - Wikipedia](https://en.wikipedia.org/wiki/Abbe_number)

[Abbe Number - Wolfram Formula Repository](https://resources.wolframcloud.com/FormulaRepository/resources/Abbe-Number#:~:text=The%20Abbe%20number%2C%20also%20known,of%20V%20indicating%20low%20dispersion.)

[OpenPBR Surface specification](https://academysoftwarefoundation.github.io/OpenPBR/)

[Enterprise PBR Shading Model](https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec-2022x.md.html)

[Mikhail N. Polyanskiy. Refractive Index Database (2023)](https://refractiveindex.info)

[SCHOTT. Interactive Abbe Diagram (2023)](https://www.schott.com/en-gb/special-selection-tools/interactive-abbe-diagram)


## Appendix: Full Khronos Copyright Statement

Copyright 2023 The Khronos Group Inc.

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
