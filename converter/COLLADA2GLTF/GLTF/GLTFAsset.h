#ifndef __GLTFAsset__
#define __GLTFAsset__

#include <stdarg.h>
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAccessorCache.h"

namespace GLTF
{
#define CONFIG_BOOL(X) (asset->converterConfig()->config()->getBool(X))
#define CONFIG_STRING(X) (asset->converterConfig()->config()->getString(X))
#define CONFIG_DOUBLE(X) (asset->converterConfig()->config()->getDouble(X))
#define CONFIG_INT32(X) (asset->converterConfig()->config()->getInt32(X))
#define CONFIG_UINT32(X) (asset->converterConfig()->config()->getUInt32(X))

    const std::string kRawOutputStream = "rawOutputStream";
    const std::string kCompressionOutputStream = "compression";
    
    class GLTFAnimationFlattener;
    
    typedef std::vector <shared_ptr<JSONObject> > AnimatedTargets;
    typedef shared_ptr <AnimatedTargets> AnimatedTargetsSharedPtr;

    typedef std::map<std::string  , std::string > ShaderIdToShaderString;
    typedef std::map<std::string /* openCOLLADA uniqueID */, COLLADAFW::UniqueId > MaterialUIDToEffectUID;
    typedef std::map<std::string , unsigned int> SamplerHashtoSamplerIndex;
    
    typedef std::map<std::string /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<std::string /* openCOLLADA uniqueID from AnimationList*/, AnimatedTargetsSharedPtr > UniqueIDToAnimatedTargets;
    typedef std::map<std::string  , std::string > ImageIdToImagePath;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDToParentsOfInstanceNode;
    typedef std::map<std::string , shared_ptr<GLTFAnimationFlattener> > UniqueIDToAnimationFlattener;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDTOfLightToNodes;
    
    typedef std::map<std::string , shared_ptr<JSONObject> > OriginalIDToTrackedObject;
    typedef std::map<std::string , std::string > UniqueIDToOriginalUID;
    typedef std::map<std::string , shared_ptr<JSONValue> > UniqueIDToJSONValue;

    typedef std::map<GLTFAccessorCache , std::string> UniqueIDToAccessor;

    typedef std::map<std::string , shared_ptr <COLLADAFW::Object> > UniqueIDToOpenCOLLADAObject;
    
    typedef std::map<std::string , shared_ptr<GLTFOutputStream> > NameToOutputStream;
    
    /* Abstract
     *
     *
     */
    class GLTFAssetModifier {
    public:
        GLTFAssetModifier();
        virtual ~GLTFAssetModifier();
        
        virtual bool init() { return true; };
        virtual void modify(shared_ptr<JSONObject> glTFAsset) = 0;
        virtual const std::string& modifierId() = 0;
        virtual void cleanup() { };
    };
    
    class GLTFAsset
    {
    public:
        GLTFAsset();
        
        shared_ptr <GLTFConfig> converterConfig();
        shared_ptr <JSONObject> convertionResults();

        shared_ptr <GLTFProfile> profile();
        shared_ptr <JSONObject> root();

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
        
        void setValueForUniqueId(const std::string& uniqueId, shared_ptr<JSONValue> obj);
        shared_ptr<JSONValue> getValueForUniqueId(const std::string& uniqueId);
        bool containsValueForUniqueId(const std::string& uniqueId);
        
        void prepareForProfile(shared_ptr<GLTFProfile> profile);

        std::string getSharedBufferId();

        std::vector <shared_ptr<GLTFAssetModifier> > &assetModifiers() { return this->_assetModifiers; };
    
    //TODO: all those still need cleanup and should be moved to private
    protected:
        void launchModifiers();
    public:
        MaterialUIDToEffectUID          _materialUIDToEffectUID;
        MaterialUIDToName               _materialUIDToName;
        UniqueIDToAnimatedTargets       _uniqueIDToAnimatedTargets;
        OriginalIDToTrackedObject       _originalIDToTrackedObject;
        UniqueIDToParentsOfInstanceNode _uniqueIDToParentsOfInstanceNode;
        SamplerHashtoSamplerIndex       _samplerHashtoSamplerIndex;
        UniqueIDTOfLightToNodes         _uniqueIDOfLightToNodes;
        
        UniqueIDToOriginalUID           _uniqueIDToOriginalID;
        UniqueIDToOpenCOLLADAObject     _uniqueIDToOpenCOLLADAObject;
        FlattenerMapsForAnimationID     _flattenerMapsForAnimationID;
        UniqueIDToAccessor              _uniqueIDToAccessorObject;
        UniqueIDToAnimationFlattener    _targetUIDWithPathToAnimationFlattener;

    private:
        shared_ptr <GLTFProfile>        _profile;
        shared_ptr <JSONObject>         _root;

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

        UniqueIDToJSONValue             _uniqueIDToJSONValue;
        
        NameToOutputStream              _nameToOutputStream;
        GLTF::GLTFWriter                _writer;
        std::string                     _sharedBufferId;
        
        std::vector <shared_ptr<GLTFAssetModifier> > _assetModifiers;
    };

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif