using System.Collections.Generic;

namespace glTFLoader.Schema
{
    public class glTFProperty
    {
        /// <summary>
        /// Dictionary object with extension-specific objects.
        /// </summary>
        public Dictionary<string, object> Extensions { get; set; }

        /// <summary>
        /// Application-specific data.
        /// </summary>
        public object Extras { get; set; }
    }
}
