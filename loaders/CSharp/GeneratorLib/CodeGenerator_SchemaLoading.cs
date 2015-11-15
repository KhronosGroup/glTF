using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace GeneratorLib
{
    public partial class CodeGenerator
    {
        private string m_directory;
        private string m_rootSchemaName;

        public CodeGenerator(string rootSchemaFilePath)
        {
            rootSchemaFilePath = Path.GetFullPath(rootSchemaFilePath);
            m_directory = Path.GetDirectoryName(rootSchemaFilePath);
            m_rootSchemaName = Path.GetFileName(rootSchemaFilePath);
        }

        public Dictionary<string, Schema> FileSchemas { get; private set; }

        public void ParseSchemas()
        {
            FileSchemas = new Dictionary<string, Schema>();
            ParseSchema(m_rootSchemaName);
        }

        public void ExpandSchemaReferences()
        {
            var keys = FileSchemas.Keys.ToArray();
            foreach (var key in keys)
            {
                ExapandSchemaReferences(FileSchemas[key]);
            }
        }

        public void EvaluateInheritance()
        {
            EvaluateInheritance(FileSchemas[m_rootSchemaName]);
        }

        private void EvaluateInheritance(Schema schema)
        {
            if (schema.Type != null)
            {
                foreach (var type in schema.Type)
                {
                    if (!type.IsReference) continue;

                    EvaluateInheritance(FileSchemas[type.Name]);
                }
            }

            if (schema.Properties != null)
            {
                foreach (var property in schema.Properties)
                {
                    EvaluateInheritance(property.Value);
                }
            }

            if (schema.Items != null)
            {
                EvaluateInheritance(schema.Items);
            }

            if (schema.Extends == null) return;
            var baseSchema = FileSchemas[schema.Extends.Name];
            if (baseSchema.Type.Length == 1 && baseSchema.Type[0].Name == "object") return;

            if (schema.DictionaryValueType != null && schema.DictionaryValueType.IsReference)
            {
                EvaluateInheritance(FileSchemas[schema.DictionaryValueType.Name]);
            }

            EvaluateInheritance(FileSchemas[schema.Extends.Name]);

            var baseType = FileSchemas[schema.Extends.Name];

            if (schema.Properties != null && baseType.Properties != null)
            {
                foreach (var property in baseType.Properties)
                {
                    schema.Properties.Add(property.Key, property.Value);
                }
            }

            foreach (var property in baseType.GetType().GetProperties())
            {
                if (!property.CanRead || !property.CanWrite) continue;

                if (property.GetValue(schema) == null)
                {
                    property.SetValue(schema, property.GetValue(baseType));
                }
            }

            schema.Extends = null;
        }

        private void ExapandSchemaReferences(Schema schema)
        {
            if (schema.Properties != null)
            {
                var keys = schema.Properties.Keys.ToArray();
                foreach (var key in keys)
                {
                    if (string.IsNullOrWhiteSpace(schema.Properties[key].ReferenceType))
                    {
                        ExapandSchemaReferences(schema.Properties[key]);
                        continue;
                    }

                    schema.Properties[key] = FileSchemas[schema.Properties[key].ReferenceType];
                }
            }
        }

        private void ParseSchema(string schemaFile)
        {
            if (FileSchemas.ContainsKey(schemaFile))
            {
                return;
            }

            var schema = Deserialize(schemaFile);
            FileSchemas.Add(schemaFile, schema);
            ParseSchemasReferencedFromSchema(schema);
        }

        private void ParseSchemasReferencedFromSchema(Schema schema)
        {
            if (schema.Type != null)
            {
                foreach (var type in schema.Type)
                {
                    if (!type.IsReference) continue;

                    ParseSchema(type.Name);
                }
            }

            if (schema.Properties != null)
            {
                foreach (var property in schema.Properties)
                {
                    if (string.IsNullOrWhiteSpace(property.Value.ReferenceType))
                    {
                        ParseSchemasReferencedFromSchema(property.Value);
                        continue;
                    }

                    ParseSchema(property.Value.ReferenceType);
                }
            }

            if (schema.Extends != null && schema.Extends.IsReference)
            {
                ParseSchema(schema.Extends.Name);
            }

            if (schema.DictionaryValueType != null && schema.DictionaryValueType.IsReference)
            {
                ParseSchema(schema.DictionaryValueType.Name);
            }

            if (schema.Items != null && !string.IsNullOrWhiteSpace(schema.Items.ReferenceType))
            {
                ParseSchema(schema.Items.ReferenceType);
            }
        }
        
        private Schema Deserialize(string fileName)
        {
            return JsonConvert.DeserializeObject<Schema>(
                File.ReadAllText(Path.Combine(m_directory, fileName))
                    .Replace("\"additionalProperties\" : false,", "")
                    .Replace("\"additionalProperties\" : false", "")
                    .Replace("\"$ref\"", "__ref__"));
        }
    }
}
