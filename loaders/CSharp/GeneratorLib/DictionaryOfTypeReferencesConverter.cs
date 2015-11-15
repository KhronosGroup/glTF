using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Newtonsoft.Json;

namespace GeneratorLib
{
    public class DictionaryOfTypeReferencesConverter : JsonConverter
    {
        public override object ReadJson(JsonReader reader, Type objectType, object existingValue,
            JsonSerializer serializer)
        {
            if (reader.TokenType != JsonToken.StartObject)
            {
                throw new ArgumentException("Unexpected token type " + reader.TokenType);
            }
            reader.Read();

            var retval = new Dictionary<string, TypeReference>();

            while (reader.TokenType == JsonToken.PropertyName)
            {
                var name = (string) reader.Value;
                reader.Read();
                reader.Read();
                if (reader.TokenType != JsonToken.PropertyName)
                {
                    throw new ArgumentException("Unexpected token type " + reader.TokenType);
                }

                if ((string) reader.Value != "$ref")
                {
                    throw new ArgumentException("Unexpected token " + reader.Value);
                }

                reader.Read();
                var file = (string) reader.Value;
                retval.Add(name, new TypeReference() { IsReference = true, Name = file });
                reader.Read();
                reader.Read();
            }
            
            return retval;
        }

        private TypeReference[] ReadArray(JsonReader reader)
        {
            var singleItem = ReadItem(reader);
            if (singleItem != null)
            {
                return new[] { singleItem };
            }

            if (reader.TokenType != JsonToken.StartArray)
            {
                throw new ArgumentException("Unexpected token type " + reader.TokenType);
            }

            var tokens = new List<TypeReference>();
            reader.Read();
            while (reader.TokenType != JsonToken.EndArray)
            {
                singleItem = ReadItem(reader);
                if (singleItem != null)
                {
                    tokens.Add(singleItem);
                    reader.Read();
                    continue;
                }

                throw new ArgumentException("Unexpected token type " + reader.TokenType);
            }

            return tokens.ToArray();
        }

        private TypeReference ReadItem(JsonReader reader)
        {
            if (reader.TokenType == JsonToken.String)
            {
                return new TypeReference { IsReference = false, Name = (string) reader.Value };
            }

            if (reader.TokenType == JsonToken.StartObject)
            {
                return ReadObject(reader);
            }

            return null;
        }

        private TypeReference ReadObject(JsonReader reader)
        {
            reader.Read();
            if (reader.TokenType != JsonToken.PropertyName ||
                ((string) reader.Value != "$ref" && (string) reader.Value != "type"))
            {
                throw new ArgumentException("Unexpected token type " + reader.TokenType);
            }

            reader.Read();
            if (reader.TokenType != JsonToken.String)
            {
                throw new ArgumentException("Unexpected token type " + reader.TokenType);
            }

            var retval = new TypeReference
            {
                IsReference = (string) reader.Value == "$ref",
                Name = (string) reader.Value
            };
            reader.Read();

            return retval;
        }

        public override bool CanConvert(Type type)
        {
            return true;
        }

        public override void WriteJson(JsonWriter w, object o, JsonSerializer s)
        {
            throw new NotImplementedException();
        }
    }
}
