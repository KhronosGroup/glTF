# Draco Bitstream Specification

Contributors:
- Jim Eckerlein
- ...

## Binary conventions

All integer and floating point formats are little endian.
- `float` is IEEE 754 Single precision.
- `var32` is an unsigned 32-bit integer encoded using LEB128.
- `var64` is an unsigned 64-bit integer encoded using LEB128.
- `f{n}` is an unsigned `n`-bit number with decreasing digit place value. The number is appended by padding bits to the next byte so that data remains byte-aligned.

***TODO: Should we define the LEB128 encode/decode function?***

## Draco File Format

- Draco Header
- Metadata (optional)
- Connectivity
- Attributes


## Draco Header

Name | Type | Description
-|-|-
Draco String | `uint8[5]` | Must equal "DRACO"
Major version | `uint8` | Bitstream major version number
Minor version | `uint8` | Bitstream minor version number
Encoder type | `uint8` | `0`: Point cloud, `1`: Triangular mesh
Encoder method | `uint8` | `0` Sequential encoding, `1` Mesh edgebreaker encoding
Flags | `uint16` | `0` No flag, `0x8000` Metadata present

## Metadata

This section is only present if `DracoHeader.Flags` contains the flag `0x8000`.
It contains a list of metadata records which relate to specific attributes and a global metadata record. Each metadata record contains a name-value pair list together with a list of sub metadata records, forming a metadata record tree. Each sub metadata record is a name-metadata pair. The `i`'th  attribute metadata record relates the attribute referenced by the `i`'th attribute metadata id.

To avoid ambiguities, each id inside the attribute metadata id array must be unique. Otherwise, multiple metadata records would relate to the same attribute.

Name | Type
-|-
Count of metadata records | `var32`
Array of attribute metadata ids | `var32[]`
Array of attribute metadata records | `Metadata[]`
Global metadata | `Metadata`

`Metadata` designates a single metadata record. Each record can contain zero or more name-value entries. `Metadata` is a recursive type as it can contain zero or more sub metadata records where each sub metadata record is a name-metadata pair.

`Metadata`:

- Count of entries: `var32`
- Entry array:
	- Name size: `uint8`
	- Name: `int8[]`
	- Value size: `uint8`
	- Value: `int8[]`
- Count of sub metadata records: `var32`
- Sub metadata array:
	- Name size: `uint8`
	- Name: `int8[]`
	- Sub metadata: `Metadata` (Here, the type recurses. This is not a reference or index.)

## Connectivity

### Sequential Connectivity

Name | Type
-|-
Count of faces | `var32`
Count of points | `var32`
Connectivity method | `0` Compressed indices, `1` Uncompressed indices

#### Index compression

TODO: Explain in words. Why does this compression save space?

```
int32 diff = i_current - i_prev;
int32 encoded_value = diff >= 0 ? diff << 1 : (diff << 1) | 0x1;
```

## Attributes

The attribute section contains the attribute header, followed by one or more unique attribute types such as positions or normals. Each attribute type contains one or more unique attributes.

Attributes layout description:

- Count of attributes types: `uint8`
- An array of descriptions for each attribute type to define decoding parameters if edgebreaker is the used encoding method. Each array entry  contains:
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
		- `0` Tagged: Each symbol is preceeded by a fix-length (5-bit?) bit-length variable defining the bit length of the actual value following the tag.
		TODO: byte aligned?
		- `1` Raw: Each symbol value has the same fixed bit-length. Accepted bit length are `[1, 18]`.

TODO: rANS

