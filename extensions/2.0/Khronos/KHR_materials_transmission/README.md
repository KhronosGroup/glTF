# KHR\_materials\_transmission

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Work in Progress

## DependenciesI

Written against the glTF 2.0 spec.

## Overview

Many optically transparent materials are impossible to represent in a physically plausible manner with the core glTF 2.0 PBR material. Alpha coverage (exposed via the alpha channel of `baseColorTexture`) is useful for transparent materials such as gauze that don’t reflect, refract, absorb or scatter light. However, most physical materials don’t fit into this category. Transparent glass and plastics are great examples of this. In the simplest case, specular reflections off the surface of the glass should still be visible on a fully transparent mesh. Using alpha coverage of 0% would make the reflections invisible as well.

<figure>
<img src="./figures/OpacityComparison.png"/>
<figcaption><em>Alpha coverage of 20% (left) vs 80% optical transparency (right). Note that alpha coverage can't make the surface colourless nor can it produce any absorption effects. It also attenuates the specular reflections, making them disappear completely when opacity is 0.0.</em></figcaption>
</figure>

This extension provides a way to define glTF 2.0 materials that are transparent to light in a physically plausible way. That is, it enables the creation of transparent materials that absorb, reflect and transmit light depending on the incident angle and the wavelength of light. Common uses cases for thin-surface transmissive materials include plastics and glass.

This extension aims to address the simplest and most common use cases for optical transparency: infinitely-thin materials with no complex scattering (e.g. dynamic scattering) or dispersion. Dealing exclusively with “thin” materials (i.e. materials where only the surface is considered and not the volume) allows many simplifications to be made when calculating things like refraction and absorption.

## Extending Materials

A transparent material is defined by adding the `KHR_materials_transmission` extension to any glTF material. When present, the extension indicates that a material should be rendered as a transparent surface and be blended as defined in the following spec. Alpha coverage and `doubleSided` properties still apply to transparent materials. Note that, as rendering transparent objects presents many difficult-to-solve issues with primitive ordering, this extension does not dictate rendering algorithms.

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
|**transmissionFactor** | `number` | The base percentage of light that is transmitted through the surface.| No, default:`1.0`|
|**transmissionTexture** | [`textureInfo`] | A greyscale texture that defines the transmission percentage of the surface. This will be multiplied by transmissionFactor. | No |

### transmissionFactor 

The amount of light that is transmitted by the surface rather than diffusely re-emitted. This is a percentage of all the light that penetrates a surface (i.e. isn’t specularly reflected) rather than a percentage of the total light that hits a surface. A value of 1.0 means that 100% of the light that penetrates the surface is transmitted through. This is the default.

<figure>
  <img src="./figures/ConstantTransmission.png"/>
<figcaption><em>Defaulting to 1.0 allows making a material transparent by simple defining the extension.</em></figcaption>
</figure>

### transmissionTexture 

A greyscale texture that defines the amount of light that is transmitted by the surface rather than absorbed and re-emitted. A value of 1.0 means that 100% of the light that penetrates the surface (i.e. isn’t specularly reflected) is transmitted through. The value is linear and is multiplied by the transmissionFactor to determine the total transmission value. Note that use of transmission values does not disallow usage of alpha coverage (via `baseColor.a`).

<figure>
  <img src="./figures/TransmissionTexture.png"/>
<figcaption><em>Controlling transmission amount with transmissionTexture.</em></figcaption>
</figure>

## Implementing Transmission ##

From the core [glTF BRDF](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#appendix-b-brdf-implementation), we have:

*f* = *f*<sub>*diffuse*</sub> + *f*<sub>*specular*</sub>
*f*<sub>*diffuse*</sub> = (1 - *F*) * *diffuse*
*f*<sub>*specular*</sub> = *F* * *G* * *D* / (4 * dot(*N*, *L*) * dot(*N*, *V*))
, where *F* is the Surface Reflection Ratio.

We will now add an additional term for the transmitted light:

*f* = *f*<sub>*diffuse*</sub> + *f*<sub>*specular*</sub> + *f*<sub>*transmission*</sub>

*f*<sub>*transmission*</sub> = (1 - *F*) * *T* * *D<sub>T</sub>* * *baseColor*
where *T* is the transmission percentage defined by this extension's `transmission` and `transmissionTexture` properties and *D<sub>T</sub>* is the distribution function for the transmitted light. The distribution function is the same Trowbridge-Reitz model used by specular reflection except sampled along the view vector rather than the reflection. The *baseColor* factor causes the transmitted light to be tinted by the surface.

Light that penetrates a surface and is transmitted will not be diffusely reflected so we also need to modify the diffuse calculation to account for this.
*f*<sub>*diffuse*</sub> = (1 - *F*) * (1 - *T*) * *diffuse*

Optical transparency does not require any changes whatsoever to the specular term.

## Transparent Metals

Metals effectively absorb all refracted light (light that isn't reflected), preventing transmission.
The metallic parameter of a glTF material effectively scales the `baseColor` of the material toward black while, at the same time scaling the F0 (reflectivity) value towards 1.0. This makes the material opaque for metallic values of 1.0 because transmitted light is attenuated out by absorption.

## Modeling Absorption

Absorption is usually defined as an amount of light at each frequency that is absorbed over a given distance through a medium (usually described by Beer’s Law). However, since this extension deals exclusively with very thin surfaces, we can treat absorption as a constant. In fact, rather than absorbed light, we can talk about its inverse: transmitted light. The `baseColor` of the material serves this purpose as it already defines how the light that penetrates the surface is colored by the material. In this model, the transmitted light will be modulated by this color as it passes through.

<figure>
  <img src="./figures/ConstantTransmission.png"/>
<figcaption><em>The baseColor of the material is used to tint the light being transmitted.</em></figcaption>
</figure>

### Modeling Refraction

Since the surface is considered to be infinitely thin, we will ignore macroscopic refraction caused by the orientation of the surface. However, microfacets on either side of the thin surface will cause light to be refracted in random directions, effectively blurring the transmitted light. This microfacet lobe is exactly the same as the specular lobe except sampled along the line of sight through the surface.


## Realtime Implementation Notes

Implementations of transmission are expected to vary, especially in real-time engines, depending on the capabilities of the runtime, but should endeavor to achieve plausibility. 


### Refractive Blurring
Some rasterizer implementations may opt to sample prefiltered IBL or dynamic light probes to simulate refractive blurring due to surface roughness. Others may choose to sample from a blurred version of the rendered scene behind a surface. Raytracers are expected to use more physically correct refraction with a BSDF supporting multi-scattering like the modified [Smith](https://eheitzresearch.wordpress.com/240-2/) model.

<figure>
  <img src="./figures/Roughness.png"/>
<figcaption><em>Refraction due to surface roughness.</em></figcaption>
</figure>

**TODO** - *This is fine for raytracers or for rasterizers that just sample the existing IBL for refracted light. However, what about rasterizers that want to cheaply render transmission in screen-space (I think this is pretty common)? Typically, the refractive blurring is done by sampling mips or manually-blurred versions of the background scene. This can be eye-balled by an implementation but is there a slightly more mathematically rigorous version that we could recommend?*

### Blending

Although the math here is straightforward, it doesn't consist of a single blend equation and so, in practice, can't be rendered in a single draw call with blending enabled. Also note that alpha coverage still needs to work as it does with the core glTF material spec; blending between the destination framebuffer and this surface material. Therefore, `blendMode` should, in general, be set to "OPAQUE" unless alpha coverage is used.

<figure>
  <img src="./figures/TransmissionWithMask.png"/>
<figcaption><em>Alpha coverage and optical transparency can be used at the same time so that some areas of a surface are transparent while others disappear entirely.</em></figcaption>
</figure>

### Multiple Layers

Rendering transparency in an efficient manner is a difficult problem, especially when an arbitrary number of transparent polygons may be overlapping in view. This is because the rendering is order-dependent (i.e. we see background objects through forground objects). It may not be possible on the target platform to render every transparent polygon in the correct order in a reasonable time. Therefore, correct ordering is not an absolute requirement when implementing this extension in realtime renderers (offline renderers are assumed to not have this ordering issue). However, it is expected that reflected and transmitted light are blended in the way defined by this extension.