# KHR_billboard

## Contributors

- Adam Morris, Bentley Systems
- Sean Lilley, Bentley Systems
- Paul Connelly, Bentley Systems
- Patrick Härtl, UX3D

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Table of Contents

- [Overview](#overview)
- [Defining a Billboard](#defining-a-billboard)
- [Handling defined Billboards](#handling-defined-billboards)
- [Known Implementations](#known-implementations)
- [Sample Implementation](#sample-implementation)
- [Resources](#resources)

## Overview

Billboards are viewport-aligned images that are positioned within a 3D scene. Billboards can be used for rendering vegetation, 3D UI elements such as spatial markers and tex labels, low level of detail sprites in the distance, animated particles, and more in a computationally efficient manner. This extension specifies support for defining billboards in glTF models and scenes.

Many engines support billboards and can use this extension to import billboards into a scene.

Billboards can be defined for any glTF Node in a model. The content for the billboard is handled by the Node's mesh or child nodes. Properties for the billboard allow content creators to define the view direction, rotational axis, up vector (when view direction is parallel to the up vector), if the billboard scales with distance, and whether or not the billboard should always be visible.

## Defining a Billboard

To enable a Node to be used as a billboard, the desired Node object should include the `KHR_billboard` extension with the desired properties. All properties are optional and define sensible defaults.

The content of a billboard is defined by the meshes and children of the Node. This allows a wide array of different content types to be handled by the extension.

### Properties

| Property | Default | Description |
| -------- | ------- | ----------- |
| `scaleWithDistance` | `false` | Defines if the billboard should scale on distance change or not. The initial distance between the active camera and node is used as reference. |
| `viewDirection` | `[0,0,1]` | Can be used to change the facing direction of a node (default defined by glTF spec: +Z) |
| `up` | `[0,1,0]` | Needs to be defined if `viewDirection` is parallel to the default `up` vector. `up` and `viewDirection` do not need to be perpendicular (this can be computed by the implementation) |
| `rotationAxis` | `["X", "Y", "Z"]` | An array defining the axes used for rotation. By default the billboard is rotated around all axes. Possible values are `X`, `Y`, and/or `Z`. For example, to only rotate around the Y axis, set this property to `["Y"]`. |
| `overlay` | `false` | Defines if the billboard should be renderer in front of all other meshes and therefore never occluded. If two billboards with this property set to `true` overlap, their original node translation should be considered for ordering. |

### Example

```json
"nodes": [
    {
        "extensions": {
            "KHR_billboard" : {
                "scaleWithDistance" : false,
                "viewDirection": [0, 0, 1],
                "up": [0, 1, 0],
                "rotationAxis": ["X", "Y", "Z"],
                "overlay": false
            }
        }
    }
]
```

## Handling defined Billboards

When the `KHR_billboard` extension is defined on a node, the node's global rotation and scale are replaced by the extension and node's parent global rotation and scale are not used. The updated global transform of the node is applied to associated meshes and children as defined by the core glTF specification. Implementations should make a best effort to follow the parameters of the billboard's properties.

### Child Nodes

Children of a node that defines `KHR_billboard` are treated normally within the context of the glTF node transform. (See: [glTF 2.0 Specification, §3.5.3 Transformations](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#transformations)) This allows content creators to apply an additional adjustment to the billboard that will be applied.

_Note: If a child node also defines `KHR_billboard`, the child must be treated independently of the parent declaration higher in the node hierarchy. In this case, the child node's global rotation and scale are replaced by the extension as defined in [Handling defined Billboards](#handling-defined-billboards)._

## Known Implementations

- TODO: List of known implementations, with links to each if available.

## Sample Implementation

_Note: This section is non-normative and only describes one approach to implementing billboards._

All vectors should be normalized between operations if not mentioned otherwise. 

If `viewDirection` and `up` are not orthogonal one can fix it by calculating the cross product between them to get the right vector and doing another cross product with this right vector and `viewDirection` to get the corrected `up` vector.

$up = (forward \times up) \times forward$

To start with calculating the correct model rotation, extract the world translation from the target node. To compute the new model forward vector subtract the world translation from the camera translation. For implementing the `rotationAxis` set the specific axis of the calculated camera position to zero.

Now we can compute the rotation $r_1$ between the current model forward vector and the new model forward vector. Most math libraries have an inbuilt function for this. This rotation should also be applied to the `up` vector.

Now we need to calculate the correct rotation for the up vector of the model. Coincidentally, this can be deferred from the up vector of the camera:

Fist, calculate the cross product of the camera right vector and the negated new model forward vector to get the new up vector. Now, compute the rotation $r_2$ between the the previously rotated `up` vector and the new up vector.

To get the model local rotation, both rotations need to be multiplied.

$model_{rotation} = r_1 * r_2$

Therefore, the rotation consists of the $r_1$ which rotates the model to look at the camera and $r_2$ which makes sure that camera up and model up vector are aligned.

To compose the model matrix, one first adds the model scale. If `scaleWithDistance` is `true`, a scaling factor needs to be applied based on the initial distance between the camera and the model.

$scaleFactor = \frac{(worldTranslation_{model} * viewMatrix).z}{(worldTranslation_{model} * initialViewMatrix).z}$

Now we can apply the $model_{rotation}$ to the model matrix and set the translation of the model matrix to the original one, since billboards only affect rotation and scale.

If `overlay` option is given, the depth value of affected primitives needs to be set to zero. If multiple billboards with overlay are present, than they need to be depth sorted based on their position and an increasing Z value near zero should be used. This avoids Z-fighting.

## Resources

- [Billboard explainer in glTF-External-Reference](https://github.com/KhronosGroup/glTF-External-Reference/blob/main/explainers/billboard.md)
- [glTF 2.0 Specification §3.5.2](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#transformations)
