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
#include "COLLADASaxFWLLoader.h"
#include "GLTF-Open3DGC.h"
#include "profiles/webgl-1.0/GLTFWebGL_1_0_Profile.h"
#include "GitSHA1.h"
#include <algorithm>

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
	
    /*
     Handles Channel creation + additions
     */
    static void __AddChannel(shared_ptr <GLTFAnimation> cvtAnimation,
                             const std::string &targetID,
                             const std::string &path) {
        shared_ptr<JSONObject> trChannel(new JSONObject());
        shared_ptr<JSONObject> trTarget(new JSONObject());
        
        trChannel->setString("sampler", cvtAnimation->getSamplerIDForName(path));
        trChannel->setValue("target", trTarget);
        trTarget->setString("id", targetID);
        trTarget->setString("path", path);
        cvtAnimation->channels()->appendValue(trChannel);
    }
        
    /*
    static shared_ptr <GLTFEffect> defaultEffect() {
        std::string uniqueId = "__glTF__defaultMaterial";
        
        shared_ptr <GLTFEffect> cvtEffect(new GLTFEffect(uniqueId));
        shared_ptr <JSONObject> values(new JSONObject());
        
        cvtEffect->setValues(values);
        cvtEffect->setLightingModel("Phong");
        
        //retrieve the type, parameterName -> symbol -> type
        double red = 1, green = 1, blue = 1, alpha = 1;
        shared_ptr <JSONObject> slotObject(new JSONObject());
        slotObject->setValue("value", serializeVec4(red, green, blue, alpha));
        slotObject->setString("type", "FLOAT_VEC4");
        values->setValue("diffuse", slotObject);
        
        shared_ptr<JSONObject> techniqueGenerator(new JSONObject());
        shared_ptr <JSONObject> attributeSemantics = serializeAttributeSemanticsForPrimitiveAtIndex(mesh.get(),j);
        
        techniqueGenerator->setString("lightingModel", cvtEffect->getLightingModel());
        techniqueGenerator->setValue("attributeSemantics", attributeSemantics);
        techniqueGenerator->setValue("values", cvtEffect->getValues());
        //techniqueGenerator->setValue("techniqueExtras", techniqueExtras);
        //techniqueGenerator->setValue("texcoordBindings", texcoordBindings);
        
        cvtEffect->setTechniqueGenerator(techniqueGenerator);
        cvtEffect->setName(uniqueId);
        
        return cvtEffect;
    }
     */

    
    bool COLLADA2GLTFWriter::write()
	{
        ifstream inputVertices;
        ifstream inputIndices;
        ifstream inputAnimation;
        ifstream inputCompression;
        ofstream ouputStream;

        this->_converterContext._geometryByteLength = 0;
        this->_converterContext._animationByteLength = 0;
        
        this->_extraDataHandler = new ExtraDataHandler();

        this->_converterContext.shaderIdToShaderString.clear();
        this->_converterContext._uniqueIDToMeshes.clear();

        //shared_ptr <GLTFEffect> aDefaultEffect = defaultEffect();
        //this->_converterContext._uniqueIDToEffect[0] = aDefaultEffect;

        
        /*
         1. We output vertices and indices separatly in 2 different files
         2. Then output them in a single file
         */
        COLLADABU::URI inputURI(this->_converterContext.inputFilePath.c_str());
        COLLADABU::URI outputURI(this->_converterContext.outputFilePath.c_str());
        
        std::string folder = outputURI.getPathDir();
        std::string fileName = inputURI.getPathFileBase();
        
        std::string sharedBufferID = fileName;
        std::string outputFilePath = outputURI.getPathDir() + fileName + ".bin";
        
        this->_converterContext._vertexOutputStream = new GLTFOutputStream(folder, fileName, "vertices");
        this->_converterContext._indicesOutputStream = new GLTFOutputStream(folder, fileName, "indices");
        this->_converterContext._animationOutputStream = new GLTFOutputStream(folder, fileName, "animations");
        this->_converterContext._compressionOutputStream = new GLTFOutputStream(folder, fileName, "compression");
        std::string compressedBufferID = this->_converterContext._compressionOutputStream->id();

        ouputStream.open (outputFilePath.c_str(), ios::out | ios::ate | ios::binary);
        
        this->_converterContext.profile = shared_ptr <GLTFWebGL_1_0_Profile> (new GLTFWebGL_1_0_Profile());
        
        this->_converterContext.root = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
        this->_converterContext.root->setString("profile", this->_converterContext.profile->id());
        this->_converterContext.root->setValue("nodes", shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject()));
        
        shared_ptr<JSONObject> asset = this->_converterContext.root->createObjectIfNeeded("asset");
        std::string version = "collada2gltf@"+std::string(g_GIT_SHA1);
        asset->setString("generator",version);
        
		COLLADAFW::Root root(&this->_loader, this);
        
        this->_loader.registerExtraDataCallbackHandler(this->_extraDataHandler);
		if (!root.loadDocument( this->_converterContext.inputFilePath))
			return false;
                
        // ----
        UniqueIDToAnimation::const_iterator UniqueIDToAnimationsIterator;
        shared_ptr <GLTF::JSONObject> animationsObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("animations", animationsObject);
        
        std::vector<GLTFAnimationFlattener*> flatteners;
        
        for (UniqueIDToAnimationsIterator = this->_converterContext._uniqueIDToAnimation.begin() ; UniqueIDToAnimationsIterator != this->_converterContext._uniqueIDToAnimation.end() ; UniqueIDToAnimationsIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            
            shared_ptr<GLTFAnimation> animation = (*UniqueIDToAnimationsIterator).second;
            
            std::vector<std::string> allTargets = animation->targets()->getAllKeys();
            for (size_t i = 0 ; i < allTargets.size() ; i++) {
                std::string targetID = allTargets[i];
                
                shared_ptr <JSONObject> target =  animation->targets()->getObject(targetID);
                std::string path = target->getString("path");

                shared_ptr<GLTFAnimationFlattener> animationFlattener = animation->animationFlattenerForTargetUID(targetID);
                if (std::find(flatteners.begin(), flatteners.end(), animationFlattener.get())!=flatteners.end()) {
                    continue;
                }
                flatteners.push_back(animationFlattener.get());

                size_t count = 0;
                float* rotations = 0;
                float* positions = 0;
                float* scales = 0;
                
                animationFlattener->allocAndFillAffineTransformsBuffers(&positions, &rotations, &scales, count);
                
                if (animationFlattener->hasAnimatedScale()) {
                    //Scale
                    setupAndWriteAnimationParameter(animation,
                                                      "scale",
                                                      "FLOAT_VEC3",
                                                      (unsigned char*)scales,
                                                      count * sizeof(float) * 3, false,
                                                      this->_converterContext);
                    __AddChannel(animation, targetID, "scale");
                    free(scales);
                }
                
                if (animationFlattener->hasAnimatedTranslation()) {
                    //Translation
                    setupAndWriteAnimationParameter(animation,
                                                      "translation",
                                                      "FLOAT_VEC3",
                                                      (unsigned char*)positions,
                                                      count * sizeof(float) * 3, false,
                                                      this->_converterContext);
                    __AddChannel(animation, targetID, "translation");
                    free(positions);
                }
                
                if (animationFlattener->hasAnimatedRotation()) {
                    //Rotation
                    setupAndWriteAnimationParameter(animation,
                                                      "rotation",
                                                      "FLOAT_VEC4",
                                                      (unsigned char*)rotations,
                                                      count * sizeof(float) * 4, false,
                                                      this->_converterContext);
                    __AddChannel(animation, targetID, "rotation");
                    free(rotations);
                }
            }
        }

        //reopen .bin files for vertices and indices
        size_t verticesLength = this->_converterContext._vertexOutputStream->length();
        size_t indicesLength = this->_converterContext._indicesOutputStream->length();
        size_t animationLength = this->_converterContext._animationOutputStream->length();
        size_t compressionLength = this->_converterContext._compressionOutputStream->length();

        shared_ptr <GLTFBuffer> compressionBuffer(new GLTFBuffer(this->_converterContext._compressionOutputStream->id(), compressionLength));

        inputVertices.open(this->_converterContext._vertexOutputStream->outputPathCStr(), ios::in | ios::binary);
        inputIndices.open(this->_converterContext._indicesOutputStream->outputPathCStr(), ios::in | ios::binary);
        inputAnimation.open(this->_converterContext._animationOutputStream->outputPathCStr(), ios::in | ios::binary);
        inputCompression.open(this->_converterContext._compressionOutputStream->outputPathCStr(), ios::in | ios::binary);
        
        this->_converterContext._vertexOutputStream->close();
        this->_converterContext._indicesOutputStream->close();
        this->_converterContext._animationOutputStream->close();
        this->_converterContext._compressionOutputStream->close();

        char* bufferIOStream = (char*)malloc(sizeof(char) * verticesLength);
        inputVertices.read(bufferIOStream, verticesLength);
        ouputStream.write(bufferIOStream, verticesLength);
        free(bufferIOStream);
        
        bufferIOStream = (char*)malloc(sizeof(char) * indicesLength);
        inputIndices.read(bufferIOStream, indicesLength);
        ouputStream.write(bufferIOStream, indicesLength);
        free(bufferIOStream);

        //add padding for https://github.com/KhronosGroup/glTF/issues/167
        //it is known that the other buffers are all FLOAT, so as a minimal fix we just have to align indices (that are short) on FLOAT when writting them.
        size_t rem = indicesLength % 4;
        if (rem) {
            char pad[3];
            size_t paddingForAlignement = 4 - rem;
            ouputStream.write(pad, paddingForAlignement);
            indicesLength += paddingForAlignement;
        }
        
        bufferIOStream = (char*)malloc(sizeof(char) * animationLength);
        inputAnimation.read(bufferIOStream, animationLength);
        ouputStream.write(bufferIOStream, animationLength);
        free(bufferIOStream);

        inputVertices.close();
        inputIndices.close();
        inputAnimation.close();
        inputCompression.close();

        remove(this->_converterContext._vertexOutputStream->outputPathCStr());
        remove(this->_converterContext._indicesOutputStream->outputPathCStr());
        remove(this->_converterContext._animationOutputStream->outputPathCStr());
        
        if (compressionLength == 0) {
            remove(this->_converterContext._compressionOutputStream->outputPathCStr());
        }
        
        delete this->_converterContext._vertexOutputStream;
        delete this->_converterContext._indicesOutputStream;
        delete this->_converterContext._animationOutputStream;
        delete this->_converterContext._compressionOutputStream;
        
        shared_ptr <GLTFBuffer> sharedBuffer(new GLTFBuffer(sharedBufferID, verticesLength + indicesLength + animationLength));

        //---
        shared_ptr <GLTFBufferView> verticesBufferView(new GLTFBufferView(sharedBuffer, 0, verticesLength));
        shared_ptr <GLTFBufferView> indicesBufferView(new GLTFBufferView(sharedBuffer, verticesLength, indicesLength));
        shared_ptr <GLTFBufferView> genericBufferView(new GLTFBufferView(sharedBuffer, verticesLength + indicesLength, animationLength));
        shared_ptr <GLTFBufferView> compressionBufferView(new GLTFBufferView(compressionBuffer, 0, compressionLength));
        
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
                        PrimitiveVector primitives = mesh->getPrimitives();
                        /*
                         for (size_t k = 0 ; ((shouldSkipMesh == false) && (k < primitives.size())) ; k++) {
                         shared_ptr <GLTF::GLTFPrimitive> primitive = primitives[k];
                         
                         if (primitive->getMaterialID().length() == 0) {
                         primitive->setMaterialID(aDefaultEffect->getID());
                         }
                         }
                         */
                        
                        bool isCompressed = false;
                        if (mesh->getExtensions()) {
                            isCompressed = mesh->getExtensions()->contains("Open3DGC-compression");
                        }
                        
                        void *serializationContext[4];
                        serializationContext[0] = isCompressed ? (void*)compressionBufferView.get() : (void*)verticesBufferView.get();
                        serializationContext[1] = isCompressed ? (void*)compressionBufferView.get() : (void*)indicesBufferView.get();
                        serializationContext[2] = isCompressed ? (void*)compressionBufferView.get() : (void*)genericBufferView.get();
                        serializationContext[3] = (void*)&this->_converterContext;

                        shared_ptr <GLTF::JSONObject> meshObject = serializeMesh(mesh.get(), (void*)serializationContext);

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
                                shared_ptr <GLTF::JSONObject> meshAttributeObject = serializeMeshAttribute(meshAttribute.get(), (void*)serializationContext);
                                attributes->setValue(meshAttribute->getID(), meshAttributeObject);
                            }
                        }
                        
                        //serialize indices
                        primitives = mesh->getPrimitives();
                        unsigned int primitivesCount =  (unsigned int)primitives.size();
                        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
                            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
                            shared_ptr <GLTF::GLTFIndices> uniqueIndices =  primitive->getUniqueIndices();
                            shared_ptr <GLTF::JSONObject> serializedIndices = serializeIndices(uniqueIndices.get(), (void*)serializationContext);
                            
                            indices->setValue(uniqueIndices->getID(), serializedIndices);
                        }
                        
                        meshesObject->setValue(mesh->getID(), meshObject);
                    }
                }
            }
        }
        
        // ----
        shared_ptr <GLTF::JSONObject> materialsObject = this->_converterContext.root->createObjectIfNeeded("materials");
        UniqueIDToEffect::const_iterator UniqueIDToEffectIterator;
        
        for (UniqueIDToEffectIterator = this->_converterContext._uniqueIDToEffect.begin() ; UniqueIDToEffectIterator != this->_converterContext._uniqueIDToEffect.end() ; UniqueIDToEffectIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <GLTF::GLTFEffect> effect = (*UniqueIDToEffectIterator).second;
            if (effect->getTechniqueGenerator()) {
                materialsObject->setValue(effect->getID(), serializeEffect(effect.get(), &this->_converterContext));
            }
        }
        
        // ----
        shared_ptr <GLTF::JSONObject> skins = this->_converterContext.root->createObjectIfNeeded("skins");
        
        UniqueIDToSkin::const_iterator UniqueIDToSkinIterator;
        
        for (UniqueIDToSkinIterator = this->_converterContext._uniqueIDToSkin.begin() ; UniqueIDToSkinIterator != this->_converterContext._uniqueIDToSkin.end() ; UniqueIDToSkinIterator++) {
            
            shared_ptr <GLTFSkin> skin = (*UniqueIDToSkinIterator).second;
            shared_ptr<JSONArray> joints = skin->getJointsIds();
            shared_ptr<JSONArray> jointsWithOriginalSids(new JSONArray());

            //resolve the sid and use the original ones
            std::vector <shared_ptr <JSONValue> > values = joints->values();
            for (size_t i = 0 ; i < values.size() ; i++) {
                shared_ptr<JSONString> jointId = static_pointer_cast<JSONString>(values[i]);
                shared_ptr<JSONObject> node = this->_converterContext._uniqueIDToNode[jointId->getString()];
                if (node->contains("jointId")) {
                    jointsWithOriginalSids->appendValue(static_pointer_cast <JSONValue> (node->getValue("jointId")));
                }
            }
            skin->setJointsIds(jointsWithOriginalSids);
            shared_ptr <JSONObject> serializedSkin = serializeSkin(skin.get());
            shared_ptr <JSONObject> inverseBindMatrices = static_pointer_cast<JSONObject>(skin->extras()->getValue("inverseBindMatrices"));
            inverseBindMatrices->setString("bufferView", genericBufferView->getID());
            serializedSkin->setValue("inverseBindMatrices", inverseBindMatrices);
            skins->setValue(skin->getId(), serializedSkin);
        }
        
        // ----
        
        for (UniqueIDToAnimationsIterator = this->_converterContext._uniqueIDToAnimation.begin() ; UniqueIDToAnimationsIterator != this->_converterContext._uniqueIDToAnimation.end() ; UniqueIDToAnimationsIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            
            shared_ptr<GLTFAnimation> animation = (*UniqueIDToAnimationsIterator).second;
            std::vector <shared_ptr <GLTFAnimation::Parameter> > *parameters = animation->parameters();
            
            //Replace OpenCOLLADA uniqueID by Original IDs
            shared_ptr <JSONArray> channels = animation->channels();
            for (size_t i = 0 ; i < channels->values().size() ; i++) {
                shared_ptr<JSONObject> channel = static_pointer_cast<JSONObject>(channels->values()[i]);
                shared_ptr<JSONObject> target = channel->getObject("target");
                std::string originalID = this->_converterContext._uniqueIDToOriginalID[target->getString("id")];
                target->setString("id", originalID);
            }
            
            for (size_t i = 0 ; i < animation->parameters()->size() ; i++) {
                shared_ptr <GLTFAnimation::Parameter> parameter = (*parameters)[i];
                
                if (parameter->extensions()->getKeysCount() > 0) {
                    if (parameter->extensions()->contains("Open3DGC-compression")) {
                        shared_ptr<JSONObject> compressionObject = parameter->extensions()->getObject("Open3DGC-compression");
                        if (compressionObject->contains("compressedData")) {
                            shared_ptr<JSONObject> compressedData = compressionObject->getObject("compressedData");
                            compressedData->setString("bufferView", compressionBufferView->getID());
                        }
                    }
                }
                
                parameter->setBufferView(genericBufferView);
            }
            
            if (animation->channels()->values().size() > 0) {
                shared_ptr <JSONObject> animationObject = serializeAnimation(animation.get(), &this->_converterContext);
            
                animationsObject->setValue(animation->getID(), animationObject);
            }
        }
        
        shared_ptr <JSONObject> buffersObject(new JSONObject());
        shared_ptr <JSONObject> bufferObject = serializeBuffer(sharedBuffer.get(), sharedBufferID+".bin",  0);
        shared_ptr <JSONObject> bufferObject1 = serializeBuffer(compressionBuffer.get(), compressedBufferID+".bin",  0);

        this->_converterContext.root->setValue("buffers", buffersObject);
        
        if (sharedBuffer->getByteLength() > 0) {
            buffersObject->setValue(sharedBufferID, bufferObject);
            bufferObject->setString("type", "arraybuffer");
        }

        if (compressionBuffer->getByteLength() > 0) {
            buffersObject->setValue(compressedBufferID, bufferObject1);
            if (this->_converterContext.compressionMode == "ascii")
                bufferObject1->setString("type", "text");
        }

        //FIXME: below is an acceptable short-cut since in this converter we will always create one buffer view for vertices and one for indices.
        //Fabrice: Other pipeline tools should be built on top of the format manipulate the buffers and end up with a buffer / bufferViews layout that matches the need of a given application for performance. For instance we might want to concatenate a set of geometry together that come from different file and call that a "level" for a game.
        shared_ptr <JSONObject> bufferViewsObject(new JSONObject());
        this->_converterContext.root->setValue("bufferViews", bufferViewsObject);
        
        shared_ptr <JSONObject> bufferViewVerticesObject = serializeBufferView(verticesBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewIndicesObject = serializeBufferView(indicesBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewGenericObject = serializeBufferView(genericBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewCompressionObject = serializeBufferView(compressionBufferView.get(), 0);

        bufferViewsObject->setValue(indicesBufferView->getID(), bufferViewIndicesObject);
        bufferViewsObject->setValue(verticesBufferView->getID(), bufferViewVerticesObject);
        //if (animationLength > 0) {
            bufferViewsObject->setValue(genericBufferView->getID(), bufferViewGenericObject);
        //}
        if (compressionLength > 0) {
            bufferViewsObject->setValue(compressionBufferView->getID(), bufferViewCompressionObject);
        }
        
        bufferViewIndicesObject->setUnsignedInt32("target", this->_converterContext.profile->getGLenumForString("ELEMENT_ARRAY_BUFFER"));
        bufferViewVerticesObject->setUnsignedInt32("target", this->_converterContext.profile->getGLenumForString("ARRAY_BUFFER"));
        
        //---
        this->_converterContext.root->removeValue("lightsIds");

        this->_converterContext.root->write(&this->_writer);
        
        bool sceneFlatteningEnabled = false;
        if (sceneFlatteningEnabled) {
            //second pass to actually output the JSON of the node themselves (and for all sub nodes)
            processSceneFlatteningInfo(&this->_sceneFlatteningInfo);
        }
        
        ouputStream.flush();
        ouputStream.close();
        
        if (sharedBuffer->getByteLength() == 0)
            remove(outputFilePath.c_str());
        
        delete this->_extraDataHandler;
        
        printf("geometry:%d bytes\n", (int)this->_converterContext._geometryByteLength);
        printf("animations:%d bytes\n", (int)this->_converterContext._animationByteLength);
        
        printf("[scene] total bytes:%d\n", (int) (verticesLength + indicesLength + animationLength + compressionLength) );
        
        
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
        
        COLLADABU::Math::Matrix4 transpose = matrix.transpose();

        for (int i = 0 ; i < 4 ; i++)  {
            const COLLADABU::Math::Real * real = transpose[i];
            
            m[(i*4) + 0] = (float)real[0];
            m[(i*4) + 1] = (float)real[1];
            m[(i*4) + 2] = (float)real[2];
            m[(i*4) + 3] = (float)real[3];
        }
    }
    
    shared_ptr <GLTF::JSONArray> COLLADA2GLTFWriter::serializeMatrix4Array(const COLLADABU::Math::Matrix4 &matrix)
    {
        float m[16];
        shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());

        __GetFloatArrayFromMatrix(matrix, m);
        
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
    
    void COLLADA2GLTFWriter::registerObjectWithOriginalUID(std::string objectUID, shared_ptr <JSONObject> obj, shared_ptr <JSONObject> objLib)
    {
        if (this->_converterContext._originalIDToTrackedObject.count(objectUID) == 0) {
            if (!objLib->contains(objectUID)) {
                objLib->setValue(objectUID, obj);
                this->_converterContext._originalIDToTrackedObject[objectUID] = obj;
            } else {
                printf("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", objectUID.c_str());
            }
        } else {
            printf("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", objectUID.c_str());
        }
    }
    
    shared_ptr <GLTF::JSONObject> serializeAttributeSemanticsForPrimitiveAtIndex(GLTFMesh* mesh, unsigned int idx)
    {
        shared_ptr <GLTFPrimitive> primitive = mesh->getPrimitives()[idx];
                                
        shared_ptr <GLTF::JSONObject> semantics(new GLTF::JSONObject());
        shared_ptr<JSONArray> sets(new JSONArray());
        
        size_t count = primitive->getVertexAttributesCount();
        for (size_t j = 0 ; j < count ; j++) {
            Semantic semantic = primitive->getSemanticAtIndex(j);
            std::string semanticString = GLTFUtils::getStringForSemantic(semantic);

            shared_ptr<JSONObject> semanticInfo;
            if (semantics->contains(semanticString) == false) {
                semanticInfo = shared_ptr<JSONObject> (new JSONObject());
                semantics->setValue(semanticString, semanticInfo);
            }
            
            unsigned int indexOfSet = 0;
            if (semantic == GLTF::TEXCOORD) {
                indexOfSet = primitive->getIndexOfSetAtIndex(j);
            }
            
            sets->appendValue(shared_ptr<JSONNumber> (new JSONNumber(indexOfSet)));
        }
        
        return semantics;
    }
    
    
    bool COLLADA2GLTFWriter::writeMeshFromUIDWithMaterialBindings(COLLADAFW::UniqueId uniqueId,
                                                                   MaterialBindingArray &materialBindings,
                                                                   shared_ptr <GLTF::JSONArray> &meshesArray) {
        unsigned int meshUID = (unsigned int)uniqueId.getObjectId();
        shared_ptr<JSONObject> meshExtras = this->_extraDataHandler->getExtras(uniqueId);
        
        MeshVectorSharedPtr meshes;
        MeshVectorSharedPtr meshes2;

        size_t meshesCount = 0;
        if (!this->_converterContext._uniqueIDToMeshes.count(meshUID)) {
            if (this->_converterContext._uniqueIDToMesh.count(uniqueId.toAscii())) {
                meshes = shared_ptr<MeshVector> (new MeshVector());
                
                shared_ptr<GLTFMesh> unifiedMesh = this->_converterContext._uniqueIDToMesh[uniqueId.toAscii()];
                if  (createMeshesWithMaximumIndicesCountFromMeshIfNeeded(unifiedMesh.get(), 65535, *meshes) == false) {
                    meshes->push_back(unifiedMesh);
                }

                if (this->_converterContext.compressionType == "won") {
                    meshes2 = shared_ptr<MeshVector> (new MeshVector());
                    for (size_t j = 0 ; j < meshes->size() ; j++) {
                        shared_ptr<GLTFMesh> aMesh = (*meshes)[j];
                        if (!createMeshesFromMeshPrimitives(aMesh.get(), *meshes2)) {
                            meshes2->push_back(aMesh);
                        }
                    }
                } else {
                    meshes2 = meshes;
                }
                
#if USE_WEBGLLOADER
                //now compress meshes
                if (this->_converterContext.compressionType == "won") {
                    for (size_t i = 0 ; i < meshes2->size() ; i++) {
                        shared_ptr <GLTF::GLTFMesh> mesh = (*meshes2)[i];
                        compress(mesh);
                    }
                }
#endif
                
                meshesCount = meshes2->size();
                if (meshesCount) {
                    for (size_t i = 0 ; i < meshes2->size() ; i++) {
                        if ((*meshes2)[i]->getPrimitives().size() > 0) {
                            writeAllMeshBuffers((*meshes2)[i], this->_converterContext);
                        }
                    }
                }
                
                meshes = meshes2;
                                
                this->_converterContext._uniqueIDToMeshes[meshUID] = meshes;
            }
        } else {
            meshes = this->_converterContext._uniqueIDToMeshes[meshUID];
            meshesCount = meshes->size();
        }
        
        if (meshesCount > 0) {
            for (size_t meshIndex = 0 ; meshIndex < meshes->size() ; meshIndex++) {
                shared_ptr <GLTFMesh> mesh = (*meshes)[meshIndex];
                
                if (!mesh) {
                    continue;
                }
                
                /*
                if (sceneFlatteningInfo) {
                    GLTF::IndexSetToMeshAttributeHashmap& semanticMap = mesh->getMeshAttributesForSemantic(GLTF::POSITION);
                    shared_ptr <GLTF::GLTFMeshAttribute> vertexMeshAttribute = semanticMap[0];
                    
                    BBOX vertexBBOX(COLLADABU::Math::Vector3(vertexMeshAttribute->getMin()),
                                    COLLADABU::Math::Vector3(vertexMeshAttribute->getMax()));
                    vertexBBOX.transform(worldMatrix);
                    
                    sceneFlatteningInfo->sceneBBOX.merge(&vertexBBOX);
                }
                */
                PrimitiveVector primitives = mesh->getPrimitives();
                for (size_t j = 0 ; j < primitives.size() ; j++) {
                    shared_ptr <GLTF::GLTFPrimitive> primitive = primitives[j];
                    
                    //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
                    int materialBindingIndex = -1;
                    for (size_t k = 0; k < materialBindings.getCount() ; k++) {
                        //printf("materialID in binding:%d primitveObjectID:%d\n", materialBindings[k].getMaterialId(),
                        //   primitive->getMaterialObjectID());
                        
                        if (materialBindings[k].getMaterialId() == primitive->getMaterialObjectID()) {
                            materialBindingIndex = (unsigned int)k;
                        }
                    }
                    
                    shared_ptr<JSONObject> texcoordBindings(new JSONObject());
                    
                    if (materialBindingIndex != -1) {
                        unsigned int referencedMaterialID = (unsigned int)materialBindings[materialBindingIndex].getReferencedMaterial().getObjectId();
                        
                        /* will be needed to get semantic & set association to create the shader */
                        const TextureCoordinateBindingArray &textureCoordBindings = materialBindings[materialBindingIndex].getTextureCoordinateBindingArray();
                        
                        COLLADAFW::UniqueId effectUID = this->_converterContext._materialUIDToEffectUID[referencedMaterialID];
                        unsigned int effectID = (unsigned int)effectUID.getObjectId();
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
                                        texcoordBindings->setString(slot, shaderSemantic);
                                        if ((slot == "diffuse") && effectExtras->getBool("ambient_diffuse_lock")) {
                                            texcoordBindings->setString("ambient", shaderSemantic);
                                        }
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
                        shared_ptr <JSONObject> attributeSemantics = serializeAttributeSemanticsForPrimitiveAtIndex(mesh.get(),j);
                    
                        shared_ptr<JSONObject> techniqueGenerator(new JSONObject());
                        
                        techniqueGenerator->setString("lightingModel", effect->getLightingModel());
                        techniqueGenerator->setValue("attributeSemantics", attributeSemantics);
                        techniqueGenerator->setValue("values", effect->getValues());
                        techniqueGenerator->setValue("techniqueExtras", techniqueExtras);
                        techniqueGenerator->setValue("texcoordBindings", texcoordBindings);
                        
                        effect->setTechniqueGenerator(techniqueGenerator);
                        effect->setName(materialName);
                        primitive->setMaterialID(effect->getID());
                    }
                    //printf(":::: %s\n",primitive->getMaterialID().c_str());
                }
                
                meshesArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(mesh->getID())));
                /*if (sceneFlatteningInfo) {
                    shared_ptr <MeshFlatteningInfo> meshFlatteningInfo(new MeshFlatteningInfo(meshUID, parentMatrix));
                    sceneFlatteningInfo->allMeshes.push_back(meshFlatteningInfo);
                }*/
            }
        }
        return true;
    }
    

    bool COLLADA2GLTFWriter::writeNode( const COLLADAFW::Node* node,
                                       shared_ptr <GLTF::JSONObject> nodesObject,
                                       COLLADABU::Math::Matrix4 parentMatrix,
                                       SceneFlatteningInfo* sceneFlatteningInfo)
    {
        bool shouldExportTRS = this->_converterContext.alwaysExportTRS;
        const NodePointerArray& nodes = node->getChildNodes();
        std::string nodeOriginalID = node->getOriginalId();
        if (nodeOriginalID.length() == 0) {
            nodeOriginalID = uniqueIdWithType("node", node->getUniqueId());
        }
        
        std::string uniqueUID = node->getUniqueId().toAscii();
        
        COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
        
        shared_ptr <GLTF::JSONObject> nodeObject(new GLTF::JSONObject());
        nodeObject->setString("name",node->getName());
        
        this->_converterContext._uniqueIDToOpenCOLLADAObject[uniqueUID] = shared_ptr <COLLADAFW::Object> (node->clone());
        this->_converterContext._uniqueIDToOriginalID[uniqueUID] = nodeOriginalID;

        this->_converterContext._uniqueIDToNode[uniqueUID] = nodeObject;
        if (node->getType() == COLLADAFW::Node::JOINT) {
            const string& sid = node->getSid();
            nodeObject->setString("jointId",sid);
        }
        
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
            
            this->_converterContext._uniqueIDToAnimatedTargets[animationListID.toAscii()] = animatedTargets;
            shared_ptr <JSONObject> animatedTarget(new JSONObject());
            std::string animationID = animationListID.toAscii();
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::MATRIX)  {
                animatedTarget->setString("target", uniqueUID);
                animatedTarget->setString("path", "MATRIX");
                animatedTarget->setString("transformId", animationID);
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::TRANSLATE)  {
                animatedTarget->setString("target", uniqueUID);
                animatedTarget->setString("path", "translation");
                animatedTarget->setString("transformId", animationID);
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::ROTATE)  {
                animatedTarget->setString("target", uniqueUID);
                animatedTarget->setString("path", "rotation");
                animatedTarget->setString("transformId", animationID);
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
          
          if (tr->getTransformationType() == COLLADAFW::Transformation::SCALE)  {
            animatedTarget->setString("target", uniqueUID);
            animatedTarget->setString("path", "scale");
            animatedTarget->setString("transformId", animationID);
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
        
        const InstanceControllerPointerArray& instanceControllers = node->getInstanceControllers();
        shared_ptr <GLTF::JSONArray> meshesArray(new GLTF::JSONArray());
        unsigned int count = instanceControllers.getCount();
        if (count > 0) {
            for (unsigned int i = 0 ; i < count; i++) {
                InstanceController* instanceController = instanceControllers[i];
                MaterialBindingArray &materialBindings = instanceController->getMaterialBindings();
                COLLADAFW::UniqueId uniqueId = instanceController->getInstanciatedObjectId();
                
                if (this->_converterContext._uniqueIDToSkin.count((unsigned int)uniqueId.getObjectId()) > 0) {
                    shared_ptr <GLTF::JSONArray> skinMeshesArray(new GLTF::JSONArray());

                    shared_ptr<GLTFSkin> skin = this->_converterContext._uniqueIDToSkin[(unsigned int)uniqueId.getObjectId()];
                    UniqueId meshUniqueId(skin->getSourceUID());
                    writeMeshFromUIDWithMaterialBindings(meshUniqueId, materialBindings, skinMeshesArray);
                    
                    //write instanceController
                    shared_ptr<JSONObject> serializedInstanceController(new JSONObject());
                    //const COLLADAFW::SkeletonUrlList&  skeletonList = instanceController->getSkeletonUrlList();
                    
                    UniqueId skinDataUniqueId(skin->extras()->getString("uniqueId"));
                    
                    //looks like the only way to get the <skeleton> information from OpenCOLLADA
                    Loader::InstanceControllerDataList list = this->_loader.getInstanceControllerDataListMap()[skinDataUniqueId];
                    
                    Loader::InstanceControllerData instanceControllerData = *list.begin();
                    
                    shared_ptr<JSONObject> instanceSkin(new JSONObject());
                    
                    shared_ptr<JSONArray> skeletons(new JSONArray());
                    URIList::iterator listIterator;
                    for(listIterator = instanceControllerData.skeletonRoots.begin(); listIterator != instanceControllerData.skeletonRoots.end(); ++listIterator) {
                        std::string skeleton = listIterator->getFragment();
                        skeletons->appendValue(shared_ptr<JSONString>(new JSONString(skeleton)));
                    }
                    
                    instanceSkin->setValue("skeletons", skeletons);
                    instanceSkin->setString("skin", skin->getId());
                    
                    //FIXME: this should account for morph as sources to
                    instanceSkin->setValue("sources", skinMeshesArray);
                    nodeObject->setValue("instanceSkin", instanceSkin);
                }
            }
        }
        
        // save mesh
		const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();
        
        count = (unsigned int)instanceGeometries.getCount();
        if (count > 0) {
            for (unsigned int i = 0 ; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                COLLADAFW::UniqueId uniqueId = instanceGeometry->getInstanciatedObjectId();
                writeMeshFromUIDWithMaterialBindings(uniqueId, materialBindings, meshesArray);
            }
        }
        
        if (meshesArray->values().size() > 0)
            nodeObject->setValue("meshes", meshesArray);

        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        nodeObject->setValue("children", childrenArray);
        
        count = (unsigned int)nodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++)  {
            std::string childOriginalID = nodes[i]->getOriginalId();
            if (childOriginalID.length() == 0) {
                childOriginalID = uniqueIdWithType("node", nodes[i]->getUniqueId());
            }
            childrenArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(childOriginalID)));
        }
        
        this->_converterContext._uniqueIDToOriginalID[uniqueUID] = nodeOriginalID;
        
        registerObjectWithOriginalUID(nodeOriginalID, nodeObject, nodesObject);
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            this->writeNode(nodes[i], nodesObject, worldMatrix, sceneFlatteningInfo);
        }
        
        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = (unsigned int)instanceNodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++) {
            InstanceNode* instanceNode  = instanceNodes[i];
            std::string id = instanceNode->getInstanciatedObjectId().toAscii();
            shared_ptr<JSONArray> parents;
            if (this->_converterContext._uniqueIDToParentsOfInstanceNode.count(id) == 0) {
                parents =  shared_ptr<JSONArray> (new JSONArray());
                this->_converterContext._uniqueIDToParentsOfInstanceNode[id] = parents;
            } else {
                parents = this->_converterContext._uniqueIDToParentsOfInstanceNode[id];
            }
            
            parents->appendValue(shared_ptr<JSONString> (new JSONString(node->getUniqueId().toAscii())));
        }
        
        shared_ptr <GLTF::JSONArray> lightsInNode(new GLTF::JSONArray());

        const InstanceLightPointerArray& instanceLights = node->getInstanceLights();
        count = (unsigned int)instanceLights.getCount();
        
        //For a given light, keep track of all the nodes holding it
        if (count) {
            shared_ptr<JSONObject> lights = this->_converterContext.root->createObjectIfNeeded("lights");
            for (unsigned int i = 0 ; i < count ; i++) {
                InstanceLight* instanceLight  = instanceLights[i];
                std::string id = instanceLight->getInstanciatedObjectId().toAscii();
                
                shared_ptr<JSONObject> light = this->_converterContext._uniqueIDToLight[id];
                std::string lightUID = this->_converterContext._uniqueIDToOriginalID[id];

                shared_ptr<JSONArray> listOfNodesPerLight;
                if (this->_converterContext._uniqueIDOfLightToNodes.count(id) == 0) {
                    listOfNodesPerLight =  shared_ptr<JSONArray> (new JSONArray());
                    this->_converterContext._uniqueIDOfLightToNodes[lightUID] = listOfNodesPerLight;
                } else {
                    listOfNodesPerLight = this->_converterContext._uniqueIDOfLightToNodes[lightUID];
                }
                
                listOfNodesPerLight->appendValue(JSONSTRING(nodeOriginalID));
                lightsInNode->appendValue(shared_ptr <JSONString> (new JSONString(lightUID)));
                lights->setValue(lightUID, light);
            }
            //We just want a single light per node
            //https://github.com/KhronosGroup/glTF/issues/13
            //nodeObject->setValue("lights", lightsInNode);
            nodeObject->setValue("light", lightsInNode->values()[0]);
            if (count > 1) {
                //FR: AFAIK no authoring tool export multiple light per node, but we'll warn if that's the case
                //To fix this, dummy sub nodes should be created.
                static bool printedOnce = false;
                if (printedOnce) {
                    printf("WARNING: some unhandled lights because some nodes carry more than a single light\n");
                    printedOnce = false;
                }
                
            }
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
            std::string nodeUID = nodePointerArray[i]->getOriginalId();
            if (nodeUID.length() == 0) {
                nodeUID = uniqueIdWithType("node", nodePointerArray[i]->getUniqueId());
            }
                        
            shared_ptr <GLTF::JSONString> nodeIDValue(new GLTF::JSONString(nodeUID));
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
            
            std::string id = node->getUniqueId().toAscii();
            if (this->_converterContext._uniqueIDToParentsOfInstanceNode.count(id) > 0) {
                shared_ptr<JSONArray> parents = this->_converterContext._uniqueIDToParentsOfInstanceNode[id];
                std::vector <shared_ptr <JSONValue> > values = parents->values();
                for (size_t k = 0 ; k < values.size() ; k++) {
                    shared_ptr<JSONString> value = static_pointer_cast<JSONString>(values[k]);

                    shared_ptr<JSONObject> parentNode = this->_converterContext._uniqueIDToNode[value->getString()];
                    if (parentNode) {
                        shared_ptr <JSONArray> children = parentNode->createArrayIfNeeded("children");
                        children->appendValue(shared_ptr <JSONString>(new JSONString(node->getOriginalId())));
                    }
                }
            }
            
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
                
                if (this->_converterContext._uniqueIDToMeshes.count(meshID) == 0) {
                        shared_ptr<GLTFMesh> cvtMesh = convertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, this->_converterContext);
                    if (cvtMesh) {
                        //here we stock the mesh with unified indices but we may have to handle additional mesh attributes
                        this->_converterContext._uniqueIDToMesh[geometry->getUniqueId().toAscii()] = cvtMesh;
                    }
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
	unsigned int __GetGLWrapMode(COLLADAFW::Sampler::WrapMode wrapMode, GLTFProfile *profile) {
        switch (wrapMode) {
            case COLLADAFW::Sampler::WRAP_MODE_UNSPECIFIED:
            case COLLADAFW::Sampler::WRAP_MODE_NONE:
            case COLLADAFW::Sampler::WRAP_MODE_WRAP:
                return profile->getGLenumForString("REPEAT");
            case COLLADAFW::Sampler::WRAP_MODE_MIRROR:
                return profile->getGLenumForString("MIRRORED_REPEAT");
            case COLLADAFW::Sampler::WRAP_MODE_CLAMP:
                return profile->getGLenumForString("CLAMP_TO_EDGE");
            default:
                break;
        }
        return profile->getGLenumForString("REPEAT");
    }
    
    static unsigned int __GetFilterMode(COLLADAFW::Sampler::SamplerFilter wrapMode, GLTFProfile *profile) {
        switch (wrapMode) {
            case COLLADAFW::Sampler::SAMPLER_FILTER_UNSPECIFIED:
            case COLLADAFW::Sampler::SAMPLER_FILTER_NONE:
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR:
                return profile->getGLenumForString("LINEAR");
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST:
                return profile->getGLenumForString("NEAREST");
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST:
                return profile->getGLenumForString("NEAREST_MIPMAP_NEAREST");
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST:
                return profile->getGLenumForString("LINEAR_MIPMAP_NEAREST");
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR:
                return profile->getGLenumForString("NEAREST_MIPMAP_LINEAR");
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR:
                return profile->getGLenumForString("LINEAR_MIPMAP_LINEAR");
            default:
                break;
        }
        return profile->getGLenumForString("LINEAR");
    }
    
    std::string COLLADA2GLTFWriter::getSamplerUIDForParameters(unsigned int wrapS,
                                                               unsigned int wrapT,
                                                               unsigned int minFilter,
                                                               unsigned int maxFilter) {
        std::string samplerHash = GLTFUtils::toString(wrapS)+GLTFUtils::toString(wrapT)+GLTFUtils::toString(minFilter)+GLTFUtils::toString(maxFilter);
        bool addSampler = false;
        size_t index = 0;
        
        if (this->_converterContext._samplerHashtoSamplerIndex.count(samplerHash) == 0) {
            index = this->_converterContext._samplerHashtoSamplerIndex.size();
            this->_converterContext._samplerHashtoSamplerIndex[samplerHash] = index;            
            addSampler = true;
        } else {
            index = this->_converterContext._samplerHashtoSamplerIndex[samplerHash];
        }
        
        std::string samplerUID = "sampler_"+GLTFUtils::toString(index);
        if (addSampler) {
            shared_ptr <JSONObject> sampler2D(new JSONObject());
            
            sampler2D->setUnsignedInt32("wrapS", wrapS);
            sampler2D->setUnsignedInt32("wrapT", wrapT);
            sampler2D->setUnsignedInt32("minFilter", minFilter);
            sampler2D->setUnsignedInt32("magFilter", maxFilter);

            shared_ptr <GLTF::JSONObject> samplers = this->_converterContext.root->createObjectIfNeeded("samplers");
            samplers->setValue(samplerUID, sampler2D);
        }
        
        return samplerUID;
    }
    
    void COLLADA2GLTFWriter::handleEffectSlot(const COLLADAFW::EffectCommon* commonProfile,
                                              std::string slotName,
                                              shared_ptr <GLTFEffect> cvtEffect,
                                              shared_ptr <JSONObject> extras)
    {
        shared_ptr <JSONObject> values = cvtEffect->getValues();
        GLTFProfile* profile = this->_converterContext.profile.get();

        ColorOrTexture slot;
        
        if (slotName == "diffuse")
            slot = commonProfile->getDiffuse();
        else if (slotName == "ambient") {
            bool lockAmbientWithDiffuse = false;
            if (extras) {
                lockAmbientWithDiffuse = extras->getBool("ambient_diffuse_lock");
            }
            if (lockAmbientWithDiffuse) {
                slot = commonProfile->getDiffuse();
            } else {
                slot = commonProfile->getAmbient();
            }
        }
        else if (slotName == "emission")
            slot = commonProfile->getEmission();
        else if (slotName == "specular")
            slot = commonProfile->getSpecular();
        else if (slotName == "reflective")
            slot = commonProfile->getReflective();
        else
            return;
        
        //retrieve the type, parameterName -> symbol -> type
        double red = 1, green = 1, blue = 1, alpha = 1;
        if (slot.isColor() && (slotName != "reflective")) {
            const Color& color = slot.getColor();
            if (slot.getType() != COLLADAFW::ColorOrTexture::UNSPECIFIED) {
                red = color.getRed();
                green = color.getGreen();
                blue = color.getBlue();
                alpha = color.getAlpha();
            }
            shared_ptr <JSONObject> slotObject(new JSONObject());
            slotObject->setValue("value", serializeVec4(red, green, blue, alpha));
            slotObject->setUnsignedInt32("type", profile->getGLenumForString("FLOAT_VEC4"));
            values->setValue(slotName, slotObject);
            
        } else if (slot.isTexture()) {
            const Texture&  texture = slot.getTexture();
            const SamplerPointerArray& samplers = commonProfile->getSamplerPointerArray();
            const Sampler* sampler = samplers[texture.getSamplerId()];
            std::string imageUID = uniqueIdWithType("image",sampler->getSourceImage());
            
            std::string texcoord = texture.getTexcoord();
            
            cvtEffect->addSemanticForTexcoordName(texcoord, slotName);
            shared_ptr <JSONObject> slotObject(new JSONObject());
            
            //do we need to export a new texture ? if yes compose a new unique ID
            slotObject->setUnsignedInt32("type", profile->getGLenumForString("SAMPLER_2D"));
            
            //do we need a new sampler ?
            GLTFProfile* profile = this->_converterContext.profile.get();
            std::string samplerUID = this->getSamplerUIDForParameters(__GetGLWrapMode(sampler->getWrapS(), profile),
                                                                      __GetGLWrapMode(sampler->getWrapT(), profile),
                                                                      __GetFilterMode(sampler->getMinFilter(), profile),
                                                                      __GetFilterMode(sampler->getMagFilter(), profile));
            
            std::string textureUID = "texture_" + imageUID;
            
            shared_ptr <GLTF::JSONObject> textures = this->_converterContext.root->createObjectIfNeeded("textures");
            if (textures->contains(textureUID) == false) {
                shared_ptr <JSONObject> textureObject(new JSONObject());
                textureObject->setString("source", imageUID);
                textureObject->setString("sampler", samplerUID);
                textureObject->setUnsignedInt32("format", profile->getGLenumForString("RGBA"));
                textureObject->setUnsignedInt32("internalFormat", profile->getGLenumForString("RGBA"));
                textureObject->setUnsignedInt32("target", profile->getGLenumForString("TEXTURE_2D"));
                textures->setValue(textureUID, textureObject);
            }

            slotObject->setString("value", textureUID);
            values->setValue(slotName, slotObject);
        } else {
            // nothing to do, no texture or color
        }
    }
        
    bool COLLADA2GLTFWriter::writeEffect( const COLLADAFW::Effect* effect )
	{
        GLTFProfile* profile = this->_converterContext.profile.get();
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
        
        if (commonEffects.getCount() > 0) {
            std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS
            uniqueId += "material.";
#else
            uniqueId += "effect.";
#endif
            uniqueId += GLTF::GLTFUtils::toString(effect->getUniqueId().getObjectId());;
            
            shared_ptr <GLTFEffect> cvtEffect(new GLTFEffect(effect->getOriginalId()));
            shared_ptr <JSONObject> values(new JSONObject());
            
            cvtEffect->setValues(values);
            
            const COLLADAFW::EffectCommon* effectCommon = commonEffects[0];
            
            switch (effectCommon->getShaderType()) {
                case COLLADAFW::EffectCommon::SHADER_BLINN:
                    cvtEffect->setLightingModel("Blinn");
                    break;
                case COLLADAFW::EffectCommon::SHADER_CONSTANT:
                    cvtEffect->setLightingModel("Constant");
                    break;
                case COLLADAFW::EffectCommon::SHADER_PHONG:
                    cvtEffect->setLightingModel("Phong");
                    break;
                case COLLADAFW::EffectCommon::SHADER_LAMBERT:
                    cvtEffect->setLightingModel("Lambert");
                    break;
                default:
                    break;
            }
            
            shared_ptr<JSONObject> extras = this->_extraDataHandler->getExtras(effect->getUniqueId());
            
            handleEffectSlot(effectCommon,"diffuse" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"ambient" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"emission" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"specular" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"reflective" , cvtEffect, extras);
            
            if (this->_converterContext.alwaysExportFilterColor) {
                shared_ptr <JSONObject> slotObject(new JSONObject());
                slotObject->setValue("value", serializeVec4(1, 1, 1, 1));
                slotObject->setUnsignedInt32("type", profile->getGLenumForString("FLOAT_VEC4"));
                values->setValue("filterColor", slotObject);
            }
            
            if (!isOpaque(effectCommon) || this->_converterContext.alwaysExportTransparency) {
                shared_ptr <JSONObject> transparency(new JSONObject());
                transparency->setDouble("value", this->getTransparency(effectCommon));
                transparency->setUnsignedInt32("type", profile->getGLenumForString("FLOAT"));
                values->setValue("transparency", transparency);
            }
            
            //should check if has specular first and the lighting model (if not lambert)
            double shininess = effectCommon->getShininess().getFloatValue();
            if (shininess >= 0) {
                if (shininess < 1) {
                    shininess *= 128.0;
                }
                shared_ptr <JSONObject> shininessObject(new JSONObject());
                shininessObject->setUnsignedInt32("type", profile->getGLenumForString("FLOAT"));
                shininessObject->setDouble("value", shininess);
                values->setValue("shininess", shininessObject);
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
        shared_ptr <GLTF::JSONObject> projectionObject(new GLTF::JSONObject());

        std::string id = uniqueIdWithType("camera", camera->getUniqueId());
        
        camerasObject->setValue(id, cameraObject);
        
        switch (camera->getCameraType()) {
            case Camera::UNDEFINED_CAMERATYPE:
                printf("WARNING: unknown camera type: using perspective\n");
                break;
            case Camera::ORTHOGRAPHIC:
            {
                cameraObject->setString("type", "orthographic");
                cameraObject->setValue("orthographic", projectionObject);
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        projectionObject->setDouble("xmag", camera->getXMag().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        projectionObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        projectionObject->setDouble("xmag", camera->getXMag().getValue());
                        projectionObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        projectionObject->setDouble("xmag", camera->getXMag().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        projectionObject->setDouble("ymag", camera->getYMag().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
            case Camera::PERSPECTIVE:
            {
                cameraObject->setString("type", "perspective");
                cameraObject->setValue("perspective", projectionObject);
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        projectionObject->setDouble("xfov", camera->getXFov().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        projectionObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        projectionObject->setDouble("xfov", camera->getXFov().getValue());
                        projectionObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        projectionObject->setDouble("xfov", camera->getXFov().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        projectionObject->setDouble("yfov", camera->getYFov().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
        }
        
        projectionObject->setDouble("znear", camera->getNearClippingPlane().getValue());
        projectionObject->setDouble("zfar", camera->getFarClippingPlane().getValue());
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeImage( const COLLADAFW::Image* openCOLLADAImage )
	{
        shared_ptr <GLTF::JSONObject> images = this->_converterContext.root->createObjectIfNeeded("images");
        shared_ptr <GLTF::JSONObject> image(new GLTF::JSONObject());
        
        std::string imageUID = uniqueIdWithType("image",openCOLLADAImage->getUniqueId());
        
        images->setValue(imageUID, image);
        /*
         bool relativePath = true;
         if (pathDir.size() > 0) {
         if ((pathDir[0] != '.') || (pathDir[0] == '/')) {
         relativePath = false;
         }
         }*/
        const COLLADABU::URI &imageURI = openCOLLADAImage->getImageURI();
        std::string relPathFile = imageURI.getPathFile();
        
        if (imageURI.getPathDir().substr(0,2) != "./") {
            relPathFile = imageURI.getPathDir() + imageURI.getPathFile();
        } else {
            relPathFile = imageURI.getPathDir().substr(2) + imageURI.getPathFile();
        }
        image->setString("path", relPathFile);

        return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLight( const COLLADAFW::Light* light )
	{
        //FIXME: add projection
        shared_ptr <JSONObject> glTFLight(new JSONObject());
        shared_ptr <JSONObject> description(new JSONObject());

        COLLADAFW::Light::LightType lightType = light->getLightType();
		Color color = light->getColor();

        float constantAttenuation =  (float)light->getConstantAttenuation().getValue();
        float linearAttenuation =  (float)light->getLinearAttenuation().getValue();
        float quadraticAttenuation =  (float)light->getQuadraticAttenuation().getValue();

        shared_ptr <JSONValue> lightColor = serializeVec3(color.getRed(), color.getGreen(), color.getBlue());
        
        switch (lightType) {
            case COLLADAFW::Light::AMBIENT_LIGHT:
                glTFLight->setString("type", "ambient");
                break;
            case COLLADAFW::Light::DIRECTIONAL_LIGHT:
                glTFLight->setString("type", "directional");
                break;
            case COLLADAFW::Light::POINT_LIGHT: {
                glTFLight->setString("type", "point");

                description->setValue("constantAttenuation", shared_ptr <JSONNumber> (new JSONNumber(constantAttenuation)));
                description->setValue("linearAttenuation", shared_ptr <JSONNumber> (new JSONNumber(linearAttenuation)));
                description->setValue("quadraticAttenuation", shared_ptr <JSONNumber> (new JSONNumber(quadraticAttenuation)));
            }
                break;
            case COLLADAFW::Light::SPOT_LIGHT: {
                glTFLight->setString("type", "spot");

                float fallOffAngle =  (float)light->getFallOffAngle().getValue();
                float fallOffExponent = (float)light->getFallOffExponent().getValue();
                
                description->setValue("constantAttenuation", shared_ptr <JSONNumber> (new JSONNumber(constantAttenuation)));
                description->setValue("linearAttenuation", shared_ptr <JSONNumber> (new JSONNumber(linearAttenuation)));
                description->setValue("quadraticAttenuation", shared_ptr <JSONNumber> (new JSONNumber(quadraticAttenuation)));
                
                description->setValue("fallOffAngle", shared_ptr <JSONNumber> (new JSONNumber(fallOffAngle)));
                description->setValue("fallOffExponent", shared_ptr <JSONNumber> (new JSONNumber(fallOffExponent)));
            }
                break;
            default:
                return false;
        }
        
        description->setValue("color", lightColor);
        glTFLight->setValue(glTFLight->getString("type"), description);
        
        this->_converterContext._uniqueIDToLight[light->getUniqueId().toAscii()] = glTFLight;
        this->_converterContext._uniqueIDToOriginalID[light->getUniqueId().toAscii()] = light->getOriginalId();
        
        shared_ptr<JSONArray> lightsIds = this->_converterContext.root->createArrayIfNeeded("lightsIds");
        lightsIds->appendValue(shared_ptr<JSONString>(new JSONString(light->getOriginalId())));
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimation( const COLLADAFW::Animation* animation)
	{
        shared_ptr <GLTFAnimation> cvtAnimation = convertOpenCOLLADAAnimationToGLTFAnimation(animation);
        
        cvtAnimation->setOriginalID(animation->getOriginalId());
        
        if (this->_converterContext._flattenerMapsForAnimationID.count(animation->getOriginalId()) == 0) {
            this->_converterContext._flattenerMapsForAnimationID[animation->getOriginalId()] = shared_ptr <AnimationFlattenerForTargetUID> (new AnimationFlattenerForTargetUID());
        }
        
        this->_converterContext._uniqueIDToAnimation[(unsigned int)animation->getUniqueId().getObjectId()] = cvtAnimation;
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimationList( const COLLADAFW::AnimationList* animationList )
	{
        const COLLADAFW::AnimationList::AnimationBindings &animationBindings = animationList->getAnimationBindings();
        
        AnimatedTargetsSharedPtr animatedTargets = this->_converterContext._uniqueIDToAnimatedTargets[animationList->getUniqueId().toAscii()];
        
        for (size_t i = 0 ; i < animationBindings.getCount() ; i++) {
            shared_ptr <GLTFAnimation> cvtAnimation = this->_converterContext._uniqueIDToAnimation[(unsigned int)animationBindings[i].animation.getObjectId()];

            AnimationFlattenerForTargetUIDSharedPtr animationFlattenerMap = this->_converterContext._flattenerMapsForAnimationID[cvtAnimation->getOriginalID()];
            for (size_t j = 0 ; j < animatedTargets->size() ; j++) {
                shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[j];
                shared_ptr<GLTFAnimationFlattener> animationFlattener;
                std::string targetUID = animatedTarget->getString("target");
                if (animationFlattenerMap->count(targetUID) == 0) {
                    COLLADAFW::Node *node = (COLLADAFW::Node*)this->_converterContext._uniqueIDToOpenCOLLADAObject[targetUID].get();
                    animationFlattener = shared_ptr<GLTFAnimationFlattener> (new GLTFAnimationFlattener(node));
                    (*animationFlattenerMap)[targetUID] = animationFlattener;
                }
            }

            cvtAnimation->registerAnimationFlatteners(animationFlattenerMap);
            
            const COLLADAFW::AnimationList::AnimationClass animationClass = animationBindings[i].animationClass;
            if (!GLTF::writeAnimation(cvtAnimation, animationClass, animatedTargets, this->_converterContext)) {
                //if an animation failed to convert, we don't want to keep track of it.
                this->_converterContext._uniqueIDToAnimation.erase(this->_converterContext._uniqueIDToAnimation.find((const unsigned int)animationBindings[i].animation.getObjectId()));
            }
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData )
	{
        shared_ptr <GLTFSkin> glTFSkin(new GLTFSkin(skinControllerData->getOriginalId()));
        
        glTFSkin->extras()->setString("uniqueId", skinControllerData->getUniqueId().toAscii());
        
        glTFSkin->setBindShapeMatrix(serializeMatrix4Array(skinControllerData->getBindShapeMatrix()));
        
		const COLLADAFW::UIntValuesArray& jointsPerVertex = skinControllerData->getJointsPerVertex();
		const COLLADAFW::IntValuesArray& jointIndices = skinControllerData->getJointIndices();
		const COLLADAFW::UIntValuesArray& weightIndices = skinControllerData->getWeightIndices();
		const COLLADAFW::FloatOrDoubleArray& weights = skinControllerData->getWeights();

        size_t vertexCount = skinControllerData->getVertexCount();
        size_t maxNumberOfInfluencesPerVertex = 0;
        //Pre-pass to figure out what's the maxium number of influences we are dealing with
        for (size_t i = 0 ; i < vertexCount ; i++) {
            if (jointsPerVertex[i] > maxNumberOfInfluencesPerVertex) {
                maxNumberOfInfluencesPerVertex = jointsPerVertex[i];
            }
        }
        
        size_t index = 0;
        size_t bucketSize = 4;
        size_t componentSize = sizeof(float);
        size_t skinAttributeSize = componentSize * vertexCount * bucketSize;
        float *weightsPtr = (float*)malloc(skinAttributeSize);
        float *bonesIndices = (float*)malloc(skinAttributeSize);

        memset(weightsPtr, 0, skinAttributeSize);
        memset(bonesIndices, 0, skinAttributeSize);
        
        for (size_t i = 0 ; i < vertexCount ; i++) {
            size_t pairCount = jointsPerVertex[i];
			if ( pairCount == 0 )
				continue;
            
			if ( weights.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT ) {
				const COLLADAFW::FloatArray* floatWeights = weights.getFloatValues();
				for (size_t j = 0; j < pairCount; ++j, ++index) {
                    if (j < bucketSize) {
                        bonesIndices[(i * bucketSize) + j] = (float)jointIndices[index];
                        weightsPtr[(i * bucketSize) + j] = (*floatWeights)[weightIndices[index]];
                    }
				}
			}
			else if ( weights.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE ) {
				//in this case, cast the double to float
                const COLLADAFW::DoubleArray* doubleWeights = weights.getDoubleValues();
				for (size_t j = 0; j < pairCount; ++j, ++index) {
                    if (j < bucketSize) {
                        bonesIndices[(i * bucketSize) + j] = (float)jointIndices[index];
                        weightsPtr[(i * bucketSize) + j]  = (float)(*doubleWeights)[weightIndices[index]];
                    }
				}
			}
        }
        
        //inverse bind matrice
        const Matrix4Array& matrices = skinControllerData->getInverseBindMatrices();
        size_t matricesSize = sizeof(float) * 16 * skinControllerData->getJointsCount();
        float *matricesPtr = (float*)malloc(matricesSize);
        for (size_t i = 0 ; i < skinControllerData->getJointsCount() ; i++) {
            __GetFloatArrayFromMatrix(matrices[i], matricesPtr + (i*16));
        }
        shared_ptr <GLTFBufferView> inverseBindMatricesView = createBufferViewWithAllocatedBuffer(matricesPtr, 0, matricesSize, true);
        glTFSkin->setInverseBindMatrices(inverseBindMatricesView);
        
        shared_ptr<JSONObject> inverseBindMatrices(new JSONObject());
        inverseBindMatrices->setUnsignedInt32("type", this->_converterContext.profile->getGLenumForString("FLOAT_MAT4"));
        inverseBindMatrices->setUnsignedInt32("count", skinControllerData->getJointsCount());
        inverseBindMatrices->setUnsignedInt32("byteOffset", 0);
        glTFSkin->extras()->setValue("inverseBindMatrices", inverseBindMatrices);
        
        inverseBindMatrices->setUnsignedInt32("byteOffset",this->_converterContext._animationOutputStream->length());
        shared_ptr<GLTFBuffer> buffer = glTFSkin->getInverseBindMatrices()->getBuffer();
        this->_converterContext._animationOutputStream->write(buffer);

        //
        shared_ptr <GLTFBufferView> weightsView = createBufferViewWithAllocatedBuffer(weightsPtr, 0, skinAttributeSize, true);
        shared_ptr <GLTFMeshAttribute> weightsAttribute(new GLTFMeshAttribute());
        
        weightsAttribute->setBufferView(weightsView);
        weightsAttribute->setComponentsPerAttribute(bucketSize);
        weightsAttribute->setByteStride(componentSize * bucketSize);
        weightsAttribute->setComponentType(GLTF::FLOAT);
        weightsAttribute->setCount(vertexCount);

        glTFSkin->setWeights(weightsAttribute);
        
        shared_ptr <GLTFBufferView> jointsView = createBufferViewWithAllocatedBuffer(bonesIndices, 0, skinAttributeSize, true);
        shared_ptr <GLTFMeshAttribute> jointsAttribute(new GLTFMeshAttribute());
        
        jointsAttribute->setBufferView(jointsView);
        jointsAttribute->setComponentsPerAttribute(bucketSize);
        jointsAttribute->setByteStride(componentSize * bucketSize);
        jointsAttribute->setComponentType(GLTF::FLOAT);
        jointsAttribute->setCount(vertexCount);

        glTFSkin->setJoints(jointsAttribute);

        this->_converterContext._uniqueIDToSkin[(unsigned int)skinControllerData->getObjectId()] = glTFSkin;
        
		return true;
	}
    
	//--------------------------------------------------------------------
    
    static shared_ptr<GLTFMeshAttribute> __CreateAttributeByApplyingRemapTable(shared_ptr<GLTFMeshAttribute> meshAttribute, size_t vertexCount, unsigned int* remapTableForPositions) {
        shared_ptr <GLTFMeshAttribute> targetAttribute(new GLTFMeshAttribute());
        unsigned char* sourcePtr = (unsigned char*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
        size_t bufferSize = meshAttribute->getVertexAttributeByteLength() * vertexCount;
        unsigned char* destinationPtr = (unsigned char*)malloc(bufferSize);

        targetAttribute->setComponentsPerAttribute(meshAttribute->getComponentsPerAttribute());
        targetAttribute->setByteStride(meshAttribute->getByteStride());
        targetAttribute->setComponentType(meshAttribute->getComponentType());
        targetAttribute->setCount(vertexCount);

        for (size_t i = 0 ; i < vertexCount  ; i++) {
            unsigned int rindex = remapTableForPositions[i];
            
            void *ptrSrc = sourcePtr + (rindex * meshAttribute->getByteStride());
            void *ptrDst = destinationPtr + (i * targetAttribute->getByteStride());
            memcpy(ptrDst, ptrSrc , meshAttribute->getVertexAttributeByteLength());
        }
        
        shared_ptr<GLTFBufferView> targetView = createBufferViewWithAllocatedBuffer(destinationPtr, 0, bufferSize, true);
        targetAttribute->setBufferView(targetView);
        
        return targetAttribute;
    }
    
	bool COLLADA2GLTFWriter::writeController( const COLLADAFW::Controller* controller ) {
        if (controller->getControllerType() == COLLADAFW::Controller::CONTROLLER_TYPE_SKIN) {
            COLLADAFW::SkinController* skinController = (COLLADAFW::SkinController*)controller;
            
            //Now we get the skin and the mesh, and
            shared_ptr <GLTFSkin> glTFSkin = this->_converterContext._uniqueIDToSkin[(unsigned int)skinController->getSkinControllerData().getObjectId()];
            shared_ptr<GLTFMesh> mesh = this->_converterContext._uniqueIDToMesh[skinController->getSource().toAscii()];
            
            glTFSkin->setSourceUID(skinController->getSource().toAscii());
            UniqueId test(glTFSkin->getSourceUID());
            
            unsigned int *remapTableForPositions = mesh->getRemapTableForPositions();
            size_t vertexCount = mesh->getMeshAttributesForSemantic(GLTF::POSITION)[0]->getCount();
            //Now we remap the bone indices and weight attribute in respect of deindexing we have
            
            shared_ptr<GLTFMeshAttribute> weightsAttribute = __CreateAttributeByApplyingRemapTable(glTFSkin->getWeights(), vertexCount, remapTableForPositions);
            GLTF::IndexSetToMeshAttributeHashmap& weightsAttributes = mesh->getMeshAttributesForSemantic(GLTF::WEIGHT);
            weightsAttributes[0] = weightsAttribute;
            mesh->setMeshAttributesForSemantic(GLTF::WEIGHT, weightsAttributes);
            
            shared_ptr<GLTFMeshAttribute> jointsAttribute = __CreateAttributeByApplyingRemapTable(glTFSkin->getJoints(), vertexCount, remapTableForPositions);
            GLTF::IndexSetToMeshAttributeHashmap& jointsAttributes = mesh->getMeshAttributesForSemantic(GLTF::JOINT);
            jointsAttributes[0] = jointsAttribute;
            mesh->setMeshAttributesForSemantic(GLTF::JOINT, jointsAttributes);
            
            //FIXME: find a way around this... the JSONVertexAttribute qualifies the semantic and set for Mesh Attribute but this could use renaming cleanup... since it is exported as "semantics", GLTFSemantic would probably be a better choice.
            GLTF::PrimitiveVector primitives = mesh->getPrimitives();
            for (size_t i = 0 ; i < primitives.size() ; i++) {
                shared_ptr<GLTFPrimitive> primitive = primitives[i];
                primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>( new GLTF::JSONVertexAttribute(GLTF::JOINT,0)));
                primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>( new GLTF::JSONVertexAttribute(GLTF::WEIGHT,0)));
            }
            
            //save the list of joints, first as uniqueIds, and then we will replace with sid's
            shared_ptr <JSONArray> joints(new JSONArray());
            UniqueIdArray& jointsUID = skinController->getJoints();
            for (size_t i = 0 ; i < jointsUID.getCount() ; i++) {
                shared_ptr<JSONString> jointId(new JSONString(jointsUID[i].toAscii()));
                joints->appendValue(jointId);
            }
            glTFSkin->setJointsIds(joints);
            
        }
		return true;
	}
    
} // namespace COLLADA2JSON
