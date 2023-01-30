<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF Extensions

Extensions are an important element of the glTF ecosystem. They can extend the core glTF model format with new properties and functionalities. 

## Existing glTF Extensions

The [glTF Extension Registry](ExtensionRegistry.md) shows all extensions that are currently tracked in this repository.

## Developing glTF Extensions

Everybody can propose new extensions for glTF. But in order to ensure high quality, robustness, and interoperability of the extensions that are hosted in the Khronos glTF repository, some guidelines have to to followed. 

### Extension Development - Methods

In the following, extensions are roughly classified by their maturity. This description focusses on the _methods_ that are used in the development process. Further details about the _formal_ process (e.g. for ratification) or the _technical_ process (e.g. the proper format for the specification) will be given in later sections.

- **Extension ideas:** This refers to high-level ideas about the _features_ that may be supported by glTF via an extension. 
- **Draft extension proposals:** This refers an early phase of the development of a technical solution for an extension.
- **Extension proposals:** A proposal for an extension that includes a proper technical specification, and is supposed to become an official, complete specification in the Khronos repository.
- **Complete extensions:** An extension that has been marked as complete and final, and is now hosted in the Khronos glTF GitHub repository.
- **Ratified extensions:** An extension that went through the formal process of being ratified by Khronos. More details on that are given in section [TODO LINK].
- **Promoted to core:** An extension that has become part of the glTF core specification (hypothetical)
- **Archived extensions:** An extension that is kept for compatibility reasons, but should no longer be used in new glTF assets.
  
The following sections describe these states in more detail. 

#### Extension Ideas

Extension ideas should be discussed in a dedicated GitHub Issue. 

It is important to discuss these ideas _early_ and _publicly_: Others might have the same idea, and making the idea public makes it easier to gather early feedback, find collaborators, and eventually reach a consensus. The first post of the GitHub issue should therefore outline the idea in a form that enables this sort of feedback: The post has to state a clear, high-level _goal_. It should describe the _domain_ in which the extension will be applied, including possible _use-cases_. It should clearly define the _scope_ and the _limits_ of the extension idea.

On this level, the idea does not yet have to include approaches for a technical _solution_.

#### Draft Extension Proposals

Draft extension proposals should be mainained in GitHub Pull Requests that are marked as _'Draft'_. 

After an extension idea has been discussed, possible collaborators have been identified, and when the idea is interesting for the broader community, a first draft of a technical solution can be created. This process can start based on the template that is found at [TODO LINK]. Contributors and collaborators can provide feedback. Alternatives for technical solutions - and their advantages and disadvantages - are discussed. The goal of this work is to develop a robust technical specification of the extension. The technical requirements for such a specification are described in section [TODO LINK].

#### Extension Proposals

Draft extension proposals should be mainained in GitHub Pull Requests that are marked as _'Ready for Review'_. 

TODO ....

#### Complete Extensions

Complete extensions are extensions that have been merged into the Khronos glTF repository. Their formal status may vary, depending on the ratification state [TODO LINK]

TODO ....


#### Ratified Extensions

Extensions that have been ratified by the Khronos Group, and are covered by the Khronos IP framework.

TODO ....


#### Promoted To Core

Extensions that cover a functionality that has become part of a new glTF version, and therefore does no longer require an extension. This is a hypothetical scenario. There are no plans for major glTF version updates for the near future.

#### Archived Extensions

Extensions that had been Complete or Ratified, but are no longer recommended for creating new glTF assets. These extensions are kept for backward compatibility, and may be required for reading older glTF assets that used these extensions. 



### Extension Development - Formal

TODO Describe the possible ratification process. Differentiate betreen Khronos- and non-Khronos members.


### Extension Development - Technical

TODO Describe the process on a technical level.

Start with the role of prefixes, and guidelines how to register own ones, or when to use `KHR` or `EXT`...

---

NOTE: The following sections contain the old information. This will have to be extended significantly (maybe even moved into its own file)

---

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
