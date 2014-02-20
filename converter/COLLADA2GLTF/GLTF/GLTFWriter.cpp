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
#include "GLTFAsset.h"
#include "../shaders/commonProfileShaders.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF 
{
    //-- Serializers

    shared_ptr <GLTF::JSONObject> serializeEffect(GLTFEffect* effect, void *context) {
        shared_ptr <GLTF::JSONObject> effectObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> instanceTechnique(new GLTF::JSONObject());
        shared_ptr <JSONObject> techniqueGenerator = effect->getTechniqueGenerator();
        
        GLTF::GLTFAsset *asset= (GLTF::GLTFAsset*)context;
        
        std::string techniqueID = GLTF::getReferenceTechniqueID(techniqueGenerator, *asset);
        
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
    
    /*
    shared_ptr <GLTF::JSONObject> serializeMesh(GLTFMesh* mesh, void *context)
    {
        shared_ptr <GLTF::JSONObject> meshObject(new GLTF::JSONObject());
        meshObject->setString(kName, mesh->getName());

        mesh->resolveAttributes();
                
        //primitives
        meshObject->setValue(kPrimitives, mesh->getPrimitives());

        if (mesh->getExtensions()->getKeysCount() > 0) {
            meshObject->setValue(kExtensions, mesh->getExtensions());
            if (mesh->getExtensions()->contains("won-compression")) {
                shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->getValue("won-compression"));
                if (compressionObject->contains("compressedData")) {
                    shared_ptr<JSONObject> compressionData = compressionObject->getObject("compressedData");
                    GLTFBufferView *bufferView = (GLTFBufferView*)((void**)context)[0];
                    compressionData->setString(kBufferView, bufferView->getID());
                }
            }
            
            if (mesh->getExtensions()->contains("Open3DGC-compression")) {
                shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->getValue("Open3DGC-compression"));
                if (compressionObject->contains("compressedData")) {
                    shared_ptr<JSONObject> compressionData = compressionObject->getObject("compressedData");
                    GLTFBufferView *bufferView = (GLTFBufferView*)((void**)context)[0];
                    compressionData->setString(kBufferView, bufferView->getID());
                }
            }
        }
        
        return meshObject;
    }
    */
    
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

    //-- Writer
    
    GLTFWriter::GLTFWriter():
    _writer(0) {
        _fd = 0;
    }

    bool GLTFWriter::initWithPath(const std::string &path) {
        this->_fd = fopen(path.c_str(), "w");
        if (this->_fd) {
            this->_fileStream = new rapidjson::FileStream(this->_fd);
            if (this->_fileStream) {
                this->_writer = new rapidjson::PrettyWriter <rapidjson::FileStream>(*this->_fileStream);
                return this->_writer != 0;
            }
        }
        
        return false;
    }
        
    GLTFWriter::~GLTFWriter() {
        if (_fd) {
            delete this->_fileStream;
            delete this->_writer;
            fclose(this->_fd);
        }
    }
        
    //base
    void GLTFWriter::writeArray(JSONArray* array, void *context) {
        this->_writer->StartArray();
        
        vector <shared_ptr <JSONValue> > values = array->values();
        size_t count = values.size();
        for (size_t i = 0 ; i < count ; i++) {
            values[i]->write(this, context);
        }
        
        this->_writer->EndArray();
    }
    
    void GLTFWriter::writeObject(JSONObject* object, void *context) {
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
    
    void GLTFWriter::writeNumber(JSONNumber* number, void *context) {
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
        
    void GLTFWriter::writeString(JSONString* str, void *context) {
        this->_writer->String(str->getCString());
    }
    
    void GLTFWriter::write(JSONValue* value, void* context) {
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
