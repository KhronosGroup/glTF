using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace glTFLoader.Shared
{
    public class ArrayConverter : JsonConverter
    {
        private int m_minItems = -1, m_maxItems = -1;
        public ArrayConverter()
        {
        }

        public ArrayConverter(int minItems, int maxItems)
        {
            m_minItems = minItems;
            m_maxItems = maxItems;
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (objectType == typeof(bool[])) return ReadImpl<bool>(reader);
            if (objectType == typeof(int[])) return ((long[])ReadImpl<long>(reader)).Select((v) => (int)v).ToArray();
            if (objectType == typeof(string[])) return ReadImpl<string>(reader);
            if (objectType == typeof(float[])) return ReadFloats(reader);
            if (objectType == typeof(object[])) return ReadImpl<object>(reader);

            throw new NotImplementedException();
        }

        private object ReadImpl<t_array>(JsonReader reader)
        {
            if (reader.TokenType != JsonToken.StartArray)
            {
                return new[] { (t_array) reader.Value };
            }

            reader.Read();
            var values = new List<t_array>();
            while (reader.TokenType != JsonToken.EndArray)
            {
                values.Add((t_array)reader.Value);
                reader.Read();
            }

            var array = values.ToArray();
            VerifyArraySize(array);
            return array;
        }

        private object ReadFloats(JsonReader reader)
        {
            if (reader.TokenType != JsonToken.StartArray)
            {
                return new[] { SingleValueToFloat(reader.TokenType, reader.Value) };
            }

            reader.Read();
            var values = new List<float>();
            while (reader.TokenType != JsonToken.EndArray)
            {
                values.Add(SingleValueToFloat(reader.TokenType, reader.Value));
                reader.Read();
            }

            var array = values.ToArray();
            VerifyArraySize(array);
            return array;
        }

        private float SingleValueToFloat(JsonToken tokenType, object value)
        {
            switch (tokenType)
            {
                case JsonToken.Integer:
                    return (long) value;
                case JsonToken.Float:
                    return (float)(double) value;
                default:
                    throw new NotImplementedException(tokenType.ToString());
            }
        }

        private void VerifyArraySize<t_array>(t_array[] array)
        {
            if (m_minItems != -1 && m_minItems > array.Length)
            {
                throw new ArgumentException($"{array.Length} items provided for array with minimum length of {m_minItems}");
            }

            if (m_maxItems != -1 && m_maxItems < array.Length)
            {
                throw new ArgumentException($"{array.Length} items provided for array with maximum length of {m_maxItems}");
            }
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
