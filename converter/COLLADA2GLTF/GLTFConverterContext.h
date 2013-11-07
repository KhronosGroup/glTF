#ifndef __GLTFConverterContext__
#define __GLTFConverterContext__

#include "GLTF-OpenCOLLADA.h"

namespace GLTF
{
#define CONFIG_BOOL(X) (converterContext.converterConfig()->config()->getBool(X))
#define CONFIG_STRING(X) (converterContext.converterConfig()->config()->getString(X))
#define CONFIG_DOUBLE(X) (converterContext.converterConfig()->config()->getDouble(X))
#define CONFIG_INT32(X) (converterContext.converterConfig()->config()->getInt32(X))
#define CONFIG_UINT32(X) (converterContext.converterConfig()->config()->getUInt32(X))

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
    typedef std::map<std::string , shared_ptr<JSONObject> > OriginalIDToTrackedObject;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDToParentsOfInstanceNode;
    typedef std::map<std::string , shared_ptr<GLTFAnimationFlattener> > UniqueIDToAnimationFlattener;
    typedef std::map<std::string , unsigned int> SamplerHashtoSamplerIndex;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDTOfLightToNodes;
    typedef std::map<std::string , shared_ptr<JSONObject> > UniqueIDToLight;
    typedef std::map<std::string , std::string > UniqueIDToOriginalUID;
    typedef std::map<std::string , shared_ptr <COLLADAFW::Object> > UniqueIDToOpenCOLLADAObject;
    
    //TODO: cleanup, this was previously a typedef. It will be mostly reworked thanks to the GLTFConfig class.
    class GLTFConverterContext
    {
    public:
        GLTFConverterContext() {
            this->_converterConfig = shared_ptr<GLTFConfig> (new GLTFConfig());
        }
        
        shared_ptr <GLTFConfig> converterConfig() {
            return this->_converterConfig;
        }
        
    public:
        std::string inputFilePath;
        std::string outputFilePath;
        
        shared_ptr <GLTFProfile> profile;
        shared_ptr <JSONObject> root;
        
        ShaderIdToShaderString shaderIdToShaderString;
        UniqueIDToMesh _uniqueIDToMesh;
        UniqueIDToMeshes _uniqueIDToMeshes;
        UniqueIDToEffect _uniqueIDToEffect;
        MaterialUIDToEffectUID _materialUIDToEffectUID;
        MaterialUIDToName _materialUIDToName;
        UniqueIDToAnimation _uniqueIDToAnimation;
        UniqueIDToAnimatedTargets _uniqueIDToAnimatedTargets;
        OriginalIDToTrackedObject _originalIDToTrackedObject;
        UniqueIDToSkin _uniqueIDToSkin;
        UniqueIDToNode _uniqueIDToNode;
        UniqueIDToParentsOfInstanceNode _uniqueIDToParentsOfInstanceNode;
        SamplerHashtoSamplerIndex _samplerHashtoSamplerIndex;
        UniqueIDTOfLightToNodes _uniqueIDOfLightToNodes;
        UniqueIDToLight _uniqueIDToLight;
        UniqueIDToOriginalUID _uniqueIDToOriginalID;
        
        UniqueIDToOpenCOLLADAObject _uniqueIDToOpenCOLLADAObject;
        
        FlattenerMapsForAnimationID _flattenerMapsForAnimationID;
        
        GLTFOutputStream *_vertexOutputStream;
        GLTFOutputStream *_indicesOutputStream;
        GLTFOutputStream *_animationOutputStream;
        GLTFOutputStream *_compressionOutputStream;
        
        size_t _geometryByteLength;
        size_t _animationByteLength;
        
    private:
        shared_ptr <GLTFConfig> _converterConfig;

    } ;

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif