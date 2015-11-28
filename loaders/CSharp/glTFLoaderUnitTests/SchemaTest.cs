using System;
using System.IO;
using System.Runtime.Remoting.Messaging;
using glTFLoader;
using glTFLoader.Schema;
using Newtonsoft.Json;
using NUnit.Framework;

namespace glTFLoaderUnitTests
{
    [TestFixture]
    public class SchemaTest
    {
        private const string RelativePathToSamplesDir = @"..\..\..\..\..\sampleModels\";

        [Test]
        public void SchemaLoad()
        {
            foreach (var dir in Directory.EnumerateDirectories(Path.GetFullPath(RelativePathToSamplesDir)))
            {
                foreach (var file in Directory.EnumerateFiles(Path.Combine(dir, "glTF")))
                {
                    if (file.EndsWith("gltf"))
                    {
                        try
                        {
                            CallContext.LogicalSetData("UriRootPath", Path.GetFullPath(Path.GetDirectoryName(file)));
                            var deserializedFile = JsonConvert.DeserializeObject<Gltf>(File.ReadAllText(file));
                            Assert.IsNotNull(deserializedFile);
                        }
                        catch (Exception e)
                        {
                            throw new Exception(file, e);
                        }
                    }
                }
            }
        }

        [Test]
        public void SerializeTest()
        {
            foreach (var dir in Directory.EnumerateDirectories(Path.GetFullPath(RelativePathToSamplesDir)))
            {
                foreach (var file in Directory.EnumerateFiles(Path.Combine(dir, "glTF")))
                {
                    if (file.EndsWith("gltf"))
                    {
                        try
                        {
                            var deserializedFile = Interface.LoadModel(Path.GetFullPath(file));
                            Assert.IsNotNull(deserializedFile);
                            var serializedFile = glTFLoader.Interface.SerializeModel(deserializedFile);
                            Assert.IsNotNull(serializedFile);
                            Interface.SaveModel(deserializedFile, (@".\"+Path.GetFileName(file)));
                            deserializedFile = Interface.LoadModel(@".\" + Path.GetFileName(file));
                            Assert.IsNotNull(deserializedFile);

                        }
                        catch (Exception e)
                        {
                            throw new Exception(file, e);
                        }
                    }
                }
            }
        }

        [Test]
        public void EmbeddedSchemaLoad()
        {
            foreach (var dir in Directory.EnumerateDirectories(Path.GetFullPath(RelativePathToSamplesDir)))
            {
                foreach (var file in Directory.EnumerateFiles(Path.Combine(dir, "glTF-Embedded")))
                {
                    if (file.EndsWith("gltf"))
                    {
                        try
                        {
                            CallContext.LogicalSetData("UriRootPath", Path.GetFullPath(Path.GetDirectoryName(file)));
                            var deserializedFile = JsonConvert.DeserializeObject<Gltf>(File.ReadAllText(file));
                            Assert.IsNotNull(deserializedFile);
                        }
                        catch (Exception e)
                        {
                            throw new Exception(file, e);
                        }
                    }
                }
            }
        }
    }
}
