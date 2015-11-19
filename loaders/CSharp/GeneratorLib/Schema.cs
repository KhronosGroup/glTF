using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace GeneratorLib
{
    public class Schema
    {
        public string ResolvedType { get; set; }

        [JsonProperty("__ref__")]
        public string ReferenceType { get; set; }

        public string Title { get; set; }

        [JsonConverter(typeof(ArrayOfTypeReferencesConverter))]
        public TypeReference[] Type { get; set; }

        public string Description { get; set; }

        [JsonConverter(typeof (TypeReferenceConverter))]
        public TypeReference Extends { get; set; }

        public Dictionary<string, Schema> Properties { get; set; }

        public Dictionary<string, Schema> PatternProperties { get; set; }

        [JsonProperty("additionalProperties")]
        public Schema DictionaryValueType { get; set; }

        public Schema Disallowed { get; set; }

        public int DivisibleBy { get; set; }

        public Dictionary<string, string> Dependencies { get; set; }
        
        public Schema Items { get; set; }

        public string Id { get; set; }

        public Schema AdditionalItems { get; set; }

        public string Pattern { get; set; } //format regex?

        public string Format { get; set; }

        public object Default { get; set; }

        [JsonProperty("gltf_detailedDescription")]
        public string DetailedDescription { get; set; }

        [JsonProperty("gltf_webgl")]
        public string WebGl { get; set; }

        public int? MinItems { get; set; }

        public int? MaxItems { get; set; }

        public object Enum { get; set; }

        public object Minimum { get; set; }

        public object Maximum { get; set; }

        public bool Required { get; set; }

        public bool UniqueItems { get; set; }

        public bool ExclusiveMinimum { get; set; }

        public int MinLength { get; set; }

        public int MaxLength { get; set; }
    }

    public class TypeReference
    {
        public bool IsReference { get; set; }

        public string Name { get; set; }
    }
}
