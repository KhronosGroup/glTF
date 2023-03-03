# KHR\_materials\_anisotropy

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)
* Ed Mackey, AGI [@emackey](https://github.com/emackey)
* Alex Wood, AGI [@abwood](https://twitter.com/abwood)
* Nicolas Savva, Autodesk, [@nicolassavva-autodesk](https://github.com/nicolassavva-autodesk)
* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
* Don McCurdy, Google [@donrmccurdy](https://twitter.com/donrmccurdy)
* Emmett Lalish, Google [@elalish](https://github.com/elalish)
* Alexey Knyazev, Individual Contributor, [@lexaknyazev](https://github.com/lexaknyazev)
* Bruce Cherniak, Intel
* Gary Hsu, Microsoft [@bghgary](https://twitter.com/bghgary)
* Ben Houston, Threekit, [@bhouston](https://twitter.com/BenHouston3D)
* Jim Eckerlein, UX3D
* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)
* Eric Chadwick, Wayfair, [echadwick-wayfair](https://github.com/echadwick-wayfair)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines the anisotropic property of a material as observable with brushed metals for instance.
An asymmetric specular lobe model is introduced to allow for such phenomena.
The visually distinct feature of that lobe is the elongated appearance of the specular reflection.
For a single punctual light source, the specular reflection will eventually degenerate into a zero width line in the limit,
that is where the material is fully anisotropic, as opposed to be fully isotropic in which case the specular reflection is radially symmetric.

## Extending Materials

Sample values:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_anisotropy": {
                    "anisotropyFactor": 0.6,
                    "anisotropyDirection": [1.0, -1.0],
                    "anisotropyTexture": {
                        "index": 0
                    }
                }
            }
        }
    ]
}
```

|                         | Type                                                                | Description               | Required                  |
| ----------------------- | ------------------------------------------------------------------- | ------------------------- | ------------------------- |
| **anisotropyFactor**    | `number`                                                            | The anisotropy strength.  | No, default: `0.0`        |
| **anisotropyDirection** | `array`                                                             | The direction of the anisotropy in tangent, bitangent space. The reader is expected to normalize this. The presence of anisotropyTexture completely overrides this value. | No, default: `[1.0, 0.0]` |
| **anisotropyTexture**   | [`textureInfo`](/specification/2.0/README.md#reference-textureInfo) | The anisotropy texture. Red and green channels represent the anisotropy direction in [-1, 1] tangent, bitangent space. The magnitude of this vector is multiplied by anisotropyFactor to obtain the anisotropy strength.   | No                        |

## Anisotropy

Two new material properties are introduced: an explicit anisotropyFactor parameter and the direction in which the specular reflection elongates relative to the surface tangents.
The anisotropyFactor parameter is a dimensionless number in the range `[0, 1]` and forms an injective relation to the roughness distribution along two orthogonal directions, one of which is the direction parameter and the other the result of crossing the direction and the geometric normal.

| Direction    | Anisotropy 0.5              | Anisotropy 0.0              |
| ------------ | --------------------------- | --------------------------- |
| `[1.0, 0.0]` | ![Fig. 3](figures/fig3.jpg) | ![Fig. 2](figures/fig2.jpg) |
| `[1.0, 1.0]` | ![Fig. 6](figures/fig6.jpg) | ![Fig. 5](figures/fig5.jpg) |
| `[0.0, 1.0]` | ![Fig. 1](figures/fig1.jpg) | ![Fig. 5](figures/fig5.jpg) |

To achieve certain surface finishes, it is possible to define the anisotropy and its direction using a texture.

![Fig. 8](figures/fig8.jpg)
![Fig. 10](figures/fig10.jpg)

## Implementation

While uniform and textured anisotropy are multiplied, uniform and textured direction definitions are mutually exclusive and the latter overrides the former. (While it is surely possible to add both directions in terms of their complex argument, the computational overhead may not justify the additional editorial convenience.)

```glsl
float anisotropy = u_Anisotropy;
vec3 direction = normalize(vec3(u_AnisotropyDirection, 0.0));

#if HAS_ANISOTROPY_MAP
direction = texture(uv, u_AnisotropyDirectionSampler).xy * 2.0 - vec2(1.0);
anisotropy *= length(direction);
#endif

vec3 anisotropicT = normalize(TBN * direction);
vec3 anisotropicB = normalize(cross(normal_geometric, anisotropicT));

float TdotL = dot(anisotropicT, l);
float BdotL = dot(anisotropicB, l);
float TdotH = dot(anisotropicT, h);
float BdotH = dot(anisotropicB, h);

f_specular += intensity * NdotL * BRDF_specularAnisotropicGGX(f0, f90, alphaRoughness,
    VdotH, NdotL, NdotV, NdotH,
    BdotV, TdotV, TdotL, BdotL, TdotH, BdotH, anisotropy);
```

The anisotropic GGX is defined as follows:

```glsl
vec3 BRDF_specularAnisotropicGGX(vec3 f0, vec3 f90, float alphaRoughness,
    float VdotH, float NdotL, float NdotV, float NdotH, float BdotV, float TdotV,
    float TdotL, float BdotL, float TdotH, float BdotH, float anisotropy)
{
    float at = max(alphaRoughness * (1.0 + anisotropy), 0.00001);
    float ab = max(alphaRoughness * (1.0 - anisotropy), 0.00001);

    vec3 F = F_Schlick(f0, f90, VdotH);
    float V = V_GGX_anisotropic(NdotL, NdotV, BdotV, TdotV, TdotL, BdotL, anisotropy, at, ab);
    float D = D_GGX_anisotropic(NdotH, TdotH, BdotH, anisotropy, at, ab);

    return F * V * D;
}

float D_GGX_anisotropic(float NdotH, float TdotH, float BdotH, float anisotropy, float at, float ab)
{
    float a2 = at * ab;
    vec3 f = vec3(ab * TdotH, at * BdotH, a2 * NdotH);
    float w2 = a2 / dot(f, f);
    return a2 * w2 * w2 / M_PI;
}

float V_GGX_anisotropic(float NdotL, float NdotV, float BdotV, float TdotV, float TdotL, float BdotL,
    float anisotropy, float at, float ab)
{
    float GGXV = NdotL * length(vec3(at * TdotV, ab * BdotV, NdotV));
    float GGXL = NdotV * length(vec3(at * TdotL, ab * BdotL, NdotL));
    float v = 0.5 / (GGXV + GGXL);
    return clamp(v, 0.0, 1.0);
}
```

The parametrization of `at` and `ab`, denoting roughness values along both anisotropic directions respectively, is taken from [4].

## Reference

1. [Google Filament - Anisotropic model](https://google.github.io/filament/Filament.md.html#materialsystem/anisotropicmodel)
2. [Google Filament Materials Guide - Anisotropic model](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/anisotropy)
3. [Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)
4. [Christopher Kulla and Alejandro Conty. 2017. Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf)
