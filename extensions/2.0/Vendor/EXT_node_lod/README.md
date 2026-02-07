# EXT\_node\_lod

## Contributors

* Takahiro Aoyagi, Mozilla, [@takahirox](https://github.com/takahirox)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This `EXT_node_lod` extension adds the ability to specify various Levels
of Detail (LOD) to a glTF asset. An implementation of this extension can
use the LODs for various scenarios like rendering a different LOD based 
upon the distance of the object or progressively loading the glTF asset
starting with the lowest LOD.

This extension allows an LOD to be defined at `node` level. The LODs can
specify different geometry and materials across various levels.

The extension is added to the highest LOD level. The extension defines an
`lod` property which is an array containing the objects including the
`node` property that points to the index of the `nodes` as LOD node and
the screen coverage number as a hint to determine the switching threshold
for the LOD.

Every `node` index in the array of the extension points to the index of
another `node` object which should be used as the LOD that is lower level
than the previous array element.

An implementation of this extension can parse through the `lod` array of
the extension to create the list of LOD levels available for an asset.
If an asset containing this extension is loaded in a client that doesn't
implement the extension, then the highest LOD level will be loaded and
all lower LODs will be ignored.

## Examples

```json
"nodes": [
    {
        "name": "High_LOD",
        "mesh": 0,
        "extensions": {
            "EXT_node_lod": {
                "lod": [
                    {"node": 1, "coverage": 0.5},
                    {"node": 2, "coverage": 0.2},
                ],
            },
        },
    },
    {
        "name": "Medium_LOD",
        "mesh": 1,
    },
    {
        "name": "Low_LOD",
        "mesh": 2,
    },
]
```

In this node, *node[0]* is the highest LOD. The first element in the `lod` array,
*node[1]*, is the next lower LOD (or the Medium_LOD). The second element in the
`lod` array, *node[2]*, is the lowest LOD.

## Screen coverage

The `coverage` property in the `lod` array specifies a screen coverage
value that can be used as a hint to determine the switching threshold for the
various LODs.

Screen coverage is a ratio between `0.0` and `1.0` representing how big the
mesh's rendered screen surface area is in the total screen surface area.

The highest LOD `coverage` value is implied to `1.0`. The `coverage` value
must be between `0.0` and `1.0`, and must be between the next higher LOD
`coverage` value and the next lower LOD `coverage` value.

The LOD is expected to be selected to be rendered if the current screen
coverage is between the LOD's `coverage` value and the next lower LOD
`coverage` value (or 0 if it is the lowest LOD).

In the example above the screen coverage values can be used as follows:

* LOD0(high) Rendered from 1.0 to 0.5
* LOD1(medium) Rendered from 0.5 to 0.2
* LOD2(low) Rendered from 0.2 to 0

## LOD node restriction

For simplicity there are some restrictions for LOD nodes.

### Nested LOD nodes are disallowed

A node referred by a `node` of a `lod` array element of the `EXT_node_lod` extension
or its descendant nodes **MUST NOT** define the `EXT_node_lod` extension.

```json
"nodes": [
    {
        "name": "High_LOD",
        "mesh": 0,
        "extensions": {
            "EXT_node_lod": {
                "lod": [
                    {"node": 1, "coverage": 0.5},
                ],
            },
        },
    },
    {
        "name": "Low_LOD",
        "mesh": 1,
        "extensions": {
            // Nested LOD is Invalid. 
            "EXT_node_lod": {
                "lod": [
                    {"node": 2, "coverage": 0.5},
                ],
            },
        },
    },
    ....
]
```

### Child nodes are part of that LOD

A node that defines the `EXT_node_lod` extension acts as the root node
for the highest LOD and its descendant nodes are part of that LOD. Similarly
a node referred by a `node` of a `lod` array element of the `EXT_node_lod`
extension is the root node for a lower LOD and  its descendant nodes will
be part of that LOD.

```json
"nodes": [
    {
        "name": "High_LOD",
        "mesh": 0,
        // child node[2] is part of the High LOD
        "children": [2],
        "extensions": {
            "EXT_node_lod": {
                "lod": [
                    {"node": 1, "coverage": 0.5},
                ],
            },
        },
    },
    {
        "name": "Low_LOD",
        "mesh": 1,
        // child node[3] is part of the Low LOD
        "children": [3],
    },
    ....
]
```

### LOD nodes are regarded as part of the scene node graph

Not only the highest LOD node and its descendant nodes but also the lower
LOD nodes and their children nodes **MUST** be regarded as part of the
scene node graph and the lower LOD nodes **MUST** be regarded as siblings
of the highest LOD node (so the parent of the highest LOD node is also
regarded as the parent of the lower LOD nodes) when cheking whether the
scene graph follows the node hierarchy rule.

Refer to [the Scenes - Nodes and Hierarchy section](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#nodes-and-hierarchy)
in the glTF core specification for the node hierarchy rule details.

```json
"nodes": [
    {
        "name": "Parent",
        "children": [1],
    },
    {
        "name": "High_LOD",
        "mesh": 0,
        "extensions": {
            "EXT_node_lod": {
                // node[2] is regarded as a sibling of this node and
                // regarded as whose parent is node[0]
                "lod": [
                    {"node": 2, "coverage": 0.5},
                ],
            },
        },
    },
    {
        "name": "Low_LOD",
        "mesh": 1,
        // Invalid because of forming a cycle in the node graph
        "children": [0],
    },
    {
        // Invalid because node[2] has two parents node[0] and node[3]
        "children": [2],
    },
    ....
]
```

## LOD Array property

| Property | Type | Description | Requires |
|:------|:------|:------|:------|
| `lod` | `lod [1-*]` | Lower LODs definition | :white_check_mark: Yes |

## Properties of LOD Array element

| Property | Type | Description | Requires |
|:------|:------|:------|:------|
| `node` | `integer` | `node` index for this LOD level. | :white_check_mark: Yes |
| `coverage` | `number` | Screen coverage for switching for this LOD level. The value must be between `0.0` and `1.0`, and must be between the next higher LOD `coverage` value and the next lower LOD `coverage` value. | :white_check_mark: Yes |

## Schema

* [node.EXT_node_lod.schema.json](schema/node.EXT_node_lod.schema.json)

## Reference

* [glTF `MSFT_lod` extension](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/MSFT_lod):
A glTF LOD vendor extension proposed by Microsoft. It allows more flexible
LOD configuration. The `EXT_node_lod` extension has simpler data structure
than `MSFT_lod` so less flexible but easier to write an implementation.
