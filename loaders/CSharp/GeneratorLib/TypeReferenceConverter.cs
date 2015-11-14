using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace GeneratorLib
{
    public class TypeReferenceConverter : JsonConverter
    {
        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var converter = new ArrayOfTypeReferencesConverter();
            var result = (TypeReference[])converter.ReadJson(reader, objectType, existingValue, serializer);
            if (result == null)
            {
                return null;
            }

            if (result.Length == 1)
            {
                return result[0];
            }

            throw new ArgumentException("Expected one type and found multiple");
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
