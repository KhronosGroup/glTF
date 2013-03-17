
#include <string>
#include <algorithm>

#include "COLLADASaxFWLLoader.h"

#include "COLLADABUStableHeaders.h"
#include "COLLADABUNativeString.h"
#include "COLLADABUStringUtils.h"

#include "COLLADAFWRoot.h"
#include "COLLADAFWGeometry.h"
#include "COLLADAFWCamera.h"
#include "COLLADAFWMesh.h"
#include "COLLADAFWMeshVertexData.h"
#include "COLLADAFWNode.h"
#include "COLLADAFWVisualScene.h"
#include "COLLADAFWInstanceGeometry.h"
#include "COLLADAFWInstanceCamera.h"
#include "COLLADAFWMaterial.h"
#include "COLLADAFWEffect.h"
#include "COLLADAFWImage.h"
#include "COLLADABUURI.h"

#include "JSONExport.h"

#include "GLTFConverterContext.h"

namespace JSONExport
{
    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId)
    {
        std::string id = "";
        id += type + "_" + JSONExport::JSONUtils::toString(uniqueId.getObjectId());
        return id;
    }

}