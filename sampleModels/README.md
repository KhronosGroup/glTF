Sample models are provided in as many of the following formats as possible:
* glTF (.gltf) with separate resources: .bin (geometry, animation, skins), .glsl (shaders), and image files
* glTF (.gltf) with embedded resources
* Binary glTF (.glb) using the [KHR_binary_glTF](https://github.com/KhronosGroup/glTF/blob/master/extensions/Khronos/KHR_binary_glTF/README.md) extension
* glTF (.gltf) using the [KHR_materials_common](https://github.com/KhronosGroup/glTF/blob/master/extensions/Khronos/KHR_materials_common/README.md) extension
* Original COLLADA (.dae) or other source format

See the `README.txt` in each model's directory for usage restrictions.

Sample models help the glTF ecosystem, if you are able to contribute a model, see the [contributing section](#contributing-sample-models) below.

| Model                                         | Screenshot                                       | Description|
|-----------------------------------------------|--------------------------------------------------|------------|
| [box](box)                                    | ![](box/screenshot/screenshot.png)               | One mesh and one material. Start with this. |
| [boxWithoutIndices](boxWithoutIndices)        | ![](boxWithoutIndices/screenshot/screenshot.png) | Box without indices for testing the `drawArrays` path. |
| [boxTextured](boxTextured)                    | ![](boxTextured/screenshot/screenshot.png)       | Box with one texture. Start with this to test textures. |
| [boxSemantics](boxSemantics)                  | ![](boxSemantics/screenshot/screenshot.png)      | Includes extra uniforms to test all uniform semantics. |
| [duck](duck)                                  | ![](duck/screenshot/screenshot.png)              | The COLLADA duck. One texture. |
| [2_cylinder_engine engine](2_cylinder_engine) | ![](2_cylinder_engine/screenshot/screenshot.png) | Small CAD data set, including hierarchy. |
| [Reciprocating_Saw saw](Reciprocating_Saw)    | ![](Reciprocating_Saw/screenshot/screenshot.png) | Small CAD data set, including hierarchy. |
| [gearbox](gearbox_assy)                       | ![](gearbox_assy/screenshot/screenshot.png)      | Medium-sized CAD data set, including hierarchy. |
| [buggy](buggy)                                | ![](buggy/screenshot/screenshot.png)             | Medium-sized CAD data set, including hierarchy |
| [boxAnimated](boxAnimated)                    | ![](boxAnimated/screenshot/screenshot.png)       | Rotation and Translation Animations. Start with this to test animations. |
| [CesiumMilkTruck](CesiumMilkTruck)            | ![](CesiumMilkTruck/screenshot/screenshot.png)   | Textured. Multiple nodes/meshes. Animations. |
| [RiggedSimple](RiggedSimple)                  | ![](RiggedSimple/screenshot/screenshot.png)      | Animations. Skins. Start with this to test skinning. |
| [RiggedFigure](RiggedFigure)                  | ![](RiggedFigure/screenshot/screenshot.png)      | Animations. Skins. |
| [CesiumMan](CesiumMan)                        | ![](CesiumMan/screenshot/screenshot.png)         | Textured. Animations. Skins. |
| [monster](monster)                            | ![](monster/screenshot/screenshot.png)           | Textured. Animations. Skins. |
| [brainsteam](brainsteam)                      | ![](brainsteam/screenshot/screenshot.png)        | Animations. Skins. |

## Contributing Sample Models

We appreciate sample model contributions; they help ensure a consistent glTF ecosystem.

To contribute a model, open a pull request with:
* A new subdirectory containing
   * The model in as many glTF variations as possible (using the same directory structure as the others ([example](example))).  Tools for converting to glTF are [here](https://github.com/KhronosGroup/glTF#converters).
   * A README.txt with any usage restrictions.
* Add the new model to the above table.

If you have any questions, submit an [issue](https://github.com/KhronosGroup/glTF/issues).
