# KHR\_materials\_sheen

## Khronos 3D Formats Working Group

* TODO

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

TODO

## Extending Materials

The PBR sheen materials are defined by adding the `KHR_materials_sheen` extension to any glTF material. 
For example, the following defines a material like velvet.

```json
{
    "materials": [
        {
            "name": "velvet",
            "extensions": {
                "KHR_materials_sheen": {
                    "sheenFactor": 1.0
                }
            }
        }
    ]
}
```

### Sheen

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                                  | Type                                                                            | Description                            | Required                       |
|----------------------------------|---------------------------------------------------------------------------------|----------------------------------------|--------------------------------|
|**sheenFactor**                   | `number`                                                                        | The sheen intensity.                   | No, default: `1.0`             |
|**sheenColor**                    | `array`                                                                         | The sheen color.                       | No, default: `[1.0, 1.0, 1.0]` |
|**sheenTexture**                  | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo)             | The sheen color and intensity texture. | No                             |
|

The sheen roughness is identical to the material roughness to simplify the configuration. `sheenRoughness = materialRoughness`

The sheen formula `f_sheen` is different from the specular and clear coat BRDF.
```
sheenTerm = sheenColor * sheenIntensity * sheenDistribution * sheenVisibility;
```

As you notice there is no fresnel for the sheen layer.

The sheen distribution follows the "Charlie" sheen definition from ImageWorks (Estevez and Kulla):
```
alphaG = sheenRoughness * sheenRoughness
invR = 1 / alphaG
cos2h = NdotH * NdotH
sin2h = 1 - cos2h
sheenDistribution = (2 + invR) * pow(sin2h, invR * 0.5) / (2 * PI);
```

The sheen visibility is following the one defined by Ashkimin:
```
sheenVisibility = 1 / (4 * (ndotl + ndotv - ndotl * ndotv))
```

The full layer would then be:
```
f_sheen = lightColor * sheenTerm * PI * ndotl
```

The following abstract code describes how the base and sheen layers should be blended together:
```
specularity = mix(metallicF0, baseColor, metallic)
reflectance = max(max(specularity.r, specularity.g), specularity.b)

final = f_emissive + f_diffuse + f_specular + (1 - reflectance) * f_sheen
```

If `sheenFactor = 0`, the sheen layer is disabled and the material is behaving like the core Metallic-Roughness material:

```
f = f_emissive + f_diffuse + f_specular
```
  
If textures are not set, the default values of the clearcoat layer textures are used and the values are not inherited from the underlying Metallic-Roughness (or other) material. If one wants to have the same textures, one have to explicitly set the same texture sources.

## Appendix

TODO

## Reference

### Theory, Documentation and Implementations

[NP13 David Neubelt, Matt Pettineo – “Crafting a Next-Gen Material Pipeline for The Order: 1886”, SIGGRAPH 2013](https://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf)
[EK17 Alejandro Conty Estevez, Christopher Kulla – “Production Friendly Microfacet Sheen BRDF”, SIGGRAPH 2017](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_sheen.pdf)
[AS07 Michael Ashikhmin, Simon Premoze – “Distribution-based BRDFs”, 2007](http://www.cs.utah.edu/~premoze/dbrdf/dBRDF.pdf)
[cloth-shading](https://knarkowicz.wordpress.com/2018/01/04/cloth-shading/)