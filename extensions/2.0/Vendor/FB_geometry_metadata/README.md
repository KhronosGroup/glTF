# FB\_geometry\_metadata

## Contributors

* Pär Winzell, Facebook, [@zellski](https://twitter.com/zellski)
* Andrew Prasetyo Jo, Facebook

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension annotates glTF `scene` objects with a summary of the cumulative geometric complexity and scene-space extents of the scene's associated scene graph. Each recursively referenced mesh is enumerated for total vertex and primitive count, and each individual vertex is transformed into scene-space for a perfectly computed bounding box.

Some proven real-world applications for this information:
- When a viewer needs to make a quick, high-level decision which `scene` to display. This is perhaps most meaningful e.g. when `scenes` are used to represent different variants (e.g. LoD levels) of a model.
- When server-side systems need to make decisions based on geometric complexity, and don't want to have to include their own complex matrix/vector code.

## Specifying Geometry Data

The summary data is defined as an optional extension to a glTF `scene`, by adding an `extensions` property and defining a `FB_geometry_metadata` property with the scene metadata inside it:

```javascript
"extensions": {
    "FB_geometry_metadata" : {
        "vertexCount": 28727,
        "primitiveCount": 15451,
        "sceneBounds": {
            "min": [
                -3.0651053919852,
                0,
                -2.6551087079312
            ],
            "max": [
                3.0651053919852,
                9.8512265772065,
                2.6538096091834
            ]
        }
    }
}
```

## Limitations
All these values should be computed with disregard for animation: all morph targets are inactive, all `node` transforms are in their static form and unaffected by any animation. Consequently, an application that parses the model should interpret the values the same way.

## Computing the data

Conceptually, computing and gathering this information is very straight-forward. On a hypothetical glTF `node` object:

```javascript
  public function computeGeometryMetadata(
    GltfMat44f $world_transform,
    inout A3DGltfBounds $bounds,
    inout int $vertex_count,
    inout int $primitive_count,
  ): void {
    // compute world transform forward
    $world_transform = A3DGltfMath::multiplyMatrices(
      $world_transform,
      $this->computeLocalTransform(),
    );

    // if this node references a mesh, traverse it
    $this->mesh?->computeGeometryMetadata(
      $world_transform,
      inout $bounds,
      inout $vertex_count,
      inout $primitive_count,
    );

    // depth-first scene graph traversal
    foreach ($this->getChildren() as $child) {
      $child->computeGeometryMetadata(
        $world_transform,
        inout $bounds,
        inout $vertex_count,
        inout $primitive_count,
      );
    }
  }
```

and on the `mesh` `primitive`, simply:

```javascript
  public function computeGeometryMetadata(
    GltfMat44f $world_transform,
    inout A3DGltfBounds $bounds,
    inout int $vertex_count,
    inout int $primitive_count,
  ): void {
    $vertex_count += $this->getVertexCount();
    $primitive_count += $this->getPrimitiveCount();

    // this is very expensive, and will one day move to native code
    foreach ($this->positions as $position) {
      $bounds->expandByVec(
        A3DGltfMath::transformVec3($world_transform, $position),
      );
    }
  }
```
