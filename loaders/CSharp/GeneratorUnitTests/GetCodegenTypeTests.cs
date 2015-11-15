using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using GeneratorLib;
using NUnit.Framework;

namespace GeneratorUnitTests
{
    [TestFixture]
    public class GetCodegenTypeTests
    {
        [Test]
        public void SchemaReferenceTypeNotNullTest()
        {
            Schema schemaHasReferenceType = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schemaHasReferenceType.ReferenceType = "schema.json";
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schemaHasReferenceType, "name", out attributes, out defaultValue));
        }

        [Test]
        public void NoTypeTest()
        {
            Schema schemaNoType = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schemaNoType.DictionaryValueType = null;
            schemaNoType.Type = null;
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schemaNoType, "NoType", out attributes, out defaultValue));
        }


        [Test]
        public void SchemaIsNotValidDictionary()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schema.DictionaryValueType = new Schema();
            schema.Type = null;
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schema, "NoTypeDictionary", out attributes, out defaultValue));
            var typeRef = new TypeReference();
            schema.Type = new[] {typeRef};
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schema, "NoTypeDictionary", out attributes, out defaultValue));
            typeRef.Name = "string";
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schema, "NoTypeDictionary", out attributes, out defaultValue));
        }
    }
}
