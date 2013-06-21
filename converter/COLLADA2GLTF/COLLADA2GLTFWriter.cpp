// Copyright (c) 2012, Motorola Mobility, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of the Motorola Mobility, Inc. nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// reminder; to run recursively against all dae files: find . -name '*.dae' -exec dae2json {} \;
#include "COLLADA2GLTFWriter.h"
#include "GLTFExtraDataHandler.h"

using namespace std::tr1;
using namespace std;
using namespace COLLADAFW;
using namespace COLLADABU;
using namespace COLLADASaxFWL;
using namespace rapidjson;

namespace GLTF
{
    //--------------------------------------------------------------------
	COLLADA2GLTFWriter::COLLADA2GLTFWriter( const GLTFConverterContext &converterArgs, PrettyWriter <FileStream> *jsonWriter ):
    _converterContext(converterArgs),
    _visualScene(0)
	{
        this->_writer.setWriter(jsonWriter);
	}
    
	//--------------------------------------------------------------------
	COLLADA2GLTFWriter::~COLLADA2GLTFWriter()
	{
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::reportError( const std::string& method, const std::string& message)
	{
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::write()
	{
        ifstream inputVertices;
        ifstream inputIndices;
        ifstream inputAnimations;
        ifstream inputCompressedData;
        ofstream ouputStream;
        
        this->_extraDataHandler = new ExtraDataHandler();

        this->_converterContext.shaderIdToShaderString.clear();
        this->_converterContext._uniqueIDToMeshes.clear();

        /*
         1. We output vertices and indices separatly in 2 different files
         2. Then output them in a single file
         */
        COLLADABU::URI inputURI(this->_converterContext.inputFilePath.c_str());
        COLLADABU::URI outputURI(this->_converterContext.outputFilePath.c_str());
        
        std::string sharedVerticesBufferID = inputURI.getPathFileBase() + "vertices" + ".bin";
        std::string sharedIndicesBufferID = inputURI.getPathFileBase() + "indices" + ".bin";
        std::string sharedAnimationsBufferID = inputURI.getPathFileBase() + "animations" + ".bin";
        std::string sharedCompressedDataBufferID = inputURI.getPathFileBase() + "compressedData" + ".bin";
        std::string sharedBufferID = inputURI.getPathFileBase() + ".bin";
        std::string outputVerticesFilePath = outputURI.getPathDir() + sharedVerticesBufferID;
        std::string outputIndicesFilePath = outputURI.getPathDir() + sharedIndicesBufferID;
        std::string outputAnimationsFilePath = outputURI.getPathDir() + sharedAnimationsBufferID;
        std::string outputCompressedDataFilePath = outputURI.getPathDir() + sharedCompressedDataBufferID;
        std::string outputFilePath = outputURI.getPathDir() + sharedBufferID;
        
        this->_verticesOutputStream.open (outputVerticesFilePath.c_str(), ios::out | ios::ate | ios::binary);
        this->_indicesOutputStream.open (outputIndicesFilePath.c_str(), ios::out | ios::ate | ios::binary);
        this->_animationsOutputStream.open (outputAnimationsFilePath.c_str(), ios::out | ios::ate | ios::binary);
        this->_compressedDataOutputStream.open (outputCompressedDataFilePath.c_str(), ios::out | ios::ate | ios::binary);
        ouputStream.open (outputFilePath.c_str(), ios::out | ios::ate | ios::binary);
        
        this->_converterContext.root = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
        this->_converterContext.root->setString("profile", "WebGL 1.0");
        this->_converterContext.root->setString("version", "0.3");
        this->_converterContext.root->setValue("nodes", shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject()));
        
        COLLADASaxFWL::Loader loader;
		COLLADAFW::Root root(&loader, this);
        
        loader.registerExtraDataCallbackHandler(this->_extraDataHandler);
		if (!root.loadDocument( this->_converterContext.inputFilePath))
			return false;
        
        
        //reopen .bin files for vertices and indices
        size_t verticesLength = this->_verticesOutputStream.tellp();
        size_t indicesLength = this->_indicesOutputStream.tellp();
        size_t compressedDataLength = this->_compressedDataOutputStream.tellp();
        size_t animationsLength = this->_animationsOutputStream.tellp();
        
        this->_verticesOutputStream.flush();
        this->_verticesOutputStream.close();
        this->_indicesOutputStream.flush();
        this->_indicesOutputStream.close();
        this->_compressedDataOutputStream.flush();
        this->_compressedDataOutputStream.close();
        this->_animationsOutputStream.flush();
        this->_animationsOutputStream.close();
        
        inputVertices.open(outputVerticesFilePath.c_str(), ios::in | ios::binary);
        inputIndices.open(outputIndicesFilePath.c_str(), ios::in | ios::binary);
        inputAnimations.open(outputAnimationsFilePath.c_str(), ios::in | ios::binary);
        inputCompressedData.open(outputCompressedDataFilePath.c_str(), ios::in | ios::binary);
        
        char* bufferIOStream = (char*)malloc(sizeof(char) * verticesLength);
        inputVertices.read(bufferIOStream, verticesLength);
        ouputStream.write(bufferIOStream, verticesLength);
        free(bufferIOStream);
        bufferIOStream = (char*)malloc(sizeof(char) * indicesLength);
        inputIndices.read(bufferIOStream, indicesLength);
        ouputStream.write(bufferIOStream, indicesLength);
        free(bufferIOStream);
        bufferIOStream = (char*)malloc(sizeof(char) * animationsLength);
        inputAnimations.read(bufferIOStream, animationsLength);
        ouputStream.write(bufferIOStream, animationsLength);
        free(bufferIOStream);

        bufferIOStream = (char*)malloc(sizeof(char) * compressedDataLength);
        inputCompressedData.read(bufferIOStream, compressedDataLength);
        ouputStream.write(bufferIOStream, compressedDataLength);
        free(bufferIOStream);

        inputVertices.close();
        inputIndices.close();
        inputAnimations.close();
        inputCompressedData.close();
        
        remove(outputIndicesFilePath.c_str());
        remove(outputVerticesFilePath.c_str());
        remove(outputAnimationsFilePath.c_str());
        remove(outputCompressedDataFilePath.c_str());
        
        //---
        
        shared_ptr <GLTFBuffer> sharedBuffer(new GLTFBuffer(sharedBufferID, verticesLength + indicesLength + animationsLength));
        
        shared_ptr <GLTFBufferView> verticesBufferView(new GLTFBufferView(sharedBuffer, 0, verticesLength));
        shared_ptr <GLTFBufferView> indicesBufferView(new GLTFBufferView(sharedBuffer, verticesLength, indicesLength));
        shared_ptr <GLTFBufferView> animationsBufferView(new GLTFBufferView(sharedBuffer, verticesLength + indicesLength, animationsLength));
        shared_ptr <GLTFBufferView> compressedDataBufferView(new GLTFBufferView(sharedBuffer, verticesLength + indicesLength + animationsLength, compressedDataLength));
        
        // ----
        UniqueIDToMeshes::const_iterator UniqueIDToMeshesIterator;
        shared_ptr <GLTF::JSONObject> meshesObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("meshes", meshesObject);
        
        shared_ptr <GLTF::JSONObject> attributes = this->_converterContext.root->createObjectIfNeeded("attributes");
        shared_ptr <GLTF::JSONObject> indices = this->_converterContext.root->createObjectIfNeeded("indices");

        for (UniqueIDToMeshesIterator = this->_converterContext._uniqueIDToMeshes.begin() ; UniqueIDToMeshesIterator != this->_converterContext._uniqueIDToMeshes.end() ; UniqueIDToMeshesIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            MeshVectorSharedPtr meshes = (*UniqueIDToMeshesIterator).second;
            if (meshes) {
                for (size_t j = 0 ; j < meshes->size() ; j++) {
                    shared_ptr<GLTFMesh> mesh = (*meshes)[j];
                    if (mesh) {
                        /* some exporter bring meshes not used in the scene graph,
                         for the moment we have to remove these meshes because this involves the side effect of not having a material assigned. Which makes it incomplete.
                         */
                        bool shouldSkipMesh = false;
                        PrimitiveVector primitives = mesh->getPrimitives();
                        for (size_t k = 0 ; ((shouldSkipMesh == false) && (k < primitives.size())) ; k++) {
                            shared_ptr <GLTF::GLTFPrimitive> primitive = primitives[k];
                            if (primitive->getMaterialID().length() == 0)
                                shouldSkipMesh  = true;
                        }
                        if (shouldSkipMesh)
                            continue;
                        
                        void *buffers[3];
                        buffers[0] = (void*)verticesBufferView.get();
                        buffers[1] = (void*)indicesBufferView.get();
                        buffers[2] = (void*)compressedDataBufferView.get();
                        
                        shared_ptr <GLTF::JSONObject> meshObject = serializeMesh(mesh.get(), (void*)buffers);

                        //serialize attributes
                        vector <GLTF::Semantic> allSemantics = mesh->allSemantics();
                        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
                            GLTF::Semantic semantic = allSemantics[i];
                            
                            GLTF::IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
                            GLTF::IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = mesh->getMeshAttributesForSemantic(semantic);
                            
                            //FIXME: consider turn this search into a method for mesh
                            for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                                //(*it).first;             // the key value (of type Key)
                                //(*it).second;            // the mapped value (of type T)
                                shared_ptr <GLTF::GLTFMeshAttribute> meshAttribute = (*meshAttributeIterator).second;
                                
                                shared_ptr <GLTF::JSONObject> meshAttributeObject = serializeMeshAttribute(meshAttribute.get(), (void*)buffers);
                                
                                attributes->setValue(meshAttribute->getID(), meshAttributeObject);
                            }
                        }
                        
                        //serialize indices
                        primitives = mesh->getPrimitives();
                        unsigned int primitivesCount =  (unsigned int)primitives.size();
                        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
                            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
                            shared_ptr <GLTF::GLTFIndices> uniqueIndices =  primitive->getUniqueIndices();
                            
                            shared_ptr <GLTF::JSONObject> serializedIndices = serializeIndices(uniqueIndices.get(), (void*)buffers);
                            indices->setValue(uniqueIndices->getID(), serializedIndices);
                        }
                        
                        meshesObject->setValue(mesh->getID(), meshObject);
                    }
                }
            }
        }
        
        // ----
        shared_ptr <GLTF::JSONObject> materialsObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("materials", materialsObject);
        
        UniqueIDToEffect::const_iterator UniqueIDToEffectIterator;
        
        for (UniqueIDToEffectIterator = this->_converterContext._uniqueIDToEffect.begin() ; UniqueIDToEffectIterator != this->_converterContext._uniqueIDToEffect.end() ; UniqueIDToEffectIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <GLTF::GLTFEffect> effect = (*UniqueIDToEffectIterator).second;
            if (effect->getTechniqueID() != "") {
                shared_ptr <GLTF::JSONObject> effectObject = serializeEffect(effect.get(), 0);
                //FIXME:HACK: effects are exported as materials
                materialsObject->setValue(effect->getID(), effectObject);
            }
        }
        
        // ----
        UniqueIDToAnimation::const_iterator UniqueIDToAnimationsIterator;
        shared_ptr <GLTF::JSONObject> animationsObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("animations", animationsObject);
        
        for (UniqueIDToAnimationsIterator = this->_converterContext._uniqueIDToAnimation.begin() ; UniqueIDToAnimationsIterator != this->_converterContext._uniqueIDToAnimation.end() ; UniqueIDToAnimationsIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            
            shared_ptr<GLTFAnimation> animation = (*UniqueIDToAnimationsIterator).second;
            std::vector <shared_ptr <GLTFAnimation::Parameter> > *parameters = animation->parameters();
            for (size_t i = 0 ; i < animation->parameters()->size() ; i++) {
                shared_ptr <GLTFAnimation::Parameter> parameter = (*parameters)[i];
                parameter->setBufferView(animationsBufferView);
            }
            
            if (animation->channels()->values().size() > 0) {
                shared_ptr <JSONObject> animationObject = serializeAnimation(animation.get());
            
                animationsObject->setValue(animation->getID(), animationObject);
            }
        }
        
        shared_ptr <JSONObject> buffersObject(new JSONObject());
        shared_ptr <JSONObject> bufferObject = serializeBuffer(sharedBuffer.get(), 0);
        
        this->_converterContext.root->setValue("buffers", buffersObject);
        buffersObject->setValue(sharedBufferID, bufferObject);
        
        //FIXME: below is an acceptable short-cut since in this converter we will always create one buffer view for vertices and one for indices.
        //Fabrice: Other pipeline tools should be built on top of the format manipulate the buffers and end up with a buffer / bufferViews layout that matches the need of a given application for performance. For instance we might want to concatenate a set of geometry together that come from different file and call that a "level" for a game.
        shared_ptr <JSONObject> bufferViewsObject(new JSONObject());
        this->_converterContext.root->setValue("bufferViews", bufferViewsObject);
        
        shared_ptr <JSONObject> bufferViewIndicesObject = serializeBufferView(indicesBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewVerticesObject = serializeBufferView(verticesBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewAnimationsObject = serializeBufferView(animationsBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewCompressedDataObject = serializeBufferView(compressedDataBufferView.get(), 0);
        bufferViewsObject->setValue(indicesBufferView->getID(), bufferViewIndicesObject);
        bufferViewsObject->setValue(verticesBufferView->getID(), bufferViewVerticesObject);
        if (animationsLength > 0) {
            bufferViewsObject->setValue(animationsBufferView->getID(), bufferViewAnimationsObject);
        }
        if (compressedDataLength > 0) {
            bufferViewsObject->setValue(compressedDataBufferView->getID(), bufferViewCompressedDataObject);
        }
        
        bufferViewIndicesObject->setString("target", "ELEMENT_ARRAY_BUFFER");
        bufferViewVerticesObject->setString("target", "ARRAY_BUFFER");
        
        //---
        
        this->_converterContext.root->write(&this->_writer);
        
        bool sceneFlatteningEnabled = false;
        if (sceneFlatteningEnabled) {
            //second pass to actually output the JSON of the node themselves (and for all sub nodes)
            processSceneFlatteningInfo(&this->_sceneFlatteningInfo);
        }
        
        ouputStream.flush();
        ouputStream.close();
        
        delete this->_extraDataHandler;
        
		return true;
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::cancel( const std::string& errorMessage )
	{
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::start()
	{
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::finish()
	{
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeGlobalAsset( const COLLADAFW::FileInfo* asset )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
    
    static void __GetFloatArrayFromMatrix(const COLLADABU::Math::Matrix4 &matrix, float *m) {
        shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());
        
        for (int i = 0 ; i < 4 ; i++)  {
            const COLLADABU::Math::Real * real = matrix[i];
            
            m[(i*4) + 0] = real[0];
            m[(i*4) + 1] = real[1];
            m[(i*4) + 2] = real[2];
            m[(i*4) + 3] = real[3];
        }
    }
    
    shared_ptr <GLTF::JSONArray> COLLADA2GLTFWriter::serializeMatrix4Array(const COLLADABU::Math::Matrix4 &matrix)
    {
        float m[16];
        shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());
        COLLADABU::Math::Matrix4 transpose = matrix.transpose();
        
        __GetFloatArrayFromMatrix(transpose, m);
        
        for (int i = 0 ; i < 16; i++)  {
            array->appendValue(shared_ptr <GLTF::JSONValue> (new GLTF::JSONNumber(m[i])));
        }
        
        return array;
    }
    
    float COLLADA2GLTFWriter::getTransparency(const COLLADAFW::EffectCommon* effectCommon)
    {
        //super naive for now, also need to check sketchup work-around
        if (effectCommon->getOpacity().isTexture()) {
            return 1;
        }
        float transparency = static_cast<float>(effectCommon->getOpacity().getColor().getAlpha());
        
        return this->_converterContext.invertTransparency ? 1 - transparency : transparency;
    }
    
    float COLLADA2GLTFWriter::isOpaque(const COLLADAFW::EffectCommon* effectCommon)
    {
        return getTransparency(effectCommon)  >= 1;
    }
    
    void COLLADA2GLTFWriter::registerObjectWithUniqueUID(std::string objectUID, shared_ptr <JSONObject> obj, shared_ptr <JSONObject> objLib)
    {
        if (this->_converterContext._uniqueIDToTrackedObject.count(objectUID) == 0) {
            if (!objLib->contains(objectUID)) {
                objLib->setValue(objectUID, obj);
                this->_converterContext._uniqueIDToTrackedObject[objectUID] = obj;
            } else {
                printf("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", objectUID.c_str());
            }
        } else {
            printf("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", objectUID.c_str());
        }
    }

    bool COLLADA2GLTFWriter::writeNode( const COLLADAFW::Node* node,
                                       shared_ptr <GLTF::JSONObject> nodesObject,
                                       COLLADABU::Math::Matrix4 parentMatrix,
                                       SceneFlatteningInfo* sceneFlatteningInfo)
    {
        bool shouldExportTRS = false;
        const NodePointerArray& nodes = node->getChildNodes();
        const std::string& nodeUID = uniqueIdWithType("node", node->getUniqueId());
        COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
        
        shared_ptr <GLTF::JSONObject> nodeObject(new GLTF::JSONObject());
        nodeObject->setString("name",node->getName());
        
        bool nodeContainsLookAtTr = false;
        const InstanceCameraPointerArray& instanceCameras = node->getInstanceCameras();
        size_t camerasCount = instanceCameras.getCount();
        if (camerasCount > 0) {
            InstanceCamera* instanceCamera = instanceCameras[0];
            shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
            
            std::string cameraId = uniqueIdWithType("camera", instanceCamera->getInstanciatedObjectId());
            nodeObject->setString("camera", cameraId);
            
            //FIXME: just handle the first camera within a node now
            //for (size_t i = 0 ; i < camerasCount ; i++) {
            //}
            //Checks if we have a "look at" transformm because it is not handled by getTransformationMatrix when baking the matrix. (TODO: file a OpenCOLLADA issue for that).
            const TransformationPointerArray& transformations = node->getTransformations();
            size_t transformationsCount = transformations.getCount();
            for (size_t i = 0 ; i < transformationsCount ; i++) {
                const Transformation* tr = transformations[i];
                
                if (tr->getTransformationType() == Transformation::LOOKAT) {
                    Lookat* lookAt = (Lookat*)tr;
                    buildLookAtMatrix(lookAt, matrix);
                    nodeContainsLookAtTr = true;
                    break;
                }
            }
            
            if (nodeContainsLookAtTr && (transformationsCount > 1)) {
                //FIXME: handle warning/error
                printf("WARNING: node contains a look at transform combined with other transforms\n");
            }
        }
        
        if (!nodeContainsLookAtTr) {
            node->getTransformationMatrix(matrix);
        }
        
        float scale[3];
        float translation[3];
        float rotation[4];
        
        const TransformationPointerArray& transformations = node->getTransformations();
        size_t transformationsCount = transformations.getCount();
        for (size_t i = 0 ; i < transformationsCount ; i++) {
            const Transformation* tr = transformations[i];
            const UniqueId& animationListID = tr->getAnimationList();
            if (!animationListID.isValid())
                continue;
            shared_ptr<AnimatedTargets> animatedTargets(new AnimatedTargets());
            this->_converterContext._uniqueIDToAnimatedTargets[animationListID.getObjectId()] = animatedTargets;
            shared_ptr <JSONObject> animatedTarget(new JSONObject());
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::MATRIX)  {
                animatedTarget->setString("target", nodeUID);
                animatedTarget->setString("path", "MATRIX");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::TRANSLATE)  {
                animatedTarget->setString("target", nodeUID);
                animatedTarget->setString("path", "translation");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }

            
            if (tr->getTransformationType() == COLLADAFW::Transformation::ROTATE)  {
                animatedTarget->setString("target", nodeUID);
                animatedTarget->setString("path", "rotation");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
        }
        
        
        const COLLADABU::Math::Matrix4 worldMatrix = parentMatrix * matrix;
                
        if (shouldExportTRS) {
            GLTF::decomposeMatrix(matrix, translation, rotation, scale);
            
            nodeObject->setValue("translation", serializeVec3(translation[0], translation[1], translation[2]));
            nodeObject->setValue("rotation", serializeVec4(rotation[0], rotation[1], rotation[2], rotation[3]));
            nodeObject->setValue("scale", serializeVec3(scale[0], scale[1], scale[2]));
            
        } else {
            nodeObject->setValue("matrix", this->serializeMatrix4Array(matrix));
        }
        
        
        // save mesh
		const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();
        
        unsigned int count = (unsigned int)instanceGeometries.getCount();
        if (count > 0) {
            shared_ptr <GLTF::JSONArray> meshesArray(new GLTF::JSONArray());
            nodeObject->setValue("meshes", meshesArray);
            
            for (unsigned int i = 0 ; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                
                COLLADAFW::UniqueId uniqueId = instanceGeometry->getInstanciatedObjectId();
                shared_ptr<JSONObject> meshExtras = this->_extraDataHandler->getExtras(uniqueId);
                
                unsigned int meshUID = (unsigned int)uniqueId.getObjectId();
                MeshVectorSharedPtr meshes = this->_converterContext._uniqueIDToMeshes[meshUID];
                
                if (meshes)
                {
                    for (size_t meshIndex = 0 ; meshIndex < meshes->size() ; meshIndex++) {
                        shared_ptr <GLTFMesh> mesh = (*meshes)[meshIndex];
                        
                        if (!mesh) {
                            continue;
                        }
                        
                        if (sceneFlatteningInfo) {
                            GLTF::IndexSetToMeshAttributeHashmap& semanticMap = mesh->getMeshAttributesForSemantic(GLTF::POSITION);
                            shared_ptr <GLTF::GLTFMeshAttribute> vertexMeshAttribute = semanticMap[0];
                            
                            BBOX vertexBBOX(COLLADABU::Math::Vector3(vertexMeshAttribute->getMin()),
                                            COLLADABU::Math::Vector3(vertexMeshAttribute->getMax()));
                            vertexBBOX.transform(worldMatrix);
                            
                            sceneFlatteningInfo->sceneBBOX.merge(&vertexBBOX);
                        }
                        
                        PrimitiveVector primitives = mesh->getPrimitives();
                        for (size_t j = 0 ; j < primitives.size() ; j++) {
                            shared_ptr <GLTF::GLTFPrimitive> primitive = primitives[j];
                            
                            //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
                            int materialBindingIndex = -1;
                            for (size_t k = 0; k < materialBindings.getCount() ; k++) {
//                                printf("materialID in binding:%d primitveObjectID:%d\n", materialBindings[k].getMaterialId(),
                                    //   primitive->getMaterialObjectID());
                                
                                if (materialBindings[k].getMaterialId() == primitive->getMaterialObjectID()) {
                                    materialBindingIndex = (unsigned int)k;
                                }
                            }
                            
                            std::map<std::string , std::string > texcoordBindings;
                            
                            if (materialBindingIndex != -1) {
                                unsigned int referencedMaterialID = (unsigned int)materialBindings[materialBindingIndex].getReferencedMaterial().getObjectId();
                                
                                /* will be needed to get semantic & set association to create the shader */
                                const TextureCoordinateBindingArray &textureCoordBindings = materialBindings[materialBindingIndex].getTextureCoordinateBindingArray();
                                
                                COLLADAFW::UniqueId effectUID = this->_converterContext._materialUIDToEffectUID[referencedMaterialID];
                                unsigned int effectID = effectUID.getObjectId();
                                shared_ptr<JSONObject> effectExtras = this->_extraDataHandler->getExtras(effectUID);
                                
                                std::string materialName = this->_converterContext._materialUIDToName[referencedMaterialID];
                                shared_ptr <GLTFEffect> effect = this->_converterContext._uniqueIDToEffect[effectID];
                                
                                // retrieve the semantic to be associated
                                size_t coordBindingsCount = textureCoordBindings.getCount();
                                if (coordBindingsCount > 0) {
                                    //some models come with a setIndex > 0, we do not handle this, we need to find what's the minimum index and substract it to ensure start at set=0
                                    size_t minimumIndex = textureCoordBindings[0].getSetIndex();
                                    for (size_t coordIdx = 1 ; coordIdx < coordBindingsCount ; coordIdx++) {
                                        if (textureCoordBindings[coordIdx].getSetIndex() < minimumIndex)
                                            minimumIndex = textureCoordBindings[coordIdx].getSetIndex();
                                    }
                                    
                                    for (size_t coordIdx = 0 ; coordIdx < coordBindingsCount ; coordIdx++) {
                                        std::string texcoord = textureCoordBindings[coordIdx].getSemantic();
                                        SemanticArrayPtr semanticArrayPtr = effect->getSemanticsForTexcoordName(texcoord);
                                        
                                        std::string shaderSemantic = "TEXCOORD_"+ GLTFUtils::toString(textureCoordBindings[coordIdx].getSetIndex() - minimumIndex);
                                        
                                        if (semanticArrayPtr) {
                                            for (size_t semanticIndex = 0 ; semanticIndex < semanticArrayPtr->size() ; semanticIndex++){
                                                std::string slot = (*semanticArrayPtr)[semanticIndex];
                                                texcoordBindings[slot] = shaderSemantic;
                                            }
                                        }
                                    }
                                }
                                
                                shared_ptr<JSONObject> techniqueExtras(new JSONObject());
                                if (meshExtras->contains("double_sided")) {
                                    techniqueExtras->setBool("double_sided", meshExtras->getBool("double_sided"));
                                }
                                if (effectExtras->contains("double_sided")) {
                                    techniqueExtras->setBool("double_sided", effectExtras->getBool("double_sided"));
                                }
                                
                                //generate shaders if needed
                                const std::string& techniqueID = GLTF::getReferenceTechniqueID(effect->getValues(),
                                                                                         techniqueExtras,
                                                                                         texcoordBindings,
                                                                                         this->_converterContext);
                                
                                effect->setTechniqueID(techniqueID);
                                effect->setName(materialName);
                                primitive->setMaterialID(effect->getID());
                            }
                            //printf(":::: %s\n",primitive->getMaterialID().c_str());

                        }


                        meshesArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(mesh->getID())));
                        if (sceneFlatteningInfo) {
                            shared_ptr <MeshFlatteningInfo> meshFlatteningInfo(new MeshFlatteningInfo(meshUID, parentMatrix));
                            sceneFlatteningInfo->allMeshes.push_back(meshFlatteningInfo);
                        }
                    }
                }
            }
        }
        
        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        nodeObject->setValue("children", childrenArray);
        
        count = (unsigned int)nodes.getCount();
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            std::string id = uniqueIdWithType("node", nodes[i]->getUniqueId());
            childrenArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(id)));
        }
        
        registerObjectWithUniqueUID(nodeUID, nodeObject, nodesObject);
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            this->writeNode(nodes[i], nodesObject, worldMatrix, sceneFlatteningInfo);
        }
        
        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = (unsigned int)instanceNodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++) {
            InstanceNode* instanceNode  = instanceNodes[i];
            
            std::string id = uniqueIdWithType("node", instanceNode->getInstanciatedObjectId());
            childrenArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(id)));
        }
        
        return true;
    }
    
    // Flattening   [UNFINISHED CODE]
    //conditions for flattening
    // -> same material
    // option to merge of not non-opaque geometry
    //  -> check per primitive that sources / semantic layout matches
    // [meshAttributes]
    // -> for all meshes
    //   -> collect all kind of semantic
    // -> for all meshes
    //   -> get all meshAttributes
    //   -> transforms & write vtx attributes
    bool COLLADA2GLTFWriter::processSceneFlatteningInfo(SceneFlatteningInfo* sceneFlatteningInfo)
    {
        /*
         MeshFlatteningInfoVector allMeshes = sceneFlatteningInfo->allMeshes;
         //First collect all kind of meshAttributes available
         size_t count = allMeshes.size();
         for (size_t i = 0 ; i < count ; i++) {
         shared_ptr <MeshFlatteningInfo> meshInfo = allMeshes[i];
         MeshVectorSharedPtr *meshes = this->_uniqueIDToMeshes[meshInfo->getUID()];
         // shared_ptr <MeshAttributeVector> meshAttributes = mesh->meshAttributes();
         }
         */
        return true;
    }
    
    bool COLLADA2GLTFWriter::writeVisualScene( const COLLADAFW::VisualScene* visualScene )
	{
        //FIXME: only one visual scene assumed/handled
        shared_ptr <GLTF::JSONObject> scenesObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> sceneObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> nodesObject = static_pointer_cast <GLTF::JSONObject> (this->_converterContext.root->getValue("nodes"));
        shared_ptr <GLTF::JSONObject> rootObject(new GLTF::JSONObject());
        
		const NodePointerArray& nodePointerArray = visualScene->getRootNodes();
        size_t nodeCount = nodePointerArray.getCount();
        
        this->_converterContext.root->setValue("scenes", scenesObject);
        this->_converterContext.root->setString("scene", "defaultScene");
        
        scenesObject->setValue("defaultScene", sceneObject); //FIXME: should use this id -> visualScene->getOriginalId()
        
        //first pass to output children name of our root node
        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        sceneObject->setValue("nodes", childrenArray);
        
        for (size_t i = 0 ; i < nodeCount ; i++) {
            std::string id = uniqueIdWithType("node", nodePointerArray[i]->getUniqueId());
            
            shared_ptr <GLTF::JSONString> nodeIDValue(new GLTF::JSONString(id));
            childrenArray->appendValue(static_pointer_cast <GLTF::JSONValue> (nodeIDValue));
        }
        
        rootObject->setValue("children", childrenArray);
        
        for (size_t i = 0 ; i < nodeCount ; i++) {
            //FIXME: &this->_sceneFlatteningInfo
            this->writeNode(nodePointerArray[i], nodesObject, COLLADABU::Math::Matrix4::IDENTITY, NULL);
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeScene( const COLLADAFW::Scene* scene )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes )
	{
        const NodePointerArray& nodes = libraryNodes->getNodes();
        
        shared_ptr <GLTF::JSONObject> nodesObject = static_pointer_cast <GLTF::JSONObject> (this->_converterContext.root->getValue("nodes"));
        
        size_t count = nodes.getCount();
        for (size_t i = 0 ; i < count ; i++) {
            const COLLADAFW::Node *node = nodes[i];
            
            if (!this->writeNode(node,  nodesObject, COLLADABU::Math::Matrix4::IDENTITY, 0))
                return false;
        }
        
        return true;
	}
    
	//--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeGeometry( const COLLADAFW::Geometry* geometry )
	{
        switch (geometry->getType()) {
            case Geometry::GEO_TYPE_MESH:
            {
                const COLLADAFW::Mesh* mesh = (COLLADAFW::Mesh*)geometry;
                unsigned int meshID = (unsigned int)geometry->getUniqueId().getObjectId();
                MeshVectorSharedPtr meshes;
                
                if (this->_converterContext._uniqueIDToMeshes.count(meshID) == 0) {
                    meshes =  shared_ptr<MeshVector> (new MeshVector);
                    
                    convertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, (*meshes));
                    
                    if (meshes->size()) {
                        for (size_t i = 0 ; i < meshes->size() ; i++) {
                            if ((*meshes)[i]->getPrimitives().size() > 0) {
                                (*meshes)[i]->writeAllBuffers(this->_verticesOutputStream, this->_indicesOutputStream, this->_compressedDataOutputStream);
                            }
                        }
                    }
                    
                    this->_converterContext._uniqueIDToMeshes[meshID] = meshes;
                }
            }
                break;
            case Geometry::GEO_TYPE_SPLINE:
            case Geometry::GEO_TYPE_CONVEX_MESH:
                // FIXME: handle convertion to mesh
            case Geometry::GEO_TYPE_UNKNOWN:
                //FIXME: handle error
                return false;
        }
        
        return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeMaterial( const COLLADAFW::Material* material )
	{
        const UniqueId& effectUID = material->getInstantiatedEffect();
		unsigned int materialID = (unsigned int)material->getUniqueId().getObjectId();
        this->_converterContext._materialUIDToName[materialID] = material->getName();
        this->_converterContext._materialUIDToEffectUID[materialID] = effectUID;
		return true;
	}
    
    
	//--------------------------------------------------------------------
    //FIXME: should be different depending on profiles. now assuming WebGL
	static std::string __GetGLWrapMode(COLLADAFW::Sampler::WrapMode wrapMode) {
        switch (wrapMode) {
            case COLLADAFW::Sampler::WRAP_MODE_UNSPECIFIED:
            case COLLADAFW::Sampler::WRAP_MODE_NONE:
            case COLLADAFW::Sampler::WRAP_MODE_WRAP:
                return "REPEAT";
            case COLLADAFW::Sampler::WRAP_MODE_MIRROR:
                return "MIRRORED_REPEAT";
            case COLLADAFW::Sampler::WRAP_MODE_CLAMP:
                return "CLAMP_TO_EDGE";
            default:
                break;
        }
        return "REPEAT";
    }
    
    static std::string __GetFilterMode(COLLADAFW::Sampler::SamplerFilter wrapMode) {
        switch (wrapMode) {
            case COLLADAFW::Sampler::SAMPLER_FILTER_UNSPECIFIED:
            case COLLADAFW::Sampler::SAMPLER_FILTER_NONE:
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR:
                return "LINEAR";
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST:
                return "NEAREST";
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST:
                return "NEAREST_MIPMAP_NEAREST";
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST:
                return "LINEAR_MIPMAP_NEAREST";
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR:
                return "NEAREST_MIPMAP_LINEAR";
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR:
                return "LINEAR_MIPMAP_LINEAR";
            default:
                break;
        }
        return "LINEAR";
    }
    
    void COLLADA2GLTFWriter::handleEffectSlot(const COLLADAFW::EffectCommon* commonProfile,
                                              std::string slotName,
                                              shared_ptr <GLTFEffect> cvtEffect)
    {
        shared_ptr <JSONObject> values = cvtEffect->getValues();
        
        ColorOrTexture slot;
        
        if (slotName == "diffuse")
            slot = commonProfile->getDiffuse();
        else if (slotName == "ambient")
            slot = commonProfile->getAmbient();
        else if (slotName == "emission")
            slot = commonProfile->getEmission();
        else if (slotName == "specular")
            slot = commonProfile->getSpecular();
        else if (slotName == "reflective")
            slot = commonProfile->getReflective();
        else
            return;
        
        
        //retrieve the type, parameterName -> symbol -> type
        double red = 1, green = 1, blue = 1;
        if (slot.isColor()) {
            const Color& color = slot.getColor();
            if (slot.getType() != COLLADAFW::ColorOrTexture::UNSPECIFIED) {
                red = color.getRed();
                green = color.getGreen();
                blue = color.getBlue();
            }
            shared_ptr <JSONObject> slotObject(new JSONObject());
            slotObject->setValue("value", serializeVec3(red,green,blue));
            slotObject->setString("type", "FLOAT_VEC3");
            values->setValue(slotName, slotObject);
            
        } else if (slot.isTexture()) {
            
            const Texture&  texture = slot.getTexture();
            const SamplerPointerArray& samplers = commonProfile->getSamplerPointerArray();
            const Sampler* sampler = samplers[texture.getSamplerId()];
            const UniqueId& imageUID = sampler->getSourceImage();
            
            shared_ptr <JSONObject> sampler2D(new JSONObject());
            
            std::string texcoord = texture.getTexcoord();
            
            cvtEffect->addSemanticForTexcoordName(texcoord, slotName);
            shared_ptr <JSONObject> slotObject(new JSONObject());

            sampler2D->setString("wrapS", __GetGLWrapMode(sampler->getWrapS()));
            sampler2D->setString("wrapT", __GetGLWrapMode(sampler->getWrapT()));
            sampler2D->setString("minFilter", __GetFilterMode(sampler->getMinFilter()));
            sampler2D->setString("magFilter", __GetFilterMode(sampler->getMagFilter()));
            sampler2D->setString("image", uniqueIdWithType("image",imageUID));
            slotObject->setString("type", "SAMPLER_2D");
            slotObject->setValue("value", sampler2D);

            values->setValue(slotName, slotObject);
        } else {
            // nothing to do, no texture or color
        }
    }
    
    bool COLLADA2GLTFWriter::writeEffect( const COLLADAFW::Effect* effect )
	{
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
        
        if (commonEffects.getCount() > 0) {
            std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS
            uniqueId += "material.";
#else
            uniqueId += "effect.";
#endif
            uniqueId += GLTF::GLTFUtils::toString(effect->getUniqueId().getObjectId());;
            
            shared_ptr <GLTFEffect> cvtEffect(new GLTFEffect(uniqueId));
            shared_ptr <JSONObject> values(new JSONObject());
            
            cvtEffect->setValues(values);
            
            const COLLADAFW::EffectCommon* effectCommon = commonEffects[0];
            
            handleEffectSlot(effectCommon,"diffuse" , cvtEffect);
            handleEffectSlot(effectCommon,"ambient" , cvtEffect);
            handleEffectSlot(effectCommon,"emission" , cvtEffect);
            handleEffectSlot(effectCommon,"specular" , cvtEffect);
            handleEffectSlot(effectCommon,"reflective" , cvtEffect);
            
            if (!isOpaque(effectCommon)) {
                shared_ptr <JSONObject> transparency(new JSONObject());
                transparency->setDouble("value", this->getTransparency(effectCommon));
                transparency->setString("type", "FLOAT");
                values->setValue("transparency", transparency);
            }
            
            //should check if has specular first and the lighting model (if not lambert)
            double shininess = effectCommon->getShininess().getFloatValue();
            if (shininess >= 0) {
                if (shininess < 1) {
                    shininess *= 128.0;
                }
                shared_ptr <JSONObject> shininessObject(new JSONObject());
                shininessObject->setString("type", "FLOAT");
                shininessObject->setDouble("value", shininess);
                values->setValue("shininesss", shininessObject);
            }
            
            this->_converterContext._uniqueIDToEffect[(unsigned int)effect->getUniqueId().getObjectId()] = cvtEffect;
            
        }
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeCamera( const COLLADAFW::Camera* camera )
	{
        shared_ptr <GLTF::JSONObject> camerasObject = static_pointer_cast <GLTF::JSONObject> (this->_converterContext.root->getValue("cameras"));
        if (!camerasObject) {
            camerasObject = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
            this->_converterContext.root->setValue("cameras", camerasObject);
        }
        
        shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
        
        std::string id = uniqueIdWithType("camera", camera->getUniqueId());
        
        camerasObject->setValue(id, cameraObject);
        
        switch (camera->getCameraType()) {
            case Camera::UNDEFINED_CAMERATYPE:
                printf("WARNING: unknown camera type: using perspective\n");
                cameraObject->setString("cameraType", "perspective");
                break;
            case Camera::ORTHOGRAPHIC:
            {
                cameraObject->setString("projection", "orthographic");
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
            case Camera::PERSPECTIVE:
            {
                cameraObject->setString("projection", "perspective");
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
        }
        
        cameraObject->setDouble("znear", camera->getNearClippingPlane().getValue());
        cameraObject->setDouble("zfar", camera->getFarClippingPlane().getValue());
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeImage( const COLLADAFW::Image* openCOLLADAImage )
	{
        shared_ptr <GLTF::JSONObject> images = this->_converterContext.root->createObjectIfNeeded("images");
        shared_ptr <GLTF::JSONObject> image(new GLTF::JSONObject());
        
        images->setValue(uniqueIdWithType("image",openCOLLADAImage->getUniqueId()), image);
        /*
         bool relativePath = true;
         if (pathDir.size() > 0) {
         if ((pathDir[0] != '.') || (pathDir[0] == '/')) {
         relativePath = false;
         }
         }*/
        const COLLADABU::URI &imageURI = openCOLLADAImage->getImageURI();
        std::string relPathFile = imageURI.getPathFile();
        if (imageURI.getPathDir() != "./") {
            relPathFile = imageURI.getPathDir() + imageURI.getPathFile();
        } else {
            relPathFile = imageURI.getPathFile();
        }
        image->setString("path", relPathFile);
        
        this->_converterContext._imageIdToImagePath[uniqueIdWithType("image",openCOLLADAImage->getUniqueId()) ] = relPathFile;
        return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLight( const COLLADAFW::Light* light )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimation( const COLLADAFW::Animation* animation )
	{
        shared_ptr <GLTFAnimation> cvtAnimation = convertOpenCOLLADAAnimationToGLTFAnimation(animation);
        
        this->_converterContext._uniqueIDToAnimation[animation->getUniqueId().getObjectId()] = cvtAnimation;
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimationList( const COLLADAFW::AnimationList* animationList )
	{
        const COLLADAFW::AnimationList::AnimationBindings &animationBindings = animationList->getAnimationBindings();
        
        AnimatedTargetsSharedPtr animatedTargets = this->_converterContext._uniqueIDToAnimatedTargets[animationList->getUniqueId().getObjectId()];
        
        for (size_t i = 0 ; i < animationBindings.getCount() ; i++) {
            shared_ptr <GLTFAnimation> cvtAnimation = this->_converterContext._uniqueIDToAnimation[animationBindings[i].animation.getObjectId()];
            const COLLADAFW::AnimationList::AnimationClass animationClass = animationBindings[i].animationClass;
            if (!GLTF::writeAnimation(cvtAnimation, animationClass, animatedTargets, this->_animationsOutputStream, this->_converterContext)) {
                this->_converterContext._uniqueIDToAnimation.erase(this->_converterContext._uniqueIDToAnimation.find(animationBindings[i].animation.getObjectId()));
            }
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeController( const COLLADAFW::Controller* Controller )
	{
		return true;
	}
    
} // namespace COLLADA2JSON
