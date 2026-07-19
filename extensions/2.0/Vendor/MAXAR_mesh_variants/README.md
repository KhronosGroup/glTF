# MAXAR_mesh_variants

## Contributors

* Erik Dahlström, Maxar, [@erikdahlstrom](https://github.com/erikdahlstrom)
* Sam Suhag, Cesium, [@sanjeetsuhag](https://github.com/sanjeetsuhag)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows for a compact glTF representation of multiple mesh variants of an asset.

## Variants

For a glTF asset, a mesh `variant` represents a combination of meshes that are selected for rendering by a set of nodes based on _mappings_.

All available _variants_ are defined at the glTF root object extension as an array of objects, each with a _name_ property.

The _default_ property represents the index of the variant that is selected by default. The meshes that are mapped to the default variant must represent the set of meshes initially selected by the nodes for rendering, as per vanilla glTF behavior.

## Mappings

For a given node, each entry in the mappings array represents a mesh that should be selected for rendering when one of its variants is active. Each entry in the mappings array is an object that specifies a mesh by its index in the root level `meshes` array and and array of variants, each by their respective indices in the root level `variants` array.

A variant may only be used once across all the entries in the mappings array.

When the active variant is referenced in a mapping, a compliant viewer will select its meshes for
rendering. Application-specific logic may allow the activation of different variants per-node, enabling different runtime configurations of the model.

## Example

_This section is non-normative._

The following example illustrates the representation of a car model with varying states of damage applied to the model.

| Variants               |
| ---------------------- |
| `Pristine` - _default_ |
| `Damaged`              |
| `Destroyed`            |

At the root level, this will be described as follows:

```javascript
{
  "asset": {"version": "2.0"},
  "extensions": {
    "MAXAR_mesh_variants": {
      "default": 0,
      "variants": [
        {"name": "Pristine"  },
        {"name": "Damaged"   },
        {"name": "Destroyed" },
      ]
    }
  }
}
```

For the purposes of illustration, let's consider that the model consists of 3 nodes - body, wheels and lights.

```javascript
"nodes": [
  {
    "name": "Car Body",
    "mesh": 0,
    "extensions": {
      "MAXAR_mesh_variants" : {
        "mappings": [
          {
            "mesh": 0,
            "variants": [0]
          },
          {
            "mesh": 1,
            "variants": [1]
          },
          {
            "mesh": 2,
            "variants": [2]
          }
        ],
      }
    }
  },
  {
    "name": "Car Wheels",
    "mesh": 3,
    "extensions": {
      "MAXAR_mesh_variants" : {
        "mappings": [
          {
            "mesh": 3,
            "variants": [0]
          },
          {
            "mesh": 4,
            "variants": [1]
          },
          {
            "mesh": 5,
            "variants": [2]
          }
        ],
      }
    }
  },
  {
    "name": "Car Lights",
    "mesh": 6,
    "extensions": {
      "MAXAR_mesh_variants" : {
        "mappings": [
          {
            "mesh": 6,
            "variants": [0]
          },
          {
            "mesh": 7,
            "variants": [1, 2]
          },
        ],
      }
    }
  }
]
```

## Optional vs. Required

This extension is considered optional, meaning it should be placed in the glTF root's `extensionsUsed` list, but not in the `extensionsRequired` list.

## Schema Updates

- **glTF extension JSON schema**: [MAXAR_mesh_variants.schema.json](./schema/glTF.MAXAR_mesh_variants.schema.json)
- **glTF node extension JSON schema**: [node.MAXAR_mesh_variants.schema.json](./schema/node.MAXAR_mesh_variants.schema.json)
