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

#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "../GLTFConverterContext.h"
#include "../shaders/commonProfileShaders.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF 
{
    //-- Serializers

    shared_ptr <GLTF::JSONObject> serializeBuffer(GLTFBuffer* buffer, std::string path, void *context)
    {
        shared_ptr <GLTF::JSONObject> bufferObject(new GLTF::JSONObject());
        
        bufferObject->setUnsignedInt32("byteLength", (unsigned int)buffer->getByteLength());
        bufferObject->setString("path", path);
        
        return bufferObject;
    }
    
    shared_ptr <GLTF::JSONObject> serializeBufferView(GLTFBufferView* bufferView, void *context)
    {
        shared_ptr <GLTF::JSONObject> bufferObject(new GLTF::JSONObject());
        
        bufferObject->setUnsignedInt32("byteLength", (unsigned int)bufferView->getByteLength());
        bufferObject->setUnsignedInt32("byteOffset", (unsigned int)bufferView->getByteOffset());
        bufferObject->setString("buffer", bufferView->getBuffer()->getID());
        
        return bufferObject;
    }

    shared_ptr <GLTF::JSONObject> serializeEffect(GLTFEffect* effect, void *context)
    {
        shared_ptr <GLTF::JSONObject> effectObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> instanceTechnique(new GLTF::JSONObject());
        shared_ptr <JSONObject> techniqueGenerator = effect->getTechniqueGenerator();
        
        GLTF::GLTFConverterContext *converterContext= (GLTF::GLTFConverterContext*)context;
        
        std::string techniqueID = GLTF::getReferenceTechniqueID(techniqueGenerator, *converterContext);
        
        effectObject->setString("name", effect->getName());
        effectObject->setValue("instanceTechnique", instanceTechnique);
        instanceTechnique->setString("technique", techniqueID);
        shared_ptr<JSONObject> outputs(new JSONObject());
        shared_ptr <JSONObject> values = effect->getValues();
        std::vector <std::string> keys = values->getAllKeys();
        for (size_t i = 0 ; i < keys.size() ; i++) {
            shared_ptr <JSONObject> parameter = static_pointer_cast <JSONObject> (values->getValue(keys[i]));
            shared_ptr <JSONObject> parameterValue = static_pointer_cast <JSONObject> (parameter->getValue("value"));
            shared_ptr<JSONObject> output(new JSONObject());
            if (parameterValue) {
                outputs->setValue(keys[i], parameterValue);
            }
        }
        instanceTechnique->setValue("values", outputs);
    
        return effectObject;
    }
    
    shared_ptr <GLTF::JSONObject> serializeMesh(GLTFMesh* mesh, void *context)
    {
        shared_ptr <GLTF::JSONObject> meshObject(new GLTF::JSONObject());
        
        meshObject->setString("name", mesh->getName());
        
        //primitives
        shared_ptr <GLTF::JSONArray> primitivesArray(new GLTF::JSONArray());
        meshObject->setValue("primitives", primitivesArray);
        
        PrimitiveVector primitives = mesh->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
            
            void *primitiveContext[2];
            
            primitiveContext[0] = mesh;
            primitiveContext[1] = context;
            
            shared_ptr <GLTF::JSONObject> primitiveObject = serializePrimitive(primitive.get(), primitiveContext);
            
            primitivesArray->appendValue(primitiveObject);
        }
        
        if (mesh->getExtensions()->getKeysCount() > 0) {
            meshObject->setValue("extensions", mesh->getExtensions());
            if (mesh->getExtensions()->contains("won-compression")) {
                shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->getValue("won-compression"));
                if (compressionObject->contains("compressedData")) {
                    shared_ptr<JSONObject> compressionData = compressionObject->getObject("compressedData");
                    GLTFBufferView *bufferView = (GLTFBufferView*)((void**)context)[0];
                    compressionData->setString("bufferView", bufferView->getID());
                }
                
            }
            
            if (mesh->getExtensions()->contains("Open3DGC-compression")) {
                shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->getValue("Open3DGC-compression"));
                if (compressionObject->contains("compressedData")) {
                    shared_ptr<JSONObject> compressionData = compressionObject->getObject("compressedData");
                    GLTFBufferView *bufferView = (GLTFBufferView*)((void**)context)[0];
                    compressionData->setString("bufferView", bufferView->getID());
                }
                
            }
        }
        
        return meshObject;
    }
    
    shared_ptr <JSONObject> serializeMeshAttribute(GLTFMeshAttribute* meshAttribute, void *context)
    {
        shared_ptr <JSONObject> meshAttributeObject = shared_ptr<JSONObject>(new JSONObject());
        void** serializationContext = (void**)context;

        GLTFConverterContext *converterContext = (GLTFConverterContext*)serializationContext[3];
        
        meshAttributeObject->setUnsignedInt32("byteStride", (unsigned int)meshAttribute->getByteStride());
        meshAttributeObject->setUnsignedInt32("byteOffset", (unsigned int)meshAttribute->getByteOffset());
        meshAttributeObject->setUnsignedInt32("count", (unsigned int)meshAttribute->getCount());
        meshAttributeObject->setUnsignedInt32("type", meshAttribute->getGLType(converterContext->profile));
        
        GLTFBufferView *bufferView = context ? (GLTFBufferView*)serializationContext[0] : meshAttribute->getBufferView().get();

        meshAttributeObject->setString("bufferView", bufferView->getID());
        
        const double* min = meshAttribute->getMin();
        if (min) {
            shared_ptr <GLTF::JSONArray> minArray(new GLTF::JSONArray());
            meshAttributeObject->setValue("min", minArray);
            for (size_t i = 0 ; i < meshAttribute->getComponentsPerAttribute() ; i++) {
                minArray->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(min[i])));
            }
        }
        
        const double* max = meshAttribute->getMax();
        if (max) {
            shared_ptr <GLTF::JSONArray> maxArray(new GLTF::JSONArray());
            meshAttributeObject->setValue("max", maxArray);
            for (size_t i = 0 ; i < meshAttribute->getComponentsPerAttribute() ; i++) {
                maxArray->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(max[i])));
            }
        }
        
        return meshAttributeObject;
    }
    
    
    shared_ptr <GLTF::JSONObject> serializeIndices(GLTFIndices* indices, void *context)
    {
        shared_ptr <GLTF::JSONObject> indicesObject(new GLTF::JSONObject());
        void** serializationContext = (void**)context;
        GLTFConverterContext *converterContext = (GLTFConverterContext*)serializationContext[3];

        GLTFBufferView *bufferView = (GLTFBufferView*)serializationContext[1];
        
        indicesObject->setUnsignedInt32("type", converterContext->profile->getGLenumForString("UNSIGNED_SHORT"));
        indicesObject->setString("bufferView", bufferView->getID());
        indicesObject->setUnsignedInt32("byteOffset", (unsigned int)indices->getByteOffset());
        indicesObject->setUnsignedInt32("count", (unsigned int)indices->getCount());
        
        return indicesObject;
    }
    
    shared_ptr <GLTF::JSONObject> serializePrimitive(GLTFPrimitive* primitive, void *context)
    {
        void** primitiveContext = (void**)context;
        shared_ptr <GLTF::JSONObject> primitiveObject(new GLTF::JSONObject());
        
        GLTFMesh* mesh = (GLTFMesh*)primitiveContext[0];
        
        primitiveObject->setString("primitive", primitive->getType());
        
        primitiveObject->setString("material", primitive->getMaterialID());
        
        shared_ptr <GLTF::JSONObject> semantics(new GLTF::JSONObject());
        primitiveObject->setValue("semantics", semantics);
        
        size_t count = primitive->getVertexAttributesCount();
        for (size_t j = 0 ; j < count ; j++) {
            GLTF::Semantic semantic = primitive->getSemanticAtIndex(j);
            
            std::string semanticAndSet = GLTFUtils::getStringForSemantic(semantic);
            
            unsigned int indexOfSet = 0;
            if (semantic == GLTF::TEXCOORD) {
                indexOfSet = primitive->getIndexOfSetAtIndex(j);
                semanticAndSet += "_" + GLTFUtils::toString(indexOfSet);
            }
            semantics->setString(semanticAndSet, mesh->getMeshAttributesForSemantic(semantic)[indexOfSet]->getID());
        }
        shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
        primitiveObject->setString("indices", uniqueIndices->getID());

        return primitiveObject;
    }
    
    shared_ptr <JSONValue> serializeVec3(double x,double y, double z) {
        shared_ptr <JSONArray> vec3(new GLTF::JSONArray());
        
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(x)));
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(y)));
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(z)));
        
        return vec3;
    }
    
    shared_ptr <JSONValue> serializeVec4(double x,double y, double z, double w) {
        shared_ptr <JSONArray> vec4(new GLTF::JSONArray());
        
        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(x)));
        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(y)));
        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(z)));

        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(w)));

        return vec4;
    }

    shared_ptr<JSONObject> serializeAnimationParameter(GLTFAnimation::Parameter* animationParameter, void *context) {
        shared_ptr <JSONObject> animationParameterObject(new JSONObject());
        GLTFConverterContext* converterContext = (GLTFConverterContext*)context;
        
        animationParameterObject->setString("bufferView", animationParameter->getBufferView()->getID());
        animationParameterObject->setUnsignedInt32("type", converterContext->profile->getGLenumForString(animationParameter->getType()));
        animationParameterObject->setUnsignedInt32("byteOffset", animationParameter->getByteOffset());
        animationParameterObject->setUnsignedInt32("count", animationParameter->getCount());
        
        return animationParameterObject;
    }

    shared_ptr<JSONObject> serializeAnimation(GLTFAnimation* animation, void *context) {
        shared_ptr <JSONObject> animationObject(new JSONObject());
        shared_ptr <JSONObject> parametersObject(new JSONObject());
        GLTFConverterContext* converterContext = (GLTFConverterContext*)context;

        animationObject->setUnsignedInt32("count", animation->getCount());
        animationObject->setValue("samplers", animation->samplers());
        animationObject->setValue("channels", animation->channels());
        
        std::vector <shared_ptr <GLTFAnimation::Parameter> >  *parameters = animation->parameters();
        for (size_t i = 0 ; i < parameters->size() ; i++) {
            shared_ptr<JSONObject> parameterObject = serializeAnimationParameter((*parameters)[i].get(), converterContext);
            
            parametersObject->setValue((*parameters)[i]->getID(), parameterObject);
        }
        animationObject->setValue("parameters", parametersObject);
        
        return animationObject;
    }
    
    shared_ptr<JSONObject> serializeSkin(GLTFSkin* skin) {
        shared_ptr <JSONObject> skinObject(new JSONObject());
        
        skinObject->setValue("joints", skin->getJointsIds());
        skinObject->setValue("bindShapeMatrix", skin->getBindShapeMatrix());
        
        return skinObject;
    }
    
    //-- Writer
    
    GLTFWriter::GLTFWriter(rapidjson::PrettyWriter <rapidjson::FileStream> *writer):
    _writer(writer)
    {
    }
    
    GLTFWriter::GLTFWriter():
    _writer(0)
    {
    }
    
    GLTFWriter::~GLTFWriter()
    {
    }
    
    void GLTFWriter::setWriter(rapidjson::PrettyWriter <rapidjson::FileStream> *writer)
    {
        this->_writer = writer;
    }
    
    rapidjson::PrettyWriter <rapidjson::FileStream>* GLTFWriter::getWriter()
    {
        return this->_writer;
    }
    
    //base
    void GLTFWriter::writeArray(JSONArray* array, void *context)
    {
        this->_writer->StartArray();
        
        vector <shared_ptr <JSONValue> > values = array->values();
        size_t count = values.size();
        for (size_t i = 0 ; i < count ; i++) {
            values[i]->write(this, context);
        }
        
        this->_writer->EndArray();
    }
    
    void GLTFWriter::writeObject(JSONObject* object, void *context)
    {
        this->_writer->StartObject(); 

        vector <std::string> keys = object->getAllKeys();
        size_t count = keys.size();
        
        for (size_t i = 0 ; i < count ; i++) {
            shared_ptr <JSONValue> value = object->getValue(keys[i]);
            const std::string& key = keys[i];
            this->_writer->String(key.c_str());
            if (value)
                value->write(this, context);
        }
        
        this->_writer->EndObject(); 
    }
    
    void GLTFWriter::writeNumber(JSONNumber* number, void *context)
    {
        JSONNumber::JSONNumberType type = number->getType();
        
        switch (type) {
            case JSONNumber::UNSIGNED_INT32:
                this->_writer->Uint(number->getUnsignedInt32());
                break;
            case JSONNumber::INT32:
                this->_writer->Int(number->getInt32());
                break;
            case JSONNumber::DOUBLE:
            {   
                double value = number->getDouble();
                this->_writer->Double(value);
                break;
            }
            case JSONNumber::BOOL:
            {   
                bool value = number->getBool();
                this->_writer->Bool(value);
            }
                break;
            default:
                //FIXME: TODO Handle error
                break;
        }
    }
        
    void GLTFWriter::writeString(JSONString* str, void *context)
    {
        this->_writer->String(str->getCString());
    }
    
    void GLTFWriter::write(JSONValue* value, void* context)
    {
        switch (value->getType()) {
            case GLTF::NUMBER:
                this->writeNumber((JSONNumber*)value, context);
                break;
            case GLTF::OBJECT:
                this->writeObject((JSONObject*)value, context);
                break;
            case GLTF::ARRAY:
                this->writeArray((JSONArray*)value, context);
                break;
            case GLTF::STRING:
                this->writeString((JSONString*)value, context);
                break;
            default:
                //FIXME: report error, should not reach
                break;
        }
    }
        
}
