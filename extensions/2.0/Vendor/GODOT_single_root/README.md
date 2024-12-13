# GODOT_single_root

## Contributors

- Aaron Franke, Godot Engine.

## Status

Complete

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows flagging glTF assets as having a single root node to enable more efficient and accurate workflows with various DCC tools, such as by directly importing the asset's node hierarchy without creating additional scene nodes. This flag imposes certain constraints on glTF assets as described below.

Many games engines import glTF files as objects with a single root node. Unity has prefabs with a single root GameObject, and Godot has scenes with a single root node. Since glTF allows defining multiple root nodes, engines will insert the glTF root nodes as children of the "real" root node, which makes it difficult for a glTF file to define the properties of the "real" root node. This is important for things like physics bodies and character controllers, which are defined on the root node, so that all child nodes are moved with the body. Aside from physics, a single root node would avoid an extra node in the tree when round-tripping between glTF and these engines.

The `GODOT_single_root` glTF extension solves this problem by imposing additional constraints to ensure the entire glTF file can be parsed into one of these objects with a single root node. Therefore, all other nodes are either descendants of this node, or are not in the glTF scene node tree (in which case they behave the same as base glTF). Implementations can detect `GODOT_single_root` and import the single root node as the object's "real" root node in the scene/prefab/etc. The extension contains no data, it only restricts behavior. The restrictions are as follows:

- The document `"scenes"` array MUST have exactly one scene.
- The document `"scene"` index MUST be set to 0, the index of the only scene in the `"scenes"` array.
- The scene MUST have exactly one node, the single root node.
- The scene's node index MUST be set to 0, the index of the first node in the `"nodes"` array.
- The `"translation"`, `"rotation"`, `"scale"`, and `"matrix"` properties of the single root node MUST either be not defined or have default values.

These restrictions make the glTF `"scenes"` and `"scene"` properties fully redundant, so that implementations using `GODOT_single_root` do not need to parse the `"scenes"` or `"scene"` properties. Since there is only one root node, node index 0 is reserved as the single root node. However, the `"scenes"` and `"scene"` properties must be kept in the glTF file for compatibility with implementations that do not support this extension. This means that the glTF file must contain properties equivalent to this:

```json
    "scene": 0,
    "scenes": [{ "nodes": [0] }],
```

Note: Other properties set on the scene JSON, such as the name etc, are allowed, but not required or expected. If the scene does not have a name, use the single root node's name as the scene name.

The single root node acts as the root object that represents the entire glTF file, since all other nodes are either descendants or unused, and all resources such as meshes are either used by this node, by descendants, or unused. This requires the glTF file to only contain one scene, with one root node. Multiple scenes per file is not allowed, since it goes against this goal.

Forbidding the root node to have a transform is required because:
- A glTF file does not have a transform, only its nodes do. With this extension, the single root node represents the glTF file.
- This keeps local directions such as forward consistent when orienting the scene vs orienting the single root node.
- A glTF scene's transform should be determined only by the instances of the scene, such as instances inside of a game engine.
- Without this restriction, a root node with a transform would be transformed differently in implementations with and without `GODOT_single_root` when using functions such as look at, which must not happen.

### Example:

This minimal example defines a node named SingleRootNode as the scene's single root node:

```json
{
    "asset": {
        "version": "2.0"
    },
    "extensionsUsed": [
        "GODOT_single_root"
    ],
    "nodes": [
        {
            "name": "SingleRootNode"
        }
    ],
    "scene": 0,
    "scenes": [{ "nodes": [0] }]
}
```

## glTF Schema Updates

This extension consists of a new `"GODOT_single_root"` string which can be added to the glTF's `"extensionsUsed"` array and because it is optional, it does not need to be added to the `"extensionsRequired"` array.

The `GODOT_single_root` extension defines no data, and should not be added to `"extensions"` anywhere. It only needs to exist in `"extensionsUsed"`.

### glTF Object Model

This extension defines no properties that can be read or manipulated by the glTF Object Model.

### JSON Schema

This glTF extension has no schema.

## Known Implementations

- Godot Engine 4.2 or later: https://github.com/godotengine/godot/pull/81851

## Resources:

- Godot PackedScene: https://docs.godotengine.org/en/stable/classes/class_packedscene.html#class-packedscene-method-instantiate
- Unity Prefabs: https://docs.unity3d.com/Manual/Prefabs.html
