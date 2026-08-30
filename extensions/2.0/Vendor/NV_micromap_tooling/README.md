# NV_micromap_tooling

## Contributors

- Nia Bickford, NVIDIA, [@NBickford-NV](https://github.com/NBickford-NV)
- Christoph Kubisch, NVIDIA, [@pixeljetstream](https://github.com/pixeljetstream)
- Martin-Karl Lefrancois, NVIDIA, [@mklefrancois](https://github.com/mklefrancois)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec. References definitions from the `NV_displacement_micromap` specification.

## Overview

This extension allows tools to store and retrieve data about how to generate micromaps.

Micromesh generation is sometimes separated into different tools: for instance, a *remesher* might generate a low-resolution mesh from a high-resolution mesh, and then call a *baker* to generate a displaced micromap (DMM) that makes the low-resolution mesh resemble the high-resolution mesh.

Tools earlier in pipelines may generate auxiliary data that subsequent tools may use to generate results of potentially higher quality. For instance, the remesher may know what vertices in the high-resolution mesh combined to produce a vertex in the low-resolution mesh; this is information the baker cannot easily re-compute. A remesher could provide an intended subdivision level for each triangle; if the baker uses these, it could get higher-quality visual results while generating a file of equivalent size.

| Warning                                                      |
| ------------------------------------------------------------ |
| This extension's for internal use only at the moment, because it's possible the need for it might go away if we can interchange data between tools without glTF. |

## Example

To tag a primitive with data subsequent tools may use, add the `NV_micromap_tooling` extension to that primitive's `extensions`, and fill it with properties.

This extension allows adding arbitrary JSON members. In the example below, we list two *standard properties* defined in this extension: `directionBounds`, which has the same meaning as in `NV_displacement_micromap` but here does not require a `micromap` member, and `subdivisionLevels`, described in the Properties section. Implementations **MAY** add their own properties, such as `myCustomProperty` below, with implementation-defined JSON values.

```json
"meshes": [
    {
        "primitives": [
            {
                "indices": 1,
                "material": 0,
                "attributes": {
                    "POSITION": 2,
                    "NORMAL": 3,
                    "TANGENT": 4
                },
                "extensions": {
                    "NV_micromap_baking": {
                        "directionBounds": 0,
                        "subdivisionLevels": 5,
                        "myCustomProperty": {
                            "arbitraryData": "...",
                            ...
                        }
                    }
                }
            }
        ]
    }
]
```

## Properties

We define the following standard property strings and descriptions. All properties are optional.

| Name                  | Type      | Description                                                  |
| --------------------- | --------- | ------------------------------------------------------------ |
| **mapIndices**        | `integer` | Same as in `NV_displacement_micromap`.                       |
| **mapOffset**         | `integer` | Same as in `NV_displacement_micromap`.                       |
| **directions**        | `integer` | Same as in `NV_displacement_micromap`.                       |
| **directionBounds**   | `integer` | Same as in `NV_displacement_micromap`. This should give a shell for raycasting when baking: in other words, a tool should assume that the displaced mesh can fit between the shell defined by this mesh with all values equal to 0.0, and this mesh with all values equal to 1.0. |
| **primitiveFlags**    | `integer` | Same as in `NV_displacement_micromap`.                       |
| **subdivisionLevels** | `integer` | The index of an `accessor`; element `i` of this accessor gives an intended subdivision level for primitive triangle `i`. Its `type` **MUST** be `"SCALAR"`, and its `componentType` **MUST** be 5121 (unsigned byte) or 5123 (unsigned short). |

Implementations **MAY** add other properties, whose type and definition are *implementation-defined*. This extension provides no guarantee that two implementations will agree on the definitions or types of implementation-defined properties.

