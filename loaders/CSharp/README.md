This is a C# reference loader for glTF.  It's as simple to use as Interface.LoadModel("PathToModel.gltf").  You can use this loader in your project by importing the "glTF Loader" NuGet package.  Additional examples can be found in the gltfLoaderUnitTests project.

Build Instructions
-------------

To build the project, load the CSharp.sln solution.  Click "Start". This will build glTFLoader_Shared, GeneratorLib, and Generator. It will then run the Generator. The generator reads the .spec files defining the glTF standard and generates a set of classes that will be used as the schema for loading your models. The glTFLoader project can now be built.  You will need glTFLoader.dll and glTFLoader_shared.dll in order to use the loader in any subsequent project.
