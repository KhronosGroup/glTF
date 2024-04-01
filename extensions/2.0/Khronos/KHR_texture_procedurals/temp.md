


## **Introduction**

The following is a design and discussion document for a proposed new Khronos extension to allow for shading networks to be specified within a glTF file.


### **Requirements**

The overarching requirements are listed here with each section adding additional follow-on requirements as necessary.



* The format must be compact enough and suitable for efficient transmission
* The format must be "easy" to generate and parse
* The format must be able to support the minimum requirements to represent MaterialX shade trees (and as follows, OpenUSD shade trees)

Initial Khronos requirements include:



* The format will only allow representation of "pattern" graphs (procedural "textures").
* The graphs are connectable to any existing downstream supported surface or displacement shader such as glTF PBR and unlit shading models.
    * The input connections or "mappings" on these shading models adhere to the current restrictions. We will call these "allowable" mappings.
* Graphs may be “instanced” by allowing for a definition to be specified along with a graph implementation.


## **Schema / Definitions**

The[ MaterialX "standard" library](https://github.com/AcademySoftwareFoundation/MaterialX/blob/main/libraries/stdlib/stdlib_defs.mtlx) specifies the "schema" / "definitions" allowed for procedural graphs. 

This includes:



* Allowed data types
* Minimal input and output representation requirements
* Minimal node representation requirements
* The allowable set of node type definitions.

All MaterialX library definitions have an implementation which can either be:



* A procedural shader graph
* Shader source code

This provides the "reference" implementation for each definition.

The implementations should be adhered to as closely as possible. It is assumed that simple intrinsic functions must be adhered to otherwise the result can vary too widely. e.g. basic “math” implementations must be adhered to. 

MaterialX code generation can optionally be used to provide implicit adherence for supported shading languages.This currently includes: GLSL, ESSL, Vulkan, OSL, MSL and MDL implementations. 

Examples of implementations which do not use MaterialX code generation include Houdini (VEX), AMD (ProRender), and ThreeJS.

Note that:



* Node definitions are strongly typed, disallowing connections between ports on nodes of differing types. This is true for MaterialX and OpenUSD..
* Nodes definitions can have polymorphic variations (e.g. an "add" node can have float, vec2, vec3, vec4, color3, color4 variants)


### **Versioning**



* _MaterialX version_: The definition libraries for MaterialX can change between full or patch releases. A MaterialX document only keeps a major and minor release version number of the form: “&lt;major>.&lt;minor>” This number should be kept with the procedural to indicate what version of the library was used. The MaterialX run-time provides utilities to perform upgrades to the version being executed.
* _Node definition versions_: Node definitions can have versions (as denoted by a version metadata tag of the form “&lt;major>.&lt;minor>.&lt;patch>”)


### **Definition Exclusions**

Exclusions:



* Node definitions which have inputs or outputs with these types are excluded:
    * arrays
    * structures (Note that a proposal for support for this is under discussion but has not been finalized)
* With the exception of filename inputs any node definitions with string inputs or outputs are excluded. This includes string enumerations which must be mapped to numbers.


### **Definition Representation**

MaterialX definitions are defined in XML format but a complimentary proposal is to allow for MaterialX to be representable as `JSON`. From this representation a glTF compatible JSON representation can be produced. 


## **Representation**


### In this section we define basic terminology.  \
 \
**A "Procedural"**

 \
A “procedural” represents a function. The function may simply pass data through without change.



* An atomic function is represented as a shading node which can have:
    * Data can be passed into anode  via one or more `input ports. `Instances of a given node may specify 0  or more of these inputs. If specified, these inputs override the default input value defined by the node definition.
    * Data can be passed out of a node via one or more `output ports.`All outputs specified on a node’s corresponding definition must be specified for each node instance.
* Nodes can be grouped into a `graph` to form more complex functions. Such graphs are composed of a DAG of shading nodes connected via their node  input and output ports.
* A `graph` can specify “`interface"` nodes to allow for data to flow between graph and non-graph elements. \

    * `graph inputs` are `interface` nodes that allow for data to flow to graph node inputs.
    * `graph output`s are `interface` nodes that allow for data to flow from graph node outputs to downstream destinations. \

* For glTF it is assumed that a  graph _cannot be nested _(contain another graph). Though allowed for OpenUSD all such configurations must be “flattened” to single level graphs. As MaterialX does not support  nested graphs OpenUSD utilities perform flattening upon conversion to MaterialX. \



### **A “Procedural Definition”**

Procedural definitions specify the interfaces (inputs and outputs) and its associated node graph implementation. This allows for custom definitions to be specified within a glTF document.

The structure for declaring definitions matches that used in MaterialX. This allows for these definitions to be convertible back into MaterialX and hence be consumable by UsdMtlx to create OpenUSD SDR (shading node registry) entries. That is, it is possible to create custom definitions within glTF which are inherently supported by OpenUSD integrations which support MaterialX definitions.



* A procedural definition element is composed of:
    * 0 or more inputs
    * 1 or more outputs
    * a definition identifier or “category”
    * an output type
    * a globally unique identifier

Nodes of a given procedural definition category can be instantiated in the exact same manner as definitions which come provided by MaterialX libraries.


### **Data Types**

inputs and outputs adhere to the types defined by MaterialX with the aforementioned restrictions.

The available types are:



* single float
* single integer
* tuples:
    * color3, color4 : 3 and 4 channel color
    * vector2, vector3, vector4 : 2, 3, 4 channel float vector
    * integer2, integer3, integer4 : 2, 3, 4 channel integer vector
* matrix:
    * 3x3, and 4x4 : Matrices of floats of size 3 or 4.


### **Internal Graph Connections**

Graph connections can be made:



* Between an `interface input` and a `node input`
* Between a node `output` and an `interface output`
* Between a node `output` and a `node input`.


### **External Non-Graph Connections**


#### **Graph Output Connections **



* A glTF material requires a root surface shader node. For MaterialX this must be a glTF PBR node (`gltf_pbr)`. For OpenUSD this would be the equivalent `Shader`’s identifier would be: `info:id=ND_gltf_pbr`.
* Any root node input can be connected to a `procedural` graph output if the PBR node input can currently accept an input binding.

**Graph Input Connections**



* Graph `input`s can either be bound to
    * a uniform value (constant). Values may be animated. 
    * a geometric stream.
    * a texture.


## **Storage**



* A flat **<code>procedurals</code> </strong>structure (<strong>array</strong>):
    * Contains one or more <code>procedural graphs</code>
    * For a given graph:
        * An input reference to an image is represented by an index to a <code>texture</code> element  within a glTF <code>textures</code> array.
        * An input reference to a stream is specified using a meshes <code>primitive</code> index number. \

* An optional read-only flat <strong>procedural_definitions </strong>structure (<strong>array</strong>):
    * Contains one or more procedural definitions and their associated nodegraph implementation.
    * These elements <strong>cannot be connected to nor modified</strong>.


## <strong>Correspondence with MaterialX / OpenUSD </strong>


### **Procedurals**

Each node instance or node graph instance (called a “**compound**” graph in MaterialX):



* Should have a string identifier (`name`). This `name` can be a graph path. it must be unique within the array to allow for reconstruction of a corresponding MaterialX / OpenUSD graph.
* Must have a node type. (e.g. an `add` node, an `image` node, a `noise` node, etc. or custom type).
* Must have an output type. 
* Must carry though any version identifiers. 
* May have additional meta-data for user information such as UI hints,

Each procedural definition:



* Must have a name identifier
* Must specify the node type as nodedef.
* Must have a 1 or more outputs
* Can have 1 or more inputs
* Can have a version identifier and an indication if it is default version
* May have additional meta-data for user information such as UI hints,

Each procedural node graph implementation (called a “**functional_”_** graph in MaterialX)



* Has a 1:1 association with a procedural definition. 
    * This is denoted by specifying a procedural definition identifier (index into the procedurals_definitions array)
* Has **no inputs **as these are defined by the procedural definition
* Has 1 or more outputs corresponding to the procedural definition.

The node type and output type:



* Determines the appropriate node graph or code implementation to use. Output type is required as definitions can be polymorphic.
* Determines the appropriate MaterialX or OpenUSD elements to be re-instantiated as necessary for interop.

It is recommended that real-world units (unit_) _and color space _(colorspace) _information be maintained for interop with MaterialX / OpenUSD. If not, then the appropriate conversion will need to be baked in the input values or textures.

Any additional meta-data may be carried through to the procedural declaration as designed. This includes data such as UI hints, node placement, documentation, etc. This should be provided as a tooling option.

It is possible to include precision information as is used by OpenUSD in this manner.


### **Type / Channel Handling**

If a system allows for type-less connections, then these integrations must insert the correct type conversion nodes. For example, if a `color3` is connected to a `vector3` input, then a `convert` node must be inserted.

A DCC and /or MaterialX itself may include implicit channel extraction as part of a connection specification. This must be made explicit by inserting the appropriate channel extraction nodes such as a `swizzle` or `extract` node.


## **Syntax**

The following outlines the JSON syntax. For the most part a  “_checkerboard_” example will be used with the graph and definition coming from MaterialX.


### **Extension Declaration**

Assuming the name of the extension is `KHR_texture_procedurals`, the following shows the extension being included in the `extensionsUsed` array.

 "extensionsUsed": [

    "KHR_texture_procedurals"

  ]

with the same extension listed under the extensions array:

  "extensionsUsed": [

    "**KHR_texture_procedurals**"

  ],

  "extensions": {

    "**KHR_texture_procedurals**": {

    }

  }


### **Mime type Information**

It is assumed that a mimetype is always required. As part of the mimetype a version is specified. This is the version of the MaterialX library specification used when writing to glTF. 

Below is an example of the mimetype and the associated version 1.39.


```
{
 "extensions": {
   "KHR_texture_procedurals": {
     "mimetype": "application/mtlx+json;version=1.39",
   }
 }
}
```


this would correspond to the following XML element in MaterialX:


```
<materialx version="1.39">
</materialx>
```


A single glTF file can **never **have more than one version of MaterialX nodes specified. It is up to the application to perform the appropriate upgrade to the runtime version of MaterialX being used. For example if one file has MaterialX 1.38 and another has 1.39, then the application must upgrade the 1.38 procedurals to 1.39.

Note that the version in the mimetype <span style="text-decoration:underline;">is not </span>the extension version. If in the future the schema needs to be modified (perhaps due to changes in MaterialX) then a new extension version would be required.



* This requires some more discussion. e.g. do we allow array ports to avoid a version change once they are supported in MaterialX?.


### **Declaring a  Procedurals Array**

The produrals array is declared within the extension within the “extensions” list. \
`{`


```
 "extensions": {
   "KHR_texture_procedurals": {
     "mimetype": "application/mtlx+json;version=1.39",
     "procedurals": []
   }
 }
```


`}`   


### **Declaring a  Procedural Graph**

Each nodegraph in MaterialX is encapsulated as a procedural graph.

 \
Given a MaterialX graph:


```
<nodegraph name="My_Checker">
</nodegraph>
```


the procedurals array would contain the following, noting the node type indicates it is a “graph”.


```
{
 "procedurals": [
   {
     "name": "My_Checker",
     "nodetype": "nodegraph"
   }
 ]
}
```



### **Declaring a Node**

Within the graph, one or more nodes can be declared within a **<code>nodes</code> </strong>child arrays

Below an example a node instance is declared by specifying both the node type (`nodetype)` and output type (`type`) to identify the appropriate `subtract `node to use.

The node declares two 2-vector inputs called `in1`and in2 under an “**inputs**”  array.  A single output called “out” is declared under the nodes “**outputs**” array.

 `{`


```
 // Procedurals list
 "procedurals": [
   {
     // Checkerboard graph
     "name": "My_Checker",
     "nodetype": "nodegraph",
     // List of nodes within the procedural graph
     "nodes": [
       {
         // Subtract node
         "name": "N_mtlxsubtract",
         "nodetype": "subtract",
         "type": "vector2",
         // Subtract node inputs
         "inputs": [
           {
             "name": "in1", // Name of input
             "type": "vector2", // Type of input
             "value": [
               1,
               1
             ] // Value of input
           },
           {
             "name": "in2",
             "type": "vector2",
             "value": [
               0,
               0
             ]
           }
         ],
         // Subtract node outputs
         "outputs": [
           {
             "name": "out",
             "type": "vector2"
           }
         ]
       }
     ]
   }
 ]
}
```


This corresponds to these nodes as specified using MaterialX XML syntax:

 `&lt;subtract name="N_mtlxsubtract" type="vector2">`


```
   <input name="in1" type="vector2" value="1,1 " />
   <input name="in2" type="vector2" value="0.0" />
   <output name= "out" type= "vector2" />
</subtract>
```


Note that in MaterialX, outputs are not explicitly specified on nodes but are required to determine output type without requiring referencing additional definition data.


### **Connecting Node Outputs to Node Inputs**

In this example a `multiply` node's output is connected to the previous subtract node's `in1 `input.

The connection is within the context of the “nodes” array and as such the connection is specified as an index into an entry of this array. 

To denote that the connection is to a node the “**node**” specifier is used to specify the index. The previous “**value**” specified is removed as having both a value and a connection is disallowed to avoid ambiguity as to what specifier has precedence.


```
{
 "nodes": [
   {
     // Subtract node occupies array index 0 in the nodes array
     "name": "N_mtlxsubtract",
     "nodetype": "subtract",
     "type": "vector2",
     "inputs": [
       {
         "name": "in1",
         "type": "vector2",
         "value": [
           0.4,
           0.5
         ]
       },
       {
         "name": "in2",
         "type": "vector2",
         "node": 1 // Reference multiply node
       }
     ],
     "outputs": [
       {
         "name": "out",
         "type": "vector2"
       }
     ]
   },
   {
     // Multiply node occupies array index 1 in the nodes array
     "name": "N_mtlxmultiply",
     "nodetype": "multiply",
     "type": "vector",
     "outputs": [
       {
         "name": "out",
         "type": "vector2"
       }
     ]
   }
 ]
}
```


with the corresponding MaterialX:


```
<nodegraph name="My_Checker">
   <subtract name="N_mtlxsubtract" type="vector2">
       <input name="in1" type="vector2" value= "0.4, 0.5" />
       <input name="in2" type="vector2" nodename="N_mtlxmult" />
       <output name= "out" type= "vector2">
   </subtract>
   <multiply name="N_mtlxmultiply" type="vector2" />
   <output name= "out" type= "vector2">
   </multiply>
</nodegraph>
```



#### **Connecting to Upstream Nodes with Multiple Outputs**

Some node definitions can have multiple outputs specified. When connecting a downstream input the specific upstream output must be explicitly specified. 

Thus an output specifier is required. This matches what is currently done in MaterialX. In OpenUSD the connection path includes the specific output port by name.

The following is an example which uses a separate node which has multiple outputs.

The MaterialX graph looks like this, with the separate node variant being the one for 3 channel color – thus having 3 outputs, 1 for each channel. 

 


```
<nodegraph name="nodegraph1">
   <!-- Separate node -->
   <separate3 name="separate3_color3" type="multioutput">
       <!-- 3 available outputs  -->
       <output name="outr" type="float" />
       <output name="outg" type="float" />
       <output name="outb" type="float" />
   </separate3>
   <!-- Combine node -->
   <combine3 name="combine3_color3" type="color3">
       <!-- Connect inputs to specific upstream outputs -->
       <input name="in1" type="float" output="outg" nodename="separate3_color3" />
       <input name="in2" type="float" output="outr" nodename="separate3_color3" />
       <input name="in3" type="float" output="outb" nodename="separate3_color3" />
   </combine3>
</nodegraph>
```


The corresponding procedural representation would look like this with the additional output qualifiers on the input connections: 0,1, 2 to disambiguate which output to use on the node connection referenced by index 0.

As noted previously, these outputs are not considered to be interface outputs and thus are **not **connectable to any downstream material input.


```
{
 "procedurals": [
   {
     "name": "nodegraph1",
     "nodetype": "nodegraph",
     "nodes": [
       // Separate node
       {
         "name": "nodegraph1/separate3_color3",
         "nodetype": "separate3",
         // Type is "multioutput" since there are multiple outputs
         "type": "multioutput",
         "outputs": [ // There are 3 possible outputs
           {
             "name": "outr",
             "type": "float"
           },
           {
             "name": "outg",
             "type": "float"
           },
           {
             "name": "outb",
             "type": "float"
           }
         ]
       }


       // Combine node
       {
         "name": "nodegraph1/combine3_color3",
         "nodetype": "combine3",
         "type": "color3",
         "inputs": [
           {
             "name": "in1",
             "type": "float",
             "node": 0,
             "output": 1 // Output specifier "outg"
           },
           {
             "name": "in2",
             "type": "float",
             "node": 0,
             "output": 0 // Output specifier "outr"
           },
           {
             "name": "in3",
             "type": "float",
             "node": 0,
             "output": 2 // Output specifier "outb"
           }
         ]
       }
     ]
   }
 }
]
}
```



### **Graph Inputs**

To route data into a graph, child inputs need to be declared within an “**inputs**” array. 

In the following example the checkerboard graph specifies the “inputs” array and shows one interface input called “color1”. 


```
{
 "procedurals": [
   {
     "name": "NG_checkerboard_color3",
     "nodetype": "nodegraph",
     // List of inputs
     "inputs": [
       {
         "name": "color1", // input 0
         "type": "color3",
         "value": [
           1.0,
           1.0,
           1.0
         ],
         "uiname": "Color 1" // Optional
       }
     ]
   }
}
```


the corresponding MaterialX would look like this:


```
<nodegraph name="My_Checker">
   <!-- Graph's interface inputs -->
   <input name="color1" type="color3" uiname="Color 1" value="1.0, 1.0, 1.0" />
</nodegraph>


```


Note that UI meta-data  such as “UI name” (uiname) can be optionally carried through for user display purposes.


### **Binding Resources to Node Inputs**


#### **Mapping Image Filenames**

This example shows the mapping of a MaterialX **filename **reference for one variant of an “image” node called “image_color3”. Note that there is no concept of binary references within MaterialX.

The input texture coordinates are specified by a “texture coordinate” node called `texcoord_vector2.`


```
<nodegraph name= "image_lookup">
   <image name="image_color3" type="color3">
       <input name="file" type="filename" value="myfile.png" />
       <input name="texcoord" type="vector2" nodename="texcoord_vector2" />
   </image>
   <texcoord name="texcoord_vector2" type="vector2">
       <input name="index" type="integer" value="1" />
   </texcoord>
</nodegraph>
```


The mapping to the nodes array for the node graph would look like this:


```
{
 "procedurals": [
   {
     "name": "image_lookup",
     "nodetype": "nodegraph",
     "nodes": [
       {
         "name": "image_color3",
         "nodetype": "image",
         "type": "color3",
         "inputs": [
           {
             "name": "file",
             "type": "filename",
             "texture": 0 // Reference textures array entry: 0
           },
           {
             "name": "texcoord",
             "type": "vector2",
             "node": 1 // Reference to texcoord node: 1
           }
         ]
       },
       {
         "name": "texcoord_vector2",
         "nodetype": "texcoord",
         "type": "vector2",
         "inputs": [
           {
             "name:": "index",
             "type": "integer",
             "value": 1 // Stream index
           }
         ]
       }
     ]
   }
 ]
}
```


The texture number referencing the textures array at index 0.

Texture transformations and sampling information needs to be parsed and set as appropriate on the texture entry. Below is a sample texture entry with the appropriate “source” setup which would use the filename value (“myfile.png”).


```
{
 "textures": [
   {
     "name": "image_color3",
     "source": 0, // "images" reference index
     "texCoord": 1,
     "extensions": {
       "KHR_texture_transform": { // Transform information         
       }
     },
     "sampler": 0 // "samplers" reference index
   }
 ]
}
```


The appropriate matching **image **entry needs to be added to the images array and the URI set to the MaterialX node’s original filename if using an external image reference. If it is not important to maintain the original filename then the image can be encoded as desired as a binary reference.

All filename resolution needs to be performed to determine the final URI. (e.g. resolving path prefixes (fileprefix) and any token substitutions). Note that this is “lossy” in that it is not possible to reconstruct the original unresolved MaterialX image filename.


```
{
 "images": [
   {
     "name": "image_color3",
     "uri": "myimage.png" // Resolved URI
   }
 ]
}
```


 

Any texture coordinate filtering information is remapped with sampler items being added as needed to the **samplers **array. 


```
{
 "samplers": [
   {
     "wrapS": 10497,
     "wrapT": 10497,
     "magFilter": 9729,
     "minFilter": 9986
   }
 ]
}
```


Texture transforms are procedural in nature and may **not** be mappable to a single texture coordinate matrix transform. A transform which is represented solely as a matrix may be mappable.


#### **Numbered Input Stream Binding**

A texture coordinate node (texcoord) specifies that the texture coordinate `primitive` stream is stream 1.  \
 \
Implementations will need to check for a stream with a label of `TEXCOORD_1` in a given meshes primitives. The mesh to search would be the one assigned to the material using the procedural graph.

See the glTF specification for [specific glTF stream names](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html). Note that currently there are no stream names for multiple sets of _tangents_,_ bitangents_, _normals _or _positions_.

Other stream type requirements such as color streams can be specified using an appropriate node corresponding to a MaterialX node. Below we show the usage of a “geomcolor” node. \
A value of 1 means that an implicit binding to color stream COLOR_1 is desired.


```
{
 "nodes": [
   {
     "name": "geomcolor_color3",
     "nodetype": "geomcolor",
     "type": "color3",
     "inputs": [
       {
         "name": "index",
         "type": "integer",
         "value": 1  // Stream index 1 == COLOR_1
       }
     ]
   }
 ]
}
```


Note that `COLOR_0` has an implied binding for modulation of base color. Thus index 0 (`COLOR_0`) should not be referenced in a procedural.


#### **Named Input Stream Handling**

Some APIs / DCCs use named stream references and their corresponding MaterialX nodes. These would need to be mapped to a numbered stream. A heuristic could be added such as "st0" in OpenUSD can be mapped to texcoord index 0. (This is under discussion at time of writing but has not be resolved as of yet )


### **Graph Outputs**


#### **Declaring an Output**

Outputs on a graph must be declared within an “outputs” array for a given node graph

For the checkerboard example we have one output called “out” as shown below


```
{
 "procedurals": [
   {
     "name": "My_Checker",
     "nodetype": "nodegraph",
     // List of outputs
     "outputs": [
       {
         "name": "out",
         "type": "color3"
       }
     ]
   }
 ]
}
```


The corresponding MaterialX would look like this:


```
<nodegraph name="My_Checker">
   <output name="out" type="color3" />
</nodegraph>
```


For the most part having an output which is not connected to anything is of no use as no data will flow through.


#### **Connecting To Upstream Nodes**

In the following example we connect a graph output to an upstream "mtlxmix" node to form the first example of a “complete” node graph where data flows into and out of the graph.


```
{
 "procedurals": [
   {
     "name": "My_Checker",
     "nodetype": "nodegraph",
     // Graph inputs
     "inputs": [
       {
         "name": "color1", // input 0
         "type": "color3",
         "value": [
           1.0,
           1.0,
           1.0
         ],
         "uiname": "Color 1" // Optional
       },
       {
         "name": "color2", // input 1
         "type": "color3",
         "value": [
           0,
           0,
           0
         ]
       }
     ],
     // Graph nodes
     "nodes": [
       {
         // Mix node at array index 0
         "name": "N_mtlxmix",
         "nodetype": "mix",
         "type": "color3",
         "inputs": [
           {
             "name": "bg",
             "type": "color3",
             "input": 1 // Reference color2 input specified at index 1 in the "inputs" array
           },
           {
             "name": "fg",
             "type": "color3",
             "input": 0 // Reference color1 input  specified at index 1 in the "inputs" array
           },
           {
             "name": "mix",
             "type": "float",
             "value": 0.3
           }
         ]
       }
     ],
     // Graph outputs
     "outputs": [
       {
         "name": "out",
         "type": "color3",
         "node": 0, // Reference "N_mtlxmix" at index 0 in the nodes array.
         "output": 0 // Optional. Not required as N_mtlxmix only has 1 output
       }
     ]
   }
 ]
}
```


The corresponding MaterialX looks like this:


```
<materialx version="1.38">
   <nodegraph name="My_Checker">
       <!-- Graph's interface inputs -->
       <input name="color1" type="color3" uiname="Color 1" value="1.0, 1.0, 1.0" />
       <input name="color2" type="color3" uiname="Color 2" value="0.0, 0.0, 0.0" />

       <!-- Mix node -->
       <mix name="N_mtlxmix" type="color3">
           <input name="bg" type="color3" interfacename="color2" />
           <input name="fg" type="color3" interfacename="color1" />
           <input name="mix" type="float" value="0.5" />
       </mix>

       <!-- Graph's interface outputs -->
       <output name="out" type="color3" nodename="N_mtlxmix" />
   </nodegraph>
</materialx>
```



### **Connecting Output to Downstream Root Node**

To connect a graph output to a surface or displacement shader input the procedural extension can be declared within the a texture reference for a given material.

In this example the extension is specified on the "baseColorTexture" entry. Using the previous procedural graph the index references the **node graph **at location 0 in the `procedurals` array. \
As with all connections an output is required if the upstream graph has more than one output.

Note that at the current time each texture reference requires an "index" entry to be specified. This can be used to reference the "fallback" texture to use instead of the procedural graph. This could be the “baked” version of the graph, or simply reference an embedded 1 pixel “dummy” image which directly resides inside the glTF document.


```
{
 "materials": [
   {
     "name": "default_checker",
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 0,
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0, // nodegraph instance direct usage
             "output": 0 // optional. Require if > 1 output on the graph
           }
         }
       }
     }
   },
   // unlit material:
   {
     "name": "gltf_pbr_surfaceshader",
     "extensions": {
       "KHR_materials_unlit": {}
     },
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 0,
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0 // nodegraph instance direct usage
           }
         }
       }
     }
   }
 ]
}
```


The fallback texture can be specified as shown. This example specifies a 1-pixel “pink” image. \
Note again that MaterialX does not support non-file image references so this should either be ignored or translated into a file texture. In general these textures should just be ignored.


```
{
 "images": [
   {
     "url": "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVQI12P4z/AfAAQAAf/zKSWvAAAAAElFTkSuQmCC"
   }
 ],
 "textures": [
   {
     "source": 0
   }
 ]
}
```


Note that a different fallback could be kept for different types of mappings such as a normal map versus a color mapping.


### **Keeping Baked and Procedural Options**

It is possible to keep both a procedural and a non-procedural version of the same graph. The non-procedural version would just use the existing workflow of "baking" the graph into \
an image.

In the example below we have each of two materials keep a `baseColorTexture` reference to a baked `texture`, while also keeping a `baseColorTexture` reference to a `procedural`:


```
{
 "materials": [
   {
     "name": "gltf_pbr_surfaceshader",
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 6, // Ref. to texture
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0 // Ref. to procedural
           }
         }
       }
     }
   }
   {
     "name": "unlit_surfaceshader",
     "extensions": {
       "KHR_materials_unlit": {}
     },
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 6, // Ref. to texture
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 3 // Ref. to procedural
           }
         }
       }
     }
   }
 ]
}
```



### **Node Graph Reuse**

In order to reuse a procedural node graph a definition and corresponding “functional” nodegraph must be declared.

 \
All declarations and functional graphs are specified in a separate procedural_definitions within the KHR_texture_procedurals extension parent.

It is possible to declare a definition for a procedural graph either just within the glTF document or convert a MaterialX node definition (nodedef) to reside within the glTF document. 



* The glTF document should **never** store any existing “standard” library definitions to avoid conflicts with MaterialX.
* _It is possible to convert the procedural definition into a MaterialX (nodedef) definition. After which the definition can be consumed by OpenUSD._

For a definition:



* nodetype: Must be “nodedef”
* node: Is the name of the node type and can be used wherever nodetype is specified for a node instance.
* The child lists of inputs and outputs are specified. Default geometric streams can be specified for some input types using the defaultgeomprop meta-data specifier. This would need to be converted to a numbered stream (_TBD how to map this_ )

For a functional nodegraph



* nodetype: Must be “nodegraph”
* nodedef : must be specified to show correspondence with the definition.
* A child list of outputs can be specified. The outputs must match those of the definition.
* **It is invalid to specify a list of inputs**.

Below is the actual interface for the “checkboard” definition as specified within Material. The graph nodes have been omitted for clarity.


```
{
 "procedural_definitions": [
   {
     // Definition
     "name": "ND_checkerboard_color3",
     "nodetype": "nodedef", // required
     "node": "checkerboard", // required - "nodetype" for instances of this definition
     "nodegroup": "texture2d", // optional but recommended to be interpreted as a "texture"
     "inputs": [
       {
         "name": "color1",
         "doc": "The first color used in the checkerboard pattern.",
         "type": "color3",
         "value": [
           1.0,
           1.0,
           1.0
         ]
       },
       {
         "name": "color2",
         "doc": "The second color used in the checkerboard pattern.",
         "type": "color3",
         "value": [
           0.0,
           0.0,
           0.0
         ]
       },
       {
         "name": "uvtiling",
         "doc": "The tiling of the checkerboard pattern along each axis, with higher values producing smaller squares. Default is (8, 8).",
         "type": "vector2",
         "value": [
           8,
           8
         ]
       },
       {
         "name": "uvoffset",
         "doc": "The offset of the checkerboard pattern along each axis. Default is (0, 0).",
         "type": "vector2",
         "value": [
           0,
           0
         ]
       },
       {
         "name": "texcoord",
         "doc": "The input 2d space. Default is the first texture coordinates.",
         "type": "vector2",
         "defaultgeomprop": "UV0"
       }
     ],
     "outputs": [
       {
         "name": "out",
         "type": "color3"
       }
     ]
   },
   // Functional node graph
   {
     "name": "NG_checkerboard_color3",
     "nodetype": "nodegraph", // required
     "type": "color3", // required
     "nodedef": 0, // required definition reference
     "outputs": [
       {
         "name": "out",
         "type": "color3"
       }
     ],
     "nodes": [
       // The nodes omitted...
     ]
   }
 }


```


The corresponding definition and functional graph in MaterialX looks like this.  \



```
<materialx version="1.38">
   <nodedef name="ND_checkerboard_color3" node="checkerboard" nodegroup="procedural2d">
       <input name="color1" type="color3" uiname="Color 1" value="1.0, 1.0, 1.0"
           doc="The first color used in the checkerboard pattern." />
       <input name="color2" type="color3" uiname="Color 2" value="0.0, 0.0, 0.0"
           doc="The second color used in the checkerboard pattern." />
       <input name="uvtiling" type="vector2" uiname="UV Tiling" value="8, 8"
           doc="The tiling of the checkerboard pattern along each axis, with higher values producing smaller squares. Default is (8, 8)." />
       <input name="uvoffset" type="vector2" uiname="UV Offset" value="0, 0"
           doc="The offset of the checkerboard pattern along each axis. Default is (0, 0)." />
       <input name="texcoord" type="vector2" uiname="Texture Coordinates" defaultgeomprop="UV0"
           doc="The input 2d space. Default is the first texture coordinates." />
       <output name="out" type="color3" />
   </nodedef>

   <!-- Node graph with reference to definition -->
   <nodegraph name= "NG_checkerboard_color3" type= "color" nodedef= "ND_checkerboard_color3">
       <output name="out" type="color3" />
       <!-- nodegraph implementation omitted  -->
   </nodegraph>
</materialx>   
```



### Using Custom Definitions


#### **Declaring Custom Definition Node Instances**

Once declared, a node instance of the custom type can be instantiated in the “procedurals” array and used in the same way as a node definition externally.

In the example below we create variants by declaring “checkerboard” node instances within a graph. We modify the inputs on each node to create a “red” and “green” variants.


```
{
 "procedurals": [
   // checkerboard compound graph is here at index 0

   // Checkerboard variants graph : array index 1
   {      
     "name": "checkboard_variants",
     "nodetype": "nodegraph",
      "nodes": [
        {
          "name": "checkerboard_green",
          "nodetype": "checkerboard", // Instance of a checkerboard
          "type": "color3",
         // Override one input's value so that it is "green"
         "inputs": [
            {
              "name": "color1",
              "type": "color3",
              "value": [
                0,
                1,
                0
              ]
            }
          ],
          "outputs": [
            {
              "name": "out",
              "type": "color3"
            }
          ]
        },
        {
          "name" : "checkboard_red",
          "nodetype": "checkerboard",
          "type": "color3",
          "inputs": [
            {
              "name": "color1",
              "type": "color3",
              "value": [
                1,
                0,
                0
              ]
            }
          ],
          "outputs": [
            {
              "name": "out",
              "type": "color3"
            }
          ]
        }
      ],
        // Route the outputs of the checkerboard nodes
        "outputs": [
        {
          "name": "green_out",
          "type": "color3",
          "node": 0
        },
        {
          "name": "red_out",
          "type": "color3",
          "node": 1
        }
      ]    
   }
 ]
}
```


Note that:



* The compound nodegraph for checker is not shown in the “procedurals” array for brevity. It is assumed in this example to occupy array location 0.
* Any number of inputs specified by the definition can be overridden. In this case “in1” color inputs on the “checkerboard” are overridden.


#### **Connecting Variants to a Material Texture Inputs**

The connections of custom nodes to a shader’s texture input is the same as before (by procedurals array index). Each material shown below references the graph with the variants. The “output” is specified to indicate which variant to connect to. In this case output 0 is the green variant and 1 the red variant.


```
{
 // Material references
 "materials": [
   {
     "name": "green_checker",
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 0,
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0, // graph with variants
             "output": 0 // "green checker" variant
           }
         }
       }
     },
     "name": "red_checker",
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 0,
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0, // graph with variants
             "output": 1 // "red checker" variant
           }
         }
       }
     }
   }
 ]
}
```



## Summary: Constructs and Connections

The following two diagrams shows:



1. The proposed constructs (in blue) 
2. The proposed organization and connections with existing glTF elements (in purple)

Each diagram is color coded, noting that 



1. Definitions and associated implementations in the “procedural_definitions” array are considered to be “immutable” and “unconnectable”. (In red)
2. Any other top-level node, nodegraph or stand-alone input can be modified and/or reference an appropriate glTF upstream texture or geometric stream as appropriate; and any top-level nodes, nodegraph, output can be connected to a downstream material input. (In green)


#### 

<p id="gdcalert1" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image1.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert2">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image1.png "image_tooltip")



#### 

<p id="gdcalert2" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image2.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert3">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image2.png "image_tooltip")



### JSON Schema

Below is an initial proposed JSON schema.

Inputs and outputs are derivations of “ports”. Note that slight differences nodegraph input and node input as a node input cannot reference a interface input since this would be a self-referential connection.


```
{
 "$schema": "http://json-schema.org/draft-07/schema#",
 "type": "object",

 "definitions" : {


   "port" :
   {
     "doc" : "A stand-alone, node or nodegraph port object",
     "type": "object",
     "properties": {
       "name": { "type": "string" },
       "nodetype": { "type": "string" },
       "type": { "type": "string" }
     },
     "required": ["nodetype", "type"]
   },

   "typedef":
   {
     "_doc:" : "Supported type definition strings",
     "type" : "string",      
     "enum": ["float", "integer", "boolean", "string", "filename", "color3", "color4", "vector2", "vector3", "vector4", "matrix33", "matrix44", "multioutout" ]
   },


   "value":
   {
     "doc": "Input value. Single number, boolean, string, or array of numbers",
     "oneOf": [
       {
         "type": "array",
         "minItems": 2,
         "maxItems": 4,
         "items": { "type": "number" }
       },
       { "type": "string" },
       { "type": "boolean" },
       { "type": "number" }
     ]
   },

   "graph_input" :
   {
     "doc" : "Input port on a node graph. Value or a connection to a texture",
     "allOf": [
       { "$ref": "#/definitions/port" },
       { "type" : "object",
         "properties": {
           "name": { "type": "string" },
           "nodetype": { "type": "string", "const" : "input" },
           "type": { "$ref": "#/definitions/typedef" },
           "value": { "$ref": "#/definitions/value" }
         },
         "required": ["nodetype", "type"],
         "oneOf" : [
             {
               "properties": { "value": { "$ref": "#/definitions/value" } },
               "required": ["value"]
             },
             {
               "properties": { "texture": { "type": "integer" }},
               "required": ["texture"]
             }
           ]
       }
     ]
   },

   "node_input" :
   {
     "doc" : "An input port on a node. Value or a connection must be specified",
     "allOf": [
       { "$ref": "#/definitions/port" },
       { "type" : "object",
         "properties": {
           "name": { "type": "string" },
           "nodetype": { "type": "string", "const": "input" },
           "type": {  "$ref": "#/definitions/typedef" }
         },
         "required": ["nodetype", "type"],
         "oneOf" : [
             {
               "properties": { "value": { "$ref": "#/definitions/value" } },
               "required": ["value"]
             },
             {
               "properties": {
                 "node": {
                   "type": "integer",
                   "_doc": "A connection to a node in the nodes array"
                 },
                 "output": {
                   "type": "integer",
                   "_doc": "Connection to an output on node. Required if > 1 output"
                 } 
               },
               "required": ["node"]                             
             },
             {
               "_doc": "A connection to a graph input",
               "properties": { "input": { "type": "integer" }},
               "required": ["input"]
             },
             {
               "_doc:" : "A connection to a texture in the textures array",
               "properties": { "texture": { "type": "integer" }},
               "required": ["texture"]
             }
         ]
       }
     ]
   },

   "output_port" : {
     "_doc": "A stand-alone, node or nodegraph output port",
     "type": "object",     
     "properties": {
       "name": { "type": "string" },
       "nodetype": { "type": "string" },
       "type": { "$ref": "#/definitions/typedef" }
     },
     "required": ["nodetype", "type"]
   },

   "node" : {
     "_doc": "A node in a node graph",
     "type": "object",
     "properties": {
       "name": { "type": "string" },
       "nodetype": {
         "_doc": "Library node category or one defined in procedural_definitions",
         "type": "string"
       },
       "type": { "type": "string" },
       "inputs": {
         "type": "array",
         "items": {
           "$ref": "#/definitions/node_input"
         }
       },
       "outputs": {
         "type": "array",
         "items": {
           "$ref": "#/definitions/output_port"
         }
       }
     },
     "required" : ["nodetype", "type", "outputs"]
   },

   "nodegraph" : {
     "type": "object",
     "properties": {
       "name": { "type": "string" },
       "nodetype": {
         "type": "string",
         "const": "nodegraph"
       },
       "type": {  "$ref": "#/definitions/typedef" },

       "inputs": {
         "type": "array",
         "items" : {"$ref": "#/definitions/graph_input"}
       },

       "outputs": {
         "type": "array",
         "items": { "$ref": "#/definitions/output_port" }
       },

       "nodes": {
         "type": "array",
         "items": { "$ref": "#/definitions/node" }
         }
     },
     "required" : ["nodetype", "type", "outputs", "nodes"]     
   },

   "definition_item" : {
     "type": "object",
     "properties": {
       "name": { "type": "string" },
       "nodetype": { "type": "string" },
       "type": { "$ref": "#/definitions/typedef" }
     },
     "required" : ["nodetype", "type"]
   },

   "nodedef" : {
     "_doc": "A node definition",
     "allOf": [
       { "$ref": "#/definitions/definition_item" },
       { "type" : "object",
         "properties": {
           "node" : {
             "_doc": "Node classification used by nodetype references",
             "type": "string"
           },
           "nodetype" : {
             "type" : "string",
             "const" : "nodedef"
           },
           "version": {
             "_doc" : "Version of the classification",
             "type": "string"
           },
           "nodegroup": {
             "_doc" : "Grouping. Should be provided",
             "type": "string",
             "default": "texture2d"
           },
           "isdefaultversion": { "type": "boolean" },

           "inputs": {
             "type": "array",
             "items" : {"$ref": "#/definitions/graph_input"}
           },

           "outputs": {
             "type": "array",
             "items": { "$ref": "#/definitions/output_port" }
           }
         },
         "required" : ["node", "outputs", "inputs"]     
       }
     ]
   },

   "nodegraph_implementatnion" : {
     "_doc": "A node graph that is a definition's implementation.",
     "allOf": [
       { "$ref": "#/definitions/definition_item" },
       { "type" : "object",
         "properties": {
           "nodetype" : {
             "type" : "string",
             "const" : "nodegraph"
           },
           "nodedef": {
             "_doc": "The node definition for this function",
             "type": "number"
           },

           "outputs": {
             "type": "array",
             "items": { "$ref": "#/definitions/output_port" }
           },

           "nodes": {
             "type": "array",
             "items": { "$ref": "#/definitions/node" }
           }
         },
         "required" : ["nodedef", "outputs", "nodes"]   
       }             
     ]
   }
 },

 "properties": {
   "materials": {
     "type": "array",
     "items": {
       "type": "object",
       "properties": {
         "extensions": {
           "type": "object",
           "properties": {
             "KHR_materials_unlit": { "type": "object" }
           }
         },
         "name": { "type": "string" },
         "pbrMetallicRoughness": {
           "type": "object",
           "properties": {
             "baseColorTexture": {
               "type": "object",
               "properties": {
                 "index": { "type": "integer" },
                 "extensions": {
                   "type": "object",
                   "properties": {
                     "KHR_texture_procedurals": {
                       "type": "object",
                       "properties": {
                         "index": { "type": "integer" },
                         "output": { "type": "integer" }
                       },
                       "required": ["index"]
                     }
                   },
                   "required": ["KHR_texture_procedurals"]
                 }
               }
             }
           }
         }
       },
       "required": ["pbrMetallicRoughness"]
     }
   },
   "textures": {
     "type": "array",
     "items": {
       "type": "object",
       "properties": {
         "source": { "type": "integer" }
       },
       "required": ["source"]
     }
   },
   "images": {
     "type": "array",
     "items": {
       "type": "object",
       "properties": {
         "uri": { "type": "string" },
         "name": { "type": "string" }
       },
       "required": ["uri"]
     }
   },
   "extensionsUsed": {
     "type": "array",
     "items": { "type": "string" }
   },
   "extensions": {
     "type": "object",
     "properties": {
       "KHR_texture_procedurals": {
         "type": "object",
         "properties": {
           "mimetype": { "type": "string" },

           "procedurals": {
             "_doc": "A list of procedural texture graphs",
             "type": "array",
             "items": { "$ref": "#/definitions/nodegraph" }
           },

           "procedural_definitions": {
             "_doc": "A list of node definitions and corresponding implementations",
             "type": "array",
             "items": { "$ref": "#/definitions/definition_item" }
           }
         },
         "required": ["mimetype", "procedurals"]
       }
     },
     "required": ["KHR_texture_procedurals"]
   }
 },
 "required": ["materials", "extensionsUsed", "extensions"]
}
```



## Examples


### Full Checkerboard Example



<p id="gdcalert3" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image3.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert4">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image3.png "image_tooltip")


The full checkerboard example is shown below:



<p id="gdcalert4" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image4.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert5">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image4.png "image_tooltip")



```
{
 "materials": [
   {
     "name": "gltf_pbr_surfaceshader",
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 0,
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0
           }
         }
       }
     }
   }
 ],
 "textures": [
   {
     "source": 0
   }
 ],
 "images": [
   {
     "uri": "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVQI12P4z/AfAAQAAf/zKSWvAAAAAElFTkSuQmCC",
     "name": "KHR_texture_procedural_fallback"
   }
 ],
 "extensionsUsed": [
   "KHR_texture_procedurals"
 ],
 "extensions": {
   "KHR_texture_procedurals": {
     "mimetype": "application/mtlx+json;version=1.38",
     "procedurals": [
       {
         "name": "My_Checker",
         "nodetype": "nodegraph",
         "type": "color3",
         "inputs": [
           {
             "name": "color1",
             "nodetype": "input",
             "type": "color3",
             "value": [
               1.0,
               0.0,
               0.0
             ]
           },
           {
             "name": "color2",
             "nodetype": "input",
             "type": "color3",
             "value": [
               0.0,
               1.0,
               0.0
             ]
           },
           {
             "name": "uvtiling",
             "nodetype": "input",
             "type": "vector2",
             "value": [
               8.0,
               8.0
             ]
           },
           {
             "name": "uvoffset",
             "nodetype": "input",
             "type": "vector2",
             "value": [
               0.0,
               0.0
             ]
           }
         ],
         "outputs": [
           {
             "name": "out",
             "nodetype": "output",
             "type": "color3",
             "node": 5
           }
         ],
         "nodes": [
           {
             "name": "N_mtlxmult",
             "nodetype": "multiply",
             "type": "vector2",
             "inputs": [
               {
                 "name": "in1",
                 "nodetype": "input",
                 "type": "vector2",
                 "node": 6
               },
               {
                 "name": "in2",
                 "nodetype": "input",
                 "type": "vector2",
                 "input": 2
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "vector2"
               }
             ]
           },
           {
             "name": "N_mtlxsubtract",
             "nodetype": "subtract",
             "type": "vector2",
             "inputs": [
               {
                 "name": "in1",
                 "nodetype": "input",
                 "type": "vector2",
                 "node": 0
               },
               {
                 "name": "in2",
                 "nodetype": "input",
                 "type": "vector2",
                 "input": 3
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "vector2"
               }
             ]
           },
           {
             "name": "N_mtlxfloor",
             "nodetype": "floor",
             "type": "vector2",
             "inputs": [
               {
                 "name": "in",
                 "nodetype": "input",
                 "type": "vector2",
                 "node": 1
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "vector2"
               }
             ]
           },
           {
             "name": "N_mtlxdotproduct",
             "nodetype": "dotproduct",
             "type": "float",
             "inputs": [
               {
                 "name": "in1",
                 "nodetype": "input",
                 "type": "vector2",
                 "node": 2
               },
               {
                 "name": "in2",
                 "nodetype": "input",
                 "type": "vector2",
                 "value": [
                   1.0,
                   1.0
                 ]
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "float"
               }
             ]
           },
           {
             "name": "N_modulo",
             "nodetype": "modulo",
             "type": "float",
             "inputs": [
               {
                 "name": "in1",
                 "nodetype": "input",
                 "type": "float",
                 "node": 3
               },
               {
                 "name": "in2",
                 "nodetype": "input",
                 "type": "float",
                 "value": 2.0
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "float"
               }
             ]
           },
           {
             "name": "N_mtlxmix",
             "nodetype": "mix",
             "type": "color3",
             "inputs": [
               {
                 "name": "bg",
                 "nodetype": "input",
                 "type": "color3",
                 "input": 1
               },
               {
                 "name": "fg",
                 "nodetype": "input",
                 "type": "color3",
                 "input": 0
               },
               {
                 "name": "mix",
                 "nodetype": "input",
                 "type": "float",
                 "node": 4
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "color3"
               }
             ]
           },
           {
             "name": "texcoord",
             "nodetype": "texcoord",
             "type": "vector2",
             "inputs": [
               {
                 "name": "index",
                 "nodetype": "input",
                 "type": "integer",
                 "value": 1
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "vector2"
               }
             ]
           }
         ]
       }
     ]
   }
 }
}
```


The checkerboard as a definition looks like the following:


```
{
    "textures": [
        {
            "source": 0
        }
    ],
    "images": [
        {
            "uri": "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVQI12P4z/AfAAQAAf/zKSWvAAAAAElFTkSuQmCC",
            "name": "KHR_texture_procedural_fallback"
        }
    ],
    "extensionsUsed": [
        "KHR_texture_procedurals"
    ],
    "extensions": {
        "KHR_texture_procedurals": {
            "mimetype": "application/mtlx+json;version=1.38",
            "procedural_definitions": [
                {
                    // Definition
                    "name": "ND_checkerboard_color3",
                    "nodetype": "nodedef", // required
                    "node": "checkerboard", // required - "nodetype" for instances of this definition
                    "nodegroup": "texture2d", // optional but recommended to be interpreted as a "texture"
                    "inputs": [
                        {
                            "name": "color1",
                            "doc": "The first color used in the checkerboard pattern.",
                            "type": "color3",
                            "value": [
                                1.0,
                                1.0,
                                1.0
                            ]
                        },
                        {
                            "name": "color2",
                            "doc": "The second color used in the checkerboard pattern.",
                            "type": "color3",
                            "value": [
                                0.0,
                                0.0,
                                0.0
                            ]
                        },
                        {
                            "name": "uvtiling",
                            "doc": "The tiling of the checkerboard pattern along each axis, with higher values producing smaller squares. Default is (8, 8).",
                            "type": "vector2",
                            "value": [
                                8,
                                8
                            ]
                        },
                        {
                            "name": "uvoffset",
                            "doc": "The offset of the checkerboard pattern along each axis. Default is (0, 0).",
                            "type": "vector2",
                            "value": [
                                0,
                                0
                            ]
                        },
                        {
                            "name": "texcoord",
                            "doc": "The input 2d space. Default is the first texture coordinates.",
                            "type": "vector2",
                            "defaultgeomprop": "UV0"
                        }
                    ],
                    "outputs": [
                        {
                            "name": "out",
                            "type": "color3"
                        }
                    ]
                },
                // Functional node graph
                {
                    "name": "NG_checkerboard_color3",
                    "nodetype": "nodegraph", // required
                    "type": "color3", // required
                    "nodedef": 0, // required definition reference
                    "outputs": [
                        {
                            "name": "out",
                            "type": "color3"
                        }
                    ],
                    "nodes": [
                        {
                            "name": "N_mtlxmult",
                            "nodetype": "multiply",
                            "type": "vector2",
                            "inputs": [
                                {
                                    "name": "in1",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "node": 6
                                },
                                {
                                    "name": "in2",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "input": 2
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "vector2"
                                }
                            ]
                        },
                        {
                            "name": "N_mtlxsubtract",
                            "nodetype": "subtract",
                            "type": "vector2",
                            "inputs": [
                                {
                                    "name": "in1",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "node": 0
                                },
                                {
                                    "name": "in2",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "input": 3
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "vector2"
                                }
                            ]
                        },
                        {
                            "name": "N_mtlxfloor",
                            "nodetype": "floor",
                            "type": "vector2",
                            "inputs": [
                                {
                                    "name": "in",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "node": 1
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "vector2"
                                }
                            ]
                        },
                        {
                            "name": "N_mtlxdotproduct",
                            "nodetype": "dotproduct",
                            "type": "float",
                            "inputs": [
                                {
                                    "name": "in1",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "node": 2
                                },
                                {
                                    "name": "in2",
                                    "nodetype": "input",
                                    "type": "vector2",
                                    "value": [
                                        1.0,
                                        1.0
                                    ]
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "float"
                                }
                            ]
                        },
                        {
                            "name": "N_modulo",
                            "nodetype": "modulo",
                            "type": "float",
                            "inputs": [
                                {
                                    "name": "in1",
                                    "nodetype": "input",
                                    "type": "float",
                                    "node": 3
                                },
                                {
                                    "name": "in2",
                                    "nodetype": "input",
                                    "type": "float",
                                    "value": 2.0
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "float"
                                }
                            ]
                        },
                        {
                            "name": "N_mtlxmix",
                            "nodetype": "mix",
                            "type": "color3",
                            "inputs": [
                                {
                                    "name": "bg",
                                    "nodetype": "input",
                                    "type": "color3",
                                    "input": 1
                                },
                                {
                                    "name": "fg",
                                    "nodetype": "input",
                                    "type": "color3",
                                    "input": 0
                                },
                                {
                                    "name": "mix",
                                    "nodetype": "input",
                                    "type": "float",
                                    "node": 4
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "color3"
                                }
                            ]
                        },
                        {
                            "name": "texcoord",
                            "nodetype": "texcoord",
                            "type": "vector2",
                            "inputs": [
                                {
                                    "name": "index",
                                    "nodetype": "input",
                                    "type": "integer",
                                    "value": 1
                                }
                            ],
                            "outputs": [
                                {
                                    "nodetype": "output",
                                    "name": "out",
                                    "type": "vector2"
                                }
                            ]
                        }
                    ]
                }
            ],
            "procedurals": [
                // checkerboard compound graph is here at index 0
                // Checkerboard variants graph : array index 1
                {
                    "name": "checkboard_variants",
                    "nodetype": "nodegraph",
                    "nodes": [
                        {
                            "name": "checkerboard_green",
                            "nodetype": "checkerboard", // Instance of a checkerboard
                            "type": "color3",
                            // Override one input's value so that it is "green"
                            "inputs": [
                                {
                                    "name": "color1",
                                    "type": "color3",
                                    "value": [
                                        0,
                                        1,
                                        0
                                    ]
                                }
                            ],
                            "outputs": [
                                {
                                    "name": "out",
                                    "type": "color3"
                                }
                            ]
                        },
                        {
                            "name": "checkboard_red",
                            "nodetype": "checkerboard",
                            "type": "color3",
                            "inputs": [
                                {
                                    "name": "color1",
                                    "type": "color3",
                                    "value": [
                                        1,
                                        0,
                                        0
                                    ]
                                }
                            ],
                            "outputs": [
                                {
                                    "name": "out",
                                    "type": "color3"
                                }
                            ]
                        }
                    ],
                    // Route the outputs of the checkerboard nodes
                    "outputs": [
                        {
                            "name": "green_out",
                            "type": "color3",
                            "node": 0
                        },
                        {
                            "name": "red_out",
                            "type": "color3",
                            "node": 1
                        }
                    ]
                }
            ]
        }
    },
    {
        // Material references
        "materials": [
            {
                "name": "green_checker",
                "pbrMetallicRoughness": {
                    "baseColorTexture": {
                        "index": 0,
                        "extensions": {
                            "KHR_texture_procedurals": {
                                "index": 0, // graph with variants
                                "output": 0 // "green checker" variant
                            }
                        }
                    }
                },
                "name": "red_checker",
                "pbrMetallicRoughness": {
                    "baseColorTexture": {
                        "index": 0,
                        "extensions": {
                            "KHR_texture_procedurals": {
                                "index": 0, // graph with variants
                                "output": 1 // "red checker" variant
                            }
                        }
                    }
                }
            }
        ]
    }
}
```



### Explicit Channel Extraction Example

The following graph extracts a given channel from a color and then combines it before output:


```
<?xml version="1.0"?>
<materialx version="1.38">
 <nodegraph name="nodegraph1">
   <output name="output_color3" type="color3" xpos="7.021739" ypos="-4.482759" nodename="combine3_color3" />
   <extract name="extract_color3" type="float" xpos="1.333333" ypos="-5.517241">
     <input name="in" type="color3" interfacename="input_color3" />
     <input name="index" type="integer" value="1" />
   </extract>
   <combine3 name="combine3_color3" type="color3" xpos="4.014493" ypos="-4.965517">
     <input name="in1" type="float" nodename="extract_color3" />
   </combine3>
   <input name="input_color3" type="color3" value="0.716381, 0.642354, 0.0122608" xpos="-1.268116" ypos="-5.913793" />
 </nodegraph>
 <gltf_pbr name="gltf_pbr_surfaceshader" type="surfaceshader" xpos="6.768116" ypos="-7.681035">
   <input name="base_color" type="color3" output="output_color3" nodegraph="nodegraph1" />
 </gltf_pbr>
 <surfacematerial name="surfacematerial" type="material" xpos="11.246377" ypos="-7.413793">
   <input name="surfaceshader" type="surfaceshader" nodename="gltf_pbr_surfaceshader" />
 </surfacematerial>
</materialx>
```


 \
This results in the follow procedurals: \
 \
`{`


```
 "materials": [
   {
     "name": "gltf_pbr_surfaceshader",
     "pbrMetallicRoughness": {
       "baseColorTexture": {
         "index": 0,
         "output": 0,
         "extensions": {
           "KHR_texture_procedurals": {
             "index": 0
           }
         }
       }
     }
   }
 ],
 "textures": [
   {
     "source": 0
   }
 ],
 "images": [
   {
     "uri": "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVQI12P4z/AfAAQAAf/zKSWvAAAAAElFTkSuQmCC",
     "name": "KHR_texture_procedural_fallback"
   }
 ],
 "extensionsUsed": [
   "KHR_texture_procedurals"
 ],
 "extensions": {
   "KHR_texture_procedurals": {
     "mimetype": "application/mtlx+json;version=1.38",
     "procedurals": [
       {
         "name": "nodegraph1",
         "nodetype": "nodegraph",
         "type": "color3",
         "inputs": [
           {
             "name": "input_color3",
             "nodetype": "input",
             "type": "color3",
             "value": [
               0.716381,
               0.642354,
               0.0122608
             ]
           }
         ],
         "outputs": [
           {
             "name": "output_color3",
             "nodetype": "output",
             "type": "color3",
             "node": 1
           }
         ],
         "nodes": [
           {
             "name": "extract_color3",
             "nodetype": "extract",
             "type": "float",
             "inputs": [
               {
                 "name": "in",
                 "nodetype": "input",
                 "type": "color3",
                 "input": 0
               },
               {
                 "name": "index",
                 "nodetype": "input",
                 "type": "integer",
                 "value": 1
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "float"
               }
             ]
           },
           {
             "name": "combine3_color3",
             "nodetype": "combine3",
             "type": "color3",
             "inputs": [
               {
                 "name": "in1",
                 "nodetype": "input",
                 "type": "float",
                 "node": 0
               }
             ],
             "outputs": [
               {
                 "nodetype": "output",
                 "name": "out",
                 "type": "color3"
               }
             ]
           }
         ]
       }
     ]
   }
 }
}
```



## **Validation / Proof of Concept**


### **Direct Consumption **


#### **NVIDIA MDL Prototype **



<p id="gdcalert5" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/image5.png). Store image on your image server and adjust path/filename/extension if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert6">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/image5.png "image_tooltip")




* Parses the JSON to map to equivalent [MDL functions](https://github.com/AcademySoftwareFoundation/MaterialX/tree/main/source/MaterialXGenMdl/mdl/materialx)
* These functions are the same implementations used for MaterialX code generation
* Parsing logic is “straight-forward” and small (few hundred lines) with sufficient information thus far to construct the appropriate MDL for rendering. “name” is not required in this context.
* Geometry bindings are “straight-foward” since they map to equivalent MDL functions already.
* Further prototyping forth-coming. \


**Possible Targets**



* ThreeJS Node Graphs (model-viewer?), Babylon Node Graphs, sample-viewer etc.
* Meta integration ?


#### **Reconversion back to MaterialX**



* Any MaterialX consumer could use this.
* Adds bi-directional interop check and graph reconstruction.


## **Items on Queue **



* NPR nodes.
* Finalize syntax