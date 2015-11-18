using System;
using System.Collections.Generic;
using System.Net;
using System.Xml;
using GeneratorLib;
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
            CodegenTypeFactory.ExtractEnumValues(m_enumMap, spec);
            Assert.AreEqual(3638, m_enumMap.Count);
            Assert.AreEqual("BYTE", m_enumMap[5120]);
        }
    }
}
