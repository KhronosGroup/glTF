# KHR_anchor_plane

## Contributors

- Ben Houston, Threekit, @bhouston
- Dwight Rodgers, Adobe
- Pete Falco, Adobe

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows a glTF to specify that it can bind and orient itself to other objects.  This first extension only specifies horizontal-floors, horizontal-ceilings and vertical-walls at attachment options.  It is intended that further extensions will enable additional attachment options, for example: body parts (faces, hands, feet) as well as explicit images targets.

## Design Goals

The goal is to enable glTF files to intrinsically denote that they are designed and are capable of binding to specific locations in the real world.  The attachment specification should be compatible with existing augmented reality tool kits.

## Anchors

One or more anchors are supported.  An object may be able to both attach on the floor as well as on the wall.  When it is on the wall, it may be attached in a different manner than on the floor, or it may be the same.  This specification allows for flexibility in this regards.

[Question from Ben Houston: Additional extensions will likely want to specify more "types" of anchors in the future.  How should those be supported in this framework?  Separate lists for each type of anchor extension?  I guess that makes sense.]
"extensions": {
    "KHR_anchors_plane" : {
        "anchors_plane": [
            {
                "name": "ceiling-attachment",
                "type": "vertical-wall",
                "node": 1,
                "translation": [ 1, 0, 0 ],
                "rotation": [ 0, 0, 0, 1 ],
                "scale": [ 1, 1, 1 ]
            }
        ]
    }
}

### Names

We are allowing each name to have an optional name.  This enables the anchors to be used in the future in more generic fashions, such as support general mate points.

### Types

**vertical-wall** anchors to a vertical wall-like surface.  It can also attach to the side of a cabinet or any other vertical wall like structure.  The orientation of the anchor relative to the wall is such that X is to the right, Y is up, and Z is out of the wall.

**horizontal-floor** anchors to a horizontal floor-like surface.  It can also attach to the top of a table or cabinet.  The orientation of the anchor relative to the floor is such that X is to the right, Y is out of the floor and Z is towards the viewer.

**horizontal-ceiling** anchors to a horizontal ceiling-like surface.  The orientation of the anchor relative to the ceiling is such that X is to the right, Y is into the ceiling and Z is towards the viewer.

### Node

This specifies the attachment node.  By default it is assumed that the attachment node is the root of the model.  But in some cases, this may not be desired.  This optional parameter enables more complex attachments.

The scale and sheer of the node is ignored, only its position and rotation is utilized.

### Transform

The optional the anchor relative to its local frame is optional.  In the majority of cases this is not required.  But in some case, specifying the relative transform can make content creation easier.
