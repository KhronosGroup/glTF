# GRIFFEL\_bim\_data

## Contributors

* Kiryl Ambrazheichyk, Griffel Studio LLC, [kiryl@griffelstudio.com](mailto:kiryl@griffelstudio.com)
* Aliaksandr Valodzin, Griffel Studio LLC, [alexander@griffelstudio.com](mailto:alexander@griffelstudio.com)
* Aliaksandr Nazarau, Griffel Studio LLC
* Ekaterina Kudelina, Griffel Studio LLC, [@uncio](https://github.com/uncio)

## Status

Draft

## Dependencies

Written against the glTF 2.0 spec.

## Overview


AEC industry operates huge 3D models with enormous data bound to them. This data is used to represent real world objects, construction parts and elements in digital environment. This data is nesessary to share designs between companies, different software, between architects and engineers, designers and workers on site.
IFC has it all.
But it's complex industry standard file format, all-inclusive, with wide specification.
Very often we simply need to display BIM models with some data for elements. We need to click elements throughout the 3D model and quickly read a few parameters of these elements: like whether it's a wood or steel frame, what width the window has or what top elevation of the floor is. We don't need to know all the classifications and miscellaneous properties of a particular door knob - just to see the geometry and most important stuff.
That's why glTF BIM data extension was created - to keep the most nesessary data within 3D models in the most compact and simple way just to display it on the other side. That's often needed in AR applications, web viewers for BIM models, HoloLens and Unity projects, while prototyping or sharing architectural designs.


The goal was to keep all the data in a simple and compact way.
That's achived by storing only unique property names and values in separate arrays of strings. No matter what type property value is: text, number or boolean - it's up to the reader to convert the value from string or display it as it is written in the extension.
Property names and values are mapped in a separate array of unique key value pairs.





This extension defines Building Information Modeling properties for Nodes in the glTF file.

The extension assumes that each Node with a Mesh on it represents some building element.

For the glTF model to be able to represent a building model Nodes tree SHOULD be as follows:
```
Main Model
├Storey 1
└Storey 2
 ├Category 1
 └Category 2
  ├Element 1
  └Element 2
Linked Model
├Storey 1
└Storey 2
  └...so on
```

This extension actually can be adopted by any other industry like medical or agriculture, where some metadata should be transfered with the leave Nodes. Let's collaborate together to make it useful and worldwide.




## Examples

There are two concrete columns, one is 3 meters high and 400x400 mm section, the other is 600 mm circular section and 4 meters high.
Properties of the first:




## Implementation notice

There are two options to implement extension:
### Embed properties into glTF json
### Write properties to separate binary file which optionally can be compressed
Use [MessagePaсk](https://msgpack.org/) to serialize and compress BIM data properties.








## glTF Schema Updates

### JSON Schema Root

[glTF.GRIFFEL_bim_data.schema.json](schema/glTF.GRIFFEL_bim_data.schema.json)

### JSON Schema Node

[node.GRIFFEL_bim_data.schema.json](schema/node.GRIFFEL_bim_data.schema.json)

## Known Implementations

* [glTF Exporter](https://apps.autodesk.com/RVT/en/Detail/Index?id=8451869436709222290) - add-in for Autodesk Revit

## Resources

* [MessagePack](https://msgpack.org/) - binary serialization format
* [Griffel Studio Blog](https://griffelstudio.com/blog/) - information about extension
