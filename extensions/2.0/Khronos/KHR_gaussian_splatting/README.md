# KHR\_gaussian\_splatting

## Contributors

- Jason Sobotka, Cesium
- Renaud Keriven, Cesium
- Adam Morris, Cesium
- Sean Lilley, Cesium
- Projit Bandyopadhyay, Niantic Spatial
- Daniel Knoblauch, Niantic Spatial
- Ronald Poirrier, Esri
- Jean-Philippe Pons, Esri

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Table of Contents

- [Overview](#overview)
- [Adding 3D Gaussian Splats to Primitives](#adding-3d-gaussian-splats-to-primitives)
    - [Geometry Type](#geometry-type)
    - [Schema Example](#schema-example)
    - [Extension Properties](#extension-properties)
        - [Attributes](#attributes)
    - [Accessors](#accessors)
- [Implementation](#implementation)
- [Known Implementations](#known-implementations)
- [Resources](#resources)

## Overview

This extension defines support for storing 3D Gaussian splats in glTF, bringing structure and conformity to the 3D Gaussian splatting space. 3D Gaussian splatting uses fields of Gaussians that can be treated as a point cloud for the purposes of storage. 3D Gaussian splats are defined by their position, rotation, scale, and spherical harmonics which provide both diffuse and specular color. These values are stored as values on a point primitive. Since we treat the 3D Gaussian splats as points primitives, a graceful fallback to treating the data as a sparse point cloud is possible.

## Adding 3D Gaussian Splats to Primitives

When a primitive contains an `extension` property defining `KHR_gaussian_splatting`, this indicates to the client that the primitive should be treated as a 3D Gaussian splatting field.

The extension must be listed in `extensionsUsed`:

```json
  "extensionsUsed" : [
    "KHR_gaussian_splatting"
  ]
```

Other extensions that depend on this extension such as 3D Gaussian splatting compression extensions may require that this extension be included in `extensionsRequired`.

## Geometry Type

The `mode` of the `primitive` must be `POINTS`.

## Schema Example

Example shown below including optional properties. This extension only affects any `primitive` nodes containting 3D Gaussian splat data.

```json
"meshes": [{
    "primitives": [{
        "attributes": {
            "POSITION": 0,
            "COLOR_0": 1,
            "KHR_gaussian_splatting:SCALE": 2,
            "KHR_gaussian_splatting:ROTATION": 3,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_0": 4,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_1": 5,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_2": 6
        },
        "mode": 0,
        "indices": 7,
        "extensions": {
            "KHR_gaussian_splatting": {
                "kernel": "ellipse",
                "sortingMethod": "cameraDistance",
                "projection": "perspective"
            }
        }
    }]
}]
```

## Extension Properties

### Kernel

Gaussian splats can have a variety of shapes and this has the potential to change over time. The `kernel` property is an optional property that provides an indication to the renderer the properties of the kernel used. Renderers are free to ignore any values they do not recognize. In the event that `kernel` is either not provided or not recognized, the default value of `ellipse` should be assumed. Additional kernel types can be added over time by supplying an extension that defines an alternative shape definition.

| Kernel Type | Description |
| --- | --- |
| ellipse | A 2D ellipse kernel used to project an ellipsoid shape in 3D space. |

```json
"meshes": [{
    "primitives": [{
        // snip...
        "extensions": {
            "KHR_gaussian_splatting": {
                "kernel": "ellipse"
            }
        }
    }]
}]
```

#### Ellipse Kernel

A 2D `ellipse` kernel type is often used to represent 3D Gaussian splats in an ellipsoid shape. This simple type contains no parameters. This is the shape used by the reference renderer implementations for 3D Gaussian splatting.

#### Adding additional Kernel Types

*This section is non-normative.*

In order to add additional kernel types, a new extension should be defined that extends `KHR_gaussian_splatting`. This new extension should define the new kernel type and any parameters it may require. A renderer that recognizes the new kernel type can then use the parameters defined in the new extension to render the splats appropriately. Renderers that do not recognize the new kernel type should fall back to the default `ellipse` type.

For example, a new extension `EXT_gaussian_splatting_kernel_customShape` could be defined that adds a new kernel type `customShape` with additional parameters.

```json
"meshes": [{
    "primitives": [{
        // snip... 
        "extensions": {
            "KHR_gaussian_splatting": {
                "kernel": "customShape",
                "extensions": {
                    "EXT_gaussian_splatting_kernel_customShape": {
                        "customParameter1": 1.0,
                        "customParameter2": [0.0, 1.0, 0.0]
                    }
                }
            },
        }
    }]
}]
```

### Projection

The `projection` property is an optional hint that specifies how the Gaussians should be projected onto the kernel shape. This is typically provided by the training process for the splats. This is a freeform string field to allow additional projection methods to be specified as they become available. The default value is `perspective`.

Renderers are free to ignore any values they do not recognize and fallback to the default, but are encouraged to follow the non-normative list below.

#### Known Projection Methods

*This section is non-normative and not comprehensive. It may change over time.*

| Projection Method | Description |
| --- | --- |
| perspective | (Default) The typical 3D perspective projection based on scene depth. |
| orthographic | A orthogonal projection of splats into a scene to preserve shape and scale and reduce distortion. |

### Sorting Method

The `sortingMethod` property is an optional hint that specifies how the Gaussian particles should be sorted during the rendering process. This typically is provided by the training process for the splats. This is a freeform string field to allow new sorting methods to be specified as they become available. The default value is `cameraDistance`.

Renderers are free to ignore any values they do not recognize, but are encouraged to follow the non-normative list below.

#### Known Sorting Methods

*This section is non-normative and not comprehensive. It may change over time.*

| Sorting Method | Description |
| --- | --- |
| cameraDistance | (Default) Sort splats based on the distance from the camera to the splat position. |
| zDepth | Sort splats based on their projected z-depth in the camera projection. |

## Attributes

Each 3D Gaussian splat has the following attributes. At minimum the attributes must contain `POSITION`, `COLOR_0`, `KHR_gaussian_splatting:ROTATION`, and `KHR_gaussian_splatting:SCALE`. `KHR_gaussian_splatting:SH_DEGREE_ℓ_COEF_n` attributes hold the spherical harmonics data and are not required. `POSITION` and `COLOR_0` are defined by the base glTF specification. If higher degrees of Spherical Harmonics are used then lower degrees are required implicitly.

| Splat Data | glTF Attribute | Accessor Type | Component Type | Required | Notes |
| --- | --- | --- | --- | --- | --- |
| Rotation | KHR_gaussian_splatting:ROTATION | VEC4 | _float_ <br/>_signed byte_ normalized <br/>_unsigned byte_ normalized <br/>_signed short_ normalized <br/>_unsigned short_ normalized | yes | Rotation is a quaternion with `w` as the scalar. (xyzw) |
| Scale | KHR_gaussian_splatting:SCALE | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_unsigned byte_ normalized <br/>_signed short_ normalized <br/>_unsigned short_ normalized | yes | |
| Spherical Harmonics degree 1 | KHR_gaussian_splatting:SH_DEGREE_1_COEF_n (n = 0 to 2) | VEC3 | float | no (yes if degree 2 or 3 are used) | |
| Spherical Harmonics degree 2 | KHR_gaussian_splatting:SH_DEGREE_2_COEF_n (n = 0 to 4) | VEC3 | float | no (yes if degree 3 is used) | |
| Spherical Harmonics degree 3 | KHR_gaussian_splatting:SH_DEGREE_3_COEF_n (n = 0 to 6) | VEC3 | float | no | |

The `KHR_gaussian_splatting:ROTATION` and `KHR_gaussian_splatting:SCALE` attributes support quantized storage using normalized `byte` or `short` component types to reduce file size. If quantization is not needed, content creators should use the `float` component type for maximum precision.

The value of `COLOR_0` is derived by multiplying the 3 diffuse color components of the 3D Gaussian splat with the constant zeroth-order Spherical Harmonic (ℓ = 0) for the RGB channels. The alpha channel should contain the opacity of the splat.

Each increasing degree of spherical harmonics requires more coeffecients. At the 1st degree, 3 sets of coeffcients are required, increasing to 5 sets for the 2nd degree, and increasing to 7 sets at the 3rd degree. With all 3 degrees, this results in 45 spherical harmonic coefficients stored in the `_SH_DEGREE_ℓ_COEF_n` attributes.

## Implementation

*This section is non-normative.*

Rendering is broadly two phases: Pre-rasterization sorting and rasterization.

### Splat Sorting

Given that splatting uses many layered Gaussians blended to create complex effects, splat ordering is view dependent and must be sorted based on the splat's distance from the current camera position. The details are largely dependent on the platform targeted.

In the seminal paper, the authors took a hardware accelerated approach using CUDA. The scene is broken down into tiles with each tile processed in parallel. The splats within each tile are sorted via a GPU accelerated Radix sort. The details are beyond the scope of this document, but it can be found on [their GitHub repository](https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/rasterizer_impl.cu). 

The approach outlined here differs in that it operates within the browser with WebGL, so direct GPU access is unavailable.

Regardless of how the data is stored and structured, sorting visible Gaussians is a similar process whether using the CPU or GPU.

First, obtain the model view matrix by multiplying the model matrix of the asset being viewed with the camera view matrix:

```javascript
    const modelViewMatrix = new Matrix4();
    const modelMatrix = renderResources.model.modelMatrix;
    Matrix4.multiply(cam.viewMatrix, modelMatrix, modelViewMatrix);
```

Second, calculate Z-depth of each splat (median point, this does not factor in volume) for our depth sort.
This can be accomplished by taking the dot product of the splat position (x, y, z) with the view z-direction.

```javascript
    const zDepthCalc = (index) => 
        splatPositions[index * 3] * modelViewMatrix[2] +
        splatPositions[index * 3 + 1] * modelViewMatrix[6] + 
        splatPositions[index * 3 + 2] * modelViewMatrix[10]
```

No particular sorting method is required, but count and Radix sorts are generally performant. Between the two, the authors have found Radix to be consistently faster (10-15%) while using less memory.

### Rasterizing

In the vertex shader, first compute the covariance in 3D and then 2D space. In optimizing implementations, 3D covariance can be computed ahead of time.

The 3D covariance matrix can be represented as:
$$\Sigma = RSS^TR^T$$

Where `S` is the scaling matrix and `R` is the rotation matrix.

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

The Gaussian is finally rendered using the conic matrix applying its alpha derived from the Gaussian opacity multiplied by its exponential falloff.

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

### Rendering from a Texture

Instead of rendering directly from attribute vertex buffers, Gaussian splats can be packed into a texture. This approach offers a few benefits: single source of data on the gpu, smaller size, pre-computed 3D covariance, and most importantly instead of sorting all vertex buffers we only have to update a single index buffer.

The texture format is `RGBA32UI`.

Gaussian splats are packed into 32 bytes with the following format:

| Data | Type | Size (bytes) | Byte Offset |
| --- | --- | --- | --- |
| POSITION | float | 12 | 0 |
| (UNUSED) | none | 4 | 12 |
| 3D Covariance | half float | 12 | 16 |
| COLOR_0 (RGBA) | unsigned byte | 4 | 28 |

`_SCALE` and `_ROTATION` are used to compute the 3D covariance ahead of time. This part of computation is not view-dependent. It's computed as it is above in the vertex shader code. Once computed, take the 6 unique values of the 3D covariance matrix and convert them to half-float for compactness. Each Gaussian splat occupies 2 pixels of the texture.

[See packing implementation here](https://github.com/CesiumGS/cesium-wasm-utils/blob/main/wasm-splats/src/texture_gen.rs)

Accessed via `usampler2D`:

```glsl
  highp usampler2D u_gsplatAttributeTexture;
```

#### Sorting and Indexes

With the Gaussian splat attributes packed into a texture the sorting only has to act upon the index of the splat at runtime. Gaussian splats are sorted as above, but instead of sorting each vertex buffer you can only sort the index values. When the glTF is loaded, Gaussian splats can be indexed in the order read.

#### Extracting Data in the Vertex Shader

Given a texture with a width of 2048 pixels, access it:

```glsl
  uint texIdx = uint(a_splatIndex); //_INDEX
  ivec2 posCoord = ivec2((texIdx & 0x3ffu) << 1, texIdx >> 10); //wrap every 2048 pixels
```

Extract the position data:

```glsl
  vec4 splatPosition = vec4( uintBitsToFloat(uvec4(texelFetch(u_splatAttributeTexture, posCoord, 0))) );
```

Then covariance and color data are extracted together:

```glsl
  uvec4 covariance = uvec4(texelFetch(u_splatAttributeTexture, covCoord, 0));

  //reconstruct matrix
  vec2 u1 = unpackHalf2x16(covariance.x) ;
  vec2 u2 = unpackHalf2x16(covariance.y);
  vec2 u3 = unpackHalf2x16(covariance.z);
  mat3 Vrk = mat3(u1.x, u1.y, u2.x, u1.y, u2.y, u3.x, u2.x, u3.x, u3.y);

  //reconstruct color
  v_splatColor = vec4(covariance.w & 0xffu, (covariance.w >> 8) & 0xffu, (covariance.w >> 16) & 0xffu, (covariance.w >> 24) & 0xffu) / 255.0;
```

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

// TODO
