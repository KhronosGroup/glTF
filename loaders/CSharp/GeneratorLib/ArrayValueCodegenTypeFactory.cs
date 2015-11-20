using System;
using System.CodeDom;
using System.Linq;
using glTFLoader.Shared;
using Newtonsoft.Json.Linq;

namespace GeneratorLib
{
    public class ArrayValueCodegenTypeFactory
    {
        public static CodegenType MakeCodegenType(string name, Schema schema)
        {
            if (!(schema.Items?.Type?.Length > 0))
            {
                throw new InvalidOperationException("Array type must contain an item type");
            }

            if (schema.Enum != null)
            {
                throw new InvalidOperationException();
            }

            if (schema.Items.Disallowed != null)
            {
                throw new NotImplementedException();
            }

            var returnType = new CodegenType()
            {
                Attributes = new CodeAttributeDeclarationCollection
                    {
                        new CodeAttributeDeclaration(
                            "Newtonsoft.Json.JsonConverterAttribute",
                            new [] {
                                new CodeAttributeArgument(new CodeTypeOfExpression(typeof(ArrayConverter))),
                                new CodeAttributeArgument(
                                    new CodeArrayCreateExpression(typeof(object), new CodeExpression[]
                                    {
                                        new CodePrimitiveExpression(schema.MinItems ?? -1),
                                        new CodePrimitiveExpression(schema.MaxItems ?? -1),
                                        new CodePrimitiveExpression(schema.Items.MinLength),
                                        new CodePrimitiveExpression(schema.Items.MaxLength),
                                    })
                                ),
                            }
                        )
                    }
            };

            if (schema.Items.Minimum != null || schema.Items.Maximum != null)
            {
                // TODO: enforce this
                // throw new NotImplementedException();
            }

            if (schema.Items.Type.Length > 1)
            {
                returnType.CodeType = new CodeTypeReference(typeof(object[]));
                return returnType;
            }
            if (schema.Items.Type[0].Name == "boolean")
            {
                if (schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray)schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(bool), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((bool)x)).ToArray());
                }
                returnType.CodeType = new CodeTypeReference(typeof(bool[]));
                return returnType;
            }
            if (schema.Items.Type[0].Name == "string")
            {
                if (schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray)schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(string), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((string)x)).ToArray());
                }

                returnType.CodeType = new CodeTypeReference(typeof(string[]));
                return returnType;
            }
            if (schema.Items.Type[0].Name == "integer")
            {
                if (schema.HasDefaultValue())
                {
                    var defaultVauleArray = ((JArray)schema.Default).Select(x => (CodeExpression)new CodePrimitiveExpression((int)(long)x)).ToArray();
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(int), defaultVauleArray);
                }
                returnType.CodeType = new CodeTypeReference(typeof(int[]));
                return returnType;
            }
            if (schema.Items.Type[0].Name == "number")
            {
                if (schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray)schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(float), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((float)x)).ToArray());
                }
                returnType.CodeType = new CodeTypeReference(typeof(float[]));
                return returnType;
            }
            if (schema.Items.Type[0].Name == "object")
            {
                if (schema.HasDefaultValue())
                {
                    throw new NotImplementedException("Array of Objects has default value");
                }

                returnType.CodeType = new CodeTypeReference(typeof(object[]));
                return returnType;
            }

            throw new NotImplementedException("Array of " + schema.Items.Type[0].Name);
        }
    }
}
