# AGI\_articulations

## Contributors

* Greg Beatty, Analytical Graphics, Inc.
* Ed Mackey, Analytical Graphics, Inc.
* Alex Wood, Analytical Graphics, Inc.

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds articulations, attach points, and related metadata to the glTF model:

- [Articulations](#articulations)
    - [Stages](#stages)
    - [Pointing Vectors](#pointing-vectors)
- [Attach Points](#attach-points)

### Articulations

In glTF 2.0, the core animation system is geared towards the typical graphics industry usage pattern, where the model author(s) decide which parts of the model will move, as well as the exact nature and timing of those movements, at author time.  It does not allow for a case where certain parts of the model are designated by the author as movable, but their exact movements are not known until after the model is built.

For example, imagine the case of a radio dish on a motorized mount.  When the model is built, the model author might know that the dish can be pointed in certain directions, but might not know (until the dish is in active use) where it is pointing at any given time.  The glTF model of this dish needs to call out the names and limitations of the available motions, but the model does not contain the actual sequence of motions to perform, which may not be known until runtime or live simulation of the dish in action.

In this case, we say that the model can be *articulated*, and we need *articulations* to define the names and ranges of allowable motions (degrees of freedom) of all movable nodes on a model.  Applying an articulation to a node does not preclude also applying a glTF core animation to the same node, if that is desired.  Implementations are free to choose how and when to exercise the available articulations, and it is expected that this information will be external to the glTF file (else it could simply be encoded as an animation).

The glTF node that contains the example radio dish could ask for an articulation like this:

```json
"nodes": [
    {
        "extensions": {
            "AGI_articulations": {
                "articulationName": "Radio_Dish"
            }
        },
        "mesh": // etc...
    }
]
```

Multiple glTF nodes can all reference the same articulation name, such that a single articulation drives all of the selected node transforms in unison.  This can be useful for example to drive multiple wheels on a truck with a single articulation.

At the root level extensions object of the glTF, the extension must define articulations matching all of the articulation names requested by nodes in the model:

```json
"AGI_articulations": {
    "articulations": [
        {
            "name": "Radio_Dish",
            "stages": [
                ...  // See next section
            ]
        }
    ]
}
```

#### Stages

Each articulation contains one or more named stages.  Each stage provides a single degree of freedom of transformation for the node, along with minimum, initial, and maximum values for the value of that stage.

The `type` property identifies the transformation that this stage is applying to the glTF node.  The available types are:

- `xTranslate` - The stage value is a translation along the node's X axis.
- `yTranslate` - The stage value is a translation along the node's Y axis.
- `zTranslate` - The stage value is a translation along the node's Z axis.
- `xRotate` - The stage value is a counter-clockwise rotation in degrees about the node's +X axis.
- `yRotate` - The stage value is a counter-clockwise rotation in degrees about the node's +Y axis.
- `zRotate` - The stage value is a counter-clockwise rotation in degrees about the node's +Z axis.
- `xScale` - The stage value is a scaling factor to multiply along the node's X axis.
- `yScale` - The stage value is a scaling factor to multiply along the node's Y axis.
- `zScale` - The stage value is a scaling factor to multiply along the node's Z axis.
- `uniformScale` - The stage value is uniform scaling factor to apply to the node.

The order in which the stages appear is significant.  The stages' requested transformations are applied to the node in order of appearance, after the glTF node's own transformations.  When a model author is creating the list of stages, typically one would list translations first, then the rotations (typically Yaw, Pitch, Roll), and finally scale.

Note that while most of these stages are axis-aligned, they are aligned to the local node's axes, and the node's own glTF transform is applied first.  This means that off-axis rotations, translations, and scales can be achieved by modifying the node or parent node's orientation, to align the local axes as needed.

Here's an example showing two rotations applied to the example radio dish from the previous section.

```json
"AGI_articulations": {
    "articulations": [
        {
            "name": "Radio_Dish",
            "stages": [
                {
                    "name": "Azimuth",
                    "type": "yRotate",
                    "minimumValue": -180.0,
                    "maximumValue": 180.0,
                    "initialValue": 0.0
                },
                {
                    "name": "Elevation",
                    "type": "xRotate",
                    "minimumValue": 0.0,
                    "maximumValue": 90.0,
                    "initialValue": 30.0
                }
            ]
        }
    ]
}
```

#### Pointing Vectors

Articulations that contain at least one and at most two axes of rotation are allowed to contain a Pointing Vector.  This unit-length vector acts as a kind of forward direction vector for a given node that is intended to be aimed at some external target.  For example, a satellite with solar panels on it may have a rotation articulation for the panels, and a pointing vector indicating the direction that the light-receiving material is facing within that node.  The implementation could then point the solar panels at the Sun.

The pointing is clamped at the limits of the specified rotation articulation(s).  For example, certain kinds of solar panels can only rotate on a single axis, or a radio dish can only sweep through a certain number of degrees along an axis.  The glTF node will be oriented to best point at the target while staying within the provided limits.

The following example shows a radio dish with a +Z pointing vector.  The glTF specification defines that the front or "forward" side of a model faces +Z, and "up" is +Y.  But individual nodes may have their own local concept of "forward" within the node's local transform, which might not have any correlation with the global glTF axes or the glTF specification's advice on orientation.  For example, the radio dish may be mounted on the top of a truck, or the side of a satellite, and may not face "forward" relative to the larger model.  Thus, the Pointing Vector should be expressed in the node's local coordinate system, and should indicate which direction that particular node is facing.

```json
"AGI_articulations": {
    "articulations": [
        {
            "name": "Radio_Dish",
            "stages": [
                {
                    "name": "Azimuth",
                    "type": "yRotate",
                    "minimumValue": -180.0,
                    "maximumValue": 180.0,
                    "initialValue": 0.0
                },
                {
                    "name": "Elevation",
                    "type": "xRotate",
                    "minimumValue": 0.0,
                    "maximumValue": 90.0,
                    "initialValue": 30.0
                }
            ],
            "pointingVector": [
                0.0,
                0.0,
                1.0
            ]
        }
    ]
}
```

### Attach Points

An attach point is a named node where external objects, analysis, particle effects, or anything else may be attached.  It is attached to the origin (and optionally, the orientation) of the named node.  An otherwise empty node may be used as an attach point, optionally with its own translation and orientation.

For example, a model with a radio dish may have an attach point at the end of the dish, for line-of-sight analysis or FOV visualization.  A model with a rocket engine may have an attach point at the back of the engine as a target for a particle system visualization, using the orientation of the attach point to control the exit vector of the particles.

```json
"nodes": [
    {
        "name": "ParticleSystem1",
        "extensions": {
            "AGI_articulations": {
                "isAttachPoint": true
            }
        }
    }
]
```

## Optional vs. Required

Generally, this extension is considered optional, meaning it should be placed in the glTF root's `extensionsUsed` list, but not in the `extensionsRequired` list.  The only exception to this is if a model is so reliant on one or more articulation `initialValue` settings being non-zero that the model just doesn't look right when loaded without considering the metadata.  In that case, the extension can be placed in both `extensionsUsed` and `extensionsRequired`, such that it cannot be loaded without the articulations.  But this is not recommended.

## See Also

- The [AGI_stk_metadata](../AGI_stk_metadata/) extension
- The [GMDF Schema](https://github.com/AnalyticalGraphicsInc/gmdf) for keeping this data separate from glTF.

## glTF Schema Updates

- **glTF root extension JSON schema**: [gltf.AGI_articulations.schema.json](schema/gltf.AGI_articulations.schema.json)
- **glTF node extension JSON schema**: [node.AGI_articulations.schema.json](schema/node.AGI_articulations.schema.json)

## Known Implementations

* [STK (Systems Tool Kit)](https://www.agi.com/products/engineering-tools) version 11.5 and higher
