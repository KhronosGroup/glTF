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
        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (objectType == typeof(bool[])) return ReadImpl<bool>(reader);
            if (objectType == typeof(int[])) return ((long[])ReadImpl<long>(reader)).Select((v) => (int)v).ToArray();
            if (objectType == typeof(string[])) return ReadImpl<string>(reader);
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

            return values.ToArray();
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
