# KHR_nodes_disable

## Contributors

* Norbert Nopper, UX3D (@UX3DGpuSoftware)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

In general, all nodes with its child nodes are traversed and the meshes are rendered every frame. However, there are use cases, where specific meshes should be visible and some should be hidden. During runtime, these nodes are shown or others are hidden. A sample use case are products having specific attachments.  
Goal is to have all meshes in the glTF and to show/hide specific meshes depending on the product situation.
  
If the visible property is `false`, the contents of the node are not visible or used. Main use case is not to render the mesh. By default, the change affects only the specified node. However, if the `recursive` property is set to `true`, all child nodes are affected as well.
  
In general, if the `recursive` property is enabled, a renderer can stop traversing the children. There is an exception, if the nodes are joints. In this case, the matrices are still calculated.
  
This extension does have an affect on the node itself. E.g. if the node contains a punctual light as given in the extension, the light would not be used for lighting the scene as well.

## glTF Schema Updates

The `KHR_nodes_disable` may contain the following properties:

| Name        | Type      | Default | Description
|-------------|-----------|---------|-----------------------------
| `visible`   | `boolean` | `true`  | Node is visible or not.
| `recursive` | `boolean` | `false` | Valid for children as well.

### JSON Schema

[KHR_nodes_disable.schema.json](schema/KHR_nodes_disable.schema.json)

### Example JSON

```json
{
  "node": [
    {
      "extensions": {
        "KHR_nodes_disable": {
          "visible": false,
          "recursive": true
        }
      }
    }
  ]
}
```
