# KHR\_mesh\_instancing

## Khronos 3D Formats Working Group

* John Cooke
* Don McCurdy

## Acknowledgments

* TODO

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is specfically designed to enable GPU instancing which renders many copies of a single mesh at once using a small number of draw calls.  It's useful for things 
like trees, grass, road signs, etc.  The TRANSLATION, ROTATION, and SCALE attributes allows the mesh to be displayed at many different locations with different rotations and scales.  
Custom attributes can use the underscore mechanism to achieve other effects (i.e. _ID, _TRANSFORM4x3, etc.).

## Extending Nodes with per instance attributes

Instancing is defined by adding the `KHR_mesh_instancing` extension to any glTF node that has a mesh.  Instancing only applies to mesh nodes, there is no defined behavior for a node 
with this extension that doesn't also have a mesh.  Applying to nodes rather than meshes allows the same mesh to be used by several nodes, instanced or otherwise.  The attributes 
section contains accessor ids for the POSITION, ROTATION, and SCALE attribute buffers, all of which are optional.  Note that the attributes specify a world transform, even though the
node can exist anywhere within the node tree no parent transforms are applied to the attributes (because the attributes are uploaded directy to the gpu). For example, the following 
defines some instancing attributes to a node with mesh.  

```json
{
    "nodes": [
        {
            "mesh": 0,
            "name": "teapot",
            "extensions": {
                "KHR_mesh_instancing": {
                    "attributes": {
                        "TRANSLATION": 0,
                        "ROTATION": 1,
                        "SCALE": 2,
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