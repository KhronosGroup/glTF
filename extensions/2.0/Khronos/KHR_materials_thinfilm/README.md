# KHR\_materials\_thinfilm

## Khronos 3D Formats Working Group

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

TODO  

## Extending Materials

The thin film materials are defined by adding the `KHR_materials_thinfilm` extension to any glTF material.

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_thinfilm": {
                    "thinfilmFactor": 1.0
                }
            }
        }
    ]
}
```

### Thinfilm

All implementations should use the same calculations for the BRDF inputs. Implementations of the BRDF itself can vary based on device performance and resource constraints. See [appendix](/specification/2.0/README.md#appendix-b-brdf-implementation) for more details on the BRDF calculations.

|                             | Type                                                                | Description                      | Required              |
|-----------------------------|---------------------------------------------------------------------|----------------------------------|-----------------------|
|**thinfilmFactor**           | `number`                                                            | The thin film intensity.         | No, default: `0.0`    |
|**thinfilmTexture**          | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The thin film intensity texture. | No                    |
|**thinfilmThicknessMinimum** | `number`                                                            | The thin film minimum thickness. | No, default: `400.0`  |
|**thinfilmThicknessMaximum** | `number`                                                            | The thin film maximum thickness. | No, default: `1200.0` |
|**thinfilmThicknessTexture** | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The thin film thickness texture. | No                    |

```glsl
float thinfilmThickness = mix(thinfilmThicknessMinimum, thinfilmThicknessMaximum, thinfilmTexture.g) / 1200.0;
float thinfilmIntensity = thinfilmTexture.r * thinfilmFactor;
```

The thin film effect is overwriting the F (Surface Reflection Ratio) from the specular glossiness effect.

```glsl
vec3 thinfilmSpecularReflection(vec3 F0, float VdotH, float thinfilmIntensity, float thinfilmThicknessNormalized)
{
    vec3 F = fresnelReflection(F0, VdotH);

    if (thinfilmIntensity == 0.0)
    {
        return F;
    }

    vec3 lutSample = texture(lutTexture, thinfilmThicknessNormalized, VdotH)).rgb - 0.5;
    vec3 intensity = thinfilmIntensity * 4.0 * F0 * (1.0 - F0);
    return clamp(lutSample * intensity + F, 0.0, 1.0);    
}
```

## Appendix

TODO

## Reference

### Theory, Documentation and Implementations

[A Practical Extension to Microfacet Theory for the Modeling of Varying Iridescence](https://belcour.github.io/blog/research/2017/05/01/brdf-thin-film.html)
[Practical Multilayered Materials - Call of Duty - page 62](https://blog.selfshadow.com/publications/s2017-shading-course/drobot/s2017_pbs_multilayered.pdf)
