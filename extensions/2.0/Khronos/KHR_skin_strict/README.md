# KHR\_skin\_strict

## Contributors

* TBD

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

`KHR_skin_strict` introduces no new features or schema to the glTF file format. Instead, it defines a strict subset of the existing skinning specification, and imposes requirements — beyond those of the original specification — on tools creating glTF assets with a `skins` entry.

Implementations of skinned mesh animation vary between engines, for historical and performance reasons. This stricter skinning definition increases portability and simplicity for client implementations, at the cost of some flexibility and increased responsibilities for authoring tools.

When used, `KHR_skin_strict` must be an *optional* extension. Because it is a subset of the original specification, clients that do not recognize the extension can always safely read files that include the extension, so inclusion in the `extensionsRequired` list is neither necessary nor allowed. Presence of the `KHR_skin_strict` extension acts as a hint, for clients that recognize it, that the safer/stricter skinning requirements have been met.

Client implementations may choose to require that glTF assets containing one or more skins include the `KHR_skin_strict` extension, and reject assets that do not. Authoring tools are advised to include the extension whenever possible.

## Requirements

* All skins *must* define the `skeleton` property.
* The `skeleton` node for a skin *must* be a joint, and the direct parent of the entire joint subtree for that skin.
* All joints in a skin *must* define a single fully-connected subtree, sharing a single root joint, with no non-joint nodes coming between the joints of the tree.
* A joint node *must* belong only to a single skin.
* A joint node *must not* define any other functional role, such as `node.mesh`, `node.camera`, or `node.skin`. Likewise, joint nodes cannot use extensions that instantiate resources in the scene (like `KHR_lights_punctual`).

> **NOTE:** A node is considered a joint for a skin if and only if the node is included in the skin's `joints` list.

## Extending assets

The `KHR_skin_strict` extension is listed in `extensionsUsed`, but has no other explicit presence in the schema.

```json
{
    "extensionsUsed": ["KHR_skin_strict"],
    ...
}
```
