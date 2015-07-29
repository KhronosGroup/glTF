#ifndef __GLTFAsset__
#define __GLTFAsset__

#include <stdarg.h>
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAccessorCache.h"

namespace GLTF
{
    class GLTFAsset;
    
    class GLTFAssetValueEvaluator {
    public:
        //evaluate is mandatory
        virtual void evaluationWillStart(GLTFAsset*) {};
        virtual void evaluate(JSONValue* value, GLTFAsset* asset) = 0;
        virtual void evaluationDidComplete(GLTFAsset*) {};
        
        virtual ~GLTFAssetValueEvaluator() {};
    };
    
#define CONFIG_BOOL(asset,X) (asset->converterConfig()->config()->getBool(X))
#define CONFIG_STRING(asset, X) (asset->converterConfig()->config()->getString(X))
#define CONFIG_DOUBLE(asset, X) (asset->converterConfig()->config()->getDouble(X))
#define CONFIG_INT32(asset, X) (asset->converterConfig()->config()->getInt32(X))
#define CONFIG_UINT32(asset, X) (asset->converterConfig()->config()->getUInt32(X))

    const std::string kCompressionOutputStream = "compression";

    class GLTFAnimationFlattener;

    typedef std::vector <std::shared_ptr<JSONObject> > AnimatedTargets;
    typedef std::shared_ptr <AnimatedTargets> AnimatedTargetsSharedPtr;

    typedef std::map<std::string, std::string > ShaderIdToShaderString;
    typedef std::map<std::string /* openCOLLADA uniqueID */, COLLADAFW::UniqueId > MaterialUIDToEffectUID;
    typedef std::map<std::string, unsigned int> SamplerHashtoSamplerIndex;

    typedef std::map<std::string /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<std::string /* openCOLLADA uniqueID from AnimationList*/, AnimatedTargetsSharedPtr > UniqueIDToAnimatedTargets;
    typedef std::map<std::string, std::string > ImageIdToImagePath;
    typedef std::map<std::string, std::shared_ptr<JSONArray> > UniqueIDToParentsOfInstanceNode;
    typedef std::map<std::string, std::shared_ptr<GLTFAnimationFlattener> > UniqueIDToAnimationFlattener;
    typedef std::map<std::string, std::shared_ptr<JSONArray> > UniqueIDTOfLightToNodes;

    typedef std::map<std::string, std::shared_ptr<JSONObject> > OriginalIDToTrackedObject;
    typedef std::map<std::string, std::string > GLTFMapStringToString;
    typedef std::map<std::string, std::shared_ptr<JSONValue> > UniqueIDToJSONValue;

    typedef std::map<GLTFAccessorCache, std::string> UniqueIDToAccessor;

    typedef std::map<std::string, std::shared_ptr <COLLADAFW::Object> > UniqueIDToOpenCOLLADAObject;

    typedef std::map<std::string, std::shared_ptr<GLTFOutputStream> > NameToOutputStream;

    //types for late binding of material
    typedef std::map<unsigned int, std::shared_ptr <COLLADAFW::MaterialBinding> > MaterialBindingsPrimitiveMap;
    typedef std::map<std::string, std::shared_ptr <MaterialBindingsPrimitiveMap> > MaterialBindingsForMeshUID;
    typedef std::map<std::string, std::shared_ptr <MaterialBindingsForMeshUID> > MaterialBindingsForNodeUID;

    class COLLADA2GLTF_EXPORT GLTFAsset : public GLTFAssetValueEvaluator, public JSONValueApplier
    {
    public:
        GLTFAsset(std::shared_ptr<GLTFWriter> writer = 0);

        std::shared_ptr <GLTFConfig> converterConfig();
        std::shared_ptr <JSONObject> convertionResults();
        std::shared_ptr <JSONObject> convertionMetaData();

        std::shared_ptr <GLTFProfile> profile();
        std::shared_ptr <JSONObject> root();

        void write();

        const std::string resourceOuputPathForPath(const std::string& resourcePath);

        std::shared_ptr<GLTFOutputStream> createOutputStreamIfNeeded(const std::string& streamName);
        void closeOutputStream(const std::string& streamName, bool removeFile);

        void log(const char * format, ...);

        void setBundleOutputPath(const std::string& bundleOutputPath);
        const std::string& getBundleOutputPath();

        void setGeometryByteLength(size_t geometryByteLength);
        size_t getGeometryByteLength();

        void setAnimationByteLength(size_t animationByteLength);
        size_t getAnimationByteLength();

        void setOutputFilePath(const std::string& outputFilePath);
        const std::string& getOutputFilePath();
        std::string getOutputFolderPath();

        void setInputFilePath(const std::string& inputFilePath);
        const std::string& getInputFilePath();

        void setInputFileData(const std::string& inputFileData);
        const std::string& getInputFileData();

        void setDistanceScale(double distanceScale);
        double getDistanceScale();

        std::string pathRelativeToInputPath(const std::string& path);
        void copyImagesInsideBundleIfNeeded();

        void setValueForUniqueId(const std::string& uniqueId, std::shared_ptr<JSONValue> obj);
        std::shared_ptr<JSONValue> getValueForUniqueId(const std::string& uniqueId);
        bool containsValueForUniqueId(const std::string& uniqueId);

        void prepareForProfile(std::shared_ptr<GLTFProfile> profile);

        std::string getSharedBufferId();

        void setOriginalId(const std::string& uniqueId, const std::string& originalId);
        std::string getOriginalId(const std::string& uniqueId);
        std::string getUniqueId(const std::string& originalId);

        std::vector <std::shared_ptr<GLTFAssetModifier> > &assetModifiers() { return this->_assetModifiers; };

        MaterialBindingsForNodeUID& materialBindingsForNodeUID() { return this->_materialBindingsForNodeUID; }

        std::shared_ptr<JSONObject> getExtras();
        void setExtras(std::shared_ptr<JSONObject>);

        void addValueEvaluator(std::shared_ptr<GLTFAssetValueEvaluator> evaluator);
        void removeValueEvaluator(std::shared_ptr<GLTFAssetValueEvaluator> evaluator);
        
        //as an GLTFAssetValueEvaluator
        virtual void evaluationWillStart(GLTFAsset*);
        virtual void evaluate(JSONValue* value, GLTFAsset* asset);
        virtual void evaluationDidComplete(GLTFAsset*);
        
        void apply(JSONValue* value, void *context);

    protected:
        void launchModifiers();
    private:
        void _performValuesEvaluation();
        bool _applyMaterialBindingsForNode(const std::string& nodeUID);
        void _applyMaterialBindings(std::shared_ptr<GLTFMesh> mesh,
            std::shared_ptr <MaterialBindingsPrimitiveMap> materialBindingPrimitiveMap,
            std::shared_ptr <JSONArray> meshesArray,
            std::shared_ptr<JSONObject> meshExtras);
        void _writeJSONResource(const std::string &resourcePath, std::shared_ptr<JSONObject> obj);
    public:
        MaterialUIDToEffectUID          _materialUIDToEffectUID;
        MaterialUIDToName               _materialUIDToName;
        OriginalIDToTrackedObject       _originalIDToTrackedObject;
        SamplerHashtoSamplerIndex       _samplerHashtoSamplerIndex;
        FlattenerMapsForAnimationID     _flattenerMapsForAnimationID;

        UniqueIDToAnimatedTargets       _uniqueIDToAnimatedTargets;
        UniqueIDToParentsOfInstanceNode _uniqueIDToParentsOfInstanceNode;
        UniqueIDTOfLightToNodes         _uniqueIDOfLightToNodes;
        UniqueIDToOpenCOLLADAObject     _uniqueIDToOpenCOLLADAObject;
        UniqueIDToAccessor              _uniqueIDToAccessorObject;
        UniqueIDToAnimationFlattener    _targetUIDWithPathToAnimationFlattener;

    private:
        std::shared_ptr <GLTFProfile>        _profile;
        std::shared_ptr <JSONObject>         _root;

        GLTFMapStringToString           _uniqueIDToOriginalID;
        GLTFMapStringToString           _originalIDToUniqueID;

        std::shared_ptr <GLTFConfig>    _converterConfig;
        std::shared_ptr <JSONObject>    _convertionResults;
        std::shared_ptr <JSONObject>    _convertionMetaData;
        std::shared_ptr<JSONObject>     _originalResourcesPath;
        std::shared_ptr<JSONObject>     _trackedResourcesPath;
        std::shared_ptr<JSONObject>     _trackedOutputResourcesPath;
        std::shared_ptr<JSONObject>     _extras;
        std::shared_ptr<JSONObject>     _meshesForMaterialBindingKey;

        unsigned int                    _prefix;
        std::string                     _inputFilePath;
        std::string                     _inputFileData;
        std::string                     _outputFilePath;
        std::string                     _bundleOutputPath;
        std::string                     _sharedBufferId;
        size_t                          _geometryByteLength;
        size_t                          _animationByteLength;
        bool                            _isBundle;
        double                          _distanceScale;

        UniqueIDToJSONValue             _uniqueIDToJSONValue;

        NameToOutputStream              _nameToOutputStream;
        std::shared_ptr<GLTF::GLTFWriter>   _writer;

        std::vector <std::shared_ptr<GLTFAssetModifier> > _assetModifiers;

        MaterialBindingsForNodeUID _materialBindingsForNodeUID;
        
        std::vector <std::shared_ptr<GLTFAssetValueEvaluator>> _evaluators;
    };

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif
