# MSFT\_lod 

## Contributors

* Saurabh Bhatia, Microsoft
* Gary Hsu, Microsoft
* Adam Gritt, Microsoft
* John Copic, Microsoft 
* Marc Appelsmeier, Microsoft
* Dag Frommhold, Microsoft 

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds the ability to specify various Levels of Detail (LOD) to a glTF asset. An implementation of this extension can use the LODs for various scenarios like rendering a different LOD based upon the distance of the object or progressively loading the glTF asset starting with the lowest LOD. 

This extension allows an LOD to be defined at the geometry `node` or the `material` level. The `node` LODs can specify different geometry and materials across various levels whereas `material` LODs can specify different materials for the same geometry. 

The `MSFT_lod` extension is added to the highest LOD level. The extensions defines an `ids` property which is an array containing the indices of the lower LOD levels. Each value in the array points to a LOD level that is lower in quality than the previous level. For example, if the extension is defined at the `node` level then the `node` object with the extension is the highest LOD level. Every value specified in the `ids` array of the extension points to the index of another `node` object which should be used as the lower LOD level. 

An implementation of this extension can parse through the `ids` array to create the list of LOD levels available for an asset. If an asset containing this extension is loaded in a client that doesn't implement the extension, then the highest LOD level will be loaded and all lower LODs will be ignored. 

The following example shows how the `MSFT_lod` extension can be specified at the `node` level to create three LOD levels:
```json
"nodes": [
    {
        "name": "High_LOD",
        "mesh": 0,
        "extensions": {
            "MSFT_lod": {
                "ids": [
                    1,
                    2
                ]
            }
        },
        "extras": {
            "MSFT_screencoverage": [
                0.5,
                0.2,
                0.01
            ]
        }
    },
    {
        "name": "Medium_LOD",
        "mesh": 1
    },
    {
        "name": "Low_LOD",
        "mesh": 2
    }
]
 ```
 Since the `MSFT_lod` extension is specified in `node[0]`, it becomes the highest LOD. The first element in the `ids` array is *1*, making `node[1]` the next lower LOD (or the *Medium_LOD*). The second element in the `ids` array is *2*, making `node[2]` the lowest LOD for this sample. 

The `MSFT_screencoverage` metadata values specified in `extras` can be used along with the `MSFT_lod` extension as a hint to determine the switching threshold for the various LODs.

In the example above the screen coverage values can be used as follows:

- LOD0 Rendered from 1.0 to 0.5
- LOD1 Rendered from 0.5 to 0.2
- LOD2 Rendered from 0.2 to 0.01
- Nothing rendered from 0.01 – 0

A similar pattern can be followed to create `material` LODs: 
```json
"materials": [
    {
        "pbrMetallicRoughness": {
            "baseColorTexture": {
                "index": 0
            }
        },
        "normalTexture": {
            "index": 1
        },
        "extensions": {
            "MSFT_lod": {
                "ids": [
                    1,
                    2
                ]
            }
        }
    },
    {
        "pbrMetallicRoughness": {
            "baseColorTexture": {
                "index": 2
            }
        },
        "normalTexture": {
            "index": 3
        },
    },
    {
        "pbrMetallicRoughness": {
            "baseColorTexture": {
                "index": 4
            }
        },
        "normalTexture": {
            "index": 5
        },
    }
]
```
If both node level and material level LODs are specified then the material level LODs only apply to the specific node LOD that they were defined for. 

## glTF Schema Updates

* **JSON schema**: [glTF.MSFT_lod.schema.json](schema/glTF.MSFT_lod.schema.json)

## Known Implementations

* [glTF-Toolkit](https://github.com/Microsoft/glTF-Toolkit) can take three individual glTF files as input LODs and merge them into a single glTF file which uses the `MSFT_lod` extension. 
* [BabylonJS](https://github.com/BabylonJS/Babylon.js/tree/master/loaders/src/glTF) has support for `MSFT_lod` extension defined at the material or node level. The current implementation progressively loads each LOD to improve the initial load time of the asset.  
* Windows Mixed Reality Home and 3D Launchers for Windows Mixed Reality use `MSFT_lod` at the `node` level to support switching LODs based on the render distance.

## License

Microsoft has made this Specification available under the Open Web Foundation Agreement Version 1.0, which is available at https://www.openwebfoundation.org/the-agreements/the-owf-1-0-agreements-granted-claims/owfa-1-0.
