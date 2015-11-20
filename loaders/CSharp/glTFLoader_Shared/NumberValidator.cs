using System;
using System.IO;
using Newtonsoft.Json;


namespace glTFLoader.Shared
{
    public class NumberValidator : JsonConverter
    {
        private readonly object m_minimum, m_maximum;
        private readonly bool m_isExclusiveMinimum, m_isExclusiveMaximum, m_hasMinimum, m_hasMaximum;

        public NumberValidator(object minimum, object maximum, bool hasMinimum, bool hasMaximum, bool isExclusiveMinimum, bool isExclusiveMaximum)
        {
            // This is a workaround so that the JsonReader can resolve the types of minimum and maximum at runtime
            m_minimum = minimum;
            m_maximum = maximum;
            m_hasMinimum = hasMinimum;
            m_hasMaximum = hasMaximum;
            m_isExclusiveMinimum = isExclusiveMinimum;
            m_isExclusiveMaximum = isExclusiveMaximum;
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            if (objectType == typeof(int) && reader.TokenType == JsonToken.Integer)
            {
                int value = (int)(long)reader.Value;
                ValidationHelpers.CheckRange(value, (int)m_minimum, (int)m_maximum, m_hasMinimum, m_hasMaximum, m_isExclusiveMinimum, m_isExclusiveMaximum);
                return value;
            }
            if (objectType == typeof(float))
            {
                float value = reader.Value.ToFloat();
                ValidationHelpers.CheckRange(value, m_minimum.ToFloat(), m_maximum.ToFloat(), m_hasMinimum, m_hasMaximum, m_isExclusiveMinimum, m_isExclusiveMaximum);
                return value;
            }

            throw new NotImplementedException();
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
