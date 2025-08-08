# EXT\_materials\_clearcoat\_darkening

## Contributors

- Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
- Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.
Must be defined as a sub-extension under the `KHR_materials_clearcoat` extension.

## Exclusions

- This extension must not be used on a material that also uses `KHR_materials_pbrSpecularGlossiness`.
- This extension must not be used on a material that also uses `KHR_materials_unlit`.

## Overview

This extension defines the amount that the clear coat darkens the underlying surface due to internal reflections within the coating. The default value of 1 implies that the physically-correct amount of reflection occurs, based on the
IOR of the coating. This behaviour, however, may not always be desired by the author as it changes the base color of the surface. Being able to turn this effect down (or off) may sometimes be desired.

<figure>
<img src=".\DarkeningCompare.jpg"/>
<figcaption>
No darkening on the left and physically-correct darkening on the right.
</figcaption>
</figure>

## Extending Materials

Adding this functionality to clearcoat materials can be done by adding the `EXT_materials_clearcoat_darkening` extension to any glTF material that already has the `KHR_materials_clearcoat` extension defined on it.  For example, the following defines a material with a dark red varnish.

```json
{
    "materials": [
        {
            "name": "varnish",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0,
                    "extensions": {
                        "EXT_materials_clearcoat_darkening": {
                            "clearcoatDarkeningFactor": 0.5
                        }
                    }
                }
            }
        }
    ]
}
```

### Clearcoat Darkening

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [Appendix B](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                            | Description                            | Required             |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|----------------------|
|**clearcoatDarkeningFactor**    | `number`                                                                                       | The % of physically-correct darkening due to interreflections within the coat.  | No, default: `1.0`   |
|**clearcoatDarkeningTexture**   | [`textureInfo`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo)  | The texture containing the % of physically-correct darkening due to interreflections within the coating. Texture values are assumed to be linear.  | No                               |

As with other factor/texture combinations, the factor and related texture value are multiplied to get the final per-texel darkening value.
```
clearcoat_darkening.rgb = clearcoatDarkeningFactor.rgb * clearcoatDarkeningTexture.r
```

## Physical Interpretation

The "darkening" being adjusted by this extension is caused by light entering the coat and bouncing around internally before reaching the base layer. This reduces the effective transmission and therefore the amount of light hitting the base layer.

Things that affect the amount of darkening observed:
1. Fresnel Effect: Higher IOR coats have stronger internal reflections, causing more darkening.

1. Viewing Angle Dependency: Grazing angles have higher Fresnel reflectance, causing more darkening.

1. Roughness Modulation: Rougher coats scatter light internally, reducing the coherent reflection effect. Increased diffuse internal reflections results in less darkening.

## Implementation (non-normative)

Some renderers (such as path-tracers) may already model this darkening behaviour by the nature of their light transport algorithms. In these cases, implementing support for this extension will involve adding a term to compensate for the energy loss when the darknening value is < 1.0. The [specification of OpenPBR](https://academysoftwarefoundation.github.io/OpenPBR/index.html#model/coat/darkening) contains a detailed description of what this entails.

Other renderers (such as rasterizers) will most likely need to add logic to approximate the loss of energy due to these internal reflections when the darkening value is > 0.0. We provide some example logic to do this below:

### Real-time Implementation

We want to calculate a multiplier that represents the amount of transmitted light that makes its way through the coat and impacts the underlying layer. To approximate this, we want to find the average reflectance, $R$, of the coat and then model an infinite number of reflections using a geometric series to get the total transmission, $T$.

$T = (1-R) / (1 + R + R² + R³ + ...)$
$T = (1-R) / (1/(1-R))$
$T = (1-R)²$

The $(1-R)$ in the numerator represents the initial transmission of light through the coat and the denominator accounts for the infinite reflections within the coating. This converges to $1/(1-R)$.

By "average reflectance", we mean the average reflectance for the round trip (light in, view out) so it can be computed as the sum of the Schlick Fresnel approximation for $dot(N, V)$ and $dot(N, L)$ and then dividing by 2.
$$R_{directlight} = (fresnel(N, V) + fresnel(N, L)) * 0.5$$
For environment lighting (IBL), however, light is coming from all angles so we calculate the hemisphere-averaged reflectance as $F_0 + 0.5 * F_{90}$ which gives a value halfway between $F_0$ and $F_{90}$. We then add to this the Schlick Fresnel approximation for $dot(N, V)$ and divide by 2.
$$R_{IBL} = (fresnel(N, V) + F_0 + 0.5 * F_{90}) * 0.5$$


When coat roughness increases, light is diffused and darkening decreases. We can approximate this empirically by scaling the average reflectance, $R$, by $1.0 - M_c * 0.5$ where $M_c$ is the roughness of the clearcoat.

## Schema

- [material.EXT_materials_clearcoat_darkening.schema.json](schema/material.EXT_materials_clearcoat_darkening.schema.json)
 
