# glTF Extension Registry

## Extensions for glTF 2.0

#### Khronos extensions

* [KHR_materials_pbrSpecularGlossiness](2.0/Khronos/KHR_materials_pbrSpecularGlossiness/README.md)
* [KHR_materials_unlit](2.0/Khronos/KHR_materials_unlit/README.md)
* [KHR_draco_mesh_compression](2.0/Khronos/KHR_draco_mesh_compression/README.md)
* [KHR_texture_transform](2.0/Khronos/KHR_texture_transform/README.md)

#### Vendor extensions

* [AGI_articulations](2.0/Vendor/AGI_articulations/README.md)
* [AGI_stk_metadata](2.0/Vendor/AGI_stk_metadata/README.md)
* [MSFT_lod](2.0/Vendor/MSFT_lod/README.md)
* [MSFT_texture_dds](2.0/Vendor/MSFT_texture_dds/README.md)
* [MSFT_packing_normalRoughnessMetallic](2.0/Vendor/MSFT_packing_normalRoughnessMetallic/README.md)
* [MSFT_packing_occlusionRoughnessMetallic](2.0/Vendor/MSFT_packing_occlusionRoughnessMetallic/README.md)

#### In progress Khronos and multi-vendor extensions

This section tracks the status of Khronos and multi-vendor extensions that are either already in development,
or that we feel show enough consensus to be highly likely for future development. We welcome feedback for these
and all other extensions (see [GitHub issues](https://github.com/KhronosGroup/glTF/issues?q=is%3Aopen+is%3Aissue+label%3Aextension)). This list is intended to give a general sense of
current priorities and direction.

For features that are not listed here but may be important for different uses, we encourage the community to
begin with vendor extensions (which do not require review), reach out for feedback and collaborators, and as
consensus forms we may consider the best way to bring a vendor extension into the broader ecosystem: via multi-vendor
extensions, Khronos extensions, or inclusion in a future version of the glTF specification.

_Draft Khronos (KHR) extensions are not ratified yet. Multi-vendor extensions do not require ratification,
but may still change before they are complete._

| Extension | Status |
|-----------|--------|
| [KHR_techniques_webgl](2.0/Khronos/KHR_techniques_webgl/README.md) | Feature-complete, seeking feedback. |
| [KHR_lights_punctual](https://github.com/KhronosGroup/glTF/pull/1223) | Feature-complete, seeking feedback. |
| KHR_compressed_texture_transmission | In development. |
| [KHR_blend](https://github.com/KhronosGroup/glTF/pull/1302) | In development. |
| [EXT_lights_image_based](https://github.com/KhronosGroup/glTF/pull/1377) | In development. |
| HDR textures ([#1220](https://github.com/KhronosGroup/glTF/issues/1220), [#1365](https://github.com/KhronosGroup/glTF/issues/1365)) | Planned. |
| Advanced PBR materials ([#1221](https://github.com/KhronosGroup/glTF/issues/1221), [#1079](https://github.com/KhronosGroup/glTF/issues/1079), ...) | Planned. |

## Extensions for glTF 1.0

#### Khronos extensions
* [KHR_binary_glTF](1.0/Khronos/KHR_binary_glTF/README.md)
* [KHR_materials_common](1.0/Khronos/KHR_materials_common/README.md)

#### Vendor extensions

* [CESIUM_RTC](1.0/Vendor/CESIUM_RTC/README.md)
* [WEB3D_quantized_attributes](1.0/Vendor/WEB3D_quantized_attributes/README.md)

# About glTF Extensions

glTF extensions extend the base glTF model format.  Extensions can introduce new properties (including properties that reference external data, and the extension can define the format of those data), new parameter semantics, reserved IDs, and new container formats.  Extensions are written against a specific version of glTF and may be promoted to core glTF in a later glTF version.

## Extension Mechanics

All glTF object properties (see [glTFProperty.schema.json](../specification/2.0/schema/glTFProperty.schema.json)) have an optional `extensions` object property that can contain new extension-specific properties.  This allows extensions to extend any part of glTF, including geometry, materials, animations, etc.  Extensions can also introduce new parameter semantics, reserved IDs, and new formats containing glTF.

Extensions can't remove existing glTF properties or redefine existing glTF properties to mean something else.

Examples include:
* **New properties**: `KHR_binary_glTF` introduces a `bufferView` property for shaders, e.g.,
```json
{
  "shaders": {
    "a_shader": {
      "extensions": {
        "KHR_binary_glTF": {
          "bufferView": "a_shader_bufferView"
        }
      }
    }
  }
}
```
* **New parameter semantics**: `CESIUM_RTC` introduces the `CESIUM_RTC_MODELVIEW` semantic.
* **Reserved IDs**: `KHR_binary_glTF` introduces an explicitly named `buffer` called `binary_glTF`.
* **New container formats**: `KHR_binary_glTF` introduces a binary format that contains the glTF JSON and geometry, textures, etc.

All extensions used in a model are listed as strings in the top-level `extensionsUsed` array; all _required_ extensions are listed as strings in the top-level `extensionsRequired` array, e.g.,
```json
{
  "extensionsUsed": [
    "KHR_binary_glTF", "VENDOR_physics"
  ],
  "extensionsRequired": [
    "KHR_binary_glTF"
  ]
}
```
This allows an engine to quickly determine if it supports the extensions needed to render the model without inspecting the the `extensions` property of all objects.

## Creating Extensions

To create a new extension, use the [extension template](Template.md) and open a pull request into this repo.  Make sure to add the extension to the glTF Extension Registry (top of this file).

If lack of extension support prevents proper geometry loading, extension specification _must_ state that (and such extension must be mentioned in `extensionsRequired` top-level glTF property).

Extensions start as a vendor extension, then can become a multi-vendor extensions if there are multiple implementations, and can become a ratified Khronos extension (the multi-vendor extension is an optional step).

## Promoting Extensions

### Vendor Extensions

A list of vendor prefixes is maintained in [Prefixes.md](Prefixes.md).  Any vendor, not just Khronos members, can request an extension prefix by submitting an [issue on GitHub](https://github.com/KhronosGroup/glTF/issues/new) requesting one.  Requests should include:
* The name of the prefix.
* The name of the vendor using the prefix.
* The `extension` GitHub label.

Vendor extension names start with the prefix followed by an underscore, e.g., `CESIUM_`.

Vendor extensions are not covered by the Khronos IP framework.

### Multi-Vendor Extensions

When an extension is implemented by more than one vendor, its name can use the reserved `EXT` prefix.  To promote a vendor extension to a multi-vendor extension, open a pull request labled `extension` that contains a new copy of the extension (even if there aren't any changes) with the new name using the `EXT` prefix, e.g., `KHR_binary_glTF`.

Multi-vendor extensions are not covered by the Khronos IP framework.

### Khronos Extensions

Khronos extensions use the reserved `KHR` prefix and are ratified by Khronos and therefore are covered by the Khronos IP framework.  Extensions that are intended to be ratified can also use the `KHR` prefix to avoid name/code/version thrashing.  Khronos members can submit an extension for ratification, which is then voted on by the Khronos Board of Promoters.

## Extensions vs. Extras

In addition to extensions, the `extras` object can also be used to extend glTF.  This is completely separate from extensions.

All glTF object properties allow adding new properties to an `extras` object sub-property, e.g.,
```json
{
  "shaders": {
    "a-vertex-shader-id": {
      "name": "user-defined-name",
      "uri" : "vertex-shader.glsl",
      "type": 35633,
      "extras" : {
        "precompiled_binary" : "path_to_precompiled_shader"
      }
    }
  }
}
```
This enables glTF models to contain application-specific properties without creating a full glTF extension.  This may be preferred for niche use cases where an extension would not be widely adopted.
