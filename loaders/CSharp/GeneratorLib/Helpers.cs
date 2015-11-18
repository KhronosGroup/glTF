using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace GeneratorLib
{
    public static class Helpers
    {
        public static bool HasDefaultValue(this Schema schema)
        {
            return schema.Default != null &&
                   (
                       (schema.Default is JObject && ((JObject) schema.Default).Count > 0) ||
                       (schema.Default is JArray && ((JArray)schema.Default).Count > 0) ||
                       (schema.Default is int) ||
                       (schema.Default is long) ||
                       (schema.Default is float) ||
                       (schema.Default is double) ||
                       (schema.Default is string) ||
                       (schema.Default is bool)
                   );
        }

        public static string TrimLeftSubstring(this string s, string substringToRemove)
        {
            return s.StartsWith(substringToRemove) ? s.Substring(substringToRemove.Length) : s;
        }

        public static string TrimRightSubstring(this string s, string substringToRemove)
        {
            return s.EndsWith(substringToRemove) ? s.Substring(0, s.Length - substringToRemove.Length) : s;
        }

        public static string ParseTitle(string rawTilte)
        {
            var words = rawTilte.ToLower().Split(' ');
            string retval = "";
            foreach (var word in words)
            {
                retval += word[0].ToString().ToUpper();
                retval += word.Substring(1);
            }
            return retval;
        }
    }
}
