# KHR\_spz\_gaussian\_splats\_compression

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
- [Adding Gaussian Splats to Primitives](#adding-gaussian-splats-to-primitives)
  - [Extension Properties](#extension-properties)
  - [Accessors](#accessors)
- [Conformance](#conformance)
- [Implementation](#implementation)
- [Schema](#schema)
- [Known Implementations](#known-implementations)
- [Resources](#resources)

## Overview

This extension defines support for storing 3D Gaussian splats in glTF, bringing structure and conformity to the Gaussian Splatting space while utilizing glTF with SPZ compression for efficient streaming and storage.

SPZ is a compression format from Niantic Labs designed for Gaussian splats. Currently, it is open sourced under the MIT license. The SPZ format is primarily used in the Niantic Labs Scaniverse app. It was purpose built for Gaussian splats and offers a balance of high compression with minimal visual fidelity loss, and allows for storing some or all of the 3D Gaussian's spherical harmonics.

At rest, the Gaussian splats are stored within the SPZ compression format. Upon being decompressed, values are mapped to glTF attributes for rendering. The [conformance](#conformance) section defines what an implementation must do when encountering this extension, and how the extension interacts with the base specification.

## Adding Gaussian Splats to Primitives

If a primitive contains an `extension` property which defines `KHR_spz_gaussian_splats_compression` then support for SPZ compression is required. At this time, there is no requirement for a backup uncompressed buffer.

The extension must be listed in `extensionsUsed`

```json
  "extensionsUsed" : [
    "KHR_spz_gaussian_splats_compression"
  ]
```

It must also be listed in `extensionsRequired`

```json
  "extensionsRequired" : [
    "KHR_spz_gaussian_splats_compression"
  ]
```

Note that while `KHR_spz_gaussian_splats_compression` is required.

### Geometry Type

The `mode` of the `primitive` must be `POINTS`.

### Schema Example

Example SPZ extension shown below. This extension only affects any `primitive` nodes containting Gaussian splat data.

```json
  "meshes": [{
      "primitives": [{
          "attributes": {
            "_ROTATION": 3,
            "COLOR_0": 1,
            "_SCALE": 2,
            "POSITION": 0,
            "_SH_DEGREE_1_COEF_0": 4,
            "_SH_DEGREE_1_COEF_1": 5,
            "_SH_DEGREE_1_COEF_2": 6,
          },
          "material": 0,
          "mode": 0,
          "extensions": {
            "KHR_spz_gaussian_splats_compression": {
              "bufferView": 0,
              "numPoints": 590392,
              "shDegree": 0,
              "fractionalBits": 12,
              "flags": 0,
              "attributes": {
                "_ROTATION": 3,
                "COLOR_0": 1,
                "_SCALE": 2,
                "POSITION": 0,
                "_SH_DEGREE_1_COEF_0": 4,
                "_SH_DEGREE_1_COEF_1": 5,
                "_SH_DEGREE_1_COEF_2": 6,
              }
            }
          }
        }]
    }],
  "buffers": [{
      "uri": "0.spz",
      "byteLength": 9753142
    }],
  "bufferViews": [{
      "buffer": 0,
      "byteLength": 9753142
    }],
```

### Extension Properties

#### bufferView

This property points to the bufferView containing the Gaussian splat data compressed with SPZ.

#### attributes

This contains the attributes that will map into the compressed SPZ data indicated by `bufferView`. At minimum it will contain `POSITION`, `COLOR_0`, `_ROTATION`, and `_SCALE`. `_SH_DEGREE_ℓ_COEF_n` attributes hold the spherical harmonics data and are not required. If higher degrees are used then lower degrees are required implicitly.

| Splat Data | glTF Attribute | Accessor Type | Component Type | Required |
| --- | --- | --- | --- | --- |
| Position | POSITION | VEC3 | float | yes |
| Color (Spherical Harmonic degree 0 (Diffuse)) | COLOR_0 | VEC4 | unsigned byte or float normalized | yes |
| Rotation | _ROTATION | VEC4 | float | yes |
| Scale | _SCALE | VEC3 | float | yes |
| Spherical Harmonics degree 1 | _SH_DEGREE_1_COEF_n (n = 0 to 2) | VEC3 | float | no (yes if degree 2 or 3 are used) |
| Spherical Harmonics degree 2 | _SH_DEGREE_2_COEF_n (n = 0 to 4) | VEC3 | float | no (yes if degree 3 is used) |
| Spherical Harmonics degree 3 | _SH_DEGREE_3_COEF_n (n = 0 to 6) | VEC3 | float | no |

Each increasing degree of spherical harmonics requires more coeffecients. At the 1st degree, 3 sets of coeffcients are required, increasing to 5 sets for the 2nd degree, and increasing to 7 sets at the 3rd degree. With all 3 degrees, this results in 45 spherical harmonic coefficients stored in the `_SH_DEGREE_ℓ_COEF_n` attributes.

**TODO:** SPZ to attributes diagram

#### SPZ header properties embedded in the compressed data stream

The following properties are within the SPZ stream header. They are exposed here to be informative and facilitate decoder setup prior to any decompression occurring.

##### numPoints

The number of Gaussian splats in the compressed data.

##### fractionalBits

The number of fraction bits to use for `POSITION` quantization.

##### shDegree

The degree of spherical harmonics contained in the compressed data. This can range from 0 to 3.

##### flags

A set of bit flags reserved for future use. Currently it is only used for an anti-aliasing flag.

### Accessors

Required `accessors` for `POSITION`, `COLOR_0`, `_ROTATION`, and `_SCALE`:

```json
  "accessors": [{
      "componentType": 5126,
      "count": 590392,
      "type": "VEC3"
    }, {
      "componentType": 5121,
      "count": 590392,
      "type": "VEC3"
    }, {
      "componentType": 5126,
      "count": 590392,
      "type": "VEC3"
    }, {
      "componentType": 5126,
      "count": 590392,
      "type": "VEC4"
    }],
```

Spherical harmonics `accessors` all follow the pattern:

```json
  "accessors": [{
    "componentType": 5126,
    "count": 590392,
    "type": "SCALAR"
  }]
```

At minimum accessors will be defined for `POSITION`, `COLOR_0`, `_ROTATION`, and `_SCALE`. Each must have `componentType`, `count`, and `type` defined.

Accessor `type` is defined for the resulting type after decompression and dequantization has occurred.

The accessor `count` must match `numPoints` in `KHR_spz_gaussian_splats_compression`.

## Conformance

The recommended process for handling SPZ compression is as follows:

- If the loader does not support `KHR_spz_gaussian_splats_compression`, it must fail.
- If the load does support `KHR_spz_gaussian_splats_compression` then:
  - The loader must process `KHR_spz_gaussian_splats_compression` data first. The loader must get the data from `KHR_spz_gaussian_splats_compression`'s `bufferView` property.
  - The loader then must process `attributes` of the `primitive`. When processing the loader must ignore any `bufferView` and `byteOffset` in the `accessor` and instead use values derived from the decompressed data streams. This data can be used to populate the `accessors` or render directly.
  - Any attributes not listed in `KHR_spz_gaussian_splats_compression`'s `attributes` but are listed in the `primitive` must then be processed by the loader.

## Implementation

*This section is non-normative.*

Rendering is broadly two phases: Pre-rasterization sorting and rasterization.

### Splat Sorting

Given that splatting uses many layered Gaussians blended to create complex effects, their ordering is view dependent and must be sorted based on their distance from the current camera position. The details are largely dependent on the platform targeted.

In the seminal paper, they took a hardware accelerated approach using CUDA. The scene is broken down into tiles with each tile processed in parallel. The splats within each tile are sorted via a GPU accelerated Radix sort. The details are beyond the scope of this document, but it can be seen here: 
[https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/rasterizer_impl.cu](https://github.com/graphdeco-inria/diff-gaussian-rasterization/blob/59f5f77e3ddbac3ed9db93ec2cfe99ed6c5d121d/cuda_rasterizer/rasterizer_impl.cu)

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

`_SCALE` and `_ROTATION` are used to compute the 3D covariance ahead of time. This part of computation is not view-dependent. It's computed as it is above in the vertex shader code. Once computed, we take the 6 unique values of our 3D covariance matrix and convert them to half-float for compactness. Each Gaussian splat occupies 2 pixels of the texture.

[See packing implementation here](https://github.com/CesiumGS/cesium-wasm-utils/blob/main/wasm-splats/src/texture_gen.rs)

Accessed via `usampler2D`:

```glsl
  highp usampler2D u_gsplatAttributeTexture;
```

#### Sorting and Indexes

With our Gaussian splat attributes packed into a texture our sorting only has to act upon a separate `_INDEX` attribute we create at runtime. Gaussian splats are sorted as above, but instead of sorting each vertex buffer we only sort the index values. When the glTF is loaded, Gaussian splats can be indexed in the order read.

#### Extracting Data in the Vertex Shader

Given a texture with a width of 2048 pixels, we can access it thusly:

```glsl
  uint texIdx = uint(a_splatIndex); //_INDEX
  ivec2 posCoord = ivec2((texIdx & 0x3ffu) << 1, texIdx >> 10); //wrap every 2048 pixels
```

Extracting position data:

```glsl
  vec4 splatPosition = vec4( uintBitsToFloat(uvec4(texelFetch(u_splatAttributeTexture, posCoord, 0))) );
```

Covariance and color data are extracted together:

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

## Schema

[SPZ Compression Schema](./schema/mesh.primitive.KHR_spz_gaussian_splats_compression.schema.json)

## Known Implementations

This is currently implemented within  [3D Tiles and CesiumJS as an experimental feature](https://github.com/CesiumGS/cesium/tree/splat-spz-concept).

## Resources

[https://github.com/nianticlabs/spz](https://github.com/nianticlabs/spz)

[https://github.com/drumath2237/spz-loader/tree/main](https://github.com/drumath2237/spz-loader/tree/main)
