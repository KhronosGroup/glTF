# AGI\_stk\_metadata

## Contributors

* Greg Beatty, Analytical Graphics, Inc.
* Ed Mackey, Analytical Graphics, Inc.
* Alex Wood, Analytical Graphics, Inc.

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds extra metadata to the glTF model.  These metadata are designed to be a 1:1 match with existing features of the product STK (Systems Tool Kit), produced by Analytical Graphics, Inc.

### Solar Panel Groups

All mesh geometry contained in nodes assigned to a Solar Panel Group is considered to be the surface of light-receiving material.  The back, sides, and other supporting structures should be in separate node(s) from the actual solar cells.  Multiple glTF nodes can belong to a single group of solar panels.  This allows the STK Solar Power Tool to analyze which portions of a model are exposed to light and are generating power.  Each group has an `efficiency` rating, as a percentage from 0.0 to 100.0, that indicates how efficiently the solar cells convert solar to electrical energy.

This example shows a glTF root extension object defining a solar panel group at 14% power efficiency:

```json
"AGI_stk_metadata": {
    "solarPanelGroups": [
        {
            "name": "Geo1",
            "efficiency": 14.0
        }
    ]
}
```

A given glTF model node that defines the geometry for the light-receiving surface of the solar panel could add itself to this group with the following:

```json
"nodes": [
    {
        "extensions": {
            "AGI_stk_metadata": {
                "solarPanelGroupName": "Geo1"
            }
        },
        "mesh": // etc...
    }
]
```

## No-Obscuration Nodes

Mesh geometry contained in "No-Obscuration" nodes will not affect the calculations of STK's Sensor Obscuration Tool.  The geometry of such marked nodes will be invisible to the sensors for the purpose of this calculation, as if the sensor is able to see through, unobscured.

To assign this property, place the extension on the desired node(s) and set the following value:

```json
"nodes": [
    {
        "extensions": {
            "AGI_stk_metadata": {
                "noObscuration": true
            }
        },
        "mesh": // etc...
    }
]
```

## Optional vs. Required

This extension is optional, meaning it should be placed in the glTF root's `extensionsUsed` list, but not in the `extensionsRequired` list.

## See Also

- The [AGI_articulations](../AGI_articulations/) extension
- The [GMDF Schema](https://github.com/AnalyticalGraphicsInc/gmdf) for keeping this data separate from glTF.

## glTF Schema Updates

- **glTF root extension JSON schema**: [gltf.AGI_stk_metadata.schema.json](schema/gltf.AGI_stk_metadata.schema.json)
- **glTF node extension JSON schema**: [node.AGI_stk_metadata.schema.json](schema/node.AGI_stk_metadata.schema.json)

## Known Implementations

* [STK (Systems Tool Kit)](https://www.agi.com/products/engineering-tools) version 11.5 and higher
