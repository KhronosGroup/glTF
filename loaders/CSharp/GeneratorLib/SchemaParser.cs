using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;

namespace GeneratorLib
{
    public class SchemaParser
    {
        private string m_directory;

        public SchemaParser(string rootDirectory)
        {
            m_directory = rootDirectory;
        }

        private Dictionary<string, Schema> FileSchemas { get; set; }

        public Dictionary<string, Schema> ParseSchemaTree(string rootFile)
        {
            FileSchemas = new Dictionary<string, Schema>();
            ParseSchema(rootFile);
            return FileSchemas;
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

            if (schema.DictionaryValueType != null && schema.DictionaryValueType.ReferenceType != null)
            {
                ParseSchema(schema.DictionaryValueType.ReferenceType);
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