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

#include "document.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;


namespace GLTF 
{
        
    shared_ptr <JSONObject> JSONObjectWithContentsOfFile(std::string filepath, char** error)
    {
        shared_ptr <GLTF::JSONObject> outObject(new JSONObject());

        outObject->initWithContentsOfFile(filepath.c_str(), error);
        
        return outObject;
    }
        
    void JSONObject::_parseRapidJSONObject(void *value) {
        rapidjson::Value *rapidjsonValue = (rapidjson::Value *)value;
        
        for (Value::ConstMemberIterator itr = rapidjsonValue->MemberBegin(); itr != rapidjsonValue->MemberEnd(); ++itr) {
            std::string key = itr->name.GetString();
            rapidjson::Value *currentValue = (rapidjson::Value *)&itr->value;

            switch (itr->value.GetType()) {
                case kNullType:
                    break;
                case kFalseType:
                case kTrueType:
                    this->setBool(key, kTrueType ? true : false);
                    break;
                case kObjectType: {
                    shared_ptr<JSONObject> obj(new JSONObject());
                    obj->_parseRapidJSONObject(currentValue);
                    this->setValue(key, obj);
                }
                    break;
                case kArrayType: {
                    shared_ptr<JSONArray> array(new JSONArray());
                    array->_parseRapidJSONArray(currentValue);
                    this->setValue(key, array);
                }
                    break;
                case kStringType:
                    this->setString(key, currentValue->GetString());
                    break;
                case kNumberType:
                    if (rapidjsonValue->IsDouble()) {
                        this->setDouble(key, currentValue->GetDouble());
                    } else if (rapidjsonValue->IsInt() || currentValue->IsInt64()) {
                        this->setInt32(key, currentValue->GetInt());
                    } else if (currentValue->IsUint() || currentValue->IsUint64()) {
                        this->setUnsignedInt32(key, currentValue->GetInt());
                    }
                    
                    break;
            }
        }
    }
    
    bool JSONObject::initWithContentsOfFile(const char *filepath, char **error)
    {
        bool status = false;
        FILE * file = fopen(filepath, "rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            unsigned long size = ftell(file);
            char* content = (char*)malloc(size + 1);
            rewind(file);
            unsigned int nread = (unsigned int) fread((void *) content, 1, size, file);
            content[size] = 0;
            if (nread == size) {
                status = this->initWithCString(content, error);
            }
            fclose(file);
        }
        return status;
    }
    
    bool JSONObject::initWithCString(const char *jsonString, char **error)
    {
        rapidjson::Document document;

        if (document.Parse<0>(jsonString).HasParseError()) {
            return false;
        } else {
            this->_parseRapidJSONObject(&document);
        }
        return true;
    }
    
    JSONObject::JSONObject():
    JSONValue(GLTF::OBJECT)
    {
    }
    
    JSONObject::~JSONObject()
    {
    }
    
    shared_ptr <GLTF::JSONObject> JSONObject::createObjectIfNeeded(const std::string& key) {
        shared_ptr <GLTF::JSONObject> outObject;
        if (!contains(key)) {
            outObject = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
            setValue(key, outObject);
        } else {
            outObject = static_pointer_cast <GLTF::JSONObject> (getValue(key));
        }
        return outObject;
    }
    
    shared_ptr <GLTF::JSONArray> JSONObject::createArrayIfNeeded(const std::string& key) {
        shared_ptr <GLTF::JSONArray> outObject;
        if (!contains(key)) {
            outObject = shared_ptr <GLTF::JSONArray> (new GLTF::JSONArray());
            setValue(key, outObject);
        } else {
            outObject = static_pointer_cast <GLTF::JSONArray> (getValue(key));
        }
        return outObject;
    }

    
    void JSONObject::setValue(const std::string &key, shared_ptr <JSONValue> value)
    {
        this->_keyToJSONValue[key] = value;
    }
    
    void JSONObject::removeValue(const std::string &key)
    {
        this->_keyToJSONValue.erase(key);
    }

    
    shared_ptr <JSONValue> JSONObject::getValue(std::string key)
    {
        return this->_keyToJSONValue[key];
    }
    
    shared_ptr <JSONObject> JSONObject::getObject(std::string key)
    {
        shared_ptr <JSONValue> value = this->_keyToJSONValue[key];
        return static_pointer_cast <JSONObject> (value);
    }

    void JSONObject::setUnsignedInt32(const std::string &key, unsigned int value)
    {
        this->setValue(key, shared_ptr <JSONNumber> (new JSONNumber((unsigned int)value)));
    }

    unsigned int JSONObject::getUnsignedInt32(const std::string &key)
    {
        if (this->contains(key)) {
            shared_ptr <JSONNumber> number = static_pointer_cast <JSONNumber> (this->getValue(key));
            return number->getUnsignedInt32();
        }
        return 0;
    }
    
    void JSONObject::setBool(const std::string &key, bool value)
    {
        this->setValue(key, shared_ptr <JSONNumber> (new JSONNumber(value)));
    }
    
    bool JSONObject::getBool(const std::string &key)
    {
        if (this->contains(key)) {
            shared_ptr <JSONNumber> number = static_pointer_cast <JSONNumber> (this->getValue(key));
            
            return number->getBool();
        }
        return false;
    }
    
    void JSONObject::setInt32(const std::string &key, int value)
    {
        this->setValue(key, shared_ptr <JSONNumber> (new JSONNumber((int)value)));
    }
    
    int JSONObject::getInt32(const std::string &key)
    {
        if (this->contains(key)) {
            shared_ptr <JSONNumber> number = static_pointer_cast <JSONNumber> (this->getValue(key));
            return number->getInt32();
        }
        return 0;
    }
    
    void JSONObject::setDouble(const std::string &key, double value)
    {
        this->setValue(key, shared_ptr <JSONNumber> (new JSONNumber((double)value)));
    }
    
    double JSONObject::getDouble(const std::string &key)
    {
        if (this->contains(key)) {
            shared_ptr <JSONNumber> number = static_pointer_cast <JSONNumber> (this->getValue(key));
            return number->getDouble();
        }
        return 0;
    }
    
    void JSONObject::setString(const std::string &key, const std::string &value)
    {
        this->setValue(key, shared_ptr <JSONString> (new JSONString(value)));
    }

    const std::string JSONObject::getString(const std::string &key)
    {
        if (this->contains(key)) {
            shared_ptr <JSONString> str = static_pointer_cast <JSONString> (this->getValue(key));
            return str->getString();
        }
        return "";
    }
    
    vector <std::string> JSONObject::getAllKeys()
    {
        vector <std::string> allKeys;
            
        KeyToJSONValue::const_iterator KeyToJSONValueIterator;
        
        for (KeyToJSONValueIterator = this->_keyToJSONValue.begin() ; KeyToJSONValueIterator != this->_keyToJSONValue.end() ; KeyToJSONValueIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                //allKeys.push_back((*KeyToJSONValueIterator).first);
                std::string key = (*KeyToJSONValueIterator).first;
                allKeys.push_back(key);
        }
        
        return allKeys;
    }
    
    bool JSONObject::contains(const std::string &key)
    {
        return this->_keyToJSONValue.count(key) > 0;
    }
    
    bool JSONObject::isEmpty() 
    {
        return this->_keyToJSONValue.empty();
    }
    
    size_t JSONObject::getKeysCount() {
        return this->_keyToJSONValue.size();
    }

}
