using System;
using System.CodeDom;
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
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schemaHasReferenceType, "name", out attributes, out defaultValue));
        }

        [Test]
        public void SchemaIsNotValidDictionaryException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schema.DictionaryValueType = new Schema();
            schema.Type = null;
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "NoTypeDictionary", out attributes, out defaultValue));
            var typeRef = new TypeReference();
            schema.Type = new[] { typeRef };
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "NoTypeDictionary", out attributes, out defaultValue));
            typeRef.Name = "string";
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "NoTypeDictionary", out attributes, out defaultValue));
        }

        [Test]
        public void DictionaryValueTypeMultiTypeTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            schema.Type = new[] { typeRef };
            schema.DictionaryValueType = new Schema();
            schema.DictionaryValueType.Type = new[] { new TypeReference(), new TypeReference(), new TypeReference() };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "MultiDictionaryValueTypeType", out attributes,
                out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(object).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void DictionaryValueTypeHasDefaultException()
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
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "HasDefaultDictionaryValue", out attributes, out defaultValue));
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
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "DictionaryValueTypeIsObject", out attributes, out defaultValue);
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
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "DictionaryValueTypeIsObject", out attributes, out defaultValue);
            Assert.AreEqual("System.Collections.Generic.Dictionary<string, Asset>", result.BaseType);
        }

        [Test]
        public void DictionaryOfStringsTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            schema.Type = new[] { typeRef };
            schema.DictionaryValueType = new Schema();
            var valueTypeRef = new TypeReference();
            schema.DictionaryValueType.Type = new[] { valueTypeRef };
            valueTypeRef.Name = "string";
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "DictionaryValueTypeIsString", out attributes, out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void SchemaHasNoTypeException()
        {
            Schema schemaNoType = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schemaNoType.DictionaryValueType = null;
            schemaNoType.Type = null;
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schemaNoType, "NoType", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectMultiTypeReturnObject()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "object";
            var typeRef2 = new TypeReference();
            typeRef2.Name = "string";
            schema.Type = new TypeReference[] { typeRef1, typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectMultiTypeReturnObject", out attributes, out defaultValue);
            Assert.AreEqual(typeof(object).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectAnyTypeReturnObject()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "any";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectAnyTypeReturnObject", out attributes, out defaultValue);
            Assert.AreEqual(typeof(object).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectIsReferenceTypeException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.IsReference = true;
            schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "IsReferenceType", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectCustomerTypeTest()
        {
            Schema schema = new Schema();
            schema.Title = "Animation";
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "object";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectCustomerType", out attributes, out defaultValue);
            Assert.AreEqual("Animation", result.BaseType);
        }

        [Test]
        public void SingleObjectCustomerTypeNoTitleException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "object";
            schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectCustomerTypeNoTitle", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectFloatTypeNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "number";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectCustomerType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(float).ToString(), result.BaseType);
        }
        [Test]
        public void SingleObjectFloatTypeHasDefaultTest()
        {
            Schema schema = new Schema();
            schema.Default = 0.1;
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "number";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectCustomerType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(float).ToString(), result.BaseType);
            Assert.AreEqual((float)(double)schema.Default, (float)((CodePrimitiveExpression)defaultValue).Value);
        }
    }
}
