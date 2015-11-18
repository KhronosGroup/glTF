using System;
using System.Collections.Generic;
using System.Net;
using System.Xml;
using NUnit.Framework;

namespace GeneratorUnitTests
{
    [TestFixture]
    public class EnumTest
    {
        private readonly Dictionary<long, string> m_enumMap = new Dictionary<long, string>();
        
        [Test]
        public void ParseTest()
        {
            var spec = new XmlDocument();
            spec.LoadXml(new WebClient().DownloadString("https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/gl.xml"));
            ExtractEnumValues(spec);
            Assert.AreEqual(3638, m_enumMap.Count);
        }

        private void ExtractEnumValues(XmlNode parentNode)
        {
            foreach (var nodeObject in parentNode)
            {
                var node = (XmlNode)nodeObject;
                ExtractEnumValues(node);
                if (node.Name == "enum" && node.Attributes?.Count >=2)
                {
                    string name = null;
                    long? value = null;
                    foreach (var attributeObject in node.Attributes)
                    {
                        var attribute = (XmlAttribute) attributeObject;
                        if (attribute.Name == "value")
                        {
                            long result;
                            value = long.TryParse(attribute.Value, out result) ? result : Convert.ToInt64(attribute.Value, 16);
                        }

                        if (attribute.Name == "name")
                        {
                            name = attribute.Value;
                        }
                    }

                    if (name != null && value != null)
                    {
                        m_enumMap[value.Value] = name;
                    }
                }
            }
        }
    }
}
