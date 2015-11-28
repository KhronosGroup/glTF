using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
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
            var bytes = GetBytes(reader);
            if (objectType == typeof (byte[]))
            {
                return bytes;
            }

            if (objectType == typeof(string))
            {
                return Encoding.UTF8.GetString(bytes);
            }

            if (objectType == typeof (Bitmap))
            {
                return new Bitmap(new MemoryStream(bytes));
            }

            throw new NotImplementedException();
        }

        private byte[] GetBytes(JsonReader reader)
        {
            if (reader.TokenType != JsonToken.String)
            {
                throw new InvalidDataException($"Expected a string, but token was {reader.TokenType}");
            }

            var rawData = (string)reader.Value;
            if (string.IsNullOrWhiteSpace((string)reader.Value))
            {
                if (m_required)
                {
                    throw new InvalidDataException("The uri is required but is empty");
                }
                return null;
            }
            
            // Data URI
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

            // File Path
            var uriRootPath = (string)CallContext.LogicalGetData("UriRootPath");
            var uriFilePath = uriRootPath + "\\" + rawData;
            if (File.Exists(uriFilePath))
            {
                return File.ReadAllBytes(uriFilePath);
            }
            throw new InvalidOperationException("The file doesn't exist");
        }

        public override bool CanConvert(Type type)
        {
            return true;
        }

        public override void WriteJson(JsonWriter w, object o, JsonSerializer s)
        {
            var result = "";
            if (o is byte[])
            {
                result += "data:application/octet-stream;base64,";
                result += Convert.ToBase64String((byte[])o);
            }
            else if (o is string)
            {
                result += "data:text/plain;base64,";
                result += Convert.ToBase64String(Encoding.UTF8.GetBytes((string)o));
            }
            else if (o is Bitmap)
            {
                result += "data:image/png;base64,";
                var bmp = (Bitmap) o;
                var stream = new MemoryStream();
                bmp.Save(stream, ImageFormat.Png);
                result += Convert.ToBase64String(stream.GetBuffer());
            }
            else
            {
                throw new NotImplementedException();
            }

            w.WriteToken(JsonToken.String, result);
        }
    }
}
