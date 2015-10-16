# glTF Extension Registry

## Draft Khronos extensions

_Draft Khronos extensions are not ratified yet._

* [KHR_binary_glTF](Khronos/KHR_binary_glTF/README.md)
* [KHR_materials_common](Khronos/KHR_materials_common)
* 
## Vendor extensions

* [CESIUM_RTC](Vendor/CESIUM_RTC/README.md)
* [WEB3D_quantized_attributes](Vendor/WEB3D_quantized_attributes/README.md)

# About glTF Extensions

glTF extensions extend the base glTF model format.  Extensions can introduce new properties (including properties that reference external data, and the extension can define the format of those data), new parameter semantics, reserved ids, and new container formats.  Extensions are written against a specific version of glTF and may be promoted to core glTF in a later glTF version. 

## Extension Mechanics

All glTF object properties (see [glTFProperty.schema.json](https://github.com/KhronosGroup/glTF/blob/master/specification/schema/glTFProperty.schema.json)) have an optional `extensions` object property that can contain new extension-specific properties.  This allows extensions to extend any part of glTF, including geometry, materials, animations, etc.  Extensions can also introduce new parameter semantics, reserved ids, and new formats containing glTF.

Extensions can't remove existing glTF properties or redefine existing glTF properties to mean something else.

Examples include:
* **New properties**: `KHR_binary_glTF` introduces a `bufferView` property for shaders, e.g.,
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
* **Reserved ids**: `KHR_binary_glTF` introduces an explicitly named `buffer` called `binary_glTF`.
* **New container formats**: `KHR_binary_glTF` introduces a binary format that contains the glTF JSON and geometry, textures, etc.

All extensions used in a model are listed as strings in the top-level `extensionsUsed` array, e.g.,
```javascript
"extensionsUsed" : [
   "KHR_binary_glTF"
]
```
This allows an engine to quickly determine if it supports the extensions needed to render the model without inspecting the the `extensions` property of all objects.

## Creating Extensions

To create a new extension, use the [extension template](Template.md) and open a pull request into this repo.  Make sure to add the extension to the glTF Extension Registry (top of this file).

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
"a-vertex-shader-id" : {
    "name": "user-defined-name",
    "uri" : "vertex-shader.glsl",
    "type": 35633,
    "extras" : {
        "Application specific" : "The extra object can contain any properties."
    }
}
```
This enables glTF models to contain application-specific properties without creating a full glTF extension.  This may be preferred for niche use cases where an extension would not be widely adopted.
