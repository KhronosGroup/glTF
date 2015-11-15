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
            var name = rawName.Substring(0,1).ToUpper() + rawName.Substring(1);
            var fieldName = "m_" + name.Substring(0,1).ToLower() + name.Substring(1);
            var type = GetCodegenType(schema);

            var propertyBackingVariable = new CodeMemberField
            {
                Type = type,
                Name = fieldName,
                Comments = { new CodeCommentStatement("<summary>", true), new CodeCommentStatement($"Backing field for {name}.", true), new CodeCommentStatement("</summary>", true) }
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
                Comments = { new CodeCommentStatement("<summary>", true), new CodeCommentStatement(schema.Description, true), new CodeCommentStatement("</summary>", true) }
            };

            target.Members.Add(property);
        }

        CodeTypeReference GetCodegenType(Schema schema)
        {
            if (schema.DictionaryValueType == null)
            {
                if (schema.Extends != null)
                {
                    return new CodeTypeReference(ParseTitle(FileSchemas[schema.Extends.Name].Title));
                }

                if (schema.Type.Length > 1)
                {
                    return new CodeTypeReference(typeof(object));
                }

                return new CodeTypeReference(GetRuntimeType(schema.Type[0]));
            }

            if (schema.DictionaryValueType.IsReference)
            {
                return new CodeTypeReference(ParseTitle(FileSchemas[schema.DictionaryValueType.Name].Title));
            }

            if (schema.DictionaryValueType.Name == "any") return new CodeTypeReference(new Dictionary<string, object>().GetType());
            if (schema.DictionaryValueType.Name == "object") return new CodeTypeReference(new Dictionary<string, object>().GetType());

            throw new NotImplementedException(schema.DictionaryValueType.Name);
        }

        Type GetRuntimeType(TypeReference typeRef)
        {
            if (typeRef.IsReference) throw new NotImplementedException();
            if (typeRef.Name == "any") return typeof(object);
            if (typeRef.Name == "object") return typeof(object);
            if (typeRef.Name == "number") return typeof(object);
            if (typeRef.Name == "string") return typeof(string);
            if (typeRef.Name == "integer") return typeof(int);
            if (typeRef.Name == "boolean") return typeof(bool);
            if (typeRef.Name == "array") return typeof(object[]);

            throw new NotImplementedException(typeRef.Name);
        }
        
        public void CSharpCodeGen()
        {
            GeneratedClasses = new Dictionary<string, CodeTypeDeclaration>();
            foreach (var schema in FileSchemas)
            {
                if (schema.Value.Type != null && schema.Value.Type[0].Name == "object")
                {
                    CodeGenClass(schema.Key);
                }
            }
        }

        private void CodeGenClass(string fileName)
        {
            string className;
            var schemaFile = RawClass(fileName, out className);
            CSharpCodeProvider csharpcodeprovider = new CSharpCodeProvider();
            var sourceFile = className + "." + csharpcodeprovider.FileExtension;

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
