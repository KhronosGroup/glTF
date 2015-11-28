using System.IO;
using System.Runtime.Remoting.Messaging;
using glTFLoader.Schema;
using Newtonsoft.Json;

namespace glTFLoader
{
    public static class Interface
    {
        public static Gltf LoadModel(string filePath)
        {
            var path = Path.GetFullPath(filePath);
            CallContext.LogicalSetData("UriRootPath", Path.GetDirectoryName(path));
            return JsonConvert.DeserializeObject<Gltf>(File.ReadAllText(path));
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
