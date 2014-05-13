#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"
#include "../GitSHA1.h"
#include "GLTF-Open3DGC.h"
#include "GLTFFlipUVModifier.h"
#include "geometryHelpers.h"

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
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
		totalVerticesCount += (unsigned int)vertexCount;
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
		unsigned compressedBufferStart = (unsigned int)compressionOutputStream->length();
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
				compressedBufferStart += (unsigned int)indicesLength; //we simulate how will be the uncompressed data here, so this is the length in short *on purpose*
            }
        }
        
        shared_ptr<GLTFAccessor> positionAttribute = mesh->getMeshAttribute(GLTF::POSITION, 0);
		vertexCount = (unsigned int)positionAttribute->getCount();
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
				compressedBufferStart += (unsigned int)buffer->getByteLength();
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
        this->_originalResourcesPath = shared_ptr<JSONObject> (new JSONObject());
        this->_convertionMetaData = shared_ptr<JSONObject> (new JSONObject());
        this->_extras = shared_ptr<JSONObject> (new JSONObject());
        this->_meshesForMaterialBindingKey = shared_ptr<JSONObject> (new JSONObject());

        this->_prefix = 0;
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

    void GLTFAsset::setOriginalId(const std::string& uniqueId, const std::string& originalId) {
        this->_uniqueIDToOriginalID[uniqueId] = originalId;
        this->_originalIDToUniqueID[originalId] = uniqueId;
    }
    
    std::string GLTFAsset::getOriginalId(const std::string& uniqueId) {
        return this->_uniqueIDToOriginalID[uniqueId];
    }
    
    std::string GLTFAsset::getUniqueId(const std::string& original) {
        return this->_originalIDToUniqueID[original];
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

    shared_ptr <JSONObject> GLTFAsset::convertionMetaData() {
        return this->_convertionMetaData;
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
            this->_originalResourcesPath->setString(path, resourcePath);
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
        if ((this->_converterConfig->boolForKeyPath("outputProgress") == false) &&
            (this->_converterConfig->boolForKeyPath("outputConvertionResults")) == false) {
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
    
    std::string GLTFAsset::getOutputFolderPath() {
        COLLADABU::URI outputURI(this->getOutputFilePath().c_str());
        std::string folder = outputURI.getPathDir();
        return folder;
    }
    
    void GLTFAsset::setInputFilePath(const std::string& inputFilePath) {
        this->_inputFilePath = inputFilePath;
        
        this->_convertionMetaData->setString("source", inputFilePath);
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
            inputURI.setPathDir(aURI.getPathDir() + inputURI.getPathDir());
            return inputURI.getURIString();
        }
    }
    
    void GLTFAsset::copyImagesInsideBundleIfNeeded() {
        if (this->_isBundle == true) {
            shared_ptr<JSONObject> images = this->_root->createObjectIfNeeded(kImages);
            size_t imagesCount = images->getKeysCount();
            if (imagesCount > 0) {
                std::vector <std::string> keys = images->getAllKeys();
                for (size_t i = 0 ; i < imagesCount ; i++) {
                    shared_ptr<JSONObject> image = images->getObject(keys[i]);
                    std::string path = image->getString("path");
                    
                    std::string originalPath = this->_originalResourcesPath->getString(path);
                    
                    std::string inputImagePath = this->pathRelativeToInputPath(originalPath).c_str();

                    COLLADABU::URI outputImagePathURI(inputImagePath.c_str());
                    
                    COLLADABU::URI outputURI(this->getOutputFilePath().c_str());
                    std::string folder = outputURI.getPathDir();
                    std::string outputPath = folder + outputImagePathURI.getPathFile();
                    
                    std::ifstream f1(inputImagePath.c_str(), std::fstream::binary);
                    std::ofstream f2(outputPath.c_str(), std::fstream::binary);
                    
                    if (this->_converterConfig->boolForKeyPath("verboseLogging")) {
                        this->log("[image]: Copy inside bundle from %s to %s\n", inputImagePath.c_str(), outputPath.c_str());
                    }
                    
                    f2 << f1.rdbuf(); // read original file into target
                }
            }
        }
    }
    
    void GLTFAsset::prepareForProfile(shared_ptr<GLTFProfile> profile) {
        this->_profile = profile;
        this->_root = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
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
    
    void GLTFAsset::_writeJSONResource(const std::string &path, shared_ptr<JSONObject> obj) {
        GLTF::GLTFWriter resultsWriter;
        COLLADABU::URI outputURI(this->resourceOuputPathForPath(path));
        std::string aPath = this->getOutputFolderPath() + outputURI.getPathFile();
        resultsWriter.initWithPath(aPath);
        obj->write(&resultsWriter);
        
        if (this->_converterConfig->boolForKeyPath("verboseLogging")) {
            this->log("[Resource]: write JSON resource at path:%s\n", aPath.c_str());
        }
    }
    
    
    shared_ptr<JSONObject> GLTFAsset::getExtras() {
        return this->_extras;
    }
    
    void GLTFAsset::setExtras(shared_ptr<JSONObject> extras) {
        this->_extras = extras;
    }

    static shared_ptr <GLTF::JSONObject> serializeAttributeSemanticsForPrimitiveAtIndex(GLTFMesh* mesh, unsigned int idx) {
        shared_ptr <GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(mesh->getPrimitives()->values()[idx]);
        
        shared_ptr <GLTF::JSONObject> semantics(new GLTF::JSONObject());
        shared_ptr<JSONArray> sets(new JSONArray());
        
        size_t count = primitive->getVertexAttributesCount();
        for (size_t j = 0 ; j < count ; j++) {
            Semantic semantic = primitive->getSemanticAtIndex((unsigned int)j);
            std::string semanticString = GLTFUtils::getStringForSemantic(semantic);
            
            shared_ptr<JSONObject> semanticInfo;
            if (semantics->contains(semanticString) == false) {
                semanticInfo = shared_ptr<JSONObject> (new JSONObject());
                semantics->setValue(semanticString, semanticInfo);
            }
            
            unsigned int indexOfSet = 0;
            if (semantic == GLTF::TEXCOORD) {
				indexOfSet = primitive->getIndexOfSetAtIndex((unsigned int)j);
            }
            
            sets->appendValue(shared_ptr<JSONNumber> (new JSONNumber(indexOfSet)));
        }
        
        return semantics;
    }

    static shared_ptr<GLTFEffect> createDefaultEffect(GLTFAsset* asset, shared_ptr <JSONObject> attributeSemantics, bool hasNormal) {
        //we have no material, create default
        shared_ptr<GLTFEffect> effect;
        std::string uniqueId = "__glTF__defaultMaterial";
        uniqueId += hasNormal ? "0" : "1";
        
        effect = shared_ptr<GLTFEffect> (new GLTFEffect(uniqueId));
        shared_ptr <JSONObject> values(new JSONObject());
        
        effect->setValues(values);
        effect->setLightingModel("Phong");
        
        //retrieve the type, parameterName -> symbol -> type
        double red = 1, green = 1, blue = 1, alpha = 1;
        shared_ptr <JSONObject> slotObject(new JSONObject());
        slotObject->setValue("value", serializeVec4(red, green, blue, alpha));
        slotObject->setUnsignedInt32("type", asset->profile()->getGLenumForString("FLOAT_VEC4"));
        values->setValue("diffuse", slotObject);
        
        shared_ptr<JSONObject> techniqueGenerator(new JSONObject());
        
        techniqueGenerator->setString("lightingModel", effect->getLightingModel());
        techniqueGenerator->setValue("attributeSemantics", attributeSemantics);
        techniqueGenerator->setValue("values", effect->getValues());
        techniqueGenerator->setValue("techniqueExtras", shared_ptr<JSONObject>(new JSONObject()));
        techniqueGenerator->setValue("texcoordBindings", shared_ptr<JSONObject>(new JSONObject()));
        
        effect->setTechniqueGenerator(techniqueGenerator);
        effect->setName(uniqueId);
        return effect;
    }
    
    static std::string buildKeyForMaterialBindingMap(shared_ptr <MaterialBindingsPrimitiveMap> materialBindingPrimitiveMap) {
        std::string materialBindingKey = "";
        size_t size = materialBindingPrimitiveMap->size();
        
        if (size > 0) {
            MaterialBindingsPrimitiveMap::const_iterator iterator;
            for (iterator = materialBindingPrimitiveMap->begin() ; iterator != materialBindingPrimitiveMap->end() ; iterator++) {
                std::shared_ptr <COLLADAFW::MaterialBinding> materialBinding = iterator->second;
                materialBindingKey += materialBinding->getReferencedMaterial().toAscii();
            }
        }
        
        return materialBindingKey;
    }

    void GLTFAsset::_applyMaterialBindings(shared_ptr<GLTFMesh> mesh,
                                           shared_ptr <MaterialBindingsPrimitiveMap> materialBindingsPrimitiveMap,
                                           shared_ptr <JSONArray> meshesArray,
                                           shared_ptr<JSONObject> meshExtras) {
        
        std::string meshOriginalID = mesh->getID();
        if (materialBindingsPrimitiveMap) {
            std::string materialBindingKey = buildKeyForMaterialBindingMap(materialBindingsPrimitiveMap);
            if (this->_meshesForMaterialBindingKey->contains(meshOriginalID) == false) {
                shared_ptr<JSONObject> meshesForBindingKey = _meshesForMaterialBindingKey->createObjectIfNeeded(meshOriginalID);
                meshesForBindingKey->setValue(materialBindingKey, mesh);
            } else {
                shared_ptr<JSONObject> meshesForBindingKey = _meshesForMaterialBindingKey->getObject(meshOriginalID);
                if (meshesForBindingKey->contains(materialBindingKey)) {
                    meshesArray->appendValue(shared_ptr<JSONString>(new JSONString(meshOriginalID)));
                    return;
                } else {
                    mesh = mesh->clone();
                    mesh->setID(meshOriginalID + "-variant-" + GLTFUtils::toString(meshesForBindingKey->getKeysCount()));
                    meshesForBindingKey->setValue(materialBindingKey, mesh);
                    this->root()->createObjectIfNeeded(kMeshes)->setValue(meshOriginalID, mesh);
                }
            }
        }
        
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        for (size_t j = 0 ; j < primitives.size() ; j++) {
            shared_ptr <GLTFEffect> effect = nullptr;
            shared_ptr<JSONObject> materials = this->root()->createObjectIfNeeded(kMaterials);
            shared_ptr <GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[j]);
            
            if (materialBindingsPrimitiveMap->count(primitive->getMaterialObjectID()) > 0) {
                COLLADAFW::MaterialBinding *materialBinding =  (*materialBindingsPrimitiveMap)[primitive->getMaterialObjectID()].get();
                shared_ptr<JSONObject> texcoordBindings(new JSONObject());
                std::string referencedMaterialID = materialBinding->getReferencedMaterial().toAscii();
                
                /* will be needed to get semantic & set association to create the shader */
                const COLLADAFW::TextureCoordinateBindingArray &textureCoordBindings = materialBinding->getTextureCoordinateBindingArray();
                COLLADAFW::UniqueId effectUID = this->_materialUIDToEffectUID[referencedMaterialID];
                std::string effectID = effectUID.toAscii();
                shared_ptr<JSONObject> effectExtras = this->_extras->contains(effectID) ? this->_extras->getObject(effectID) : nullptr;
                
                std::string materialName = this->_materialUIDToName[referencedMaterialID];
                
                if (this->containsValueForUniqueId(effectID)) {
                    effect = static_pointer_cast<GLTFEffect>(this->getValueForUniqueId(effectID));
                }
                
                // retrieve the semantic to be associated
                size_t coordBindingsCount = textureCoordBindings.getCount();
                if (coordBindingsCount > 0) {
                    //some models come with a setIndex > 0, we do not handle this, we need to find what's the minimum index and substract it to ensure start at set=0
                    size_t minimumIndex = textureCoordBindings[0].getSetIndex();
                    for (size_t coordIdx = 1 ; coordIdx < coordBindingsCount ; coordIdx++) {
                        if (textureCoordBindings[coordIdx].getSetIndex() < minimumIndex)
                            minimumIndex = textureCoordBindings[coordIdx].getSetIndex();
                    }
                    
                    size_t maxCoordsAttributesCount = mesh->getMeshAttributesCountForSemantic(GLTF::TEXCOORD) - 1;
                    for (size_t coordIdx = 0 ; coordIdx < coordBindingsCount ; coordIdx++) {
                        std::string texcoord = textureCoordBindings[coordIdx].getSemantic();
                        SemanticArrayPtr semanticArrayPtr = effect->getSemanticsForTexcoordName(texcoord);
                        
                        //work-around for https://github.com/KhronosGroup/glTF/issues/253
                        size_t setIndex = textureCoordBindings[coordIdx].getSetIndex() - minimumIndex;
                        if (setIndex > maxCoordsAttributesCount)
                            setIndex = maxCoordsAttributesCount;
                        
                        std::string shaderSemantic = "TEXCOORD_"+ GLTFUtils::toString(setIndex);
                        
                        if (semanticArrayPtr) {
                            for (size_t semanticIndex = 0 ; semanticIndex < semanticArrayPtr->size() ; semanticIndex++){
                                std::string slot = (*semanticArrayPtr)[semanticIndex];
                                texcoordBindings->setString(slot, shaderSemantic);
                                if (effectExtras != nullptr) {
                                    if ((slot == "diffuse") && effectExtras->getBool("ambient_diffuse_lock")) {
                                        texcoordBindings->setString("ambient", shaderSemantic);
                                    }
                                }
                            }
                        }
                    }
                }
                
                shared_ptr<JSONObject> techniqueExtras(new JSONObject());
                if ((meshExtras != nullptr) && meshExtras->contains("double_sided")) {
                    techniqueExtras->setBool("double_sided", meshExtras->getBool("double_sided"));
                }
                if ((effectExtras != nullptr) && effectExtras->contains("double_sided")) {
                    techniqueExtras->setBool("double_sided", effectExtras->getBool("double_sided"));
                }
                
                //generate shaders if needed
                shared_ptr <JSONObject> attributeSemantics = serializeAttributeSemanticsForPrimitiveAtIndex(mesh.get(), (unsigned int)j);
                
                shared_ptr<JSONObject> techniqueGenerator(new JSONObject());
                
                techniqueGenerator->setString("lightingModel", effect->getLightingModel());
                techniqueGenerator->setValue("attributeSemantics", attributeSemantics);
                techniqueGenerator->setValue(kValues, effect->getValues());
                techniqueGenerator->setValue("techniqueExtras", techniqueExtras);
                techniqueGenerator->setValue("texcoordBindings", texcoordBindings);
                
                effect->setTechniqueGenerator(techniqueGenerator);
                effect->setName(materialName);
                primitive->setMaterialID(meshOriginalID);
            } else {
                //https://github.com/KhronosGroup/glTF/issues/194
                //We'll deal with two cases cases of default materials
                //With or without NORMALS
                shared_ptr <JSONObject> attributeSemantics = serializeAttributeSemanticsForPrimitiveAtIndex(mesh.get(), (unsigned int)j);
                
                static shared_ptr<GLTFEffect> defaultEffectNoNormal = createDefaultEffect(this, attributeSemantics, false);
                static shared_ptr<GLTFEffect> defaultEffectWithNormal = createDefaultEffect(this, attributeSemantics, true);
                
                bool hasNormal = attributeSemantics->contains(GLTFUtils::getStringForSemantic(GLTF::NORMAL));
                
                effect = hasNormal ? defaultEffectWithNormal : defaultEffectNoNormal;
                
                primitive->setMaterialID(meshOriginalID);
                
                if (materials->contains(meshOriginalID) == false) {
                    materials->setValue(meshOriginalID, effect);
                }
            }
        }
        meshesArray->appendValue(shared_ptr<JSONString>(new JSONString(meshOriginalID)));
    }
    
    bool GLTFAsset::_applyMaterialBindingsForNode(const std::string &nodeUID) {
        
        if (this->containsValueForUniqueId(nodeUID) == false)
            return false;
        
        shared_ptr <JSONObject> node = static_pointer_cast<JSONObject>(this->getValueForUniqueId(nodeUID));
        
        shared_ptr <MaterialBindingsForMeshUID> materialBindings = this->_materialBindingsForNodeUID.count(nodeUID) > 0 ? this->_materialBindingsForNodeUID[nodeUID] : nullptr;
        
        if (materialBindings == nullptr)
            return false;
        
        shared_ptr <JSONArray> meshesArray = nullptr;

        MaterialBindingsForMeshUID::const_iterator materialBindingsIterator;
        for (materialBindingsIterator = materialBindings->begin() ; materialBindingsIterator != materialBindings->end() ; materialBindingsIterator++) {
            
            //------------- Transform & Parse MeshUID ------------------
            //IMPORTANT !! at this point meshUID is formatted as this:
            //"meshes-" + meshUID or "skin-meshes-" + meshUID
            //by parsing the prefix ("meshes-" or "skin-meshes-")
            //we will know if the meshes being processed belong
            //to the meshes array in node or the sources in instanceSkin
            //-----------------------------------------------------------
            std::string meshUID = (*materialBindingsIterator).first;
            
            std::string meshesInNodePrefix = "meshes-";
            std::string meshesInSkinningPrefix = "skin-meshes-";
            bool meshesInNode = false, meshesInSkinning = false;
            
            if (meshUID.find(meshesInNodePrefix) == 0) {
                meshesInNode = true;
                meshUID = meshUID.substr(meshesInNodePrefix.length());
                meshesArray = node->createArrayIfNeeded(kMeshes);
            } else if (meshUID.find(meshesInSkinningPrefix) == 0) {
                assert(node->contains(kInstanceSkin));
                meshesInSkinning = true;
                shared_ptr<JSONObject> instanceSkin = node->getObject(kInstanceSkin);
                meshUID = meshUID.substr(meshesInSkinningPrefix.length());
                meshesArray = instanceSkin->createArrayIfNeeded(kSources);
            }
            
            assert(meshesInSkinning || meshesInNode);
            
            shared_ptr <MaterialBindingsPrimitiveMap> materialBindingsPrimitiveMap = (*materialBindingsIterator).second;
            
            shared_ptr<JSONObject> meshExtras = this->_extras->contains(meshUID) ? this->_extras->getObject(meshUID) : nullptr;
            
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(this->getValueForUniqueId(meshUID));
            
            this->_applyMaterialBindings(mesh, materialBindingsPrimitiveMap, meshesArray, meshExtras);
            JSONValueVectorRef subMeshes = mesh->subMeshes()->values();
            for (size_t subMeshIndex = 0 ; subMeshIndex < subMeshes.size() ; subMeshIndex++) {
                this->_applyMaterialBindings(static_pointer_cast<GLTFMesh>(subMeshes[subMeshIndex]), materialBindingsPrimitiveMap, meshesArray, meshExtras);
            }
        }
        
        return true;
    }

    void GLTFAsset::write() {
        ifstream inputCompression;
        
        if (this->_converterConfig->boolForKeyPath("verboseLogging")) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                this->log("[Info]: current working directory:%s\n", cwd);
        }
        
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
        
        //here we will split meshes as needed or just pass through
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            const std::string &meshUID = meshesUIDs[i];
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshUID));
            if (mesh) {
                shared_ptr<GLTFMesh> targetMesh = createMeshWithMaximumIndicesCountFromMeshIfNeeded(mesh.get(), 65535, this->profile());
                if (targetMesh != nullptr) {
                    meshes->setValue(targetMesh->getID(), targetMesh);
                    this->setValueForUniqueId(this->getUniqueId(targetMesh->getID()), targetMesh);
                    JSONValueVectorRef meshesVector = targetMesh->subMeshes()->values();
                    size_t meshesCount = meshesVector.size();
                    if (meshesCount > 0) {
                        for (size_t j = 0 ; j < meshesCount ; j++) {
                            mesh = static_pointer_cast<GLTFMesh>(meshesVector[j]);
                            meshes->setValue(mesh->getID(), mesh);
                        }
                    }
                }
            }
        }
        
        //Handle late binding of material in node
        //So we go through all nodes and if a mesh got different bindings than the ones needed we clone the "reference" mesh and assign the binding
        //we delay this operation to now, so that we get the reference mesh splitted.
        shared_ptr <GLTF::JSONObject> nodes = this->root()->createObjectIfNeeded(kNodes);
        
        std::vector <std::string> nodesOriginalIds = nodes->getAllKeys();
        for (size_t i = 0 ; i < nodesOriginalIds.size() ; i++) {
            std::string nodeUID = this->getUniqueId(nodesOriginalIds[i]);
            this->_applyMaterialBindingsForNode(nodeUID);
        }
        
        meshesUIDs = meshes->getAllKeys();
        
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            mesh->resolveAttributes();
            GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
            unsigned int primitivesCount =  (unsigned int)primitives.size();
            //WORK-AROUND: we don't want meshes without material, which can happen if a mesh is not associated with a node.
            //In this case, the material binding - isn't resolved.
            for (unsigned int k = 0 ; k < primitivesCount ; k++) {
                shared_ptr<GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[k]);
                if (primitive->contains(kMaterial) ==  false) {
                    meshes->removeValue(mesh->getID());
                    break;
                }
            }
        }
        
        //Meshes may have changed ids here, get keys again.
        meshesUIDs = meshes->getAllKeys();

        // ----
        
        shared_ptr <GLTF::JSONObject> materials = this->_root->createObjectIfNeeded(kMaterials);
        vector <std::string> materialUIDs = materials->getAllKeys();
        for (size_t i = 0 ; i < materialUIDs.size() ; i++) {
            shared_ptr <GLTF::GLTFEffect> material = static_pointer_cast<GLTFEffect>(materials->getObject(materialUIDs[i]));
            if (!material->getTechniqueGenerator()) {
                materials->removeValue(material->getID());
            }
        }
        
        this->assetModifiers().push_back(shared_ptr<GLTFFlipUVModifier> (new GLTFFlipUVModifier()));
        
        this->launchModifiers();
        
        size_t verticesLength = 0;
        size_t indicesLength = 0;
        size_t animationLength = rawOutputStream->length();
        size_t previousLength = animationLength;
                
        //save all meshes as compressed
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            bool compressMesh = (CONFIG_STRING(this, "compressionType") == "Open3DGC") && canEncodeOpen3DGCMesh(mesh,this->_profile);
            if (compressMesh)
                writeCompressedMesh(mesh, this);
        }
        size_t compressionLength = compressionOutputStream->length();
        shared_ptr <GLTFBuffer> compressionBuffer(new GLTFBuffer(compressionOutputStream->id(), compressionLength));
        
        //save all indices
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            bool compressMesh = (CONFIG_STRING(this, "compressionType") == "Open3DGC") && canEncodeOpen3DGCMesh(mesh,this->_profile);
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
            bool compressMesh = (CONFIG_STRING(this, "compressionType") == "Open3DGC") && canEncodeOpen3DGCMesh(mesh,this->_profile);
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
            if (mesh->contains(kExtensions)) {
                isCompressed = mesh->getExtensions()->contains("Open3DGC-compression");
            }
            
            //serialize attributes
            vector <GLTF::Semantic> allSemantics = mesh->allSemantics();
            for (size_t k = 0 ; k < allSemantics.size() ; k++) {
                GLTF::Semantic semantic = allSemantics[k];
                size_t attributesCount = mesh->getMeshAttributesCountForSemantic(semantic);
                
                for (size_t j = 0 ; j < attributesCount ; j++) {
                    shared_ptr <GLTF::GLTFAccessor> meshAttribute = mesh->getMeshAttribute(semantic, j);
                    
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
            if (mesh->contains(kExtensions)) {
                shared_ptr<JSONObject> compressionData = static_pointer_cast<JSONObject>(mesh->valueForKeyPath("extensions.Open3DGC-compression.compressedData"));
                if (compressionData) {
                    compressionData->setString(kBufferView, compressionBufferView->getID());
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
                shared_ptr<JSONObject> target = channel->getObject(kTarget);
                std::string originalID = this->_uniqueIDToOriginalID[target->getString("id")];
                target->setString("id", originalID);
            }
            
            std::vector <std::string> parameterKeys = parameters->getAllKeys();
            for (size_t i = 0 ; i <parameterKeys.size() ; i++) {
                std::string parameterUID = parameters->getString(parameterKeys[i]);
                shared_ptr <JSONObject> parameterObject = accessors->getObject(parameterUID);
                shared_ptr <JSONObject> compressedData = static_pointer_cast<JSONObject>(parameterObject->valueForKeyPath("extensions.Open3DGC-compression.compressedData"));
                if (compressedData) {
                    compressedData->setString(kBufferView, compressionBufferView->getID());
                }
                parameterObject->setString(kBufferView, genericBufferView->getID());
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
        shared_ptr <JSONObject> bufferViews = this->_root->createObjectIfNeeded(kBufferViews);
        
        bufferViews->setValue(indicesBufferView->getID(), indicesBufferView);
        bufferViews->setValue(verticesBufferView->getID(), verticesBufferView);
        if ((animationLength > 0) || (compressionLength > 0)) {
            bufferViews->setValue(genericBufferView->getID(), genericBufferView);
        }
        if (compressionLength > 0) {
            bufferViews->setValue(compressionBufferView->getID(), compressionBufferView);
        }
        
        indicesBufferView->setUnsignedInt32(kTarget, this->_profile->getGLenumForString("ELEMENT_ARRAY_BUFFER"));
        verticesBufferView->setUnsignedInt32(kTarget, this->_profile->getGLenumForString("ARRAY_BUFFER"));
        
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
                
		this->convertionResults()->setUnsignedInt32(kGeometry, (unsigned int)this->getGeometryByteLength());
		this->convertionResults()->setUnsignedInt32(kAnimation, (unsigned int)this->getAnimationByteLength());
        this->convertionResults()->setUnsignedInt32(kScene, (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        this->log("[geometry] %d bytes\n", (int)this->getGeometryByteLength());
        this->log("[animations] %d bytes\n", (int)this->getAnimationByteLength());
        this->log("[scene] total bytes:%d\n", (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        this->copyImagesInsideBundleIfNeeded();
        
        if (this->converterConfig()->boolForKeyPath("outputConvertionResults")) {
            this->_writeJSONResource("results.json", this->convertionResults());
        }
        
        if (this->_isBundle || this->converterConfig()->boolForKeyPath("outputConvertionMetaData")) {
            this->_writeJSONResource("scene.bld", this->convertionMetaData());
        }
    }
    
}
