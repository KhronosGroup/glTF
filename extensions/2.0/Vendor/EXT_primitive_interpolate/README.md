# EXT\_primitive\_interpolate

## Contributors

* Emmett Lalish, [@elalish](https://github.com/elalish/)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension provides an extra data channel defining a smoothly-curving surface that interpolates between the geometric vertices, which allows a consumer of this file to arbitrarily increase the tesselation resolution of the mesh. This can be considered a type of compression, allowing a coarse triangle mesh to represent a smooth object. It is an alternative to e.g. NURBS and Subdivision Surfaces, both of which provide smooth surfaces, but neither of which interpolates the existing glTF vertices.

This extension works by replacing each straight-line edge with a Bezier curve and defining a smooth interpolation surface between these edge curves. The remainder of this specification will refer extensively to normals and tangents, but it is not referring to the glTF `NORMAL` and `TANGENT` attributes, which are independent of this specification. The tangent will refer to the extra data channel defining the position of a weighted edge Bezier control point relative to its vertex end-point. The normal at a vertex will refer to the normalized cross-product of two of these tangent vectors adjacent at the corner of a mesh triangle, or to the geometric normal of the interpolated surface at any point.

The edge Bezier curves also define the normal vector along their length, based on the adjacent tangents at each end of the halfedge. They have a property that if the three tangents at a vertex are coplanar at both ends of the edge, then the computed normals will match on both sides at every point along the edge curve, ensuring the two surfaces are G1 continuous across that edge. This sets the approach apart from PN Triangles, which are only G1 at the vertices, but G0 at the edges - continuous, but with a sharp crease. 

However, there is no requirement that the tangents be coplanar around every vertex - this is merely a simple way to ensure smoothness if desired. Non-coplanar tangents create sharp edges, but those edges are still G0 continuous and will themselves still smoothly curve, following the same Bezier shape as before. By having one end of an edge have coplanar tangents and the other not, it is even possible for a sharp crease to smoothly transition back to a G1 surface. 

This interpolation scheme handles both triangles and quads - the latter by linking two triangles by setting the halfedge tangents between them to have negative weight.

The number of tangents is equal to the number of triangle indices: 3 * numTri. For any edges that are oriented and 2-manifold (there is exactly one other halfedge with swapped vertices, and no other halfedge with the same vertices), a cubic Bezier is created, with each halfedge contributing one control point relative to its starting vertex. All other edges create quadratic Beziers, using the single control point provided by the tangent. To enable cubic Beziers along edges where vertices are duplicated due to having different attribute values (e.g discontinuous UVs), the `EXT_mesh_manifold` extension can be used in conjunction to provide the missing manifoldness information to match up halfedges. 

This extension does not specify or restrict how a consumer should subdivide or tesselate the mesh, only where each newly-created vertex should be placed based on its barycentric coordinates within its source triangle. A viewer may choose to e.g. tesselate once based on user-input, or even use a tesselation shader to dynamically increase resolution based on the current view. 

## Extending Primitives With Interpolation Data

A glTF mesh primitive is denoted as ready for interpolation by adding an `EXT_primitive_interpolate` extension object to it and obeying the following restrictions:

- it **MUST** use the `TRIANGLES` topology type;
- the `indices` property **MUST** be defined;

The required `halfedgeTangents` property added in the extension references an `accessor` which:
- **MUST** be of type `VEC4`;
- **MUST** have the same `count` as the `indices`;

Each tangent element of this accessor:
- **MUST** have all values finite;
- **SHOULD** have a weight (fourth component) between zero and one, or equal to negative one to indicate a quad;
- The three halfedge tangents corresponding to a single triangle **MUST** have no more than one with a negative weight;
- Two paired halfedge tangents (relating to the same manifold edge) **MUST** have weights with the same sign.

An example is given below, representing a portion of the included sample's JSON. This object has two materials/primitives with a single shared vertex attribute array. The triangle indices are also a single shared array, partitioned into separate primitive accessors. 

```json
"meshes": [
    {
      "primitives": [
        {
          "attributes": {
            "POSITION": 2,
            "TEXCOORD_0": 3
          },
          "mode": 4,
          "material": 0,
          "indices": 0,
          "extensions": {
            "EXT_primitive_interpolate": {
              "halfedgeTangents": 1
            }
          }
        }
      ]
    }
  ],
"accessors": [
    {
      "name": "triangle indices",
      "type": "SCALAR",
      "componentType": 5125,
      "count": 1728,
      "bufferView": 0,
      "byteOffset": 0
    },
    {
      "name": "halfedge tangents",
      "type": "VEC4",
      "componentType": 5126,
      "count": 1728,
      "bufferView": 1,
      "byteOffset": 0
    },
    {
      "name": "position",
      "type": "VEC3",
      "componentType": 5126,
      "count": 2292,
      "max": [
        50,
        50,
        50
      ],
      "min": [
        -50,
        -50,
        -50
      ],
      "bufferView": 2,
      "byteOffset": 0
    },
    {
      "name": "tex-coord",
      "type": "VEC2",
      "componentType": 5126,
      "count": 2292,
      "bufferView": 2,
      "byteOffset": 12
    }
  ],
```

## Constructing The Interpolated Surface

### Edge Bezier

The weighted Bezier control points are constructed as follows. For the kth tangent, start with the corresponding starting vertex of this halfedge, at `indices[k]`, which we will call $v_0$, while the end vertex will be $v_1$. Add to that the x, y, z (first three components) of the tangent vector, and then use the fourth component as the weight. In this form, the x, y, z elements of $b_0$ **MUST** be transformed using the same transformation as is then applied to the vertices. 

Combining the two halfedge tangents for a given manifold edge gives a weighted cubic Bezier with control points $v_0$, $b_0$, $b_1$, and $v_1$, where the end points have weights equal to one. Weighted Beziers are evaluated in homogeneous coordinates: $[wx, wy, wz, w]$, and converted to geometric coordinates by normalization:
$$Hnorm(p)=p/p_w$$

Using the standard definition of linear interpolation:
$$lerp(a,b,x)=(1-x)a+xb$$
the kth edge will yield two intermediate homogeneous points. For a cubic Bezier:
$$a0=lerp(lerp(v0,b0,x),lerp(b0,b1,x),x)$$
$$a1=lerp(lerp(b0,b1,x),lerp(b1,v1,x),x)$$
Or for a quadratic Bezier, when only one tangent is available for an edge:
$$a0=lerp(v0,b0,x)$$
$$a1=lerp(b0,v1,x)$$
Now the final Bezier position is:
$$Bez(k, x)=Hnorm(lerp(a0,a1,x))$$
and the tangent vector along this curve is:
$$BezT(k, x)=Hnorm(a1)-Hnorm(a0)$$

The above defines the shape of the edge curve (note the symmetry that the above equations give the same results when run from the opposite perspective, so the two halfedges will always agree). However, the orientation of the surface along that curve must also be defined. 

### Edge Orientation

Start by defining 3D TBN orientations for the end vertices of the halfedge. At the start vert, the tangent is just the halfedge tangent. The normal is the cross product of the two halfedge tangents corresponding to the corner of this triangle/quad (pointing to the outside). The bitangent is the cross product of the normal and tangent. All of these vectors are normalized, giving an orthonormal rotation matrix. This is converted to an equivalent quaternion $q_0$. 

The end vertex orientation, $q_1$, is similar, but its tangent is opposite of its halfedge tangent direction, so that for a flat surface $q0==q_1$. 

The intermediate orientation along the edge curve is $$q_i=slerp(q_0,q_1,x)$$
and the final orientation is $q_x=q_tq_i$
where $q_t$ transforms the tangent of $q_i$ to the $BezT(k, x)$ tangent direction by rotating about their cross product.

The quaternion $slerp$ function (spherical linear interpolation) must carefully choose the direction of rotation to ensure it roughly follows the Bezier's tangent evolution.

### Intermediate Bezier

Intermediate Bezier curves are constructed using the interpolated bezier position above as one end point. The corresponding cubic Bezier control point is defined by
$$b_{01}=Bez(k,x)+lerp(q_xq_0^{-1}t_{02},q_xq_1^{-1}t_{12},x)$$
for the 3D portion and the weight is just the lerp between these two tangent weights, and then these are combined into homogeneous coordinates again. 

In the case of a quad, the other side of this cubic Bezier comes from a symmetric calculation using the opposite edge curve. For a triangle, the opposite end is the opposite triangle vertex and its control point is defined by the lerp between the two halfedge tangents incident on that corner.

### Final Surface

For a quad, two intermediate Beziers are combined as a weighted average based on the $u,v$ coordinates of the point within the quad. The first intermediate Bezier is interpolated by $u$, while its end-point Beziers are interpolated by $v$, and its weight is $v*(1-v)$. The second is the opposite, with weight $u*(1-u)$.

For a triangle, three intermediate Beziers are combined as a weighted average based on the barycentric coordinates of the point within the triangle. For the corner corresponding to barycentric $u=1$, the intermediate Bezier intersecting that corner is interpolated by $u$, while the end-point Beziers are interpolated by $w/(1-u)$, and its weight is $vw$.

These weights ensure that the final surface normal always matches the edge curve orientation at its boundaries. 

## References

- Open source interpolation algorithm available [here](https://github.com/elalish/manifold/blob/75f42f9c53a3e089aaafacf453a32933f34995d6/src/smoothing.cpp#L61C10-L247C3).