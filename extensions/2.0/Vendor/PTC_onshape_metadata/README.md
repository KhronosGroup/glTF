# PTC\_onshape\_metadata

## Contributors

* Paul Premakumar, PTC Inc
* Evan Nowak, PTC Inc

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension adds metadata information to the glTF model. While this extension is generic in design, it maps to the way Onshape identifies nodes using IDs and handles visibility. Here is an example

```javascript
"extensions": {
    "PTC_onshape_metadata": {
        "id": ["JHD"],
        "hidden": true
        }
}
```

This extension adds id and hidden to the glTF model:


### ID

The ID can be a string, an array of strings or left undefined. The content and how to interpret the ID is upto the application.

For example the Onshape application, uses an array of strings to denote an hierarchical ID. If the ID is a string, we treat it as a non-hierarchical ID.

### Hidden

There are cases where the initial state may require certain objects to be hidden, and showing them later on. If this property isn't specified, the object should be treated as visible or hidden = false. If a parent node is hidden, all its children should also be hidden.


## glTF Schema Updates

### JSON Schema

[gltf.PTC_onshape_metadata.schema.json](schema/gltf.PTC_onshape_metadata.schema.json)
