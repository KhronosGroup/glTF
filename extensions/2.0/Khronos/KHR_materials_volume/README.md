# KHR_materials_volume

## Contributors

* TODO: Name, affiliation, and contact info for each contributor

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Needs to be combined with `KHR_materials_transmission` or `KHR_materials_translucency`.

## Exclusions

* This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
* This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Table of Contents

- [Overview](#overview)
- [Extending Materials](#extending-materials)
- [Properties](#properties)
- [Thickness Texture](#thickness-texture)
- [Refraction](#refraction)
- [Absorption and Subsurface Scattering](#absorption-and-subsurface-scattering)
- [Base Color and Absorption](#base-color-and-absorption)
- [Transmission and Translucency](#transmission-and-translucency)
- [Implementation](#implementation)
- [Reference](#reference)
- [Appendix: Full Khronos Copyright Statement](#appendix-full-khronos-copyright-statement)

## Overview

By default, a glTF 2.0 material describes the scattering properties of a surface enclosing an infinitely thin volume. The surface defined by the mesh represents a thin wall. The volume extension makes it possible to turn the surface into an interface between volumes. The mesh to which the material is attached defines the boundaries of an homogeneous medium and therefore must be manifold. Volumes provide effects like refraction, absorption and scattering. Where scattering is not subject of this extensions directly, but is provided by an additional extension KHR_materials_sss to complement the volume definition. 

<figure style="text-align:center">
<img src="./figures/thin-thick-rendering.png"><br/>
<img src="./figures/thin-thick.svg">
<figcaption><em>Renderings of various objects (top) and corresponding top-down slice through the scene (bottom). The solid line represents the mesh. The gray area represents the volume. Thin-walled materials can be applied to open (left) and closed meshes (middle). The dashed line indicates the imaginary bounds of the infinitely thin volume. The volumetric material can only be applied to closed meshes (right), resulting in volumetric effects like refraction.</em></figcaption>
</figure>

The volume extension has to be combined with `KHR_materials_transmission` or `KHR_materials_translucency`. Light that falls onto the volume boundary may enter the volume, depending on the transmission or translucency of the surface's BSDF. Light traveling through the volume is subject to absorption by the medium's particles. When hitting the surface from inside the volume, the light may either decide to leave the volume or bounce back for total internal reflection. 

<figure style="text-align:center">
<img src="./figures/refraction-absorption.svg"/>
<figcaption><em>A light ray being refracted in a volume. The volume is homogeneous and has an index of refraction of 1.5. At the boundaries, the direction of the light ray is bent according to the indices of refraction at incident and outgoing side. Inside the volume, light is attenuated as it travels through the medium into the direction of the eye. The longer it travels, to more it is attenuated.</figcaption>
</em></figure>

When light interacts with the surface, it is reflected at microfacets, taking the roughness of the material into account. For the transmission, the choice of distribution depends on the whether the lights transmits via KHR_materials_transmission or KHR_materials_translucency. In the case of *transmission*, the same microfacet distribution, is being used. For *translucency* microfacets are replaced with a diffuse cosine (lambert) distribution, please see [Transmission and Translucency](#Transmission_and_translucency) for more details. The index of refraction is taken from `KHR_materials_ior`.

## Extending Materials

The volumetric properties are defined by adding the `KHR_materials_volume` extension to any glTF material. A non-zero thickness switches from thin-walled to volumetric behavior. This requires a manifold/closed mesh. The properties of the medium are not texturable, it is assumed to be homogeneous.

```json
materials: [
    {
        "extensions": {
            "KHR_materials_volume": {
                "thicknessFactor": 1.0,
                "attenuationDistance":  0.006,
                "attenuationColor": [ 0.5, 0.5, 0.5 ]
            }
        }
    }
]
```

## Properties

The extension defines the following parameters to describe the volume.

| | Type | Description | Required |
|-|------|-------------|----------|
| **thicknessFactor** | `number` | The thickness of the volume beneath the surface in meters in the local coordinate system of the node. If the value is 0 the material is thin-walled. Otherwise the material is a volume boundary. The `doubleSided` property has no effect on volume boundaries. | No, default: `0`. |
| **thicknessTexture** | `textureInfo` | A texture that defines the thickness, stored in the G channel. This will be multiplied by `thicknessFactor`. | No |
| **attenuationDistance** | `number` | Density of the medium given as the average distance in meters that light travels in the medium before interacting with a particle. | No, default: +Infinity |
| **attenuationColor** | `number[3]` | The color that white light turns into due to absorption when reaching the attenuation distance. | No, default: `[1, 1, 1]` |

## Thickness Texture

The thickness of a volume enclosed by the mesh is typically quite difficult to compute at run-time in a rasterizer. Since glTF is primarily used with real-time rasterizers, this extension allows for the thickness of the volume to be explicitly defined, either via a 1-channel texture value or as a constant. However, deriving the thickness along a ray from the actual geometry is the preferred approach. Path-traced renderers as well as more sophisticated raster techniques should ignore the thickness texture. Note that it is still necessary to check the `thicknessFactor` to determine whether the object is thin-walled or volumetric.

## Refraction

Light rays falling through the volume boundary are refracted according to the index of refraction (IOR) given in `KHR_materials_ior`. The IOR determines the refraction angle. If `KHR_materials_ior` is not available, the IOR is 1.5.

<figure style="text-align:center">
<img src="./figures/ior.png"/>
<figcaption><em>Transmissive sphere with varying index of refraction. From left to right: 1.1, 1.5, 1.9.</em></figcaption>
</figure>

## Attenuation

The way in which a volumetric medium interacts with light, and therefore determines its appearance, is commonly specified by the attenuation coefficient σ<sub>t</sub> (also know as *extinction* coefficient). It is the probability density that a light interacts with a particle per unit distance traveled in the medium. σ<sub>t</sub> is a wavelength-dependent value. It's defined in the range [0, inf] with m<sup>-1</sup> as unit. 

Possible interactions when a light photon hits a particle are absorption and scattering. Absorption removes the light energy from the photon and translates it to other forms of energy, e.g. heat. Scattering preserves the energy, but changes the direction of the light. Both act in wavelength-dependent manner. Based on these two possible events, the attenuation coefficient is defined as the sum of two other coefficients: the absorption coefficient and the scattering coefficient.

σ<sub>t</sub> = σ<sub>a</sub> + σ<sub>s</sub>

> **NOTE**<br>
> This extension does not define the scattering part of the volumetric light transport. For all further definitions we assume the scattering coefficient σ<sub>s</sub> to be zero for all wavelength, and therefore: σ<sub>t</sub> = σ<sub>a</sub>

The infinite range of the coefficient makes it rather unintuitive to control by users. To provide a convenient parameterization, this extension exposes two derived parameters: *attenuation color c<sub>a</sub>* and *attenuation distance d<sub>a</sub>* (see [Properties](#Properties)). The relation between the two parameters and the attenuation coefficient σ<sub>t</sub> is defined as

σ<sub>t</sub> = -log(c<sub>a</sub>) / d<sub>a</sub>

For rendering, we are interested in the change of light when traversing the medium. So we need to integrate the attenuation coefficient along a path of a certain length.
TODO: add image

In a homogenous medium, σ<sub>t</sub> is constant, and we can compute the fraction of light (radiance) transmitted after traveling a distance x via Beer's law:

T(x) = e<sup>-σ<sub>a</sub>x</sup>

where T is commonly referred to as *transmittance*. 

By replacing σ<sub>t</sub> in the previous equation with our parameters attenuation color and attenuation distance we get

T(d<sub>a</sub>) = e<sup>(-log(c<sub>a</sub>) / d<sub>a</sub>) * d<sub>a</sub></sup> = c<sub>a</sub>

So, after traveling distance d<sub>a</sub> through the medium we get attenuation color c<sub>a</sub>.


## Base Color and Absorption

Base color and absorption both have an effect on the final color of a volumetric object, but the behavior is different. Base color changes the color of light at the volume boundary. Absorption occurs while the light is traveling through the volume. Depending on the distance the light travels, more or less of it is absorbed, making the overall color of the object dependent on its shape.

<figure style="text-align:center">
<img src="./figures/base-color-absorption.png"/>
<figcaption><em>Base color changes the amount of light passing through the volume boundary (left). The overall color of the object is the same everywhere, as if the object is covered with a colored, transparent foil. Absorption changes the amount of light traveling through the volume (right). The overall color depends on the distance the light traveled through it; at small distances (tail of the dragon) less light is absorbed and the color is brighter than at large distances.</em></figcaption>
</figure>

## Transmission and Translucency

The volume extension needs one of `KHR_materials_transmission` and `KHR_materials_translucency` to allow light rays to pass through the surface into the volume. Once the volume is entered however, the simulation of absorption and potentially subsurface scattering inside the medium is independent of the surface properties.

If the extension is combined with `KHR_materials_transmission`, the refraction occurs at the microfacets. That means that the thin microfacet BTDF is replaced by a microfacet BTDF that takes refraction into account. The roughness parameter affects both reflection and transmission.

<figure style="text-align:center">
<img src="./figures/transmissive-roughness.png"/>
<figcaption><em>Transmissive sphere with varying roughness. From left to right: 0.0, 0.2, 0.4.</em></figcaption>
</figure>

If the extension is combined with `KHR_materials_translucency`, the translucent BTDF will be replaced with the diffuse transmission BTDF as defines in `KHR_materials_translucency`.

<figure style="text-align:center">
<img src="./figures/translucent-roughness.png"/>
<figcaption><em>Translucent sphere with varying roughness. From left to right: 0.0, 0.2, 0.4.</em></figcaption>
</figure>

`KHR_materials_translucency` may be a good choice if the appearance should match a material which exhibits strong subsurface scattering (small scattering distance, high subsurface color). Examples for these dense materials are skin or candle wax. 
## Implementation

*This section is non-normative.*

The microfacet BTDF f<sub>transmission</sub> defined in `KHR_materials_transmission` now takes refraction into account. That means that we to use Snell's law to compute the modified half vector:

*H<sub>TR</sub>* = -normalize(*ior<sub>i</sub>* * *V* + *ior<sub>o</sub>* * *L*)

*ior<sub>i</sub>* and *ior<sub>o</sub>* denote the IOR of the incident and transmitted side of the surface, respectively. *V* is the vector pointing to the camera, *L* points to the light. In a path tracer that starts rays at the camera, *V* corresponds to the incident side of the surface, which is the side of the medium with *ior<sub>i</sub>*.

Incident and transmitted IOR have to be correctly set by the renderer, depending on whether light enters or leaves the object. An algorithm for tracking the IOR through overlapping objects is described by in [Schmidt and Budge (2002)](#SchmidtBudge2002).

The microfacet BTDF also makes use of the modified half vector and the indices of refraction.

*f*<sub>transmission</sub> = *T* * *baseColor* * (1 - *F<sub>TR</sub>*) * (abs(dot(*L*, *H<sub>TR</sub>*)) * abs(dot(*V*, *H<sub>TR</sub>*))) / (abs(dot(*L*, *N*)) * abs(dot(*V*, *N*))) * (*ior<sub>o</sub>*<sup>2</sup> * *G<sub>TR</sub>* * *D<sub>TR</sub>*) / (*ior<sub>i</sub>* * dot(*V*, *H<sub>TR</sub>*) + *ior<sub>o</sub>* * dot(*L*, *H<sub>TR</sub>*))<sup>2</sup>

where *T* is the transmission percentage defined by `KHR_materials_transmission`. The *D<sub>T<R/sub>* and *G<sub>TR</sub>* terms are the same as in the specular reflection except using the modified half vector *H<sub>TR</sub>* calculated from the refraction direction. See [Walter et al. (2007)](#Walter2007) for details.

The Fresnel term *F<sub>TR</sub>* now needs to take internal reflection into account. When using the Schlick approximation, care must be taken to use the angle that is on the dense side of the boundary, i.e., the side with the medium that has a higher IOR. In addition, total internal reflection has to be considered. Therefore, we have three cases:

Light enters medium with higher IOR: *ior<sub>o</sub>* ≥ *ior<sub>i</sub>*.

*F<sub>TR</sub>*<sup>+</sup> = F0 + (1 - F0) * (1 - abs(dot(*V*, *H<sub>TR</sub>*)))^5

Light enters medium with lower IOR and there is no total internal reflection: *ior<sub>o</sub>* < *ior<sub>i</sub>* and sin<sup>2</sup>(*θ<sub>o</sub>*) < 1. The angle at the transmitted side of the boundary (medium with lower IOR) *θ<sub>o</sub>* is computed from the angle of incidence via sin<sup>2</sup>(*θ<sub>o</sub>*) = (*ior<sub>i</sub>* / *ior<sub>o</sub>*)<sup>2</sup> (1 - dot(*V*, *H<sub>TR</sub>*)<sup>2</sup>) and thus cos(*θ<sub>o</sub>*) = sqrt(1 - sin<sup>2</sup>(*θ<sub>o</sub>*)).

*F<sub>TR</sub>*<sup>-</sup> = F0 + (1 - F0) * (1 - abs(cos(*θ<sub>o</sub>*)))^5

Light enters medium with lower IOR and there is total internal reflection: *ior<sub>o</sub>* < *ior<sub>i</sub>* and sin<sup>2</sup>(*θ<sub>o</sub>*) ≥ 1.

*F<sub>TR</sub>*<sup>TIR</sup> = 1

## Interaction with other extensions

If `KHR_materials_specular` is used in combination with `KHR_materials_volume`, specular color and specular modify F0 and F90 of the Fresnel as usual, but with one exception: In case of total internal reflection, F90 is not scaled by the specular parameter of `KHR_materials_specular`. This allows users to change the specular component of a refractive object without introducing artifacts, i.e., dark appearance in areas where total internal reflection occurs.

## Schema

- [glTF.KHR_materials_volume.schema.json](schema/glTF.KHR_materials_volume.schema.json)

## References

* [Kulla C., Conty A. (2017): Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf)<a name="KullaConty2017"></a>
* [Walter B., Marschner S., Li H., Torrance K. (2007): Microfacet Models for Refraction through Rough Surfaces](https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf)<a name="Walter2007"></a>
* [Schmidt C., Budge B. (2002): Simple Nested Dielectrics in Ray Traced Images](https://www.researchgate.net/profile/Brian_Budge/publication/247523037_Simple_Nested_Dielectrics_in_Ray_Traced_Images/links/00b7d52e001e1b88a3000000/Simple-Nested-Dielectrics-in-Ray-Traced-Images.pdf)<a name="SchmidtBudge2002"></a>

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
