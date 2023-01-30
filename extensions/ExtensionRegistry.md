<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF Extension Registry

## Extensions for glTF 2.0

### Ratified Khronos Extensions for glTF 2.0

Khronos extensions use the reserved `KHR` prefix. Once ratified by the Khronos Group, they are covered by the Khronos IP framework. Extensions that are intended to be ratified can also use the `KHR` prefix to avoid name/code/version thrashing. Khronos members can submit an extension for ratification, which is then voted on by the Khronos Board of Promoters.

The following extensions have been ratified by the Khronos Group:

* [KHR_draco_mesh_compression](2.0/Khronos/KHR_draco_mesh_compression/README.md)
* [KHR_lights_punctual](2.0/Khronos/KHR_lights_punctual/README.md)
* [KHR_materials_clearcoat](2.0/Khronos/KHR_materials_clearcoat/README.md)
* [KHR_materials_emissive_strength](2.0/Khronos/KHR_materials_emissive_strength/README.md)
* [KHR_materials_ior](2.0/Khronos/KHR_materials_ior/README.md)
* [KHR_materials_iridescence](2.0/Khronos/KHR_materials_iridescence/README.md)
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
| [KHR_animation_pointer](https://github.com/KhronosGroup/glTF/pull/2147) | Ready for testing. |
| [KHR_audio](https://github.com/KhronosGroup/glTF/pull/2137) | Ready for testing. |
| [KHR_materials_anisotropy](https://github.com/KhronosGroup/glTF/pull/1798) | Ready for testing. |
| [KHR_materials_diffuse_transmission](https://github.com/KhronosGroup/glTF/pull/1825) | Ready for testing. |
| [KHR_materials_sss](https://github.com/KhronosGroup/glTF/pull/1928) | In development. |

## Extensions for glTF 1.0

### Khronos extensions for glTF 1.0

* [KHR_binary_glTF](1.0/Khronos/KHR_binary_glTF/README.md)
* [KHR_materials_common](1.0/Khronos/KHR_materials_common/README.md)

### Vendor extensions for glTF 1.0

* [CESIUM_RTC](1.0/Vendor/CESIUM_RTC/README.md)
* [WEB3D_quantized_attributes](1.0/Vendor/WEB3D_quantized_attributes/README.md)

