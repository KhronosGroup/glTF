# Lighting

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC DIS 23090-14 2nd Edition](https://www.iso.org/standard/80900.html)

## Dependencies

Written against the glTF 2.0 spec.

## Overview

This document defines types of light sources with their corresponding extensions:
* The image-based lights via the MPEG_lights_texture_based extension
* The punctual lights via the MPEG_lights_punctual extension
The MPEG_lights_texture_based extension defines image-based lights in a glTF scene. This extension expands the functionalities of the EXT_lights_image_based extension by enabling the lighting information to vary over time. The lighting information in the MPEG_lights_texture_based extension comprises two timed dependent sequences that are:
* a video sequence wherein each frame represents the specular radiance information
* a timed metadata sequence where each sample of the sequence represents irradiance information
At each time instance of the glTF scene rendering, the corresponding specular radiance decoded frame and the irradiance sample provide the necessary information to apply image-based lighting rendering techniques.
Regarding the punctual lights extension, identified by MPEG_lights_punctual, it provides the ability to differentiate between physical and virtual light sources present as KHR_lights_punctual elements as well as providing accessor to retrieve timed dependent information it.
## Semantics

### Semantics of the MPEG_lights_texture_based extension

When present, the MPEG_lights_texture_based extension shall be included as a glTF file level extension and as a scene level extension.
The definition of all objects within MPEG_lights_texture_based extension is provided in Table 1. 

Table 1: Definitions of glTF file level objects of MPEG_lights_texture_based extension
| Name | Type | Usage | Default | Description |
|--|--|--|--|--|
| lights | array | M |  | An array of items that describe the texture-based light sources, referenced in this scene description document. |


Table 2: Definitions of item in the lights array of MPEG_lights_texture_based extension
| Name | Type | Usage | Default | Description |
|--|--|--|--|--|
| name | string | O | N/A | Name of the light |
| nature | enumeration | O | PHYSICAL | Indicates whether the lighting information corresponds to </br>PHYSICAL = 0: a physical light,</br>VIRTUAL = 1: a virtual light, </br>PHYSICAL_VIRTUAL = 2:both physical and virtual,</br>UNSPECIFIED = 3: unknown. In case of a physical light source, an AR anchor that refers to this light source should be present. |
| position | array | O | (0,0,0) | Position of both light and reflection probes associated with the lighting information.</br>In the absence of this position, the lighting information is assumed to be global (i.e. for the entire scene) for which the center is the scene origin.</br>When an anchor refers to a light source, the position of the anchor takes precedence over this position attribute. |
| projection | enumeration | O | EQUIRECTANGULAR | Provides the projection typeof the specular images. The type may be either</br>EQUIRECTANGULAR = 0,</br>CUBEMAP = 1</br>In the latter case, the 6 faces of the radiance map should be packed according to the order described by Table 10 of the ISO/IEC 23090-7 specification. |
| rotationAccessor | integer | O | N/A | Provides a reference to the accessor giving a sequence of quaternions as described in rotation of in EXT_lights_image_based.</br>When an anchor refers to a light source, the rotation of the anchor takes precedence over this rotation attribute.</br>In the absence this rotation_accessor, there is no rotation to apply to the irradiance map and specular images. |
| intensityAccessor | integer | O | N/A | Provides a reference to the accessor giving a sequence of intensity values as described in intensity of in EXT_lights_image_based.</br>In the absence this intensity_accessor, the intensity of the light is 1.0. |
| irradianceAccessor | integer | M | N/A | Provides a reference to the accessor giving a sequence of irradiance coefficient samples. Irradiance coefficients are defined as in irradianceCoefficients of  EXT_lights_image_based.</br>An irradiance coefficients sample is an array of 27 values of floating-point type corresponding to 9 vectors of 3 components, one per RGB channels. The coefficients are stored in the RGB order and from low to high order channel-major fashion i.e. [r0, g0, b0,r1, g1, b1, ... ].</br>See Khronos EXT_lights_image_based for the definition of the irradiance coefficient sample. |
| specularImages | array | M | N/A | Provides a list of references to textures providing specular images (environment map).</br>The length of the array shall be equal to “1” or “6”.</br>When the length of the array is equal to “1”, the specular images are provided in a packed format as defined by the projection attribute.</br>When the length of the array is equal to “6”, the projection attribute shall be of type cubemap and the order of faces are TBD. |


Table 3: Definitions of objects at the scene level of MPEG_lights_texture_based
| Name | Type | Usage | Default | Description |
|--|--|--|--|--|
| light | array(integer) | M |  | Reference to an item in the lights array of the MPEG_lights_texture_based extension. |


### Semantics of the MPEG_lights_punctual extension

When present, the MPEG_lights_punctual extension shall be included as a glTF file level extension.
The definition of all objects within MPEG_lights_punctual extension is provided in Table 4.

Table 4: Definitions of glTF file level objects of MPEG_lights_punctual extension
| Name | Type | Usage | Default | Description |
|--|--|--|--|--|
| lights | array | M |  | An array of items that describe the punctual light sources, referenced in this scene description document. |

Table 5: Definitions of item in the lights array of MPEG_lights_punctual extension
| Name | Type | Usage | Default | Description |
|--|--|--|--|--|
| light | integer | M |  | Index of the item in the array of the lights parameter of the KHR_lights_punctual extension |
| nature | enumeration | M |  | Indicates whether the lighting information corresponds to </br>PHYSICAL = 0: a physical light,</br>VIRTUAL = 1: a virtual light, </br>PHYSICAL_VIRTUAL = 2:both physical and virtual,</br>UNSPECIFIED = 3: unknown. </br>In case of a physical light source, an AR anchor that refers to this light source should be present |
| color_accessor | integer | O | N/A | Provides a reference to the accessor giving a sequence of color value as described in color of in KHR_lights_punctual.</br>In the absence of the accessor, the value in the referenced light is considered as the constant value for the rendering duration. |
| intensity_accessor | integer | O | N/A | Provides a reference to the accessor giving a sequence of intensity value as described in intensity of KHR_lights_punctual. </br>In the absence of the accessor, the value in the referenced light is considered as the constant value for the rendering duration. |
| range_accessor | integer | O | N/A | Provides a reference to the accessor giving a sequence of range value as described in range of in KHR_lights_punctual.</br>In the absence of the accessor, the value in the referenced light is considered as the constant value for the rendering duration. |


## Processing model 

When a scene description document contains light source defined in the MPEG_lights_texture_based extension or the MPEG_lights_punctual extension, the renderer should take into consideration this lighting information when rendering the view of the scene. The operations to be applied depends on the context of the application, e.g. if it is a virtual reality (VR) or an AR application. In addition, the renderer should also consider whether the lights are virtual or physical based on the value of the nature field.
For instance, when the scene contains both physical and virtual lighting information, such as in AR applications, then the renderer should relight the virtual elements of the scene (e.g. virtual objects) present in the scene with both physical and virtual lighting information. In some cases, both the physical and the virtual lighting information can be combined into one light source signalled as being both physical and virtual. This can simplify the relighting of virtual elements but cannot be used to relight physical elements. Regarding the real elements of the scene, the renderer should only relight them using the virtual lighting information since, by definition, the real elements of the scene are already illuminated by the physical light sources in the scene.
Regardless of the application type, when the lighting information is parsed, the Presentation Engine places the image-based light in the scene based on the rotation, intensity and irradiance values, as well as setting the specular images in the lighting model.
Both extensions MPEG_lights_texture_based and MPEG_lights_punctual allow to signal UNSPECIFIED for the nature of the lighting information. In this case, the expected rendering is determined by the application.
The Presentation Engine places the punctual lights using the same processing model as the KHR_lights_punctual elements and retrieves timed dependent information through the color, intensity and range accessors.
Depending on the view pose, the render should illuminate each object of the scene by the lights surrounding the objects, possibly by interpolating the lighting contribution based on the relative distance.

## Schema

* **JSON schema**: [MPEG_lights_texture_based.schema.json](./schema/MPEG_lights_texture_based.schema.json)
* **JSON schema**: [MPEG_lights_punctual.schema.json](./schema/MPEG_lights_punctual.schema.json)

## Known Implementations

* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html)

## Resources

* [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [ISO/IEC WD 23090-24](https://www.iso.org/standard/83696.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2022

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.

