using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GeneratorLib
{
    public class CodegenType
    {
        public CodeTypeReference CodeType { get; set; }

        public CodeAttributeDeclarationCollection Attributes { get; } = new CodeAttributeDeclarationCollection();

        public CodeExpression DefaultValue { get; set; }

        public CodeTypeMemberCollection AdditionalMembers { get; } = new CodeTypeMemberCollection();

        public CodeStatementCollection SetStatements { get; } = new CodeStatementCollection();
    }
}
