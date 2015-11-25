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
            CallContext.LogicalSetData("UriRootPath", path);
            return JsonConvert.DeserializeObject<Gltf>(File.ReadAllText(path));
        }
    }
}
