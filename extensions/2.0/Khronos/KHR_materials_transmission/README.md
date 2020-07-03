# KHR\_materials\_transmission

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Work in Progress

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many optically transparent materials are impossible to represent in a physically plausible manner with the core glTF 2.0 PBR material. This is because the core specification only includes the concept of "alpha as coverage" (exposed via the alpha channel of `baseColorFactor` and `baseColorTexture`). Many common materials like glass and plastic require significantly different rendering techniques.

## Limitations of Alpha as Coverage
Alpha-as-coverage can basically be thought of as describing whether the surface exists or not. When alpha is 0, nothing is rendered while alpha=1 means that the material is solid. It's most useful for materials such as gauze or burlap that only partially cover the area in which the material is rendered. These materials contain lots of small holes where the light can pass through so, strictly speaking, the light isn't actually entering the material itself (i.e. the surface isn't there). For other materials, like clear glass, a photon of light might actually enter the surface and be transmitted through. In this case, the light can be reflected, refracted, absorbed or scattered. Simple alpha-as-coverage cannot represent any of these effects and so we need very different logic to render them correctly.

The most important of these features is surely reflection. Even a completely transparent, infinitely thin, glass material is still visible due to its reflection of light. However, a material using purely alpha as coverage is not visible at all when alpha = 0 (because the surface effectively doesn't exist).

<figure>
<img src="./figures/OpacityComparison.png"/>
<figcaption><em>White sphere with alpha coverage of 25% (left) vs the same sphere with 100% optical transparency (right). Note that alpha coverage of 25% is shown because 0% would be completely invisible. Also note that alpha-as-coverage reflects light in proportion to its opacity. i.e. the more transparent it is, the weaker the reflections will appear.</em></figcaption>
</figure>

The `KHR_materials_transmission` extension provides a way to define glTF 2.0 materials that are transparent to light in a physically plausible way. That is, it enables the creation of transparent materials that absorb, reflect and transmit light depending on the incident angle and the wavelength of light. Common uses cases for thin-surface transmissive materials include plastics and glass.

This extension aims to address the simplest and most common use cases for optical transparency: infinitely-thin materials with no refraction, scattering, or dispersion. Dealing exclusively with “thin” materials (i.e. materials where only the surface is considered and not the volume) allows many simplifications to be made when calculating things like refraction and absorption.

## Extending Materials

A transparent material is defined by adding the `KHR_materials_transmission` extension to any glTF material. When present, the extension indicates that a material should be rendered as a transparent surface and be blended as defined in the following spec. Note that, as rendering transparent objects presents many difficult-to-solve issues with primitive ordering, this extension does not dictate rendering algorithms.

```json
materials: [
  {
    "extensions": {
       "KHR_materials_transmission": {
         "transmissionFactor": 0.8,
         "transmissionTexture": 0
       }
    }
  }
]
```

## Properties 
Only two properties are introduced with this extension and combine to describe a single value; the percentage of light that is transmitted through the surface of the material. These properties work together with the existing properties of the material to define the way light is modified as it passes through the substance. 

|   |Type|Description|Required|
|---|----|-----------|--------|
|**transmissionFactor** | `number` | The base percentage of light that is transmitted through the surface.| No, Default: `0.0 ` |
|**transmissionTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | A texture that defines the transmission percentage of the surface, stored in the `R` channel. This will be multiplied by `transmissionFactor`. | No |

### transmissionFactor 
The amount of light that is transmitted by the surface rather than diffusely re-emitted. This is a percentage of all the light that penetrates a surface (i.e. isn’t specularly reflected) rather than a percentage of the total light that hits a surface. A value of 1.0 means that 100% of the light that penetrates the surface is transmitted through.

<figure>
  <img src="./figures/ConstantTransmission.png"/>
<figcaption><em>baseColor or yellow with transmissionFactor = 1.0.</em></figcaption>
</figure>

### transmissionTexture 
The `R` channel of this texture defines the amount of light that is transmitted by the surface rather than diffusely re-emitted. A value of 1.0 in the red channel means that 100% of the light that penetrates the surface (i.e. isn’t specularly reflected) is transmitted through. The value is linear and is multiplied by the transmissionFactor to determine the total transmission value.

<figure>
  <img src="./figures/TransmissionTexture.png"/>
<figcaption><em>Controlling transmission amount with transmissionTexture.</em></figcaption>
</figure>

## Tint
You may notice that the transparent materials show above are tinted. i.e. they aren't actually transmitting 100% of the non-reflected light. This is because `KHR_materials_transmission` specifies that the `baseColor` of the material be used to model the absorption of light by the surface. This allows materials like stained glass to be easily represented with this extension.
Absorption is usually defined as an amount of light at each frequency that is absorbed over a given distance through a medium (usually described by Beer’s Law). However, since this extension deals exclusively with infinitely thin surfaces, we can treat absorption as a constant. In fact, rather than absorbed light, we can talk about its inverse: transmitted light. The `baseColor` of the material serves this purpose as it already defines how the light that penetrates the surface is colored by the material. In this model, the transmitted light will be modulated by this color as it passes through.
<figure>
  <img src="./figures/ConstantTransmission.png"/>
<figcaption><em>The baseColor of the material (yellow, in this example) is used to tint the light being transmitted.</em></figcaption>
</figure>

## Refractive Blurring
Since the surface is considered to be infinitely thin, we will ignore macroscopic refraction caused by the orientation of the surface. However, microfacets on either side of the thin surface will cause light to be refracted in random directions, effectively blurring the transmitted light. That is, the roughness of the surface directly causes the transmitted light to become blurred. This microfacet lobe is exactly the same as the specular lobe except sampled along the line of sight through the surface.
<figure>
  <img src="./figures/Roughness.png"/>
<figcaption><em>Refraction due to surface roughness.</em></figcaption>
</figure>

## Blending
The glTF `blendMode` is used for alpha-as-coverage, NOT for physically-based transparency (i.e. this extension). If alpha-as-coverage is not being used, the blend mode of the material should be set to "OPAQUE" even though it is transparent. Again, it's helpful to think of alpha-as-coverage as whether the physical surface is there or not. `transmission` applies to the surface material that exists.
Note that alpha-as-coverage can still be used along with transmission as shown in the example below.
<figure>
  <img src="./figures/TransmissionWithMask.png"/>
<figcaption><em>Alpha coverage and optical transparency can be used at the same time so that some areas of a surface are transparent while others disappear entirely.</em></figcaption>
</figure>

## Transparent Metals
Metals effectively absorb all refracted light (light that isn't reflected), preventing transmission.
The metallic parameter of a glTF material effectively scales the `baseColor` of the material toward black while, at the same time scaling the F0 (reflectivity) value towards 1.0. This makes the material opaque for metallic values of 1.0 because transmitted light is attenuated out by absorption. Therefore, for a material with `metallicFactor=1.0`, the value of `transmissionFactor` doesn't matter.

## Implementing Transmission ##
From the core [glTF BRDF](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#appendix-b-brdf-implementation), we have:

*f* = *f*<sub>*diffuse*</sub> + *f*<sub>*specular*</sub>

*f*<sub>*diffuse*</sub> = (1 - *F*) * *diffuse*

*f*<sub>*specular*</sub> = *F* * *G* * *D* / (4 * dot(*N*, *L*) * dot(*N*, *V*)), where *F* is the Surface Reflection Ratio.

We will now add an additional term for the transmitted light:

*f* = *f*<sub>*diffuse*</sub> + *f*<sub>*specular*</sub> + *f*<sub>*transmission*</sub>

*f*<sub>*transmission*</sub> = (1 - *F*) * *T* * *D<sub>T</sub>* * *baseColor*

where *T* is the transmission percentage defined by this extension's `transmission` and `transmissionTexture` properties and *D<sub>T</sub>* is the distribution function for the transmitted light. The distribution function is the same Trowbridge-Reitz model used by specular reflection except sampled along the view vector rather than the reflection. The *baseColor* factor causes the transmitted light to be tinted by the surface.

Light that penetrates a surface and is transmitted will not be diffusely reflected so we also need to modify the diffuse calculation to account for this.
*f*<sub>*diffuse*</sub> = (1 - *F*) * (1 - *T*) * *diffuse*

Optical transparency does not require any changes whatsoever to the specular term. So this can all be rearranged so that the transmitted light amounts to a modifaction of the diffuse lobe.
*f* = (1 - *F*) * (*T* * *D<sub>T</sub>* + (1 - *T*) * *diffuse*) + *f*<sub>*specular*</sub>

## Implementation Notes
Rendering transparency in a real-time rasterizer in an efficient manner is a difficult problem, especially when an arbitrary number of transparent polygons may be overlapping in view. This is because rendering transparency is order-dependent (i.e. we see background objects through foreground objects) and also because the rendering of absorption and reflections involves two distinct blend modes. Consequently, it may not be possible on the target platform to render every transparent polygon in the correct order, in a reasonable time, and with the correct blending. Therefore, correct ordering is not an absolute requirement when implementing this extension in realtime renderers, nor is rendering all potentially overlapping layers. However, it is expected that, at a minimum, opaque objects will be visible through a transmitting surface and the blending of the surface will match the above spec.
<figure>
<img src="./figures/Multi-layer Transparency.png"/>
<figcaption><em>On the left, only opaque objects are visible through the sphere. Each subsequent image shows an additional layer of transparency visible through the front-most transparent surface. The minimum expectation for a compliant renderer is to render only opaque objects (i.e. like the first image). Additional transparent layers may be rendered but are not required.</em></figcaption>
</figure>