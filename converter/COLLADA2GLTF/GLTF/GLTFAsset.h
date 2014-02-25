#ifndef __GLTFAsset__
#define __GLTFAsset__

#include <stdarg.h>
#include "../GLTF-OpenCOLLADA.h"
#include "GLTFAccessorCache.h"

namespace GLTF
{
#define CONFIG_BOOL(X) (asset.converterConfig()->config()->getBool(X))
#define CONFIG_STRING(X) (asset.converterConfig()->config()->getString(X))
#define CONFIG_DOUBLE(X) (asset.converterConfig()->config()->getDouble(X))
#define CONFIG_INT32(X) (asset.converterConfig()->config()->getInt32(X))
#define CONFIG_UINT32(X) (asset.converterConfig()->config()->getUInt32(X))

    const std::string kVerticesOutputStream = "vertices";
    const std::string kIndicesOutputStream = "indices";
    const std::string kAnimationOutputStream = "animations";
    const std::string kCompressionOutputStream = "compression";
    
    class GLTFAnimationFlattener;
    
    typedef std::vector <shared_ptr<JSONObject> > AnimatedTargets;
    typedef shared_ptr <AnimatedTargets> AnimatedTargetsSharedPtr;

    typedef shared_ptr <MeshVector> MeshVectorSharedPtr;
    typedef std::map<std::string  , std::string > ShaderIdToShaderString;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, MeshVectorSharedPtr > UniqueIDToMeshes;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, COLLADAFW::UniqueId > MaterialUIDToEffectUID;
    typedef std::map<std::string , unsigned int> SamplerHashtoSamplerIndex;
    
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<std::string /* openCOLLADA uniqueID from AnimationList*/, AnimatedTargetsSharedPtr > UniqueIDToAnimatedTargets;
    typedef std::map<std::string  , std::string > ImageIdToImagePath;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDToParentsOfInstanceNode;
    typedef std::map<std::string , shared_ptr<GLTFAnimationFlattener> > UniqueIDToAnimationFlattener;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDTOfLightToNodes;
    
    typedef std::map<std::string , shared_ptr<JSONObject> > OriginalIDToTrackedObject;
    typedef std::map<std::string , std::string > UniqueIDToOriginalUID;
    typedef std::map<std::string , shared_ptr<JSONObject> > UniqueIDToJSONObject;

    //TODO: these to be moved as UniqueIDToJSONObject
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFEffect> > UniqueIDToEffect;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFAnimation> > UniqueIDToAnimation;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFSkin> > UniqueIDToSkin;
    typedef std::map<std::string /* openCOLLADA uniqueID */, GLTFMeshSharedPtr > UniqueIDToMesh;
    
    typedef std::map<GLTFAccessorCache , std::string> UniqueIDToAccessor;

    //should be outside of asset
    typedef std::map<std::string , shared_ptr <COLLADAFW::Object> > UniqueIDToOpenCOLLADAObject;
    
    typedef std::map<std::string , shared_ptr<GLTFOutputStream> > NameToOutputStream;
    
    class GLTFAsset
    {
    public:
        GLTFAsset();
        
        shared_ptr <GLTFConfig> converterConfig();
        shared_ptr <JSONObject> convertionResults();
        
        void write();
        
        const std::string resourceOuputPathForPath(const std::string& resourcePath);

        shared_ptr<GLTFOutputStream> createOutputStreamIfNeeded(const std::string& streamName);
        void closeOutputStream(const std::string& streamName, bool removeFile);
        
        void log(const char * format, ... );

        void setBundleOutputPath(const std::string& bundleOutputPath);
        const std::string& getBundleOutputPath();
        
        void setGeometryByteLength(size_t geometryByteLength);
        size_t getGeometryByteLength();
        
        void setAnimationByteLength(size_t animationByteLength);
        size_t getAnimationByteLength();
        
        void setOutputFilePath(const std::string& outputFilePath);
        std::string getOutputFilePath();
        
        void setInputFilePath(const std::string& inputFilePath);
        std::string getInputFilePath();
        
        std::string pathRelativeToInputPath(const std::string& path);
        void copyImagesInsideBundleIfNeeded();
        
        void prepareForProfile(shared_ptr<GLTFProfile> profile);

    public:
        shared_ptr <GLTFProfile> profile;
        shared_ptr <JSONObject> root;

        ShaderIdToShaderString shaderIdToShaderString;
        UniqueIDToMesh _uniqueIDToMesh;
        UniqueIDToMeshes _uniqueIDToMeshes;
        UniqueIDToEffect _uniqueIDToEffect;
        UniqueIDToEffect _uniqueIDToDefaultEffect;
        MaterialUIDToEffectUID _materialUIDToEffectUID;
        MaterialUIDToName _materialUIDToName;
        UniqueIDToAnimation _uniqueIDToAnimation;
        UniqueIDToAnimatedTargets _uniqueIDToAnimatedTargets;
        OriginalIDToTrackedObject _originalIDToTrackedObject;
        UniqueIDToSkin _uniqueIDToSkin;
        UniqueIDToParentsOfInstanceNode _uniqueIDToParentsOfInstanceNode;
        SamplerHashtoSamplerIndex _samplerHashtoSamplerIndex;
        UniqueIDTOfLightToNodes _uniqueIDOfLightToNodes;
        
        UniqueIDToJSONObject _uniqueIDToJSONObject;

        UniqueIDToOriginalUID _uniqueIDToOriginalID;
        UniqueIDToOpenCOLLADAObject _uniqueIDToOpenCOLLADAObject;
        FlattenerMapsForAnimationID _flattenerMapsForAnimationID;
        UniqueIDToAccessor _uniqueIDToAccessorObject;
        
        UniqueIDToAnimationFlattener    _targetUIDWithPathToAnimationFlattener;
    private:
        shared_ptr <GLTFConfig>         _converterConfig;
        shared_ptr <JSONObject>         _convertionResults;
        shared_ptr<JSONObject>          _trackedResourcesPath;
        shared_ptr<JSONObject>          _trackedOutputResourcesPath;
        unsigned int                    _prefix;
        std::string                     _inputFilePath;
        std::string                     _outputFilePath;
        size_t                          _geometryByteLength;
        size_t                          _animationByteLength;
        std::string                     _bundleOutputPath;
        bool                            _isBundle;
        
        NameToOutputStream _nameToOutputStream;
        GLTF::GLTFWriter _writer;
    };

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif