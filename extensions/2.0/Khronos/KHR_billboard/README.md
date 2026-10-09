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

This extension assumes the presence of an active camera in the rendering environment.

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

## Handling Defined Billboards

When `KHR_billboard` is present on a node, that node **does not inherit orientation from its parent nodes**. The node's global transform is computed as follows:

1. The global translation is computed normally per the glTF 2.0 specification (§3.5.3), accumulating the full parent `T * R * S` chain but extracting only the resulting global-space position.
2. The global rotation and global scale of all parent nodes are **discarded** and replaced by a camera-derived rotation and billboard scale as defined below.

The final global transform of a billboard node MUST be:

```math
G_{billboard} = T_{G_{parent}} \cdot R_{camera} \cdot S_{billboard}
```

where $T_{G_{parent}}$ is the global translation derived from the parent chain, $R_{camera}$ is the camera-derived rotation matrix, and $S_{billboard}$ is the effective scale as defined below.

### Camera-Derived Rotation

Let $\hat{f}$ be the unit vector from the node's global position toward the camera position, projected and/or zeroed according to `rotationAxis` as defined in [Rotation Axis Constraints](#rotation-axis-constraints). Let $\hat{u}$ be the billboard's effective up vector as defined in [Up Vector](#up-vector). The camera-derived rotation matrix $R_{camera}$ is the unique rotation that maps `viewDirection` to $\hat{f}$ and aligns the node's local up axis with $\hat{u}$.

### Rotation Axis Constraints

The `rotationAxis` property restricts which axes of the camera-facing direction are applied. The constraint is applied in global space by zeroing the components of the camera-facing vector $\vec{f}$ (prior to normalization to $\hat{f}$) along axes not present in `rotationAxis`, then normalizing. If the resulting vector has zero length, the behavior is implementation-defined; implementations SHOULD fall back to the node's `viewDirection` transformed to global space.

When `rotationAxis` does not include all three axes, the billboard is constrained to face the camera only in the specified directions. For example, `["Y"]` produces a billboard that rotates to face the camera only around the global Y axis, suitable for upright objects such as trees or vertical signs.

### Up Vector

The `up` property defines the local up direction of the billboard in the node's local space. Implementations MUST use this vector when computing $R_{camera}$ to resolve the rotational degree of freedom around $\hat{f}$.

If `viewDirection` and `up` are parallel (within a tolerance of 1e-6 radians), the behavior is implementation-defined. Content MUST NOT define a `viewDirection` that is parallel to `up`.

### Effective Scale

If `scaleWithDistance` is `false`, $S_{billboard}$ is the node's local scale as defined in the glTF node properties.

If `scaleWithDistance` is `true`, $S_{billboard}$ is the node's local scale multiplied by a uniform scale factor:

```math
k = \frac{d_{current}}{d_{initial}}
```

where $d_{initial}$ is the view-space depth (camera-space Z) of the node's global position at the time the node is first rendered by the active camera, and $d_{current}$ is the view-space depth at the current frame. Implementations MUST recompute $d_{initial}$ if the node is removed from the scene and re-added.

### Child Nodes

Children of a billboard node inherit the billboard node's final global transform $G_{billboard}$ as their parent transform, per the glTF 2.0 specification (§3.5.3). The scene graph suspension applies only to the billboard node itself; children accumulate their local TRS on top of $G_{billboard}$ normally.

If a child node also defines `KHR_billboard`, that child's global transform is computed independently using the same rules, with the global translation derived from $G_{billboard}$ of the parent billboard node and all parent orientations again discarded.

### Overlay Rendering

If `overlay` is `true`, the billboard's primitives MUST be rendered after all non-overlay geometry. Depth testing against non-overlay geometry MUST be disabled for overlay primitives. When multiple overlay billboards overlap, they MUST be depth-sorted in descending order of their global-space distance from the camera and rendered back-to-front to produce correct alpha compositing order.

## Known Implementations

- Basic partial implementation in a CesiumJS prototype branch.

## Sample Implementation

_Note: This section is non-normative and only describes one approach to implementing billboards._

All vectors should be normalized between operations if not mentioned otherwise. 

If `viewDirection` and `up` are not orthogonal one can fix it by calculating the cross product between them to get the right vector and doing another cross product with this right vector and `viewDirection` to get the corrected `up` vector.

$up = (forward \times up) \times forward$

To start with calculating the correct model rotation, extract the global translation from the target node. To compute the new model forward vector subtract the global translation from the camera translation. For implementing the `rotationAxis` set the specific axis of the calculated camera position to zero.

Now we can compute the rotation $r_1$ between the current model forward vector and the new model forward vector. Most math libraries have an inbuilt function for this. This rotation should also be applied to the `up` vector.

Now we need to calculate the correct rotation for the up vector of the model. Coincidentally, this can be deferred from the up vector of the camera:

Fist, calculate the cross product of the camera right vector and the negated new model forward vector to get the new up vector. Now, compute the rotation $r_2$ between the the previously rotated `up` vector and the new up vector.

To get the model local rotation, both rotations need to be multiplied.

$model_{rotation} = r_1 * r_2$

Therefore, the rotation consists of the $r_1$ which rotates the model to look at the camera and $r_2$ which makes sure that camera up and model up vector are aligned.

To compose the model matrix, one first adds the model scale. If `scaleWithDistance` is `true`, a scaling factor needs to be applied based on the initial distance between the camera and the model.

$scaleFactor = \frac{(globalTranslation_{model} * viewMatrix).z}{(globalTranslation_{model} * initialViewMatrix).z}$

Now we can apply the $model_{rotation}$ to the model matrix and set the translation of the model matrix to the original one, since billboards only affect rotation and scale.

If `overlay` option is given, the depth value of affected primitives needs to be set to zero. If multiple billboards with overlay are present, than they need to be depth sorted based on their position and an increasing Z value near zero should be used. This avoids Z-fighting.

## Resources

- [Billboard explainer in glTF-External-Reference](https://github.com/KhronosGroup/glTF-External-Reference/blob/main/explainers/billboard.md)
- [glTF 2.0 Specification §3.5.2](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#transformations)
