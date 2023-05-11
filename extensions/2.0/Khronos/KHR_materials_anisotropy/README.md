# KHR\_materials\_anisotropy

## Contributors

* Mike Bond, Adobe, [@miibond](https://github.com/MiiBond)
* Ed Mackey, AGI [@emackey](https://github.com/emackey)
* Alex Wood, AGI [@abwood](https://twitter.com/abwood)
* Nicolas Savva, Autodesk, [@nicolassavva-autodesk](https://github.com/nicolassavva-autodesk)
* Tobias Haeussler, Dassault Systemes [@proog128](https://github.com/proog128)
* Bastian Sdorra, Dassault Systemes [@bsdorra](https://github.com/bsdorra)
* Don McCurdy, Individual Contributor [@donrmccurdy](https://github.com/donmccurdy)
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

This extension defines the anisotropic property of a material as observable with brushed metals for example.
An asymmetric specular lobe model is introduced to allow for such phenomena.
The visually distinct feature of that lobe is the elongated appearance of the specular reflection.

## Extending Materials

Sample values:

```json
{
    "materials": [
        {
            "extensions": {
                "KHR_materials_anisotropy": {
                    "anisotropyStrength": 0.6,
                    "anisotropyRotation": 1.57,
                    "anisotropyTexture": {
                        "index": 0
                    }
                }
            }
        }
    ]
}
```

|                         | Type     | Description               | Required           |
| ----------------------- | -------- | ------------------------- | ------------------ |
| **anisotropyStrength**  | `number` | The anisotropy strength. When anisotropyTexture is present, this value is multiplied by the blue channel. | No, default: `0.0` |
| **anisotropyRotation**  | `number` | The rotation of the anisotropy in tangent, bitangent space, measured in radians counter-clockwise from the tangent. When anisotropyTexture is present, anisotropyRotation provides additional rotation to the vectors in the texture. | No, default: `0.0` |
| **anisotropyTexture**   | [`textureInfo`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The anisotropy texture. Red and green channels represent the anisotropy direction in [-1, 1] tangent, bitangent space, to be rotated by anisotropyRotation. The blue channel contains strength as [0, 1] to be multiplied by anisotropyStrength. | No |

## Anisotropy

Two new material properties are introduced: a strength parameter and the direction in which the specular reflection elongates relative to the surface tangents.
The strength parameter is a dimensionless number in the range `[0, 1]` and increases the roughness along a chosen direction. The default direction aligns with the tangent to the mesh as described in the glTF 2.0 specification, [Meshes section](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes).

All meshes with materials that use anisotropy **SHOULD** supply `TANGENT` vectors as a mesh attribute.  If `TANGENT` vectors are not supplied for such a mesh, the mesh **MUST** supply a `normalTexture`, and tangents are computed according to rules in the [Meshes section](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes) of the glTF specification. Likewise when `TANGENT` vectors are not supplied for a mesh, the mesh **MUST NOT** supply different texture coordinates on the `normalTexture` and `anisotropyTexture`.

The `anisotropyTexture`, when supplied, encodes XY components of the direction vector in tangent space as red and green values, and strength as blue values, all stored with linear transfer function. After dequantization, red and green texel values **MUST** be mapped as follows: red [0.0 .. 1.0] to X [-1 .. 1], green [0.0 .. 1.0] to Y [-1 .. 1].  Blue does not require remapping.  When `anisotropyTexture` is not supplied, the default value is red 1.0 (X 1.0), green 0.5 (Y 0.0), and blue 1.0 (strength 1.0).  The direction of this XY vector specifies the per-texel direction of increased anisotropy roughness in tangent, bitangent space, prior to being rotated by `anisotropyRotation`.  The blue channel contains strength as [0.0 .. 1.0] to be multiplied by `anisotropyStrength` to determine the per-texel anisotropy strength.

> **Note:** The direction vector of the anisotropy is the direction in which highlights will be stretched. The direction of the micro-grooves in the material causing the anisotropy will run perpendicular. 

The direction of increased anisotropy roughness (as determined above, by default the tangent direction), has its "alpha roughness" (the square of the roughness) increased according to the following formula:

*directionAlphaRoughness* = mix( *materialAlphaRoughness*, 1.0, *strength*<sup>2</sup> )

In the above, *materialAlphaRoughness* is defined as:

*materialAlphaRoughness* = *materialRoughness*<sup>2</sup>

Finally, *mix* is defined as:

mix( *x*, *y*, *m* ) = *x* * ( 1.0 - *m* ) + ( *y* * *m* )

The roughness of the perpendicular direction (by default, the bitangent direction) is equal to the material's specified roughness.

These two different *alphaRoughness* values, call them *&alpha;<sub>t</sub>* and *&alpha;<sub>b</sub>* contribute to an extended BRDF distribution term from Burley, parameterized here in terms of the halfway vector, $\boldsymbol h$, between the view and light directions:

$$
D(\boldsymbol h) = \frac{1}{\pi \alpha_t \alpha_b\left( (\boldsymbol h \cdot \boldsymbol t)^2/\alpha_t^2 + (\boldsymbol h \cdot \boldsymbol b)^2/\alpha_b^2 + (\boldsymbol h \cdot \boldsymbol n)^2 \right)^2}
$$

From this distribution function a masking/shadowing function approximation can also be derived, similarly as for the single-$\alpha$ distribution function.

## Implementation

*This section is non-normative.*

In the following example, `u_AnisotropyStrength` is set to this extension's `anisotropyStrength`, and `u_AnisotropyRotation` is initialized as `[ cos(anisotropyRotation), sin(anisotropyRotation) ]`.

The default value of `anisotropyRotation` is zero, so when this parameter is not supplied by glTF, `u_AnisotropyRotation` will be `[ 1.0, 0.0 ]`.

### Individual lights

For a directional or point light where the light direction is a single vector, the following provides a sample implementation:

```glsl
uniform float u_AnisotropyStrength;
uniform vec2 u_AnisotropyRotation;

float anisotropy = u_AnisotropyStrength;
vec2 direction = u_AnisotropyRotation;

#if HAS_ANISOTROPY_MAP
vec3 anisotropyTex = texture(u_AnisotropyTextureSampler, uv).rgb;
direction = anisotropyTex.rg * 2.0 - vec2(1.0);
direction = mat2(u_AnisotropyRotation.x, u_AnisotropyRotation.y, -u_AnisotropyRotation.y, u_AnisotropyRotation.x) * normalize(direction);
anisotropy *= anisotropyTex.b;
#endif

vec3 anisotropicT = normalize(TBN * vec3(direction, 0.0));
vec3 anisotropicB = normalize(cross(normal_geometric, anisotropicT));

float TdotL = dot(anisotropicT, l);
float BdotL = dot(anisotropicB, l);
float TdotH = dot(anisotropicT, h);
float BdotH = dot(anisotropicB, h);

f_specular += intensity * NdotL * BRDF_specularAnisotropicGGX(f0, f90, alphaRoughness,
    VdotH, NdotL, NdotV, NdotH,
    BdotV, TdotV, TdotL, BdotL, TdotH, BdotH, anisotropy);
```

The anisotropic GGX can be approximated as follows:

```glsl
vec3 BRDF_specularAnisotropicGGX(vec3 f0, vec3 f90, float alphaRoughness,
    float VdotH, float NdotL, float NdotV, float NdotH, float BdotV, float TdotV,
    float TdotL, float BdotL, float TdotH, float BdotH, float anisotropy)
{
    float at = mix(alphaRoughness, 1.0, anisotropy * anisotropy);
    float ab = alphaRoughness;

    vec3 F = F_Schlick(f0, f90, VdotH);
    float V = V_GGX_anisotropic(NdotL, NdotV, BdotV, TdotV, TdotL, BdotL, at, ab);
    float D = D_GGX_anisotropic(NdotH, TdotH, BdotH, at, ab);

    return F * V * D;
}

float D_GGX_anisotropic(float NdotH, float TdotH, float BdotH, float at, float ab)
{
    float a2 = at * ab;
    vec3 f = vec3(ab * TdotH, at * BdotH, a2 * NdotH);
    float w2 = a2 / dot(f, f);
    return a2 * w2 * w2 / M_PI;
}

float V_GGX_anisotropic(float NdotL, float NdotV, float BdotV, float TdotV, float TdotL, float BdotL,
    float at, float ab)
{
    float GGXV = NdotL * length(vec3(at * TdotV, ab * BdotV, NdotV));
    float GGXL = NdotV * length(vec3(at * TdotL, ab * BdotL, NdotL));
    float v = 0.5 / (GGXV + GGXL);
    return clamp(v, 0.0, 1.0);
}
```

The parametrization of `at` and `ab`, denoting linear roughness values along both anisotropic directions respectively.

### Image-based lighting

For image-based lighting (IBL), generally some form of PMREM (prefiltered mipmapped radiance environment map) is used since calculating the contribution from each pixel individually is too slow for realtime graphics. In this case one must sample from this PMREM since a light vector is not available. 

Since the PMREM is sampled for a single roughness value and direction, one sample is not in general enough to get a very accurate approximation of the lighting. Implementors are encouraged to trade off speed and accuracy with their sampling strategies. 

The two $\alpha$ values are defined in such a way that the material's base roughness (also the minimum of the two directions) is a good choice for sampling the PMREM. The center of the reflection distribution is a good direction to start sampling, especially if using only a single sample. This can be approximated with the bent normal technique:

```glsl
vec3 bentNormal = cross(anisotropicB, viewDir);
bentNormal = normalize(cross(bentNormal, anisotropicB));
// This heuristic can probably be improved upon
float a = pow2(pow2(1.0 - anisotropy * (1.0 - roughness)));
bentNormal = normalize(mix(bentNormal, normal, a));
vec3 reflectVec = reflect(-viewDir, bentNormal);
// Mixing the reflection with the normal is more accurate both with and without anisotropy and keeps rough objects from gathering light from behind their tangent plane.
reflectVec = normalize(mix(reflectVec, bentNormal, roughness * roughness));
f_specular += SamplePMREM(envMap, reflectVec, roughness);
```

Further samples should be placed on both sides along the `anisotropicT` direction, spaced according to $\alpha_t$. This is more important when $\alpha_b$ is small, meaning the base material is shiny.

## Reference

1. [Google Filament - Anisotropic model](https://google.github.io/filament/Filament.md.html#materialsystem/anisotropicmodel)
2. [Google Filament Materials Guide - Anisotropic model](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/anisotropy)
3. [Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)
4. [Christopher Kulla and Alejandro Conty. 2017. Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf)
