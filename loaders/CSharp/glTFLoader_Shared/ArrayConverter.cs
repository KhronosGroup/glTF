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
        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (objectType == typeof(bool[])) return ReadImpl<bool>(reader);
            if (objectType == typeof(int[])) return ReadImpl<long>(reader).Select((v) => (int)v).ToArray();
            if (objectType == typeof (string[])) return ReadImpl<string>(reader);
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
            
            return values.ToArray();
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
            
            return values.ToArray();
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
