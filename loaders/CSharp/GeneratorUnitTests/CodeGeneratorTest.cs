using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GeneratorLib;
using NUnit.Framework;

namespace GeneratorUnitTests
{
    [TestFixture]
    public class CodeGeneratorTest
    {
        private const string RelativePathToSchemaDir = @"..\..\..\..\..\specification\schema\";

        [Test]
        public void ParseSchemas_DirectReferences()
        {
            var generator = new CodeGenerator(RelativePathToSchemaDir + "glTFProperty.schema.json");
            generator.ParseSchemas();

            Assert.AreEqual(3, generator.FileSchemas.Keys.Count);
        }

        [Test]
        public void ParseSchemas_IndirectReferences()
        {
            var generator = new CodeGenerator(RelativePathToSchemaDir + "glTF.schema.json");
            generator.ParseSchemas();

            Assert.AreEqual(39, generator.FileSchemas.Keys.Count);
        }
        
        [Test]
        public void ExpandSchemaReferences_DirectReferences()
        {
            var generator = new CodeGenerator(RelativePathToSchemaDir + "glTFProperty.schema.json");
            generator.ParseSchemas();

            Assert.AreEqual(3, generator.FileSchemas.Keys.Count);

            generator.ExpandSchemaReferences();

            Assert.IsNull(generator.FileSchemas["glTFProperty.schema.json"].Properties["extensions"].ReferenceType);
            Assert.IsNull(generator.FileSchemas["glTFProperty.schema.json"].Properties["extras"].ReferenceType);
        }

        [Test]
        public void CSharpGenTest()
        {
            var generator = new CodeGenerator(RelativePathToSchemaDir + "glTF.schema.json");
            generator.ParseSchemas();
            generator.ExpandSchemaReferences();
            generator.EvaluateInheritance();
            generator.CSharpCodeGen();
        }
    }
}
