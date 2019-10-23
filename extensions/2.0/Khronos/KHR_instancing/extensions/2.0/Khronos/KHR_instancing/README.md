# KHR\_instancing

## Khronos 3D Formats Working Group

* John Cooke
* TODO

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is specfically designed to enable GPU instancing which renders many copies of a single mesh at once using a small number of draw calls.  It's useful for things 
like trees, grass, road signs, etc.  The TRANSFORM attribute allows the mesh to be displayed at many different locations with different rotations and scales.  The ID attribute 
can be used to alter other parameters based on the run-time inplementation.

## Extending Nodes with per instance attributes

Instancing is defined by adding the `KHR_instancing` extension to any glTF node that has a mesh.  Instancing only applies to mesh nodes, there is no defined behavior for a node 
with this extension that doesn't also have a mesh.  Applying to nodes rather than meshes allows the same mesh to be used by several nodes, instanced or otherwise.  The attributes 
section contains accessor ids for the new TRANSFORM and ID attribute buffers.  For example, the following defines some instancing attributes to a node with mesh.  

```json
{
    "nodes": [
        {
            "mesh": 0,
            "name": "teapot",
            "extensions": {
                "KHR_instancing": {
                    "attributes": {
                        "TRANSFORM": 0,
                        "ID": 1
                    },
                }
            }
        }
    ]
}
```

## Appendix

TODO

## Reference

TODO

### Theory, Documentation and Implementations

TODO