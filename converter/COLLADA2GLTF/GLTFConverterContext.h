#ifndef __GLTFConverterContext__
#define __GLTFConverterContext__

namespace GLTF
{
    class GLTFAnimationFlattener;
    
    typedef std::vector <shared_ptr<JSONObject> > AnimatedTargets;
    typedef shared_ptr <AnimatedTargets> AnimatedTargetsSharedPtr;

    typedef shared_ptr <MeshVector> MeshVectorSharedPtr;
    typedef std::map<std::string  , std::string > ShaderIdToShaderString;
    typedef std::map<std::string /* openCOLLADA uniqueID */, GLTFMeshSharedPtr > UniqueIDToMesh;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, MeshVectorSharedPtr > UniqueIDToMeshes;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, COLLADAFW::UniqueId > MaterialUIDToEffectUID;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFEffect> > UniqueIDToEffect;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFAnimation> > UniqueIDToAnimation;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFSkin> > UniqueIDToSkin;
    typedef std::map<std::string /* openCOLLADA uniqueID */, shared_ptr<JSONObject> > UniqueIDToNode;
    typedef std::map<std::string /* openCOLLADA uniqueID from AnimationList*/, AnimatedTargetsSharedPtr > UniqueIDToAnimatedTargets;
    typedef std::map<std::string  , std::string > ImageIdToImagePath;
    typedef std::map<std::string , shared_ptr<JSONObject> > UniqueIDToTrackedObject;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDToParentsOfInstanceNode;
    typedef std::map<std::string , shared_ptr<GLTFAnimationFlattener> > UniqueIDToAnimationFlattener;
    typedef std::map<std::string , unsigned int> SamplerHashtoSamplerIndex;
    
    //TODO: cleanup
    //For now, GLTFConverterContext is just struct, but it is growing and may become eventually a class
    typedef struct
    {
        std::string inputFilePath;
        std::string outputFilePath;

        //TODO: add options here
        bool invertTransparency;
        bool exportAnimations;
        bool exportPassDetails;
        bool outputProgress;
        
        shared_ptr <GLTF::JSONObject> root;
        ShaderIdToShaderString shaderIdToShaderString;
        UniqueIDToMesh _uniqueIDToMesh;
        UniqueIDToMeshes _uniqueIDToMeshes;
        UniqueIDToEffect _uniqueIDToEffect;
        MaterialUIDToEffectUID _materialUIDToEffectUID;
        MaterialUIDToName _materialUIDToName;
        ImageIdToImagePath _imageIdToImagePath;
        UniqueIDToAnimation _uniqueIDToAnimation;
        UniqueIDToAnimatedTargets _uniqueIDToAnimatedTargets;
        UniqueIDToTrackedObject _uniqueIDToTrackedObject;
        UniqueIDToSkin _uniqueIDToSkin;
        UniqueIDToNode _uniqueIDToNode;
        UniqueIDToParentsOfInstanceNode _uniqueIDToParentsOfInstanceNode;
        UniqueIDToAnimationFlattener _uniqueIDToAnimationFlattener;
        SamplerHashtoSamplerIndex _samplerHashtoSamplerIndex;
        
    } GLTFConverterContext;

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif