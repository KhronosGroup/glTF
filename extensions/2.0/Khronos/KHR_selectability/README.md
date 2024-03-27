# KHR_selectability

## Contributors

* Dwight Rodgers, Adobe
* Peter Martin, Adobe
* Eoin Mcloughlin, Microsoft

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

This specification depends on `KHR_collision_shapes` to describe geometries used for collision detection.

## Overview

This extension allows glTF files (and animation and interactivity) to control selectability of node hierarchies within the scene. When used in conjunction with `KHR_interactivity` extension, the content can define the behavior that should occur when an object is "selected" (e.g. by being tapped or clicked by a user or being selected using a VR controller or equivalent). If used without the `KHR_interactivity` extension, the result of a selection action, if any, will be application specific.

Selectability is intended to be a dynamic state that can be manipulated at runtime by either the `KHR_animation_pointer` or `KHR_interactivity` extensions or both, if present.

## glTF Schema Updates
The `KHR_selectability` extension is added to the objects within the `nodes` array. The `KHR_selectability` extension object contains two objects `selectable`, which may be true or false, and `shape`, which is optional and, if present, must be the index of a top level `KHR_collision_shapes.shape`, which provides the geometry of the collider. `selectable` is read/write accessible through json pointers and influences selectability of all nodes below in the heirarchy. A value of false causes all nodes below in the hierarchy to be omitted from selection, even any nodes below that have a value of true. The absence of the KHR_selectability extension from a node behaves the same way as if the KHR_selectability extension is applied with a `selectable` value of `true` and no `shape` property.

In other words, any node that has a `KHR_selectability/shape` should react to a select event based on the logical OR of its `selectable` property and that of all of its parents, with `selectable` defaulting to true for all nodes. This allows a single change of a `selectable` flag at a high level of the hierarchy to enable or disable selection on complex (multi-node) objects.

```
"nodes": [
    {
        "mesh": 0,
        "transform": ...,
        "extensions": {
            "KHR_selectability": {
                "shape": 0
                "selectable": true
            }
        }
    }
]
```

Note that selectability is independent of visibility. An object may be invisible but selectable or vice-versa.

### JSON Schema

todo: update
[glTF.KHR_texture_video.schema.json](schema/glTF.KHR_texture_video.schema.json)


## Control by animation

`KHR_selectability` extension can be used on animation channels to control the selectability over time on an object by object basis by using `KHR_selectability` in conjunction with `KHR_animation_pointer`:

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
                                    "pointer" : "/nodes/0/extensions/KHR_selectability/selectable"
                                }
                            }
                    }
                },
    ...
```

In this example, the selectability for node zero (influencing all nodes below it in the hierarchy) is controlled by this sampler.
Note that, as long as the gltf file uses the `KHR_selectability` extension, the `/nodes/x/extensions/KHR_selectability/selectable` is writeable for all nodes x, regardless of whether `KHR_selectability` is defined in the gltf on those nodes.

## Control by interactivity

Controlling selectability via interactivity in conjunction with `KHR_interactivity` can be done by writing or reading the value at the json path `/nodes/{nodenumber}/extensions/KHR_selectability/selectable`

Note that, as long as the gltf file uses the `KHR_selectability` extension, the `/nodes/x/extensions/KHR_selectability/selectable` is readable and writable for all nodes x, regardless of whether `KHR_selectability` is defined in the gltf on those nodes. It defaults to `true` if `KHR_selectability` extension has not been defined on the node AND it has not been written by `KHR_animation_pointer`, `KHR_interactivity`, or similar.


## Known Implementations


## Resources

