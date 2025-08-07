<!--
Copyright 2015-2025 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF 2.0 Extension Development

Extensions are an important element of the glTF ecosystem. They can extend the core glTF model format with new properties and functionalities. If you have a requirement for a feature that is not supported by the core glTF specification or an existing extension, you can consider proposing a new extension to support this feature. Before you propose a new extension, please check to see if others have already identified similar requirements by reviewing the existing extensions in the [Extensions Registry](https://github.com/KhronosGroup/glTF/tree/main/extensions/README.md) and extension proposals in the [issues](https://github.com/KhronosGroup/glTF/issues) and [pull requests](https://github.com/KhronosGroup/glTF/pulls). 

> [!NOTE]
> Extensions vs. Extras
>
> Extensions aim at providing a standardized, agreed-upon way to extend the existing glTF model format. But they are not the only way to extend glTF. All glTF object properties allow adding new properties to an `extras` object sub-property, e.g.,
> ```json
> {
>   "asset": {
>     "version": 2.0,
>     "extras": {
>       "guid": "9abb92a3-39cf-4986-a758-c43d4bb4ab58",
>     }
>   }
> }
> ```
> This enables glTF models to contain application-specific properties without creating a full glTF extension. If your use-case is very application-specific, the additional properties are only supposed to be used in a custom, closed, vendor-specific environment, and an extension would not be widely adopted, this may be a preferable solution.

The processes and technical requirements for developing extensions are described in the following sections. 

## Extension Development Process Overview

The following table is a short summary of the stages in the extension development process:

| Status | Typical Start and End Deliverables | Community Engagement |
|--------|------------------------------------|----------------------|
| Proposal | This stage starts with the posting of a proposal for discussion. This can often happen by opening an [issue](https://github.com/KhronosGroup/glTF/issues/new) that explains the goal of the extension and a clear description of the desired functionality. This will usually not yet contain detailed technical specification. Instead, it should focus on the motivation behind the extension and possible use-cases, and explain why the goals cannot be accomplished with the core specification or existing extensions. When the scope and goals of the extension are clear, an Initial Draft can be created. | Identify possible collaborators and similar use-cases
| Initial Draft | An initial draft of the extension specification will be created. This usually happens in form of a [pull request](https://github.com/KhronosGroup/glTF/pulls) that is marked as 'Draft'. Most of the actual development will happen in this phase. The technical requirements that are expected from such a pull request will be described below. | Request for directional feedback rather than a detailed review of the specification. |
| Review Draft | This stage starts with the specification that was developed based on the Initial Draft and earlier feedback. This usually means that the initial 'Draft' pull request is marked as 'Ready For Review'. The specification will continue to be developed, based on the review feedback. At least one third party glTF implementation will be released to enable initial testing. | On-going discussion and detailed review of the Review Draft. |
| Release Candidate | This stage will start with the release of a stable Release Candidate specification based on the Review Draft. If not already done, the feature will be incorporated into the Khronos Sample Viewer and supported by the Khronos Asset Validator. A public notice will be published that the specification is ready for final public review, along with a given review period. | Final opportunity to provide feedback |
| Complete | The extension specification will be marked as 'Complete', the pull request will be merged, and it will be listed in the Extensions Registry | None |
| Ratified | Extensions that gained considerable adoption and turned out to be stable and widely supported can undergo a ratification process. Khronos members can submit an extension for ratification, which is then voted on by the Khronos Board of Promoters. When the extension is ratified, it will be covered by the Khronos IP framework. The extension will be marked as 'Ratified' and listed as a ratified extension the Extensions Registry. | Report bugs |

## Extension Development Process

In order to ensure high quality, robustness, and interoperability of the extensions that are hosted in the Khronos glTF repository, some guidelines have to to followed when proposing and developing an extension specification. The following sections summarize the technical requirements and expectations for extension specifications.

### Extension Names

The names of extensions have to follow a certain pattern. 

1. Names MUST begin with an UPPERCASE prefix (details below), followed by an underscore. 
2. Names MUST use lowercase snake-case following the prefix, e.g. `KHR_materials_unlit`.
3. Names SHOULD be structured as `<PREFIX>_<scope>_<feature>`, where *scope* is an existing glTF concept (e.g. mesh, texture, image) or a new (top-level) concept that is introduced by the extension, and *feature* describes the functionality being added within that scope. This structure is required for cases where it is applicable, but not required for cases where an extension can not be assigned to a specific scope.
4. Scope SHOULD be singular (e.g. mesh, texture), except where this would be inconsistent with an existing Khronos extension (e.g. materials, lights).
5. Names SHOULD NOT include special characters, except for the `_` underscore separation character. For portability and consistency, the characters SHOULD only be alphanumeric (ASCII) characters.
6. Names MUST NOT include a `:` colon, because this is used for attribute name disambiguation.

> [!NOTE]
> For historical reasons, older extensions may not follow these guidelines. Future extensions should do so.


#### Extension Name Prefixes

Khronos extensions use the reserved `KHR` prefix. These are proposed by Khronos Group members, and intended to become ratified extensions, meaning that they are covered by the Khronos IP framework.

The `EXT` prefix can be used for extensions that are expected to be supported by multiple vendors, but are not expected to be ratified by the Khronos group.

Vendor-specific extensions use one of the vendor prefixes that are maintained in [Prefixes.md](Prefixes.md). Any vendor, not just Khronos members, can request an extension prefix by submitting an [issue on GitHub](https://github.com/KhronosGroup/glTF/issues/new) requesting one. Requests should include:

- The name of the prefix.
- The name of the vendor requesting the prefix.
- The vendor's URL and/or contact information.


### Extension Mechanics

All glTF object properties (see [glTFProperty.schema.json](../specification/2.0/schema/glTFProperty.schema.json)) have an optional `extensions` object property that can contain new extension-specific properties. This allows extensions to extend any part of glTF, including geometry, materials, animations, etc. Extensions can also introduce new parameter semantics, reserved IDs, and new formats containing glTF.

Extensions may add new properties and values, such as attribute semantics or texture media types. Extensions can not remove existing glTF properties or redefine existing glTF properties to mean something else.

#### New properties

Extensions can add new properties to existing glTF objects. For example, `KHR_texture_transform` introduces a set of texture transformation properties:
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

#### New attributes

Extensions can introduce new vertex attributes for mesh primitives. In order to disambiguate the names of vertex attributes that are defined by different extensions, the attribute names MUST be prefixed by the full, case-sensitive name of the extension, followed by a `:` colon. For example, an extension that is called `EXT_example_extension` and that defines vertex attributes for a 'temperature' and a 'velocity' may call them `EXT_example_extension:TEMPERATURE` and `EXT_example_extension:VELOCITY`: 
```json
{
  "meshes" : [
    {
      "primitives" : [
        {
          "attributes" : {
            "POSITION" : 0,
            "COLOR_0" : 1,
            "EXT_example_extension:TEMPERATURE" : 2,
            "EXT_example_extension:VELOCITY" : 3
          },
          "mode" : 0
        }
      ]
    }
  ],
}
```

#### Extension declarations

All extensions used in a glTF asset are listed as strings in the top-level `extensionsUsed` array; all _required_ extensions are listed as strings in the top-level `extensionsRequired` array, e.g.,
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
This allows an engine to quickly determine if it supports the extensions needed to render the glTF asset without inspecting the `extensions` property of all objects.

An extension is considered _required_ if a typical glTF loader would fail to load the asset in the absence of support for that extension.  For example, any mesh compression extension must be listed as _required_ unless an uncompressed fallback mesh is provided with the asset.  Likewise, a texture image format extension must be listed as _required_ unless fallback textures in core formats (JPG, PNG) are also supplied. Typically, PBR or other kinds of material extensions should not be listed in _required_, because the core glTF material can be considered a valid fallback for these kinds of extensions, and such extensions will not cause a conformant glTF loader to fail.

## Creating Extensions

When an Extension Proposal has been discussed, and all collaborators agree on the basic goals and scope of the extension, then an Initial Draft of the extension specification can be created. This is still an early stage, and will usually happen in form of a draft pull request. The pull request adds the main specification text, as a file `/extensions/2.0/<extension name>/README.md` in the glTF repository. 

In the course of the development, the following elements are expected to be added to this pull request:

- A well-written, complete specification text in the main `README.md` file. 
  - The [extension template](Template.md) provides the outline for this file.
  - The specification text uses clear, normative language to describe how clients are expected to interpret the presence of the extension.
- A complete JSON schema for the elements that are added to the glTF JSON. 
  - The schema files are located in the `/schema` subdirectory of the extension specification. 
  - The schema files use JSON schema version 'draft-04'.
  - The naming of the schema files follows the conventions of the JSON schema.
  - If the extension adds a new top-level array (by extending the root glTF object), its elements inherit all properties of `glTFChildOfRootProperty.schema.json`. 
  - Other objects introduced by the extension inherit all properties of `glTFProperty.schema.json`. 
  - By glTF 2.0 conventions, schemas should allow additional properties. 
  
  For an example on how to structure the JSON schema, see [`KHR_lights_punctual`](2.0/Khronos/KHR_lights_punctual)

As shown in the extension template: The specification should list and link to known implementations of the extension.

If possible and applicable, example assets that include the extension should be provided. These will _not_ be part of the specification itself, but they can be added by appending them to comments in the pull request discussion. These assets can be used by implementors for testing and validating their implementation. When the extension is complete, these example assets may be added via pull requests to the [glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets) repository.
