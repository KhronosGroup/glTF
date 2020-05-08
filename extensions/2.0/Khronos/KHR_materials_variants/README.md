# KHR_materials_variants

## Contributors

- Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
- Renee Rashid, Facebook, [@debuggrl](https://github.com/debuggrl)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple material variants of an asset, structured to allow low-latency switching at runtime.

A typical use case is digital commerce, where a user might be presented with e.g. a pair of sneakers and the ability to switch between different colours.

## Tagged Variants

We introduce a simple tag-based extension scheme, that allows for high-level runtime swapping between which `material` is used to shade a given `mesh primitive`: the extension root contains a mandatory `mapping` property, which is an array of objects, each one associating some set of tags with a material reference.

Imagine a sneaker with shoelace holes that are made from materials that depend on the overall shoe colour in non-obvious ways:

| Tags                               | Material                        |
| ---------------------------------- | ------------------------------- |
| `sneaker_yellow`, `sneaker_orange` | `shoelace_hole_material_brown`  |
| `sneaker_red`                      | `shoelace_hole_material_purple` |
| `sneaker_black`                    | `shoelace_hole_material_yellow` |

(_the authors of this spec are not product designers, apologies for the dubious colour choices_)

Application-specific logic defines the concept of a set of active tags, which may be empty.  The currently active `material` for a `mesh primitive` is found by stepping through this array of mappings in order, and selecting the first one which contains somewhere in its list of tags any one of the currently active tags. The corresponding material is assigned to the mesh.  If none match, fall back on vanilla glTF behaviour.

In other words, **this is not a literal mapping** in its glTF form – exporters, take note.

## The variant mapping as glTF JSON

A snippet of a mesh implementing shoe holes which uses this extension might look like:

```javascript
"meshes": [
    {
        "name": "shoelace_hole",
        "primitives": [
            {
                "attributes": {
                "POSITION": 0,
                "NORMAL": 1,
                },
                "indices": 2,
                "material": 0,
                "extensions": {
                    "KHR_materials_variants" : {
                        "mapping": [
                            {
                                "tags": [ "sneaker_yellow", "sneaker_orange" ],
                                "material": 7,
                            },
                            {
                                "tags": [ "sneaker_red" ],
                                "material": 8,
                            },
                            {
                                "tags": [ "sneaker_black" ],
                                "material": 9,
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

The tag-based approach allows highly selective material switching, e.g. changing the colour of shoe laces only, or wholesale changes across diverse geometry, by reusing the same tags in switches across many distinct mesh primitives.

In the typical case, many tags may be active in the application, e.g. `{'sneaker_red', 'laces_gold'}`.

Composition also works well with tags: if a scene were built from several different variational models, each with their own set of tags, then it's easy to imagine an API call on the entire scene that takes a set of tags and passes them on to each constituent model, recursively.

## Interaction with existing glTF functionality

_This section is non-normative._

The primary purpose of this extension is to simply formalise the idea of configurability, so that members of the ecosystem can meaningfully communicate: those who produce assets and those who ingest them, and all the tooling in between.

As a secondary effect, material variants allow multiple assets — with shared geometry but different materials — to be stored more compactly, particularly relevant for the self-contained binary format GLB. When using external URIs as references to textures, applications may (optionally) process geometry only once and lazily request texture assets only when needed for a particular variant.

## Implications for Applications and APIs

_This section is non-normative._

How does an application communicate to a glTF engine what the initial variant state should be? How does it submit a runtime request for a different configuration? It's out of scope for this extension to constrain or mandate an engine's public API, but a useful implementation will require something of the sort.
