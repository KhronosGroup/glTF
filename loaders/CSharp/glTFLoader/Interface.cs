using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using glTFLoader.Schema;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace glTFLoader
{
    public static class Interface
    {
        public static Gltf LoadModel(string filePath)
        {
            var path = Path.GetFullPath(filePath);
            CallContext.LogicalSetData("UriRootPath", Path.GetDirectoryName(path));
            var bytes = File.ReadAllBytes(path);
            
            // Load a normal gltf model
            if (bytes[0] != 'g' || bytes[1] != 'l' || bytes[2] != 'T' || bytes[3] != 'F')
            {
                return JsonConvert.DeserializeObject<Gltf>(Encoding.UTF8.GetString(bytes));
            }

            // KHR_binary_glTF: Header
            var version = BitConverter.ToUInt32(bytes, 4);
            if (version != 1)
            {
                throw new NotImplementedException($"Unknown version number {version}");
            }

            var length = (int)BitConverter.ToUInt32(bytes, 8);
            if (length != bytes.Length)
            {
                throw new InvalidDataException($"The specified length of the file ({length}) is not equal to the actual length of the file ({bytes.Length}).");
            }

            var sceneFormat = BitConverter.ToInt32(bytes, 16);
            if (sceneFormat != 0)
            {
                throw new NotImplementedException("Only scene format 0(JSON) is supported.");
            }

            var sceneLength = (int)BitConverter.ToUInt32(bytes, 12);

            // KHR_binary_glTF: scene
            var bufferContents = new ArraySegment<byte>(bytes, 20 + sceneLength, length - 20 - sceneLength);

            CallContext.LogicalSetData("KHR_binary_glTF", bufferContents);

            var model = JsonConvert.DeserializeObject<Gltf>(Encoding.UTF8.GetString(bytes, 20, sceneLength));

            var segmentsToRemove = new List<string>();

            // KHR_binary_glTF: URIs
            if (model.Images != null)
            {
                foreach (var image in model.Images.Values)
                {
                    if (image.Extensions.ContainsKey("KHR_binary_glTF"))
                    {
                        var extension = (JObject)image.Extensions["KHR_binary_glTF"];
                        var bufferViewName = extension["bufferView"].Value<string>();
                        segmentsToRemove.Add(bufferViewName);
                        var view = model.BufferViews[bufferViewName];
                        image.Uri = new Bitmap(new MemoryStream(bufferContents.Array, bufferContents.Offset + view.ByteOffset, view.ByteLength));

                        var width = extension["width"].Value<long>();
                        if (width != image.Uri.Width)
                        {
                            throw new InvalidDataException($"The width of the given image ({image.Uri.Width}) does not match the width specified in the metadata ({width})");
                        }
                        var height = extension["height"].Value<long>();
                        if (height != image.Uri.Height)
                        {
                            throw new InvalidDataException($"The height of the given image ({image.Uri.Height}) does not match the width specified in the metadata ({height})");
                        }
                        var mimeType = extension["mimeType"].Value<string>();
                        if (image.Uri.RawFormat.Equals(ImageFormat.Jpeg) && mimeType != "image/jpeg")
                        {
                            throw new InvalidDataException($"Expexted a {mimeType} but image is actually a {image.Uri.RawFormat}");
                        }
                        if (image.Uri.RawFormat.Equals(ImageFormat.Png) && mimeType != "image/png")
                        {
                            throw new InvalidDataException($"Expexted a {mimeType} but image is actually a {image.Uri.RawFormat}");
                        }
                    }
                }
            }

            if (model.Shaders != null)
            {
                foreach (var shader in model.Shaders.Values)
                {
                    if (shader.Extensions.ContainsKey("KHR_binary_glTF"))
                    {
                        var extension = (JObject)shader.Extensions["KHR_binary_glTF"];
                        var bufferViewName = extension["bufferView"].Value<string>();
                        segmentsToRemove.Add(bufferViewName);
                        var view = model.BufferViews[bufferViewName];
                        shader.Uri = Encoding.UTF8.GetString(bufferContents.Array, bufferContents.Offset + view.ByteOffset, view.ByteLength);
                    }
                }
            }

            var arraySegmentsToRemove =
                segmentsToRemove.Select(s => model.BufferViews[s])
                    .Select(v => new ArraySegment<byte>(bytes, bufferContents.Offset + v.ByteOffset, v.ByteLength))
                    .ToList();

            while (arraySegmentsToRemove.Any(subSegment => bufferContents.TryRemoveFromEnd(subSegment, out bufferContents)))
            {
            }
            if (model.Buffers.ContainsKey("KHR_binary_glTF"))
            {
                model.Buffers["KHR_binary_glTF"].Uri = bufferContents.ToArray();
            }
            else
            {
                throw new NotImplementedException();
            }
            foreach (var viewName in segmentsToRemove)
            {
                model.BufferViews.Remove(viewName);
            }
            return model;
        }

        private static bool IsAtEnd<T>(this ArraySegment<T> subSegment, ArraySegment<T> longSegment)
        {
            if (subSegment.Array != longSegment.Array) return false;
            if (subSegment.Offset + subSegment.Count == longSegment.Offset + longSegment.Count && longSegment.Offset <= subSegment.Offset) return true;
            return false;
        }

        private static bool TryRemoveFromEnd<T>(this ArraySegment<T> longSegment, ArraySegment<T> subSegment, out ArraySegment<T> newSegment)
        {
            if (!subSegment.IsAtEnd(longSegment))
            {
                newSegment = longSegment;
                return false;
            }
            
            newSegment = new ArraySegment<T>(longSegment.Array, longSegment.Offset, longSegment.Count - subSegment.Count);

            return true;
        }

        public static string SerializeModel(Gltf model)
        {
            var json = JsonConvert.SerializeObject(model, Formatting.Indented);
            return json;
        }

        public static void SaveModel(Gltf model, string path)
        {
            File.WriteAllText(path, SerializeModel(model));
        }

    }



}
