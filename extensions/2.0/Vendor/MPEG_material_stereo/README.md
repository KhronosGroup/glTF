# MPEG_material_stereo

## Contributors

* ISO/IEC SC29 WG3 (MPEG Systems) - Scene Description Breakout Group
* Contacts
  * Thomas Stockhammer (MPEG-I Scene Description BoG Chair, tsto@qti.qualcomm.com)

## Status

Based on [ISO/IEC FDIS 23090-14](https://www.iso.org/standard/80900.html)
Draft proposal — for consideration in [**ISO/IEC 23090-14 (Scene Description) Edition 2 Amendment 1**](https://www.iso.org/standard/90213.html).

## Dependencies

Written against the glTF 2.0 spec.

## Overview

The `MPEG_material_stereo` extension is defined to support **stereoscopic and immersive applications** in MPEG Scene Description.  
It enables the representation of materials composed of **two texture layers** (left and right eye) that together produce a stereoscopic rendering effect.  
This extension is applied to the [`material`](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material) object in glTF.

Each layer refers to a glTF texture and identifies which eye (left or right) it is associated with, while one layer is designated as the **primary (hero) view**.

## Schema

[JSON Schema (draft-07)](./MPEG_material_stereo.schema.json)

### Properties

| Property name      | Type          | Required | Description |
|--------------------|---------------|-----------|--------------|
| `layers`           | array of [Layer](#layer-definition) | Yes | Defines the stereo layers (maximum two: left and right). |
| `extensions`       | object        | No | Extension-specific objects (as defined by glTFProperty). |
| `extras`           | any           | No | Application-specific data (as defined by glTFProperty). |

### Layer Definition

Each element of the `layers` array is an object with the following properties:

| Property name       | Type      | Required | Description |
|---------------------|-----------|-----------|--------------|
| `primaryReference`  | boolean   | Yes | Indicates whether this layer is the **primary (hero)** component. The layers array must contain **exactly one layer** where primaryReference is true. |
| `texture`           | integer   | Yes | Index of the texture within the glTF `textures` array. |
| `eyeChannel`        | integer (0 or 1) | Yes | Indicates whether the texture corresponds to the **left eye (0)** or **right eye (1)**. |

## Processing Mode

Each mesh with a reference to a material with a MPEG_material_stereo extension, references a material that aggregates stereo layers. Within the extension, exactly one layer shall be identified as the primary reference, while the additional layer may be associated with the left or right eye. A conforming implementation should combine these layers according to their designated eye channel, ensuring that the primary reference is consistently used as the basis of rendering. In case when stereo rendering is not supported by the Presentation Engine (PE), one texture which is identified as the primary reference layer in the MPEG_material_stereo extension is used. When the MPEG_material_stereo extension is not supported, the standard material glTF element can be used as fallback.

When a scene description file becomes available, the PE shall proceed as follows:

1. **Parse** the related glTF file.  
2. **Extract** the `scenes`, `nodes`, and `meshes` arrays.  
3. For each mesh referencing a material with a `MPEG_material_stereo` extension, **process** the extension data to aggregate stereo layers information. For each layer, extract:
   1. **Primary reference flag (`primaryReference`)** – indicates whether this layer is designated as the primary reference.  
   2. **Texture index (`texture`)** – refers to the index of the texture within the glTF `textures` array.  
   3. **Eye channel (`eyeChannel`)** – indicates the association of the layer with the left or right eye.  

4. According to the device stereo capability:
   - **If stereo rendering is possible**:
     - For each stereo layer identified in Step 3, load the texture referenced by its texture index from the glTF textures array.
   - **For non-stereo devices**:
     - Load the texture identified as the primary reference by its texture index from the glTF textures array.

5. Once all texture(s) are loaded, the PE **applies the material** to the mesh primitive according to its rendering capabilities:
   - **If stereo rendering is possible**:
     - Rendering is done for each eye.  
     - If a stereo layer is missing for one of the eyes, rendering for that eye falls back to using only the primary reference layer.  
   - **For non-stereo devices**:
     - Rendering is done using the primary reference layer.


## Example

```json
{
  "materials": [
    {
      "name": "StereoMaterial",
      "extensions": {
        "MPEG_material_stereo": {
          "layers": [
            {
              "primaryReference": true,
              "texture": 0,
              "eyeChannel": 0
            },
            {
              "primaryReference": false,
              "texture": 1,
              "eyeChannel": 1
            }
          ]
        }
      }
    }
  ]
}
```

## Known Implementations

* [**ISO/IEC 23090-24:2025/DAmd 1**](https://www.iso.org/standard/87584.html)

## Resources

* [**ISO/IEC 23090-14/DAmd 1**](https://www.iso.org/standard/90213.html), Information technology — Coded representation of immersive media — Part 14: Scene Description 
* [**ISO/IEC 23090-24:2025/DAmd 1**](https://www.iso.org/standard/87584.html), Information technology — Coded representation of immersive media — Part 24: Conformance and Reference Software for Scene Description for MPEG Media

## License

Copyright ISO/IEC 2025

The use of the "MPEG scene description extensions" is subject to the license as accessible here: https://standards.iso.org/ and is subject to the IPR policy as accessible here: https://www.iso.org/iso-standards-and-patents.html.
