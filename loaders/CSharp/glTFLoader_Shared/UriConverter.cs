using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Mime;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace glTFLoader.Shared
{
    public class UriConverter : JsonConverter
    {
        private readonly bool m_required;

        public UriConverter(bool required)
        {
            m_required = required;
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue,
            JsonSerializer serializer)
        {
            if (objectType == typeof (byte[]) && reader.TokenType == JsonToken.String)
            {
                var rawData = (string) reader.Value;
                if (string.IsNullOrWhiteSpace((string) reader.Value))
                {
                    if (m_required)
                    {
                        throw new InvalidDataException("The uri is required but is empty");
                    }
                    return null;
                }

                if (rawData.StartsWith("data:"))
                {
                    var endOfHead = rawData.IndexOf("base64,", StringComparison.InvariantCultureIgnoreCase);
                    if (endOfHead == -1)
                    {
                        throw new NotImplementedException("Only base64 data uris are supported");
                    }
                    endOfHead += "base64,".Length;
                    rawData = rawData.Substring(endOfHead);
                    return Convert.FromBase64String(rawData);
                }
                else
                {
                    var uriRootPath = (string)CallContext.LogicalGetData("UriRootPath");
                    var uriFilePath = uriRootPath + "\\"+rawData;
                    if (File.Exists(uriFilePath))
                    {
                        return File.ReadAllBytes(uriFilePath);
                    }
                    throw new InvalidOperationException("The file doesn't exist");
                }
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
