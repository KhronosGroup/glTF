# KHR\_materials\_transmission

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Work in Progress

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Many optically transparent materials are impossible to represent in a physically plausible manner with the core glTF 2.0 PBR material. Alpha coverage (exposed via the alpha channel of `baseColorTexture`) is useful for transparent materials such as gauze that don’t reflect, refract, absorb or scatter light. However, most physical materials don’t fit into this category. Transparent glass and plastics are great examples of this. In the simplest case, specular reflections off the surface of the glass should still be visible on a fully transparent mesh. Using alpha coverage of 0% would make the reflections invisible as well.

<figure>
<img src="./figures/coverage_vs_transparency.png"/>
<figcaption><em>Alpha coverage of 20% (left) vs 100% optical transparency (right). Note that alpha coverage can't make the surface colourless, nor can it leave the specular reflections unaffected.</em></figcaption>
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

### Extension compatibility

Rendering transparency in an efficient manner is a difficult problem. It may not be possible on the target platform to render every transparent polygon in the correct order in a reasonable time. Therefore, this is not a requirement to use this extension with realtime renderers (offline renderers are assumed to not have this ordering issue). However, it is expected that reflected and transmitted light are rendered in the way defined by this extension.

## Properties 

Only two properties are introduced with this extension and combine to describe a single value; the percentage of light that is transmitted through the surface of the material. These properties work together with the existing properties of the material to define the way light is modified as it passes through the substance. 

|   |Type|Description|Required|
|---|----|-----------|--------|
|**transmissionFactor** | `number` | The base percentage of light that is transmitted through the surface.| No, default:`1.0`|
|**transmissionTexture** | [`textureInfo`] | A greyscale texture that defines the transmission percentage of the surface. This will be multiplied by transmissionFactor. | No |

### transmissionFactor 

The amount of light that is transmitted by the surface rather than diffusely re-emitted. This is a percentage of all the light that penetrates a surface (i.e. isn’t specularly reflected) rather than a percentage of the total light that hits a surface. A value of 1.0 means that 100% of the light that penetrates the surface is transmitted through. 

### transmissionTexture 

A greyscale texture that defines the amount of light that is transmitted by the surface rather than absorbed and re-emitted. A value of 1.0 means that 100% of the light that penetrates the surface (i.e. isn’t specularly reflected) is transmitted through. The value is linear and is multiplied by the transmissionFactor to determine the total transmission value. Note that use of transmission values does not disallow usage of alpha coverage (via `baseColor.a`).

<figure>
  <img src="./figures/coverage_and_transparency.png"/>
<figcaption><em>Alpha coverage and optical transparency can be used at the same time so that some areas of a surface are transparent while others disappear entirely.</em></figcaption>
</figure>

## Implementing Transmission ##

Modeling transmission is relatively straightforward. From the [glTF BRDF](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#appendix-b-brdf-implementation), we have:

*f* = *f*<sub>*diffuse*</sub> + *f*<sub>*specular*</sub>
*f*<sub>*diffuse*</sub> = (1 - *F*) * *diffuse*
*f*<sub>*specular*</sub> = *F* * *G* * *D* / (4 * dot(*N*, *L*) * dot(*N*, *V*))
, where *F* is the Surface Reflection Ratio.

*f* = *f*<sub>*diffuse*</sub> + *f*<sub>*specular*</sub>

Optical transparency does not require any changes whatsoever to the specular term so we only want to modify the diffuse term to account for transmitted light as well. We can do this by blending *f*<sub>*diffuse*</sub> with the transmitted light, **L**<sub>transmitted</sub>, based on the `transmission` values.

*f* = mix(**f**<sub>*diffuse*</sub>, **L**<sub>transmitted</sub>, **T**) + *f*<sub>*specular*</sub>

Where **L**<sub>transmitted</sub> is the colour of light coming through the material, towards the eye. It is tinted (multiplied) by the `baseColor`, and already has refraction taken into account (as discussed below). *T* is the transmission value defined by this extension.

## Modeling Absorption

Absorption is usually defined as an amount of light at each frequency that is absorbed over a given distance through a medium (usually described by Beer’s Law). However, since this extension deals exclusively with very thin surfaces, we can treat absorption as a constant. In fact, rather than absorbed light, we can talk about its inverse: transmitted light. The `baseColor` of the material serves this purpose as it already defines how the light that penetrates the surface is colored by the material. In this model, the transmitted light will be modulated by this color as it passes through.

<figure>
  <img src="./figures/surface_tint.png"/>
<figcaption><em>The baseColor of the material is used to tint the light being transmitted.</em></figcaption>
</figure>

### Modeling Refraction

Since the surface is considered to be infinitely thin, we end up with a non-physically correct model. One the one hand, the material has no volume so light shouldn't be refracted when passing through. However, it should be expected that microfacets in a rough surface would refract the transmitted light in such a way as to blur the transmitted image. `How do we define this, mathematically??? By its nature, it's non-physically-based.`

Implementations of this are expected to vary widely, especially in real-time engines, depending on the capabilities of the runtime, but should endeavor to achieve plausibility. Simple implementations may opt to apply prefiltered IBL or dynamic light probes in a refractive manner to simulate scattering while others may choose to sample from a blurred version of the rendered scene while rendering transparency. Raytracers are expected to use more physically correct refraction with a BSDF supporting multi-scattering like the modified [Smith](https://eheitzresearch.wordpress.com/240-2/) model.


<figure>
  <img src="./figures/surface_roughness_dimension.png"/>
<figcaption><em>Roughness of 0, 0.5 and 1.0 using renderer in Adobe Dimension. Note that an issue in Dimension at the time of writing is causing rough surfaces to appear too dark.</em></figcaption>
</figure>

<figure>
  <img src="./figures/surface_roughness_babylon.png"/>
<figcaption><em>Simple implementation using Babylon.js engine. This implementation uses the mip levels of the captured opaque scene to approximate refraction of transmitted light.</em></figcaption>
</figure>
