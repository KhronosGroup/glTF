# glTF Extensions Registry

## Multi-vendor extensions

* [EXT_binary_glTF](EXT_binary_glTF/README.md)

## Vendor extensions

* [CESIUM_RTC](CESIUM_RTC/README.md)
* [CESIUM_binary_glTF](CESIUM_binary_glTF/README.md)

## About glTF Extensions

glTF extensions extend the base glTF model format.  Extensions can introduce new properties (including properties that reference external data), new parameter semantics, and new container formats.  Extensions are written against a specific version of glTF and may be promoted to core glTF in a later glTF version. 

### Extension Mechanics

All glTF object properties (see [glTFProperty.schema.json](https://github.com/KhronosGroup/glTF/blob/master/specification/schema/glTFProperty.schema.json)) have an optional `extensions` object property that can contain new extension-specific properties.  This allows extensions to extend any part of glTF, including geometry, materials, animations, etc.  Extensions can also introduce new parameter semantics and reserved ids.

Extensions can't remove existing glTF properties or redefine existing glTF properties to mean something else.

Examples include:
* **New properties**: `EXT_binary_glTF` introduces a `bufferView` property for shaders, e.g.,
```javascript
"a_shader" : {
    "extensions" : {
        "binary_glTF" : {
            "bufferView" : // ...
        }
    }
}
```
* **New parameter semantics**: `CESIUM_RTC` introduces the `CESIUM_RTC_MODELVIEW` semantic.
* **Reserved ids**: `EXT_binary_glTF` introduces an explicitly named `buffer` called `binary_glTF`.

All extensions used in a model are listed as strings in the top-level `allExtensions` array, e.g.,
```javascript
"allExtensions" : [
   "EXT_binary_glTF"
]
```
This allows an engine to quickly determine if it supports the extensions needed to render the model without inspecting the the `extensions` property of all objects.

### Creating Extensions

**TODO: Link to a template extension**

### Promoting Extensions

#### Vendor Extensions

A list of vendor prefixes is maintained in [Prefixes.md](Prefixes.md).  Any vendor, not just Khronos members, can request an extension prefix by submitting an [issue on GitHub](https://github.com/KhronosGroup/glTF/issues/) requesting one.  Requests should include:
* The name of the requested prefix.
* The name of the vendor using the prefix.
* The `extension` GitHub label.
Vendor extension names start with the prefix followed by an underscore, e.g., `CESIUM_`.

To include a vendor extension in the glTF Extensions Registry (this file), add the extension to the top part of this file and open a pull request.

Vendor extensions are not yet (or sometimes ever) ratified by Khronos and therefore are not covered by the Khronos IP framework.

#### Multi-Vendor Extensions

When an extension is implemented by more than one vendor, its name can use the reserved `EXT` prefix.  To promote a vendor extension to a multi-vendor extension, open a request labled `extension` that contains a new copy of the extension (even if there aren't any changes) with the new name, e.g., `CESIUM_binary_glTF` became `EXT_binary_glTF` (which included changes).

Multi-vendor extensions are not ratified by Khronos and therefore are not covered by the Khronos IP framework.

#### Khronos Extensions

Khronos extensions use the reserved `KHR` prefix and are ratified by Khronos and therefore are covered by the Khronos IP framework.  Khronos members can submit an extension for ratification, which is then voted on by the Khronos Board of Promoters.

### Extensions vs. Extras

In addition to extensions, the `extras` object can also be used to extend glTF.  This is completely separate from extensions.

All glTF object properties allow adding new properties to an `extras` object sub-property, e.g.,
```json
"a-vertex-shader-id" : {
    "name": "user-defined-name",
    "uri" : "vertex-shader.glsl",
    "type": 35633,
    "extras" : {
        "Application specific" : "The extra object can contain any properties."
    }
}
```
This enables glTF models to contain application-specific properties without creating a full glTF extension.  This may be preferred for niche use cases.
