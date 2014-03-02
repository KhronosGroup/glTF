#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"
#include "../GitSHA1.h"
#include "GLTF-Open3DGC.h"

using namespace std::tr1;
using namespace std;

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
    
    void GLTFAsset::setObjectForUniqueId(const std::string& uniqueId, shared_ptr<JSONObject> obj) {
        this->_uniqueIDToJSONObject[uniqueId] = obj;
    }

    shared_ptr<JSONObject> GLTFAsset::getObjectForUniqueId(const std::string& uniqueId) {
        return this->_uniqueIDToJSONObject[uniqueId];
    }
    
    bool GLTFAsset::containsObjectForUniqueId(const std::string& uniqueId) {
        return this->_uniqueIDToJSONObject.count(uniqueId) > 0;
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
    
    void GLTFAsset::prepareForProfile(shared_ptr<GLTFProfile> profile) {
        this->profile = profile;
        this->root = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
        this->root->setString(kProfile, profile->id());
        this->root->createObjectIfNeeded(kNodes);
        
        this->_writer.initWithPath(this->getOutputFilePath().c_str());
    }
    
    static void __eval(JSONValue* value, void *context) {
        value->evaluate(context);
    }
    
    void GLTFAsset::write() {
        ifstream inputVertices;
        ifstream inputIndices;
        ifstream inputAnimation;
        ifstream inputCompression;
        ofstream ouputStream;
        
        shared_ptr<GLTFOutputStream> verticesOutputStream = this->createOutputStreamIfNeeded(kVerticesOutputStream);
        shared_ptr<GLTFOutputStream> indicesOutputStream = this->createOutputStreamIfNeeded(kIndicesOutputStream);
        shared_ptr<GLTFOutputStream> animationOutputStream = this->createOutputStreamIfNeeded(kAnimationOutputStream);
        shared_ptr<GLTFOutputStream> compressionOutputStream = this->createOutputStreamIfNeeded(kCompressionOutputStream);
        
        /*
         1. We output vertices and indices separately in 2 different files
         2. Then output them in a single file
         */
        COLLADABU::URI inputURI(this->getInputFilePath().c_str());
        COLLADABU::URI outputURI(this->getOutputFilePath().c_str());
        
        std::string folder = outputURI.getPathDir();
        std::string fileName = inputURI.getPathFileBase();
        
        std::string sharedBufferID = fileName;
        std::string outputFilePath = outputURI.getPathDir() + fileName + ".bin";
        
        ouputStream.open (outputFilePath.c_str(), ios::out | ios::ate | ios::binary);
        
        shared_ptr<JSONObject> assetObject = this->root->createObjectIfNeeded("asset");
        std::string version = "collada2gltf@"+std::string(g_GIT_SHA1);
        assetObject->setString("generator",version);
        shared_ptr<JSONObject> assetExtras = assetObject->createObjectIfNeeded("extras");
        assetExtras->setBool(kPremultipliedAlpha, converterConfig()->config()->getBool(kPremultipliedAlpha));
        assetObject->setValue("extras", assetExtras);
        
        // ----
        UniqueIDToAnimation::const_iterator UniqueIDToAnimationsIterator;
        shared_ptr <GLTF::JSONObject> animationsObject(new GLTF::JSONObject());
        
        this->root->setValue("animations", animationsObject);
        
        std::vector<GLTFAnimationFlattener*> flatteners;
        
        for (UniqueIDToAnimationsIterator = this->_uniqueIDToAnimation.begin() ; UniqueIDToAnimationsIterator != this->_uniqueIDToAnimation.end() ; UniqueIDToAnimationsIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            
            std::string inputParameterName = "TIME";
            shared_ptr<GLTFAnimation> animation = (*UniqueIDToAnimationsIterator).second;
            shared_ptr<GLTFBufferView> timeBufferView = animation->getBufferViewForParameter(inputParameterName);
            
            if (animation->parameters()->contains(inputParameterName) == false) {
                setupAndWriteAnimationParameter(animation.get(),
                                                inputParameterName,
                                                "FLOAT",
                                                (unsigned char*)timeBufferView->getBufferDataByApplyingOffset(),
                                                timeBufferView->getByteLength(), true,
                                                *this);
            }
            
            std::vector<std::string> allTargets = animation->targets()->getAllKeys();
            for (size_t i = 0 ; i < allTargets.size() ; i++) {
                std::string targetID = allTargets[i];
                shared_ptr<GLTFAnimationFlattener> animationFlattener = animation->animationFlattenerForTargetUID(targetID);
                if (std::find(flatteners.begin(), flatteners.end(), animationFlattener.get()) != flatteners.end()) {
                    continue;
                }
                flatteners.push_back(animationFlattener.get());
                animation->writeAnimationForTargetID(targetID, *this);
            }
        }
        
        //reopen .bin files for vertices and indices
        size_t verticesLength = verticesOutputStream->length();
        size_t indicesLength = indicesOutputStream->length();
        size_t animationLength = animationOutputStream->length();
        size_t compressionLength = compressionOutputStream->length();
        
        shared_ptr <GLTFBuffer> compressionBuffer(new GLTFBuffer(compressionOutputStream->id(), compressionLength));
        
        this->closeOutputStream(kVerticesOutputStream, false);
        this->closeOutputStream(kIndicesOutputStream, false);
        this->closeOutputStream(kAnimationOutputStream, false);
        this->closeOutputStream(kCompressionOutputStream, false);
        
        inputCompression.open(compressionOutputStream->outputPathCStr(), ios::in | ios::binary);
        
        inputVertices.open(verticesOutputStream->outputPathCStr(), ios::in | ios::binary);
        char* bufferIOStream = (char*)malloc(sizeof(char) * verticesLength);
        inputVertices.read(bufferIOStream, verticesLength);
        ouputStream.write(bufferIOStream, verticesLength);
        free(bufferIOStream);
        inputVertices.close();
        remove(verticesOutputStream->outputPathCStr());
        
        inputIndices.open(indicesOutputStream->outputPathCStr(), ios::in | ios::binary);
        bufferIOStream = (char*)malloc(sizeof(char) * indicesLength);
        inputIndices.read(bufferIOStream, indicesLength);
        ouputStream.write(bufferIOStream, indicesLength);
        free(bufferIOStream);
        inputIndices.close();
        remove(indicesOutputStream->outputPathCStr());
        
        //add padding for https://github.com/KhronosGroup/glTF/issues/167
        //it is known that the other buffers are all FLOAT, so as a minimal fix we just have to align indices (that are short) on FLOAT when writting them.
        size_t rem = indicesLength % 4;
        if (rem) {
            char pad[3];
            size_t paddingForAlignement = 4 - rem;
            ouputStream.write(pad, paddingForAlignement);
            indicesLength += paddingForAlignement;
        }
        
        if (animationLength > 0) {
            inputAnimation.open(animationOutputStream->outputPathCStr(), ios::in | ios::binary);
            bufferIOStream = (char*)malloc(sizeof(char) * animationLength);
            inputAnimation.read(bufferIOStream, animationLength);
            ouputStream.write(bufferIOStream, animationLength);
            free(bufferIOStream);
            inputAnimation.close();
            remove(animationOutputStream->outputPathCStr());
        }
        
        inputCompression.close();
        if (compressionLength == 0) {
            remove(compressionOutputStream->outputPathCStr());
        }
        
        shared_ptr <GLTFBuffer> sharedBuffer(new GLTFBuffer(sharedBufferID, verticesLength + indicesLength + animationLength));
        
        //---
        shared_ptr <GLTFBufferView> verticesBufferView(new GLTFBufferView(sharedBuffer, 0, verticesLength));
        shared_ptr <GLTFBufferView> indicesBufferView(new GLTFBufferView(sharedBuffer, verticesLength, indicesLength));
        shared_ptr <GLTFBufferView> genericBufferView(new GLTFBufferView(sharedBuffer, verticesLength + indicesLength, animationLength));
        shared_ptr <GLTFBufferView> compressionBufferView(new GLTFBufferView(compressionBuffer, 0, compressionLength));
        
        // ----
        UniqueIDToMeshes::const_iterator UniqueIDToMeshesIterator;
        shared_ptr <GLTF::JSONObject> meshesObject(new GLTF::JSONObject());
        
        this->root->setValue("meshes", meshesObject);
        
        shared_ptr <GLTF::JSONObject> accessors = this->root->createObjectIfNeeded("accessors");
        
        for (UniqueIDToMeshesIterator = this->_uniqueIDToMeshes.begin() ; UniqueIDToMeshesIterator != this->_uniqueIDToMeshes.end() ; UniqueIDToMeshesIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            MeshVectorSharedPtr meshes = (*UniqueIDToMeshesIterator).second;
            if (meshes) {
                for (size_t j = 0 ; j < meshes->size() ; j++) {
                    shared_ptr<GLTFMesh> mesh = (*meshes)[j];
                    if (mesh) {
                        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
                        
                        bool isCompressed = false;
                        if (mesh->getExtensions()) {
                            isCompressed = mesh->getExtensions()->contains("Open3DGC-compression");
                        }
                        
                        void *serializationContext[4];
                        serializationContext[0] = isCompressed ? (void*)compressionBufferView.get() : (void*)verticesBufferView.get();
                        serializationContext[1] = isCompressed ? (void*)compressionBufferView.get() : (void*)indicesBufferView.get();
                        serializationContext[2] = isCompressed ? (void*)compressionBufferView.get() : (void*)genericBufferView.get();
                        serializationContext[3] = (void*)this;
                        
                        mesh->resolveAttributes();
                        
                        //serialize attributes
                        vector <GLTF::Semantic> allSemantics = mesh->allSemantics();
                        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
                            GLTF::Semantic semantic = allSemantics[i];
                            GLTF::IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
                            GLTF::IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = mesh->getMeshAttributesForSemantic(semantic);
                            
                            for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                                //(*it).first;             // the key value (of type Key)
                                //(*it).second;            // the mapped value (of type T)
                                shared_ptr <GLTF::GLTFAccessor> meshAttribute = (*meshAttributeIterator).second;
                                shared_ptr <GLTF::JSONObject> meshAttributeObject = meshAttribute;
                                
                                meshAttribute->setBufferView(isCompressed ? compressionBufferView : verticesBufferView);
                                
                                accessors->setValue(meshAttribute->getID(), meshAttributeObject);
                            }
                        }
                        
                        //serialize indices
                        primitives = mesh->getPrimitives()->values();
                        unsigned int primitivesCount =  (unsigned int)primitives.size();
                        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
                            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
                            shared_ptr <GLTF::GLTFAccessor> uniqueIndices =  primitive->getIndices();
                            
                            GLTFBufferView *bufferView = (GLTFBufferView*)serializationContext[1];
                            
                            uniqueIndices->setString(kBufferView, bufferView->getID());
                            uniqueIndices->setUnsignedInt32(kType, this->profile->getGLenumForString("UNSIGNED_SHORT"));
                            
                            accessors->setValue(uniqueIndices->getID(), uniqueIndices);
                        }
                        
                        meshesObject->setValue(mesh->getID(), static_pointer_cast<JSONValue>(mesh));
                    }
                }
            }
        }
        
        // ----
        
        shared_ptr <GLTF::JSONObject> materials = this->root->createObjectIfNeeded("materials");
        vector <std::string> materialUIDs = materials->getAllKeys();
        for (size_t i = 0 ; i < materialUIDs.size() ; i++) {
            shared_ptr <GLTF::GLTFEffect> effect = static_pointer_cast<GLTFEffect>(materials->getObject(materialUIDs[i]));
            if (!effect->getTechniqueGenerator()) {
                materials->removeValue(effect->getID());
            }
        }
                
        // ----
        shared_ptr <GLTF::JSONObject> skins = this->root->createObjectIfNeeded("skins");
        
        UniqueIDToSkin::const_iterator UniqueIDToSkinIterator;
        
        for (UniqueIDToSkinIterator = this->_uniqueIDToSkin.begin() ; UniqueIDToSkinIterator != this->_uniqueIDToSkin.end() ; UniqueIDToSkinIterator++) {
            
            shared_ptr <GLTFSkin> skin = (*UniqueIDToSkinIterator).second;
            shared_ptr<JSONArray> joints = skin->getJointsIds();
            shared_ptr<JSONArray> jointsWithOriginalSids(new JSONArray());
            
            //resolve the sid and use the original ones
            std::vector <shared_ptr <JSONValue> > values = joints->values();
            for (size_t i = 0 ; i < values.size() ; i++) {
                shared_ptr<JSONString> jointId = static_pointer_cast<JSONString>(values[i]);
                shared_ptr<JSONObject> node = this->_uniqueIDToJSONObject[jointId->getString()];
                if (node->contains("jointId")) {
                    jointsWithOriginalSids->appendValue(static_pointer_cast <JSONValue> (node->getValue("jointId")));
                }
            }
            skin->setJointsIds(jointsWithOriginalSids);
            shared_ptr <JSONObject> inverseBindMatrices = static_pointer_cast<JSONObject>(skin->extras()->getValue(kInverseBindMatrices));
            inverseBindMatrices->setString(kBufferView, genericBufferView->getID());
            skin->setValue(kInverseBindMatrices, inverseBindMatrices);
            skins->setValue(skin->getId(), skin);
        }
        
        // ----
        
        for (UniqueIDToAnimationsIterator = this->_uniqueIDToAnimation.begin() ; UniqueIDToAnimationsIterator != this->_uniqueIDToAnimation.end() ; UniqueIDToAnimationsIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            
            shared_ptr<GLTFAnimation> animation = (*UniqueIDToAnimationsIterator).second;
            shared_ptr<JSONObject> parameters = animation->parameters();
            
            //Replace OpenCOLLADA uniqueID by Original IDs
            shared_ptr <JSONArray> channels = animation->channels();
            for (size_t i = 0 ; i < channels->values().size() ; i++) {
                shared_ptr<JSONObject> channel = static_pointer_cast<JSONObject>(channels->values()[i]);
                shared_ptr<JSONObject> target = channel->getObject("target");
                std::string originalID = this->_uniqueIDToOriginalID[target->getString("id")];
                target->setString("id", originalID);
            }
            
            std::vector <std::string> parameterKeys = parameters->getAllKeys();
            for (size_t i = 0 ; i <parameterKeys.size() ; i++) {
                std::string parameterUID = parameters->getString(parameterKeys[i]);
                shared_ptr <JSONObject> parameterObject = accessors->getObject(parameterUID);
                if (parameterObject->contains("extensions")) {
                    shared_ptr <JSONObject> extensions = parameterObject->getObject("extensions");
                    shared_ptr<JSONObject> compressionObject = extensions->getObject("Open3DGC-compression");
                    if (compressionObject->contains("compressedData")) {
                        shared_ptr<JSONObject> compressedData = compressionObject->getObject("compressedData");
                        compressedData->setString("bufferView", compressionBufferView->getID());
                    }
                }
                parameterObject->setString("bufferView", genericBufferView->getID());
            }
            
            if (animation->channels()->values().size() > 0) {
                animationsObject->setValue(animation->getID(), animation);
            }
        }
        
        shared_ptr <JSONObject> buffersObject(new JSONObject());
        
        this->root->setValue("buffers", buffersObject);
        
        if (sharedBuffer->getByteLength() > 0) {
            sharedBuffer->setString(kPath, sharedBufferID + ".bin");
            sharedBuffer->setString(kType, "arraybuffer");
            buffersObject->setValue(sharedBufferID, sharedBuffer);
        }
        
        if (compressionBuffer->getByteLength() > 0) {
            std::string compressedBufferID = compressionOutputStream->id();
            buffersObject->setValue(compressedBufferID, compressionBuffer);
            compressionBuffer->setString(kPath, compressedBufferID + ".bin");
            if (converterConfig()->config()->getString("compressionMode") == "ascii")
                compressionBuffer->setString(kType, "text");
            else
                compressionBuffer->setString(kType, "arraybuffer");
        }
        
        //FIXME: below is an acceptable short-cut since in this converter we will always create one buffer view for vertices and one for indices.
        //Fabrice: Other pipeline tools should be built on top of the format & manipulate the buffers and end up with a buffer / bufferViews layout that matches the need of a given application for performance. For instance we might want to concatenate a set of geometry together that come from different file and call that a "level" for a game.
        shared_ptr <JSONObject> bufferViewsObject(new JSONObject());
        this->root->setValue("bufferViews", bufferViewsObject);
        
        bufferViewsObject->setValue(indicesBufferView->getID(), indicesBufferView);
        bufferViewsObject->setValue(verticesBufferView->getID(), verticesBufferView);
        if ((animationLength > 0) || (compressionLength >0)) {
            bufferViewsObject->setValue(genericBufferView->getID(), genericBufferView);
        }
        if (compressionLength > 0) {
            bufferViewsObject->setValue(compressionBufferView->getID(), compressionBufferView);
        }
        
        indicesBufferView->setUnsignedInt32("target", this->profile->getGLenumForString("ELEMENT_ARRAY_BUFFER"));
        verticesBufferView->setUnsignedInt32("target", this->profile->getGLenumForString("ARRAY_BUFFER"));
        
        //---
        //this apply MUST be before the removeValue lightsIds that follows
        this->root->apply(__eval, this);
        this->root->removeValue("lightsIds");
        
        this->root->write(&this->_writer, this);
                
        ouputStream.flush();
        ouputStream.close();
        
        if (sharedBuffer->getByteLength() == 0)
            remove(outputFilePath.c_str());
                
        this->convertionResults()->setUnsignedInt32("geometry", this->getGeometryByteLength());
        this->convertionResults()->setUnsignedInt32("animation", this->getAnimationByteLength());
        this->convertionResults()->setUnsignedInt32("scene", (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        this->log("[geometry] %d bytes\n", (int)this->getGeometryByteLength());
        this->log("[animations] %d bytes\n", (int)this->getAnimationByteLength());
        this->log("[scene] total bytes:%d\n", (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        this->copyImagesInsideBundleIfNeeded();        
    }
    
}
