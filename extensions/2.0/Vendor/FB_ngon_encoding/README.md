# FB_ngon_encoding

## Contributors

* Pär Winzell, Facebook, [zell@fb.com](mailto:zell@fb.com)
* Michael Bunnell, Facebook

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

While glTF can only deliver a polygon mesh after it's been decomposed into triangles, there are cases where access to the source topology is still useful. One common such case is [Catmull-Clarke subdivision surfaces](https://en.wikipedia.org/wiki/Catmull%E2%80%93Clark_subdivision_surface), which work best with quadrilaterals.


This extension contains **no additional data**:
```
  "meshes": [
    {
      "name": "Apple",
      "primitives": [
        {
          "indices": 0,
          "attributes": {
            "POSITION": 1,
            "NORMAL": 2,
            "COLOR_0": 3,
            "TEXCOORD_0": 4
          },
          "material": 0,
          "mode": 4,
          "extensions": {
            "FB_ngon_encoding": {}
          }
        }
      ]
    }
  ],
```

Rather, it marks the use of an implicit scheme where the **order of triangles and per-triangle vertex indices** holds all the information needed to reconstruct the original structure, and can be generated via a simple triangulation process:

- For each polygon `p`, choose one identifying vertex `v(p)`,
- Break the polygon into a fan of triangles, all anchored in that same `v(p)`,
  - (It's obvious that this can be done for convex polygons, e.g. quads, but the true constraint is a bit less strict.)
- Proceed with the next polygon `p'` where, importantly, `v(p') != v(p)`.

This lays each polygon's constituent triangles down sequentially, grouped by sharing the same `triangle.vertices[0]`. Given such a sequence, it's then trivial to decode the original structure with linear iteration. A sketch of a possible implementation can be found [below](#Resources).

Advantages of this scheme:
- The meshes remain valid glTF 2.0,
- Asset size is unchanged, since we add no new data,
- The scheme works with arbitrary mixtures of triangles, quads, and so on,
- Very simple to implement.

There are a few geometric concerns for non-quad cases:
- Polygons that can't be decomposed into a triangle fan must first be cut into ones that can,
- Even polygons that are technically convex can yield triangles narrow enough to be nearly degenerate.

On a pragmatic ecosystem note, we must also remember that glTF 2.0 normally allows reordering of triangles and triangle vertices (in fact, such operations are common optimisations), and so tools unfamiliar with this extension would likely destroy the structure we rely on here. Then again, they would presumably not preserve the extension in their output, so at worst one ends up with glTF that's unextended yet still valid.


## glTF Schema Updates

* **JSON schema**: [glTF.FB_ngon_encoding.schema.json](schema/glTF.FB_ngon_encoding.schema.json)

## Known Implementations

* [FBX2glTF](https://github.com/facebookincubator/FBX2glTF) (shortly)
* Internal Facebook AR/VR development

## Resources

A sketch of an implementation. Output each polygon as a fan of individual triangles. Make sure that the triangles for a polygon do not start with the same vertex index as the triangles of the preceding polygon.
```
// convert mesh to triangles
int startIndex = -1; // invalid vertex index
for (int p = 0; p < numPolygons; p++) {
  int start = startIndex == polygon[p].vertex[0] ? 1 : 0;
  startIndex = polygon[p].vertex[start];
  int n = polygon[p].numVertices;
  for (int v = start+2; v < start+n; v++)
    EmitTriangle(startIndex, polygon[p].vertex[v-1], polygon[p].vertex[v == n ? 0 : v]);
}
```

Reconstruction reverses the process:
```
// convert triangles back to original polygons
int startIndex = -1;
for (int t = 0; t < numTriangles; t++) {
  if (triangle[t].vertex[0] != startIndex) {
    if (startIndex >= 0)
      EndPolygon();
    BeginPolygon();
    startIndex = triangle.vertex[0];
    EmitVertex(startIndex);
    EmitVertex(triangle[t].vertex[1];
  }
  EmitVertex(triangle[t].vertex[2]);
}
if (startIndex >= 0)
	EndPolygon();
```
