<!--
Copyright 2015-2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

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
- Alexey Knyazev, Khronos
- Marco Hutter, Independent
- Arseny Kapoulkine, Independent
- Nathan Morrical, Nvidia
- Norbert Nopper, Huawei
- Zehui Lin, Huawei
- Chenxi Tu, Huawei
- Michael Nikelsky, Autodesk

## Status

Review Draft

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
- [Known Implementations](#known-implementations)
- [Resources](#resources)

## Overview

This extension defines basic support for storing 3D Gaussian splats in glTF, bringing structure and conformity to the 3D Gaussian splatting space. 3D Gaussian splatting uses fields of Gaussians that can be treated as a point cloud for the purposes of storage. 3D Gaussian splats are defined by their position, rotation, scale, and spherical harmonics which provide both diffuse and specular color. These values are stored as values on a point primitive. Since we treat the 3D Gaussian splats as points primitives, a graceful fallback to treating the data as a sparse point cloud is possible.

A key objective of this extension is to establish a solid foundation for integrating 3D Gaussian splatting into glTF, while enabling future enhancements and innovation. To achieve this, the extension is intentionally designed for to be extended itself, allowing extensions to introduce new kernel types, color spaces, projection methods, and sorting methods over time as 3D Gaussian splatting techniques evolve and become standards within the glTF ecosystem.

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

## Mathematics of rendering using the default Ellipse Kernel

To render a field of 3D Gaussian splats, the renderer must reconstruct each Gaussian splat using the same forward pass algorithm used during training. This involves using the position, scale, rotation, opacity, and spherical harmonics attributes to reconstruct the Gaussian splat in 3D space.

There are three key stages to reconstructing and rendering a 3D Gaussian splat:

 1. [3D Gaussian Representation](#3d-gaussian-representation)
 2. [Projection of 3D Gaussian onto 2D Kernel](#projection-of-3d-gaussian-onto-2d-kernel)
 3. [Rendering: Sorting and Alpha Blending](#rendering-sorting-and-alpha-blending)

This extension defines a default `ellipse` kernel type that is based on the kernel defined in [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/). See the [Ellipse Kernel](#ellipse-kernel) section for more details on how this is defined.

### 3D Gaussian Representation

Each Gaussian splat using the default `ellipse` kernel represents a 3D Gaussian defined by the following equation:

```math
G(x) = \exp\left(-\frac{1}{2}(x-\mu)^T \Sigma^{-1} (x-\mu)\right)
```

Where:
- $G(x)$ is the value of the Gaussian at position $x$.
- $x$ is a 3D position vector.
- $\mu$ is the mean vector, representing the center of the Gaussian.
- $\Sigma$ is the 3x3 covariance matrix, defining the Gaussian's size, shape, and orientation.

For stable optimization and direct manipulation, the 3x3 covariance matrix $\Sigma$ is constructed from a rotation (quaternion) and a scaling vector. Using eigendecomposition of a covariance matrix, we can express $\Sigma$ as:

```math
\Sigma = \mathbf{R}\mathbf{S}\mathbf{S}^T\mathbf{R}^T
```

Where:
- $\mathbf{R}$ is a 3x3 rotation matrix derived from the quaternion.
- $\mathbf{S}$ is a 3x3 scaling matrix constructed from the scale vector.

To derive the rotation matrix $\mathbf{R}$ from the quaternion $(q_x, q_y, q_z, q_w)$:

```math
\mathbf{R} = \begin{pmatrix}
1 - 2(q_y^2 + q_z^2) & 2(q_x q_y - q_w q_z) & 2(q_x q_z + q_w q_y) \\
2(q_x q_y + q_w q_z) & 1 - 2(q_x^2 + q_z^2) & 2(q_y q_z - q_w q_x) \\
2(q_x q_z - q_w q_y) & 2(q_y q_z + q_w q_x) & 1 - 2(q_x^2 + q_y^2)
\end{pmatrix}
```

To derive the scale matrix $\mathbf{S}$ from the scale vector $(s_x, s_y, s_z)$:

```math
\mathbf{S} = \begin{pmatrix}
s_x & 0 & 0 \\
0 & s_y & 0 \\
0 & 0 & s_z
\end{pmatrix}
```

### Projection of 3D Gaussian onto 2D Kernel

To render the scene, each 3D Gaussian splat must be projected onto a 2D kernel shape based on the camera's perspective. This involves transforming the 3D covariance matrix $\Sigma$ into a 2D covariance matrix $\Sigma'$ that represents the Gaussian's shape on the image plane:

```math
\Sigma' = \mathbf{J}\mathbf{W}\Sigma\mathbf{W}^T\mathbf{J}^T
```

Where:
- $\mathbf{W}$ is the view transformation matrix and performs a rigid transformation (rotation and translation) from the world space to the camera space. This is your standard world-to-camera extrinsic matrix.
- $\mathbf{J}$ is the Jacobian matrix of the projection transformation. This is responsible for perspective.

The Jacobian matrix $\mathbf{J}$ for standard perspective projection is defined as:

```math
\mathbf{J} = \begin{pmatrix}
\frac{f_x}{z} & 0 & -\frac{f_x x}{z^2} \\
0 & \frac{f_y}{z} & -\frac{f_y y}{z^2}
\end{pmatrix}
```

### Rendering: Sorting and Alpha Blending

Once the 2D projected Gaussian splats are computed, they must be sorted and alpha blended to produce the final image. The alpha-blending is order-dependent, so correct sorting is crucial for accurate rendering. 

The sorting method for the `ellipse` kernel is based on the depth value of each Gaussian, which is the z-coordinate in camera space. Sorting order is front-to-back based on this depth value and typically uses a radix sort for performance. See the [Sorting Method](#sorting-method) section for more details on sorting.

Once sorted, the final color of each pixel is computed by alpha blending the splats in sorted order. The alpha blending equation is defined as:

```math
C = \sum_{i \in \mathscr{N}} c_i \alpha_i \prod_{j=1}^{i-1} (1 - \alpha_j)
```

Where:
- $C$ is the final color of the pixel.
- $\mathscr{N}$ is the set of splats that contribute to the pixel.
- $c_i$ is the color of the $i$-th Gaussian. See [Lighting](#lighting) for details on how to compute this color from the spherical harmonics.
- $\alpha_i = \alpha \cdot G(x)$ where $G(x)$ is the value of the projected 2D Gaussian's probability density function evaluated at the pixel center $x$.
- $\prod_{j=1}^{i-1} (1 - \alpha_j)$ is the accumulated transmittance.

*Non-normative Note: Transmittance is often implicitly handled by the GPU's blending hardware. See [Appendix A: Sample rendering with the base Ellipse Kernel and Spherical Harmonics](#appendix-a-sample-rendering-with-the-base-ellipse-kernel-and-spherical-harmonics) for a deeper example.*

## Lighting

At the time of writing, the most common method for lighting 3D Gaussian splats is via the real spherical harmonics. This extension defines attributes to store spherical harmonic coefficients for each splat. The zeroth-order spherical harmonic coefficients are always required. Higher order coefficients are optional. Each color channel has a separate coefficient, so for each degree $ℓ$, there are $(2ℓ + 1)$ coefficients, each containing RGB values.

These rules may be relaxed by future extensions that define alternative lighting methods or have specific requirements for handling compression, such as when a compression method stores the diffuse color components as linear color values instead of the zeroth-order coefficients.

### Image State & Relighting

Image state is defined by ISO 22028-1:2016 and indicates the rendering state of the image data. **_Display-referred_** (also known as _output-referred_ in ISO 22028-1:2016) image state represents data that has gone through color-rendering appropriate for display. **_Scene-referred_** image state represents data that represents the actual radiance of the scene.

The default `ellipse` kernel based on the original 3D Gaussian splatting paper typically uses the _BT.709-sRGB_ color space with a _display-referred_ image state for training and rendering. This is different than the typical glTF PBR material model, where scene-referred linear color spaces are used. This extension defines two display-referred color spaces but scene-referred color spaces may be added by extensions. See: [Available Color Spaces](#available-color-spaces)

Implementations are allowed to relight the splats than the one they were trained in, but this may lead to visual differences compared to the original training results.

### Calculating color from Spherical Harmonics

The diffuse color of the splat can be computed by multiplying the RGB coefficients of the zeroth-order spherical harmonic by the constant real spherical harmonic value of $0.282095$. This constant is derived from the formula for the real spherical harmonic of degree 0, order 0:

```math
Y_{0,0}(θ, φ) = \frac{1}{2} \sqrt{\frac{1}{π}} ≈ 0.282095
```

To keep the spherical harmonics within the [0, 1] range, the forward pass of the training process applies a _0.5_ bias to the DC component of the spherical harmonics. The rendering process must also apply this bias when reconstructing the color values from the spherical harmonics. This allows the training to occur around 0, ensuring numeric stability for the spherical harmonics, but also allows the coefficients to remain within a valid range for easy rendering.

Ergo, to calculate the diffuse RGB color from the DC component, the formula is:

```math
Color_{diffuse} = SH_{0,0} * 0.282095 + 0.5
```

Where $SH_{0,0}$ represents the RGB coefficients of the zeroth-order real spherical harmonic.

Subsequent degrees of spherical harmonics can be used to compute more complex lighting effects, such as ambient occlusion and specular highlights, by evaluating the spherical harmonics at the appropriate angles based on the surface normal and light direction. Functions for the higher order real spherical harmonics are defined as follows:

```math
\textbf{Degree 1, ℓ = 1}\\
\begin{aligned}
Y_{1,-1}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{y}{r}\\
Y_{1,0}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{z}{r}\\
Y_{1,1}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{x}{r}\\\\
\end{aligned}
```

```math
\textbf{Degree 2, ℓ = 2}\\
\begin{aligned}
Y_{2,-2}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{xy}{r^2}\\
Y_{2,-1}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{yz}{r^2}\\
Y_{2,0}(θ, φ) &= \frac{1}{4} \sqrt{\frac{5}{\pi}} \cdot \frac{3z^2 - r^2}{r^2}\\
Y_{2,1}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{xz}{r^2}\\
Y_{2,2}(θ, φ) &= \frac{1}{4} \sqrt{\frac{15}{\pi}} \cdot \frac{x^2 - y^2}{r^2}\\\\
\end{aligned}
```

```math
\textbf{Degree 3, ℓ = 3}\\
\begin{aligned}
Y_{3,-3}(θ, φ) &= \frac{1}{4} \sqrt{\frac{35}{2\pi}} \cdot \frac{y(3x^2 - y^2)}{r^3}\\
Y_{3,-2}(θ, φ) &= \frac{1}{2} \sqrt{\frac{105}{\pi}} \cdot \frac{xyz}{r^3}\\
Y_{3,-1}(θ, φ) &= \frac{1}{4} \sqrt{\frac{21}{2\pi}} \cdot \frac{y(5z^2 - r^2)}{r^3}\\
Y_{3,0}(θ, φ) &= \frac{1}{4} \sqrt{\frac{7}{\pi}} \cdot \frac{z(5z^2 - 3r^2)}{r^3}\\
Y_{3,1}(θ, φ) &= \frac{1}{4} \sqrt{\frac{21}{2\pi}} \cdot \frac{x(5z^2 - r^2)}{r^3}\\
Y_{3,2}(θ, φ) &= \frac{1}{4} \sqrt{\frac{105}{\pi}} \cdot \frac{z(x^2 - y^2)}{r^3}\\
Y_{3,3}(θ, φ) &= \frac{1}{4} \sqrt{\frac{35}{2\pi}} \cdot \frac{x(x^2 - 3y^2)}{r^3}\\
\end{aligned}
```

For all of these functions, $r$ represents the magnitude of the position vector, calculated as $r = \sqrt{x^2 + y^2 + z^2}$. Within 3D Gaussian splatting, normalization is used to ensure that the direction vectors are unit vectors. Therefore, $r$ is equal to $1$ when evaluating the spherical harmonics for lighting calculations.

The zeroth-order spherical harmonic is always required to ensure that the diffuse color can be accurately reconstructed, but higher order spherical harmonics are optional. If higher order spherical harmonics are used, lower order spherical harmonics must also be provided. Each order of spherical harmonics must be fully defined; partial definitions are not allowed.

Extensions extending this extension may define alternative lighting methods, have specific requirements for handling compression, or define different spherical harmonics handling.

See [Appendix A: Sample rendering with the base Ellipse Kernel and Spherical Harmonics](#appendix-a-sample-rendering-with-the-base-ellipse-kernel-and-spherical-harmonics) for more details on how to properly implement the lighting used by this extension.

## Schema Example

Example shown below including optional attributes and properties. This extension only affects any `primitive` nodes containting 3D Gaussian splat data.

```json
"meshes": [{
    "primitives": [{
        "attributes": {
            "POSITION": 0,
            "COLOR_0": 1,
            "KHR_gaussian_splatting:SCALE": 2,
            "KHR_gaussian_splatting:ROTATION": 3,
            "KHR_gaussian_splatting:OPACITY": 4,
            "KHR_gaussian_splatting:SH_DEGREE_0_COEF_0": 5,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_0": 6,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_1": 7,
            "KHR_gaussian_splatting:SH_DEGREE_1_COEF_2": 8
        },
        "mode": 0,
        "extensions": {
            "KHR_gaussian_splatting": {
                "kernel": "ellipse",
                "colorSpace": "BT.709-sRGB",
                "sortingMethod": "cameraDistance",
                "projection": "perspective"
            }
        }
    }]
}]
```

## Extension Properties

### Kernel

Gaussian splats can have a variety of shapes and this has the potential to change over time. The `kernel` property is a required property that provides an indication to the renderer the properties of the kernel used. Renderers are free to ignore any values they do not recognize.

Additional kernel types can be added over time by supplying an extension that defines an alternative shape definition and parameters.

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

A 2D `ellipse` kernel type is often used to represent 3D Gaussian splats in an ellipsoid shape based on the kernel defined in [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/). This simple type contains no parameters. This is the shape used by the reference renderer implementations for 3D Gaussian splatting. Following the original reference implementation this kernel assumes a _3σ_ cut-off (Mahalanobis distance of 3 units) for correct rendering.

The mean vector for the Gaussian splat is provided by the position of the mesh primitive. This defines the center of the Gaussian splat ellipsoid in global space.

The opacity of a Gaussian splat is defined by the `KHR_gaussian_splatting:OPACITY` attribute for this kernel. It stores a normalized value between _0.0_ (transparent) and _1.0_ (opaque). A sigmoid activation function applied during training ensures the value stays within this range. Out-of-range values are invalid. This guarantees that renderers can use the stored opacity directly for alpha blending without any extra processing.

The scale (`KHR_gaussian_splatting:SCALE`) and rotation (`KHR_gaussian_splatting:ROTATION`) attributes define the size and orientation of the ellipsoid in 3D space. These attributes represent the covariance matrix of the Gaussian in a factored form. The scale attribute values correspond to the spread of the Gaussian along its local principal axes and the rotation attribute values correspond to the orientation of those axes in global space.

`KHR_gaussian_splatting:SCALE` is stored in log-space, so the actual scale along each principal axis is computed as `exp(scale)`. This allows for representing a wide range of scales while maintaining numerical stability.

`KHR_gaussian_splatting:ROTATION` is stored as a unit quaternion in the order (x, y, z, w), where `w` is the scalar component. This quaternion represents the rotation from the local space of the Gaussian to global space.

Together, the scale and rotation can be used to reconstruct the full covariance matrix of the Gaussian splat for rendering purposes. Combined with the position attribute, these values define the identity and shape of the ellipsoid in 3D space.

More details on how to interpret these attributes for rendering can be found in the [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) paper.

See [Appendix A: Sample rendering with the base Ellipse Kernel and Spherical Harmonics](#appendix-a-sample-rendering-with-the-base-ellipse-kernel-and-spherical-harmonics) for more details on how to properly implement the ellipse kernel used by this extension.

### Color Space

The `colorSpace` property is a required property that specifies the color space of the 3D Gaussian Splat when spherical harmonics are being used for the lighting. The color space is typically determined by the training process for the splats. This color space value only applies to the 3D Gaussian splatting data and does not affect any other color data in the glTF.

Unless specified otherwise by additional extensions, color space information refers to the reconstructed splat color values, therefore splat reconstruction and alpha blending must be performed on the attribute values as-is, before any color gamut or transfer function conversions.

Additional values can be added over time by defining extensions to add new color spaces. See the section, [Extending the Base Extension](#extending-the-base-extension), for more information.

#### Available Color Spaces

| Color Space | Description |
| --- | --- |
| srgb_rec709_display | BT.709 sRGB (display-referred) color space. |
| lin_rec709_display | BT.709 linear (display-referred) color space. |

*Non-normative Note: The string values for colorspace follow the color space identification pattern recommended by the ASWF Color Interoperability Forum (CIF). See: [ASWF Color Interoperability Forum](https://lf-aswf.atlassian.net/wiki/spaces/CIF/overview)*

### Projection

The `projection` property is an optional property that specifies how the Gaussians should be projected onto the kernel shape. This is typically provided by the training process for the splats. This property is meant to be extended in the future as new projections become standardized within the community.

This base extension defines a single projection method, `perspective`, which is the default value. This keeps the behavior consistent with the original 3D Gaussian splatting paper.

_Non-normative Note: See [the original 3D Gaussian Splatting paper](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) appendix A, "Details of Gradiant Computation," for more details on how the perspective projection is computed._

Additional values can be added over time by defining extensions to add new projection methods. See the section, [Extending the Base Extension](#extending-the-base-extension), for more information.

#### Known Projection Methods

| Projection Method | Description |
| --- | --- |
| perspective | (Default) The typical 3D perspective projection based on scene depth. |

### Sorting Method

The `sortingMethod` property is an optional property that specifies how the Gaussian particles should be sorted during the rendering process. This typically is provided by the training process for the splats. This property is meant to be extended in the future as new sorting methods become standardized within the community.

This base extension defines a single sorting method, `cameraDistance`, which is the default value. This keeps the behavior consistent with the original 3D Gaussian splatting paper.

Additional values can be added over time by defining extensions to add new sorting methods. See the section, [Extending the Base Extension](#extending-the-base-extension), for more information.

#### Known Sorting Methods

| Sorting Method | Description |
| --- | --- |
| cameraDistance | (Default) Sort the splats based on the length of the vector from the splat to the camera origin. |

## Attributes

| Splat Data | glTF Attribute | Accessor Type | Component Type | Required | Notes |
| --- | --- | --- | --- | --- | --- |
| Rotation | KHR_gaussian_splatting:ROTATION | VEC4 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | yes | Rotation is a quaternion with `w` as the scalar. (xyzw) |
| Scale | KHR_gaussian_splatting:SCALE | VEC3 | _float_ <br/>_signed byte_ <br/>_signed byte_ normalized <br/>_signed short_ <br/>_signed short_ normalized | yes | |
| Opacity | KHR_gaussian_splatting:OPACITY | SCALAR | _float_ <br/>_unsigned byte_ normalized <br/>_unsigned short_ normalized | yes | |
| Spherical Harmonics degree 0 | KHR_gaussian_splatting:SH_DEGREE_0_COEF_0 | VEC3 | _float_ | yes (unless using a different method for lighting) | |
| Spherical Harmonics degree 1 | KHR_gaussian_splatting:SH_DEGREE_1_COEF_n (n = 0 to 2) | VEC3 | _float_ | no (yes if degree 2 or 3 are used) | |
| Spherical Harmonics degree 2 | KHR_gaussian_splatting:SH_DEGREE_2_COEF_n (n = 0 to 4) | VEC3 | _float_ | no (yes if degree 3 is used) | |
| Spherical Harmonics degree 3 | KHR_gaussian_splatting:SH_DEGREE_3_COEF_n (n = 0 to 6) | VEC3 | _float_ | no | |

### Basic Attributes

Each 3D Gaussian splat has the following attributes. At minimum the attributes must contain `POSITION`, `KHR_gaussian_splatting:ROTATION`, `KHR_gaussian_splatting:SCALE`, and `KHR_gaussian_splatting:OPACITY`. `POSITION` is defined by the base glTF specification.

The `KHR_gaussian_splatting:ROTATION` and `KHR_gaussian_splatting:SCALE` attributes support quantized storage using normalized signed `byte` or `short` component types to reduce file size. If quantization is not needed, content creators should use the `float` component type for maximum precision.

The content of `KHR_gaussian_splatting:OPACITY`, `KHR_gaussian_splatting:ROTATION` and `KHR_gaussian_splatting:SCALE` are defined by their kernel. See the [Ellipse Kernel](#ellipse-kernel) section for more information for information about how these are defined for the default `ellipse` kernel.

### Spherical Harmonics Attributes

When spherical harmonics are being used for lighting, the coefficients for the diffuse component must be provided using the `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` attribute semantic. The zero-order spherical harmonic coefficients are always required to allow for properly handling cases where the diffuse color is not in the _BT.709_ color gamut. The `KHR_gaussian_splatting:SH_DEGREE_ℓ_COEF_n` attributes where ℓ > 0 hold the higher degrees of spherical harmonics data and are not required. If higher degrees of spherical harmonics are used then lower degrees are required implicitly.

Each increasing degree of spherical harmonics requires more coeffecients. At the 1st degree, 3 sets of coeffcients are required, increasing to 5 sets for the 2nd degree, and increasing to 7 sets at the 3rd degree. With all 3 degrees, this results in 45 spherical harmonic coefficients stored in the `KHR_gaussian_splatting:SH_DEGREE_ℓ_COEF_n` attributes.

Spherical harmonic data is packed in an (r, g, b) format within the VEC3 accessor type. Each coefficient contains 3 values representing the red, green, and blue channels of the spherical harmonic coefficient. Spherical harmonic degrees cannot be partially defined. For example, if any degree 2 spherical harmonics attribute sematnics are used, then all degree 2 and degree 1 spherical harmonic coefficients must be provided.

### Improving Fallback with COLOR_0

To support better fallback functionality, the `COLOR_0` attribute semantic from the base glTF specification may be used to provide the diffuse color of the 3D Gaussian splat. This allows renderers to color the points in the sparse point cloud when 3D Gaussian splatting is not supported by a renderer. The value of `COLOR_0` is derived by multiplying the 3 diffuse color components of the 3D Gaussian splat with the constant zeroth-order spherical harmonic (ℓ = 0) for the RGB channels. The alpha channel should contain the opacity of the splat.

*_Non-normative Note:_* If the spherical harmonics are in the BT.709 gamut, the diffuse color can be computed from the `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` attribute by multiplying each of the RGB components by the constant spherical harmonic value of _0.282095_.

## Extending the Base Extension

3D Gaussian splatting is an evolving technology with new techniques and methods being developed over time. To provide a solid foundation for 3D Gaussian splatting in glTF while allowing for future growth and innovation, this extension is designed to be extensible. New kernel types, color spaces, projection methods, and sorting methods can be added over time without requiring changes to the base extension.

Extensions may define additional attributes or custom properties as needed to support new features. Attribute semantics should be prefixed with their respective extension name to avoid naming collisions. Extensions may also define additional values for the `kernel`, `colorSpace`, `projection`, and `sortingMethod` properties. Custom properties should be included in the body of the new extension object.

*_Non-normative Note: It is possible to share data between two attributes by using the same accessor index for multiple attribute semantics. This can be useful to optimize the storage of data._*

Compression extensions that operate on 3D Gaussian splatting data should extend this base extension to ensure compatibility. Compression extensions must define how the data can be decoded back into the base 3D Gaussian splatting format defined by this extension, but may also allow optimizations specific to their compression method. (e.g. passing textures or other data directly to the GPU for decoding.)

To use an extensions that extends `KHR_gaussian_splatting`, the extension must be included within the `extensions` property of the `KHR_gaussian_splatting` extension object. The extension must also be listed in `extensionsUsed` at the top level of the glTF.

Extension authors are encouraged to define fallback behaviors for renderers that do not recognize the new extension, but this is not strictly required. If a fallback is not possible, the extension should be listed in `extensionsRequired` to ensure that renderers that do not support the extension do not attempt to render the data incorrectly.

#### Example: Adding additional Kernel Types

*This section is non-normative.*

In order to add additional kernel types, a new extension should be defined that extends `KHR_gaussian_splatting`. This new extension would define the new kernel type and any parameters it may require. A renderer that recognizes the new kernel type can then use the parameters defined in the new extension to render the splats appropriately. Renderers that do not recognize the new kernel type should fall back to the default `ellipse` type.

For example, a new extension `EXT_gaussian_splatting_kernel_customShape` could be defined that adds a new kernel type `customShape` with additional parameters.

```json
"meshes": [{
    "primitives": [{
        // ...omitted for brevity...
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

If the kernel type requires additional attributes, those attributes should be defined within the new extension using unique semantics to avoid collisions.

```json
"meshes": [{
    "primitives": [{
        "attributes": {
            "POSITION": 0,
            "KHR_gaussian_splatting:SCALE": 1,
            "KHR_gaussian_splatting:ROTATION": 2,
            "EXT_gaussian_splatting_kernel_customShape:CUSTOM_ATTRIBUTE": 3
        },
        // ...omitted for brevity...
        "extensions": {
            "KHR_gaussian_splatting": {
                "kernel": "customShape",
                "extensions": {
                    "EXT_gaussian_splatting_kernel_customShape": {
                        "customParameter1": 1.0
                    }
                }
            }
        }
    }]
}]
```

The extension must also be listed in `extensionsUsed` at the top level of the glTF.

```json
  "extensionsUsed" : [
    "KHR_gaussian_splatting",
    "EXT_gaussian_splatting_kernel_customShape"
  ]
```

## Appendix A: Sample rendering with the base Ellipse Kernel and Spherical Harmonics

*This section is non-normative.*

This appendix provides a high-level overview using WebGL2 of how to implement rendering for 3D Gaussian splats using the default `ellipse` kernel and spherical harmonics for lighting. This example assumes a basic understanding of shader programming and rendering pipelines.

*Note: This is a simplified example and may not cover all edge cases or optimizations needed for production use.*

### Rendering Process Overview

The rendering process involves the following steps:

1. **Reconstruct the 3D Gaussian**: For each splat, reconstruct the 3D Gaussian using the position, scale, and rotation attributes to form the covariance matrix.
2. **Project onto 2D Kernel**: Use the camera's view and projection matrices to project the 3D Gaussian onto a 2D kernel shape on the image plane.
3. **Sort the Splats**: Sort the splats based on their distance to the camera using the `cameraDistance` sorting method.
4. **Alpha Blend**: Perform alpha blending of the sorted splats using the opacity attribute and the projected Gaussian values to compute the final pixel colors
5. **Lighting with Spherical Harmonics**: Compute the color of each splat using the spherical harmonics coefficients provided in the attributes. The diffuse color is derived from the zeroth-order spherical harmonic, while higher-order coefficients can be used for more complex lighting effects.

See the [Rendering: Sorting and Alpha Blending](#rendering-sorting-and-alpha-blending) and [Lighting](#lighting) sections for more details on the mathematical formulations used in these steps.

### Before the Shaders

Several tasks are performed on the CPU side before rendering:

- **Data Preparation**: Load the glTF file and extract the 3D Gaussian splat attributes, including position, scale, rotation, opacity, and spherical harmonics coefficients.
- **Sorting**: Sort the splats based on their distance to the camera using the `cameraDistance` method.
- **Setup Blending State**: Configure the WebGL blending state to use alpha blending.

```javascript
// Enable alpha blending
// Set this before you issue your draw call.
gl.enable(gl.BLEND);
gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
```

Sorting can be performed on the CPU or GPU depending on the number of splats and performance considerations. It's common to use a radix sort for efficient sorting based on depth values.

### Vertex Shader Example

The vertex shader takes the 3D data for a single Gaussian splat and prepares it for the 2D rendering stage. It computes the 2D shape and position of the Gaussian splat on the screen.

<details>

<summary>Click to expand the vertex shader code example</summary>

```glsl
#version 300 es
precision highp float;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform vec2 u_focal_length;
uniform vec3 u_camera_position;

// This uniform controls the SH degree at RUNTIME.
// Set from JavaScript to 1, 2, or 3.
uniform int u_sh_degree;

// All attributes are now always declared. The shader program requires a fixed set of inputs.
in vec2 a_quad_vertex;
in vec3 a_glob_position;
in vec3 a_glob_scale;
in vec4 a_glob_rotation;
in float a_glob_opacity;

// L=0
in vec3 a_sh_0;
// L=1
in vec3 a_sh_1;
in vec3 a_sh_2;
in vec3 a_sh_3;
// L=2
in vec3 a_sh_4;
in vec3 a_sh_5;
in vec3 a_sh_6;
in vec3 a_sh_7;
in vec3 a_sh_8;
// L=3
in vec3 a_sh_9;
in vec3 a_sh_10;
in vec3 a_sh_11;
in vec3 a_sh_12;
in vec3 a_sh_13;
in vec3 a_sh_14;
in vec3 a_sh_15;

out vec3 v_color;
out float v_opacity;
out vec3 v_cov2d_inv_upper;
out vec2 v_center_px;

// --- SPHERICAL HARMONICS CONSTANTS ---
const float SH_C0 = 0.28209479177f;
const float SH_C1 = 0.4886025119f;
const float SH_C2_0 = 1.09254843059f;
const float SH_C2_1 = 0.31539156525f;
const float SH_C2_2 = 0.54627421529f;
const float SH_C3_0 = 0.5900435899f;
const float SH_C3_1 = 2.8906114426f;
const float SH_C3_2 = 0.4570457996f;
const float SH_C3_3 = 0.3731763325f;

void main() {
    // Steps 1-4: Projection, Covariance, and Bounding Box logic (identical)
    vec4 view_space_pos = u_view_matrix * vec4(a_glob_position, 1.0f);
    
    float z_depth = view_space_pos.z;
    if(z_depth <= 0.1f) {
        gl_Position = vec4(2.0f, 2.0f, 2.0f, 1.0f);
        return;
    }
    
    vec3 scale = exp(a_glob_scale);
    
    mat3 rotation_matrix = mat3(1.0f - 2.0f * (a_glob_rotation.y * a_glob_rotation.y + a_glob_rotation.z * a_glob_rotation.z), 
                                2.0f * (a_glob_rotation.x * a_glob_rotation.y - a_glob_rotation.z * a_glob_rotation.w), 
                                2.0f * (a_glob_rotation.x * a_glob_rotation.z + a_glob_rotation.y * a_glob_rotation.w), 
                                2.0f * (a_glob_rotation.x * a_glob_rotation.y + a_glob_rotation.z * a_glob_rotation.w), 
                                1.0f - 2.0f * (a_glob_rotation.x * a_glob_rotation.x + a_glob_rotation.z * a_glob_rotation.z), 
                                2.0f * (a_glob_rotation.y * a_glob_rotation.z - a_glob_rotation.x * a_glob_rotation.w), 
                                2.0f * (a_glob_rotation.x * a_glob_rotation.z - a_glob_rotation.y * a_glob_rotation.w), 
                                2.0f * (a_glob_rotation.y * a_glob_rotation.z + a_glob_rotation.x * a_glob_rotation.w), 
                                1.0f - 2.0f * (a_glob_rotation.x * a_glob_rotation.x + a_glob_rotation.y * a_glob_rotation.y));
    
    mat3 scale_matrix = mat3(scale.x, 0, 0, 
                             0, scale.y, 0,
                             0, 0, scale.z);
    
    mat3 M = rotation_matrix * scale_matrix;
    mat3 sigma3d = M * transpose(M);
    mat3 W = transpose(mat3(u_view_matrix));
    mat3 T = W * sigma3d * transpose(W);
    mat3 J = mat3(u_focal_length.x / z_depth, 0, -(u_focal_length.x * view_space_pos.x) / (z_depth * z_depth), 0, u_focal_length.y / z_depth, -(u_focal_length.y * view_space_pos.y) / (z_depth * z_depth), 0, 0, 0);
    mat3 cov2d = transpose(J) * T * J;
    
    float det = cov2d[0][0] * cov2d[1][1] - cov2d[0][1] * cov2d[0][1];
    if(det <= 0.0f) {
        gl_Position = vec4(2.0f, 2.0f, 2.0f, 1.0f);
        return;
    }
    
    float mid = 0.5f * (cov2d[0][0] + cov2d[1][1]);
    float lambda1 = mid + sqrt(max(0.01f, mid * mid - det));
    float radius = ceil(3.0f * sqrt(max(lambda1, 0.0f)));
    
    vec4 clip_space_pos = u_projection_matrix * view_space_pos;
    vec2 ndc_pos = clip_space_pos.xy / clip_space_pos.w;
    clip_space_pos.xy += a_quad_vertex * radius / u_focal_length * clip_space_pos.w;
    gl_Position = clip_space_pos;

    // --- STEP 5: Calculate View-Dependent Color using Spherical Harmonics ---
    vec3 view_dir = normalize(a_glob_position - u_camera_position);

    // L=0 is always calculated.
    vec3 result = SH_C0 * a_sh_0;

    // Use `if` statements based on the uniform to conditionally add higher-order contributions.
    if(u_sh_degree >= 1) {
        float dx = view_dir.x, dy = view_dir.y, dz = view_dir.z;
        result += SH_C1 * dy * a_sh_1;
        result += SH_C1 * dz * a_sh_2;
        result += SH_C1 * dx * a_sh_3;

        if(u_sh_degree >= 2) {
            float dx2 = dx * dx, dy2 = dy * dy, dz2 = dz * dz;
            result += SH_C2_0 * dx * dy * a_sh_4;
            result += SH_C2_0 * dy * dz * a_sh_5;
            result += SH_C2_1 * (3.0f * dz2 - 1.0f) * a_sh_6;
            result += SH_C2_0 * dx * dz * a_sh_7;
            result += SH_C2_2 * (dx2 - dy2) * a_sh_8;

            if(u_sh_degree >= 3) {
                result += SH_C3_0 * dy * (3.0f * dx2 - dy2) * a_sh_9;
                result += SH_C3_1 * dx * dy * dz * a_sh_10;
                result += SH_C3_2 * dy * (5.0f * dz2 - 1.0f) * a_sh_11;
                result += SH_C3_3 * dz * (5.0f * dz2 - 3.0f) * a_sh_12;
                result += SH_C3_2 * dx * (5.0f * dz2 - 1.0f) * a_sh_13;
                result += SH_C3_1 * dz * (dx2 - dy2) * a_sh_14;
                result += SH_C3_0 * dx * (dx2 - 3.0f * dy2) * a_sh_15;
            }
        }
    }

    v_color = 1.0f / (1.0f + exp(-result));

    // --- STEP 6: Pass Data to Fragment Shader ---
    v_opacity = a_glob_opacity;
    float inv_det = 1.0f / det;
    v_cov2d_inv_upper.x = inv_det * cov2d[1][1];
    v_cov2d_inv_upper.y = -inv_det * cov2d[0][1];
    v_cov2d_inv_upper.z = inv_det * cov2d[0][0];
    v_center_px = (ndc_pos * 0.5f + 0.5f) * vec2(gl_Position.w / u_projection_matrix[0][0] * 2.0f, gl_Position.w / u_projection_matrix[1][1] * 2.0f);
}
```

</details>

### Fragment Shader Example

<details>

<summary>Click to expand the fragment shader code example</summary>

```glsl
#version 300 es

// Use high precision for floating-point calculations to avoid artifacts.
precision highp float;

// The final view-dependent color, pre-calculated by the vertex shader
// using Spherical Harmonics.
in vec3 v_color;

// The base opacity of the Gaussian, learned during training.
in float v_opacity;

// The three unique components of the symmetric 2D inverse covariance matrix (Σ')⁻¹.
// This defines the shape and orientation of the Gaussian's ellipse on the screen.
in vec3 v_cov2d_inv_upper; // (Σ'⁻¹)[0][0], (Σ'⁻¹)[0][1], (Σ'⁻¹)[1][1]

// The 2D center of the projected Gaussian in screen pixel coordinates.
in vec2 v_center_px;


// The final color output for the current pixel. This will be sent to the
// GPU's blending unit.
out vec4 o_frag_color;

void main () {

    // --- STEP 1: Calculate G(x), the Gaussian Influence ---

    // Find the vector from the current pixel's coordinate (gl_FragCoord.xy)
    // to the center of the projected Gaussian.
    vec2 d = gl_FragCoord.xy - v_center_px;

    // Evaluate the quadratic form: (x-μ)ᵀ * (Σ')⁻¹ * (x-μ)
    // This gives us the squared Mahalanobis distance, which is the exponent's power.
    // This value is 0 at the center of the Gaussian and increases as we move away.
    float power = 0.5 * (d.x * d.x * v_cov2d_inv_upper.x + d.y * d.y * v_cov2d_inv_upper.z) + d.x * d.y * v_cov2d_inv_upper.y;
    
    // The value of the 2D Gaussian function is exp(-power).
    // If the power is large, the influence is negligible. We can discard the pixel
    // early to save computation. A threshold of 4.0 corresponds to e⁻⁴, which is
    // very transparent and visually insignificant.
    if (power > 4.0) {
        discard;
    }

    // Calculate the actual influence value of the Gaussian at this pixel.
    float G_x = exp(-power);


    // --- STEP 2: Calculate the Final Alpha Contribution (aᵢ) ---

    // The final alpha (aᵢ) is the product of the Gaussian's base opacity (α)
    // and its influence at this specific pixel (G(x)).
    float final_alpha = v_opacity * G_x;

    // Optimization: If the final alpha is practically invisible, discard the pixel.
    // This avoids sending nearly transparent pixels to the blending unit, which can
    // improve performance, especially on mobile or integrated GPUs.
    if (final_alpha < 1.0 / 255.0) {
        discard;
    }


    // --- STEP 3: Set Final Pixel Color ---

    // Output the final color in "pre-multiplied alpha" format.
    // The RGB color channels are multiplied by the final alpha.
    // The alpha channel is the final alpha itself.
    //
    // Format: vec4(color * alpha, alpha)
    //
    // This format is required for the blending function `glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)`
    // to correctly calculate the final blended color and transmittance on the GPU.
    o_frag_color = vec4(v_color * final_alpha, final_alpha);
}
```

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

- [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/)
- [Gaussian Splatting Algorithm Details](https://github.com/joeyan/gaussian_splatting/blob/main/MATH.md)

