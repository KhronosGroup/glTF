# KHR_materials_variants

## Contributors

- Mike Bond, Adobe, [@Miibond](https://github.com/MiiBond)
- Aurelien Vaysset, [Emersya](http://emersya.com)
- Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
- Renee Rashid, Facebook, [@debuggrl](https://github.com/debuggrl)
- Jeremy Cytryn, Facebook, [@jercytrn](https://github.com/jercytryn)
- Edgar Simson, OLV, [@Edzis](https://github.com/edzis)
- Daniel Beauchamp, Shopify, [@pushmatrix](http://github.com/pushmatrix)
- Stephan Leroux, Shopify, [@sleroux](http://github.com/sleroux)
- Eric Chadwick, Wayfair, [@echadwick-wayfair](https://github.com/echadwick-wayfair)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple material variants of an asset, structured to allow low-latency switching at runtime.

A typical use case is digital commerce, where a user might be presented with e.g. a pair of sneakers and the ability to switch between different colours.

![pair of sneakers](examples/shoes/photo.png)

## Design Goals

_This section is non-normative._

This extension aims to give asset creators and consumers a compact way to define multiple materials for a common set of geometry nodes in a glTF document. We explicitly optimize for the case of representing material variants which are:

* **finite** - the list of variants is bounded; multi-dimensional variants (e.g. color + texture) are possible but must ultimately be representable as a flattened list
* **premade** - the variants are produced at asset authoring time
* **holistic** - while composable, the variants are defined at the granularity of the glTF asset; sub-variants or hierarchical variants are not considered

In digital commerce, each variant may correspond to a distinct SKU or stocked product code, which naturally meet these requirements.  In games or movies, each variant may represent a fixed set of curated asset skins which can be represented, moved across the wire, and loaded efficiently.

A non-goal of this extension is to serve _configuration authorship_ use cases (i.e. configuration builders or "configurators"). These have a separate set of concerns, are more subject to complex application-specific business logic, and may benefit from dynamic and continuous configurability with complex builder rules.

While not designed around this use case, we note however that this extension does implicitly communicate at the primitive level which materials are available for that primitive, for any such applications which may be interested in leveraging this information.

## Material Variants

A _material_variant_ is a set of mappings that determine which material can be applied to a given primitive. Consider a model of a shoe where three meshes are defined along with two materials. These meshes contain an assortment of primitives that have an associated material attached.

Imagine a sneaker with shoelace holes that are made from materials that depend on the overall shoe colour in non-obvious ways:

| Meshes to Skin                     | Available Materials             |
| ---------------------------------- | ------------------------------- |
| `laces`                            | `white_matte`                   |
| `body`                             | `red_matte`                     |
| `sole`                             |                                 |

In one variant of the shoe, the primitives associated with each mesh listed below would have the following materials assigned to them:

| Meshes                             | Material                        |
| ---------------------------------- | ------------------------------- |
| `laces`                            | `white_matte`                   |
| `body`                             | `red_matte`                     |
| `sole`                             | `white_matte`                   |

In this case, the laces and sole of the shoe would be white while the body of the shoe would be red.

This material variant may have an associated SKU or product name in digital commerce. The shoe, however, also comes in an alternative color scheme that is represented by an alternative set of primitive/material mappings:

| Meshes                             | Material                        |
| ---------------------------------- | ------------------------------- |
| `laces`                            | `red_matte`                     |
| `body`                             | `white_matte`                   |
| `sole`                             | `white_matte`                   |

In the second case, the shoe would have a white body and sole but red laces. In both cases, the meshes and their associated primitives remain the same with the exception of which material is used. Looking at the laces mesh, the KHR_materials_variants extension would define this mapping as follows:

```javascript
"meshes": [
    {
        "name": "laces",
        "primitives": [
            {
                 ...,
                "extensions": {
                    "KHR_materials_variants" : {
                        "mappings": [
                            {
                                "variants": [0],
                                "material": 1,
                            },
                            {
                                "variants": [1],
                                "material": 2,
                            },
                        ],
                    }
                }
            },
            // ... more primitives ...
        ]
    },
    // ... more meshes ...
]
```

The variants property within each mapping refers to the index of a material variant located at the glTF root node:

```javascript
{
  "asset": {"version": "2.0", "generator": "Fancy 3D Tool" },
  "extensions": {
      "KHR_materials_variants": {
        "variants": [
          {"name": "Red Shoe with White Laces" },
          {"name": "White Shoe with Red Laces" },
        ]
      }
  }
}
```

## Interaction with existing glTF functionality

_This section is non-normative._

The primary purpose of this extension is to simply formalise the idea of static configurability, so that members of the ecosystem can meaningfully communicate: those who produce assets and those who ingest them, and all the tooling in between.

As a secondary effect, material variants allow multiple assets — with shared geometry but different materials — to be stored more compactly, particularly relevant for the self-contained binary format GLB. When using external URIs as references to textures, applications may (optionally) process geometry only once and lazily request texture assets only when needed for a particular variant.

## Implications for Applications and APIs

_This section is non-normative._

How does an application communicate to a glTF engine what the initial variant state should be? How does it submit a runtime request for a different configuration? It's out of scope for this extension to constrain or mandate an engine's public API, but a useful implementation will require something of the sort.

## Examples

_This section is non-normative._

For reference, we provide several simple examples of variants, both represented as distinct constituent GLB files without the extension and as multi-variant GLB files leveraging this extension.  We also include the respective net size of the individual variant GLB files and the size of the single combined multi-variant GLB, for comparison.

| Model                     | Screenshot                         | Description                                                                                                   | Net Size (MB) | Combined Size (MB) |
|---------------------------|------------------------------------|---------------------------------------------------------------------------------------------------------------|---------------|--------------------|
| [Shoes](examples/shoes)   | ![shoes](examples/shoes/photo.png)      | A simple variant commerce example with a single mesh primitive                                                | 15            | 7.5                |
| [Chair](examples/chair)   | ![chairs](examples/chair/chairs.jpg)     | A more complex commerce example, involving multi-dimensional variants - flattened into four distinct variants | 7.0           | 1.7                |
| [Helmet](examples/helmet) | ![helmets](examples/helmet/helmets.jpg)   | Gaming example, with wet and dry variants of a helmet                                                         | 8.8           | 5.0                |
