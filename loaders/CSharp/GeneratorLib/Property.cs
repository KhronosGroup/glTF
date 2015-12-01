using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GeneratorLib
{
    public class Property
    {
        private Schema m_schema;

        public Property(Schema schema)
        {
            m_schema = schema;
        }

        public string Serialize(string name)
        {
            return $"{GetDocumentationHeader()}\npublic {GetCSharpType()} {{ get; set; }}";
        }

        private string GetCSharpType()
        {
            if (m_schema.Type == null || m_schema.Type.Length < 1)
            {
                throw new ArgumentException("No type defined. Cannot be a property.");
            }

            if (m_schema.Type.Length > 1)
            {
                return "object";
            }
            else if (m_schema.Type[0].IsReference == false)
            {
                if (m_schema.Type[0].Name == "any")
                {
                    return "object";
                }
                else
                {
                    throw new NotImplementedException();
                }
            }
            else
            {
                throw new NotImplementedException();
            }
        }

        private string GetDocumentationHeader()
        {
            var description = string.IsNullOrWhiteSpace(m_schema.Description)
                ? string.Empty
                : $"\n/// {m_schema.Description}";
            var detailedDescription = string.IsNullOrWhiteSpace(m_schema.DetailedDescription)
                ? string.Empty
                : $"\n/// {m_schema.DetailedDescription}";
            var webGl = string.IsNullOrWhiteSpace(m_schema.WebGl)
                ? string.Empty
                : $"\n/// {m_schema.WebGl}";

            return $"/// <summary>{description}{detailedDescription}{webGl}\n/// </summary>";
        }
    }
}
