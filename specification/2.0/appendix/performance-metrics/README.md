# Runtime Performance Metrics 

The goal of these metrics is to assess the runtime performance needs of glTF assets.  
By runtime we mean the cpu/gpu, bandwidth and memory resources needed to render the model in realtime.  
These performance metrics are divided in two major categories - `complexity` and `memory`

## Performance Metric Categories

### Complexity  

The `complexity`category provides metrics to calculate cpu/gpu and memory bandwidth target requirements - larger numbers will mean a more powerful target device is needed.  
These metrics do not provide a mapping to target device performance, this mapping must be done by users of the metrics.  

### Memory  

The `memory` category provides metrics to calculate the runtime memory (size) needs of the model.  
Using these metrics an estimate of target memory can be calculated - however keep in mind that the exact memory requirements will vary with viewer implementations.  



|                 |             |          |                                                                |             |
|-----------------|-------------|-------------|----------|----------------------------------------------------------------|
| Name            | Type        | Category    | Property |Description                                                     |
|[vertexCount]    | Integer     | Complexity  |Scene     |Total number of vertices used by the Nodes in a scene           |  
|[nodeCount]      | Integer     | Complexity  |Scene     |Max nodecount in scene (add upp all nodes in a scene)           |  
|[primitiveCount] | Integer     | Complexity  |Scene     |Total number of referenced primitives (per scene).  This figure is the un-batched number of primitives, engines may optimize if primitives and meshes share textures. |  
|[textures]       | Integer     | Complexity  |Scene     |Flags specifying presence of materials and it's texture usage, this is the aggregated max usage. BASECOLOR, METALLICROUGHNESS, NORMAL, OCCLUSION, EMISSIVE, (SPECULARGLOSS) |  
|[attributes]     | Integer     | Memory      | Asset    | The total memory footprint, in bytes, for the models attribute buffer storage |  
|[textureSizes]  |Dimension    | Memory      | Asset    | The width and height of defined textures in the Asset. |  

Dimension is an Integer[2] containing width and height  


# Scene #  

Metrics that describe properties of a scene.  
glTF models may contain multiple scenes, only one scene will be visible at a time.  
To accomodate this the following metrics are calculated on a per-scene basis.  
It is possible to reference different meshes (nodes) using different scenes, this makes it possible to have the same texture assets but reference different primitives that use alternative materials (textures).  
Ie one model could have 3 different scenes, where the same accessors (position, uv) are used but different primitives that reference materials with varying number of texture channels.  

[nodeCount]  
This represent the number of traversed nodes in a scene.  
This is calculated by traversing nodes in each scene (depth or breadth first does not matter) - for each node increase the nodecount.  

[vertexCount]  
This represents the number of drawn vertices for each scene in the model.  
This is calculated by traversing the nodes in each scene.  
For each mesh use the POSITION attribute in each primitive, get the Accessor and add up the count field.  

[primitiveCount]  
This represents the number of primitives for each scene  
Each primitive references a material with 0 or more texture sources, has attributes and accessors.  


[textures]  
This value represents the texture sources that are used by a scene.  
This is the max value from the most 'complex' primitive that will be referenced by a Node in the scene.  
The goal of this metric is to provide a worst case texture usage where texturecount and complexity is known.  
Ie it is known if a texture is BASECOLOR or if part of PBR such as METALICROUGHNESS and now many texture sources that are used.  

[attributes]  
This value represent the total attribute buffer usage of the model. This value is calculated by adding up the size, in bytes, of all buffer objects in the Asset.  
This may give an indication of the runtime memory footprint of the buffers needed for the model.  

[textureSizes]
The total size of textures, without mipmaps, that are defined in the Asset.  
This is calculated by iterating the texture array and adding up the size of indexed images.  
The max texture size can be determined from these values.  
Some target devices may have a smaller (4096 * 4096) max texture size in which case the backend can choose to deliver another model.  

# JSON #  
This is how the output would be formatted using JSON  

```json
{
    "scene" : [
        {
            "vertexCount" : 4300,
            "nodeCount" : 20,
            "primitiveCount" : 50,
            "textures" : ["BASECOLOR", "METALLICROUGHNESS"]
        }
    ],
    "asset" : {
        "attributes" : 345234,
        "textureSizes" : [
            {
                "dimension" : [2048,2048]
            },
            {
                "dimension" : [512,512]
            },
            {
                "dimension" : [256,200]
            }
        ]
    }
}
```
