
--- Name -----------Property    ----- Description ----------------------------
[verticecount]      Scene       Total number of vertices used by a model in a scene
[nodecount]         Scene       Max nodecount in scene (add upp all nodes in a scene)
[primitivecount]    Scene       Total number of referenced primitives (per scene)
                                This figure is the un-batched number of primitives, engines may optimize if primitives and meshes share textures.
[textures]          Scene       Flags specifying presence of materials and it's texture usage, this is the aggregated max usage
                                BASECOLOR, METALLICROUGHNESS, NORMAL, OCCLUSION, EMISSIVE, (SPECULARGLOSS)


"scene" : [ { "verticecount" : 4300, "nodecount" : 20, "primitivecount" : 50 ,
              "textures" : ["BASECOLOR", "METALLICROUGHNESS"]} ]


[nodecount]
This represent the number of traversed nodes in a scene.
This is calculated by traversing nodes in each scene (depth or breadth first does not matter) - for each node increase the nodecount.

[verticecount]
This represents the number of drawn vertices for each scene in the model.
This is calculated by traversing the nodes in each scene.
For each mesh use the POSITION attribute in each primitive, get the Accessor and add up the count field.

[primitivecount]
This represents the number of primitives for each scene

[textures]
This value represents the texture sources that are used by a scene.
This is the max value from the most 'complex' primitive that will be referenced by a primitive in the scene.
The goal of this metric is to provide a worst case texture usage where texturecount and complexity is known.
Ie it is known if a texture is BASECOLOR or if part of PBR such as METALICROUGHNESS
