# Runtime GLTF Metrics 

The goal of these metrics is to assess the runtime needs of glTF assets.  
By runtime we mean the cpu/gpu, bandwidth and memory resources needed to render the model in realtime.  
These performance metrics are divided in two major categories - `complexity` and `memory`

## Metric Categories

### Complexity  

The `complexity`category provides metrics to calculate cpu/gpu and memory bandwidth target requirements - larger numbers will mean a more powerful target device is needed.  
These metrics do not provide a mapping to target device performance, this mapping must be done by users of the metrics.  

### Screen space coverage

The metrics do not include screen space coverage.  
However, implementations should generally consider the size of the render area in addition to performance metrics of the model.  
A higher performance budget may be acceptable when screen space coverage is higher.  
For most usecases it's best to assume a worst case where all or most of the screen space is filled with rendered pixels.  
Here `screen space coverage` is defined as the number of pixels rendered in the output framebuffer.  

### Bounding volume

A bounding volume, on each scene, is included as a measurement of the world space that the model occupies.  
The intention of this is to be able to estimate the screen space size of the model in for instance AR/VR usecases.  
glTF units are equal to metric meters [1.0 = 1.0m] meaning that using the bounding volume the client can estimate the portion of the visible display it will occupy.  
A volume with dimensions [0.5,0.5,0.5] viewed at 50 cm will cover a large portion of the screen and a volume with dimensions [0.05,0.05,0.05] viewed at 50 cm will cover only a small portion.  


### Memory  

The `memory` category provides metrics to calculate the runtime memory (size) needs of the model.  
Using these metrics an estimate of target memory can be calculated - however keep in mind that the exact memory requirements will vary with viewer implementations.  



|                 |             |          |                                                                |             |
|-----------------|-------------|-------------|----------|----------------------------------------------------------------|
| Name            | Type        | Category    | Property |Description                                                     |
|[drawCount]      | Integer     | Complexity  |Scene     |Total number of vertices drawn in a scene                       |  
|[nodeCount]      | Integer     | Complexity  |Scene     |Max nodecount in scene (add upp all nodes in a scene)           |  
|[primitiveCount] | Integer     | Complexity  |Scene     |Total number of referenced primitives (per scene).  This figure is the un-batched number of primitives, engines may optimize if primitives and meshes share textures. |  
|[channels]       | Integer     | Complexity  |Scene     |Flags specifying presence of materials texture usage, this is the aggregated most complex usage. BASECOLOR, METALLICROUGHNESS, NORMAL, OCCLUSION, EMISSIVE, SPECULARGLOSS, DIFFUSE, CLEARCOAT |  
|[bounds]         | Bounds      | Complexity  | Scene    | The static 3D bounds for each scene, can be used to estimate world space size of model. |
|[maxNodeDepth]   | Integer     | Memory      | Scene    | The max node depth of the scene, ie the max number of parent/child relations. This number will represent the max stack depth needed when traversing the nodegraph |
|[accessors]     | Accessor     | Memory      | Asset    | Total number and format of vertex accessors, this can be used to calculate vertex buffer memory requirements |  
|[buffers  ]     | Buffer       | Memory      | Asset    | Size and number of buffers in the Asset, memory needed to load model. For a .glb this will include textures |  
|[textureSize]   |Integer       | Memory      | Asset    | The size and number of components of textures |  

Dimension is an Integer[2] containing width and height  
Accessor is an object containing count, componentType and type, componentType and type is taken from glTF Accessor  
Bounds is an object containing min and max values.    

# Scene #  

Metrics that describe properties of a scene.  
glTF models may contain multiple scenes, only one scene will be visible at a time.  
To accomodate this the following metrics are calculated on a per-scene basis.  
It is possible to reference different meshes (nodes) using different scenes, this makes it possible to have the same texture assets but reference different primitives that use alternative materials (textures).  
Ie one model could have 3 different scenes, where the same accessors (position, uv) are used but different primitives that reference materials with varying number of texture channels.  

[nodeCount]  
This represent the total number of traversed nodes in a scene.  
This is calculated by traversing nodes in each scene (depth or breadth first does not matter) - for each node increase the nodecount.  

[drawCount]  
This represents the number of drawn vertices for each scene in the model.  
This is calculated by traversing the nodes in each scene.  
For each mesh use the POSITION attribute in each primitive, get the Accessor and add up the count field.  

[primitiveCount]  
This represents the number of primitives for each scene  
Each primitive references a material with 0 or more texture sources, has attributes and accessors.  

[channels]  
This value represents the texture source channels that are used by a scene.  
This is the max value from the most 'complex' primitive that will be referenced by a Node in the scene.  
The goal of this metric is to provide a worst case texture usage and processing cost for rendering a pixel. 
Please note that the processing cost for the different channels will vary, for instance the EMISSIVE or  
BASECOLOR channels are quite cheap but other channels may add a signification processing overhead.  

**Note**  
This value can not be used to know if texture channels are tightly packed according to uniform Sampler usage,  
just the shader texel lookup and processing needed to calculate a pixel on screen.   

`channels`  
1 BASECOLOR
2 METALLICROUGHNESS  
3 NORMAL  
4 OCCLUSION  
5 EMISSIVE  
6 SPECULARGLOSS  

[bounds]  
This value represents the static (non animated) bounding volume for the scene.  
It is calculated by traversing the nodes in each scene, applying node transform to primitive's MinMax values and updating the result.  
The resulting MinMax will contain the total transformed volume the model occupies, ie concatenated MinMax of all primitives as they are transformed.  
This will include translation, meaning that the bounds will not be centered.  


[accessors]  
This value represents the total vertex accessors defined in the asset, it is an indication of the memory requirements for the asset.  
These values does not take into account if an acessor is unused or only used in some scene.  

Note that this value will not take into account if normal/tangent/bitangent values are defined or shall be runtime computed.  
It is calculated by adding upp the accessors in the model, storing count, componentType and type.  
This may give an indication of the runtime memory footprint of the buffers needed for the model.  
componentType and type are taken from glTF specification for Accessor object.  
`count`
Number of components, ie 4 VEC3 FLOATs will require 4 * 3 * 4 bytes of memory (on most devices)

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

[buffers]  
This represents the .glb buffer or the .bin buffers referenced by a .gltf file.  
To load a model, at least this much memory is required.  
For a .gltf the textures are usually supplied separately.  

`sizeInBytes`
Size in bytes of the buffer.

[textureSize]  
The size and number of components of textures, without mipmaps, that are defined in the Asset.  
This is calculated by iterating the texture array and adding the size and components of indexed images.  

`dimension`  
Width and height, in pixels of the texture.  

`components`
The texture component count, currently only supports 8 bit per component.  
Components are as follows [4, 3, 2, 1] :  
4 components, eg (RGBA)    
3 components, eg (RGB) - this is likely to occupy 32 bits on modern hw.  
2 components, eg (MR)    
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
            "channels" : ["BASECOLOR", "METALLICROUGHNESS"],
            "bounds" : "min": [
                -0.9999999403953552,
                -1.0,
                -1.0
             ],
             "max": [
                0.9999999403953552,
                1.0,
                1.0
            ]
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
                "components" : 4
            },
            {
                "dimension" : [512,512],
                "components" : 1
            },
            {
                "dimension" : [256,200],
                "components" : 3
            }
        ]
    }
}
```
