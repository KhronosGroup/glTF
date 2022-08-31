# GRIFFEL\_bim\_data

## Contributors

* Kiryl Ambrazheichyk, Griffel Studio LLC, [kiryl@griffelstudio.com](mailto:kiryl@griffelstudio.com)
* Aliaksandr Valodzin, Griffel Studio LLC, [alexander@griffelstudio.com](mailto:alexander@griffelstudio.com)
* Aliaksandr Nazarau, Griffel Studio LLC, [@Alexanderexe](https://github.com/Alexanderexe)
* Ekaterina Kudelina, Griffel Studio LLC, [@uncio](https://github.com/uncio)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension allows keeping domain specific data in relation to geometry.

It's applicable when glTF standard `extras` property is not enough to store huge amount of metadata or when it's insufficient from the file size or data complexity perspective.

The extension was originally developed for architecture, engineering and construction (AEC) industry to keep Building Information Model (*BIM*) data, but it's suitable for any other industry which operates huge amounts of information. We encourage any subject matter experts to discuss and improve this specification to create useful general metadata glTF extension.

### Objectives

Keep metadata of each real world object in the most compact and simple way possible.

### Assumptions

Each node with a mesh represents a real world object.
Node tree represents some logical hierarchy of real world objects. For a construction site with two buildings it might look like this:

```text
Model 1
Model 2
├Storey 1
└Storey 2
 ├Category 1
 └Category 2
  ├Element 1
  └Element 2
```

Where:
Model - is a building,
Storey - includes all elements which lay or rise up starting from a particular level,
Category - groups elements by some parameter (columns, furniture, ducts, etc.),
Element - leaf `node` with `mesh` which represents a real construction product and have associated meta data.

This is a recommended approach to compose a gltf file, although the extension doesn't restrict mesh to node attachment - any node can still have a mesh and/or associated meta data. Also nodes hierarchy can be different - depending on the desired logical grouping of elements.

### Approach

A single piece of metadata for the node is represented by instance `property`. Property is a key-value pair, like 'name of the object's parameter' - 'its value'. To keep it simple name and value of the property are `strings`.
Node can also have a `type`. Type is a set of properties which is common for many nodes.
Instance properties override the same type properties (with the same key) for a node.

#### Example

There are two timber doors in the building. Each is 900 mm (3') wide. First is 2,1 m (7') high, second is 2,4 m (8') high.
In this example properties are: "Width - 900 mm", "Height - 2,1 m", "Height - 2,4 m", "Material - Timber".
A single `type` here is represented by the collection consisting of the first and the last properties as width and material are common properties for both doors.

To save some space for storing properties they are written only once in the properties collection and referenced by each node where needed. To save even more space each unique property name and each unique property value are stored in separate collections. This approach is beneficial over keeping metadata in `extras` for large amounts of data.

Types and nodes reference properties by index.
Each node can reference any number of properties and only one type.

## Implementation notice

Given the same two doors from the example above, there are two options to implement the extension:

### Embed properties into glTF json

```javascript
"nodes": [
  {
    "name": "Door 1",
    "extensions": {
      "GRIFFEL_bim_data": {
        "properties": [
          0
        ],
        "type": 0
      }
    }
  },
  {
    "name": "Door 2",
    "extensions": {
      "GRIFFEL_bim_data": {
        "properties": [
          3
        ],
        "type": 0
      }
    }
  }
],
"extensions": [
  "GRIFFEL_bim_data": {
    "propertyNames": [
      "Height",
      "Width",
      "Material"
    ],
    "propertyValues": [
      "2,1 m",
      "900 mm",
      "Timber"
      "2,4 m",
    ],
    "properties": [
      {
        "name": 0,
        "value": 0
      },
      {
        "name": 1,
        "value": 1
      },
      {
        "name": 2,
        "value": 2
      },
      {
        "name": 0,
        "value": 3
      }
    ],
    "types": [
      {
        "properties": [
          1,
          2
        ]
      }
    ]
  }
]
```

### Write properties to separate binary file

In this case each node points to the buffer view which references the buffer where metadata for this node can be found. File is a serialized `GRIFFEL_bim_data` extension object with additional `nodeProperties` field. This field maps node properties and their types to nodes by node index.

[MessagePack](https://msgpack.org/) must be used for serializing extension object in a separate binary file to achive the most compact file size and the fastest deserialization speed.

```javascript
// gltf content:
"nodes": [
  {
    "name": "Door 1",
    "extensions": {
      "GRIFFEL_bim_data": {
        "bufferView": 99
      }
    }
  },
  {
    "name": "Door 2",
    "extensions": {
      "GRIFFEL_bim_data": {
        "bufferView": 99
      }
    }
  }
]

"bufferViews": [
  // buffer view indexed 99:
  {
    "buffer": 1,
    "byteOffset": 0,
    "byteLength": 1234
  }
  
"buffers": [
  // buffer indexed 1:
  {
    "uri": "projectName.meta",
    "byteLength": 1234
  }
]

// Content of 'projectName.meta':
{
  "nodeProperties": [
    {
      "node": 0,
      "properties": [
          0
      ],
      "type": 0
    },
    {
      "node": 1,
      "properties": [
          3
      ],
      "type": 0
    }
  ],
  // propertyNames, propertyValues, properties and types collections are the same
}
```

So, for each node, if GRIFFEL_bim_data extension has `properties` or `type` - look for them in the same gltf json file. If the extension states `bufferView` - look for the data in the separate .meta file by the url from the buffer.
Nodes can have both embedded and separately stored properties, in that case any or both can be used. Exporters should take care of possible data duplication. Though, cross references between embedded and external data are not allowed. Importers should treat embedded values as of higher priority than values, stored in a separate file, as the file may be obsolete or corrupted more likely. If separately stored data references same node many times specifying new or additional properties for it, it is up to Importer: ignore duplicates or apply them all.

## glTF Schema Updates

### JSON Schema Root

[glTF.GRIFFEL_bim_data.schema.json](schema/glTF.GRIFFEL_bim_data.schema.json)

### JSON Schema Node

[node.GRIFFEL_bim_data.schema.json](schema/node.GRIFFEL_bim_data.schema.json)

## Standalone JSON Schema

[GRIFFEL_bim_data.schema.json](schema/GRIFFEL_bim_data.schema.json)

## Known Implementations

* [glTF Exporter](https://apps.autodesk.com/RVT/en/Detail/Index?id=8451869436709222290) - add-in for Autodesk Revit

## Resources

* [MessagePack](https://msgpack.org/) - binary serialization format
* [Griffel Studio Blog](https://griffelstudio.com/blog/) - information about extension
