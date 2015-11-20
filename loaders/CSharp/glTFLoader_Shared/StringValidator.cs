using System;
using System.CodeDom;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Schema;
using Newtonsoft.Json;

namespace glTFLoader.Shared
{
    public class StringValidator : JsonConverter
    {
        private readonly int m_minLength, m_maxLength;

        public StringValidator(int minLength, int maxLength)
        {
            m_minLength = minLength;
            m_maxLength = maxLength;
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue,
            JsonSerializer serializer)
        {
            if (objectType != typeof (string) || reader.TokenType != JsonToken.String)
            {
                throw new InvalidOperationException();
            }
            var value = (string)reader.Value;
            ValidationHelpers.ValidateString(value, m_minLength, m_maxLength);
            return value;
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
