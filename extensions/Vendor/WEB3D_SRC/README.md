# WEB3D_SRC


## Contributors

* Max Limper, Fraunhofer IGD, [@mlimper_cg](https://twitter.com/mlimper_cg)
* Maik Thöner, Fraunhofer IGD, [@mthoener](https://twitter.com/mthoener)


## Dependencies

Written against the glTF draft 1.0 spec.

Required extensions:
* EXT_binary_glTF
* EXT_quantized_attributes


## Overview

With glTF, it is possible to transport information about meshes, but also about scene data.
This data may include lighting, scene hierarchy, cameras, as well as other aspects of the scene.
However, it is not clear how a glTF scene could be integrated into an existing scene graph, such as a VRML or X3D scene.

This extension introduces the notion of using glTF as a `Shape Resource Container` (SRC) within an X3D scene.
Concretely speaking, an SRC contains all kinds of resources that are usually specified as part of a `Shape` node in X3D:
* Appearance (usually specified using an `X3DAppearanceNode`)
* Geometry (usually specified using an `X3DGeometryNode`)

In order to externalize this information from the X3D scene graph into a glTF file, this extension provides the following:
* A list of required extensions
* A list of relevant glTF scene content for SRC files
* An addressing scheme that allows to refer to this content from external applications or documents
* Rules that show how to deal with missing ,or ambiguous, glTF scene content when using SRC

This specification also contains recommendations how to integrate the glTF data into the X3D scene, proposing the `ExternalShape`node.
That recommendation is, however, non-normative, and it is only provided for illustrative purposes.


### Required Extensions ###

This extension depends on the following two extensions:

* EXT_binary_glTF
* EXT_quantized_attributes

This decision was made for practical reasons.
In particular, since SRC files are usually already referenced from
within a structured scene description, it is desireable to prevent another level of indirection.
This is important in order to save HTTP requests, for example.
Therefore, an SRC file is always a `Binary glTF` file.

For clarity, it is recommended that SRC files use the standard Binary glTF extenstion `.glb`.
This is also due to the fact that SRC does not introduce any changes to the glTF schema.
Therefore, it is possible that an implementation does not support SRC,
but supports the extensions `EXT_binary_glTF` and `EXT_quantized_attributes`,
loads and displays SRC content correctly.

As SRC does not introduce any changes _within_ the binary glTF files,
except for the declaration of the SRC extension,
implementations can simply use this declaration to recognize SRC content.
This information might be useful in order to recognize content that is actually being
composed inside a structured description _outside_ the SRC file.
An example could be an SRC file that contains several meshes which are all
located at the origin of the scene, but actually being transformed outside the SRC file.
Therefore, a viewer could, for example, display a warning, explaining that
external information for the SRC file might be missing.
This also applies to the connection between `meshes`, `materials` and `textures`,
which might also be specified externally in case of SRC (compare section [Possible X3D Integration](#possible-x3d-integration)).


### Relevant glTF scene content ###

The relevant scene content, which should be transported within an SRC file, can be summarized as follows:

* Geometry, implemented as `meshes`
* Appearance, implemented as `materials`
* Texture images, implemented as `textures`

This is a recommended subset of the whole data available within the glTF schema.
Since glTF can be used to specify a much wider variety of scene data,
it cannot be guaranteed that a given SRC file does _not_ contain more information that 
this recommended subset.
For example, information about lights or cameras might also be contained inside the SRC file.
This is due to the fact that SRC files are simply Binary glTF files, using the same schema as other Binary glTF content.
However, it is strongly recommended to only use the recommended subset,
as implementations dealing with SRC files will most likely ignore any other kinds of scene data.

For SRC-capable implementations, it is recommended to provide the user with feedback about
elements of the structured glTF scene description that lie outside this recommended subset
(and that have therefore been ignored).


### Addressing Scheme for SRC content ###

To address the shape content of an SRC file from within a structured description,
the following rules should be used:

* If all content within an SRC file should be used, the file is simply referenced as any other external resource
* If a specific `mesh`, `material` or `texture` should be addressed,
  a hash (#) delimiter must be used after the part of the URL that refers to the SRC file.
  After the hash delimiter, exactly one identifier must be provided, being identical to
  exactly one identifier within the glTF scene of the SRC file.
* Identifiers provided after the hash delimiter correspond to a unique name of a `mesh`,
  a `material`, or a `texture`. Other identifiers within the glTF scene of the SRC file are ignored.

#### Examples

Suppose the glTF scene description within the SRC file `src-example.glb` would contain the following:
```
"meshes" : {
        "duck_mesh": {
            "name": "A 3D mesh of a rubber duck.",
            "primitives": [
                {
                    "attributes": {
                        "NORMAL": "accessor_id0",
                        "POSITION": "accessor_id1",
                        "TEXCOORD_0": "accessor_id2"
                    },
                    "indices": "accessor_id3",
                    "material": "material_id1",
                    "primitive": 4,
		            "extensions" : {},
                    "extras" : {}     
                }
            ],
            "extensions" : {},
            "extras" : {}     
        },
        "bunny_mesh": {
            "name": "A 3D mesh of a bunny.",
            "primitives": [
                {
                    "attributes": {
                        "NORMAL": "accessor_id4",
                        "POSITION": "accessor_id5",
                        "TEXCOORD_0": "accessor_id6"
                    },
                    "indices": "accessor_id7",
                    "material": "material_id2",
                    "primitive": 4,
		            "extensions" : {},
                    "extras" : {}     
                }
            ],
            "extensions" : {},
            "extras" : {}     
        }
    }
```

Then, the following ways could be used to refer to the content from the outside:

* `src-example.glb` refers to the whole file.
  This means that the application will have to use the full content of the SRC file,
  including both meshes, at the point where this external reference is used.
  (see section [Rules for Dealing with SRC Data](#rules-for-dealing-with-src-data))
  
* `src-example.glb#duck_mesh` refers to the `mesh` identified as `duck_mesh`.
  This means that the application will only use the respective mesh at the point where this external reference is used.
    

### Rules for Dealing with SRC Data ###

This section describes how applications which externalize shape data to SRC files
should handle SRC content.

If the content that should be used is well-specified, using the
[Addressing Scheme for SRC content](#addressing-scheme-for-src-content),
the respective content should be searched within the glTF scene of the SRC file.
If the content is not available, the application should regard this as an error
(i.e., it should throw an exception or output a message, etc.).
In that case, the application should not include any content of the SRC file,
since the desired content could not be found.

Otherwise, if the content to be used is not specified, this should be interpreted
as if the full recommended content of the SRC file (`meshes`, `materials`, `textures`)
would have been requested.

Following the general philosophy of glTF,
an SRC file does not necessarily have to provide any `meshes`, `materials` or `textures`.
Except from the case when a specific piece of content is explicitly requested (see above),
an implementation should not raise any error if there are no `meshes`, `materials` or `textures`
within an SRC file.
Instead, the application must be able to deal with missing data and use default data instead.
For example, an implementation should be able to safely deal with a case where no `meshes` are provided at all.


### Possible X3D Integration

_This section is non-normative_

Within this section, two new nodes are proposed for use within X3D.

#### ExternalShape
The `ExternalShape` node inherits from `X3DShapeNode`, and it has three fields:

* _bboxCenter_ of type _SFVec3f_ (inherited)
* _bboxSize_ of type _SFVec3f_ (inherited)
* _url_ of type _MFString_ (new)

As for `Shape`, the fields _bboxCenter_ and _bboxSize_ have default values
(that also allow to signal uninitialized data, where _bboxSize_ is "-1 -1 -1").
These fields do not need to be specified, but using them in the context of
the `ExternalShape` node is very useful:
With the information about the bounding box at hand, an implementation
can decide to load only the SRC files which have content that is currently visible, for example.
This possibility can be crucial in applications dealing with large amounts of 3D data.

The _url_ field is used in a similar fashion as for the `Inline` node:
It specifies one or more references to external SRC data.
If the data can not be loaded from the first location specified within the _MFString_ multi-field,
the second location specified is used as a fallback, and so on.

Unlike a `Shape` node, an `ExternalShape` must not have any child nodes.
Instead, the information usually contained inside the `Appearance` and `Geometry`child nodes
is read from the referenced SRC file.

If there are no `materials` defined within the SRC file, an
`Appearance` node with a default `Material` node as child is implicitly used instead.

If there are not `meshes` defined within the SRC file, the X3D browser should implicitly
assume an empty geometry node (, such as an empty `IndexedFaceSet`, for example).


#### ExternalGeometry

The `ExternalGeometry` node inherits from `X3DGeometryNode`, and it has one field:

* _url_ of type _MFString_ (new)

This field is used in a similar fashion as the _url_ field of the `ExternalShape` node.

The `ExternalGeometry` node can be used to externalize mesh data to SRC files.
If the reference to the SRC file is followed by the hash delimiter and an identifier,
this must be the identifier of a mesh. 
If this mesh is not available within the SRC file, the application should regard this as an error.
In that case, the application should not include any mesh from the SRC file,
since the desired mesh could not be found.

If the reference to the SRC file is not followed by the hash delimiter,
all meshes from within the SRC file should be used. 
If there are not `meshes` defined within the SRC file, the X3D browser should implicitly
assume an empty geometry node (, such as an empty `IndexedFaceSet`, for example).

If a `mesh`, as specified within the SRC file, contains appearance information,
this information is ignored by the X3D browser, since it is overridden by the respective
`Appearance` node, which is always a sibling of the `ExternalGeometry` node,
within the corresponding parent `Shape` node.


#### Using Textures from SRC files

In addition to using regular image formats, textures can also be referred to as SRC content.
Therefore, all X3D nodes dealing with texture data from images, such as `ImageTexture`,
may also refer to textures within an SRC file.

If the reference to the SRC file, made from within a texturing node,
is followed by the hash delimiter and an identifier,
this must be the identifier of a texture. 
If this texture is not available within the SRC file, the application should regard this as an error.
In that case, the application should not include any texture from the SRC file,
since the desired texture could not be found.

If the reference to the SRC file is not followed by the hash delimiter,
the _first_ texture from within the SRC file should be used. 
Note that this last point is different from the respective behavior of `meshes` within
`ExternalGeometry` or `ExternalShape` nodes.
This decision was made for practical reasons, since it is quite simple to
show multiple meshes instead of a single one - they can simply be rendered at the same time.
In contrast, a texture always belongs to one particular object,
and it is usually unclear how to use several textures instead of a single one.


## glTF Schema Updates

This extension does not introduce any new elements or properties to the glTF schema.
It is possible to use SRC content within other applications, even if they don't support this extension.
However, implementations might take the declaration of the SRC extension as a hint,
indicating that content is supposed to be referenced (and hence also transformed or composed)
from outside the SRC file.


## Known Implementations

The [SRC](http://x3dom.org/src/) implementation by Fraunhofer IGD has served as a basis for this proposal.


## Resources
* [SRC project](http://x3dom.org/src/) (paper, background, basic writer)
* SRC writer ([code](http://x3dom.org/src/files/src_writer_source.zip))
