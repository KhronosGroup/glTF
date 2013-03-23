#ifndef __COLLADA2JSON_SHADERS__
#define __COLLADA2JSON_SHADERS__

#define SHADER_STR(Src) #Src
#define SHADER(Src) SHADER_STR(Src)

namespace JSONExport
{

std::string inferTechniqueName(shared_ptr<JSONObject> technique, GLTFConverterContext& context);

shared_ptr<JSONObject> createReferenceTechniqueBasedOnTechnique(shared_ptr<JSONObject> technique, GLTFConverterContext& context);
};

#endif