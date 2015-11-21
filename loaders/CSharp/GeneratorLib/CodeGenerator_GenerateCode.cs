using System.Collections.Generic;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
using System.IO;
using glTFLoader.Shared;
using Newtonsoft.Json;

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
            className = Helpers.ParseTitle(root.Title);

            var schemaClass = new CodeTypeDeclaration(className)
            {
                Attributes = MemberAttributes.Public
            };

            if (root.Extends != null && root.Extends.IsReference)
            {
                schemaClass.BaseTypes.Add(Helpers.ParseTitle(FileSchemas[root.Extends.Name].Title));
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
            var type = GetCodegenType(target, schema, name, out attributes, out defaultValue);

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

        public static CodeTypeReference GetCodegenType(CodeTypeDeclaration target, Schema schema, string name, out CodeAttributeDeclarationCollection attributes, out CodeExpression defaultValue)
        {
            var codegenType = CodegenTypeFactory.MakeCodegenType(name, schema);
            attributes = codegenType.Attributes;
            defaultValue = codegenType.DefaultValue;
            if (codegenType.DependentType != null)
            {
                target.Members.Add(codegenType.DependentType);
            }

            return codegenType.CodeType;
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
    }
}
