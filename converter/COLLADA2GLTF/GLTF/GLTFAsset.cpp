#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"
#include "../GitSHA1.h"
#include "GLTF-Open3DGC.h"
#include "GLTFFlipUVModifier.h"

using namespace std::tr1;
using namespace std;

namespace GLTF
{
    bool writeMeshIndices(shared_ptr <GLTFMesh> mesh, size_t startOffset, GLTFAsset* asset) {
        GLTFOutputStream* indicesOutputStream = asset->createOutputStreamIfNeeded(asset->getSharedBufferId()).get();
        typedef std::map<std::string , shared_ptr<GLTF::GLTFBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        
        shared_ptr <MeshAttributeVector> allMeshAttributes = mesh->meshAttributes();
        unsigned int indicesCount, allIndicesCount = 0;
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr <GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();

            indicesCount = (unsigned int)uniqueIndices->getCount();
            shared_ptr <GLTFBufferView> indicesBufferView = uniqueIndices->getBufferView();
            unsigned int* uniqueIndicesBuffer = (unsigned int*) indicesBufferView->getBufferDataByApplyingOffset();
            if (indicesCount > 0) {
                allIndicesCount += indicesCount;
                //FIXME: this is assuming triangles
                unsigned int trianglesCount = asset->convertionResults()->getUnsignedInt32("trianglesCount");
                trianglesCount += indicesCount / 3;
                asset->convertionResults()->setUnsignedInt32("trianglesCount", trianglesCount);
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                unsigned short* ushortIndices = 0;
                
                ushortIndices = (unsigned short*)calloc(indicesLength, 1);
                for (unsigned int idx = 0 ; idx < indicesCount ; idx++) {
                    ushortIndices[idx] = (unsigned short)uniqueIndicesBuffer[idx];
                }
                uniqueIndices->setByteOffset(indicesOutputStream->length() - startOffset);
                indicesOutputStream->write((const char*)ushortIndices, indicesLength);
                asset->setGeometryByteLength(asset->getGeometryByteLength() + indicesLength);
                free(ushortIndices);
            }
        }
        return true;
    }
    
    
    bool writeMeshAttributes(shared_ptr <GLTFMesh> mesh, size_t startOffset, GLTFAsset* asset) {
        GLTFOutputStream* vertexOutputStream = asset->createOutputStreamIfNeeded(asset->getSharedBufferId()).get();
        typedef std::map<std::string , shared_ptr<GLTF::GLTFBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        shared_ptr <MeshAttributeVector> allMeshAttributes = mesh->meshAttributes();
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        
        shared_ptr<GLTFAccessor> positionAttribute = mesh->getMeshAttribute(GLTF::POSITION, 0);
        size_t vertexCount = positionAttribute->getCount();
        unsigned int totalVerticesCount = asset->convertionResults()->getUnsignedInt32("verticesCount");
        totalVerticesCount += vertexCount;
        asset->convertionResults()->setUnsignedInt32("verticesCount", totalVerticesCount);

        for (unsigned int j = 0 ; j < allMeshAttributes->size() ; j++) {
            shared_ptr <GLTFAccessor> meshAttribute = (*allMeshAttributes)[j];
            shared_ptr <GLTFBufferView> bufferView = meshAttribute->getBufferView();
            shared_ptr <GLTFBuffer> buffer = bufferView->getBuffer();
            
            if (!bufferView.get()) {
                return false;
            }
            
            if (IDToBuffer.count(bufferView->getBuffer()->getID()) == 0) {
                meshAttribute->exposeMinMax();
                meshAttribute->setByteOffset(vertexOutputStream->length() - startOffset);
                vertexOutputStream->write(buffer);
                IDToBuffer[bufferView->getBuffer()->getID()] = buffer;
                asset->setGeometryByteLength(asset->getGeometryByteLength() + buffer->getByteLength());
            }
        }
        return true;
    }
    
    bool writeCompressedMesh(shared_ptr <GLTFMesh> mesh, GLTFAsset* asset) {
        GLTFOutputStream* compressionOutputStream = asset->createOutputStreamIfNeeded(kCompressionOutputStream).get();
        
        shared_ptr <JSONObject> floatAttributeIndexMapping(new JSONObject());
        unsigned compressedBufferStart = compressionOutputStream->length();
        encodeOpen3DGCMesh(mesh, floatAttributeIndexMapping, asset);
        typedef std::map<std::string , shared_ptr<GLTF::GLTFBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        shared_ptr <MeshAttributeVector> allMeshAttributes = mesh->meshAttributes();
        int vertexCount;
        unsigned int indicesCount, allIndicesCount = 0;
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr <GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();
            /*
             Convert the indices to unsigned short and write the blob
             */
            indicesCount = (unsigned int)uniqueIndices->getCount();
            shared_ptr <GLTFBufferView> indicesBufferView = uniqueIndices->getBufferView();
            if (indicesCount > 0) {
                allIndicesCount += indicesCount;                
                //FIXME: this is assuming triangles
                unsigned int trianglesCount = asset->convertionResults()->getUnsignedInt32("trianglesCount");
                trianglesCount += indicesCount / 3;
                asset->convertionResults()->setUnsignedInt32("trianglesCount", trianglesCount);
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                uniqueIndices->setByteOffset(compressedBufferStart);
                compressedBufferStart += indicesLength; //we simulate how will be the uncompressed data here, so this is the length in short *on purpose*
            }
        }
        
        shared_ptr<GLTFAccessor> positionAttribute = mesh->getMeshAttribute(GLTF::POSITION, 0);
        vertexCount = positionAttribute->getCount();
        unsigned int totalVerticesCount = asset->convertionResults()->getUnsignedInt32("verticesCount");
        totalVerticesCount += vertexCount;
        asset->convertionResults()->setUnsignedInt32("verticesCount", totalVerticesCount);

        for (unsigned int j = 0 ; j < allMeshAttributes->size() ; j++) {
            shared_ptr <GLTFAccessor> meshAttribute = (*allMeshAttributes)[j];
            shared_ptr <GLTFBufferView> bufferView = meshAttribute->getBufferView();
            shared_ptr <GLTFBuffer> buffer = bufferView->getBuffer();
            
            if (!bufferView.get()) { return false; }
            
            if (!IDToBuffer[bufferView->getBuffer()->getID()].get()) {
                meshAttribute->exposeMinMax();
                meshAttribute->setByteOffset(compressedBufferStart);
                compressedBufferStart += buffer->getByteLength();
                IDToBuffer[bufferView->getBuffer()->getID()] = buffer;
            }
        }
        
        return true;
    }

    std::string uniqueIdWithType(std::string type, const COLLADAFW::UniqueId& uniqueId) {
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
    }
    
    void GLTFAsset::setValueForUniqueId(const std::string& uniqueId, shared_ptr<JSONValue> obj) {
        this->_uniqueIDToJSONValue[uniqueId] = obj;
    }

    shared_ptr<JSONValue> GLTFAsset::getValueForUniqueId(const std::string& uniqueId) {
        return this->_uniqueIDToJSONValue[uniqueId];
    }
    
    bool GLTFAsset::containsValueForUniqueId(const std::string& uniqueId) {
        return this->_uniqueIDToJSONValue.count(uniqueId) > 0;
    }

    shared_ptr<GLTFOutputStream> GLTFAsset::createOutputStreamIfNeeded(const std::string& streamName) {

        if (this->_nameToOutputStream.count(streamName) == 0) {
            COLLADABU::URI inputURI(this->getInputFilePath().c_str());
            COLLADABU::URI outputURI(this->getOutputFilePath().c_str());
            
            std::string folder = outputURI.getPathDir();
            std::string fileName = inputURI.getPathFileBase();
            
            shared_ptr<GLTFOutputStream> outputStream = shared_ptr <GLTFOutputStream> (new GLTFOutputStream(folder, streamName, ""));
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
    
    shared_ptr <GLTFProfile> GLTFAsset::profile() {
        return this->_profile;
    }
    
    shared_ptr <JSONObject> GLTFAsset::root() {
        return this->_root;
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
            shared_ptr<JSONObject> images = this->_root->createObjectIfNeeded("images");
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
        this->_profile = profile;
        this->_root = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
        this->_root->setString(kProfile, profile->id());
        this->_root->createObjectIfNeeded(kNodes);
        
        this->_writer.initWithPath(this->getOutputFilePath().c_str());
    }
    
    static void __eval(JSONValue* value, void *context) {
        value->evaluate(context);
    }
    
    std::string GLTFAsset::getSharedBufferId() {
        if (this->_sharedBufferId.length() == 0) {
            COLLADABU::URI inputURI(this->getInputFilePath().c_str());
            std::string fileName = inputURI.getPathFileBase();
            this->_sharedBufferId = fileName;
        }
        return this->_sharedBufferId;
    }
    
    void GLTFAsset::launchModifiers() {
        for (size_t i = 0 ; i < this->_assetModifiers.size() ; i++) {
            shared_ptr<GLTFAssetModifier> assetModifier = this->_assetModifiers[i];
            
            assetModifier->init();
            assetModifier->modify(this->_root);
            assetModifier->cleanup();
        }
    }
    
    void GLTFAsset::write() {
        ifstream inputCompression;
        
        GLTFAsset *asset = this;
        shared_ptr<GLTFOutputStream> rawOutputStream = this->createOutputStreamIfNeeded(this->getSharedBufferId());
        shared_ptr<GLTFOutputStream> compressionOutputStream = this->createOutputStreamIfNeeded(kCompressionOutputStream);

        shared_ptr <GLTF::JSONObject> animations = this->_root->createObjectIfNeeded("animations");
        std::vector <std::string> animationsUIDs = animations->getAllKeys();
        
        for (size_t animationIndex = 0 ; animationIndex < animationsUIDs.size() ; animationIndex++) {
            std::string inputParameterName = "TIME";
            shared_ptr<GLTFAnimation> animation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationsUIDs[animationIndex]));
            shared_ptr<GLTFBufferView> timeBufferView = animation->getBufferViewForParameter(inputParameterName);
            
            if (animation->parameters()->contains(inputParameterName) == false) {
                setupAndWriteAnimationParameter(animation.get(),
                                                inputParameterName,
                                                "FLOAT",
                                                (unsigned char*)timeBufferView->getBufferDataByApplyingOffset(),
                                                timeBufferView->getByteLength(), true,
                                                this);
            }
            
            std::vector<std::string> allTargets = animation->targets()->getAllKeys();
            std::vector<GLTFAnimationFlattener*> flatteners;
            for (size_t i = 0 ; i < allTargets.size() ; i++) {
                std::string targetID = allTargets[i];
                shared_ptr<GLTFAnimationFlattener> animationFlattener = animation->animationFlattenerForTargetUID(targetID);
                if (std::find(flatteners.begin(), flatteners.end(), animationFlattener.get()) != flatteners.end()) {
                    continue;
                }
                flatteners.push_back(animationFlattener.get());
                animation->writeAnimationForTargetID(targetID, this);
            }
            animations->setValue(animation->getID(), animation);
            animations->removeValue(animationsUIDs[animationIndex]);
        }
        
        shared_ptr <GLTF::JSONObject> meshes = this->_root->createObjectIfNeeded(kMeshes);
        shared_ptr <GLTF::JSONObject> accessors = this->_root->createObjectIfNeeded(kAccessors);
        
        std::vector <std::string> meshesUIDs = meshes->getAllKeys();
        
        //WORK-AROUND: we don't want meshes without material, which can happen if a mesh is not associated with a node.
        //In this case, the material binding - isn't resolved.
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            mesh->resolveAttributes();
            GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
            unsigned int primitivesCount =  (unsigned int)primitives.size();
            for (unsigned int k = 0 ; k < primitivesCount ; k++) {
                shared_ptr<GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[k]);
                if (primitive->contains("material") ==  false) {
                    meshes->removeValue(mesh->getID());
                    break;
                }
            }
        }
        
        //Meshes may have changed ids here, get keys again.
        meshesUIDs = meshes->getAllKeys();

        // ----
        
        shared_ptr <GLTF::JSONObject> materials = this->_root->createObjectIfNeeded("materials");
        vector <std::string> materialUIDs = materials->getAllKeys();
        for (size_t i = 0 ; i < materialUIDs.size() ; i++) {
            shared_ptr <GLTF::GLTFEffect> material = static_pointer_cast<GLTFEffect>(materials->getObject(materialUIDs[i]));
            if (!material->getTechniqueGenerator()) {
                materials->removeValue(material->getID());
            }
        }
        
        this->assetModifiers().push_back(shared_ptr<GLTFFlipUVMofifier> (new GLTFFlipUVMofifier()));
        
        this->launchModifiers();
        
        size_t verticesLength = 0;
        size_t indicesLength = 0;
        size_t animationLength = rawOutputStream->length();
        size_t previousLength = animationLength;
                
        //save all meshes as compressed
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            bool compressMesh = (CONFIG_STRING("compressionType") == "Open3DGC") && canEncodeOpen3DGCMesh(mesh,this->_profile);
            if (compressMesh)
                writeCompressedMesh(mesh, this);
        }
        size_t compressionLength = compressionOutputStream->length();
        shared_ptr <GLTFBuffer> compressionBuffer(new GLTFBuffer(compressionOutputStream->id(), compressionLength));
        
        //save all indices
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            bool compressMesh = (CONFIG_STRING("compressionType") == "Open3DGC") && canEncodeOpen3DGCMesh(mesh,this->_profile);
            if (!compressMesh)
                writeMeshIndices(mesh, previousLength, this);
        }
        
        indicesLength = rawOutputStream->length() - previousLength;
        previousLength = rawOutputStream->length();
        //add padding for https://github.com/KhronosGroup/glTF/issues/167
        //it is known that the other buffers are all FLOAT, so as a minimal fix we just have to align indices (that are short) on FLOAT when writting them.
        size_t rem = indicesLength % 4;
        if (rem) {
            char pad[3];
            size_t paddingForAlignement = 4 - rem;
            rawOutputStream->write(pad, paddingForAlignement);
            indicesLength += paddingForAlignement;
            previousLength = rawOutputStream->length();
        }
        
        //save all mesh attributes
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            bool compressMesh = (CONFIG_STRING("compressionType") == "Open3DGC") && canEncodeOpen3DGCMesh(mesh,this->_profile);
            if (!compressMesh)
                writeMeshAttributes(mesh, previousLength, this);
        }
        verticesLength = rawOutputStream->length() - previousLength;
        previousLength = rawOutputStream->length();

        shared_ptr <GLTFBuffer> sharedBuffer(new GLTFBuffer(this->getSharedBufferId(), verticesLength + indicesLength + animationLength));

        //---
        shared_ptr <GLTFBufferView> genericBufferView(new GLTFBufferView(sharedBuffer, 0, animationLength));
        shared_ptr <GLTFBufferView> indicesBufferView(new GLTFBufferView(sharedBuffer, animationLength, indicesLength));
        shared_ptr <GLTFBufferView> verticesBufferView(new GLTFBufferView(sharedBuffer, indicesLength + animationLength, verticesLength));
        shared_ptr <GLTFBufferView> compressionBufferView(new GLTFBufferView(compressionBuffer, 0, compressionLength));
        
        // ----
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            
            GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
            
            bool isCompressed = false;
            if (mesh->getExtensions()) {
                isCompressed = mesh->getExtensions()->contains("Open3DGC-compression");
            }
            
            //serialize attributes
            vector <GLTF::Semantic> allSemantics = mesh->allSemantics();
            for (size_t k = 0 ; k < allSemantics.size() ; k++) {
                GLTF::Semantic semantic = allSemantics[k];
                GLTF::IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
                GLTF::IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = mesh->getMeshAttributesForSemantic(semantic);
                
                for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                    //(*it).first;             // the key value (of type Key)
                    //(*it).second;            // the mapped value (of type T)
                    shared_ptr <GLTF::GLTFAccessor> meshAttribute = (*meshAttributeIterator).second;
                    
                    meshAttribute->setBufferView(isCompressed ? compressionBufferView : verticesBufferView);
                    accessors->setValue(meshAttribute->getID(), meshAttribute);
                }
            }
            
            //serialize indices
            unsigned int primitivesCount =  (unsigned int)primitives.size();
            for (size_t k = 0 ; k < primitivesCount ; k++) {
                shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[k]);
                shared_ptr <GLTF::GLTFAccessor> uniqueIndices =  primitive->getIndices();
                
                GLTFBufferView *bufferView = isCompressed ? (GLTFBufferView*)compressionBufferView.get() : (GLTFBufferView*)indicesBufferView.get();
                
                uniqueIndices->setString(kBufferView, bufferView->getID());
                uniqueIndices->setUnsignedInt32(kType, this->_profile->getGLenumForString("UNSIGNED_SHORT"));
                accessors->setValue(uniqueIndices->getID(), uniqueIndices);
            }
            
            //set the compression buffer view
            if (mesh->getExtensions()->getKeysCount() > 0) {
                if (mesh->getExtensions()->contains("Open3DGC-compression")) {
                    shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->getValue("Open3DGC-compression"));
                    if (compressionObject->contains("compressedData")) {
                        shared_ptr<JSONObject> compressionData = compressionObject->getObject("compressedData");
                        compressionData->setString(kBufferView, compressionBufferView->getID());
                    }
                }
            }
            
        }


        // ----
        shared_ptr <GLTF::JSONObject> skins = this->_root->createObjectIfNeeded("skins");
        std::vector <std::string> skinsUIDs = skins->getAllKeys();
        
        for (size_t skinIndex = 0 ; skinIndex < skinsUIDs.size() ; skinIndex++) {
            
            shared_ptr <GLTFSkin> skin = static_pointer_cast<GLTFSkin>(skins->getObject(skinsUIDs[skinIndex]));
            shared_ptr<JSONArray> joints = skin->getJointsIds();
            shared_ptr<JSONArray> jointsWithOriginalSids(new JSONArray());
            
            //resolve the sid and use the original ones
            std::vector <shared_ptr <JSONValue> > values = joints->values();
            for (size_t i = 0 ; i < values.size() ; i++) {
                shared_ptr<JSONString> jointId = static_pointer_cast<JSONString>(values[i]);
                shared_ptr<JSONObject> node = static_pointer_cast<JSONObject>(this->_uniqueIDToJSONValue[jointId->getString()]);
                if (node->contains("jointId")) {
                    jointsWithOriginalSids->appendValue(static_pointer_cast <JSONValue> (node->getValue("jointId")));
                }
            }
            skin->setJointsIds(jointsWithOriginalSids);
            shared_ptr <JSONObject> inverseBindMatrices = static_pointer_cast<JSONObject>(skin->extras()->getValue(kInverseBindMatrices));
            inverseBindMatrices->setString(kBufferView, genericBufferView->getID());
            skin->setValue(kInverseBindMatrices, inverseBindMatrices);
            skins->setValue(skin->getId(), skin);
            skins->removeValue(skinsUIDs[skinIndex]);
        }
        
        // ----
        animationsUIDs = animations->getAllKeys();
        for (size_t animationIndex = 0 ; animationIndex < animationsUIDs.size() ; animationIndex++) {
            shared_ptr<GLTFAnimation> animation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationsUIDs[animationIndex]));
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
        }
        
        shared_ptr <JSONObject> buffersObject(new JSONObject());
        
        this->_root->setValue("buffers", buffersObject);
        
        if (sharedBuffer->getByteLength() > 0) {
            sharedBuffer->setString(kPath, this->getSharedBufferId() + ".bin");
            sharedBuffer->setString(kType, "arraybuffer");
            buffersObject->setValue(this->getSharedBufferId(), sharedBuffer);
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
        this->_root->setValue("bufferViews", bufferViewsObject);
        
        bufferViewsObject->setValue(indicesBufferView->getID(), indicesBufferView);
        bufferViewsObject->setValue(verticesBufferView->getID(), verticesBufferView);
        if ((animationLength > 0) || (compressionLength > 0)) {
            bufferViewsObject->setValue(genericBufferView->getID(), genericBufferView);
        }
        if (compressionLength > 0) {
            bufferViewsObject->setValue(compressionBufferView->getID(), compressionBufferView);
        }
        
        indicesBufferView->setUnsignedInt32("target", this->_profile->getGLenumForString("ELEMENT_ARRAY_BUFFER"));
        verticesBufferView->setUnsignedInt32("target", this->_profile->getGLenumForString("ARRAY_BUFFER"));
        
        //---
        //this apply MUST be before the removeValue lightsIds that follows
        this->_root->apply(__eval, this);
        this->_root->removeValue("lightsIds");
        
        this->_root->write(&this->_writer, this);
        
        rawOutputStream->close();
        if (compressionLength == 0) {
            this->closeOutputStream(kCompressionOutputStream, true);
        }
        
        if (sharedBuffer->getByteLength() == 0)
            remove(rawOutputStream->outputPathCStr());
                
        this->convertionResults()->setUnsignedInt32("geometry", this->getGeometryByteLength());
        this->convertionResults()->setUnsignedInt32("animation", this->getAnimationByteLength());
        this->convertionResults()->setUnsignedInt32("scene", (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        this->log("[geometry] %d bytes\n", (int)this->getGeometryByteLength());
        this->log("[animations] %d bytes\n", (int)this->getAnimationByteLength());
        this->log("[scene] total bytes:%d\n", (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        this->copyImagesInsideBundleIfNeeded();        
    }
    
}
