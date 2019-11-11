# Draco Bitstream Specification

Contributors:
- Jim Eckerlein
- ...
- 

# File Format Overview

- Draco Header
- Metadata (optional)
- Connectivity
- Attributes

# Conventions

## Variable Types

All integer and floating point formats are little endian.
- `float` is IEEE 754 Single precision.
- `var32` is an unsigned 32-bit integer encoded using LEB128.
- `var64` is an unsigned 64-bit integer encoded using LEB128.
- `f{n}` is an unsigned `n`-bit number with decreasing digit place value. The number is appended by padding bits to the next byte so that data remains byte-aligned.



## Functions 

- stack push 
- LEB128
- 

 
# Draco Header

Name | Type | Description
-|-|-
Draco String | `uint8[5]` | Must equal "DRACO"
Major version | `uint8` | Bitstream major version number
Minor version | `uint8` | Bitstream minor version number
Encoder type | `uint8` | `0`: Point cloud, `1`: Triangular mesh
Encoder method | `uint8` | `0` Sequential encoding, `1` Mesh edge breaker encoding 
Flags | `uint16` | `0` No flag, `0x8000` Meta data present 


## Meta data

This section is only present if `DracoHeader.Flags` contains the flag `0x8000`.
It contains a list of meta-data records which relate to specific attributes and a global meta data record. Each meta data record contains a name-value pair list together with a list of sub meta data records, forming a meta data record tree. Each sub meta data record is a name-meta-data pair. The `i`'th  attribute meta data record relates the attribute referenced by the `i`'th attribute meta data id.

To avoid ambiguities, each id inside the attribute meta data id array must be unique. Otherwise, multiple meta data records would relate to the same attribute.

Name | Type
-|-
Count of meta data records | `var32`
Array of attribute meta data ids | `var32[]`
Array of attribute meta data records | `Metadata[]`
Global meta data | `Metadata`

`Metadata` designates a single meta data record. Each record can contain zero or more name-value entries. `Metadata` is a recursive type as it can contain zero or more sub meta data records where each sub meta data record is a name-meta data pair.

`Metadata`:

- Count of entries: `var32`
- Entry array:
	- Name size: `uint8`
	- Name: `int8[]`
	- Value size: `uint8`
	- Value: `int8[]`
- Count of sub meta data records: `var32`
- Sub meta data array:
	- Name size: `uint8`
	- Name: `int8[]`
	- Sub meta data: `Metadata` (Here, the type recurses. This is not a reference or index.)




# Connectivity  


[ToDo] where is underlying point data stored?
->attributes data contains positions, normals, texCoords
->varying kinds of attributes are stored separately
->does this hold for edge breaker?

In general, this format stores connectivity and geometry information separately. 
In this section, it is shown how connectivity information is stored and how this connectivity information is used to construct faces.

Sequential connectivity methods preserve the underlying connectivity order, whereas usually higher compression rates cen be expected when using the EdgeBreaker algorithm.
 

There are two options for storing the connectivity, defined in encoder_method (located in the draco header):

Based on this, the correct connectivity encoder or decoder has to be chosen:
* `MESH_SEQUENTIAL_ENCODING`: Sequential Connectivity 
* `MESH_EDGEBREAKER_ENCODING`:  Edgebreaker Connectivity 

Sequential Connectivity stores indices of faces subsequently while the EdgeBreaker connectivity uses a sophisticated traversing scheme to construct faces. 
The following chart shows different possibilities of storing connectivity information that can be used in *DRACO* compressed files.

![alt text](figures/ConnectivityOverview.svg "Connectivity Overview")



# Sequential Connectivity

The sequential connectivity block starts with a preceding header. The header defines the encoding and the number of elements contained in the subsequent index block. 

## Header Block

| Variable        | Data Type           | |
| ------------- |:-------------:|:-------------:|  
|  num_faces       |          `var32`          | Stores the overall number of faces |
|  num_points       |           `var32`          | Stores the overall number of points |
|  connectivity_method |               `uint8`          |Sequential compressed (0) or uncompressed (1) indices  |
 



## Index Block

The index block can be either compressed or uncompressed. 
 
This is defined by the connectivity flag, which can have these values:

*  0: `SEQUENTIAL_COMPRESSED_INDICES`
*  1: `SEQUENTIAL_UNCOMPRESSED_INDICES`




### Uncompressed Sequential Indices()

The data type of index elements is dependent on the number of overall points. 

Hence, before parsing sequential indices, the element data type of one index element has to be evaluated depending on the maximum number of points (num_points) that have to be referenced in order to use a minimum viable size. 
The size of the data type is one to four bytes. Given the overall number of points, the following table shows the related data type format.


|  num_points       |    Required Bytes  |    Index Element Data Type    | 
| ----------------- |:------------------:| :----------------------------:| 
| 2^8     (256)     |  1                 |             `uint8`          | 
| 2^16    (65536)   |  2                 |             `uint16`         | 
| 2^21  (2097152)   |  3                 |             `var32`          | 
| 2^32 (4294967296) |  4                 |             `uint32`         | 
 
With this information, the size of the uncompressed sequential index block is given by the size of one index element data type and the number of faces:
sequential_index_block_size = sizeof(index_element) * num_faces * 3


One face is defined by exactly three point indices, hence, forming a triangle when substituting respective indices with point data. 

Three indices forming one face are stored subsequently. 

Face[i]IndexA 

![alt text](figures/IndexDiagram.svg "Index Diagram")



Face0[VertexIDA, VertexIDB, VertexIDC] Face1[VertexIDA, VertexIDB, VertexIDC]


One face is consisting of 3 indices:
face_to_vertex[0][i]
face_to_vertex[1][i]
face_to_vertex[2][i]


Parse sequential indices depending on evaluated index element size (UI8, UI16, varUI32, or UI32)

void ParseSequentialIndices() 
{
  for (i = 0; i < num_faces; ++i) 
  {
    for (j = 0; j < 3; ++j) 
	{
      face_to_vertex[j][i]                                                  uint8    uint16    var32  uint32
    }
  }
}





### Compressed Sequential Indices 



The compression of sequential indices is based on the algorithm shown by Jarek Duda in
*Asymmetric numeral systems: entropy coding combining speed of Huffman coding with compression rate of arithmetic coding* (2014):
https://arxiv.org/pdf/1311.2540.pdf

Specifically, the range variant (rANS) is used for compression, which is suitable for large coding alphabets. 

The decompressed data of size num_faces * 3 * sizeof(`uint32`) stores index differences, for what reason the uint32 has to be reinterpreted to a signed value.
The conversion from raw buffer values to signed integer values is done by shifting the bits to the right by one digit. The consumed rightmost bit determines the sign of the value respectively (1 represents a negative value, 0 represents a positive value). 

Obtained signed integer values represent the differences of subsequent indices. This means, an additional interim stage is needed to generate index values by stepping through the buffer and summing up the values. At each buffer position, the current sum represents the final index value.

1) Decompress -> Buffer of uint32[num_faces*3]
2) uint32 -> int32 (by consuming rightmost bit)
3) Index Differences -> Indices

The compression comprises two steps. 
At first, the index difference is calculated. This is based on the idea of usually encountering small differences between subsequent indices. 

Decoded Index Element in C++ == uint32


Meaning of Rightshift operation (>>)?
From C++ Code:
uint32_t encoded_val = indices_buffer[vertex_index++];
int32_t index_diff = (encoded_val >> 1);

ABCD >> 1 ==  0ABC ??

vs
From Spec: 
"Bits shifted into the MSBs as a result of the right shift have a value equal to the MSB of a prior to the shift operation."
ABCD >> 1 ==  AABC ??



	void DecodeSequentialCompressedIndices() 
	{
	  DecodeSymbols(num_faces * 3, 1, &decoded_symbols); //num_symbols, num_components, out_values
	  last_index_value = 0;
	  for (i = 0; i < num_faces; ++i) 
	  {
		for (j = 0; j < 3; ++j) 
		{
		  encoded_val = decoded_symbols[i * 3 + j];
		  index_diff = (encoded_val >> 1); //shift to the right. first bit is thrown away in this step? see rightshift definition mismatch
		  if (encoded_val & 1)
			index_diff = -index_diff;
		  val = index_diff + last_index_value;
		  face_to_vertex[j][i] = val;
		  last_index_value = val;
		}
	  }
	}





![Sequential Compressed Index Diagram](figures/SequentialCompressedIndexDiagram.svg "Sequential Compressed Index Diagram")




# EdgeBreaker Connectivity

Where can we find information?

Section 7  ->  EdgeBreaker Decoder

Section 8  -> EdgeBreaker Traversal

Section 9 ->  EdgeBreaker Traversal Valence

Section 10 ->  EdgeBreaker Traversal Prediction Degree

------

Overview


Master function: DecodeEdgebreakerConnectivityData
1) Header Block 
* ParseEdgebreakerConnectivityData 
  
	

2) Topology Split Events 
* ParseTopologySplitEvents
* ProcessSplitData();



3)  EdgebreakerTraversalStart 

	Traversal data is loaded in this section
* 8.5 EdgebreakerTraversalStart:
  * DecodeEdgebreakerTraversalStandardData()
  * EdgeBreakerTraversalValenceStart()


4)  DecodeEdgeBreakerConnectivity

	In this section, the actual connectivity is constructed. 
* =spirale reversi paper algortihm????
* =mesh_edgebreaker_decoder_impl.h ???


## Header Block




| Variable        | Data Type           | |
| ------------- |:-------------:|:-------------:|  
| edgebreaker_traversal_type | `uint8`| STANDARD_EDGEBREAKER, VALENCE_EDGEBREAKER
|  num_encoded_vertices       |          `var32`          |  Number of encoded vertices  |
|  num_faces       |           `var32`          | Number of encoded faces  |
|  num_attribute_data |               `uint8`          |  Number of encoded attributes |
|  num_encoded_symbols       |          `var32`          | Number of encoded EdgeBreaker symbols, which form the sequence   |
|  num_encoded_split_symbols       |           `var32`          | Number of encoded EdgeBreaker split symbols  |

edgebreaker_traversal_type:
*  STANDARD_EDGEBREAKER -> ParseEdgebreakerStandardSymbol
*  VALENCE_EDGEBREAKER -> EdgebreakerValenceDecodeSymbol

## Topology Split Events


Topological split events that can not be reconstructed via standard EdgeBreaker symbols are stored separately. 
At first,  one var32 value (`num_topology_splits`) is loaded that determines the number of topological split events . 
Using this value, delta values of split IDs (var32) and source IDs (var32) are loaded, i.e., one `source_id_delta` value and one `split_id_delta` value is loaded interleaved `num_topology_splits` times as shown in the following figure.  
Afterwards, `num_topology_splits` bits are read from the file that are stored in the array `source_edge_bit`. 
After reading those bits, the file reader is padded to the current byte. 

![ParseTopologySplitEvents](figures/ParseTopologySplitEvents.svg "ParseTopologySplitEvents")

------

Before loaded split events can be interpreted during EdgeBreaker traversal, delta values have to be converted to absolute values. 

[//]: # (Comment: Function is 7.5.ProcessSplitData)     

The following input is used to calculate absolute values: `source_id_delta[num_topology_splits]`, `split_id_delta[num_topology_splits]`

For all values in the array `source_id_delta` an entry in the array `source_symbol_id` is created by accumulating all values until that entry:

    source_symbol_id[i] = source_id_delta[0] + ... + source_id_delta[i]  // i = [0 .. num_topology_splits]

Note that these sums can be calculated by using a helper variable that is accumulated in a loop that assigns respective values.


The split symbol IDs in the array `split_symbol_id` are calculated by subtracting loaded `split_id_delta` values from previously calculated `source_symbol_id` values:

    split_symbol_id[i] = source_symbol_id[i]  - split_id_delta[i];  // i = [0 .. num_topology_splits]




Overall, the following data is provided for EdgeBreaker traversal and connectivity decoding: 

* `source_symbol_id[num_topology_splits]` 
* `split_symbol_id[num_topology_splits]` 
* `source_edge_bit[num_topology_splits]` 


ToDo: Define IsTopologySplit here or in traversal???
	
	
    bool IsTopologySplit(encoder_symbol_id, out_face_edge, out_encoder_split_symbol_id) 
	{
	  if (source_symbol_id.back() != encoder_symbol_id)
		return false;
 
	  out_face_edge = source_edge_bit.pop_back();
	  out_encoder_split_symbol_id = split_symbol_id.pop_back();
	  source_symbol_id.pop_back();
 
	 return true;
	}




## EdgeBreaker Traversal Data

Dependent on the EdgeBreaker header attribute `edgebreaker_traversal_type`, either standard traversal or valence EdgeBreaker traversal is accomplished.
Before the connectivity can be reconstructed, traversal type dependent data has to be loaded as shown in the next sections. 

* If `edgebreaker_traversal_type` is `STANDARD_EDGEBREAKER`, data is loaded as described in section *Standard EdgeBreaker Traversal Data*.
* If `edgebreaker_traversal_type` is `VALENCE_EDGEBREAKER`, data is loaded as described in section *Valence EdgeBreaker Traversal Data*.


   
[//]: # (   see Sec. 8.5 EdgeBreaker traversal type is controlled by attribute edgebreaker_traversal_type )
[//]: # (  STANDARD_EDGEBREAKER DecodeEdgebreakerTraversalStandardData)
[//]: # (  VALENCE_EDGEBREAKER EdgeBreakerTraversalValenceStart)

Furthermore:
 last_symbol_ = -1;
 active_context_ = -1;



### Standard EdgeBreaker Traversal Data

[//]: # (8.4. DecodeEdgebreakerTraversalStandardData)

The standard EdgeBreaker algorithm requires the following data:
* Symbol data (`eb_symbol_buffer`)
* Face data (`eb_start_face_buffer`)
* Attribute connectivity data (for each attribute one `attribute_connectivity_decoders_buffer`)


#### Symbol Data
[//]: # ( 8.1 ParseEdgebreakerTraversalStandardSymbolData )

[//]: # ( -> initializes eb_symbol_buffer of size [eb_symbol_buffer_size])  

To load the symbols required for the EdgeBreaker traversal, at first, a var64 value is loaded that defines the number of symbols. 
Afterwards, that number of symbols of type uint8 is loaded into the buffer `eb_symbol_buffer`.



#### Face Data
[//]: # ( 8.2 ParseEdgebreakerTraversalStandardFaceData )
 
[//]: # (  initializes eb_start_face_buffer of size [eb_start_face_buffer_size])

To load the face data required for the EdgeBreaker traversal, at first, a uint8 value is loaded into `eb_start_face_buffer_prob_zero`.
Then, a var32 value is loaded that defines the number of start face IDs. 
That number of start face IDs of type uint8 is loaded into the buffer `eb_start_face_buffer`.



#### Attribute Connectivity Data
[//]: # ( 8.3 ParseEdgebreakerTraversalStandardAttributeConnectivityData )
[//]: # (-> for  i = 0; i < num_attribute_data; ++i) 
[//]: # (-> initializes attribute_connectivity_decoders_buffer[i] with size specified in attribute_connectivity_decoders_size[i])

The number of attribute connectivity data that has to be loaded is defined in the EdgeBreaker header by the variable `num_attribute_data`.
For each attribute data element `i`, i.e., `num_attribute_data` times, a porobability value of type uint8 is loaded into `attribute_connectivity_decoders_prob_zero[i]`, 
a var32 value is loaded that indicates the number of connectivity decoders (`attribute_connectivity_decoders_size[i]`), 
and that number of uint8 values is loaded into `attribute_connectivity_decoders_buffer[i]`.

These buffers are used to decode the attribute seams.
[//]: # ( see 13.1 DecodeAttributeSeams) 


### Valence EdgeBreaker Traversal Data 

[ToDo]


## Decode EdgeBreaker Connectivity

	 is_vert_hole_.assign(num_encoded_vertices + num_encoded_split_symbols, true);
	 last_vert_added = -1;


The previous sections have defined, how data has to be loaded. 
In this section, the interpretation of the data is described based on the EdgeBreaker algorithm to decode the actual connectivity.

Additionally, a global array `is_vert_hole_` of size `num_encoded_vertices` + `num_encoded_split_symbols` and type bool is filled in this section to detect vertices that are located at the surface boundary. 
Initially, for all entries of that array are set to *true*.


The number of EdgeBreaker symbols `num_encoded_symbols` is specified in the EdgeBreaker header.
The actual symbols are stored in eb_symbol_buffer with a specific bit pattern. This is why the size of encoded symbols is not directly related to the size of the symbol buffer (`eb_symbol_buffer`). 
Note that symbols are stored in reverse order, i.e., they can be used directly for constructing the connectivity without prior reordering.
With respect to the selected traversal type `edgebreaker_traversal_type`, which is also specified in the EdgeBreaker header, the symbol is decoded for the valence EdgeBreaker or the standard EdgeBreaker algorithm.

[//]: # ( edgebreaker_traversal_type is VALENCE_EDGEBREAKER or STANDARD_EDGEBREAKER)

In this routine, one symbol after the other is loaded until `num_encoded_symbols` have been decoded. 
In the case of standard EdgeBreaker traversal, i.e., `edgebreaker_traversal_type` is `STANDARD_EDGEBREAKER`, the symbol decoding consists of two steps.
At first, one bit is read from `eb_symbol_buffer`. 
If this bit equals *0*, the current symbol is of type `TOPOLOGY_C` and no additional bits have to be read to decode this symbol. If the bit equals *1*, two additional bits are read. The following table shows the mapping of these 3 read bits to a topology symbol and symbol ID:


|Topology Symbol| Bit Pattern   | Topology ID |
| ------------- |:-------------:|:---------:|  
|  TOPOLOGY_C	|   0			|0
|  TOPOLOGY_S   |   100			|1
|  TOPOLOGY_L   |   110			|2
|  TOPOLOGY_R	|   101         |3
|  TOPOLOGY_E   |   111         |4

For each decoded symbol, the current symbol ID, i.e., a counter that is incremented for each decoded symbol, and the ID of the active corner (the current symbol ID multiplied by 3) are processed together with the decoded topology symbol in the following way:

### NewActiveCornerReached
Input: topology symbol, symbol ID, corner ID





	void NewActiveCornerReached(new_corner, symbol_id) 
	{
	  check_topology_split = false;

	  switch (last_symbol_) 
		{
		case TOPOLOGY_C:
		  {
			corner_a = active_corner_stack.back();
			corner_b = Previous(corner_a);

			while (PosOpposite(corner_b) >= 0) 
			{
			  b_opp = PosOpposite(corner_b);
			  corner_b = Previous(b_opp);
			}

			SetOppositeCorners(corner_a, new_corner + 1);
			SetOppositeCorners(corner_b, new_corner + 2);
			active_corner_stack.back() = new_corner;

		  }
		  vert = CornerToVert(curr_att_dec, Next(corner_a));
		  next = CornerToVert(curr_att_dec, Next(corner_b));
		  prev = CornerToVert(curr_att_dec, Previous(corner_a));
		
		  face_to_vertex[0].push_back(vert);
		  face_to_vertex[1].push_back(next);
		  face_to_vertex[2].push_back(prev);

		  is_vert_hole_[vert] = false;

		  MapCornerToVertex(new_corner, vert);
		  MapCornerToVertex(new_corner + 1, next);
		  MapCornerToVertex(new_corner + 2, prev);

		  break;


		case TOPOLOGY_S:
		  {
			corner_b = active_corner_stack.pop_back();
			for (i = 0; i < topology_split_id.size(); ++i) {
			  if (topology_split_id[i] == symbol_id) {
				active_corner_stack.push_back(split_active_corners[i]);
			  }
			}
			corner_a = active_corner_stack.back();
			SetOppositeCorners(corner_a, new_corner + 2);
			SetOppositeCorners(corner_b, new_corner + 1);
			active_corner_stack.back() = new_corner;
		  }

		  vert = CornerToVert(curr_att_dec, Previous(corner_a));
		  next = CornerToVert(curr_att_dec, Next(corner_a));
		  prev = CornerToVert(curr_att_dec, Previous(corner_b));
		  MapCornerToVertex(new_corner, vert);
		  MapCornerToVertex(new_corner + 1, next);
		  MapCornerToVertex(new_corner + 2, prev);
		  corner_n = Next(corner_b);
		  vertex_n = CornerToVert(curr_att_dec, corner_n);
	
		  ReplaceVerts(vertex_n, vert);
		 
		  face_to_vertex[0].push_back(vert);
		  face_to_vertex[1].push_back(next);
		  face_to_vertex[2].push_back(prev);
		  UpdateCornersAfterMerge(new_corner + 1, vert);
		  vertex_corners_[vertex_n] = kInvalidCornerIndex;
		  break;


		case TOPOLOGY_R:
		  {
			corner_a = active_corner_stack.back();
			opp_corner = new_corner + 2;
			SetOppositeCorners(opp_corner, corner_a);
			active_corner_stack.back() = new_corner;
		  }
		  check_topology_split = true;
		  vert = CornerToVert(curr_att_dec, Previous(corner_a));
		  next = CornerToVert(curr_att_dec, Next(corner_a));
		  prev = ++last_vert_added;
		 

		  face_to_vertex[0].push_back(vert);
		  face_to_vertex[1].push_back(next);
		  face_to_vertex[2].push_back(prev);

		  MapCornerToVertex(new_corner + 2, prev);
		  MapCornerToVertex(new_corner, vert);
		  MapCornerToVertex(new_corner + 1, next);
		  break;


		case TOPOLOGY_L:
		  {
			corner_a = active_corner_stack.back();
			opp_corner = new_corner + 1;
			SetOppositeCorners(opp_corner, corner_a);
			active_corner_stack.back() = new_corner;
		  }
		  check_topology_split = true;
		  vert = CornerToVert(curr_att_dec, Next(corner_a));
		  next = ++last_vert_added;
		  prev = CornerToVert(curr_att_dec, Previous(corner_a));
		  

		  face_to_vertex[0].push_back(vert);
		  face_to_vertex[1].push_back(next);
		  face_to_vertex[2].push_back(prev);

		  MapCornerToVertex(new_corner + 2, prev);
		  MapCornerToVertex(new_corner, vert);
		  MapCornerToVertex(new_corner + 1, next);
		  break;


		case TOPOLOGY_E:
		  active_corner_stack.push_back(new_corner);
		  check_topology_split = true;
		  vert = last_vert_added + 1;
		  next = vert + 1;
		  prev = next + 1;
		  face_to_vertex[0].push_back(vert);
		  face_to_vertex[1].push_back(next);
		  face_to_vertex[2].push_back(prev);
		  last_vert_added = prev;
		  MapCornerToVertex(new_corner, vert);
		  MapCornerToVertex(new_corner + 1, next);
		  MapCornerToVertex(new_corner + 2, prev);
		  break;
	  }




	 

	  if (check_topology_split) 
	  {
		encoder_symbol_id = num_encoded_symbols - symbol_id - 1;
		while (IsTopologySplit(encoder_symbol_id, &split_edge,
							   &enc_split_id)) {
		  act_top_corner = active_corner_stack.back();
		  if (split_edge == RIGHT_FACE_EDGE) {
			new_active_corner = Next(act_top_corner);
		  } else {
			new_active_corner = Previous(act_top_corner);
		  }
		  // Convert the encoder split symbol id to decoder symbol id.
		  dec_split_id = num_encoded_symbols - enc_split_id - 1;
		  topology_split_id.push_back(dec_split_id);
		  split_active_corners.push_back(new_active_corner);
		}
	  }
	}






------
------
------

[ToDo] VALENCE_EDGEBREAKER


Afterwards, interior edges have to be processed.




	 for (i = 0; i < num_encoded_symbols; ++i) 
	 {
		EdgebreakerDecodeSymbol();	 // decoded symbol is written to variable last_symbol_ 


		corner = 3 * i;
		NewActiveCornerReached(corner, i);

	// -> handles last_symbol_
	// -> 
	  }
	  ProcessInteriorEdges();




### Process Interior Edges









# Attributes

The attribute section contains the attribute header, followed by one or more unique attribute types such as positions or normals. Each attribute type contains one or more unique attributes.

Attributes layout description:

- Count of attributes types: `uint8`
- An array of descriptions for each attribute type to define decoding parameters if edge breaker is the used encoding method. Each array entry  contains:
	- Data id: `uint8`
	- Decoder type: `uint8`
	`0` Vertex attribute, `1` Corner attribute
	- Traversal mode: `uint8`
	`0` Depth first traversal, `1` Prediction degree traversal
- The attribute type array.
	- Number of attributes for current type: `var32`
	- The attribute array.
		- Attribute type: `uint8`
		`0` Position, `1` Normal, `2` Color, `3` Texture coordinate, `4` Generic
		- Data type: `uint8`
		`1` signed 8-bit, `2` unsigned 8-bit,
		`3` signed 16-bit, `4` unsigned 16-bit,
		`5` signed 32-bit, `6` unsigned 32-bit,
		`7` signed 64-bit, `8` signed 64-bit,
		`9` 32-bit single precision floating point,
		`10` 64-bit double precision floating point,
		`11` 8-bit Boolean
		- Number of components: `uint8`
		- Normalized: `uint8`
		- Attribute id: `var32`
		Must uniquely identify this attribute across all attributes of all attribute types.
	- Array of sequential attribute encoding for each attribute.
		- Sequential attribute encoding: `uint8`
		`0` Generic, `1` Integer, `2` Quantization, `3` Normals

Attributes data itself has to be decoded.

Attribute:
- Prediction scheme: `int8`
	- `-2` None: There is no attribute data
	- `0` Difference: Implies that the attribute entries have two components.
	- `1` Parallelogram
	- `2` Multi parallelogram
	- `3` Texture coordinates (deprecated) [TODO: remove?]
	- `4` Constrained multi parallelogram
	- `5` Texture coordinates portable [TODO: what is portable?]
	- `6` Geometric normal
- Prediction transform type, if the prediction scheme is not *None*.
	- `-1` None
	- `0` Delta: ...
	- `1` Wrap: ... (currently the only supported prediction transform by github.com/google/draco)
	- `2` Normal octahedron: ...
	- `3` Canonicalized normal octahedron: ...
- Flag whether values are compressed: `uint8`
- If compression flag is set:
	- Scheme: `uint8`
		- `0` Tagged: Each symbol is preceded by a fix-length (5-bit?) bit-length variable defining the bit length of the actual value following the tag.
		TODO: byte aligned?
		- `1` Raw: Each symbol value has the same fixed bit-length. Accepted bit length are `[1, 18]`.


11.2

## Parse Attribute Decoders Data
Loading data from file

EdgeBreaker data:

i: num_attributes_decoders

	att_dec_data_id[i]                                                              UI8
	att_dec_decoder_type[i]                                                         UI8
	att_dec_traversal_method[i]                                                     UI8

General attribute data:

i: num_attributes_decoders  
j: att_dec_num_attributes[i]

	att_dec_num_attributes[i]                                                       varUI32
	att_dec_att_type[i][j]                                                          UI8
	att_dec_data_type[i][j]                                                         UI8
	att_dec_num_components[i][j]                                                    UI8
	att_dec_normalized[i][j]                                                        UI8
	att_dec_unique_id[i][j]                                                         varUI32
	seq_att_dec_decoder_type[i][j]                                                  UI8 
	
	




## EdgeBreaker Handling
DecodeAttributeSeams  
UpdateVertexToCornerMap

## Assign Points To Corners

## Generate Sequence

	for all num_attributes_decoders 
	{
	GenerateSequence()
	if (EdgeBreaker) UpdatePointToAttributeIndexMapping()
	}




# RANS Decoding

[ToDo]

	void DecodeTaggedSymbols(num_values, num_components, out_values) 
	{
	  num_symbols_                                                                        varUI32
	  BuildSymbolTables(num_symbols_, lut_table_, probability_table_);
	  size                                                                                varUI64
	  encoded_data                                                                        UI8[size]
	  RansInitDecoder(ans_decoder_, &encoded_data[0], size, TAGGED_RANS_BASE);
	  for (i = 0; i < num_values; i += num_components) 
	  {
		RansRead(ans_decoder_, TAGGED_RANS_BASE, TAGGED_RANS_PRECISION,lut_table_, probability_table_, &size);
		for (j = 0; j < num_components; ++j) 
		{
		  val                                                                             f[size]
		  out_values.push_back(val);
		}

		ResetBitReader();
	  }
	}




