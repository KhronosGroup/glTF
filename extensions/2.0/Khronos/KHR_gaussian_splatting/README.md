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

## Table of Contents

- [Overview](#overview)
- [Adding Gaussian Splats](#adding-gaussian-splats-to-primitives)
  - [Splat Data Mapping](#splat-data-mapping)
  - [Extension Attributes](#extension-attributes)
  - [Transforming Data](#transforming-gaussian-splat-data-for-gltf)
- [Limitations](#limitations)
- [Implementation](#implementation)
- [Meshopt Compression](#meshopt-compression)
- [Schema](#schema)
- [Known Implementations](#known-implementations)
- [Resources](#resources)

## Overview

Currently, PLY files serve as the de facto standard through their sheer simplicity and ubiquity. This extension aims to bring structure and conformity to the Gaussian Splat space while utilizing glTF to its fullest extent. Gaussian splats are essentially a superset of a traditional point cloud, so we approached the extension with this mindset. If the point primitive contains the extension the renderer can know to render the point primitive as Gaussian splats instead of a points. Gaussian splats are defined by a position, opacity, rotation, scale, and spherical harmonics (both diffuse and specular) . We store these values as attributes on primitives.

This approach allows for an easy fallback in the event the glTF is loaded within a renderer that doesn't support Gaussian splats. In this scenario, the glTF file will render as a sparse point cloud to the user. It also allows for easy integration with existing compression like meshopt.

## Adding Gaussian Splats to Primitives

As Gaussian splats are defined by a position color, rotation and scale, we both map to existing attributes and define new ones. These are attached to a primitive  by defining the `extensions.KHR_gaussian_splatting` property on that mesh.

### Splat Data Mapping

To define a Gaussian splat, it must contain the following attributes:

| Splat Data | glTF Attribute | Accessor Type | Component Type |
| --- | --- | --- | --- |
| Position | POSITION | VEC3 | float |
| Color (Spherical Harmonic 0 (Diffuse) and opacity) | COLOR_0 | VEC4 | unsigned byte normalized |
| Rotation | _ROTATION | VEC4 | float |
| Scale | _SCALE | VEC3 | float |

Standard PLY splat formats have opacity and color separate, however they are combined into the COLOR_0 attribute here.

This implementation only contains the channel 0 spherical harmonic for diffuse color. Spherical Harmonic channels 1 through 15, which map the splat specular, are currently unused by the extension.

Utilizing the standard position and color attributes allows us to easily fall back in situations where `extensions.KHR_gaussian_splatting` isn't available. Or simply for alternative rendering.

### Extension attributes

`extensions.KHR_gaussian_splatting` may contain the following values:

| Attributes | Type | Description | Required |
| --- | --- | --- | --- |
| quantizedPositionScale | number | Scale value for dequantizing POSITION attribute values | No, default: `1.0` |

```json
{
    "extensions": {
        "KHR_gaussian_splatting": {
            "quantizedPositionScale": 13.228187255859375
        }
    }
}
```

### Transforming Gaussian Splat Data for glTF

The data output from the Gaussian splat training process that is stored in the typical PLY must be transformed before storing in glTF. These transformations make the data suitable for direct usage in glTF.

#### Diffuse Color

Each color channel `red`, `green`, and `blue` must each be multiplied by the zeroth-order spherical harmonic constant `0.28209479177387814`

Those can then be converted to an `unsigned byte` color channel.

#### Opacity

Opacity must be activated through the logistic function $\sigma(a) = \frac{1}{1 + e^{-a}}$ which constrains it to the range `[0 - 1)`

It can then be converted to an `unsigned byte` color channel.

#### Scale

'Activated' via exponentiation similar to `opacity`. $\exp(x)$

#### Rotation

Normalized to a unit quaternion

$$\hat{q} = \frac{q}{\|q\|} = \frac{q}{\sqrt{q_w^2 + q_x^2 + q_y^2 + q_z^2}}$$


#### Sample

Basic example shown below. This sample shows adding Guassian splats to the first primitive of a mesh. No
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

## Limitations

This extension currently lacks encoding of all of the spherical harmonics used for specular. While these are a strong feature of gaussian splats as they allow the scene to render dynamically based on viewing position, they are prohibitively large in storage size and impose a larger computational cost. This extension is instead focused on core implementation details before tackling the question of how best to store a compressed version of the spherical harmonics used by the splat. With compression comes accuracy loss, and while suitable for general cases, we did not want to impose a lossy format on potential users. Lossy data should be opt-in where possible.

The authors of this extension intend to have companion extensions for handling lossy or lossless handling of the spherical harmonics to meet the needs of all users.

## Implementation

_This section is non-normative_

Rendering is broadly two phases: Pre-rasterization sorting and rasterization.

### Splat Sorting

Given that splatting uses many layered Gaussians blended to create complex effects, their ordering is view dependent and must be sorted based on their distance from the current camera position. The details are largely dependent on the platform targeted.

In the seminal paper, they took a hardware accelerated approach using CUDA. The scene is broken down into tiles with each tile processed in parallel. The splats within each tile are sorted via a GPU accelerated Radix sort. The details are beyond the scope of this document, but it can be seen here: 
https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/rasterizer_impl.cu

Our approach differs in that we are currently operating in the browser with WebGL, so we don't have that level of hardware access.

Regardless of how your data is stored and structured, sorting visible Gaussians is a similar process.

First, we obtain our model view matrix by multiplying the model matrix of the asset we are viewing with the camera view matrix

```javascript
    const modelViewMatrix = new Matrix4();
    const modelMatrix = renderResources.model.modelMatrix;
    Matrix4.multiply(cam.viewMatrix, modelMatrix, modelViewMatrix);
```

Second, we need a method to calculate Z-depth of each splat (median point, this does not factor in volume) for our depth sort.
We accomplish this by taking the dot product of the splat position (x, y, z) with the view z-direction.

```javascript
    const zDepthCalc = (index) => 
        splatPositions[index * 3] * modelViewMatrix[2] +
        splatPositions[index * 3 + 1] * modelViewMatrix[6] + 
        splatPositions[index * 3 + 2] * modelViewMatrix[10]
```

No particular sorting method is required, but count and Radix sorts are generally performant. Between the two, we have found Radix to be consistently faster (10-15%) while using less memory.

### Rasterizing

In the vertex shader, we first must compute covariance in 3D and then 2D space. In optimizing implementations, 3D covariance can be computed ahead of time.

Our 3D covariance matrix can be represented as:
$$\Sigma = RSS^TR^T$$

Where `S` is our scaling matrix and `R` is our rotation matrix

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
```

3D Gaussians are then projected into 2D space for rendering. Algorithm Zwicker et al. [2001a]

$$\Sigma' = JW\Sigma W^TJ^T$$

- `W` is the view transformation
- `J` is the Jacobian of the affine approximation of the projective transformation
- $\Sigma$ is the 3D covariance matrix derived above (as `Vrk` below)

```glsl
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
```

The conic is the inverse of the covariance matrix:

```glsl
vec3 calculateConic(vec3 covariance2D)
{
    float det = covariance2D.x * covariance2D.z - covariance2D.y * covariance2D.y;
    return vec3(covariance2D.z, -covariance2D.y, covariance2D.x) * (1. / det); 
}
```

The Guassian is finally rendered using the conic matrix applying its alpha derived from the Gaussian opacity multiplied by its exponential falloff.

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

## Meshopt Compression

_This section is non-normative_

This extension is fully compatible with [meshopt compression](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/EXT_meshopt_compression). All attributes can be quantized and compressed. We are able to use the standard compression methods and filters.

It is recommended to do some sort of spatial sorting prior to compression like Morton order. If using [meshoptimizer](https://github.com/zeux/meshoptimizer), calling `meshopt_spatialSortRemap()` provides this for you.

### Position Attribute

It is recommended to quantize to `half float` utilizing `KHR_mesh_quantization` with an `unsigned int` accessor type. If using scaled quantization values, you may set the `quantizedPositionScale` value in `extensions.KHR_gaussian_splatting`.

| meshopt configuration | value |
| --- | --- |
| Mode | Attributes |
| Filter | None |

### Color Attribute

RGBA data is compressed as is.

| meshopt configuration | value |
| --- | --- |
| Mode | Attributes |
| Filter | None |

### Scale Attribute

Here we use the `exponential` filter to encode the data prior to compression. Any `exponential mode` can be used in conjunction with the filter.

| meshopt configuration | value |
| --- | --- |
| Mode | Attributes |
| Filter | Exponential |

### Rotation Attribute

Here we take advantage of the `quaternion` filter. 

| meshopt configuration | value |
| --- | --- |
| Mode | Attributes |
| Filter | Quaternion |

### Compression Results

- Full bitrate: 16 bits per component for position, scale, and rotation
- Minimum bitrate: 10 bits per position component, 8 bits per scale and rotation components

| Source | Splats | PLY size | glTF | meshopt glTF(full bitrate) | meshopt glTF(min bitrate) |
| --- | --- | --- | --- | --- | --- |
| Los Reyes | 1214130 | 301.1MB | 77.7MB | 35.0MB | 24.8MB |

[Cathedral in Ghent sample file](https://gist.github.com/keyboardspecialist/c0d17f1f3e06f6c581a24ffe099e4a0d)

Compressed with meshopt with minimum bitrate defined above.

## Schema

[Gaussian Splatting JSON Schema](./schema/primitive.KHR_gaussian_splatting.schema.json)

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://github.com/CesiumGS/cesium/tree/splat-shader).

## Resources

https://github.com/mkkellogg/GaussianSplats3D/issues/47#issuecomment-1801360116
https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/