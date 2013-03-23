#ifndef __GLTFConverterContext__
#define __GLTFConverterContext__

#include "COLLADAFWUniqueId.h"

namespace GLTF
{
    typedef shared_ptr <MeshVector> MeshVectorSharedPtr;
    typedef std::map<std::string  , std::string > ShaderIdToShaderString;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, MeshVectorSharedPtr > UniqueIDToMeshes;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, unsigned int /* effectID */ > MaterialUIDToEffectUID;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFEffect> > UniqueIDToEffect;
    typedef std::map<std::string  , std::string > ImageIdToImagePath;
    
    //TODO: cleanup
    //For now, GLTFConverterContext is just struct, but it is growing and may become eventually a class
    typedef struct
    {
        std::string inputFilePath;
        std::string outputFilePath;
        bool invertTransparency;
        
        //TODO: add options here
        shared_ptr <GLTF::JSONObject> root;
        ShaderIdToShaderString shaderIdToShaderString;        
        UniqueIDToMeshes _uniqueIDToMeshes;
        UniqueIDToEffect _uniqueIDToEffect;
        MaterialUIDToEffectUID _materialUIDToEffectUID;
        MaterialUIDToName _materialUIDToName;
        ImageIdToImagePath _imageIdToImagePath;
    } GLTFConverterContext;

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif