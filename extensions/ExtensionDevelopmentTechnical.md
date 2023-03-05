<!--
Copyright 2015-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

---

NOTE: The following contains some old information, but is in the process of being updated.

---

# Extension Development - Technical

The following is a summary of the technical requirements that must be met by an extension proposal:

- It must have a well-written, complete specification text, in Markdown or AsciiDoc format
- It must include a complete JSON schema for the elements that are supposed to be added to the glTF JSON
- There must be at least two independent implementations of the extension 
- There should be sample models that show how the extension is used in practice

## Specification Text

The specification text should describe the purpose, use-cases, goals, and scope of the extension. Technical aspects that are _not_ covered by the JSON Schema are described here. Implementation notes and JSON snippets can be used to show how the extension is supposed to be integrated into assets, and how implementations are supposed to process these additional elements. Figures (screenshots or diagrams) should be used to illustrate technical concepts, where applicable.

The process of developing the specification text can start based on the template that is found at [TODO LINK (The current one should be extended a bit, e.g show how to inline images and snippets - and maybe even migrate it to AsciiDoc right away...)]. 

### Naming

The title of the specification text contains the _name_ of the extension. The extension name is structured as follows:

> **NOTE:** For historical reasons, older extensions may not follow these guidelines. Future extensions should do so.

1. Names MUST begin with an UPPERCASE prefix, followed by an underscore. Prefixes are:
  - `KHR` for Khronos extensions. These are extensions that have been proposed and developed by the Khronos 3D Formats Working Group
  - `EXT` for Multi-vendor extensions. These are extensions that have been developed by multiple vendors, collaboratively
  - Vendor prefixes: Vendors may register their own prefix (see [Prefixes](Prefixes.md)). These prefixes offer a generic form of namespacing, and usually indicate that a certain vendor was the main developer of the extension.
2. Names MUST use lowercase snake-case following the prefix, e.g. `KHR_materials_unlit`.
3. Names SHOULD be structured as `<PREFIX>_<scope>_<feature>`, where *scope* is an existing glTF concept (e.g. mesh, texture, image) and *feature* describes the functionality being added within that scope. This structure is recommended, but not required.
4. Scope SHOULD be singular (e.g. mesh, texture), except where this would be inconsistent with an existing Khronos extension (e.g. materials, lights).

---

TODO: 

Basically describe (most of) the sections of the Template here. Mention the "dependencies", and add the point about `extensionsUsed` and `extensionsRequired` here or in the template.

From diagrams:

- Describe the case that a vendor creates an _Extension Proposal_ out of thin air (how to resolve overlap with other proposals etc)
- Mention the track of "Validator support" and "glTF Sample Viewer" support here?


---

TODO: Really old state:

---


#### Extension Mechanics

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

#### Creating Extensions

To create a new extension, use the [extension template](Template.md) and open a pull request into this repo.  Make sure to add the extension to the glTF Extension Registry (top of this file).

If the extension adds a new top-level array (by extending the root glTF object), its elements should inherit all properties of `glTFChildOfRootProperty.schema.json`. Other objects introduced by the extension should inherit all properties of `glTFProperty.schema.json`. By glTF 2.0 conventions, schemas should allow additional properties. See [`KHR_lights_punctual`](2.0/Khronos/KHR_lights_punctual) as an example.

If lack of extension support prevents proper geometry loading, extension specification _must_ state that (and such extension must be mentioned in `extensionsRequired` top-level glTF property).


#### Extensions vs. Extras

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
