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

TODO  

## Extending Nodes with per instance attributes

Instancing is defined by adding the `KHR_instancing` extension to any glTF mesh node.  The attributes section contains accessor ids for new TRANSFORM4x3 and ID attribute buffers. 
For example, the following defines some instancing attributes to a mesh node in the graph. Instancing only applies to mesh nodes.  Applying to nodes rather than meshes allows the 
same mesh to be attached as normal to nodes while also being instanced.  

```json
{
    "nodes": [
        {
            "mesh": 0,
            "name": "teapot",
            "extensions": {
                "KHR_instancing": {
                    "attributes": {
                        "TRANSFORM4x3": 0,
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