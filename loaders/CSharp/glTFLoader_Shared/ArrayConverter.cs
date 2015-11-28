using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace glTFLoader.Shared
{
    public class ArrayConverter : JsonConverter
    {
        private readonly int m_minItems = -1, m_maxItems = -1, m_minStringLength = -1, m_maxStringLength = -1;
        public ArrayConverter()
        {
        }

        public ArrayConverter(int minItems, int maxItems, int minStringLength, int maxStringLength)
        {
            m_minItems = minItems;
            m_maxItems = maxItems;
            m_minStringLength = minStringLength;
            m_maxStringLength = maxStringLength;
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (objectType == typeof(bool[])) return ReadImpl<bool>(reader);
            if (objectType == typeof(int[])) return ReadImpl<long>(reader).Select((v) => (int)v).ToArray();
            if (objectType == typeof (string[]))
            {
                var stringArray = ReadImpl<string>(reader);
                if (m_minStringLength != -1 || m_maxStringLength != -1)
                {
                    foreach (var s in stringArray)
                    {
                        ValidationHelpers.ValidateString(s, m_minStringLength, m_maxStringLength);
                    }
                }

                return stringArray;
            }
            if (objectType == typeof(float[])) return ReadFloats(reader);
            if (objectType == typeof(object[])) return ReadImpl<object>(reader);

            if (objectType.IsArray && objectType.GetElementType().IsEnum)
            {
                var elementType = objectType.GetElementType();
                var rawValues = ReadImpl<long>(reader).Select((v) => (int)v).ToArray();

                var resultArray = Array.CreateInstance(elementType, rawValues.Length);

                for (int i = 0; i < rawValues.Length; ++i)
                {
                    var enumerator = elementType.GetEnumValues().GetEnumerator();
                    do
                    {
                        enumerator.MoveNext();
                    } while ((int)enumerator.Current != rawValues[i]);
                    resultArray.SetValue(enumerator.Current, i);
                }

                return resultArray;
            }

            throw new NotImplementedException();
        }

        private t_array[] ReadImpl<t_array>(JsonReader reader)
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

        public override bool CanWrite
        {
            get { return false; }
        }

        public override void WriteJson(JsonWriter w, object o, JsonSerializer s)
        {
            throw new NotImplementedException();
        }
    }
}
