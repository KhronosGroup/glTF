using System;
using System.CodeDom;
using System.Linq;
using glTFLoader.Shared;
using Newtonsoft.Json.Linq;

namespace GeneratorLib
{
    public class ArrayValueCodegenTypeFactory
    {
        public static CodegenType MakeCodegenType(string name, Schema Schema)
        {
            if (!(Schema.Items?.Type?.Length > 0))
            {
                throw new InvalidOperationException("Array type must contain an item type");
            }

            if (Schema.Enum != null)
            {
                throw new InvalidOperationException();
            }

            if (Schema.Items.Disallowed != null)
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
                                        new CodePrimitiveExpression(Schema.MinItems ?? -1),
                                        new CodePrimitiveExpression(Schema.MaxItems ?? -1),
                                        new CodePrimitiveExpression(Schema.Items.MinLength),
                                        new CodePrimitiveExpression(Schema.Items.MaxLength),
                                    })
                                ),
                            }
                        )
                    }
            };

            if (Schema.Items.Type.Length > 1)
            {
                returnType.CodeType = new CodeTypeReference(typeof(object[]));
                return returnType;
            }
            
            if (Schema.Items.Type[0].Name == "integer")
            {
                if (Schema.Items.Enum != null)
                {
                    var enumType = SingleValueCodegenTypeFactory.GenIntEnumType(name, Schema.Items);
                    returnType.AdditionalMembers.Add(enumType);

                    if (Schema.HasDefaultValue())
                    {
                        var defaultValueArray = ((JArray)Schema.Default).Select(x => (CodeExpression)SingleValueCodegenTypeFactory.GetEnumField(enumType, (int)(long)x)).ToArray();
                        returnType.DefaultValue = new CodeArrayCreateExpression(enumType.Name, defaultValueArray);
                    }

                    returnType.CodeType = new CodeTypeReference(enumType.Name + "[]");
                    return returnType;
                }

                if (Schema.HasDefaultValue())
                {
                    var defaultValueArray = ((JArray)Schema.Default).Select(x => (CodeExpression)new CodePrimitiveExpression((int)(long)x)).ToArray();
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(int), defaultValueArray);
                }
                returnType.CodeType = new CodeTypeReference(typeof(int[]));
                returnType.AdditionalMembers.Add(CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }

            if (Schema.Items.Enum != null)
            {
                throw new NotImplementedException();
            }

            if (Schema.Items.Type[0].Name == "number")
            {
                if (Schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray)Schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(float), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((float)x)).ToArray());
                }
                returnType.CodeType = new CodeTypeReference(typeof(float[]));
                returnType.AdditionalMembers.Add(CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }

            if (Schema.Items.Minimum != null || Schema.Items.Maximum != null)
            {
                // TODO: implement this for int/number
                throw new NotImplementedException();
            }

            if (Schema.Items.Type[0].Name == "boolean")
            {
                if (Schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray)Schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(bool), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((bool)x)).ToArray());
                }
                returnType.CodeType = new CodeTypeReference(typeof(bool[]));
                returnType.AdditionalMembers.Add(CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }
            if (Schema.Items.Type[0].Name == "string")
            {
                if (Schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray)Schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof(string), defaultVauleArray.Select(x => (CodeExpression)new CodePrimitiveExpression((string)x)).ToArray());
                }

                returnType.CodeType = new CodeTypeReference(typeof(string[]));
                returnType.AdditionalMembers.Add(CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }
            if (Schema.Items.Type[0].Name == "object")
            {
                if (Schema.HasDefaultValue())
                {
                    throw new NotImplementedException("Array of Objects has default value");
                }

                if (Schema.Items.Title != null)
                {
                    returnType.CodeType = new CodeTypeReference(Helpers.ParseTitle(Schema.Items.Title) + "[]");
                    returnType.Attributes = null;

                    if (Schema.MinItems != null || Schema.MaxItems != null || Schema.Items.MinLength != -1 || Schema.Items.MaxLength != -1)
                    {
                        throw new NotImplementedException();
                    }
                    return returnType;
                }

                if (Schema.Items.DictionaryValueType != null)
                {
                    throw new NotImplementedException();
                }

                returnType.CodeType = new CodeTypeReference(typeof(object[]));
                returnType.AdditionalMembers.Add(CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }

            throw new NotImplementedException("Array of " + Schema.Items.Type[0].Name);
        }

        public static CodeMemberMethod CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(
           string name, CodeExpression expression)
        {
            return new CodeMemberMethod
            {
                ReturnType = new CodeTypeReference(typeof(bool)),
                Statements =
                {
                    new CodeMethodReturnStatement()
                    {
                        Expression = new CodeBinaryOperatorExpression()
                        {
                            Left = new CodeBinaryOperatorExpression()
                            {
                                Left = new CodeFieldReferenceExpression()
                                {
                                    FieldName = "m_" + name.Substring(0, 1).ToLower() + name.Substring(1)
                                },
                            Operator = CodeBinaryOperatorType.ValueEquality,
                            Right = expression
                            },
                            Operator = CodeBinaryOperatorType.ValueEquality,
                            Right = new CodePrimitiveExpression(false)
                        }
                    }
                },
                Attributes = MemberAttributes.Public | MemberAttributes.Final,
                Name = "ShouldSerialize" + name
            };
        }

        public static CodeMemberMethod CreateMethodThatChecksIfTheArrayOfValueOfAMemberIsNotEqualToAnotherExpression(
           string name, CodeExpression expression)
        {
            return new CodeMemberMethod
            {
                ReturnType = new CodeTypeReference(typeof(bool)),
                Statements =
                {
                    new CodeMethodReturnStatement()
                    {
                        Expression = new CodeBinaryOperatorExpression()
                        {
                            Left = new CodeMethodInvokeExpression(
                                new CodeFieldReferenceExpression() {FieldName = "m_" + name.Substring(0, 1).ToLower() + name.Substring(1)},
                                "SequenceEqual",
                                new CodeExpression[] { expression}
                                )
                            ,
                            Operator = CodeBinaryOperatorType.ValueEquality,
                            Right = new CodePrimitiveExpression(false)
                        }
                    }
                },
                Attributes = MemberAttributes.Public | MemberAttributes.Final,
                Name = "ShouldSerialize" + name
            };
        }
    }
}
