# FB\_asset\_variants

## Contributors

* Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
* Renee Rashid, Oculus

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

A common request for 3D models is the ability to package multiple similar variants of a model compactly in the same file, and easily switching between them at runtime. A canonical use case would be in digital commerce, where a user might be presented with e.g. a pair of sneakers along with some UI to switch between different colours.

This extension introduces a simple tag-based scheme that allows for high-level runtime swapping between which glTF root-level `scene` is active, and which `material` is used for shading, on a per-mesh-primitive basis.

A basic implementation of this extension could simply re-parse the whole model whenever the configuration changes. It's expected that more sophisticated viewers will go to some lengths to allow for rapid and seamless switching between variants.

## Switching Scenes

When variants differ in terms of geometry, they should be represented as distinct glTF `scene` graphs, i.e. separate node hierarchies. This allows for subtle changes in transforms (scaling, rotation, translation) while allowing prudent reuse of meshes.

```javascript
"extensions": {
    "FB_asset_variants" : {
        "variants": [
            {
                "tag": "sneaker_size_7",
                "description": "Sneakers, Size 7",
                "scene": 0
            },
            {
                "tag": "sneaker_size_8",
                "description": "Sneakers, Size 8",
                "scene": 1                
            },
            ...
        ]
    }
}
```

The inline descriptions are optional, and meant only as human-readable fallback data whenever the model is not contained in a dedicated application.

## Switching Materials

When variants differ in terms of shading properties, the most direct approach is for each `mesh primitive` to have switchable `material` references:

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
                    "FB_asset_variants" : {
                        "variants": [
                            {
                                "tag": "sneaker_red",
                                "description": "Sneakers: Red",
                                "material": 0
                            },
                            {
                                "tag": "sneaker_black",
                                "description": "Sneakers: Black",
                                "material": 1
                            },
                            ...

                        ]
                    }
                }
            }
        ]
    }
]
```

The tag-based approach allows highly selective material switching, e.g. changing the colour of shoe laces only, or wholesale changes across diverse geometry, by reusing the same tags in switches across many distinct mesh primitives.

## Tags & Applications

The tagging approach used herein is meant to provide expressive power in a simple and orthogonal way. In the typical case, many tags will be active, e.g. `['sneaker_red', 'sneaker_size_7']`. This approach also eases composition: populating a scene with several different models (that use this extension) works as one would expect.

One consideration remains: how does an application communicate what the initial variant state should be, and how does it submit a runtime request for a different configuration? While it's out of scope for this extension to constrain or mandate an engine's public API, it seems clear that any useful implementation of this spec will require something of the sort.

## Limitations

A third and very powerful way to express geometric variation would be through static poses, using the animation features that glTF already supports –– specifically skinning and morph targets. We considered such support outside the scope of this extension, but would welcome its inclusion if other parties are excited about the possibility.
