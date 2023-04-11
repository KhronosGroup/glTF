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
| **anisotropyStrength**  | `number` | The anisotropy strength.  | No, default: `0.0` |
| **anisotropyRotation**  | `number` | The rotation of the anisotropy in tangent, bitangent space, measured in radians counter-clockwise from the tangent. When anisotropyTexture is present, anisotropyRotation provides additional rotation to the vectors in the texture. | No, default: `0.0` |
| **anisotropyTexture**   | [`textureInfo`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-textureinfo) | The anisotropy texture. Red and green channels represent the anisotropy direction in [-1, 1] tangent, bitangent space. The vector is rotated by anisotropyRotation, and multiplied by anisotropyStrength, to obtain the final anisotropy direction and strength. | No |

## Anisotropy

Two new material properties are introduced: a strength parameter and the direction in which the specular reflection elongates relative to the surface tangents.
The strength parameter is a dimensionless number in the range `[0, 1]` and increases the roughness along a chosen direction. The default direction aligns with the tangent to the mesh as described in the glTF 2.0 specification, [Meshes section](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes).

All meshes that use nonzero anisotropy strength **SHOULD** supply `TANGENT` vectors as a mesh attribute.  If `TANGENT` vectors are not supplied for such a mesh, `TEXCOORD_0` **MUST** be supplied instead, such that tangent vectors can be automatically calculated.

The `anisotropyTexture`, when supplied, specifies a 2D vector in the red and green channels.  The texture encodes XY components of the vector in tangent space as red and green values stored with linear transfer function. After dequantization, texel values **MUST** be mapped as follows: red [0.0 .. 1.0] to X [-1 .. 1], green [0.0 .. 1.0] to Y [-1 .. 1].  When `anisotropyTexture` is not supplied, the default value is red 1.0 (X 1.0) and green 0.5 (Y 0.0).  The direction of this vector specifies the per-texel direction of increased anisotropy roughness in tangent, bitangent space, prior to being rotated by `anisotropyRotation`.  The magnitude of this vector is multiplied by `anisotropyStrength` to determine the per-texel anisotropy strength.

The direction of increased anisotropy roughness (as determined above, by default the tangent direction), has its "alpha roughness" (the square of the roughness) altered according to the following formulas:

alpha_roughness = material_roughness<sup>2</sup>

direction_alpha_roughness = mix( alpha_roughness, 1.0, strength<sup>2</sup> )

mix( x, y, m ) = x * ( 1.0 - m ) + ( y * m )

The roughness of the perpendicular direction (by default, the bitangent direction) is equal to the material's specified roughness.

## Implementation

In the following example, `u_AnisotropyStrength` is set to this extension's `anisotropyStrength`, and `u_AnisotropyRotation` is initialized as `[ cos(anisotropyRotation), sin(anisotropyRotation) ]`.

The default value of `anisotropyRotation` is zero, so when this parameter is not supplied by glTF, `u_AnisotropyRotation` will be `[ 1.0, 0.0 ]`.

```glsl
uniform float u_AnisotropyStrength;
uniform vec2 u_AnisotropyRotation;

float anisotropy = u_AnisotropyStrength;
vec2 direction = u_AnisotropyRotation;

#if HAS_ANISOTROPY_MAP
direction = texture(u_AnisotropyTextureSampler, uv).rg * 2.0 - vec2(1.0);
anisotropy *= length(direction);
direction = mat2(u_AnisotropyRotation.x, u_AnisotropyRotation.y, -u_AnisotropyRotation.y, u_AnisotropyRotation.x) * normalize(direction);
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

The anisotropic GGX is defined as follows:

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

## Reference

1. [Google Filament - Anisotropic model](https://google.github.io/filament/Filament.md.html#materialsystem/anisotropicmodel)
2. [Google Filament Materials Guide - Anisotropic model](https://google.github.io/filament/Materials.md.html#materialmodels/litmodel/anisotropy)
3. [Blender Principled BSDF](https://docs.blender.org/manual/en/latest/render/shader_nodes/shader/principled.html)
4. [Christopher Kulla and Alejandro Conty. 2017. Revisiting Physically Based Shading at Imageworks](https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf)
