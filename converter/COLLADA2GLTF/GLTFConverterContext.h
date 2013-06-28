#ifndef __GLTFConverterContext__
#define __GLTFConverterContext__

namespace GLTF
{
    typedef std::vector <shared_ptr<JSONObject> > AnimatedTargets;
    typedef shared_ptr <AnimatedTargets> AnimatedTargetsSharedPtr;

    typedef shared_ptr <MeshVector> MeshVectorSharedPtr;
    typedef std::map<std::string  , std::string > ShaderIdToShaderString;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, MeshVectorSharedPtr > UniqueIDToMeshes;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, COLLADAFW::UniqueId /* effectID */ > MaterialUIDToEffectUID;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFEffect> > UniqueIDToEffect;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFAnimation> > UniqueIDToAnimation;
    typedef std::map<unsigned int /* openCOLLADA uniqueID from AnimationList*/, AnimatedTargetsSharedPtr > UniqueIDToAnimatedTargets;
    typedef std::map<std::string  , std::string > ImageIdToImagePath;
    typedef std::map<std::string , shared_ptr<JSONObject> > UniqueIDToTrackedObject;

    //TODO: cleanup
    //For now, GLTFConverterContext is just struct, but it is growing and may become eventually a class
    typedef struct
    {
        std::string inputFilePath;
        std::string outputFilePath;
        bool invertTransparency;
        bool exportAnimations;
        bool exportPassDetails;
        
        //TODO: add options here
        shared_ptr <GLTF::JSONObject> root;
        ShaderIdToShaderString shaderIdToShaderString;        
        UniqueIDToMeshes _uniqueIDToMeshes;
        UniqueIDToEffect _uniqueIDToEffect;
        MaterialUIDToEffectUID _materialUIDToEffectUID;
        MaterialUIDToName _materialUIDToName;
        ImageIdToImagePath _imageIdToImagePath;
        UniqueIDToAnimation _uniqueIDToAnimation;
        UniqueIDToAnimatedTargets _uniqueIDToAnimatedTargets;
        UniqueIDToTrackedObject _uniqueIDToTrackedObject;

    } GLTFConverterContext;

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif