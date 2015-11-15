using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
            var filePath = Path.GetFullPath(Path.Combine(RelativePathToSamplesDir, @"boxTextured\glTF\CesiumTexturedBoxTest.gltf"));
            string contents = File.ReadAllText(filePath);
            Assert.IsNotNull(contents);
            var result = JsonConvert.DeserializeObject<Gltf>(contents);
            Assert.IsNotNull(result);
            foreach (var dir in Directory.EnumerateDirectories(Path.GetFullPath(RelativePathToSamplesDir)))
            {
                foreach (var file in Directory.EnumerateFiles(Path.Combine(dir, "glTF")))
                {
                    if (file.EndsWith("gltf"))
                    {
                        try
                        {
                            var deserializedFile = JsonConvert.DeserializeObject<Gltf>(File.ReadAllText(file));
                            Assert.IsNotNull(deserializedFile);
                        }
                        catch (Exception e)
                        {
                            throw new Exception(file,e);
                        }
                        
                    }
                }
            }
        }
    }
}
