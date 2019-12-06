# KHR_materials_volume

## Contributors

* TODO: Name, affiliation, and contact info for each contributor

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. Needs to be combined with `KHR_materials_transmission`.

## Overview

This extension adds material parameters to configure the volume beneath the surface of closed meshes, providing effects like absorption and subsurface scattering. The transparency of the surface is controlled by `KHR_materials_transmission`.

Light hitting an object will first interact with the surface according the BSDF. The BSDF determines how much and in which direction the incident light is reflected and transmitted. Light that is transmitted enters the volume, where it is absorbed or scattered by particles inside the medium. Finally, the light hits the surface again from the backside, evaluating the BSDF a second time to leave the object.

The light rays are reflected or refracted at the micro-facets, taking the roughness of the material into account. The index of refraction used for computing the refraction direction is taken from the BSDF, potentially set by `KHR_materials_ior`.

<figure style="text-align:center">
<img src="./figures/volume.svg"/>
<figcaption>Object with homogeneous volume and an index of refraction of 1.5.</figcaption>
</figure>

## Extending Materials

The volumetric properties are defined by adding the `KHR_materials_volume` extension to any glTF material.

```json
materials: [
    {
        "extensions": {
            "KHR_materials_volume": {
                "sigmaT": [ 78.03, 17.83, 126.29 ],
                "albedo": [ 0.29, 0.82, 0.99 ],
                "g": 0.0
            }
        }
    }
]
```

Note that there are no textures in this extension, the medium is homogeneous. If a texture is defined in `KHR_materials_ior`, it is ignored if `KHR_materials_volume` is active.

## Properties

The extension provides two parameters to describe the medium and one parameter to control the phase function.

| | Type | Description | Required |
|-|------|-------------|----------|
| **sigmaT** | `number[3]` | Attenuation coefficient in inverse scene units. | No, default: `[0.0, 0.0, 0.0]` |
| **albedo** | `number[3]` | Single-scattering albedo. | No, default: `[0.0, 0.0, 0.0]` |
| **g** | `number` | Anisotropy of the phase function in range [-1, 1] | No, default: `0` | 

### Conversions

Alternatively, subsurface scattering can be parametrized with absorption coefficient **sigmaA** and scattering coefficient **sigmaT**. These are converted as follows:

```
sigmaT = sigmaS + sigmaA
albedo = sigmaS / (sigmaS + sigmaA)
```

Sometimes the mean free path length **mfp** is given:

```
sigmaT = 1 / mfp
```

The attenuation color is a 3-channel value that goes up to infinity. By splitting it into two values, an RGB attenuation color and a distance, the color is easier to control:

```
sigmaT = -log(attenuationColor) / attenuationDistance
```

A more user-friendly parametrization for albedo is proposed in [Kulla and Conty (2017): Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf). It shows how to map from the overall color of the material to single-scattering albedo:

```
s = 4.09712 + 4.20863 * color - sqrt(9.59217 + 41.68086 * color + 17.7126 * color^2)
albedo = (1 - s^2) / (1 - gs^2)
```

## Known Implementations

* TODO: List of known implementations, with links to each if available.

## Resources

* TODO: Resources, if any.
