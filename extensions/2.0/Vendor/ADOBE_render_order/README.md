# ADOBE_render_order

## Contributors

* Jaswant Singh Ranawat, Adobe(ranawat@adobe.com)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

To avoid Z-fighting in two coplanar meshes render order is needed with depth writing and depth testing options. It could be helpful while drawing three-dimensional translucent objects to get different appeareances. If you want to render both opaque and translucent objects in the same scene, then you want to use the depth buffer to perform hidden-surface removal for any objects that lie behind the opaque objects. Therefore this extension adds four properties to node `group`, `renderOrder`, `depthTest` and `depthWrite`. `renderOrder` value allows the default rendering order of scene graph objects to be overridden although opaque and transparent objects remain sorted independently. When `group` property is set to true, all descendants nodes will be sorted and rendered together. Sorting is from lowest to highest renderOrder.

## glTF Schema Updates

It may contain the following properties:

| Name       | Type       | Default      | Description |
|----------|------------|--------------|--------------------------------- |
| `group`   | `boolean` | `false` | specifies whether node is a group or not. |
| `renderOrder`   | `integer` | `0` | The render order of the node in the parent group. |
| `depthTest` | `boolean`   | `true`        | specifies whether to perform depth test or not. |
| `depthWrite`    | `boolean` | `true` | specifies whether to perform depth write or not. |


if none of the parent is specified as group and renderOrder is provided then scene node is considered as group. Marking leaf nodes as group will have no impact.

### Example JSON

```json
{
    "nodes": [
        {
            "children" : [ 1,4 ],
            "extensions": {
                "ADOBE_render_order": {
                "group" : true
                }
            }
        },
        {
            "children": [ 2,3 ],
            "extensions": {
                "ADOBE_render_order": {
                "group" : true,
                "renderOrder": 0,
                }
            }
        },
        {
            "mesh": 0,
            "extensions": {
                "ADOBE_render_order": {
                "renderOrder": 0,
                "depthTest" : true,
                "depthWrite" : false
                }
            }
        },
        {
            "mesh": 1,
            "extensions": {
                "ADOBE_render_order": {
                "renderOrder": 1,
                "depthTest" : true,
                "depthWrite" : false
                }
            }
        },
        {
            "mesh": 2,
            "extensions": {
                "ADOBE_render_order": {
                "renderOrder": 1,
                "depthTest" : true,
                "depthWrite" : false
                }
            }
        }
    ]
}
```

### JSON Schema

TODO: Links to the JSON schema for the new extension properties.

## Known Implementations

* TODO: List of known implementations, with links to each if available.

## Resources

* TODO: Resources, if any.
