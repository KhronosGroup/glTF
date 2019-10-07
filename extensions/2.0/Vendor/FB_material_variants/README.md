# FB_material_variants

## Contributors

- Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
- Renee Rashid, Oculus

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple variants of an asset, structured to allow low-latency switching between them at runtime. A typical use case is digital commerce, where a user might be presented with e.g. a pair of sneakers and the ability to switch between different colours.

We introduce a simple tag-based scheme, that allows for high-level runtime swapping between which `material` is used to shade a given `mesh primitive`:

```javascript
"meshes": [
    {
        "primitives": [
            {
                "attributes": {
                "POSITION": 0,
                "NORMAL": 1,
                },
                "indices": 2,
                "material": 0,
                "extensions": {
                    "FB_material_variants" : {
                        "mapping": [
                            {
                                "tags": [ "sneaker_yellow" ],
                                "material": 0,
                            },
                            {
                                "tags": [ "sneaker_red" ],
                                "material": 1,
                            },
                        ],
                    }
                }
            }
        ]
    }
]
```

The tag-based approach allows highly selective material switching, e.g. changing the colour of shoe laces only, or wholesale changes across diverse geometry, by reusing the same tags in switches across many distinct mesh primitives. In the typical case, many tags will be active, e.g. `['sneaker_red', 'laces_gold']`.

Composition also works well with tags: if a scene were built from several different variational models, each with their own set of tags, then it's easy to imagine an API call on the entire scene that takes a set of tags and passes them on to each constituent model, recursively.

## Interaction with existing glTF functionality

The primary purpose of this extension is to simply formalise the idea of configurability, so that members of the ecosystem can meaningfully communicate: those who produce asset and those who ingest them, and all the tooling inbetween.

Beyond that, it's also a compression scheme: a way to compactly represent multiple assets that have a lot in common (e.g. most geometry, some textures). In this respect, it's most meaningful for the self-contained binary format GLB, where the asset file contains all its data up-front, both that which is shared between variants and that which differs.

If we also allow external references, i.e. images and buffers to source from URLs,then we can additionally come up with sophisticated mechanisms where geometry and textures are loaded separately, dynamically. There's pragmatic pros and cons to going down this route, all beyond the scope of this document. Each application will make its own decisions there; meawhile, the extension proposed works well regardless.

## Implications for Applications and APIs

How does an application communicate to a glTF engine what the initial variant state should be? How does it submit a runtime request for a different configuration? It's out of scope for this extension to constrain or mandate an engine's public API, but a useful implementation will require something of the sort.
