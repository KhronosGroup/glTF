#ifndef __COLLADA2JSON_SHADERS__
#define __COLLADA2JSON_SHADERS__

#define SHADER_STR(Src) #Src
#define SHADER(Src) SHADER_STR(Src)

namespace JSONExport
{

std::string getTechniqueNameForProfile(const COLLADAFW::EffectCommon* effectCommon, JSONExport::COLLADA2JSONContext& context);

shared_ptr <JSONExport::JSONObject> createTechniqueForProfile(const COLLADAFW::EffectCommon* effectCommon, JSONExport::COLLADA2JSONContext& context);
};

#endif