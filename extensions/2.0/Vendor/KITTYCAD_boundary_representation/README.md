# KITTYCAD\_boundary\_representation

## Contributors

* David Harvey-Macaulay, KittyCAD [@alteous](https://github.com/alteous)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

# Overview

A [boundary representation](https://en.wikipedia.org/wiki/Boundary_representation) (abbreviated as 'B-rep') provides an exact definition of solid volumes. B-reps can represent smooth surfaces such as spheres more precisely than polygonal meshes.

B-reps need to be tessellated to be able to be rendered using standard graphics APIs such as Vulkan and OpenGL. For backward compatibility, this extension allows for B-reps to be associated with a tessellation (i.e., a mesh) that has been precomputed. When this is the case, the extension will appear in the `extensionsUsed` property. Where tessellations are not provided, the extension is listed in the `extensionsRequired` property and nodes are permitted to reference a sole B-rep item.

## Concepts

### Solid

A _solid_ represents a single volume of solid material. The boundary of the volume is called the solid's _outer shell_. In addition to its outer shell, a solid may have internal hollow regions; such regions are called _inner shells_. Both outer shells and inner shells are represented by the `Shell` data structure.

![solid-example](figures/solid.png)

#### Example

The interior hollow space of a tennis ball would be represented by an inner shell. The surface of the tennis ball would be represented by its outer shell.

![tennis-ball-cross-section](https://content.instructables.com/FRV/UI1L/J2AW02WE/FRVUI1LJ2AW02WE.jpg?auto=webp&frame=1&width=1024&height=1024&fit=bounds&md=da5cf8a96b88acbea9eb082a7b378ae7)

### Shell

A _shell_ is a collection of _faces_ in 3D space which form a 'watertight' volume. A shell is represented by the `Shell` data structure.

### Face

A _face_ is a portion of a _surface_ in 3D space. Multiple faces are referenced to form _shells_.

Faces are associated with the 3D surface they are defined on. The boundary of faces are defined by a set of _edges_ which form a closed _loop_ in 3D space, following the contours of its associated surface. The boundary of a face is defined by its _outer loop_. In a similar manner to _shells_, faces may have internal hollow regions; such regions are called _inner loops_. Both outer loops and inner loops are represented by the `Loop` data structure.

![face-example](figures/face.png)

#### Example

This face is formed by a closed loop of 3D curves which are coincident to its associated planar surface. The grid visualizes the associated planar surface.

### Loop

A _loop_ is a set of _edges_ that combine to form a closed circuit. Edges are referenced in the loop so that their direction may be reversed.

![loop-example](figures/loop.png)

## Orientation

Many objects in boundary representation have two _orientation states_. Such objects are called _orientable_ objects. These are often given colloquial terms such as 'right/wrong side', 'up/down', 'in/out', 'forward/backward', et cetera. When referencing orientable objects, it is important to state which orientation of the object is desired. The orientation of a referenced object is described as being 'same-sense' or 'opposite-sense'. This extension utilizes the sign bit of floating point numbers to select the desired orientation. A positive sign selects the 'same-sense' and a negative sign selects the 'opposite-sense'.

#### Example

```json
"loops": [
  {
    "edges": [0, 1]
  },
  {
    "edges": [-0, -1]
  }
]
```

### Shell

Outer shells have all their faces oriented to point outside of the shell interior. Inner shells have all their faces oriented towards the shell interior. A shell could be referenced in its opposite sense when used to form a hollow region in a _solid_.

### Face

The circuit direction of the outer loop of a face determines its orientation. A face is typically oriented facing 'outwards' from the interior of its parent shell. This is the case when the outer loop forms a counter-clockwise circuit when viewed from above and outside the material. This circuit direction is often called its _winding order_. A face could be be referenced its in opposite sense when used to form an inner shell.

### Surface

The orientation of a surface refers to the normal vector evaluated at any point on the surface. In the opposite-sense, the normal vector evaluated at any point on the surface is inverted, i.e., all components are multiplied by -1.

### Edges

Edges are always oriented in the same sense as their associated 3D or 2D curves.

Edges are referenced in their opposite sense to form loops with a specific _winding order_ (see faces).

### Curve

Curves are oriented in the direction of increasing values of their evaluation parameter.