# KHR\_materials\_cmnConstant

## Contributors

* Don McCurdy, Google, <mailto:donmccurdy@google.com>

## Status

Draft (not ratified yet)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension defines an unlit ("constant") model based material for use with glTF 2.0 in the same way as the core specification defines Physically Based Rendering (PBR) based materials. In mobile devices with limited resources, unlit materials offer a performant alternative to higher-quality shading models.

This extension defines a single material type, Constant.

## Extending Materials

The common Constant material is defined by adding the `KHR_materials_cmnConstant` extension to any glTF material. When present, the extension indicates that a material should use the Constant shading model. Unrelated default values in the default PBR shading model, such as `diffuseFactor`, should be ignored.

```
{
    "materials": [
        {
            "name": "MyUnlitMaterial",
            "emissiveFactor": [ 0.2, 0.1, 0.0 ],
            "extensions": {
                "KHR_materials_cmnConstant": {}
            }
        }
    ]
}
```

### Constant

The Constant material model describes a constantly shaded surface that is independent of lighting. The material is defined only by properties already
present in the [glTF 2.0 material specification](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#material): no new properties are added by this extension.

Additional properties are allowed, but may lead to undefined behaviour in conforming viewers.

The material is intended for unlit shading. Color is calculated as:

```
color = <emissiveTerm> + <occlusionTerm> * <ambientLight>
```

where

* `emissiveTerm` – emissiveFactor * emissiveTexture
* `occlusionTerm` – occlusionFactor * occlusionTexture
* `ambientLight` – A constant amount of ambient light contribution coming from the scene, i.e. the sum of all ambient light values.

> **Implementation Note**: The Constant material model can also be emulated by setting specific factors to `0` in Blinn-Phong or Lambert shaders. Viewers may optionally take advantage of this to reduce the number of materials they maintain.
