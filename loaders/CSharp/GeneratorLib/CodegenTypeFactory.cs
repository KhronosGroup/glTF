using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Data.Common;
using System.Diagnostics.Contracts;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http.Headers;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using glTFLoader.Shared;
using Newtonsoft.Json.Linq;

namespace GeneratorLib
{
    public static class CodegenTypeFactory
    {
        private static readonly object s_enumMapLock = new object();

        public static CodegenType MakeCodegenType(string name, Schema schema)
        {
            if (schema.Disallowed != null || schema.Pattern != null)
            {
                throw new NotImplementedException();
            }

            if (schema.ReferenceType != null)
            {
                throw new InvalidOperationException("We don't support de-referencing here.");
            }

            if (!(schema.Type?.Length >= 1))
            {
                throw new InvalidOperationException("This schema does not represent a type");
            }

            if (schema.DictionaryValueType == null)
            {
                if (schema.Type.Length == 1 && !schema.Type[0].IsReference && schema.Type[0].Name == "array")
                {
                    return MakeArrayType(name, schema);
                }

                return MakeSingleValueType(name, schema);
            }

            if (schema.Type.Length == 1 && schema.Type[0].Name == "object")
            {
                return MakeDictionaryType(name, schema);
            }

            throw new InvalidOperationException();
        }

        private static CodegenType MakeSingleValueType(string name, Schema schema)
        {
            CodegenType returnType = new CodegenType();
            if (schema.Minimum != null || schema.Maximum != null)
            {
                returnType.Attributes = new CodeAttributeDeclarationCollection
                {
                    new CodeAttributeDeclaration(
                        "Newtonsoft.Json.JsonConverterAttribute",
                        new[]
                        {
                            new CodeAttributeArgument(new CodeTypeOfExpression(typeof (NumberValidator))),
                            new CodeAttributeArgument(
                                new CodeArrayCreateExpression(typeof (object), new CodeExpression[]
                                {
                                    new CodePrimitiveExpression(schema.Minimum ?? 0),
                                    new CodePrimitiveExpression(schema.Maximum ?? 0),
                                    new CodePrimitiveExpression(schema.Minimum != null),
                                    new CodePrimitiveExpression(schema.Maximum != null),
                                    new CodePrimitiveExpression(schema.ExclusiveMinimum),
                                    new CodePrimitiveExpression(schema.ExclusiveMaximum),
                                })
                                ),
                        }
                        )
                };
            }

            if (schema.Type.Length > 1)
            {
                returnType.CodeType = new CodeTypeReference(typeof(object));
                return returnType;
            }

            var typeRef = schema.Type[0];
            if (typeRef.IsReference)
            {
                throw new NotImplementedException();
            }

            if (typeRef.Name == "any")
            {
                if (schema.Enum != null || schema.Default != null)
                {
                    throw new NotImplementedException();
                }

                returnType.CodeType = new CodeTypeReference(typeof(object));
                return returnType;
            }

            if (typeRef.Name == "object")
            {
                if (schema.Enum != null || schema.HasDefaultValue())
                {
                    throw new NotImplementedException();
                }

                if (schema.Title != null)
                {
                    returnType.CodeType = new CodeTypeReference(Helpers.ParseTitle(schema.Title));
                    return returnType;
                }
                throw new NotImplementedException();
            }

            if (typeRef.Name == "number")
            {
                if (schema.Enum != null)
                {
                    throw new NotImplementedException();
                }

                if (schema.HasDefaultValue())
                {
                    returnType.DefaultValue = new CodePrimitiveExpression((float)(double)schema.Default);
                }
                returnType.CodeType = new CodeTypeReference(typeof(float));
                return returnType;
            }

            if (typeRef.Name == "string")
            {
                if (schema.Enum != null)
                {
                    var enumName = $"{name}Enum";
                    var enumType = new CodeTypeDeclaration()
                    {
                        IsEnum = true,
                        Attributes = MemberAttributes.Public,
                        Name = enumName
                    };

                    foreach (var value in (JArray)schema.Enum)
                    {
                        enumType.Members.Add(new CodeMemberField(enumName, (string)value));
                    }

                    returnType.DependentType = enumType;
                    returnType.CodeType = new CodeTypeReference(enumName);

                    if (schema.HasDefaultValue())
                    {
                        for (var i = 0; i < enumType.Members.Count; i++)
                        {
                            if (enumType.Members[i].Name == schema.Default.ToString())
                            {
                                returnType.DefaultValue =
                                    new CodeFieldReferenceExpression(new CodeTypeReferenceExpression(enumName),
                                        (string)schema.Default);

                                return returnType;
                            }
                        }
                        throw new InvalidDataException("The default value is not in the enum list");
                    }

                    return returnType;
                }

                if (schema.HasDefaultValue())
                {
                    returnType.DefaultValue = new CodePrimitiveExpression((string)schema.Default);
                }
                returnType.CodeType = new CodeTypeReference(typeof(string));
                return returnType;
            }

            if (typeRef.Name == "integer")
            {
                if (schema.Enum != null)
                {
                    var enumName = $"{name}Enum";
                    var enumType = new CodeTypeDeclaration()
                    {
                        IsEnum = true,
                        Attributes = MemberAttributes.Public,
                        Name = enumName
                    };

                    string defaultItemName = null;

                    if (schema.EnumNames == null || ((JArray)schema.Enum).Count != schema.EnumNames.Length)
                    {
                        throw new InvalidOperationException("Enum names must be defined for each integer enum");
                    }

                    foreach (var index in Enumerable.Range(0, schema.EnumNames.Length))
                    {
                        var value = (int) (long) ((JArray) schema.Enum)[index];
                        enumType.Members.Add(new CodeMemberField(enumName, schema.EnumNames[index])
                        {
                            InitExpression = new CodePrimitiveExpression(value)
                        });

                        if (schema.HasDefaultValue() && (int)(long)schema.Default == value)
                        {
                            defaultItemName = schema.EnumNames[index];
                        }
                    }

                    returnType.DependentType = enumType;
                    returnType.CodeType = new CodeTypeReference(enumName);

                    if (schema.HasDefaultValue())
                    {
                        if (defaultItemName == null)
                        {
                            throw new InvalidDataException("The default value is not in the enum list");
                        }

                        returnType.DefaultValue =
                            new CodeFieldReferenceExpression(new CodeTypeReferenceExpression(enumName),
                                defaultItemName);

                        return returnType;
                    }

                    return returnType;
                }

                if (schema.Default != null)
                {
                    returnType.DefaultValue = new CodePrimitiveExpression((int)(long)schema.Default);
                }

                returnType.CodeType = new CodeTypeReference(typeof(int));
                return returnType;
            }

            if (typeRef.Name == "boolean")
            {
                if (schema.Enum != null)
                {
                    throw new NotImplementedException();
                }

                if (schema.Default != null)
                {
                    returnType.DefaultValue = new CodePrimitiveExpression((bool)schema.Default);
                }
                returnType.CodeType = new CodeTypeReference(typeof(bool));
                return returnType;
            }

            throw new NotImplementedException(typeRef.Name);
        }

        private static CodegenType MakeArrayType(string name, Schema schema)
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

        private static CodegenType MakeDictionaryType(string name, Schema schema)
        {
            var returnType = new CodegenType();

            if (schema.DictionaryValueType.Type.Length > 1)
            {
                returnType.CodeType = new CodeTypeReference(typeof(Dictionary<string, object>));
                return returnType;
            }

            if (schema.HasDefaultValue())
            {
                throw new NotImplementedException("Defaults for dictionaries are not yet supported");
            }

            if (schema.DictionaryValueType.Type[0].Name == "object")
            {

                if (schema.DictionaryValueType.Title != null)
                {
                    returnType.CodeType = new CodeTypeReference($"System.Collections.Generic.Dictionary<string, {Helpers.ParseTitle(schema.DictionaryValueType.Title)}>");
                    return returnType;
                }
                returnType.CodeType = new CodeTypeReference(typeof(Dictionary<string, object>));
                return returnType;
            }

            if (schema.DictionaryValueType.Type[0].Name == "string")
            {
                returnType.CodeType = new CodeTypeReference(typeof(Dictionary<string, string>));
                return returnType;
            }

            throw new NotImplementedException($"Dictionary<string,{schema.DictionaryValueType.Type[0].Name}> not yet implemented.");
        }
    }
}
