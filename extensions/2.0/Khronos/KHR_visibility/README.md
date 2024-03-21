# KHR_visibility

## Contributors

* Dwight Rodgers, Adobe
* Peter Martin, Adobe
* Emmett Lalish, Google

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF animations and interactivity to control visibility of node hierarchies within the scene. It is intended for use in conjunction with `KHR_animation_pointer` or `KHR_interactivity` extensions.

## glTF Schema Updates

The `KHR_visibility` extension is added to the objects within the `nodes` array. The `KHR_visibility` extension object contains a single object `visible` which may be true or false. This valuable is read/write accessible through json pointers and controls visibility of all nodes in the subgraph. A value of false causes all nodes below in the hierarchy to be omitted from display, even any nodes below that have a value of true. The absence of the KHR_visiblity extension from a node behaves the same way as if the KHR_visibility extension is applied, with a value of true.

In other words, a node should be rendered visible based on the logical OR of its `visible` property and that of all of its parents, with `visible` defaulting to true for all nodes. This allows a single change of a `visible` flag at a high level of the hierarchy to hide or show complex (multi-node) objects.

```
"nodes": [
    {
        "mesh": 0,
        "transform": ...,
        "extensions": {
            "KHR_visibility": {
                "visible": false
            }
        }
    }
]
```

### JSON Schema

todo: update
[glTF.KHR_texture_video.schema.json](schema/glTF.KHR_texture_video.schema.json)

## Control by animation

`KHR_visibility` extension can be used with animation channels to control visibility of an object by using `KHR_visibility` in conjunction with `KHR_animation_pointer`:

```
{
    "animations": [
        {
            "channels": [
                {
                    "sampler": 0,
                    "target": {
                        "path": "pointer",
                        "extensions": {
                                "KHR_animation_pointer" : {
                                    "pointer" : "/nodes/0/extensions/KHR_visibility/visible"
                                }
                            }
                    }
                },
    ...
```

In this example, the visibility for node zero and all nodes below it in the hierarchy is controlled by this sampler.
Note that, as long as the gltf file uses the KHR_visibility extension, the /nodes/x/extensions/KHR_visibility/visible is writeable for all nodes x, regardless of whether KHR_visibility is defined in the gltf on those nodes.

## Control by interactivity

Controlling visibility via interactivity in conjunction with the `KHR_interactivity` extension can be done by setting the value at the json path `/nodes/{nodenumber}/extensions/KHR_visibility/visible`

Note that, as long as the gltf file uses the KHR_visibility extension, the /nodes/x/extensions/KHR_visibility/visible is readable and writable for all nodes x, regardless of whether KHR_visibility is defined in the gltf on those nodes. It defaults to true if KHR_visibility extension has not been defined on the node AND it has not been written by either KHR_animation_pointer, KHR_interactivity, or similar.


## Known Implementations


## Resources

