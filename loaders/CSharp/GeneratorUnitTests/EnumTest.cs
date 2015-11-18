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
        private Dictionary<long, string> EnumMap = new Dictionary<long, string>();
        
        [Test]
        public void ParseTest()
        {
            var spec = new XmlDocument();
            spec.LoadXml(new WebClient().DownloadString("https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/gl.xml"));
            ExtractEnumValues(spec);
            Assert.AreEqual(3638, EnumMap.Count);
        }

        private void ExtractEnumValues(XmlNode parentNode)
        {
            foreach (var nodeObject in parentNode)
            {
                var node = (XmlNode)nodeObject;
                ExtractEnumValues(node);
                if (node.Name == "enum" && node.Attributes.Count >=2)
                {
                    string name = null;
                    long? value = null;
                    foreach (var attributeObject in node.Attributes)
                    {
                        var attribute = (XmlAttribute) attributeObject;
                        if (attribute.Name == "value")
                        {
                            long result;
                            if (long.TryParse(attribute.Value, out result))
                            {
                                value = result;
                            }
                            else
                            {
                                value = Convert.ToInt64(attribute.Value, 16);
                            }
                        }

                        if (attribute.Name == "name")
                        {
                            name = attribute.Value;
                        }
                    }

                    if (name != null && value != null)
                    {
                        EnumMap[value.Value] = name;
                    }
                }
            }
        }
    }
}
