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
The `KHR_selectability` extension is added to the objects within the `nodes` array. The `KHR_selectability` extension object contains two objects `selectable`, which may be true or false, and `shape`, which is optional and, if present, must be the index of a top level `KHR_collision_shapes.shape`, which provides the geometry of the collider. `selectable` is read/write accessible through json pointers and influences selectability of all nodes below in the heirarchy. 

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

### JSON Schema

todo: update
[glTF.KHR_texture_video.schema.json](schema/glTF.KHR_texture_video.schema.json)

## Selectability calculation

Whether a `select` action on an object should cause a select event for that object, or whether it should proceed to the next object further along the selection ray is calculated as follows: if there are, in the current dynamic state of the data model, no nodes at or above the current node with `KHR_selectability/selectable` having a value of `false`, AND, if there is at least one node at or above the current node with `KHR_selectability/selectable` having a value of `true` AND the same node has a `KHR_selectability/shape` value, then the select event will be fired for the lowest object in the hierarchy that has both a `shape` and `selectable=true`, and may proceed up the heirarchy as defined in `KHR_interactivity` or in application specific responses to select. Otherwise, selection may proceed to the next mesh further along the selection ray until these requirements are satisfied or all objects along the ray have been exhausted.

In summary, selectability requires a shape and is controlled by the logical AND of selectability of the node and all of its parents, excluding from the calculation those parents for whom selectability is undefined. This allows a single change of selectability at high levels of the hierarchy to, at runtime, enable or disable the selectability of complex (multi-node) objects below in the hierarchy.

Note that selectability is independent of visibility. An object may be invisible but selectable or vice-versa.

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

In this example, the selectability for node zero and all nodes below it in the hierarchy is controlled by this sampler.
Note that, as long as the gltf file uses the KHR_selectability extension, the /nodes/x/extensions/KHR_selectability/selectable is writeable for all nodes x, regardless of whether KHR_selectability is defined in the gltf on those nodes. Once written to, the node behaves as though KHR_selectability is defined on the given node.

## Control by interactivity

Controlling selectability via interactivity in conjunction with `KHR_interactivity` can be done by writing or reading the value at the json path `/nodes/{nodenumber}/extensions/KHR_selectability/selectable`

Note that, as long as the gltf file uses the KHR_selectability extension, the /nodes/x/extensions/KHR_selectability/selectable is writable for all nodes x, regardless of whether KHR_selectability is defined in the gltf on those nodes. When read, selectability is undefined on a given node unless KHR_selectability has been defined on the node in the gltf OR it has been written by KHR_animation_pointer, KHR_interactivity, or similar.


## Known Implementations


## Resources

