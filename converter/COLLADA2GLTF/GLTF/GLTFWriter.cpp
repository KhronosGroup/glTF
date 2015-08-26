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
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{    
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

    //-- Default Writer
    
    GLTFDefaultWriter::GLTFDefaultWriter() :
        _writer(0)
    {
        _fd = 0;
    }

    bool GLTFDefaultWriter::initWithPath(const std::string &path) {
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
        
    GLTFDefaultWriter::~GLTFDefaultWriter() {
        if (_fd) {
            delete this->_fileStream;
            delete this->_writer;
            fclose(this->_fd);
        }
    }
        
    //base
    void GLTFDefaultWriter::writeArray(JSONArray* array, void *context) {
        this->_writer->StartArray();
        
        vector <shared_ptr <JSONValue> > values = array->values();
        size_t count = values.size();
        for (size_t i = 0 ; i < count ; i++) {
            values[i]->write(this, context);
        }
        
        this->_writer->EndArray();
    }
    
    void GLTFDefaultWriter::writeObject(JSONObject* object, void *context) {
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
    
    void GLTFDefaultWriter::writeNumber(JSONNumber* number, void *context) {
        JSONNumber::JSONNumberType type = number->getNumberType();
        
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
        
    void GLTFDefaultWriter::writeString(JSONString* str, void *context) {
        this->_writer->String(str->getCString());
    }
    
    void GLTFDefaultWriter::write(JSONValue* value, void* context) {
        JSONType jsonType = value->getJSONType();
        
        if (jsonType == kJSONNumber) {
            this->writeNumber((JSONNumber*)value, context);
        } else if (jsonType == kJSONObject) {
            this->writeObject((JSONObject*)value, context);
        } else if (jsonType == kJSONArray) {
            this->writeArray((JSONArray*)value, context);
        } else if (jsonType == kJSONString) {
            this->writeString((JSONString*)value, context);
        } 
        
    }
        
}
