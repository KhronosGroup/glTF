<!--
Copyright 2026 The Khronos Group Inc.
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

Release Candidate

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines basic support for storing 3D Gaussian splats in glTF assets, bringing structure and conformity to the 3D Gaussian splatting space. 3D Gaussian splatting uses fields of Gaussians that can be treated as a point cloud for the purposes of storage. This extension defines 3D Gaussian splats by their position, rotation, scale, opacity, and spherical harmonics, which provide both diffuse and specular color. These values are stored as attributes on a point primitive. Since the extension treats the 3D Gaussian splats as point primitives, a graceful fallback to treating the data as a sparse point cloud is possible.

A key objective of this extension is to establish a solid foundation for integrating 3D Gaussian splatting into glTF assets, while enabling future enhancements and innovation. To achieve this, the extension is intentionally designed to be extended itself, allowing future extensions to introduce new kernel types, color spaces, projection methods, and sorting methods over time as 3D Gaussian splatting techniques evolve and become standards within the glTF ecosystem.

## Extending Mesh Primitives

When a mesh primitive contains an `extension` property defining `KHR_gaussian_splatting`, this indicates to the client that the primitive SHOULD be treated as a 3D Gaussian splatting field.

Other extensions that depend on this extension such as 3D Gaussian splatting compression extensions MAY require that this extension be included in `extensionsRequired`.

The extension object contains the following properties:

|                   | Type     | Description                                        | Required                       |
| ----------------- | -------- | -------------------------------------------------- | ------------------------------ |
| **kernel**        | `string` | The kernel used to generate the Gaussians.         | :white_check_mark: Yes         |
| **colorSpace**    | `string` | The color space of the reconstructed color values. | :white_check_mark: Yes         |
| **projection**    | `string` | The projection method for rendering the Gaussians. | No, default `"perspective"`.   |
| **sortingMethod** | `string` | The sorting method for rendering the Gaussians.    | No, default `"cameraDistance"` |

### Kernel

Gaussian splats can have a variety of shapes and this has the potential to change over time. The `kernel` property is a required property that defines an exact Gaussian type, i.e., its shape, attributes, intended rendering techniques, and interactions with the base glTF features.

This extension defines only one kernel type called `"ellipse"`, that is a 2D ellipse kernel used to project an ellipsoid shape in 3D space. Additional kernel types can be added over time by supplying an extension that defines an alternative definition. See the section [Ellipse Kernel](#ellipse-kernel) below for the exact definitions.

### Color Space

The `colorSpace` property is a required property that specifies the color space of the reconstructed 3D Gaussian Splat color values. The color space is typically determined by the training process for the splats. This color space value only applies to the 3D Gaussian splatting data and does not affect any other color data in the glTF.

Unless specified otherwise by additional extensions, this color space information refers to the reconstructed splat color values, therefore all intermediate rendering steps (including but not limited to alpha blending) MUST be performed before any color gamut or transfer function conversions.

Additional values can be added over time by defining extensions to add new color spaces. See the section [Extending the Base Extension](#extending-the-base-extension) for more information.

#### Available Color Spaces

| Color Space           | Description                                   |
| --------------------- | --------------------------------------------- |
| `srgb_rec709_display` | BT.709 sRGB (display-referred) color space.   |
| `lin_rec709_display`  | BT.709 linear (display-referred) color space. |

> [!NOTE]
> These string values follow the color space identification pattern recommended by the ASWF Color Interoperability Forum (CIF). See: [ASWF Color Space Encodings for Displays](https://github.com/AcademySoftwareFoundation/ColorInterop/blob/main/Recommendations/02_DisplayColorSpaces/DisplayColorSpaces.md).

### Projection

The `projection` property is an optional property that specifies how the Gaussians are projected onto the kernel shape. This is typically provided by the training process for the splats. This property is meant to be extended in the future as new projections become standardized within the community.

This base extension defines a single projection method, `"perspective"`, which is the default value.

Additional values can be added over time by defining extensions to add new projection methods. See the section [Extending the Base Extension](#extending-the-base-extension) for more information.

### Sorting Method

The `sortingMethod` property is an optional property that specifies how the Gaussian particles are sorted during the rendering process. This typically is provided by the training process for the splats. This property is meant to be extended in the future as new sorting methods become standardized within the community.

This base extension defines a single sorting method, `cameraDistance`, which is the default value. This method sorts the splats based on the length of the vector from the splat to the camera origin (viewer's position).

Additional values can be added over time by defining extensions to add new sorting methods. See the section, [Extending the Base Extension](#extending-the-base-extension), for more information.

## Ellipse Kernel

A 2D ellipse kernel type represents 3D Gaussian splats in an ellipsoid shape.

### Dependencies on glTF

The `mode` property of the mesh primitive MUST be `POINTS` (0).

Unless specified otherwise by additional Gaussian splats extensions, the glTF material referenced from the mesh primitive (if any) MUST be ignored for splat rendering.

> [!NOTE]
> If this extension is not supported, implementations are expected to follow the base glTF specification and thus render the splat primitive as a point cloud. In such a case, the referenced glTF material will be used.

The global transformation matrix of the node containing a mesh with splat primitives SHOULD conform to all of the following conditions:

- The last row is $(0, 0, 0, 1)$ (implicitly enforced by the base glTF specification).
- The lengths of the first three columns are finite positive numbers.
- The determinant of the upper-left 3x3 matrix formed by dividing each matrix element by the length of its column is close to positive one.

These rules ensure that the transformation matrix is decomposable into regular translation, rotation, and positive scale values. Splat rendering with non-decomposable transformation matrices or with negative scale values is undefined.

The camera used for splat rendering SHOULD use perspective projection. Splat rendering with non-perspective projections is undefined.

### Attributes

This kernel defines the following attributes for the mesh primitive.

| Splat Data                   | Attribute Semantic                | Accessor Type | Component Type | Required |
| ---------------------------- | --------------------------------- | - | - | - |
| Position                     | `POSITION`                        | VEC3   | Inherited from the glTF specification | :white_check_mark: Yes |
| Rotation                     | `KHR_gaussian_splatting:ROTATION` | VEC4   | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | :white_check_mark: Yes |
| Scale                        | `KHR_gaussian_splatting:SCALE`    | VEC3   | _float_ <br/>_unsigned byte_ <br/>_unsigned byte_ normalized <br/>_unsigned short_ <br/>_unsigned short_ normalized | :white_check_mark: Yes |
| Opacity                      | `KHR_gaussian_splatting:OPACITY`  | SCALAR | _float_ <br/>_unsigned byte_ normalized <br/>_unsigned short_ normalized | :white_check_mark: Yes |
| Spherical Harmonics degree 0 | `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0`     | VEC3 | _float_ | :white_check_mark: Yes |
| Spherical Harmonics degree 1 | `KHR_gaussian_splatting:SH_DEGREE_1_COEF_[0-2]` | VEC3 | _float_ | no (yes if degree 2 or 3 are used) |
| Spherical Harmonics degree 2 | `KHR_gaussian_splatting:SH_DEGREE_2_COEF_[0-4]` | VEC3 | _float_ | no (yes if degree 3 is used) |
| Spherical Harmonics degree 3 | `KHR_gaussian_splatting:SH_DEGREE_3_COEF_[0-6]` | VEC3 | _float_ | no |

The mean vector for the Gaussian splat is provided by the `POSITION` position of the mesh primitive. This defines the center of the Gaussian splat ellipsoid in local space. The effective global mean vector for the Gaussian splat is derived from the `POSITION` attribute value and the global transformation matrix of the glTF node that instantiates the mesh containing the splat primitive as defined in the glTF specification.

The scale (`KHR_gaussian_splatting:SCALE`) attribute values correspond to the spread of the Gaussian along its local principal axes. Scale values are linear and MUST NOT be negative.

The rotation (`KHR_gaussian_splatting:ROTATION`) attribute values correspond to the orientation of those axes in local space. Rotation values are stored as unit quaternions in the usual glTF order.

> [!NOTE]
> This guarantees that renderers can use quaternion values directly without renormalization.

The effective covariance matrix for the Gaussian splat is derived from the `KHR_gaussian_splatting:ROTATION` and `KHR_gaussian_splatting:SCALE` attributes and the effective transformation matrix of the glTF node that instantiates the mesh containing the splat primitive as described below.

The opacity of a Gaussian splat is defined by the `KHR_gaussian_splatting:OPACITY` attribute. It stores a normalized linear value between _0.0_ (transparent) and _1.0_ (opaque). Out-of-range values are invalid.

> [!NOTE]
> A sigmoid activation function applied during training ensures the value stays within the valid range. This guarantees that renderers can use the stored opacity directly for alpha blending without any extra processing.

#### Spherical Harmonics Attributes

The `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` attribute semantic provides the diffuse component coefficients for the spherical harmonics. The zeroth-order spherical harmonic coefficients are always required. The `KHR_gaussian_splatting:SH_DEGREE_l_COEF_n` attributes where `l` is greater than zero hold the higher degrees of spherical harmonics data. To use higher degrees of spherical harmonics the lower degrees MUST be defined.

Each increasing degree of spherical harmonics requires more coefficients. At the first degree, three sets of coefficients are required, increasing to five sets for the second degree, and increasing to seven sets at the third degree. With all three degrees, this results in 45 spherical harmonic coefficients stored in the `KHR_gaussian_splatting:SH_DEGREE_l_COEF_n` attributes.

Attributes are packed from the lowest order ($m$) to highest for each degree. For example, `KHR_gaussian_splatting:SH_DEGREE_1_COEF_0` provides $m = -1$ coefficients, `KHR_gaussian_splatting:SH_DEGREE_1_COEF_1` provides $m = 0$ coefficients, and `KHR_gaussian_splatting:SH_DEGREE_1_COEF_2` provides $m = 1$ coefficients.

Each coefficient contains three values representing the red, green, and blue channels of the spherical harmonic. Spherical harmonic degrees MUST NOT be partially defined, that is, either all coefficients for a given degree and all lower degrees MUST be defined or none.

## Ellipse Kernel Rendering

To render a field of 3D Gaussian splats, the renderer reconstructs each Gaussian splat using the same forward pass algorithm used during training, projects it onto a 2D plane, computes its color, and composes it with other splats.

This kernel assumes a _3σ_ cut-off (Mahalanobis distance of 3 units) for correct rendering.

> [!NOTE]
> This is the shape used by the original [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) paper.

### Splat Reconstruction

Each Gaussian splat represents a 3D Gaussian defined by the following equation:

```math
G(x) = \exp\left(-\frac{1}{2}(x-\mu)^T \Sigma^{-1} (x-\mu)\right)
```

Where:

- $G(x)$ is the value of the Gaussian at position $x$.
- $x$ is a 3D position vector.
- $\mu$ is the mean vector, representing the center of the Gaussian.
- $\Sigma$ is the 3x3 covariance matrix, defining the Gaussian's size, shape, and orientation.

The first step is combining the scale and rotation attributes of the splat with the global transformation matrix of the glTF node containing the splat primitive to produce the 3x3 covariance matrix.

The 3x3 3D covariance matrix $\Sigma$ is constructed as follows:

```math
\Sigma = \mathbf{M}\mathbf{C}\mathbf{C}^T\mathbf{M}^T
```

Where:

- $\mathbf{M}$ is a 3x3 upper-left submatrix of the node's global transformation matrix;
- $\mathbf{C}$ is a 3x3 matrix composed of the splat's scale and rotation attributes as defined below.

```math
\mathbf{C} = \begin{pmatrix}
s_x (1 - 2(q_y^2 + q_z^2)) & s_y (2(q_x q_y - q_w q_z)) & s_z (2(q_x q_z + q_w q_y)) \\
s_x (2(q_x q_y + q_w q_z)) & s_y (1 - 2(q_x^2 + q_z^2)) & s_z (2(q_y q_z - q_w q_x)) \\
s_x (2(q_x q_z - q_w q_y)) & s_y (2(q_y q_z + q_w q_x)) & s_z (1 - 2(q_x^2 + q_y^2))
\end{pmatrix}
```

### Splat Projection

The second step is projecting each splat onto a 2D kernel shape based on the camera's perspective transformation. This involves projecting the splat's mean vector (using the regular perspective projection math) and transforming the 3D covariance matrix $\Sigma$ into a 2D covariance matrix $\Sigma'$ that represents the Gaussian's shape on the image plane. The covariance matrix transformation uses the affine approximation:

```math
\Sigma' = \mathbf{J}\mathbf{W}\Sigma\mathbf{W}^T\mathbf{J}^T
```

Where:

- $\mathbf{W}$ is the 3x3 matrix that performs view-dependent transformation from the global space to the camera space; this is usually the upper-left 3x3 submatrix of camera's view transformation.
- $\mathbf{J}$ is the Jacobian matrix of the perspective projection transformation function including the perspective divide. This Jacobian matrix typically includes the camera's focal length and the splat's 3D mean vector transformed by the camera's view transformation matrix (rotation and translation).

> [!NOTE]
> Since the construction details of the view and perspective projection matrices are implementation-specific, the formal definitions of $\mathbf{W}$ and $\mathbf{J}$ are not provided in this specification.

### Splat Lighting

Each splat's color is computed from its spherical harmonics coefficients and the normalized viewing direction (vector from the camera position to the splat's mean in the global space) as described in the [Lighting](#lighting) section below.

Implementations MAY ignore higher-degree coefficients for performance reasons.

The spherical harmonics coefficients SHOULD be transformed by the rotation component of the global transformation matrix of the glTF node that instantiates the mesh containing the splat primitive.

> [!NOTE]
> This transformation is usually performed using Wigner-D matrices.

### Splat Composition

After the previous steps, splats are sorted and alpha blended to produce the final image. The alpha-blending is order-dependent, so correct sorting is crucial for accurate rendering.

The sorting is based on the distance between the camera position and the splat's mean vector in the global space. Sorting order is back-to-front based on this depth value and typically uses a radix sort for performance. See the [Sorting Method](#sorting-method) section for more details on sorting.

Once sorted, the final color of each pixel is computed by alpha blending the splats in the sorted order. The alpha blending equation is defined as:

```math
C = \sum_{i \in \mathscr{N}} c_i \alpha_i \prod_{j=1}^{i-1} (1 - \alpha_j)
```

Where:

- $C$ is the final color of the pixel. This value MUST be either clamped or tonemapped in an implementation-specific manner to fit into the splat's color space.
- $\mathscr{N}$ is the set of splats that contribute to the pixel.
- $c_i$ is the color of the $i$-th Gaussian. Unless specified otherwise by additional extensions, negative $c_i$ values MUST be clamped to zero.
- $\alpha_i = \alpha \cdot G(x)$ where $G(x)$ is the value of the projected 2D Gaussian's probability density function evaluated at the pixel center $x$ and $\alpha$ is the splat's opacity attribute value.
- $\prod_{j=1}^{i-1} (1 - \alpha_j)$ is the accumulated transmittance.

> [!NOTE]
> Within rasterizer implementations, transmittance is usually handled by the GPU's blending hardware.
>
> Since intermediate results of splats composition are not explicitly limited to any range, blending should be performed using floating-point color buffer formats. This approach more closely follows the design of the original paper.
>
> In a case when a normalized color buffer format is used instead, e.g., for performance or memory reasons, blending should be performed with alpha-premultiplied values by passing the $c_i \alpha_i$ product to the blending unit and setting the source blend factor to one. While still lossy, this approach causes less range clamping than passing color and alpha values separately.
>
> If using unsigned normalized color buffers, clamping of $c_i$ values less than zero and $C$ values greater then one happens automatically.

## Lighting

This extension defines 3D Gaussian splat lighting via the real spherical harmonics. The corresponding attributes store spherical harmonic coefficients for each splat. The zeroth-order spherical harmonic coefficients are always required. Higher order coefficients are optional. Each color channel has a separate coefficient, so for each degree $l$, there are $(2l + 1)$ coefficients, each containing values contributing to the red, green, and blue color components.

These rules MAY be relaxed by future extensions that define alternative lighting methods or have specific requirements for handling compression, such as when a compression method stores the diffuse color components as linear color values instead of the zeroth-order coefficients.

### Image State & Relighting

Image state is defined by ISO 22028-1:2016 and indicates the rendering state of the image data. **_Display-referred_** (also known as _output-referred_ in ISO 22028-1:2016) image state represents data that has gone through color-rendering appropriate for display. **_Scene-referred_** image state represents data that represents the actual radiance of the scene.

The ellipse kernel defined in this specification uses a _display-referred_ image state for training and rendering. This is similar to the material model described in the `KHR_materials_unlit` glTF extension, i.e., glTF scene lighting, exposure settings, and tonemapping generally do not affect rendered splats.

Still, implementations can relight the splats, e.g., to simulate shadows or additional light sources, or to adjust the rendered splats for an environment different than the one they were trained in. Naturally, this could lead to visual differences compared to the original results.

### Calculating color from Spherical Harmonics

The real spherical harmonics included with the [Condon–Shortley phase](https://mathworld.wolfram.com/Condon-ShortleyPhase.html) are used to calculate the color and specular of the Gaussians. 3D Gaussian splatting can use up to 45 spherical harmonic coefficients. Each coefficient is split into 3 color channels. This means that each degree has:

```math
SH_{n} \times 3 = SH_{total}
```

Where $SH_{n}$ represents the number of coefficients for degree $n$. This gives us 3 coefficients for Degree 0, 9 for Degree 1, 15 for Degree 2, and finally 21 for Degree 3.

The diffuse color of the splat can be computed by multiplying the RGB coefficients of the zeroth-order real spherical harmonic by the normalization constant value of $\approx0.282095$. This constant is derived from the formula for the real spherical harmonic of degree 0, order 0:

```math
Y_{0,0}(θ, φ) = \frac{1}{2} \sqrt{\frac{1}{π}} ≈ 0.282095
```

To keep the spherical harmonics within the [0, 1] range, the forward pass of the training process applies a _0.5_ bias to the zeroth-order component of the spherical harmonics. The rendering process also applies this bias when reconstructing the color values from the spherical harmonics. This allows the training to occur around 0, ensuring numeric stability for the spherical harmonics, but also allows the coefficients to remain within a valid range for easy rendering.

Ergo, to calculate the diffuse RGB color from only the zeroth-order component, the formula is:

```math
Color_{diffuse} = SH_{0,0} * 0.2820947917738781 + 0.5
```

Where $SH_{0,0}$ represents the vector of the RGB coefficients for the zeroth-order real spherical harmonic.

The Condon–Shortley phase is the $(-1)^m$ sign factor included in some spherical‑harmonic definitions to keep their algebraic and normalization properties consistent. This convention ensures the spherical harmonics are orthogonal and behave cleanly with rotation. As defined, odd values for order ($m$) are negated. For Degree 2 and Degree 3, this gives some of the constants negative signs.

Subsequent degrees of spherical harmonics are used to compute more complex lighting effects, such as ambient occlusion and specular highlights, by evaluating the spherical harmonics at the appropriate angles based on the surface normal and light direction. As an example, the Degree 1 functions are:

```math
\begin{aligned}
Y_{1,-1}(θ, φ) &= -\sqrt{\frac{3}{4\pi}} \cdot \frac{y}{r}\\
Y_{1,0}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{z}{r}\\
Y_{1,1}(θ, φ) &= -\sqrt{\frac{3}{4\pi}} \cdot \frac{x}{r}\\
\end{aligned}
```

A full list of all spherical harmonic functions and constants including the Condon–Shortley phase can be found in [Appendix A: Spherical Harmonics Reference](#appendix-a-spherical-harmonics-reference).

For all of these functions, $r$ represents the magnitude of the position vector, calculated as $r = \sqrt{x^2 + y^2 + z^2}$. Within 3D Gaussian splatting, normalization is used to ensure that the direction vectors are unit vectors. Therefore, $r$ is equal to $1$ when evaluating the spherical harmonics for lighting calculations.

We can use these functions combined with the zero-order component to calculate the full color of a Gaussian:

```math
\begin{aligned}
Color_{SH_{0}} =\,&SH_{0,0} \cdot 0.2820947917738781\\\\
Color_{SH_{1}} =\,&SH_{1,-1} \cdot y \cdot -0.4886025119029199\,+\\
                  &SH_{1,0} \cdot z \cdot 0.4886025119029199\,+\\
                  &SH_{1,1} \cdot x \cdot -0.4886025119029199\\\\
Color_{SH_{2}} =\,&SH_{2,-2} \cdot xy \cdot 1.092548430592079\,+\\
                  &SH_{2,-1} \cdot yz \cdot -1.092548430592079\,+\\
                  &SH_{2,0} \cdot (2z^2 - x^2 - y^2) \cdot 0.3153915652525200\,+\\
                  &SH_{2,1} \cdot xz \cdot -1.092548430592079\,+\\
                  &SH_{2,2} \cdot (x^2 - y^2) \cdot 0.5462742152960395\\\\
Color_{SH_{3}} =\,&SH_{3,-3} \cdot y(3x^2 - y^2) \cdot -0.5900435899266435\,+\\
                  &SH_{3,-2} \cdot xyz \cdot 2.890611442640554\,+\\
                  &SH_{3,-1} \cdot y(4z^2 - x^2 - y^2) \cdot -0.4570457994644657\,+\\
                  &SH_{3,0} \cdot z(2z^2 - 3x^2 - 3y^2) \cdot 0.3731763325901154\,+\\
                  &SH_{3,1} \cdot x(4z^2 - x^2 - y^2) \cdot -0.4570457994644657\,+\\
                  &SH_{3,2} \cdot z(x^2 - y^2) \cdot 1.445305721320277\,+\\
                  &SH_{3,3} \cdot x(x^2 - 3y^2) \cdot -0.5900435899266435\\\\
Color_{final} =\,&Color_{SH_{0}} + Color_{SH_{1}} + Color_{SH_{2}} + Color_{SH_{3}} + 0.5
\end{aligned}
```

Where $SH_{l,m}$ represents the RGB spherical harmonic coefficients for a particular degree ($l$) and order ($m$) and $x$, $y$, and $z$ represent the independent parts of the direction vector.

The zeroth-order spherical harmonic is always required to ensure that the diffuse color can be accurately reconstructed, but higher order spherical harmonics are optional. If higher order spherical harmonics are used, lower order spherical harmonics MUST also be provided. Each order of spherical harmonics MUST be fully defined; partial definitions are not allowed.

Extensions extending this extension MAY define alternative lighting methods, have specific requirements for handling compression, or define different spherical harmonics handling.

See [Appendix A: Spherical Harmonics Reference](#appendix-a-spherical-harmonics-reference) for an easy reference of the spherical harmonic basis functions and normalization constants.

> [!NOTE]
> For training software and exporters, it is recommended that the Gaussians are trained within the [glTF coordinate system](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units) when targeting glTF. Otherwise, when converting pre-trained data from other coordinate systems into the glTF coordinate system, the positions, quaternions and spherical harmonics will have to be rotated on load.

## glTF JSON Example

Partial glTF JSON example shown below including optional attributes and properties.

```json
{
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
                    "colorSpace": "srgb_rec709_display",
                    "sortingMethod": "cameraDistance",
                    "projection": "perspective"
                }
            }
        }]
    }]
}
```

## Fallback Behavior

To support fallback functionality, the `COLOR_0` attribute semantic from the base glTF specification MAY be used to provide the diffuse color of the 3D Gaussian splat. This allows renderers to color the points in the sparse point cloud when 3D Gaussian splatting is not supported by a renderer. The value of `COLOR_0` is derived by multiplying the 3 diffuse color components of the 3D Gaussian splat with the constant zeroth-order spherical harmonic ($l$ = 0) for the RGB channels. The alpha channel SHOULD contain the opacity of the splat.

> [!NOTE]
> If the color space is `"srgb_rec709_display"` or `"lin_rec709_display"`, the diffuse color for the ellipse kernel suitable for `COLOR_0` fallback usage can be computed from the `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` attribute by multiplying each of the RGB components by the constant spherical harmonic value of _0.282095_, adding _0.5_, and clamping the sum to the $[0, 1]$ range; if the color space is `"srgb_rec709_display"`, the clamped values would have to be decoded from sRGB to linear because the `COLOR_0` attribute contains linear values as per the glTF specification.

## Extending the Base Extension

3D Gaussian splatting is an evolving technology with new techniques and methods being developed over time. To provide a solid foundation for 3D Gaussian splatting in glTF while allowing for future growth and innovation, this extension is designed to be extensible. New kernel types, color spaces, projection methods, and sorting methods can be added over time without requiring changes to the base extension.

Extensions MAY define additional attributes or custom properties as needed to support new features. Attribute semantics SHOULD be prefixed with their respective extension name to avoid naming collisions. Extensions MAY also define additional values for the `kernel`, `colorSpace`, `projection`, and `sortingMethod` properties. Custom properties SHOULD be included in the body of the new extension object.

> [!NOTE]
> It is possible to share data between two attributes by using the same accessor index for multiple attribute semantics. This can be useful to optimize the storage of data.

Compression extensions that operate on 3D Gaussian splatting data SHOULD extend this base extension to ensure compatibility. Compression extensions SHOULD define how the data can be decoded back into the base 3D Gaussian splatting format defined by this extension, but MAY also allow optimizations specific to their compression method, e.g., passing textures or other data directly to the GPU for decoding.

To use an extension that extends `KHR_gaussian_splatting`, the extension MUST be included within the `extensions` property of the `KHR_gaussian_splatting` extension object. The new extension MUST also be listed in `extensionsUsed` at the top level of the glTF.

Extension authors are encouraged to define fallback behaviors for renderers that do not recognize the new extension, but this is not strictly required. If a fallback is not possible, the new extension SHOULD also be listed in `extensionsRequired` to ensure that renderers that do not support the extension do not attempt to render the data incorrectly.

### Example: Adding additional Kernel Types

_This section is non-normative._

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

## Appendix A: Spherical Harmonics Reference

_This appendix is non-normative and provided for informational purposes only._

### Real Spherical Harmonic Basis Functions

Degrees $0$ through $3$, in cartesian space. Including the Condon–Shortley phase $(-1)^m$.

```math
\textbf{Degree 0, l = 0}\\
\begin{aligned}
Y_{0,0}(θ, φ) &= \frac{1}{2} \sqrt{\frac{1}{\pi}}\\\\
\end{aligned}
```

```math
\textbf{Degree 1, l = 1}\\
\begin{aligned}
Y_{1,-1}(θ, φ) &= -\sqrt{\frac{3}{4\pi}} \cdot \frac{y}{r}\\
Y_{1,0}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{z}{r}\\
Y_{1,1}(θ, φ) &= -\sqrt{\frac{3}{4\pi}} \cdot \frac{x}{r}\\\\
\end{aligned}
```

```math
\textbf{Degree 2, l = 2}\\
\begin{aligned}
Y_{2,-2}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{xy}{r^2}\\
Y_{2,-1}(θ, φ) &= -\frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot -\frac{yz}{r^2}\\
Y_{2,0}(θ, φ) &= \frac{1}{4} \sqrt{\frac{5}{\pi}} \cdot \frac{3z^2 - r^2}{r^2}\\
Y_{2,1}(θ, φ) &= -\frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot -\frac{xz}{r^2}\\
Y_{2,2}(θ, φ) &= \frac{1}{4} \sqrt{\frac{15}{\pi}} \cdot \frac{x^2 - y^2}{r^2}\\\\
\end{aligned}
```

```math
\textbf{Degree 3, l = 3}\\
\begin{aligned}
Y_{3,-3}(θ, φ) &= -\frac{1}{4} \sqrt{\frac{35}{2\pi}} \cdot \frac{y(3x^2 - y^2)}{r^3}\\
Y_{3,-2}(θ, φ) &= \frac{1}{2} \sqrt{\frac{105}{\pi}} \cdot \frac{xyz}{r^3}\\
Y_{3,-1}(θ, φ) &= -\frac{1}{4} \sqrt{\frac{21}{2\pi}} \cdot \frac{y(5z^2 - r^2)}{r^3}\\
Y_{3,0}(θ, φ) &= \frac{1}{4} \sqrt{\frac{7}{\pi}} \cdot \frac{z(5z^2 - 3r^2)}{r^3}\\
Y_{3,1}(θ, φ) &= -\frac{1}{4} \sqrt{\frac{21}{2\pi}} \cdot \frac{x(5z^2 - r^2)}{r^3}\\
Y_{3,2}(θ, φ) &= \frac{1}{4} \sqrt{\frac{105}{\pi}} \cdot \frac{z(x^2 - y^2)}{r^3}\\
Y_{3,3}(θ, φ) &= -\frac{1}{4} \sqrt{\frac{35}{2\pi}} \cdot \frac{x(x^2 - 3y^2)}{r^3}\\
\end{aligned}
```

### Table of Constants

| Used In | Expression | Approximate Constant |
| --- | --- | --- |
| $Y_{0,0}(θ, φ)$ | $\frac{1}{2} \sqrt{\frac{1}{\pi}}$ | $0.2820947917738781$ |
| $Y_{1,-1}(θ, φ)$<br>$Y_{1,1}(θ, φ)$ | $\sqrt{\frac{3}{4\pi}}$ | $-0.4886025119029199$ |
| $Y_{1,0}(θ, φ)$ | $\sqrt{\frac{3}{4\pi}}$ | $0.4886025119029199$ |
| $Y_{2,-2}(θ, φ)$ | $\frac{1}{2} \sqrt{\frac{15}{\pi}}$ | $1.092548430592079$ |
| $Y_{2,-1}(θ, φ)$<br>$Y_{2,1}(θ, φ)$ | $-\frac{1}{2} \sqrt{\frac{15}{\pi}}$ | $-1.092548430592079$ |
| $Y_{2,0}(θ, φ)$ | $\frac{1}{4} \sqrt{\frac{5}{\pi}}$ | $0.3153915652525200$ |
| $Y_{2,2}(θ, φ)$ | $\frac{1}{4} \sqrt{\frac{15}{\pi}}$ | $0.5462742152960395$ |
| $Y_{3,-3}(θ, φ)$<br>$Y_{3,3}(θ, φ)$ | $-\frac{1}{4} \sqrt{\frac{35}{2\pi}}$ | $-0.5900435899266435$ |
| $Y_{3,-2}(θ, φ)$<br>$Y_{3,2}(θ, φ)$ | $\frac{1}{2} \sqrt{\frac{105}{\pi}}$ | 2.890611442640554 |
| $Y_{3,-1}(θ, φ)$<br>$Y_{3,1}(θ, φ)$ | $-\frac{1}{4} \sqrt{\frac{21}{2\pi}}$ | $-0.4570457994644657$ |
| $Y_{3,0}(θ, φ)$ | $\frac{1}{4} \sqrt{\frac{7}{\pi}}$ | $0.3731763325901154$ |
| $Y_{3,2}(θ, φ)$ | $\frac{1}{4} \sqrt{\frac{105}{\pi}}$ | $1.445305721320277$ |

## Known Implementations

_TODO: Add known implementations before final ratification._

## Resources

- [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/)
- [Condon–Shortley phase](https://mathworld.wolfram.com/Condon-ShortleyPhase.html)
- [ASWF Color Space Encodings for Displays](https://github.com/AcademySoftwareFoundation/ColorInterop/blob/main/Recommendations/02_DisplayColorSpaces/DisplayColorSpaces.md)

## Full Khronos Copyright Statement

Copyright 2026 The Khronos Group Inc.

This Specification is protected by copyright laws and contains material proprietary
to Khronos. Except as described by these terms, it or any components
may not be reproduced, republished, distributed, transmitted, displayed, broadcast
or otherwise exploited in any manner without the express prior written permission
of Khronos.

Khronos grants a conditional copyright license to use and reproduce the unmodified
Specification for any purpose, without fee or royalty, EXCEPT no licenses to any patent,
trademark or other intellectual property rights are granted under these terms.

Khronos makes no, and expressly disclaims any, representations or warranties,
express or implied, regarding this Specification, including, without limitation:
merchantability, fitness for a particular purpose, non-infringement of any
intellectual property, correctness, accuracy, completeness, timeliness, and
reliability. Under no circumstances will Khronos, or any of its Promoters,
Contributors or Members, or their respective partners, officers, directors,
employees, agents or representatives be liable for any damages, whether direct,
indirect, special or consequential damages for lost revenues, lost profits, or
otherwise, arising from or in connection with these materials.

This specification has been created under the Khronos Intellectual Property Rights
Policy, which is Attachment A of the Khronos Group Membership Agreement available at
https://www.khronos.org/files/member_agreement.pdf. Khronos grants a conditional
copyright license to use and reproduce the unmodified specification for any purpose,
without fee or royalty, EXCEPT no licenses to any patent, trademark or other
intellectual property rights are granted under these terms. Parties desiring to
implement the specification and make use of Khronos trademarks in relation to that
implementation, and receive reciprocal patent license protection under the Khronos
IP Policy must become Adopters and confirm the implementation as conformant under
the process defined by Khronos for this specification;
see https://www.khronos.org/conformance/adopters/file-format-adopter-program.

Where this Specification identifies specific sections of external references, only those
specifically identified sections define normative functionality. The Khronos Intellectual
Property Rights Policy excludes external references to materials and associated enabling
technology not created by Khronos from the Scope of this Specification, and any licenses
that may be required to implement such referenced materials and associated technologies
must be obtained separately and may involve royalty payments.

Khronos® is a registered trademark, and glTF™ is a trademark of The Khronos Group Inc. All
other product names, trademarks, and/or company names are used solely for identification
and belong to their respective owners.
