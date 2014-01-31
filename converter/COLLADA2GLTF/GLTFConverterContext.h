#ifndef __GLTFConverterContext__
#define __GLTFConverterContext__

#include <stdarg.h>
#include "GLTF-OpenCOLLADA.h"

namespace GLTF
{
#define CONFIG_BOOL(X) (converterContext.converterConfig()->config()->getBool(X))
#define CONFIG_STRING(X) (converterContext.converterConfig()->config()->getString(X))
#define CONFIG_DOUBLE(X) (converterContext.converterConfig()->config()->getDouble(X))
#define CONFIG_INT32(X) (converterContext.converterConfig()->config()->getInt32(X))
#define CONFIG_UINT32(X) (converterContext.converterConfig()->config()->getUInt32(X))

    class AccessorCache
    {
    public:
        AccessorCache(void *pData, size_t length) :
            m_length(length)
        {
            m_pData = new unsigned char[m_length];
            memcpy(m_pData, pData, m_length);
        }

        AccessorCache(const AccessorCache& rhs) :
            m_length(rhs.m_length)
        {
            m_pData = new unsigned char[m_length];
            memcpy(m_pData, rhs.m_pData, m_length);
        }

        ~AccessorCache()
        {
            delete m_pData;
        }

        bool operator<(const AccessorCache& rhs) const
        {
            if (m_length != rhs.m_length)
            {
                return m_length < rhs.m_length;
            }

            if (m_pData == rhs.m_pData)
            {
                return false;
            }

            return memcmp(m_pData, rhs.m_pData, m_length) < 0;
        }

    private:
        const AccessorCache& operator=(const AccessorCache& rhs);
        unsigned char *m_pData;
        size_t m_length;
    };

    class GLTFAnimationFlattener;
    
    typedef std::vector <shared_ptr<JSONObject> > AnimatedTargets;
    typedef shared_ptr <AnimatedTargets> AnimatedTargetsSharedPtr;

    typedef shared_ptr <MeshVector> MeshVectorSharedPtr;
    typedef std::map<std::string  , std::string > ShaderIdToShaderString;
    typedef std::map<std::string /* openCOLLADA uniqueID */, GLTFMeshSharedPtr > UniqueIDToMesh;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, MeshVectorSharedPtr > UniqueIDToMeshes;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, COLLADAFW::UniqueId > MaterialUIDToEffectUID;
    typedef std::map<std::string , unsigned int> SamplerHashtoSamplerIndex;
    
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, std::string > MaterialUIDToName;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFEffect> > UniqueIDToEffect;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFAnimation> > UniqueIDToAnimation;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<GLTFSkin> > UniqueIDToSkin;
    typedef std::map<std::string /* openCOLLADA uniqueID from AnimationList*/, AnimatedTargetsSharedPtr > UniqueIDToAnimatedTargets;
    typedef std::map<std::string  , std::string > ImageIdToImagePath;
    typedef std::map<std::string , shared_ptr<JSONObject> > OriginalIDToTrackedObject;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDToParentsOfInstanceNode;
    typedef std::map<std::string , shared_ptr<GLTFAnimationFlattener> > UniqueIDToAnimationFlattener;
    typedef std::map<std::string , shared_ptr<JSONArray> > UniqueIDTOfLightToNodes;

    typedef std::map<std::string , shared_ptr<JSONValue> > UniqueIDToJSONValue;
    typedef std::map<std::string , std::string > UniqueIDToOriginalUID;
    typedef std::map<std::string , shared_ptr <COLLADAFW::Object> > UniqueIDToOpenCOLLADAObject;
    typedef std::map<AccessorCache , std::string> UniqueIDToAccessor;
    
    class GLTFConverterContext
    {
    public:
        GLTFConverterContext() : _isBundle(false) {
            this->_trackedResourcesPath = shared_ptr<JSONObject> (new JSONObject());
            this->_trackedOutputResourcesPath = shared_ptr<JSONObject> (new JSONObject());
            this->_converterConfig = shared_ptr<GLTFConfig> (new GLTFConfig());
            this->_convertionResults = shared_ptr<JSONObject> (new JSONObject());
            this->_prefix = 0;
            this->_geometryByteLength = 0;
            this->_animationByteLength = 0;
        }
        
        shared_ptr <GLTFConfig> converterConfig() {
            return this->_converterConfig;
        }
        
        shared_ptr <JSONObject> convertionResults() {
            return this->_convertionResults;
        }
        
        const std::string resourceOuputPathForPath(const std::string& resourcePath) {
            if (this->_isBundle == false) {
                return resourcePath;
            }
            
            if (this->_trackedResourcesPath->contains(resourcePath)) {
                return this->_trackedResourcesPath->getString(resourcePath);
            } else {
                COLLADABU::URI resourceURI(resourcePath);
                std::string path = resourceURI.getPathFile();
                if (this->_trackedOutputResourcesPath->contains(path)) {
                    std::string existingResourcePath = this->_trackedOutputResourcesPath->getString(path);
                    if (existingResourcePath == resourcePath) {
                        path = path + "_" + GLTFUtils::toString(this->_prefix);
                        this->_prefix++;
                        this->_trackedOutputResourcesPath->setString(path, resourcePath);
                        this->_trackedResourcesPath->setString(resourcePath, path);
                    }
                    
                } else {
                    this->_trackedOutputResourcesPath->setString(path, resourcePath);
                    this->_trackedResourcesPath->setString(resourcePath, path);
                }
                return path;
            }
        }
        
        void setBundleOutputPath(const std::string& bundleOutputPath) {
            COLLADABU::URI inputPathURI(this->_inputFilePath.c_str());
            COLLADABU::URI outputBundlePathURI(bundleOutputPath.c_str());
            
            this->_isBundle = true;
            
            if (GLTFUtils::isAbsolutePath(outputBundlePathURI.toNativePath()) == false) {
                //If we do not have an absolute path, then we want to be relative to the input path
                outputBundlePathURI.setPath(inputPathURI.getPathDir(), outputBundlePathURI.getPathFileBase(), outputBundlePathURI.getPathExtension());
                this->_bundleOutputPath = outputBundlePathURI.toNativePath();
                
                COLLADABU::URI outputPathURI(outputBundlePathURI.getURIString() + "/" + outputBundlePathURI.getPathFileBase() + "." + "json");
                this->_outputFilePath = outputPathURI.toNativePath();
//                this->log("outputBundlePath:%s\n",outputBundlePathURI.toNativePath().c_str());
//                this->log("outputPath:%s\n",outputPathURI.toNativePath().c_str());
            } else {
                this->_bundleOutputPath = outputBundlePathURI.toNativePath();
                COLLADABU::URI outputPathURI(outputBundlePathURI.getURIString() + "/" + outputBundlePathURI.getPathFileBase()  + "." + "json");
                this->_outputFilePath = outputPathURI.toNativePath();
            }
            COLLADABU::Utils::createDirectoryIfNeeded(this->_bundleOutputPath.c_str());
        }
        
        const std::string& getBundleOutputPath() {
            return this->_bundleOutputPath;
        };
        
        void log(const char * format, ... ) {
            if ((this->_converterConfig->boolForKeyPath("outputProgress", false) == false) &&
                (this->_converterConfig->boolForKeyPath("outputConvertionResults", false)) == false) {
                char buffer[1000];
                va_list args;
                va_start (args, format);
                vsprintf (buffer,format, args);
                va_end (args);
                
                printf("%s",buffer);
            }
        }
        
        void setGeometryByteLength(size_t geometryByteLength) {
            this->_geometryByteLength = geometryByteLength;
        }
        
        size_t getGeometryByteLength() {
            return this->_geometryByteLength;
        }

        void setAnimationByteLength(size_t animationByteLength) {
            this->_animationByteLength = animationByteLength;
        }
        
        size_t getAnimationByteLength() {
            return this->_animationByteLength;
        }

        void setOutputFilePath(const std::string& outputFilePath) {
            this->_outputFilePath = outputFilePath;
        }
        
        std::string getOutputFilePath() {
            return this->_outputFilePath;
        }

        void setInputFilePath(const std::string& inputFilePath) {
            this->_inputFilePath = inputFilePath;
        }
        
        std::string getInputFilePath() {
            return this->_inputFilePath;
        }

        std::string pathRelativeToInputPath(const std::string& path) {
            if (GLTFUtils::isAbsolutePath(path) == true) {
                return path;
            } else {
                COLLADABU::URI aURI(this->_inputFilePath.c_str());
                COLLADABU::URI inputURI(path.c_str());
                aURI.setPath(aURI.getPathDir(), inputURI.getPathFileBase(), inputURI.getPathExtension());
                return aURI.getURIString();
            }
        }
        
        void copyImagesInsideBundleIfNeeded() {
            if (this->_isBundle == true) {
                shared_ptr<JSONObject> images = this->root->createObjectIfNeeded("images");
                size_t imagesCount = images->getKeysCount();
                if (imagesCount > 0) {
                    std::vector <std::string> keys = images->getAllKeys();
                    for (size_t i = 0 ; i < imagesCount ; i++) {
                        shared_ptr<JSONObject> image = images->getObject(keys[i]);
                        std::string path = image->getString("path");
                        //printf("\n****\nkey:%s\npath:%s\n", keys[i].c_str(), path.c_str());
                        
                        COLLADABU::URI outputImagePathURI(this->_bundleOutputPath + "/" + this->_trackedResourcesPath->getString(path).c_str());
                        //printf("copy %s\n to %s\n", path.c_str(), outputImagePathURI.toNativePath().c_str());
                        
                        //copy the file
                        std::string inputImagePath = this->pathRelativeToInputPath(path).c_str();
                        //printf("inputImagePath %s\n\n", inputImagePath.c_str());

                        std::ifstream f1(inputImagePath.c_str(), std::fstream::binary);
                        std::ofstream f2(outputImagePathURI.toNativePath().c_str(), std::fstream::binary);
                        
                        f2 << f1.rdbuf(); // read original file into target
                    }
                }
            }
        }
        
    public:
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
        UniqueIDToParentsOfInstanceNode _uniqueIDToParentsOfInstanceNode;
        SamplerHashtoSamplerIndex _samplerHashtoSamplerIndex;
        UniqueIDTOfLightToNodes _uniqueIDOfLightToNodes;
        
        UniqueIDToJSONValue _uniqueIDToJSONValue;

        UniqueIDToOriginalUID _uniqueIDToOriginalID;
        UniqueIDToOpenCOLLADAObject _uniqueIDToOpenCOLLADAObject;
        FlattenerMapsForAnimationID _flattenerMapsForAnimationID;
        UniqueIDToAccessor _uniqueIDToAccessorObject;
        
        GLTFOutputStream *_vertexOutputStream;
        GLTFOutputStream *_indicesOutputStream;
        GLTFOutputStream *_animationOutputStream;
        GLTFOutputStream *_compressionOutputStream;
        
        
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
    };

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId);

}
#endif