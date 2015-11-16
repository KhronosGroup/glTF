using System;
using System.CodeDom;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using GeneratorLib;
using Newtonsoft.Json.Linq;
using NUnit.Framework;

namespace GeneratorUnitTests
{
    [TestFixture]
    public class GetCodegenTypeTests
    {
        [Test]
        public void SchemaReferenceTypeNotNullException()
        {
            Schema schemaHasReferenceType = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schemaHasReferenceType.ReferenceType = "schema.json";
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schemaHasReferenceType, "name", out attributes, out defaultValue));
        }

        [Test]
        public void SchemaHasNoTypeException()
        {
            Schema schemaNoType = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schemaNoType.DictionaryValueType = null;
            schemaNoType.Type = null;
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(schemaNoType, "NoType", out attributes, out defaultValue));
        }


        [Test]
        public void SchemaIsNotValidDictionaryException()
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

        [Test]
        public void MultiDictionaryValueTypeTypeTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            schema.Type = new[] { typeRef };
            schema.DictionaryValueType = new Schema();
            schema.DictionaryValueType.Type = new[] {new TypeReference(), new TypeReference(), new TypeReference()};
            var result = CodeGenerator.GetCodegenType(schema, "MultiDictionaryValueTypeType", out attributes,
                out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(object).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void DefaultDictionaryValueException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            schema.Type = new[] { typeRef };
            schema.DictionaryValueType = new Schema();
            schema.DictionaryValueType.Type = new[] { typeRef };
            schema.Default = JObject.Parse(@"{""default"":""defalut""}");
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(schema, "HasDefaultDictionaryValue", out attributes, out defaultValue));
        }

        [Test]
        public void DictionaryValueTypeIsObjectTitleIsNull()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            schema.Type = new[] { typeRef };
            schema.DictionaryValueType = new Schema();
            schema.DictionaryValueType.Type = new[] { typeRef };
            var result = CodeGenerator.GetCodegenType(schema, "DictionaryValueTypeIsObject", out attributes, out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(object).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void DictionaryValueTypeIsObjectIsAnotherClass()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            schema.Type = new[] { typeRef };
            schema.DictionaryValueType = new Schema();
            schema.DictionaryValueType.Title = "Asset";
            schema.DictionaryValueType.Type = new[] { typeRef };
            var result = CodeGenerator.GetCodegenType(schema, "DictionaryValueTypeIsObject", out attributes, out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(object).ToString(), result.TypeArguments[1].BaseType);
        }
    }
}
