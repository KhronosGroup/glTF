<!--
Copyright 2015-2024 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# KHR_gaussian_splatting

## Contributors

- Jason Sobotka, Cesium
- Adam Morris, Cesium
- Sean Lilley, Cesium

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

Currently, PLY files serve as the de facto standard through their sheer simplicity and ubiquity. This extension aims to bring structure and conformity to the Gaussian Splat space while utilizing glTF to its fullest extent. Gaussian Splats are essentially a superset of a traditional point cloud, so we approached the extension with this mindset. The **position**, **rotation**, **scale**, and **diffuse color** properties are stored as standard attributes on a point primitive. If the point primitive contains the extension the renderer can know to render the point primitive as Gaussian Splats instead of a points.

This approach allows for an easy fallback in the event the glTF is loaded within a renderer that doesn't support Gaussian Splats. In this scenario, the glTF file will render as a sparse point cloud to the user. It also allows for easy integration with existing compression like meshopt.

### Splat Data Mapping

| Splat Data | glTF Attribute |
| --- | --- |
| Position | POSITION |
| Color (Diffuse, Spherical Harmonic 0) | COLOR_0 RGB channels |
| Opacity (Spherical Harmonic 0 Alpha) | COLOR_0 A channel |
| Rotation | _ROTATION |
| Scale | _SCALE |

Spherical Harmonic channels 1 through 15, which map the splat specular, are currently unused by the extension.

### Extension attributes

| Attributes | Type | Description | Required
| --- | --- | --- | --- |
| quantizedPositionScale | number | Scale value for dequantizing POSITION attribute values | No, default: `1.0` |


## Extending glTF node

Sample:

```json
{
    "accessors": [
        {
            "type": "VEC3",
            "componentType": 5126
        },
        {
            "type": "VEC4",
            "componentType": 5121,
            "normalized": true
        },
        {
            "type": "VEC4", 
            "componentType": 5126
        },
        {
            "type": "VEC3",
            "componentType": 5126
        }
    ],
    "meshes": [
        {
            "primitives": [
                {
                    "mode": 0,
                    "attributes": {
                        "POSITION": 0,
                        "COLOR_0": 1,
                        "_ROTATION": 2,
                        "_SCALE": 3
                    },
                    "extensions": {
                        "KHR_gaussian_splatting": {
                            "quantizedPositionScale": 1.0
                        }
                    }
                }
            ]
        }
    ]
}
```

## Implementation

_This section is non-normative_

In this example, we follow a reference implementation for rendering Gaussian Splats.

In the vertex shader, we first must compute covariance in 3D and then 2D space. In optimizing implementations, 3D covariance can be computed ahead of time. 

```glsl
//https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/forward.cu#L118
void calculateCovariance3D(vec4 rotation, vec3 scale, out float[6] covariance3D)
{
    mat3 S = mat3(
        scale[0], 0, 0,
        0, scale[1], 0,
        0, 0, scale[2]
    );

    float r = rot.w;
    float x = rot.x;
    float y = rot.y;
    float z = rot.z;

    mat3 R = mat3(
        1. - 2. * (y * y + z * z), 2. * (x * y - r * z), 2. * (x * z + r * y),
        2. * (x * y + r * z), 1. - 2. * (x * x + z * z), 2. * (y * z - r * x),
        2. * (x * z - r * y), 2. * (y * z + r * x), 1. - 2. * (x * x + y * y)
    );

    mat3 M = S * R;
    mat3 Sigma = transpose(M) * M;

    covariance3D = float[6](
        Sigma[0][0], Sigma[0][1], Sigma[0][2],
        Sigma[1][1], Sigma[1][2], Sigma[2][2]
    );
}

//https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/forward.cu#L74
vec3 calculateCovariance2D(vec3 worldPosition, float cameraFocal_X, float cameraFocal_Y, float tan_fovX, float tan_fovY, float[6] covariance3D, mat4 viewMatrix)
{
    vec4 t = viewmatrix * vec4(worldPos, 1.0);

    float limx = 1.3 * tan_fovx;
    float limy = 1.3 * tan_fovy;
    float txtz = t.x / t.z;
    float tytz = t.y / t.z;
    t.x = min(limx, max(-limx, txtz)) * t.z;
    t.y = min(limy, max(-limy, tytz)) * t.z;

    mat3 J = mat3(
        focal_x / t.z, 0, -(focal_x * t.x) / (t.z * t.z),
        0, focal_y / t.z, -(focal_y * t.y) / (t.z * t.z),
        0, 0, 0
    );

    mat3 W =  mat3(
        viewmatrix[0][0], viewmatrix[1][0], viewmatrix[2][0],
        viewmatrix[0][1], viewmatrix[1][1], viewmatrix[2][1],
        viewmatrix[0][2], viewmatrix[1][2], viewmatrix[2][2]
    );
    mat3 T = W * J;
    mat3 Vrk = mat3(
        covariance3D[0], covariance3D[1], covariance3D[2],
        covariance3D[1], covariance3D[3], covariance3D[4],
        covariance3D[2], covariance3D[4], covariance3D[5]
    );

    mat3 cov = transpose(T) * transpose(Vrk) * T;

    cov[0][0] += .3;
    cov[1][1] += .3;
    return vec3(cov[0][0], cov[0][1], cov[1][1]);
}

vec3 calculateConic(vec3 covariance2D)
{
    float det = covariance2D.x * covariance2D.z - covariance2D.y * covariance2D.y;
    return vec3(covariance2D.z, -covariance2D.y, covariance2D.x) * (1. / det); 
}
```

```glsl
//https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/forward.cu#L330

in vec2 vertexPosition;
in vec2 screenPosition;
in vec3 conic;
in vec4 color;

out vec4 splatColor;

vec2 d = screenPosition - vertexPosition;
float power = -0.5 * (conic.x * d.x * d.x + conic.z * d.y * d.y) - conic.y * d.x * d.y);

if(power > 0.) 
    discard;

float alpha = min(.99f, color.a * exp(power));

if(alpha < 1./255.)
    discard;

splatColor = vec4(color * alpha, alpha);
```


## Schema

[Gaussian Splatting JSON Schema](./schema/primitive.KHR_gaussian_splatting.schema.json)



## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://github.com/CesiumGS/cesium/tree/splat-shader).

## Resources

https://github.com/mkkellogg/GaussianSplats3D/issues/47#issuecomment-1801360116
https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/