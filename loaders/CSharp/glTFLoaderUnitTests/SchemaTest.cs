using System;
using System.IO;
using System.Runtime.Remoting.Messaging;
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
