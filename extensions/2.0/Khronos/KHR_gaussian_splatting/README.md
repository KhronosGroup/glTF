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

## Lighting

At the time of writing, the most common method for lighting 3D Gaussian splats is via the real spherical harmonics. This extension defines attributes to store spherical harmonic coefficients for each splat. The zeroth-order spherical harmonic coefficients are always required. Higher order coefficients are optional. Each color channel has a separate coefficient, so for each degree $ℓ$, there are $(2ℓ + 1)$ coefficients, each containing RGB values.

These rules may be relaxed by future extensions that define alternative lighting methods or have specific requirements for handling compression, such as when a compression method stores the diffuse color components as linear color values instead of the zeroth-order coefficients.

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

**Degree 1, ℓ = 1**

$`
\begin{aligned}
Y_{1,-1}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{y}{r}\\
Y_{1,0}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{z}{r}\\
Y_{1,1}(θ, φ) &= \sqrt{\frac{3}{4\pi}} \cdot \frac{x}{r}\\
\end{aligned}
`$

**Degree 2, ℓ = 2**

$`
\begin{aligned}
Y_{2,-2}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{xy}{r^2}\\
Y_{2,-1}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{yz}{r^2}\\
Y_{2,0}(θ, φ) &= \frac{1}{4} \sqrt{\frac{5}{\pi}} \cdot \frac{3z^2 - r^2}{r^2}\\
Y_{2,1}(θ, φ) &= \frac{1}{2} \sqrt{\frac{15}{\pi}} \cdot \frac{xz}{r^2}\\
Y_{2,2}(θ, φ) &= \frac{1}{4} \sqrt{\frac{15}{\pi}} \cdot \frac{x^2 - y^2}{r^2}\\
\end{aligned}
`$

**Degree 3, ℓ = 3**

$`
\begin{aligned}
Y_{3,-3}(θ, φ) &= \frac{1}{4} \sqrt{\frac{35}{2\pi}} \cdot \frac{y(3x^2 - y^2)}{r^3}\\
Y_{3,-2}(θ, φ) &= \frac{1}{2} \sqrt{\frac{105}{\pi}} \cdot \frac{xyz}{r^3}\\
Y_{3,-1}(θ, φ) &= \frac{1}{4} \sqrt{\frac{21}{2\pi}} \cdot \frac{y(5z^2 - r^2)}{r^3}\\
Y_{3,0}(θ, φ) &= \frac{1}{4} \sqrt{\frac{7}{\pi}} \cdot \frac{z(5z^2 - 3r^2)}{r^3}\\
Y_{3,1}(θ, φ) &= \frac{1}{4} \sqrt{\frac{21}{2\pi}} \cdot \frac{x(5z^2 - r^2)}{r^3}\\
Y_{3,2}(θ, φ) &= \frac{1}{4} \sqrt{\frac{105}{\pi}} \cdot \frac{z(x^2 - y^2)}{r^3}\\
Y_{3,3}(θ, φ) &= \frac{1}{4} \sqrt{\frac{35}{2\pi}} \cdot \frac{x(x^2 - 3y^2)}{r^3}\\
\end{aligned}
`$

For all of these functions, $r$ represents the magnitude of the position vector, calculated as $r = \sqrt{x^2 + y^2 + z^2}$. Within 3D Gaussian splatting, normalization is used to ensure that the direction vectors are unit vectors. Therefore, $r$ is equal to $1$ when evaluating the spherical harmonics for lighting calculations.

Other extensions may define alternative lighting methods, have specific requirements for handling compression, or define different spherical harmonics handling.

See [Appendix A: Rendering with the base Ellipse Kernel and Spherical Harmonics](#appendix-a-rendering-with-the-base-ellipse-kernel-and-spherical-harmonics) for more details on how to properly implement the lighting used by this extension.

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

See [Appendix A: Rendering with the base Ellipse Kernel and Spherical Harmonics](#appendix-a-rendering-with-the-base-ellipse-kernel-and-spherical-harmonics) for more details on how to properly implement the ellipse kernel used by this extension.

### Color Space

The `colorSpace` property is a required property that specifies the color space of the 3D Gaussian Splat when spherical harmonics are being used for the lighting. The color space is typically determined by the training process for the splats. This color space value only applies to the 3D Gaussian splatting data and does not affect any other color data in the glTF.

Unless specified otherwise by additional extensions, color space information refers to the reconstructed splat color values, therefore splat reconstruction and alpha blending must be performed on the attribute values as-is, before any color gamut or transfer function conversions.

Additional values can be added over time by defining extensions to add new color spaces. See the section, [Extending the Base Extension](#extending-the-base-extension), for more information.

#### Available Color Spaces

| Color Space | Description |
| --- | --- |
| srgb_rec709_display | BT.709 sRGB (display-referred) color space. |
| lin_rec709_scene | BT.709 linear (scene-referred) color space. |

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

## Appendix A: Rendering with the base Ellipse Kernel and Spherical Harmonics

*This section is non-normative.*

The ellipse kernel defined in `KHR_gaussian_splatting` is based on the kernel and implementation described in the original research paper, [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/). This kernel is defined by projecting a 3D ellipsoid onto a 2D plane using a perspective projection. The ellipsoid is defined by its mean (position), covariance matrix (derived from scale and rotation), and opacity. 

TODO FINISH THIS THING

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

- [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/)
- [Gaussian Splatting Algorithm Details](https://github.com/joeyan/gaussian_splatting/blob/main/MATH.md)

