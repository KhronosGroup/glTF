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
                                new CodeAttributeArgument(new CodeTypeOfExpression(typeof(ArrayConverter)))
                            }
                        )
                    }
            };

            if (Schema.Items.Type.Length > 1)
            {
                returnType.CodeType = new CodeTypeReference(typeof(object[]));
                returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }

            EnforceRestrictionsOnSetValues(returnType, name, Schema);

            if (Schema.Items.Type[0].Name == "integer")
            {
                if (Schema.Items.Enum != null)
                {
                    var enumType = SingleValueCodegenTypeFactory.GenIntEnumType(name, Schema.Items);
                    returnType.AdditionalMembers.Add(enumType);

                    if (Schema.HasDefaultValue())
                    {
                        var defaultValueArray =((JArray) Schema.Default).Select(x =>(CodeExpression)SingleValueCodegenTypeFactory.GetEnumField(enumType, (int) (long) x)).ToArray();
                        returnType.DefaultValue = new CodeArrayCreateExpression(enumType.Name, defaultValueArray);
                        returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheArrayOfValueOfAMemberIsNotEqualToAnotherExpression(name, returnType.DefaultValue));
                    }
                    else
                    {
                        returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null))); 
                    }

                    returnType.CodeType = new CodeTypeReference(enumType.Name + "[]");
                    return returnType;
                }

                if (Schema.HasDefaultValue())
                {
                    var defaultValueArray =
                        ((JArray) Schema.Default).Select(
                            x => (CodeExpression) new CodePrimitiveExpression((int) (long) x)).ToArray();
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof (int), defaultValueArray);
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheArrayOfValueOfAMemberIsNotEqualToAnotherExpression(name, returnType.DefaultValue));
                }
                else
                {
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                }
                returnType.CodeType = new CodeTypeReference(typeof(int[]));

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
                    var defaultVauleArray = (JArray) Schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof (float),
                        defaultVauleArray.Select(x => (CodeExpression) new CodePrimitiveExpression((float) x)).ToArray());
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheArrayOfValueOfAMemberIsNotEqualToAnotherExpression(name, returnType.DefaultValue));
                }
                else
                {
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                }

                returnType.CodeType = new CodeTypeReference(typeof(float[]));

                return returnType;
            }

            if (Schema.Items.Minimum != null || Schema.Items.Maximum != null)
            {
                throw new NotImplementedException();
            }

            if (Schema.Items.Type[0].Name == "boolean")
            {
                if (Schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray) Schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof (bool),
                        defaultVauleArray.Select(x => (CodeExpression) new CodePrimitiveExpression((bool) x)).ToArray());
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheArrayOfValueOfAMemberIsNotEqualToAnotherExpression(name, returnType.DefaultValue));
                }
                else
                {
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                }
                returnType.CodeType = new CodeTypeReference(typeof(bool[]));
                return returnType;
            }
            if (Schema.Items.Type[0].Name == "string")
            {
                if (Schema.HasDefaultValue())
                {
                    var defaultVauleArray = (JArray) Schema.Default;
                    returnType.DefaultValue = new CodeArrayCreateExpression(typeof (string),
                        defaultVauleArray.Select(x => (CodeExpression) new CodePrimitiveExpression((string) x))
                            .ToArray());
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheArrayOfValueOfAMemberIsNotEqualToAnotherExpression(name, returnType.DefaultValue));
                }
                else
                {
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                }
                returnType.CodeType = new CodeTypeReference(typeof(string[]));

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
                    returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                    returnType.Attributes = null;

                    if (Schema.MinItems != null || Schema.MaxItems != null || Schema.Items.MinLength != null || Schema.Items.MaxLength != null)
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
                returnType.AdditionalMembers.Add(Helpers.CreateMethodThatChecksIfTheValueOfAMemberIsNotEqualToAnotherExpression(name, new CodePrimitiveExpression(null)));
                return returnType;
            }

            throw new NotImplementedException("Array of " + Schema.Items.Type[0].Name);
        }

        private static void EnforceRestrictionsOnSetValues(CodegenType returnType, string name, Schema schema)
        {
            if (schema.MinItems != null)
            {
                returnType.SetStatements.Add(new CodeConditionStatement
                {
                    Condition = new CodeBinaryOperatorExpression
                    {
                        Left = new CodePropertyReferenceExpression(new CodePropertySetValueReferenceExpression(), "Length"),
                        Operator = CodeBinaryOperatorType.LessThan,
                        Right = new CodePrimitiveExpression(schema.MinItems)
                    },
                    TrueStatements =
                    {
                        new CodeThrowExceptionStatement(new CodeObjectCreateExpression(typeof(ArgumentException), new CodeExpression[] { new CodePrimitiveExpression("Array not long enough") } ))
                    }
                });
            }

            if (schema.MaxItems != null)
            {
                returnType.SetStatements.Add(new CodeConditionStatement
                {
                    Condition = new CodeBinaryOperatorExpression
                    {
                        Left = new CodePropertyReferenceExpression(new CodePropertySetValueReferenceExpression(), "Length"),
                        Operator = CodeBinaryOperatorType.GreaterThan,
                        Right = new CodePrimitiveExpression(schema.MaxItems)
                    },
                    TrueStatements =
                    {
                        new CodeThrowExceptionStatement(new CodeObjectCreateExpression(typeof(ArgumentException), new CodeExpression[] { new CodePrimitiveExpression("Array too long") } ))
                    }
                });
            }

            if (schema.Items.Minimum != null || schema.Items.Maximum != null || schema.Items.MinLength != null || schema.Items.MaxLength != null)
            {
                returnType.SetStatements.Add(new CodeVariableDeclarationStatement(typeof(int), "index", new CodePrimitiveExpression(0)));
            }

            if (schema.Items.Minimum != null)
            {
                returnType.SetStatements.Add(LoopThroughArray(new CodePropertySetValueReferenceExpression(), new CodeStatementCollection
                {
                    new CodeConditionStatement
                    {
                        Condition = new CodeBinaryOperatorExpression
                        {
                            Left = new CodeArrayIndexerExpression
                            {
                                TargetObject = new CodePropertySetValueReferenceExpression(),
                                Indices = { new CodeVariableReferenceExpression("index") },
                            },
                            Operator = schema.Items.ExclusiveMinimum ? CodeBinaryOperatorType.LessThanOrEqual : CodeBinaryOperatorType.LessThan,
                            Right = new CodePrimitiveExpression(schema.Items.Minimum)
                        },
                        TrueStatements = { new CodeThrowExceptionStatement(new CodeObjectCreateExpression(typeof(ArgumentOutOfRangeException))) }
                    }
                }));
            }

            if (schema.Items.Maximum != null)
            {
                returnType.SetStatements.Add(LoopThroughArray(new CodePropertySetValueReferenceExpression(), new CodeStatementCollection
                {
                    new CodeConditionStatement
                    {
                        Condition = new CodeBinaryOperatorExpression
                        {
                            Left = new CodeArrayIndexerExpression
                            {
                                TargetObject = new CodePropertySetValueReferenceExpression(),
                                Indices = { new CodeVariableReferenceExpression("index") },
                            },
                            Operator = schema.Items.ExclusiveMaximum ? CodeBinaryOperatorType.GreaterThanOrEqual : CodeBinaryOperatorType.GreaterThan,
                            Right = new CodePrimitiveExpression(schema.Items.Maximum)
                        },
                        TrueStatements = { new CodeThrowExceptionStatement(new CodeObjectCreateExpression(typeof(ArgumentOutOfRangeException))) }
                    }
                }));
            }

            if (schema.Items.MinLength != null)
            {
                returnType.SetStatements.Add(LoopThroughArray(new CodePropertySetValueReferenceExpression(), new CodeStatementCollection
                {
                    new CodeConditionStatement
                    {
                        Condition = new CodeBinaryOperatorExpression
                        {
                            Left = new CodePropertyReferenceExpression(new CodeArrayIndexerExpression
                            {
                                TargetObject = new CodePropertySetValueReferenceExpression(),
                                Indices = { new CodeVariableReferenceExpression("index") },
                            }, "Length"),
                            Operator = CodeBinaryOperatorType.LessThan,
                            Right = new CodePrimitiveExpression(schema.Items.MinLength)
                        },
                        TrueStatements = { new CodeThrowExceptionStatement(new CodeObjectCreateExpression(typeof(ArgumentException))) }
                    }
                }));
            }

            if (schema.Items.MaxLength != null)
            {
                returnType.SetStatements.Add(LoopThroughArray(new CodePropertySetValueReferenceExpression(), new CodeStatementCollection
                {
                    new CodeConditionStatement
                    {
                        Condition = new CodeBinaryOperatorExpression
                        {
                            Left = new CodePropertyReferenceExpression(new CodeArrayIndexerExpression
                            {
                                TargetObject = new CodePropertySetValueReferenceExpression(),
                                Indices = { new CodeVariableReferenceExpression("index") },
                            }, "Length"),
                            Operator = CodeBinaryOperatorType.GreaterThan,
                            Right = new CodePrimitiveExpression(schema.Items.MaxLength)
                        },
                        TrueStatements = { new CodeThrowExceptionStatement(new CodeObjectCreateExpression(typeof(ArgumentException))) }
                    }
                }));
            }
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

        private static CodeStatement LoopThroughArray(CodeExpression array, CodeStatementCollection statements)
        {
            var returnValue =  new CodeIterationStatement
            {
                InitStatement =
                    new CodeAssignStatement(new CodeVariableReferenceExpression("index"), new CodePrimitiveExpression(0)),
                TestExpression = new CodeBinaryOperatorExpression
                {
                    Left = new CodeVariableReferenceExpression("index"),
                    Operator = CodeBinaryOperatorType.LessThan,
                    Right = new CodePropertyReferenceExpression
                    {
                        PropertyName = "Length",
                        TargetObject = array
                    }
                },
                IncrementStatement = new CodeAssignStatement
                {
                    Left = new CodeVariableReferenceExpression("index"),
                    Right = new CodeBinaryOperatorExpression
                    {
                        Left = new CodeVariableReferenceExpression("index"),
                        Operator = CodeBinaryOperatorType.Add,
                        Right = new CodePrimitiveExpression(1)
                    }
                }
            };
            returnValue.Statements.AddRange(statements);
            return returnValue;
        }
    }
}
