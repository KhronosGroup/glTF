<!--
Copyright 2023 NVIDIA Corporation. All rights reserved.
-->

# NV_materials_mdl

## Contributors

* Timothy Blut, NVIDIA, tblut@nvidia.com
* Lutz Kettner, NVIDIA, lkettner@nvidia.com
* Kai Rohmer, NVIDIA, krohmer@nvidia.com
* Jan Jordan, NVIDIA, jjordan@nvidia.com
* Martin-Karl Lefrancois, NVIDIA, mlefrancois@nvidia.com

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

A fully conforming implementation must also support the `EXT_lights_ies` extension.

## Overview

This extension provides the ability to store and transfer materials in glTF assets that are defined in the NVIDIA Material Definition Language (MDL) as defined in the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification). The intention of this extension is to enable high-quality material definitions in glTF for MDL-capable applications and renderers in addition to the standard glTF 2.0 PBR model.

MDL materials are defined in MDL modules that are stored in files with the `.mdl` file extension. References to `.mdl` files are resolved in the configuration context of a conforming implementation, in particular, using search paths as documented in the Appendix F of the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification). As an alternative, an MDL module can also be embedded in the asset. MDL materials can accept textures and other resources as input arguments. This extension uses the images in the core glTF 2.0 standard for JPG and PNG textures. Additionally, image objects may also refer to EXR and VDB resources using `image/x-exr` and `application/x-vdb` media types respectively if this extension is used. The [EXT_lights_ies](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Vendor/EXT_lights_ies) extension is used for IES light profiles, and MBSDF isotropic measured BSDF data as defined in Appendix B in the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification) is part of this extension.

A conforming implementation of this extension must be able to load the referenced MDL modules, validate that the referenced function calls and the user-defined types have compatible definitions in those MDL modules, and use those functions in the referenced material bindings to render the mesh with the respective MDL material instead of the standard glTF PBR material. A function call is validated against an MDL module by checking if the call with its typed arguments finds a matching function definition including overload resolution as defined in Section 12.4 in the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification). A user-defined type is validated against an MDL module by checking if all its fields or enumerations values exist for the corresponding type in the MDL module.

Since the MDL material replaces the standard glTF material, it is highly recommended that authoring applications also provide a closely matching standard glTF PBR material next to an MDL material such that implementations that do not understand this extension can nonetheless show a representative rendering of this glTF file. Defining a fallback material is, however, optional. The [best practices](#best-practices) section provides more details on this matter. 

MDL materials are defined through functions in the language that return the built-in type `material`. This extension operates on function calls of functions defined in referenced MDL modules. These function calls are defined in the `functionCalls` array property which is defined in the top-level `NV_materials_mdl` extension object. Modules which are referenced by the function calls and types are defined in the `modules` array. Lastly, BSDF measurement resources are defined in the `bsdfMeasurements` array.

```json
"extensions": {
    "NV_materials_mdl": {
        "modules": [
            ...
        ],
        "bsdfMeasurements": [
            ...
        ],
        "functionCalls": [
            ...
        ]
    }
}
```

## Adding an MDL Material to an Existing Material
An MDL material is referenced by the glTF material by specifying an index into the `functionCalls` array using the `funcionCall` property within a material's `extensions.NV_materials_mdl` property. This function call index represents the root node of a graph of function calls and must have the built-in return type `material`.

Example:
```json
"materials": [
    {
        "extensions": {
            "NV_materials_mdl": {
                "functionCall": 1
            }
        }
    }
]
```

## Defining MDL Modules
All modules that are referenced by the function calls in the `functionCalls` array must be listed in the `modules` array. A `module` has the following properties:

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `uri` | `string` | The URI (or IRI) of the MDL module. | No |
| `bufferView` | `integer` | The index of the buffer view that contains the MDL module. | No |
| `mimeType` | `string` | The MDL module's media type. | No |
| `modulePath` | `string` | Relative path of the module. | No |
| `name` | `string` | glTF name of the module. | No |

Each module defines either the `uri` property, which contains the URI (or IRI) of an external MDL module file or a data-URI with embedded data, or the `bufferView` property, which points to a buffer view object that contains an MDL module. When the `bufferView` property is defined, then the `mimeType` property must also be defined. The module format must match the `mimeType` property when the latter is defined. If a data-URI or buffer view object is used, then the `modulePath` property must be defined, which specifies a path relative to the glTF file that is used for importing by other modules, e.g., if an MDL module that has the path `./my/embedded/module.mdl` relative to the glTF file is embedded into a buffer view object, then the `modulePath` property must have this path as its value. If `uri` contains a valid path to an external MDL module file, then `modulePath` must not be defined.

This extension currently supports only MDL modules of `application/vnd.mdl` media type.

MDL modules can be located in MDL search paths as defined in Section 2.2 and Appendix F of the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification). If this is the case, then the URI must have the `mdl://` scheme prefix. For example, `./my/module.mdl` and `my/module.mdl` are paths relative to the glTF file, whereas `mdl:///base.mdl` and `mdl:///vMaterials_2/Wood/Wood_Bark.mdl` are paths relative to the MDL search paths.

Embedded MDL modules can be dependencies of other MDL modules. Therefore, they are required to be listed in the `modules` array in the order in which they must be loaded, such that all dependencies to these modules can be resolved.

Example:
```json
"modules": [
    {
        "uri": "./my/module.mdl"
    },
    {
        "uri": "mdl:///base.mdl"
    },
    {
        "bufferView": 0,
        "mimeType": "application/vnd.mdl",
        "modulePath": "some/embedded/module.mdl"
    }
]
```

## Defining BSDF Measurements

BSDF measurements (MBSDFs) as defined in Appendix B of the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification) are listed in the `bsdfMeasurements` array. A `bsdfMeasurement` can have the following properties: 

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `uri` | `string` | The URI (or IRI) of the MBSDF. | No |
| `bufferView` | `integer` | The index of the buffer view that contains the MBSDF. | No |
| `mimeType` | `string` | The BSDF measurement's media type. | No |
| `name` | `string` | glTF name of the BSDF measurement. | No |

Each BSDF measurement defines either the `uri` property, which contains the URI (or IRI) of an external MBSDF file or a data-URI with embedded data, or the `bufferView` property, which points to a buffer view object that contains an MBSDF file. When the `bufferView` property is defined, then the `mimeType` property must also be defined. The MBSDF format must match the `mimeType` property when the latter is defined.

This extension currently supports only MBSDFs of `application/vnd.mdl-mbsdf` media type.

Example:
```json
"bsdfMeasurements": [
    {
        "uri": "measurement.mbsdf"
    },
    {
        "bufferView": 0,
        "mimeType": "application/vnd.mdl-mbsdf"
    }
]
```

## Defining MDL Function Calls
Function calls are listed in the `functionCalls` array. A `functionCall` can have the following properties: 

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `module` | `integer` | The containing module's index. | No |
| `functionName` | `string` | The unqualified function name. | :white_check_mark: Yes |
| `type` | `functionCall.type` | The function's return type. | :white_check_mark: Yes |
| `arguments` | `array` | A list of function arguments. | No |
| `name` | `string` | glTF name of the function call. | No |

For built-in functions, e.g., the `float3` and `texture_2d` constructors, the `module` property must not be defined. The operator functions as defined in Section 12.10 of the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification) are valid built-in functions.

### Function and Argument Types
The `functionCall.type` object is used to specify the return type of function calls and value function arguments. It's defined by the following properties:

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `module` | `integer` | The containing module's index. | No |
| `typeName` | `string` | The unqualified type name. | :white_check_mark: Yes |
| `arraySize` | `integer` | The array size. | No |
| `modifier` | `string` | The type modifier (`"uniform"` or `"varying"`). | No |

For the built-in MDL data types listed in Section 6 of the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification) the `module` property must not be defined.

If the `arraySize` property is specified, then an array type is described by the object.

### Function Arguments
Function argument objects can have the following properties:

| Property | Type | Description | Required |
|:---------|:-----|:------------|:---------|
| `name` | `string` | The argument name as defined in the MDL function declaration. | :white_check_mark: Yes |
| `functionCall` | `integer` | The index of the referenced function call. | No |
| `type` | `functionCall.type` | The type of the literal value. | No |
| `value` | `any` | The literal value. | No |

Function arguments can either be value or call arguments, depending on if the `functionCall` or `value` properties are specified. If the `value` property is specified, then the `type` property must also be specified. A call argument references a function call who's return value is used as the argument's value. A value argument's value is defined directly as a literal. The following types are supported as value arguments and can be specified as literals:

* Built-in scalar, vector, and matrix types
* `string` and `color`
* Built-in and user-defined enumeration types
* Arrays of the above types

For the vector and matrix types each component must be specified separately and for a `color` value the `r`, `g` and `b` values must be specified. Note that matrices in MDL follow column-major conventions, e.g., `float2x3` has 2 columns and 3 rows, and thus, matrix values are to be specified as an array with the values in column-major order. For enumerations, the value name should be specified as a string, e.g., `intensity_radiant_exitance` for the built-in enumeration type `intensity_mode`.

Example:
```json
"arguments": [
    {
        "name": "arg_call",
        "functionCall": 2
    },
    {
        "name": "arg_color",
        "type": {
            "typeName": "color"
        },
        "value": [ 1.0, 0.0, 0.0 ]
    },
    {
        "name": "arg_string",
        "type": {
            "typeName": "string"
        },
        "value": "some string value"
    },
    {
        "name": "arg_float2x3_array",
        "type": {
            "typeName": "float2x3",
            "arraySize": 2
        },
        "value": [
            [ 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 ],
            [ 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 ]
        ]
    },
    {
        "name": "arg_enum",
        "type": {
            "typeName": "intensity_mode"
        },
        "value": "intensity_radiant_exitance"
    }
]
```

## Special Functions
A conforming implementation is required to recognize a set of special functions.

### Array Constructor
The array constructor function has the special name `T[]` and takes `N` arguments with the array element type `Type` named `value0`, `value1`, `value2`, and so on. The return type must be the array element type `Type` and array size `N`.

Example:
```json
{
    "functionName": "T[]",
    "type": {
        "typeName": "float",
        "arraySize": 3
    },
    "arguments": [
        {
            "name": "value0",
            "type": {
                "typeName": "float"
            },
            "value": 42.0
        },
        {
            "name": "value1",
            "functionCall": 1
        },
        {
            "name": "value2",
            "functionCall": 2
        }
    ]
}
```

### Member Accessor
Member accessors (the *dot operator*) are named after the member being accessed preceded by the structures name followed by the dot operator (e.g., `texture_return.tint` or `float3.x`). The function has a single argument `s` which is the instance of the structure type from which the member should be accessed and the return type is the type of the member. Unlike most other special functions, the member accessor is defined in the module in which the structure type is defined.

Example:
```json
{
    "module": 1,
    "functionName": "texture_return.tint",
    "type": {
        "typeName": "color"
    },
    "arguments": [
        {
            "name": "s",
            "functionCall": 2
        }
    ]
}
```

### Type-Cast Operator
The name of the type-cast operator function is `operator_cast` and has a single argument `cast` with the type `Type`. The return type is a cast-compatible type `Type2` as defined in the [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification).

Example:
```json
{
    "functionName": "operator_cast",
    "type": {
        "module": 0,
        "typeName": "two_colors"
    },
    "arguments": [
        {
            "name": "cast",
            "type": {
                "module": 0,
                "typeName": "color_pair"
            },
            "functionCall": 1
        }
    ]
}
```

### Array Length Operator
The name of the array length operator function is `operator_len` and has a single argument `a` which must be an array type. The return type is `int`.

Example:
```json
{
    "functionName": "operator_len",
    "type": {
        "typeName": "int"
    },
    "arguments": [
        {
            "name": "a",
            "functionCall": 1
        }
    ]
}
```

### Resource Constructors
The resource constructors (`texture_2d`, `bsdf_measurement`, `light_profile`, etc.) are special cases where the `name` argument must be a uniform integer index referencing the resource in the respective array, instead of a string containing the path to the actual resource file as specified in the MDL specification.
```json
{
    "functionName": "texture_2d",
    "type": {
        "typeName": "texture_2d"
    },
    "arguments": [
        {
            "name": "name",
            "type": {
                "typeName": "int",
                "modifier": "uniform"
            },
            "value": 2
        }
    ]
}
```

The `name` argument of the MDL resource type constructors refers to the following resource arrays:
| Resource Type | glTF Array | Description |
| --------------|------------|-------------|
| `texture_2d` | `images` | The core glTF images array. |
| `texture_3d` | `images` | The core glTF images array. |
| `texture_cube` | `images` | The core glTF images array. |
| `texture_ptex` | `images` | The core glTF images array. |
| `bsdf_measurement` | `NV_materials_mdl.bsdfMeasurements` | The BSDF measurements array of this extension. |
| `light_profile` | `EXT_lights_ies.lights` | The lights array of the `EXT_lights_ies` extension. |

## Example
Below is a simple example for an MDL module containing a user-defined structure and enumeration type, and a material function definition which is referenced in the glTF file. The module path relative to the glTF file is `./my/example/module.mdl`.
```C
mdl 1.8;

export struct my_struct {
    float member;
};

export enum my_enum {
    my_enum_value_1,
    my_enum_value_2
};

export material my_material(
    float3 arg_float3,
    uniform my_enum arg_enum,
    uniform color arg_color
) = material(...);
```

The material function definition can be used to create a function call which returns a material instance with specific arguments:
```C
my_material(
    arg_float3: float3(1.0, 1.0, 1.0),
    arg_enum: my_enum_value_1,
    arg_color: base::file_texture(
        texture: texture_2d(
            name: "./some/path/image.png"
        ) 
    ).tint
);
```

The part of the example glTF below defines the previously shown material function call from the example module `./my/example/module.mdl`. It's assumed that the `images` array contains an image object at index 0 that points to the image path `./some/path/image.png`.

```json
"extensions": {
    "NV_materials_mdl": {
        "modules": [
            {
                "uri": "./my/example/module.mdl"
            },
            {
                "uri": "mdl:///base.mdl"
            }
        ],
        "functionCalls": [
            {
                "module": 0,
                "functionName": "my_material",
                "type": {
                    "typeName": "material"
                },
                "arguments": [
                    {
                        "name": "arg_float3",
                        "type": {
                            "typeName": "float3"
                        },
                        "value": [ 1.0, 1.0, 1.0 ]
                    },
                    {
                        "name": "arg_enum",
                        "type": {
                            "module": 0,
                            "typeName": "my_enum",
                            "modifier": "uniform"
                        },
                        "value": "my_enum_value_1"
                    },
                    {
                        "name": "arg_color",
                        "functionCall": 1
                    }
                ]
            },
            {
                "module": 1,
                "functionName": "texture_return.tint",
                "type": {
                    "typeName": "color"
                },
                "arguments": [
                    {
                        "name": "s",
                        "functionCall": 2
                    }
                ]
            },
            {
                "module": 1,
                "functionName": "file_texture",
                "type": {
                    "module": 1,
                    "typeName": "texture_return"
                },
                "arguments": [
                    {
                        "name": "texture",
                        "functionCall": 3
                    }
                ]
            },
            {
                "functionName": "texture_2d",
                "type": {
                    "typeName": "texture_2d"
                },
                "arguments": [
                    {
                        "name": "name",
                        "type": {
                            "typeName": "int",
                            "modifier": "uniform"
                        },
                        "value": 0
                    }
                ]
            }
        ]
    }
}
```

## Best Practices

If the asset does not provide any fallback material, then `NV_materials_mdl` should be present in both `extensionsUsed` and `extensionsRequired`. The `material` node will then only have the `extensions` property:
```json
"materials": [
    {
        "extensions": {
            "NV_materials_mdl": {
                "functionCall": 0
            }
        }
    }
]
```

When a fallback material is provided, `NV_materials_mdl` should only be present in `extensionsUsed`, but not in `extensionsRequired`. It is highly recommended that a closely matching fallback material is provided for implementations that do not support this extension.

## JSON Schema

* [glTF.NV_materials_mdl.schema.json](schema/glTF.NV_materials_mdl.schema.json)
* [material.NV_materials_mdl.schema.json](schema/material.NV_materials_mdl.schema.json)
* [functionCall.schema.json](schema/functionCall.schema.json)
* [functionCall.argument.schema.json](schema/functionCall.argument.schema.json)
* [functionCall.type.schema.json](schema/functionCall.type.schema.json)
* [module.schema.json](schema/module.schema.json)
* [bsdfMeasurement.schema.json](schema/bsdfMeasurement.schema.json)

## Known Implementations

* [NVIDIA Omniverse](https://www.nvidia.com/en-us/omniverse/)
* [NVIDIA Material Definition Language SDK](https://github.com/NVIDIA/MDL-SDK)

## Resources

* [NVIDIA Material Definition Language SDK](https://github.com/NVIDIA/MDL-SDK)
* [MDL Language Specification](https://github.com/NVIDIA/MDL-SDK/tree/master/doc/specification)
* [OpenEXR](https://www.openexr.com/)
* [OpenVDB](https://www.openvdb.org/)
