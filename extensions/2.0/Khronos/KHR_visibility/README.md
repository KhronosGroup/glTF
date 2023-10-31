# KHR_visibility

## Contributors

* Dwight Rodgers, Adobe
* Emmett Lalish, Google

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows glTF animations and interactivity to control visibility of node hierarchies within the scene. It is intended for use in conjunction with `KHR_animation_pointer` or `KHR_interactivity` extensions.

## glTF Schema Updates

The `KHR_visible` extension is added to the objects within the `nodes` array. The `KHR_visible` extension object contains a single object `visible` which may be true or false. This valuable is read/write accessible through json pointers and controls visibility of all nodes in the subgraph. A value of false causes all nodes below in the hierarchy to be omitted from display, even anynodes below that have a value of true. A value of true behaves the same as if no value is specified.

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

`KHR_visibility` extension can be used on animation channels to control the playhead on an object by object basis by using `KHR_visibility` in conjunction with `KHR_animation_pointer`:

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
                                "KHR_visibility" : {
                                    "pointer" : "/nodes/0/extensions/KHR_visibility/visibility"
                                }
                            }
                    }
                },
    ...
```

In this example, the visibility for node zero and all nodes below it in the hierarchy is controlled by this sampler.

## Control by interactivity

Controlling visibility via interactivity in conjunction with `KHR_interactivity` can be done by setting the value at the json path `/nodes/{nodenumber}/extensions/KHR_visibility/visibility`

## Known Implementations


## Resources

