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

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF 
{
    //-- Serializers

    shared_ptr <GLTF::JSONObject> serializeBuffer(GLTFBuffer* buffer, void *context)
    {
        shared_ptr <GLTF::JSONObject> bufferObject(new GLTF::JSONObject());
        
        bufferObject->setUnsignedInt32("byteLength", (unsigned int)buffer->getByteLength());
        bufferObject->setString("path", buffer->getID());
        
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
        
        shared_ptr <GLTF::JSONObject> techniques(new GLTF::JSONObject());
        
        std::string techniqueID = effect->getTechniqueID();
        
        effectObject->setString("technique", techniqueID);
        effectObject->setString("name", effect->getName());

        shared_ptr <GLTF::JSONObject> technique = effect->getTechnique();
        techniques->setValue(techniqueID, technique);
        effectObject->setValue("techniques", techniques);
        
        return effectObject;
    }
    
    shared_ptr <GLTF::JSONObject> serializeMesh(GLTFMesh* mesh, void *context)
    {
        shared_ptr <GLTF::JSONObject> meshObject(new GLTF::JSONObject());
        
        meshObject->setString("name", mesh->getName());
        
        //primitives
        shared_ptr <GLTF::JSONArray> primitivesArray(new GLTF::JSONArray());
        meshObject->setValue("primitives", primitivesArray);
        
        //accessors
        shared_ptr <GLTF::JSONObject> accessorsObject(new GLTF::JSONObject());
        meshObject->setValue("attributes", accessorsObject);
        
        shared_ptr <MeshAttributeVector> allMeshAttributes = mesh->accessors();
        
        PrimitiveVector primitives = mesh->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
            shared_ptr <GLTF::GLTFIndices> uniqueIndices =  primitive->getUniqueIndices();
            
            void *primitiveContext[2];
            
            primitiveContext[0] = mesh;
            primitiveContext[1] = context;
            
            shared_ptr <GLTF::JSONObject> primitiveObject = serializePrimitive(primitive.get(), primitiveContext);
            
            primitivesArray->appendValue(primitiveObject);
        }
        
        vector <GLTF::Semantic> allSemantics = mesh->allSemantics();
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            GLTF::Semantic semantic = allSemantics[i];
            
            GLTF::IndexSetToMeshAttributeHashmap::const_iterator accessorIterator;
            GLTF::IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = mesh->getMeshAttributesForSemantic(semantic);
            
            //FIXME: consider turn this search into a method for mesh
            for (accessorIterator = indexSetToMeshAttribute.begin() ; accessorIterator != indexSetToMeshAttribute.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <GLTF::GLTFMeshAttribute> accessor = (*accessorIterator).second;
                
                shared_ptr <GLTF::JSONObject> accessorObject = serializeMeshAttribute(accessor.get(), context);
                
                accessorsObject->setValue(accessor->getID(), accessorObject);
            }
        }
        
        return meshObject;
    }
    
    shared_ptr <GLTF::JSONObject> serializeMeshAttribute(GLTFMeshAttribute* accessor, void *context)
    {
        shared_ptr <JSONObject> accessorObject = shared_ptr<JSONObject>(new JSONObject());
        
        accessorObject->setUnsignedInt32("byteStride", (unsigned int)accessor->getByteStride());
        accessorObject->setUnsignedInt32("byteOffset", (unsigned int)accessor->getByteOffset());
        accessorObject->setUnsignedInt32("componentsPerAttribute", (unsigned int)accessor->getElementsPerVertexAttribute());
        accessorObject->setUnsignedInt32("count", (unsigned int)accessor->getCount());
        accessorObject->setString("componentType", GLTFUtils::getStringForGLType(accessor->getComponentType()));
        
        void** buffers = (void**)context;
        GLTFBufferView *bufferView = context ? (GLTFBufferView*)buffers[0] : accessor->getBufferView().get();

        accessorObject->setString("bufferView", bufferView->getID());
        
        const double* min = accessor->getMin();
        if (min) {
            shared_ptr <GLTF::JSONArray> minArray(new GLTF::JSONArray());
            accessorObject->setValue("min", minArray);
            for (size_t i = 0 ; i < accessor->getElementsPerVertexAttribute() ; i++) {
                minArray->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(min[i])));
            }
        }
        
        const double* max = accessor->getMax();
        if (max) {
            shared_ptr <GLTF::JSONArray> maxArray(new GLTF::JSONArray());
            accessorObject->setValue("max", maxArray);
            for (size_t i = 0 ; i < accessor->getElementsPerVertexAttribute() ; i++) {
                maxArray->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(max[i])));
            }
        }
        
        return accessorObject;
    }
    
    
    shared_ptr <GLTF::JSONObject> serializeIndices(GLTFIndices* indices, void *context)
    {
        shared_ptr <GLTF::JSONObject> indicesObject(new GLTF::JSONObject());
        void** buffers = (void**)context;
        
        GLTFBufferView *bufferView = context ? (GLTFBufferView*)buffers[1] : indices->getBufferView().get();
        
        indicesObject->setString("type", GLTFUtils::getStringForGLType(GLTF::UNSIGNED_SHORT));
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
        
        size_t count = primitive->getIndicesInfosCount();
        for (size_t j = 0 ; j < count ; j++) {
            GLTF::Semantic semantic = primitive->getSemanticAtIndex(j);
            
            std::string semanticAndSet = GLTFUtils::getStringForSemantic(semantic);
            
            unsigned int indexOfSet = 0;
            if ((semantic != GLTF::POSITION) && (semantic != GLTF::NORMAL)) {
                indexOfSet = primitive->getIndexOfSetAtIndex(j);
                semanticAndSet += "_" + GLTFUtils::toString(indexOfSet);
            }
            semantics->setString(semanticAndSet,
                                     mesh->getMeshAttributesForSemantic(semantic)[indexOfSet]->getID());
        }
        
        shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
        shared_ptr <GLTF::JSONObject> serializedIndices = serializeIndices(uniqueIndices.get(), primitiveContext[1]);
        primitiveObject->setValue("indices", serializedIndices);
        
        return primitiveObject;
    }
    
    shared_ptr <GLTF::JSONValue> serializeVec3(double x,double y, double z) {
        shared_ptr <GLTF::JSONArray> vec3(new GLTF::JSONArray());
        
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(x)));
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(y)));
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(z)));
        
        return vec3;
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
