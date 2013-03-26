#ifndef __COLLADA2GLTF_SHADERS__
#define __COLLADA2GLTF_SHADERS__

#define SHADER_STR(Src) #Src
#define SHADER(Src) SHADER_STR(Src)

namespace GLTF
{
    std::string getReferenceTechniqueID(shared_ptr<JSONObject> technique, std::vector<shared_ptr<JSONObject> > &texcoordBindings, GLTFConverterContext& context);
};

#endif