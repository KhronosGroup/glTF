<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF 2.0 Extension Registry

## Ratified Khronos Extensions

The following extensions have been ratified by the Khronos Group:

* [KHR_animation_pointer](2.0/Khronos/KHR_animation_pointer/README.md)
* [KHR_draco_mesh_compression](2.0/Khronos/KHR_draco_mesh_compression/README.md)
* [KHR_lights_punctual](2.0/Khronos/KHR_lights_punctual/README.md)
* [KHR_materials_anisotropy](2.0/Khronos/KHR_materials_anisotropy/README.md)
* [KHR_materials_clearcoat](2.0/Khronos/KHR_materials_clearcoat/README.md)
* [KHR_materials_dispersion](2.0/Khronos/KHR_materials_dispersion/README.md)
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
* [EXT_mesh_gpu_instancing](2.0/Vendor/EXT_mesh_gpu_instancing/README.md)
* [EXT_meshopt_compression](2.0/Vendor/EXT_meshopt_compression/README.md)
* [EXT_texture_webp](2.0/Vendor/EXT_texture_webp/README.md)

Khronos extensions use the reserved `KHR` prefix. Once ratified by the Khronos Group, they are covered by the Khronos IP framework.  Extensions that are intended to be ratified can also use the `KHR` prefix to avoid name/code/version thrashing. Khronos members can submit an extension for ratification, which is then voted on by the Khronos Board of Promoters.

## In-progress Khronos and Multi-Vendor Extensions and Projects

This section tracks the status of extensions and projects that are in development and is intended to provide the glTF community with a general sense of the priorities and direction of the glTF roadmap. The items listed are felt to show enough consensus to be considered for future ratification.  We welcome community feedback and encourage everyone to add comments to the Github Pull Requests.  All items are listed in alphabetical order. There is no order of priority of items with the same status.


| Extensions | Status | Pull Request |
|------------|--------|--------------|
| KHR_accessor_float64 | Review Draft | [PR #2397](https://github.com/KhronosGroup/glTF/pull/2397) |
| KHR_audio_graph | Proposal | [PR #2421](https://github.com/KhronosGroup/glTF/pull/2421) |
| KHR_collision_shapes | Review Draft | [PR #2370](https://github.com/KhronosGroup/glTF/pull/2370) |
| KHR_interactivity | Review Draft | [PR #2293](https://github.com/KhronosGroup/glTF/pull/2293) |
| KHR_materials_diffuse_transmission | Release Candidate | [Specification](2.0/Khronos/KHR_materials_diffuse_transmission/README.md) |
| KHR_materials_subsurface | Initial Draft | [PR #1928](https://github.com/KhronosGroup/glTF/pull/1928) |
| KHR_node_hoverability | Review Draft | [PR #2426](https://github.com/KhronosGroup/glTF/pull/2426) |
| KHR_node_selectability | Review Draft | [PR #2422](https://github.com/KhronosGroup/glTF/pull/2422) |
| KHR_node_visibility | Review Draft | [PR #2410](https://github.com/KhronosGroup/glTF/pull/2410) |
| KHR_physics_rigid_bodies | Review Draft | [PR #2424](https://github.com/KhronosGroup/glTF/pull/2424) |
| KHR_texture_procedurals | Initial Draft | [PR #2381](https://github.com/KhronosGroup/glTF/pull/2381) |
| EXT_texture_procedurals_mx_1_39 | Review Draft | [PR #2381](https://github.com/KhronosGroup/glTF/pull/2381) |
| KHR_texture_video | Initial Draft | [PR #2285](https://github.com/KhronosGroup/glTF/pull/2285) |

| Projects | Status | Pull Request |
|----------|--------|--------------|
| glTF External References | Proposal | [Repository](https://github.com/KhronosGroup/glTF-External-Reference/tree/main) |


**How to view the latest draft specifications from a GitHub pull request** - Visit the files section of the Pull Request and find the file: README.md. In the top-right of that file's title bar select the three dots (...) and select ‘View file’.  This will render the latest version of the draft specification.

### Specification Status Explained
The status provides a guide to the current position of a specification through the main stages of extension development, and the community feedback that is requested at each stage.  This is a general guide and the stages and deliverables for an individual extension or project may differ.  Additional information can be found in the read.me file for each extension or project.

| Status | Typical Start and End Deliverables | Community Engagement |
|--------|------------------------------------|----------------------|
| Proposal | This stage starts with the posting of a proposal document for discussion with an emphasis on the motivation and requirements of the desired feature/s rather than a detailed specification. These discussions may lead to an Initial Draft. |
| Initial Draft | During this stage an early draft specification will undergo ongoing development but it will typically not be ready for detailed review | Request for directional feedback rather than a detailed review of the specification. |
| Review Draft | This stage starts with the publication of a well-formed Review Draft of the specification based on the Initial Draft and earlier feedback.  The specification will continue to be developed, including adding any missing JSON schema, initial test assets, etc. At least one third party glTF implementation will be released to enable initial testing. | On-going discussion and detailed review of the Review Draft. |
| Release Candidate | This stage will start with the release of a stable Release Candidate specification based on the Review Draft.  If not already done, the feature will be incorporated into the Khronos Sample Viewer and supported by the Khronos Asset Validator.  A public notice will be published that the specification is ready for final public review, along with a given review period. After this review period the specification will be put forward for ratification. | Final opportunity to provide feedback |
| Ratified | Once ratified by the Khronos board the extension or project will move to the Ratified section of the glTF 2.0 specification. | Report bugs |

### Adding Additional Features to glTF

If you have a requirement for a feature that is not listed as part of a Ratified or In-Progress extensions, please check to see if others have already identified similar requirements by reviewing the existing [Vendor Extensions](https://github.com/KhronosGroup/glTF/tree/main/extensions#vendor-extensions-for-gltf-20), [Multi-Vendor extensions](https://github.com/KhronosGroup/glTF/tree/main/extensions#multi-vendor-extensions-for-gltf-20), [glTF GitHub Issues](https://github.com/KhronosGroup/glTF/issues) and [glTF Pull Requests](https://github.com/KhronosGroup/glTF/pulls). If you do find something on a similar topic we recommend you reach out and collaborate. If nothing matches your requirement we encourage you to reach out to the community for feedback and collaborators.  As a consensus develops please reach out to Khronos to discuss how an extension can be brought into the broader ecosystem, and potentially added to the official Khronos glTF Roadmap.

## Multi-Vendor Extensions

When an extension is implemented by more than one vendor, its name can use the reserved `EXT` prefix. Multi-vendor extensions are typically not covered by the Khronos IP framework, with a few notable exceptions (listed above) that have been through the Khronos ratification process after becoming widely used under the `EXT` prefix.

* [EXT_lights_ies](2.0/Vendor/EXT_lights_ies/README.md)
* [EXT_lights_image_based](2.0/Vendor/EXT_lights_image_based/README.md)
* [EXT_mesh_manifold](2.0/Vendor/EXT_mesh_manifold/README.md)

## Vendor Extensions

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

The list of vendor prefixes is maintained in [Prefixes.md](Prefixes.md).  Any vendor, not just Khronos members, can request an extension prefix by submitting an [issue on GitHub](https://github.com/KhronosGroup/glTF/issues/new) requesting one.  Requests should include:

* The name of the prefix.
* The name of the vendor requesting the prefix.
* The vendor's URL and/or contact information.

## Archived Extensions

_Archived extensions may be useful for reading older glTF files, but they are no longer recommended for creating new files._

* [KHR_materials_pbrSpecularGlossiness](2.0/Archived/KHR_materials_pbrSpecularGlossiness/README.md)
* [KHR_techniques_webgl](2.0/Archived/KHR_techniques_webgl/README.md)
* [KHR_xmp](2.0/Archived/KHR_xmp/README.md)

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
    "KHR_draco_mesh_compression", "VENDOR_physics"
  ],
  "extensionsRequired": [
    "KHR_draco_mesh_compression"
  ]
}
```
This allows an engine to quickly determine if it supports the extensions needed to render the model without inspecting the `extensions` property of all objects.

An extension is considered _required_ if a typical glTF loader would fail to load the asset in the absence of support for that extension.  For example, any mesh compression extension must be listed as _required_ unless an uncompressed fallback mesh is provided with the asset.  Likewise, a texture image format extension must be listed as _required_ unless fallback textures in core formats (JPG, PNG) are also supplied. Typically, PBR or other kinds of material extensions should not be listed in _required_, because the core glTF material can be considered a valid fallback for these kinds of extensions, and such extensions will not cause a conformant glTF loader to fail.

## Creating Extensions

To create a new extension, use the [extension template](Template.md) and open a pull request into this repo.  Make sure to add the extension to the glTF Extension Registry (top of this file).

If the extension adds a new top-level array (by extending the root glTF object), its elements should inherit all properties of `glTFChildOfRootProperty.schema.json`. Other objects introduced by the extension should inherit all properties of `glTFProperty.schema.json`. By glTF 2.0 conventions, schemas should allow additional properties. See [`KHR_lights_punctual`](2.0/Khronos/KHR_lights_punctual) as an example.

If lack of extension support prevents proper geometry loading, extension specification _must_ state that (and such extension must be mentioned in `extensionsRequired` top-level glTF property).

## Naming

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
