using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.ComponentModel.Design.Serialization;
using Microsoft.CSharp;
using System.IO;
using System.Linq.Expressions;
using System.Reflection;
using glTFLoader.Shared;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace GeneratorLib
{
    public partial class CodeGenerator
    {
        public Dictionary<string, CodeTypeDeclaration> GeneratedClasses { get; set; }

        public CodeCompileUnit RawClass(string fileName, out string className)
        {
            var root = FileSchemas[fileName];
            var schemaFile = new CodeCompileUnit();
            var schemaNamespace = new CodeNamespace("glTFLoader.Schema");
            className = ParseTitle(root.Title);
            var schemaClass = new CodeTypeDeclaration(className);
            schemaClass.Attributes = MemberAttributes.Public;
            if (root.Extends != null && root.Extends.IsReference)
            {
                schemaClass.BaseTypes.Add(ParseTitle(FileSchemas[root.Extends.Name].Title));
            }

            foreach (var property in root.Properties)
            {
                AddProperty(schemaClass, property.Key, property.Value);
            }

            GeneratedClasses[fileName] = schemaClass;
            schemaNamespace.Types.Add(schemaClass);
            //new CodeAttributeDeclaration(new CodeTypeReference(new CodeTypeParameter()))
            schemaFile.Namespaces.Add(schemaNamespace);
            return schemaFile;
        }

        private void AddProperty(CodeTypeDeclaration target, string rawName, Schema schema)
        {
            var name = rawName.Substring(0, 1).ToUpper() + rawName.Substring(1);
            var fieldName = "m_" + name.Substring(0, 1).ToLower() + name.Substring(1);
            CodeAttributeDeclarationCollection attributes;
            CodeExpression defaultValue;
            var type = GetCodegenType(schema, name, out attributes, out defaultValue);

            var propertyBackingVariable = new CodeMemberField
            {
                Type = type,
                Name = fieldName,
                Comments = { new CodeCommentStatement("<summary>", true), new CodeCommentStatement($"Backing field for {name}.", true), new CodeCommentStatement("</summary>", true) },
                InitExpression = defaultValue
            };

            target.Members.Add(propertyBackingVariable);
            var property = new CodeMemberProperty
            {
                Type = type,
                Name = name,
                Attributes = MemberAttributes.Public | MemberAttributes.Final,
                HasGet = true,
                GetStatements = { new CodeMethodReturnStatement(new CodeFieldReferenceExpression(new CodeThisReferenceExpression(), fieldName)) },
                HasSet = true,
                SetStatements = { new CodeAssignStatement(new CodeFieldReferenceExpression(new CodeThisReferenceExpression(), fieldName), new CodePropertySetValueReferenceExpression()) },
                Comments = { new CodeCommentStatement("<summary>", true), new CodeCommentStatement(schema.Description, true), new CodeCommentStatement("</summary>", true) },
                CustomAttributes = attributes
            };

            target.Members.Add(property);
        }

        public static CodeTypeReference GetCodegenType(Schema schema, string name, out CodeAttributeDeclarationCollection attributes, out CodeExpression defaultValue)
        {
            attributes = null;
            defaultValue = null;

            if (schema.ReferenceType != null)
            {
                throw new InvalidOperationException();
            }

            if (schema.DictionaryValueType == null)
            {
                if (schema.Type == null)
                {
                    throw new InvalidOperationException();
                }
                // Single Objects
                if (schema.Type.Length > 1) return new CodeTypeReference(typeof(object));
                var typeRef = schema.Type[0];
                attributes = null;
                if (typeRef.IsReference) throw new NotImplementedException();
                if (typeRef.Name == "any") return new CodeTypeReference(typeof(object));
                if (typeRef.Name == "object")
                {
                    if (schema.Title != null) return new CodeTypeReference(ParseTitle(schema.Title));
                    throw new NotImplementedException();
                }
                if (typeRef.Name == "number")
                {
                    if (schema.Default != null)
                    {
                        defaultValue = new CodePrimitiveExpression((float)schema.Default);
                    }
                    return new CodeTypeReference(typeof(float));
                }
                if (typeRef.Name == "string")
                {
                    if (schema.Default != null)
                    {
                        defaultValue = new CodePrimitiveExpression((string)schema.Default);
                    }
                    return new CodeTypeReference(typeof(string));
                }
                if (typeRef.Name == "integer")
                {
                    if (schema.Default != null)
                    {
                        defaultValue = new CodePrimitiveExpression((int)(long)schema.Default);
                    }

                    return new CodeTypeReference(typeof(int));
                }
                if (typeRef.Name == "boolean")
                {
                    if (schema.Default != null)
                    {
                        defaultValue = new CodePrimitiveExpression((bool)schema.Default);
                    }
                    return new CodeTypeReference(typeof(bool));
                }

                // Arrays
                if (typeRef.Name == "array")
                {
                    attributes = new CodeAttributeDeclarationCollection
                    {
                        new CodeAttributeDeclaration(
                            "Newtonsoft.Json.JsonConverterAttribute",
                            new [] {
                                new CodeAttributeArgument(new CodeTypeOfExpression(typeof(ArrayConverter))),
                                new CodeAttributeArgument(
                                    new CodeArrayCreateExpression(typeof(object), new CodeExpression[]
                                    {
                                        new CodePrimitiveExpression(schema.MinItems ?? -1),
                                        new CodePrimitiveExpression(schema.MaxItems ?? -1),
                                    })
                                ),
                            }
                        )
                    };

                    if (schema.Items.Type.Length > 1) return new CodeTypeReference(typeof(object[]));
                    if (schema.Items.Type[0].Name == "boolean")
                    {
                        if (schema.Default != null)
                        {
                            var defaultVauleArray = (JArray)schema.Default;
                            defaultValue = new CodeArrayCreateExpression(typeof (bool), defaultVauleArray.Select(x=>(CodeExpression)new CodePrimitiveExpression((bool)x)).ToArray());
                        }
                        return new CodeTypeReference(typeof(bool[]));
                    }
                    if (schema.Items.Type[0].Name == "string")
                    {
                        if (schema.Default != null)
                        {
                            var defaultVauleArray = (JArray)schema.Default;
                            defaultValue = new CodeArrayCreateExpression(typeof(string), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((string)x)).ToArray());
                        }
                        return new CodeTypeReference(typeof(string[]));
                    }
                    if (schema.Items.Type[0].Name == "integer")
                    {
                        if (schema.Default != null)
                        {
                            var defaultVauleArray = (JArray)schema.Default;
                            defaultValue = new CodeArrayCreateExpression(typeof(int), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((int)(long)x)).ToArray());
                        }
                        return new CodeTypeReference(typeof(int[]));
                    }
                    if (schema.Items.Type[0].Name == "number")
                    {
                        if (schema.Default != null)
                        {
                            var defaultVauleArray = (JArray)schema.Default;
                            defaultValue = new CodeArrayCreateExpression(typeof(float), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((float)x)).ToArray());
                        }
                        return new CodeTypeReference(typeof(float[]));
                    }
                    if (schema.Items.Type[0].Name == "object") return new CodeTypeReference(typeof(object[]));

                    throw new NotImplementedException("Array of " + schema.Items.Type[0].Name);
                }

                throw new NotImplementedException(typeRef.Name);
            }

            if (schema.Type == null || schema.Type[0].Name != "object")
            {
                throw new InvalidOperationException();
            }

            // Dictionaries
            if (schema.DictionaryValueType.Type.Length > 1)
            {
                return new CodeTypeReference(typeof(Dictionary<string, object>));
            }
            if (schema.Default!=null && ((JObject)schema.Default).Count > 0)
            {
                throw new NotImplementedException();
            }
            if (schema.DictionaryValueType.Type[0].Name == "object")
            {
                if (schema.DictionaryValueType.Title != null)
                    return new CodeTypeReference($"System.Collections.Generic.Dictionary<string, {ParseTitle(schema.DictionaryValueType.Title)}>");
                return new CodeTypeReference(typeof(Dictionary<string, object>));
            }

            if (schema.DictionaryValueType.Type[0].Name == "string")
            {
                return new CodeTypeReference(typeof(Dictionary<string, string>));
            }

            throw new NotImplementedException();
        }

        [JsonConverter(typeof(ArrayConverter))]
        public object ArrayPropertiesTemplate { get; set; }

        public void CSharpCodeGen(string outputDirectory)
        {
            GeneratedClasses = new Dictionary<string, CodeTypeDeclaration>();
            foreach (var schema in FileSchemas)
            {
                if (schema.Value.Type != null && schema.Value.Type[0].Name == "object")
                {
                    CodeGenClass(schema.Key, outputDirectory);
                }
            }
        }

        private void CodeGenClass(string fileName, string outputDirectory)
        {
            string className;
            var schemaFile = RawClass(fileName, out className);
            CSharpCodeProvider csharpcodeprovider = new CSharpCodeProvider();
            var sourceFile = Path.Combine(outputDirectory, className + "." + csharpcodeprovider.FileExtension);

            IndentedTextWriter tw1 = new IndentedTextWriter(new StreamWriter(sourceFile, false), "    ");
            csharpcodeprovider.GenerateCodeFromCompileUnit(schemaFile, tw1, new CodeGeneratorOptions());
            tw1.Close();
        }

        public static string ParseTitle(string RawTilte)
        {
            var words = RawTilte.ToLower().Split(' ');
            string retval = "";
            foreach (var word in words)
            {
                retval += word[0].ToString().ToUpper();
                retval += word.Substring(1);
            }
            return retval;
        }
    }
}
