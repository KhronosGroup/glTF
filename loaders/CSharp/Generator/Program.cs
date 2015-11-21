using System.IO;
using GeneratorLib;

namespace Generator
{
    class Program
    {
        static void Main(string[] args)
        {
            var generator = new CodeGenerator(@"..\..\..\..\..\specification\schema\glTF.schema.json");
            generator.ParseSchemas();
            generator.ExpandSchemaReferences();
            generator.EvaluateInheritance();
            generator.CSharpCodeGen(Path.GetFullPath(@"..\..\..\glTFLoader\ParserSchema"));
        }
    }
}
