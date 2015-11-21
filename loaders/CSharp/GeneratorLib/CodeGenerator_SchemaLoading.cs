using System.Collections.Generic;
using System.IO;
using System.Linq;

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
            FileSchemas = new SchemaParser(m_directory).ParseSchemaTree(m_rootSchemaName);
        }

        public void ExpandSchemaReferences()
        {
            ExpandSchemaReferences(FileSchemas[m_rootSchemaName]);
        }

        private void ExpandSchemaReferences(Schema schema)
        {
            foreach (var typeReference in new TypeReferenceEnumerator(schema))
            {
                if (typeReference.IsReference)
                {
                    ExpandSchemaReferences(FileSchemas[typeReference.Name]);
                }
            }

            if (schema.Properties != null)
            {
                var keys = schema.Properties.Keys.ToArray();
                foreach (var key in keys)
                {
                    if (!string.IsNullOrEmpty(schema.Properties[key].ReferenceType))
                    {
                        schema.Properties[key] = FileSchemas[schema.Properties[key].ReferenceType];
                    }

                    ExpandSchemaReferences(schema.Properties[key]);
                }
            }

            if (schema.DictionaryValueType != null)
            {
                if (!string.IsNullOrEmpty(schema.DictionaryValueType.ReferenceType))
                {
                    schema.DictionaryValueType = FileSchemas[schema.DictionaryValueType.ReferenceType];
                }

                ExpandSchemaReferences(schema.DictionaryValueType);
            }

            if (schema.Items != null)
            {
                if (!string.IsNullOrEmpty(schema.Items.ReferenceType))
                {
                    schema.Items = FileSchemas[schema.Items.ReferenceType];
                }

                ExpandSchemaReferences(schema.Items);
            }
        }

        public void EvaluateInheritance()
        {
            EvaluateInheritance(FileSchemas[m_rootSchemaName]);
        }

        private void EvaluateInheritance(Schema schema)
        {
            foreach (var subSchema in new SchemaEnumerator(schema))
            {
                EvaluateInheritance(subSchema);
            }

            foreach (var typeReference in new TypeReferenceEnumerator(schema))
            {
                if (typeReference.IsReference)
                {
                    EvaluateInheritance(FileSchemas[typeReference.Name]);
                }
            }

            if (schema.Extends == null) return;

            // var baseSchema = FileSchemas[schema.Extends.Name];
            // if (baseSchema.Type.Length == 1 && baseSchema.Type[0].Name == "object") return;

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
    }
}
