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

### Projection

The `projection` property is an optional hint that specifies how the Gaussians should be projected onto the kernel shape. This is typically provided by the training process for the splats. The default value is `perspective`.

Renderers may ignore this property or values they do not support and assume `perspective` based on their implementation's capabilities.

Additional values can be added over time by defining an extension that adds new projection methods.

#### Known Projection Methods

| Projection Method | Description |
| --- | --- |
| perspective | (Default) The typical 3D perspective projection based on scene depth. |
| orthographic | An orthogonal projection of splats into a scene to preserve shape and scale and reduce distortion. |

### Sorting Method

The `sortingMethod` property is an optional hint that specifies how the Gaussian particles should be sorted during the rendering process. This typically is provided by the training process for the splats. The default value is `cameraDistance`.

Renderers may ignore this property or values they do not support and assume `cameraDistance` based on their implementation's capabilities.

Additional values can be added over time by defining an extension that adds new sorting methods.

#### Known Sorting Methods

*This section is non-normative and not comprehensive. It may change over time.*

| Sorting Method | Description |
| --- | --- |
| cameraDistance | (Default) Sort the splats based on the length of the vector from the splat to the camera origin. |
| zDepth | Sort the splats based on the magnitude of the z-component of the splat's position. (e.g. Only along the viewing axis of the camera.) |

### Color Space

The `colorSpace` property is an optional hint that specifies the color space of the 3D Gaussian Splat color & lighting data. The color space is typically determined by the training process for the splats. The default value is `sRGB` to align with the base glTF specification. This color space value only applies to the 3D Gaussian Splatting data and does not affect any other color data in the glTF.

Renderers may ignore this property and assume `sRGB` based on their implementation's capabilities.

Additional values can be added over time by defining an extension that adds new color spaces.

#### Available Color Spaces

| Color Space | Description |
| --- | --- |
| sRGB | (Default) Standard RGB color space. |
| linear | Linear color space. |
| ACEScg | ACEScg color space. |
| DCI-P3 | DCI P3 color space. |
| Rec. 2020 | Rec. 2020 color space. |

Similar to Kernels, color spaces can be added through additional extensions as needed.

## Attributes

Each 3D Gaussian splat has the following attributes. At minimum the attributes must contain `POSITION`, `COLOR_0`, `KHR_gaussian_splatting:ROTATION`, and `KHR_gaussian_splatting:SCALE`. `KHR_gaussian_splatting:SH_DEGREE_ℓ_COEF_n` attributes hold the spherical harmonics data and are not required. `POSITION` and `COLOR_0` are defined by the base glTF specification. If higher degrees of Spherical Harmonics are used then lower degrees are required implicitly.

| Splat Data | glTF Attribute | Accessor Type | Component Type | Required | Notes |
| --- | --- | --- | --- | --- | --- |
| Rotation | KHR_gaussian_splatting:ROTATION | VEC4 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | yes | Rotation is a quaternion with `w` as the scalar. (xyzw) |
| Scale | KHR_gaussian_splatting:SCALE | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | yes | |
| Spherical Harmonics degree 1 | KHR_gaussian_splatting:SH_DEGREE_1_COEF_n (n = 0 to 2) | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | no (yes if degree 2 or 3 are used) | |
| Spherical Harmonics degree 2 | KHR_gaussian_splatting:SH_DEGREE_2_COEF_n (n = 0 to 4) | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | no (yes if degree 3 is used) | |
| Spherical Harmonics degree 3 | KHR_gaussian_splatting:SH_DEGREE_3_COEF_n (n = 0 to 6) | VEC3 | _float_ <br/>_signed byte_ normalized <br/>_signed short_ normalized | no | |

The `KHR_gaussian_splatting:ROTATION` and `KHR_gaussian_splatting:SCALE` attributes support quantized storage using normalized signed `byte` or `short` component types to reduce file size. If quantization is not needed, content creators should use the `float` component type for maximum precision.

The value of `COLOR_0` is derived by multiplying the 3 diffuse color components of the 3D Gaussian splat with the constant zeroth-order Spherical Harmonic (ℓ = 0) for the RGB channels. The alpha channel should contain the opacity of the splat.

Each increasing degree of spherical harmonics requires more coeffecients. At the 1st degree, 3 sets of coeffcients are required, increasing to 5 sets for the 2nd degree, and increasing to 7 sets at the 3rd degree. With all 3 degrees, this results in 45 spherical harmonic coefficients stored in the `_SH_DEGREE_ℓ_COEF_n` attributes.

## Known Implementations

This is currently implemented within [3D Tiles and CesiumJS as an experimental feature](https://cesium.com/learn/cesiumjs/ref-doc/Cesium3DTileset.html?classFilter=3D).

## Resources

- [3D Gaussian Splatting for Real-Time Radiance Field Rendering](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/)

