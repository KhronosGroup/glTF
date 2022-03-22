<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF Extension Registry

## Extensions for glTF 2.0

### Ratified Khronos Extensions for glTF 2.0

Khronos extensions use the reserved `KHR` prefix. Once ratified by the Khronos Group, they are covered by the Khronos IP framework.  Extensions that are intended to be ratified can also use the `KHR` prefix to avoid name/code/version thrashing. Khronos members can submit an extension for ratification, which is then voted on by the Khronos Board of Promoters.

The following extensions have been ratified by the Khronos Group:

* [KHR_draco_mesh_compression](2.0/Khronos/KHR_draco_mesh_compression/README.md)
* [KHR_lights_punctual](2.0/Khronos/KHR_lights_punctual/README.md)
* [KHR_materials_clearcoat](2.0/Khronos/KHR_materials_clearcoat/README.md)
* [KHR_materials_ior](2.0/Khronos/KHR_materials_ior/README.md)
* [KHR_materials_sheen](2.0/Khronos/KHR_materials_sheen/README.md)
* [KHR_materials_specular](2.0/Khronos/KHR_materials_specular/README.md)
* [KHR_materials_transmission](2.0/Khronos/KHR_materials_transmission/README.md)
* [KHR_materials_unlit](2.0/Khronos/KHR_materials_unlit/README.md)
* [KHR_materials_variants](2.0/Khronos/KHR_materials_variants/README.md)
* [KHR_materials_volume](2.0/Khronos/KHR_materials_volume/README.md)
* [KHR_mesh_quantization](2.0/Khronos/KHR_mesh_quantization/README.md)
* [KHR_texture_basisu](2.0/Khronos/KHR_texture_basisu/README.md)
* [KHR_texture_transform](2.0/Khronos/KHR_texture_transform/README.md)
* [KHR_xmp_json_ld](2.0/Khronos/KHR_xmp_json_ld/README.md)

### Multi-Vendor Extensions for glTF 2.0

When an extension is implemented by more than one vendor, its name can use the reserved `EXT` prefix. Multi-vendor extensions are not covered by the Khronos IP framework.

* [EXT_lights_image_based](2.0/Vendor/EXT_lights_image_based/README.md)
* [EXT_mesh_gpu_instancing](2.0/Vendor/EXT_mesh_gpu_instancing/README.md)
* [EXT_meshopt_compression](2.0/Vendor/EXT_meshopt_compression/README.md)
* [EXT_texture_webp](2.0/Vendor/EXT_texture_webp/README.md)

### Vendor Extensions for glTF 2.0

The list of vendor prefixes is maintained in [Prefixes.md](Prefixes.md).  Any vendor, not just Khronos members, can request an extension prefix by submitting an [issue on GitHub](https://github.com/KhronosGroup/glTF/issues/new) requesting one.  Requests should include:

* The name of the prefix.
* The name of the vendor requesting the prefix.
* The vendor's URL and/or contact information.

Vendor extensions are not covered by the Khronos IP framework.

* [ADOBE_materials_clearcoat_specular](2.0/Vendor/ADOBE_materials_clearcoat_specular/README.md)
* [ADOBE_materials_thin_transparency](2.0/Vendor/ADOBE_materials_thin_transparency/README.md)
* [AGI_articulations](2.0/Vendor/AGI_articulations/README.md)
* [AGI_stk_metadata](2.0/Vendor/AGI_stk_metadata/README.md)
* [CESIUM_primitive_outline](2.0/Vendor/CESIUM_primitive_outline/README.md)
* [FB_geometry_metadata](2.0/Vendor/FB_geometry_metadata/README.md)
* [MSFT_lod](2.0/Vendor/MSFT_lod/README.md)
* [MSFT_packing_normalRoughnessMetallic](2.0/Vendor/MSFT_packing_normalRoughnessMetallic/README.md)
* [MSFT_packing_occlusionRoughnessMetallic](2.0/Vendor/MSFT_packing_occlusionRoughnessMetallic/README.md)
* [MSFT_texture_dds](2.0/Vendor/MSFT_texture_dds/README.md)

### Archived Extensions for glTF 2.0

_Archived extensions may be useful for reading older glTF files, but they are no longer recommended for creating new files._

* [KHR_materials_pbrSpecularGlossiness](2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md)
* [KHR_techniques_webgl](2.0/Archived/KHR_techniques_webgl/README.md)
* [KHR_xmp](2.0/Archived/KHR_xmp/README.md)

### In-progress Khronos and multi-vendor extensions for glTF 2.0

_Draft Khronos (KHR) extensions are not ratified yet. Multi-vendor (EXT) extensions do not require ratification,
but may still change before they are complete._

This section tracks the status of Khronos and multi-vendor extensions that are either already in development,
or that we feel show enough consensus to be highly likely for future development. We welcome feedback for these
and all other extensions (see [GitHub issues](https://github.com/KhronosGroup/glTF/issues?q=is%3Aopen+is%3Aissue+label%3Aextension)). This list is intended to give a general sense of current priorities and direction.

For features that are not listed here but may be important for different uses, we encourage the community to
begin with vendor extensions (which do not require review), reach out for feedback and collaborators, and as
consensus forms we may consider the best way to bring a vendor extension into the broader ecosystem: via multi-vendor
extensions, Khronos extensions, or inclusion in a future version of the glTF specification.

| Extension | Status |
|-----------|--------|
| [KHR_materials_emissive_strength](https://github.com/KhronosGroup/glTF/pull/1994) | Ready for testing. |
| [KHR_materials_iridescence](https://github.com/KhronosGroup/glTF/pull/2027) | Ready for testing. |
| [KHR_materials_anisotropy](https://github.com/KhronosGroup/glTF/pull/1798) | Ready for testing. |
| [KHR_materials_translucency](https://github.com/KhronosGroup/glTF/pull/1825) | In development. |
| [KHR_materials_sss](https://github.com/KhronosGroup/glTF/pull/1928) | In development. |
| [KHR_animation2](https://github.com/KhronosGroup/glTF/pull/2033) | In development. |

## Extensions for glTF 1.0

### Khronos extensions for glTF 1.0

* [KHR_binary_glTF](1.0/Khronos/KHR_binary_glTF/README.md)
* [KHR_materials_common](1.0/Khronos/KHR_materials_common/README.md)

### Vendor extensions for glTF 1.0

* [CESIUM_RTC](1.0/Vendor/CESIUM_RTC/README.md)
* [WEB3D_quantized_attributes](1.0/Vendor/WEB3D_quantized_attributes/README.md)

# About glTF Extensions

glTF extensions extend the base glTF model format.  Extensions can introduce new properties (including properties that reference external data, and the extension can define the format of those data), new parameter semantics, reserved IDs, and new container formats.  Extensions are written against a specific version of glTF and may be promoted to core glTF in a later glTF version.

## Extension Mechanics

All glTF object properties (see [glTFProperty.schema.json](../specification/2.0/schema/glTFProperty.schema.json)) have an optional `extensions` object property that can contain new extension-specific properties.  This allows extensions to extend any part of glTF, including geometry, materials, animations, etc.  Extensions can also introduce new parameter semantics, reserved IDs, and new formats containing glTF.

Extensions can't remove existing glTF properties or redefine existing glTF properties to mean something else.

Examples include:
* **New properties**: `KHR_texture_transform` introduces a set of texture transformation properties, e.g.,
```json
{
  "materials": [{
    "emissiveTexture": {
      "index": 0,
      "extensions": {
        "KHR_texture_transform": {
          "offset": [0, 1],
          "rotation": 1.57079632679,
          "scale": [0.5, 0.5]
        }
      }
    }
  }]
}
```
Extensions may add new properties and values, such as attribute semantics or texture mime types. In all Khronos (KHR) extensions, and as best practice for vendor extensions, these feature additions are designed to allow safe fallback consumption in tools that do not recognize an extension in the `extensionsUsed` array.

All extensions used in a model are listed as strings in the top-level `extensionsUsed` array; all _required_ extensions are listed as strings in the top-level `extensionsRequired` array, e.g.,
```json
{
  "extensionsUsed": [
    "KHR_materials_pbrSpecularGlossiness", "VENDOR_physics"
  ],
  "extensionsRequired": [
    "KHR_materials_pbrSpecularGlossiness"
  ]
}
```
This allows an engine to quickly determine if it supports the extensions needed to render the model without inspecting the `extensions` property of all objects.

## Creating Extensions

To create a new extension, use the [extension template](Template.md) and open a pull request into this repo.  Make sure to add the extension to the glTF Extension Registry (top of this file).

If the extension adds a new top-level array (by extending the root glTF object), its elements should inherit all properties of `glTFChildOfRootProperty.schema.json`. Other objects introduced by the extension should inherit all properties of `glTFProperty.schema.json`. By glTF 2.0 conventions, schemas should allow additional properties. See [`KHR_lights_punctual`](2.0/Khronos/KHR_lights_punctual) as an example.

If lack of extension support prevents proper geometry loading, extension specification _must_ state that (and such extension must be mentioned in `extensionsRequired` top-level glTF property).

### Naming

> **NOTE:** For historical reasons, older extensions may not follow these guidelines. Future extensions should do so.

1. Names MUST begin with an UPPERCASE prefix, followed by an underscore. Prefixes are `KHR` for [Khronos extensions](#ratified-khronos-extensions-for-gltf-20), `EXT` for [Multi-vendor extensions](#multi-vendor-extensions-for-gltf-20), and others may be reserved for [Vendor extensions](#vendor-extensions-for-gltf-20).
2. Names MUST use lowercase snake-case following the prefix, e.g. `KHR_materials_unlit`.
3. Names SHOULD be structured as `<PREFIX>_<scope>_<feature>`, where *scope* is an existing glTF concept (e.g. mesh, texture, image) and *feature* describes the functionality being added within that scope. This structure is recommended, but not required.
4. Scope SHOULD be singular (e.g. mesh, texture), except where this would be inconsistent with an existing Khronos extension (e.g. materials, lights).

## Extensions vs. Extras

In addition to extensions, the `extras` object can also be used to extend glTF.  This is completely separate from extensions.

All glTF object properties allow adding new properties to an `extras` object sub-property, e.g.,
```json
{
  "asset": {
    "version": 2.0,
    "extras": {
      "guid": "9abb92a3-39cf-4986-a758-c43d4bb4ab58",
    }
  }
}
```
This enables glTF models to contain application-specific properties without creating a full glTF extension.  This may be preferred for niche use cases where an extension would not be widely adopted.
