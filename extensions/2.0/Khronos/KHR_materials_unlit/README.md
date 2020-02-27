# KHR\_materials\_unlit

## Contributors

* Don McCurdy, Google, [@donrmccurdy](https://twitter.com/donrmccurdy)
* Norbert Nopper, UX3D, <mailto:nopper@ux3d.io>
* Gary Hsu, Microsoft, [@bghgary](https://twitter.com/bghgary)
* Scott Nagy, Microsoft, [@visageofscott](https://twitter.com/visageofscott)
* Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
* Michael Feldstein, Facebook, [@msfeldstein](https://twitter.com/msfeldstein)
* Robert Long, Mozilla, [@arobertlong](https://twitter.com/arobertlong)

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Exclusions

This extension must not be used on a material that also uses any of the following extensions:

- `KHR_materials_clearcoat`
- `KHR_materials_pbrSpecularGlossiness`

## Overview

This extension defines an unlit shading model for use in glTF 2.0
materials, as an alternative to the Physically Based Rendering (PBR) shading
models provided by the core specification. Three motivating uses cases for
unlit materials include:

* Mobile devices with limited resources, where unlit materials offer a
performant alternative to higher-quality shading models.
* Photogrammetry, in which lighting information is already present and
additional lighting should not be applied.
* Stylized materials (such as "anime" or "hand-drawn" looks) in which lighting is
undesirable for aesthetic reasons.

These use cases are not mutually exclusive: artists may pick an unlit material
for performance reasons, and make aesthetic decisions to complement that
choice. As a result, client implementations capable of rendering PBR should not
automatically "upgrade" to fully-shaded PBR. Any core PBR properties specified
on an unlit material (except `baseColor`) are meant only as fallbacks for
clients that do not support the `KHR_materials_unlit` extension. The extension,
whether required or optional within the asset, indicates preference for an
unlit visual style.

## Extending Materials

The common Unlit material is defined by adding the
`KHR_materials_unlit` extension to any glTF material. When present, the
extension indicates that a material should be unlit and use available
`baseColor` values, alpha values, and vertex colors while ignoring all
properties of the default PBR model related to lighting or color. Alpha
coverage and doubleSided still apply to unlit materials.

```json
{
    "materials": [
        {
            "name": "MyUnlitMaterial",
            "pbrMetallicRoughness": {
                "baseColorFactor": [ 0.5, 0.8, 0.0, 1.0 ]
            },
            "extensions": {
                "KHR_materials_unlit": {}
            }
        }
    ]
}
```

### Definition

The Unlit material model describes a constantly shaded surface that is
independent of lighting. The material is defined only by properties already
present in the [glTF 2.0 material specification](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#material).
No new properties are added by this extension — it is effectively a boolean
flag indicating use of an unlit shading model. Additional properties on the
extension object are allowed, but may lead to undefined behaviour in conforming
viewers.

Color is calculated as:

```
color = <baseColorTerm>
```

`<baseColorTerm>` is the product of `baseColorFactor`, `baseColorTexture`, and vertex color (if any), as defined by the [core glTF material specification](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#metallic-roughness-material).

### Example

The following example defines an unlit material with a constant red color.

```json
"materials": [
    {
        "name": "red_unlit_material",
        "pbrMetallicRoughness": {
            "baseColorFactor": [ 1.0, 0.0, 0.0, 1.0 ]
        },
        "extensions": {
            "KHR_materials_unlit": {}
        }
    }
]
```

### Extension compatibility and fallback materials

When possible, authoring tools should provide a fallback material definition
and mark the `KHR_materials_unlit` extension as optional. Models
including the extension optionally will still render in all clients that
support the core glTF 2.0 specification, falling back to a PBR metal/rough
material. In clients that do not support the extension, direct lighting will
be applied and performance may be degraded.

Defining a fallback to the default PBR shading model is achieved by setting
appropriate parameters on the default material.

```json
"materials": [
    {
        "name": "unlit_with_fallback",
        "pbrMetallicRoughness": {
            "baseColorFactor": [ 1.0, 1.0, 1.0, 1.0 ],
            "baseColorTexture": { "index": 0 },
            "roughnessFactor": 0.9,
            "metallicFactor": 0.0
        },
        "extensions": {
            "KHR_materials_unlit": {}
        }
    }
]
```

> **Implementation Note:** For best fallback behavior in clients that do not
> implement the `KHR_materials_unlit` extension, authoring tools may use:
>
> * `metallicFactor` is `0` and `emissiveFactor` is `[0, 0, 0]`.
> * `roughnessFactor` is greater than `0.5`.
> * Omit `metallicRoughnessTexture`, `occlusionTexture`, `emissiveTexture`,
> and `normalTexture`.

> **Implementation Note:** When `KHR_materials_unlit` is included with another
> extension specifying a shading model on the same material, the result is
> undefined.
