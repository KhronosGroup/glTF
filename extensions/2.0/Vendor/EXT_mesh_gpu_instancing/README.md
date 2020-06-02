# EXT\_mesh\_gpu\_instancing

## Contributors

* John Cooke, OTOY, <mailto:john.cooke@otoy.com>
* Don McCurdy
* Arseny Kapoulkine, [@zeuxcg](https://twitter.com/zeuxcg)

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is specfically designed to enable GPU instancing which renders many copies of a single mesh at once using a small number of draw calls.  It's useful for things 
like trees, grass, road signs, etc.  The TRANSLATION, ROTATION, and SCALE attributes allows the mesh to be displayed at many different locations with different rotations and scales.  
Custom attributes can use the underscore mechanism to achieve other effects (i.e. _ID, _COLOR, etc.).

## Extending Nodes With Instance Attributes

Instancing is defined by adding the `EXT_mesh_gpu_instancing` extension to any glTF node that has a mesh. Instancing only applies to mesh nodes, there is no defined behavior for a node 
with this extension that doesn't also have a mesh. Applying to nodes rather than meshes allows the same mesh to be used by several nodes, instanced or otherwise. The attributes 
section contains accessor ids for the TRANSLATION, ROTATION, and SCALE attribute buffers, all of which are optional. The attributes specify an object space transform that should be 
multipled by the node's world transform in the shader to produce the final world transform for the instance. For example, the following defines some instancing attributes to a node with mesh.  

```json
{
    "nodes": [
        {
            "mesh": 0,
            "name": "teapot",
            "extensions": {
                "EXT_mesh_gpu_instancing": {
                    "attributes": {
                        "TRANSLATION": 0,
                        "ROTATION": 1,
                        "SCALE": 2,
                        "_ID" : 3
                    },
                }
            }
        }
    ]
}
```

Valid accessor type and component type for each attribute semantic property are defined below.

|Name|Accessor Type|Component Type(s)|Description|
|----|----------------|-----------------|-----------|
|`"TRANSLATION"`|`"VEC3"`|`5126`&nbsp;(FLOAT)|XYZ translation vector|
|`"ROTATION"`|`"VEC4"`|`5126`&nbsp;(FLOAT)<br>`5120`&nbsp;(BYTE)&nbsp;normalized<br>`5122`&nbsp;(SHORT)&nbsp;normalized|XYZW rotation quaternion|
|`"SCALE"`|`"VEC3"`|`5126`&nbsp;(FLOAT)|XYZ scale vector|

All attribute accessors in a given node **must** have the same `count`.

> **Implementation Note:** When instancing is used on the node, the non-instanced version of the mesh should not be rendered.

## Transformation Order

When using instancing, the instance transform matrix is constructed by multiplying translation (if present), rotation (if present), and scale in the same order as they are for nodes:

    InstanceXform = Translation * Rotation * Scale
    
The node's world transform matrix is constructed by multiplying the node's local transform matrix from the root of the tree up until (and including) the node itself:

    NodeWorldXform = RootXform * ... * ParentLocalXform * NodeLocalXform
    
The resulting transforms are applied to mesh positions/normals/etc., with instance transformation applied first:

    WorldPosition = NodeWorldXform * InstanceXform * VertexPosition
    
> Note: the construction above assumes column vectors; for row vectors, the matrix multiplication order should be reversed
