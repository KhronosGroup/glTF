# KHR\_animation2

## Contributors

* Norbert Nopper, UX3D [@UX3DGpuSoftware](https://twitter.com/UX3DGpuSoftware)  

## Status

Experimental

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This extension is based on the animation features of glTF 2.0. The structure of the schemas are the same, however the properties are changed and adapted.  
  
The main changes are, that the output value is always a scalar and that the target property is a [JSON Pointer](https://datatracker.ietf.org/doc/html/rfc6901).

Following is summarizing the advantages of this approach:

### Compatibility

Tools e.g. like [Blender](https://www.blender.org/) do follow the same approach:  
For each channel/property, one can have an individual animation curve.  
In this apprach, the cases are reduced, where a complete or partial sampling is required.  

### Minimum data set

For e.g. a translation, all three values have to be stored. If the translation is only changed e.g. for the x value, the y and z values always have to be stored as well.  
In this approach, only the changing value has to be stored.

### Easier implementation

In this approach, always one key and one output scalar is used. It is independent, if the target is a `vec3`, `vec4` or an `array`.  This simplifies the data storage and reading, as only either one or three values (for cubic splines) are stored per frame.  

### Flexibility

At this point of time, one can only target the transformation or weight data of a node.  
With this approach, one can technically target any value (`scalar`, `vec2`, `vec3`, `vec4` and `array`) in glTF e.g.

* Color factors in materials
* Camera field of view

It even works on extensions and because of this future proof:

* Light color factors
* Texture transformation values

In a first step, even using a JSON pointer, the targets and their expected behaviour should be clearly defined.  
