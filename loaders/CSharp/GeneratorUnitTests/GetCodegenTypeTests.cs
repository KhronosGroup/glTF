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
            Schema SchemaHasReferenceType = new Schema();
            SchemaHasReferenceType.ReferenceType = "Schema.json";
            Assert.Throws<InvalidOperationException>(() => CodegenTypeFactory.MakeCodegenType("name", SchemaHasReferenceType));
        }

        [Test]
        public void SchemaIsNotValidDictionaryException()
        {
            Schema Schema = new Schema
            {
                DictionaryValueType = new Schema(),
                Type = null
            };
            Assert.Throws<InvalidOperationException>(() => CodegenTypeFactory.MakeCodegenType("NoTypeDictionary", Schema));
            var typeRef = new TypeReference();
            Schema.Type = new[] { typeRef };
            Assert.Throws<InvalidOperationException>(() => CodegenTypeFactory.MakeCodegenType("NoTypeDictionary", Schema));
            typeRef.Name = "string";
            Assert.Throws<InvalidOperationException>(() => CodegenTypeFactory.MakeCodegenType("NoTypeDictionary", Schema));
        }

        [Test]
        public void DictionaryValueTypeMultiTypeTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            Schema.Type = new[] { typeRef };
            Schema.DictionaryValueType = new Schema();
            Schema.DictionaryValueType.Type = new[] { new TypeReference(), new TypeReference(), new TypeReference() };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "MultiDictionaryValueTypeType", out attributes,
                out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(object).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void DictionaryValueTypeHasDefaultException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            Schema.Type = new[] { typeRef };
            Schema.DictionaryValueType = new Schema();
            Schema.DictionaryValueType.Type = new[] { typeRef };
            Schema.Default = JObject.Parse(@"{""default"":""defalut""}");
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "HasDefaultDictionaryValue", out attributes, out defaultValue));
        }

        [Test]
        public void DictionaryValueTypeIsObjectTitleIsNull()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            Schema.Type = new[] { typeRef };
            Schema.DictionaryValueType = new Schema();
            Schema.DictionaryValueType.Type = new[] { typeRef };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "DictionaryValueTypeIsObject", out attributes, out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(object).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void DictionaryValueTypeIsObjectIsAnotherClass()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            Schema.Type = new[] { typeRef };
            Schema.DictionaryValueType = new Schema();
            Schema.DictionaryValueType.Title = "Asset";
            Schema.DictionaryValueType.Type = new[] { typeRef };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "DictionaryValueTypeIsObject", out attributes, out defaultValue);
            Assert.AreEqual("System.Collections.Generic.Dictionary<string, Asset>", result.BaseType);
        }

        [Test]
        public void DictionaryOfStringsTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef = new TypeReference();
            typeRef.Name = "object";
            Schema.Type = new[] { typeRef };
            Schema.DictionaryValueType = new Schema();
            var valueTypeRef = new TypeReference();
            Schema.DictionaryValueType.Type = new[] { valueTypeRef };
            valueTypeRef.Name = "string";
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "DictionaryValueTypeIsString", out attributes, out defaultValue);
            Assert.IsTrue(result.BaseType.Contains("Dictionary"));
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[0].BaseType);
            Assert.AreEqual(typeof(string).ToString(), result.TypeArguments[1].BaseType);
        }

        [Test]
        public void SchemaHasNoTypeException()
        {
            Schema SchemaNoType = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            SchemaNoType.DictionaryValueType = null;
            SchemaNoType.Type = null;
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), SchemaNoType, "NoType", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectMultiTypeReturnObject()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "object";
            var typeRef2 = new TypeReference();
            typeRef2.Name = "string";
            Schema.Type = new TypeReference[] { typeRef1, typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectMultiTypeReturnObject", out attributes, out defaultValue);
            Assert.AreEqual(typeof(object).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectAnyTypeReturnObject()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "any";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectAnyTypeReturnObject", out attributes, out defaultValue);
            Assert.AreEqual(typeof(object).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectIsReferenceTypeException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.IsReference = true;
            Schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "IsReferenceType", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectCustomerTypeTest()
        {
            Schema Schema = new Schema();
            Schema.Title = "Animation";
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "object";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectCustomerType", out attributes, out defaultValue);
            Assert.AreEqual("Animation", result.BaseType);
        }

        [Test]
        public void SingleObjectCustomerTypeNoTitleException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "object";
            Schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectCustomerTypeNoTitle", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectFloatTypeNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "number";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectFloatType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(float).ToString(), result.TypeArguments[0].BaseType);
        }

        [Test]
        public void SingleObjectFloatTypeHasDefaultTest()
        {
            Schema Schema = new Schema();
            Schema.Default = 0.1;
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "number";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectFloatType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(float).ToString(), result.BaseType);
            Assert.AreEqual((float)(double)Schema.Default, (float)((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void SingleObjectStringTypeNoDefaultNoEnumTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(string).ToString(), result.BaseType);
        }

        [Test]
        public void SingleObjectStringTypeHasDefaultNoEnumTest()
        {
            Schema Schema = new Schema();
            Schema.Default = "Empty";
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(string).ToString(), result.BaseType);
            Assert.AreEqual(Schema.Default, ((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void SingleObjectStringTypeNoDefaultHasEnumTest()
        {
            Schema Schema = new Schema();
            var expectedResult = new string[] { "One", "Two", "Three" };
            Schema.Enum = new JArray(expectedResult);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            CodeTypeDeclaration target = new CodeTypeDeclaration();
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(target, Schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual("SingleObjectStringTypeEnum", result.BaseType);
            var members = new CodeTypeMember[3];
            ((CodeTypeDeclaration)target.Members[0]).Members.CopyTo(members, 0);
            CollectionAssert.AreEquivalent(expectedResult, members.Select((m) => m.Name));
        }

        [Test]
        public void SingleObjectStringTypeHasDefaultHasEnumTest()
        {
            Schema Schema = new Schema();
            Schema.Default = "One";
            var expectedResult = new string[] { "One", "Two", "Three" };
            Schema.Enum = new JArray(expectedResult);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            CodeTypeDeclaration target = new CodeTypeDeclaration();
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(target, Schema, "SingleObjectStringType", out attributes, out defaultValue);
            Assert.AreEqual("SingleObjectStringTypeEnum", result.BaseType);
            var members = new CodeTypeMember[3];
            ((CodeTypeDeclaration)target.Members[0]).Members.CopyTo(members, 0);
            CollectionAssert.AreEquivalent(expectedResult, members.Select((m) => m.Name));
            Assert.AreEqual(Schema.Default, ((CodeFieldReferenceExpression)defaultValue).FieldName);
        }

        [Test]
        public void SingleObjectStringTypeInvalidDefaultHasEnumException()
        {
            Schema Schema = new Schema();
            Schema.Default = "Four";
            var expectedResult = new string[] { "One", "Two", "Three" };
            Schema.Enum = new JArray(expectedResult);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            CodeTypeDeclaration target = new CodeTypeDeclaration();
            var typeRef1 = new TypeReference();
            typeRef1.Name = "string";
            Schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<InvalidDataException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "InvalidDefaultValue", out attributes, out defaultValue));
        }

        [Test]
        public void SingleObjectIntegerTypeNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "integer";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectIntegerType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(int).ToString(), result.TypeArguments[0].BaseType);
        }

        [Test]
        public void SingleObjectIntegerTypeHasDefaultTest()
        {
            Schema Schema = new Schema();
            Schema.Default = 1L;
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "integer";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectIntegerType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(int).ToString(), result.BaseType);
            Assert.AreEqual((int)(long)Schema.Default, ((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void SingleObjectBoolTypeNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "boolean";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectBoolType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(bool).ToString(), result.TypeArguments[0].BaseType);
        }

        [Test]
        public void SingleObjectBoolTypeHasDefaultTest()
        {
            Schema Schema = new Schema();
            Schema.Default = false;
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "boolean";
            Schema.Type = new TypeReference[] { typeRef1 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "SingleObjectBoolType", out attributes, out defaultValue);
            Assert.AreEqual(typeof(bool).ToString(), result.BaseType);
            Assert.AreEqual(Schema.Default, ((CodePrimitiveExpression)defaultValue).Value);
        }

        [Test]
        public void ArrayItemIsNullException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "ArrayItemIsNullException", out attributes, out defaultValue));
        }

        [Test]
        public void ArrayItemTypeIsNullException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            Assert.Throws<InvalidOperationException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "ArrayItemTypeIsNullException", out attributes, out defaultValue));
        }

        [Test]
        public void ArrayItemsIsAnArrayTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            Schema.Items.Type = new TypeReference[] { typeRef2, typeRef2, typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "ArrayItemIsArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(object).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsBooleanNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "boolean";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "BooleanArray", out attributes, out defaultValue);
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
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "string";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "StringArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(string).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsStringHasDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            Schema.Default = new JArray(new string[] { "One", "Two", "Three" });
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "string";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "StringArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(string).ToString(), result.ArrayElementType.BaseType);
            Assert.AreEqual("One", ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[0])).Value);
            Assert.AreEqual("Two", ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[1])).Value);
            Assert.AreEqual("Three", ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[2])).Value);
        }

        [Test]
        public void ArrayItemsIsIntegerNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "IntegerArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(int).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsIntegerHasDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            Schema.Default = new JArray(new int[] {1 ,3 ,5});
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "IntegerArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(int).ToString(), result.ArrayElementType.BaseType);
            Assert.AreEqual(1, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[0])).Value);
            Assert.AreEqual(3, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[1])).Value);
            Assert.AreEqual(5, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[2])).Value);
        }

        [Test]
        public void ArrayItemsIsFloatNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "number";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "FloatArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(float).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsFloatHasDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            Schema.Default = new JArray(new float[] { 1.1f, 3.3f, 5.5f });
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "number";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "FloatArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(float).ToString(), result.ArrayElementType.BaseType);
            Assert.AreEqual(1.1f, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[0])).Value);
            Assert.AreEqual(3.3f, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[1])).Value);
            Assert.AreEqual(5.5f, ((CodePrimitiveExpression)(((CodeArrayCreateExpression)defaultValue).Initializers[2])).Value);
        }

        [Test]
        public void ArrayItemsIsObjectNoDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "object";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            var result = CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "ObjectArray", out attributes, out defaultValue);
            Assert.NotNull(result.ArrayElementType);
            Assert.AreEqual(typeof(object).ToString(), result.ArrayElementType.BaseType);
        }

        [Test]
        public void ArrayItemsIsObjectHasDefaultTest()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            Schema.Default = new JArray(new object[] { "Hello", 3.3f, 5 });
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "object";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "ObjectArray", out attributes, out defaultValue));
        }

        [Test]
        public void ArrayItemsTypeNotImplementedTypeException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "array";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "random";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "NotImplementedArray", out attributes, out defaultValue));
        }

        [Test]
        public void UnhandledSchemaTypeException()
        {
            Schema Schema = new Schema();
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var typeRef1 = new TypeReference();
            typeRef1.Name = "random";
            Schema.Type = new TypeReference[] { typeRef1 };
            Schema.Items = new Schema();
            var typeRef2 = new TypeReference();
            typeRef2.Name = "integer";
            Schema.Items.Type = new TypeReference[] { typeRef2 };
            Assert.Throws<NotImplementedException>(() => CodeGenerator.GetCodegenType(new CodeTypeDeclaration(), Schema, "UnhandledSchemaType", out attributes, out defaultValue));
        }
    }
}
