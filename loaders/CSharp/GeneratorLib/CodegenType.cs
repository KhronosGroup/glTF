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

        public CodeAttributeDeclarationCollection Attributes { get; set; }

        public CodeExpression DefaultValue { get; set; }

        public CodeTypeDeclaration DependentType { get; set; }

        public CodeMemberMethod ShouldSerializeMethod { get; set; }
    }
}
