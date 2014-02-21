#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "GLTFAsset.h"

namespace GLTF
{
    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId)
    {
        std::string id = "";
        id += type + "_" + GLTF::GLTFUtils::toString(uniqueId.getObjectId());
        return id;
    }
    
    GLTFAsset::GLTFAsset():_isBundle(false) {
        this->_trackedResourcesPath = shared_ptr<JSONObject> (new JSONObject());
        this->_trackedOutputResourcesPath = shared_ptr<JSONObject> (new JSONObject());
        this->_converterConfig = shared_ptr<GLTFConfig> (new GLTFConfig());
        this->_convertionResults = shared_ptr<JSONObject> (new JSONObject());
        this->_prefix = 0;
        this->_geometryByteLength = 0;
        this->_animationByteLength = 0;
        this->setGeometryByteLength(0);
        this->setAnimationByteLength(0);
        this->shaderIdToShaderString.clear();
        this->_uniqueIDToMeshes.clear();
    }
    
    shared_ptr<GLTFOutputStream> GLTFAsset::createOutputStreamIfNeeded(const std::string& streamName) {

        if (this->_nameToOutputStream.count(streamName) == 0) {
            COLLADABU::URI inputURI(this->getInputFilePath().c_str());
            COLLADABU::URI outputURI(this->getOutputFilePath().c_str());
            
            std::string folder = outputURI.getPathDir();
            std::string fileName = inputURI.getPathFileBase();
            
            shared_ptr<GLTFOutputStream> outputStream = shared_ptr <GLTFOutputStream> (new GLTFOutputStream(folder, fileName, streamName));
            this->_nameToOutputStream[streamName] = outputStream;
        }
        
        return this->_nameToOutputStream[streamName];
    }
    
    void GLTFAsset::closeOutputStream(const std::string& streamName, bool removeFile) {
        
        if (this->_nameToOutputStream.count(streamName) > 0) {
            shared_ptr<GLTFOutputStream> outputStream = this->_nameToOutputStream[streamName];
            
            outputStream->close();
            if (removeFile) {
                remove(outputStream->outputPathCStr());
            }
            
            //FIXME: We keep it around as it's informations are still accessed once close
            //Would be better to remove the entry from the map when closed...
            //this->_nameToOutputStream.erase(streamName);
        }
    }

    shared_ptr <GLTFConfig> GLTFAsset::converterConfig() {
        return this->_converterConfig;
    }
    
    shared_ptr <JSONObject> GLTFAsset::convertionResults() {
        return this->_convertionResults;
    }
    
    const std::string GLTFAsset::resourceOuputPathForPath(const std::string& resourcePath) {
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
    
    void GLTFAsset::setBundleOutputPath(const std::string& bundleOutputPath) {
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
    
    const std::string& GLTFAsset::getBundleOutputPath() {
        return this->_bundleOutputPath;
    };
    
    void GLTFAsset::log(const char * format, ... ) {
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
    
    void GLTFAsset::setGeometryByteLength(size_t geometryByteLength) {
        this->_geometryByteLength = geometryByteLength;
    }
    
    size_t GLTFAsset::getGeometryByteLength() {
        return this->_geometryByteLength;
    }
    
    void GLTFAsset::setAnimationByteLength(size_t animationByteLength) {
        this->_animationByteLength = animationByteLength;
    }
    
    size_t GLTFAsset::getAnimationByteLength() {
        return this->_animationByteLength;
    }
    
    void GLTFAsset::setOutputFilePath(const std::string& outputFilePath) {
        this->_outputFilePath = outputFilePath;
    }
    
    std::string GLTFAsset::getOutputFilePath() {
        return this->_outputFilePath;
    }
    
    void GLTFAsset::setInputFilePath(const std::string& inputFilePath) {
        this->_inputFilePath = inputFilePath;
    }
    
    std::string GLTFAsset::getInputFilePath() {
        return this->_inputFilePath;
    }
    
    std::string GLTFAsset::pathRelativeToInputPath(const std::string& path) {
        if (GLTFUtils::isAbsolutePath(path) == true) {
            return path;
        } else {
            COLLADABU::URI aURI(this->_inputFilePath.c_str());
            COLLADABU::URI inputURI(path.c_str());
            aURI.setPath(aURI.getPathDir(), inputURI.getPathFileBase(), inputURI.getPathExtension());
            return aURI.getURIString();
        }
    }
    
    void GLTFAsset::copyImagesInsideBundleIfNeeded() {
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
}
