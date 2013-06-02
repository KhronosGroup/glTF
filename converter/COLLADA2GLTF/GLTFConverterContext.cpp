#include "GLTF.h"
#include "GLTF-OpenCOLLADA.h"
#include "GLTFConverterContext.h"

namespace GLTF
{
    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId)
    {
        std::string id = "";
        id += type + "_" + GLTF::GLTFUtils::toString(uniqueId.getObjectId());
        return id;
    }

}