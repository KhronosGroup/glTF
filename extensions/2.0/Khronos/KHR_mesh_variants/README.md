# KHR_mesh_variants

## Contributors

- Edgar Simson, OLV ([edgar.simson@olv.global](mailto:edgar.simson@olv.global))

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

Builds upon and to be used in tandem with [KHR_materials_variants](https://github.com/KhronosGroup/glTF/pull/1681).

## Overview

This extension adds variant support for model geometries to cover various usecases discussed in 3D Commerce Configurability TSG.

### Context

A lot of product configurators allow users to choose between multiple variants of some parts of the full product or just toggle visibility of some parts. For example, that could be:

- rims and spoilers of a car
- face guards of a baseball helmet
- legs of a sofa
- straps of a watch
- laces of a shoe
- strap and hook for a handbag
- armrest for a chair

Currently to support that developers are writing proprietary logic to change visibility of parts of the geometry based on proprietary hints added in the 3D model by artists. This makes it hard to reuse the same model across viewers.

To simplify artist and developer workflows and increase interoperability, this extension defines how to show or hide meshes based on active tags passed to the viewer.

This extension does not cover exposure of configuration options to the user or gathering user input. It considers that too coupled with business logic and requiring custom logic beyond the scope of this extension.

### Definition

The extension is available to node (TBD) elements.
It defines two optional array fields matching the possible visibility states - `visibleWhen` and `hiddenWhen`.
Each visibility field can contain zero or more conditions to trigger the matching visibility state.
Each condition can be either a single tag or an array of one or more tags.
Each tag is a single string.
When all tags in any of the conditions are included in the active tags of the viewer, that triggers the matching visibility state.
If both visibility states are triggered the element remains visible.

For related usage of tags see [KHR_materials_variants](https://github.com/KhronosGroup/glTF/pull/1681).

### Examples

There are several scenarios that need to be supported

- hide mesh by default
  ```js
  // TODO
  ```
- show mesh (hide mesh unless)
  - when a particular tag is active - likely the most popular usage
    ```json
    "KHR_mesh_variants": {
        "visibleWhen": [
            "laces_thin",
        ]
    }
    ```
    or in the array form
    ```json
    "KHR_mesh_variants": {
        "visibleWhen": [
            ["laces_wide"],
        ]
    }
    ```
  - when a list of tags is active
    ```json
    "KHR_mesh_variants": {
        "visibleWhen": [
            ["faceguard_flap", "faceguard-side_left"]
        ]
    }
    ```
  - when at least one of multiple conditions is active
    ```json
    "KHR_mesh_variants": {
        "visibleWhen": [
            ["spoiler_A", "trunk_F"],
            "spoiler_B",
            "spoiler_D",
            "demo-mode-on"
        ]
    }
    ```
- hide mesh
  - all the same conditions as for showing a mesh
    ```json
    "KHR_mesh_variants": {
        "hiddenWhen": [
            "laces_none",
        ]
    }
    ```

## HELP WANTED

Several topis are not yet clear to the author:

- Which structure should host the extension - mesh, node or primitive?
- How to hide some meshes by default, also in viewers that do not support this extension?
- Is it possible to achieve similar mental model to `MSFT_lod` while enabling empty nodes by default?
- Does the name `KHR_mesh_variants` reflect the functionality close enough to keep it synchronized with `KHR_materials_variants` or would `KHR_mesh_visibility` fit better?

## glTF Schema Updates

TODO

### JSON Schema

TODO

### Example JSON

TODO

## Known Implementations

TODO
