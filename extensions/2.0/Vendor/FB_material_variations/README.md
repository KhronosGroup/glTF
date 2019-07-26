# FB\_material\_variations

## Contributors

* Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
* Renee Rashid, Oculus

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple variations of an asset, structured to allow easy switching between them at runtime. A typical use case is digital commerce, where a user might be presented with e.g. a pair of sneakers and the ability to switch between different colours.

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
                    "FB_material_variations" : {
                        "variations": [
                            {
                                "tags": [ "sneaker_yellow" ],
                                "material": 0
                            },
                            {
                                "tag": [ "sneaker_black" ],
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

The tag-based approach allows highly selective material switching, e.g. changing the colour of shoe laces only, or wholesale changes across diverse geometry, by reusing the same tags in switches across many distinct mesh primitives. In the typical case, many tags will be active, e.g. `['sneaker_red', 'laces_gold']`. This approach also eases composition: a scene built from several different models (that use this extension) works as one would expect.

## Implications for Applications and APIs

How does an application communicate to a glTF engine what the initial variation state should be? How does it submit a runtime request for a different configuration? It's out of scope for this extension to constrain or mandate an engine's public API, but a useful implementation of this spec will require something of the sort.

A basic implementation of this extension could simply re-parse the whole model whenever the configuration changes. It's expected that more sophisticated viewers will do upfront work in order to optimise for low-latency switching between variations.
