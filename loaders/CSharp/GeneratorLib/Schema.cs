using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace GeneratorLib
{
    public class SchemaEnumerator : IEnumerable<Schema>
    {
        private readonly Schema m_schema;

        public SchemaEnumerator(Schema schema)
        {
            m_schema = schema;
        }

        public IEnumerator<Schema> GetEnumerator()
        {
            return (m_schema.Properties != null ? m_schema.Properties.Values : Enumerable.Empty<Schema>())
                .Concat(m_schema.PatternProperties != null ? m_schema.PatternProperties.Values : Enumerable.Empty<Schema>())
                .Concat(m_schema.AdditionalItems != null ? new[] { m_schema.AdditionalItems } : Enumerable.Empty<Schema>())
                .Concat(m_schema.DictionaryValueType != null ? new[] { m_schema.DictionaryValueType } : Enumerable.Empty<Schema>())
                .Concat(m_schema.Items != null ? new[] { m_schema.Items } : Enumerable.Empty<Schema>())
                .GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }

    public class TypeReferenceEnumerator : IEnumerable<TypeReference>
    {
        private readonly Schema m_schema;

        public TypeReferenceEnumerator(Schema schema)
        {
            m_schema = schema;
        }

        public IEnumerator<TypeReference> GetEnumerator()
        {
            return (m_schema.Type ?? Enumerable.Empty<TypeReference>())
                .Concat(m_schema.Extends != null ? new[] { m_schema.Extends } : Enumerable.Empty<TypeReference>())
                .Concat(m_schema.ReferenceType != null ? new[] { new TypeReference() { IsReference = true, Name = m_schema.ReferenceType } } : Enumerable.Empty<TypeReference>())
                .GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }

    public class Schema
    {
        public Schema AdditionalItems { get; set; }

        public Dictionary<string, string> Dependencies { get; set; }

        public object Default { get; set; }

        public string Description { get; set; }

        [JsonProperty("gltf_detailedDescription")]
        public string DetailedDescription { get; set; }

        [JsonProperty("additionalProperties")]
        public Schema DictionaryValueType { get; set; }

        public object Disallowed { get; set; }

        public int DivisibleBy { get; set; }

        [JsonConverter(typeof(TypeReferenceConverter))]
        public TypeReference Extends { get; set; }

        public object Enum { get; set; }

        [JsonProperty("gltf_enumNames")]
        public string[] EnumNames { get; set; }

        public bool ExclusiveMaximum { get; set; } = false;

        public bool ExclusiveMinimum { get; set; } = false;

        public string Format { get; set; }

        public Schema Items { get; set; }

        public string Id { get; set; }

        public int? MaxItems { get; set; }

        public int MaxLength { get; set; } = -1;

        public object Maximum { get; set; }

        public int? MinItems { get; set; }

        public int MinLength { get; set; } = -1;

        public object Minimum { get; set; }

        public Dictionary<string, Schema> PatternProperties { get; set; }

        public string Pattern { get; set; }

        public Dictionary<string, Schema> Properties { get; set; }

        [JsonProperty("__ref__")]
        public string ReferenceType { get; set; }

        public bool Required { get; set; } = false;

        public string ResolvedType { get; set; }

        public string Title { get; set; }

        [JsonConverter(typeof(ArrayOfTypeReferencesConverter))]
        public TypeReference[] Type { get; set; }

        public bool UniqueItems { get; set; }

        [JsonProperty("gltf_uriType")]
        public UriType UriType { get; set; } = UriType.None;

        [JsonProperty("gltf_webgl")]
        public string WebGl { get; set; }

    }

    public class TypeReference
    {
        public bool IsReference { get; set; }

        public string Name { get; set; }
    }

    public enum UriType
    {
        None,
        Application,
        Text,
        Image
    }
}
