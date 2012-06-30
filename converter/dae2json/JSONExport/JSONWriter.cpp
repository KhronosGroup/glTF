// Copyright (c) 2011-2012, Motorola Mobility, Inc.
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

#include "JSONExport.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace JSONExport 
{
    JSONWriter::JSONWriter(rapidjson::PrettyWriter <rapidjson::FileStream> *writer):
    _writer(writer)
    {
    }
    
    JSONWriter::JSONWriter():
    _writer(0)
    {
    }
    
    JSONWriter::~JSONWriter()
    {
    }
    
    void JSONWriter::setWriter(rapidjson::PrettyWriter <rapidjson::FileStream> *writer)
    {
        this->_writer = writer;
    }
    
    rapidjson::PrettyWriter <rapidjson::FileStream>* JSONWriter::getWriter()
    {
        return this->_writer;
    }
    
    //base
    void JSONWriter::writeArray(JSONArray* array, void *context)
    {
        this->_writer->StartArray();
        
        vector <shared_ptr <JSONValue>> values = array->values();
        size_t count = values.size();
        for (int i = 0 ; i < count ; i++) {
            values[i]->write(this, context);
        }
        
        this->_writer->EndArray();
    }
    
    void JSONWriter::writeObject(JSONObject* object, void *context)
    {
        this->_writer->StartObject(); 

        vector <std::string> keys = object->getAllKeys();
        size_t count = keys.size();
        
        for (size_t i = 0 ; i < count ; i++) {
            shared_ptr <JSONValue> value = object->getValue(keys[i]);
            this->_writer->String(keys[i].c_str());
            value->write(this, context);
        }
        
        this->_writer->EndObject(); 
    }
    
    void JSONWriter::writeNumber(JSONNumber* number, void *context)
    {
        JSONNumber::JSONNumberType type = number->getType();
        
        switch (type) {
            case JSONNumber::JSONNumberType::UNSIGNED_INT32:
                this->_writer->Uint(number->getUnsignedInt32());
                break;
            case JSONNumber::JSONNumberType::INT32:
                this->_writer->Int(number->getInt32());
                break;
            case JSONNumber::JSONNumberType::DOUBLE:
            {   
                double value = number->getDouble();
                this->_writer->Double(value);
            }
                break;
            default:
                //FIXME: TODO Handle error
                break;
        }
    }
        
    void JSONWriter::writeString(JSONString* str, void *context)
    {
        this->_writer->String(str->getCString());
    }
        
    shared_ptr <JSONExport::JSONObject> JSONWriter::serializeBuffer(JSONBuffer* buffer, void *context)
    {
        shared_ptr <JSONExport::JSONObject> bufferObject(new JSONExport::JSONObject());
        
        bufferObject->setString("type", "ArrayBuffer");
        bufferObject->setUnsignedInt32("byteLength", buffer->getByteSize());
        bufferObject->setString("path", buffer->getID());
        
        return bufferObject;
    }
    
    shared_ptr <JSONExport::JSONObject> JSONWriter::serializeEffect(JSONEffect* effect, void *context)
    {
        shared_ptr <JSONExport::JSONObject> effectObject(new JSONExport::JSONObject());
        shared_ptr <JSONExport::JSONArray> colorObject(new JSONExport::JSONArray());
            
        effectObject->setValue("color", colorObject);
        
        float *rgbColor = effect->getRGBColor();
        colorObject->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)rgbColor[0])));
        colorObject->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)rgbColor[1])));
        colorObject->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)rgbColor[2])));

        return effectObject;
    }
        
    shared_ptr <JSONExport::JSONObject> JSONWriter::serializeMesh(JSONMesh* mesh, void *context)
    {
        typedef map<std::string , shared_ptr<JSONExport::JSONBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
                
        shared_ptr <JSONExport::JSONObject> meshObject(new JSONExport::JSONObject());
                
        meshObject->setString("type", "mesh");
        meshObject->setString("name", mesh->getName());

        shared_ptr <JSONExport::JSONArray> primitivesArray(new JSONExport::JSONArray());

        meshObject->setValue("primitives", primitivesArray);
        
        vector <shared_ptr< JSONExport::JSONAccessor> > allAccessors = mesh->remappedAccessors();
        
        std::vector< shared_ptr<JSONExport::JSONPrimitive> > primitives = mesh->getPrimitives();
        unsigned int primitivesCount =  primitives.size();
        for (int i = 0 ; i < primitivesCount ; i++) {
            
            shared_ptr<JSONExport::JSONPrimitive> primitive = primitives[i];   
            shared_ptr <JSONExport::JSONIndices> uniqueIndices =  primitive->getUniqueIndices();

            void *primitiveContext[2];
            
            primitiveContext[0] = mesh;
            primitiveContext[1] = context;
            
            shared_ptr <JSONExport::JSONObject> primitiveObject = this->serializePrimitive(primitive.get(), primitiveContext);
            
            primitivesArray->appendValue(primitiveObject);
            
            //FIXME: check to remove that code
            unsigned int indicesCount = primitive->getUniqueIndices()->getCount();
            if (indicesCount <= 0) {
                // FIXME: report error
            } else {
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                shared_ptr <JSONExport::JSONBuffer> indiceBuffer(new JSONExport::JSONBuffer(uniqueIndices->getBuffer()->getID(), indicesLength)); 
                IDToBuffer[indiceBuffer->getID()] = indiceBuffer;
            }
            
        }
                
        vector <JSONExport::Semantic> allSemantics = mesh->allSemantics();
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            JSONExport::Semantic semantic = allSemantics[i];
            
            JSONExport::IndexSetToAccessorHashmap::const_iterator accessorIterator;
            JSONExport::IndexSetToAccessorHashmap& indexSetToAccessor = mesh->getAccessorsForSemantic(semantic);
            
            //FIXME: consider turn this search into a method for mesh
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> accessor = (*accessorIterator).second;
                
                shared_ptr <JSONExport::JSONObject> accessorObject = this->serializeAccessor(accessor.get(), context);

                meshObject->setValue(accessor->getID(), accessorObject);
            }
        }
        
        return meshObject;
    }
    
    shared_ptr <JSONExport::JSONObject> JSONWriter::serializeAccessor(JSONAccessor* accessor, void *context)
    {
        shared_ptr <JSONObject> accessorObject = shared_ptr<JSONObject>(new JSONObject());
                
        accessorObject->setString("type", "accessor");
        accessorObject->setUnsignedInt32("byteStride", accessor->getByteStride());
        accessorObject->setUnsignedInt32("byteOffset", accessor->getByteOffset());
        accessorObject->setUnsignedInt32("elementsPerValue", accessor->getElementsPerVertexAttribute());
        accessorObject->setUnsignedInt32("count", accessor->getCount());
        accessorObject->setString("elementType", JSONUtils::getStringForType(accessor->getElementType()));
        
        JSONBuffer* buffer = context ? (JSONBuffer*)context : accessor->getBuffer().get();

        accessorObject->setString("buffer", buffer->getID());
        
        //FIXME: should not assume float here
        float* min = (float*)accessor->getMin();
        if (min) {         
            shared_ptr <JSONExport::JSONArray> minArray(new JSONExport::JSONArray());
            
            accessorObject->setValue("min", minArray);
            
            minArray->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)min[0])));
            minArray->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)min[1])));
            minArray->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)min[2])));
        }
        
        //FIXME: should not assume float here
        float* max = (float*)accessor->getMax();
        if (max) {
            shared_ptr <JSONExport::JSONArray> maxArray(new JSONExport::JSONArray());
            
            accessorObject->setValue("max", maxArray);
            
            maxArray->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)max[0])));
            maxArray->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)max[1])));
            maxArray->appendValue(shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)max[2])));
        }
        
        return accessorObject;
    }

    
    shared_ptr <JSONExport::JSONObject> JSONWriter::serializeIndices(JSONIndices* indices, void *context)
    {
        shared_ptr <JSONExport::JSONObject> indicesObject(new JSONExport::JSONObject());

        JSONBuffer* buffer = context ? (JSONBuffer*)context : indices->getBuffer().get();

        indicesObject->setString("type", JSONUtils::getStringForTypedArray(JSONExport::UNSIGNED_SHORT));
        indicesObject->setString("buffer", buffer->getID());
        indicesObject->setUnsignedInt32("byteOffset", indices->getByteOffset());
        indicesObject->setUnsignedInt32("length", indices->getCount());

        return indicesObject;
    }
    
    shared_ptr <JSONExport::JSONObject> JSONWriter::serializePrimitive(JSONPrimitive* primitive, void *context)
    {
        void** primitiveContext = (void**)context;
        shared_ptr <JSONExport::JSONObject> primitiveObject(new JSONExport::JSONObject());

        JSONMesh* mesh = (JSONMesh*)primitiveContext[0];
        
        primitiveObject->setString("type", "primitive");
        primitiveObject->setString("primitive", primitive->getType());
        primitiveObject->setString("material", primitive->getMaterialID());
        
        shared_ptr <JSONExport::JSONArray> vertexAttributesArray(new JSONExport::JSONArray());
        primitiveObject->setValue("vertexAttributes", vertexAttributesArray);
        
        size_t count = primitive->allIndices().size();
        for (int j = 0 ; j < count ; j++) {
            shared_ptr <JSONExport::JSONObject> indicesObject(new JSONExport::JSONObject());
            shared_ptr <JSONExport::JSONIndices> indices = primitive->allIndices()[j];
            
            JSONExport::Semantic semantic = indices->getSemantic();
            
            vertexAttributesArray->appendValue(indicesObject);
            
            indicesObject->setString("semantic", JSONUtils::getStringForSemantic(semantic));            
            unsigned int indexOfSet = 0;
            if (mesh->getAccessorsForSemantic(semantic).size() > 1) {
                indexOfSet = indices->getIndexOfSet();
                indicesObject->setString("set", JSONUtils::toString(indices->getIndexOfSet()));
            }                                    
            indicesObject->setString("accessor", mesh->getAccessorsForSemantic(semantic)[indexOfSet]->getID());
        }
        
        shared_ptr <JSONExport::JSONIndices> uniqueIndices = primitive->getUniqueIndices();
        
        shared_ptr <JSONExport::JSONObject> serializedIndices = this->serializeIndices(uniqueIndices.get(), primitiveContext[1]);
        
        primitiveObject->setValue("indices", serializedIndices);
            
        return primitiveObject;
    }
    
    void JSONWriter::write(JSONValue* value, void* context) 
    {
        switch (value->getType()) {
            case JSONExport::NUMBER:
                this->writeNumber((JSONNumber*)value, context);
                break;
            case JSONExport::OBJECT:
                this->writeObject((JSONObject*)value, context);
                break;
            case JSONExport::ARRAY:
                this->writeArray((JSONArray*)value, context);
                break;
            case JSONExport::STRING:
                this->writeString((JSONString*)value, context);
                break;
            default:
                //FIXME: report error, should not reach
                break;
        }
    }
        
}
