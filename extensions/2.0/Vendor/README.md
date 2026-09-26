<!--
Copyright 2026 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF 2.0 Vendor Extensions

## Historical List

This directory and associated sub-directories contain historical vendor extensions to glTF 2.0. In July 2026 this was superseded by the [glTF-Community Repo](). The new repo has license terms suited to community contributions and allows vendors to specify their own license terms.

This extension list will be maintained as long as Khronos believes it to be important, and at least until 31 December 2026.

## Vendor Extensions

This is a historical list of vendor extensions. All extensions listed here are in the process of moving to the new [glTF-Community](https://github.com/KhronosGroup/glTF-Community/) repo. There will be no additions to this list.


Vendor extensions are not covered by the Khronos IP framework.

* [ADOBE_materials_clearcoat_specular](2.0/Vendor/ADOBE_materials_clearcoat_specular/README.md)
* [ADOBE_materials_clearcoat_tint](2.0/Vendor/ADOBE_materials_clearcoat_tint/README.md)
* [ADOBE_materials_thin_transparency](2.0/Vendor/ADOBE_materials_thin_transparency/README.md)
* [AGI_articulations](2.0/Vendor/AGI_articulations/README.md)
* [AGI_stk_metadata](2.0/Vendor/AGI_stk_metadata/README.md)
* [CESIUM_primitive_outline](2.0/Vendor/CESIUM_primitive_outline/README.md)
* [FB_geometry_metadata](2.0/Vendor/FB_geometry_metadata/README.md)
* [GODOT_single_root](2.0/Vendor/GODOT_single_root/README.md)
* [GRIFFEL_bim_data](2.0/Vendor/GRIFFEL_bim_data/README.md)
* [MPEG_accessor_timed](2.0/Vendor/MPEG_accessor_timed/README.md)
* [MPEG_animation_timing](2.0/Vendor/MPEG_animation_timing/README.md)
* [MPEG_audio_spatial](2.0/Vendor/MPEG_audio_spatial/README.md)
* [MPEG_buffer_circular](2.0/Vendor/MPEG_buffer_circular/README.md)
* [MPEG_media](2.0/Vendor/MPEG_media/README.md)
* [MPEG_mesh_linking](2.0/Vendor/MPEG_mesh_linking/README.md)
* [MPEG_scene_dynamic](2.0/Vendor/MPEG_scene_dynamic/README.md)
* [MPEG_texture_video](2.0/Vendor/MPEG_texture_video/README.md)
* [MPEG_viewport_recommended](2.0/Vendor/MPEG_viewport_recommended/README.md)
* [MSFT_lod](2.0/Vendor/MSFT_lod/README.md)
* [MSFT_packing_normalRoughnessMetallic](2.0/Vendor/MSFT_packing_normalRoughnessMetallic/README.md)
* [MSFT_packing_occlusionRoughnessMetallic](2.0/Vendor/MSFT_packing_occlusionRoughnessMetallic/README.md)
* [MSFT_texture_dds](2.0/Vendor/MSFT_texture_dds/README.md)
* [NV_materials_mdl](2.0/Vendor/NV_materials_mdl/README.md)

The list of vendor prefixes is maintained in the [glTF-Community Prefixes table](https://github.com/KhronosGroup/glTF-Community/blob/main/vendors/PREFIX.md).  See the [instructions](https://github.com/KhronosGroup/glTF-Community/blob/main/vendors/README.md) for requesting a prefix. 

## Archived Extensions

_Archived extensions may be useful for reading older glTF files, but they are no longer recommended for creating new files._

* [KHR_materials_pbrSpecularGlossiness](2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md)
* [KHR_techniques_webgl](2.0/Archived/KHR_techniques_webgl/README.md)
* [KHR_xmp](2.0/Archived/KHR_xmp/README.md)

# About glTF Extensions

glTF extensions extend the base glTF model format.  Extensions can introduce new properties (including properties that reference external data, and the extension can define the format of those data), new parameter semantics, reserved IDs, and new container formats.  Extensions are written against a specific version of glTF and may be promoted to core glTF in a later glTF version.
