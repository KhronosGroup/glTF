# Runtime Performance Metrics 

The goal of these metrics is to assess the runtime performance needs of glTF assets.  
By runtime we mean the cpu/gpu, bandwidth and memory resources needed to render the model in realtime.  
These performance metrics are divided in two major categories - `complexity` and `memory`

## Performance Metric Categories

### Complexity  

The `complexity`category provides metrics to calculate cpu/gpu and memory bandwidth target requirements - larger numbers will mean a more powerful target device is needed.  
These metrics do not provide a mapping to target device performance, this mapping must be done by users of the metrics.  

### Screen space coverage

The metrics do not include screen space coverage.  
However, implementations should generally consider the size of the render area in addition to performance metrics of the model.  
A higher performance budget may be acceptable when screen space coverage is higher.  
For most usecases it's best to assume a worst case where all or most of the screen space is filled with rendered pixels.  


### Memory  

The `memory` category provides metrics to calculate the runtime memory (size) needs of the model.  
Using these metrics an estimate of target memory can be calculated - however keep in mind that the exact memory requirements will vary with viewer implementations.  



|                 |             |          |                                                                |             |
|-----------------|-------------|-------------|----------|----------------------------------------------------------------|
| Name            | Type        | Category    | Property |Description                                                     |
|[vertexCount]    | Integer     | Complexity  |Scene     |Total number of vertices used by the Nodes in a scene           |  
|[nodeCount]      | Integer     | Complexity  |Scene     |Max nodecount in scene (add upp all nodes in a scene)           |  
|[primitiveCount] | Integer     | Complexity  |Scene     |Total number of referenced primitives (per scene).  This figure is the un-batched number of primitives, engines may optimize if primitives and meshes share textures. |  
|[textures]       | Integer     | Complexity  |Scene     |Flags specifying presence of materials texture usage, this is the aggregated most complex usage. BASECOLOR, METALLICROUGHNESS, NORMAL, OCCLUSION, EMISSIVE, SPECULARGLOSS, DIFFUSE, CLEARCOAT |  
|[materials]      | Integer     | Complexity  |Scene     | Flags specifying presence of materials, this is the aggregated most complex usage.  |
|[maxNodeDepth]   | Integer     | Memory      | Scene    | The max node depth of the scene, ie the max number of parent/child relations. This number will represent the max stack depth needed when traversing the nodegraph |
|[accessors]     | Accessor     | Memory      | Asset    | Total number and format of vertex accessors, this can be used to calculate vertex buffer memory requirements |  
|[textureSize]  |Integer    | Memory      | Asset    | The size and format of textures |  

Dimension is an Integer[2] containing width and height  
Accessor is an object containing count, componentType and type, componentType and type is taken from glTF Accessor  


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
The goal of this metric is to provide a worst case texture usage cost for rendering a pixel.  

`textures`  
1 BASECOLOR
2 METALLICROUGHNESS  
3 NORMAL  
4 OCCLUSION  
5 EMISSIVE  
6 SPECULARGLOSS  
7 DIFFUSE  
8 CLEARCOAT  


[materials]  
This value represents the materials used by a scene.  
This is the max value from the most 'complex' primitive that will be referenced by a Node in the scene.  
The goal of this metric is to provide a worst case computational cost for rendering a pixel.  
  
1 PBR
2 SPECULARGLOSS (KHR_materials_pbrSpecularGlossiness)
2 UNLIT (KHR_materials_unlit) 


[accessors]  
This value represent the total vertex accessor usage of the model.  
It is calculated by adding upp the accessors in the model, storing count, componentType and type.  
This may give an indication of the runtime memory footprint of the buffers needed for the model.  
componentType and type are taken from glTF specification for Accessor object.  
`componentType`  
5120 BYTE  
5121 UNSIGNED_BYTE  
5122 SHORT  
5123 UNSIGNED_SHORT  
5125 UNSIGNED_INT  
5126 FLOAT  

`type`  
"SCALAR"  
"VEC2"  
"VEC3"  
"VEC4"  
"MAT2"  
"MAT3"  
"MAT4"  


[textureSize]  
The size and format of textures, without mipmaps, that are defined in the Asset.  
This is calculated by iterating the texture array and adding the size and format of indexed images.  
Formats are as follows [4, 3, 2, 1] :
4 component, eg (RGBA)    
3 component, eg (RGB)    
2 component, eg (MR)    
1 component, eg (O)  

It is up to the client to calculate the actual memory requirements on the target device as this may vary depending on hardware and (future) compressed format support.

The client can use these values to get the max texture size.  
Some target devices may have a smaller (4096 * 4096) max texture size in which case the client can choose to deliver another model.  

# JSON #  
This is how the output would be formatted using JSON  

```json
{
    "scene" : [
        {
            "vertexCount" : 4300,
            "nodeCount" : 20,
            "primitiveCount" : 50,
            "textures" : ["BASECOLOR", "METALLICROUGHNESS"],
            "materials" : ["PBR", "SPECULARGLOSS"],
            "maxNodeDepth" : 7
        }
    ],
    "asset" : {
        "accessors" : [
            {
                "count" : 4980,
                "componentType" : 5125,
                "type" : "VEC3"
            }
        ],
        "textureSize" : [
            {
                "dimension" : [2048,2048],
                "format" : 4
            },
            {
                "dimension" : [512,512],
                "format" : 1
            },
            {
                "dimension" : [256,200],
                "format" : 3
            }
        ]
    }
}
```
