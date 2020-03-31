# EXT\_mesh\_gpu\_instancing

## Khronos 3D Formats Working Group

* John Cooke
* Don McCurdy
* Arseny Kapoulkine 

## Acknowledgments

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is specfically designed to enable GPU instancing which renders many copies of a single mesh at once using a small number of draw calls.  It's useful for things 
like trees, grass, road signs, etc.  The TRANSLATION, ROTATION, and SCALE attributes allows the mesh to be displayed at many different locations with different rotations and scales.  
Custom attributes can use the underscore mechanism to achieve other effects (i.e. _ID, _TRANSFORM4x3, etc.).

## Extending Nodes with per instance attributes

Instancing is defined by adding the `EXT_mesh_gpu_instancing` extension to any glTF node that has a mesh.  Instancing only applies to mesh nodes, there is no defined behavior for a node 
with this extension that doesn't also have a mesh.  Applying to nodes rather than meshes allows the same mesh to be used by several nodes, instanced or otherwise.  The attributes 
section contains accessor ids for the TRANSLATION, ROTATION, and SCALE attribute buffers, all of which are optional.  The attributes specify an object space transform that should be 
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

## Appendix


## Reference


### Theory, Documentation and Implementations
