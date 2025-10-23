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
- Michael Nikelsky, Autodesk

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

## Lighting

At the time of writing, the most common method for lighting 3D Gaussian splats is via spherical harmonics. This extension defines attributes to store spherical harmonic coefficients for each splat. The zeroth-order spherical harmonic coefficients are always required. Higher order coefficients are optional.

These rules may be relaxed by future extensions that define alternative lighting methods or have specific requirements for handling compression, such as when a compression method stores the diffuse color components as linear color values instead of the zeroth-order coefficients.

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
        "indices": 8,
        "extensions": {
            "KHR_gaussian_splatting": {
                "kernel": "ellipse",
                "colorSpace": "BT.709",
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

A 2D `ellipse` kernel type is often used to represent 3D Gaussian splats in an ellipsoid shape. This simple type contains no parameters. This is the shape used by the reference renderer implementations for 3D Gaussian splatting. Following the original reference implementation this kernel assumes a 3σ cut-off (Mahalanobis distance of 3 units) for correct rendering.

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

### Color Space

The `colorSpace` property is an optional property that specifies the color space of the 3D Gaussian Splat when spherical harmonics are being used for the lighting. The color space is typically determined by the training process for the splats. The default value is `BT.709` to align with the base glTF specification. This color space value only applies to the 3D Gaussian splatting data and does not affect any other color data in the glTF.

Renderers may ignore this property and assume `BT.709` based on their implementation's capabilities. When a renderer does not support the specified color space, it may clamp to the `BT.709` color space, but this may cause quality issues for the 3D Gaussian splatting data.

Additional values can be added over time by defining an extension that adds new color spaces.

#### Available Color Spaces

| Color Space | Description |
| --- | --- |
| BT.709-srgb | BT.709 color space. (Default) |
| BT.709-linear | BT.709 linear color space. |
| BT.2020-itu | BT.2020-ITU color space. |
| BT.2020-hlg | BT.2020 HLG (Hybrid Log-Gamma) color space. |
| BT.2020-pq | BT.2020 PQ (Perceptual Quantizer) color space. |
| BT.2020-linear | BT.2020 linear color space. |
| Display-P3 | Display P3 color space. |

### Projection

The `projection` property is an optional property that specifies how the Gaussians should be projected onto the kernel shape. This is typically provided by the training process for the splats. The default value is `perspective`.

Renderers may ignore this property or values they do not support and assume `perspective` based on their implementation's capabilities.

Additional values can be added over time by defining an extension that adds new projection methods.

#### Known Projection Methods

| Projection Method | Description |
| --- | --- |
| perspective | (Default) The typical 3D perspective projection based on scene depth. |
| orthographic | An orthogonal projection of splats into a scene to preserve shape and scale and reduce distortion. |

### Sorting Method

The `sortingMethod` property is an optional property that specifies how the Gaussian particles should be sorted during the rendering process. This typically is provided by the training process for the splats. The default value is `cameraDistance`.

Renderers may ignore this property or values they do not support and assume `cameraDistance` based on their implementation's capabilities.

Additional values can be added over time by defining an extension that adds new sorting methods.

#### Known Sorting Methods

*This section is non-normative and not comprehensive. It may change over time.*

| Sorting Method | Description |
| --- | --- |
| cameraDistance | (Default) Sort the splats based on the length of the vector from the splat to the camera origin. |
| zDepth | Sort the splats based on the magnitude of the z-component of the splat's position. (e.g. Only along the viewing axis of the camera.) |

## Attributes

| Splat Data | glTF Attribute | Accessor Type | Component Type | Required | Notes |
| --- | --- | --- | --- | --- | --- |
| Rotation | KHR_gaussian_splatting:ROTATION | VEC4 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | yes | Rotation is a quaternion with `w` as the scalar. (xyzw) |
| Scale | KHR_gaussian_splatting:SCALE | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | yes | |
| Opacity | KHR_gaussian_splatting:OPACITY | SCALAR | _float_ <br/>_unsigned byte_ normalized <br/>_unsigned short_ normalized | yes | |
| Spherical Harmonics degree 0 | KHR_gaussian_splatting:SH_DEGREE_0_COEF_0 | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | yes (unless using a different method for lighting) | |
| Spherical Harmonics degree 1 | KHR_gaussian_splatting:SH_DEGREE_1_COEF_n (n = 0 to 2) | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | no (yes if degree 2 or 3 are used) | |
| Spherical Harmonics degree 2 | KHR_gaussian_splatting:SH_DEGREE_2_COEF_n (n = 0 to 4) | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | no (yes if degree 3 is used) | |
| Spherical Harmonics degree 3 | KHR_gaussian_splatting:SH_DEGREE_3_COEF_n (n = 0 to 6) | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | no | |

### Basic Attributes

Each 3D Gaussian splat has the following attributes. At minimum the attributes must contain `POSITION`, `KHR_gaussian_splatting:ROTATION`, `KHR_gaussian_splatting:SCALE`, and `KHR_gaussian_splatting:OPACITY`. `POSITION` is defined by the base glTF specification.

The `KHR_gaussian_splatting:ROTATION` and `KHR_gaussian_splatting:SCALE` attributes support quantized storage using normalized signed `byte` or `short` component types to reduce file size. If quantization is not needed, content creators should use the `float` component type for maximum precision.

### Spherical Harmonics Attributes

When spherical harmonics are being used for lighting, the coefficients for the diffuse component must be provided using the `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` attribute semantic. The zero-order spherical harmonic coefficients are always required to allow for properly handling cases where the diffuse color is not in the _BT.709_ color gamut. The `KHR_gaussian_splatting:SH_DEGREE_ℓ_COEF_n` attributes where ℓ > 0 hold the higher degrees of spherical harmonics data and are not required. If higher degrees of spherical harmonics are used then lower degrees are required implicitly.

Each increasing degree of spherical harmonics requires more coeffecients. At the 1st degree, 3 sets of coeffcients are required, increasing to 5 sets for the 2nd degree, and increasing to 7 sets at the 3rd degree. With all 3 degrees, this results in 45 spherical harmonic coefficients stored in the `_SH_DEGREE_ℓ_COEF_n` attributes.

Spherical harmonic data is packed in an (r, g, b) format within the VEC3 accessor type. Each coefficient contains 3 values representing the red, green, and blue channels of the spherical harmonic coefficient. Spherical harmonic degrees cannot be partially defined. For example, if any degree 2 spherical harmonics attribute sematnics are used, then all degree 2 and degree 1 spherical harmonic coefficients must be provided.

### Improving Fallback with COLOR_0

To support better fallback functionality, the `COLOR_0` attribute semantic from the base glTF specification may be used to provide the diffuse color of the 3D Gaussian splat. This allows renderers to color the points in the sparse point cloud when 3D Gaussian splatting is not supported by a renderer. The value of `COLOR_0` is derived by multiplying the 3 diffuse color components of the 3D Gaussian splat with the constant zeroth-order spherical harmonic (ℓ = 0) for the RGB channels. The alpha channel should contain the opacity of the splat.

*_Non-normative Note:_* If the spherical harmonics are in the BT.709 gamut, the diffuse color can be computed from the `KHR_gaussian_splatting:SH_DEGREE_0_COEF_0` attribute by multiplying each of the RGB components by the constant spherical harmonic value of _0.282095_.

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

- [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/)

