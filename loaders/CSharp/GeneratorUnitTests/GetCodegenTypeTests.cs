using System;
using System.CodeDom;
using System.Collections.Generic;
using System.IO;
using System.Linq;
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
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectFloatType", out attributes, out defaultValue);
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
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectFloatType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(float).ToString(), result.BaseType);
            Assert.AreEqual((float)(double)schema.Default, (float)((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void SingleObjectStringTypeNoDefaultNoEnumTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(string).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectStringTypeHasDefaultNoEnumTest()
        {
            Schema schema = new Schema();
            schema.Default = "Empty";
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(string).ToString(), result.BaseType);
            Assert.AreEqual(schema.Default, ((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void SingleObjectStringTypeNoDefaultHasEnumTest()
        {
            Schema schema = new Schema();
            var expectedResult = new string[] { "One", "Two", "Three" };
            schema.Enum = new JArray(expectedResult);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            CodeTypeDeclaration target = new CodeTypeDeclaration();
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(target, schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual("SingleObjectStringTypeEnum", result.BaseType);
            var members = new CodeTypeMember[3];
            ((CodeTypeDeclaration)target.Members[0]).Members.CopyTo(members, 0);
            CollectionAssert.AreEquivalent(expectedResult, members.Select((m) => m.Name));
        }

        [Test]
        public void SingleObjectStringTypeHasDefaultHasEnumTest()
        {
            Schema schema = new Schema();
            schema.Default = "One";
            var expectedResult = new string[] { "One", "Two", "Three" };
            schema.Enum = new JArray(expectedResult);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            CodeTypeDeclaration target = new CodeTypeDeclaration();
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(target, schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual("SingleObjectStringTypeEnum", result.BaseType);
            var members = new CodeTypeMember[3];
            ((CodeTypeDeclaration)target.Members[0]).Members.CopyTo(members, 0);
            CollectionAssert.AreEquivalent(expectedResult, members.Select((m) => m.Name));
            Assert.AreEqual(schema.Default, ((CodeFieldReferenceExpression)defaultValue).FieldName);
        }

        [Test]
        public void SingleObjectStringTypeInvalidDefaultHasEnumException()
        {
            Schema schema = new Schema();
            schema.Default = "Four";
            var expectedResult = new string[] { "One", "Two", "Three" };
            schema.Enum = new JArray(expectedResult);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            CodeTypeDeclaration target = new CodeTypeDeclaration();
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<InvalidDataException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "InvalidDefaultValue", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectIntegerTypeNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "integer";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectIntegerType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(int).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectIntegerTypeHasDefaultTest()
        {
            Schema schema = new Schema();
            schema.Default = 1L;
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "integer";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectIntegerType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(int).ToString(), result.BaseType);
            Assert.AreEqual((int)(long)schema.Default, ((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void SingleObjectBoolTypeNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "boolean";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectBoolType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(bool).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectBoolTypeHasDefaultTest()
        {
            Schema schema = new Schema();
            schema.Default = false;
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "boolean";
            schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "SingleObjectBoolType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(bool).ToString(), result.BaseType);
            Assert.AreEqual(schema.Default, ((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void ArrayItemIsNullException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "ArrayItemIsNullException", out attributes, out defaultValue));
        }

        [Test]
        public void ArrayItemTypeIsNullException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "ArrayItemTypeIsNullException", out attributes, out defaultValue));
        }

        [Test]
        public void ArrayItemHasMinHasMaxTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            schema.MinItems = 1;
            schema.MaxItems = 16;
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "ArrayItemHasMinHasMax", out attributes, out defaultValue);
            Assert.AreEqual((int)schema.MinItems, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)(attributes[0].Arguments[1].Value)).Initializers[0])).Value);
            Assert.AreEqual((int)schema.MaxItems, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)(attributes[0].Arguments[1].Value)).Initializers[1])).Value);
        }

        [Test]
        public void ArrayItemsIsAnArrayTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            schema.Items.Type = new TypeReference[] { typeRef2, typeRef2, typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "ArrayItemIsArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(object).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsBooleanNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "boolean";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "BooleanArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(bool).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsBooleanHasDefaultTest()
        {
            var expectedValues = new[] { true, true, false };
            var schema = new Schema
            {
                Default = new JArray(expectedValues),
                Type = new [] { new TypeReference { Name = "array" } },
                Items = new Schema
                {
                    Type = new[] { new TypeReference { Name = "boolean" } }
                }
            };

            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;

            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "BooleanArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(bool).ToString(), result.ArrayElementType.BaseType);
            var resultValues = (((CodeArrayCreateExpression) defaultValue).Initializers.Cast<CodePrimitiveExpression>()).Select(a => (bool)(a.Value));
            CollectionAssert.AreEqual(expectedValues, resultValues);
        }

        [Test]
        public void ArrayItemsIsStringNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "string";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "StringArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(string).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsStringHasDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schema.Default = new JArray(new string[] { "One", "Two", "Three" });
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "string";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "StringArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(string).ToString(), result.ArrayElementType.BaseType);
            Assert.AreEqual("One", ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[0])).Value);
            Assert.AreEqual("Two", ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[1])).Value);
            Assert.AreEqual("Three", ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[2])).Value);
        }

        [Test]
        public void ArrayItemsIsIntegerNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "IntegerArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(int).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsIntegerHasDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schema.Default = new JArray(new int[] {1 ,3 ,5});
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "IntegerArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(int).ToString(), result.ArrayElementType.BaseType);
            Assert.AreEqual(1, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[0])).Value);
            Assert.AreEqual(3, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[1])).Value);
            Assert.AreEqual(5, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[2])).Value);
        }

        [Test]
        public void ArrayItemsIsFloatNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "number";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "FloatArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(float).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsFloatHasDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schema.Default = new JArray(new float[] { 1.1f, 3.3f, 5.5f });
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "number";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "FloatArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(float).ToString(), result.ArrayElementType.BaseType);
            Assert.AreEqual(1.1f, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[0])).Value);
            Assert.AreEqual(3.3f, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[1])).Value);
            Assert.AreEqual(5.5f, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[2])).Value);
        }

        [Test]
        public void ArrayItemsIsObjectNoDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "object";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "ObjectArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(object).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsObjectHasDefaultTest()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            schema.Default = new JArray(new object[] { "Hello", 3.3f, 5 });
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "object";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "ObjectArray", out attributes, out defaultValue));
        }

        [Test]
        public void ArrayItemsTypeNotImplementedTypeException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "random";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "NotImplementedArray", out attributes, out defaultValue));
        }

        [Test]
        public void UnhandledSchemaTypeException()
        {
            Schema schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "random";
            schema.Type = new TypeReference[] { typeRef1 };
            schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            schema.Items.Type = new TypeReference[] { typeRef2 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), schema, "UnhandledSchemaType", out attributes, out defaultValue));
        }
    }
}
