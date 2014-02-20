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

#ifndef __JSON_ARRAY__
#define __JSON_ARRAY__

#include "GLTF.h"

#include "document.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF 
{
    
    void JSONArray::_parseRapidJSONArray(void *value) {
        rapidjson::Value *rapidjsonValue = (rapidjson::Value *)value;
        
        for (Value::ConstValueIterator currentValue = rapidjsonValue->Begin(); currentValue != rapidjsonValue->End(); ++currentValue)
            
            switch (currentValue->GetType()) {
                case kNullType:
                    break;
                case kFalseType:
                case kTrueType:
                    this->appendValue(shared_ptr<JSONNumber> (new JSONNumber(kTrueType ? true: false)));
                    break;
                case kObjectType: {
                    shared_ptr<JSONObject> obj(new JSONObject());
                    obj->_parseRapidJSONObject((void*)currentValue);
                    this->appendValue(obj);
                }
                    break;
                case kArrayType: {
                    shared_ptr<JSONArray> obj(new JSONArray());
                    obj->_parseRapidJSONArray((void*)currentValue);
                    this->appendValue(obj);
                }
                    break;
                case kStringType:
                    this->appendValue(shared_ptr<JSONString> (new JSONString(currentValue->GetString())));
                    break;
                case kNumberType:
                    if (rapidjsonValue->IsDouble()) {
                        this->appendValue(shared_ptr<JSONNumber> (new JSONNumber(currentValue->GetDouble())));
                    } else if (rapidjsonValue->IsInt() || currentValue->IsInt64()) {
                        this->appendValue(shared_ptr<JSONNumber> (new JSONNumber(currentValue->GetInt())));
                    } else if (currentValue->IsUint() || currentValue->IsUint64()) {
                        this->appendValue(shared_ptr<JSONNumber> (new JSONNumber(currentValue->GetUint())));
                    }
                    
                    break;
            }
    }

    JSONArray::JSONArray():
    JSONValue(),  _values(_allvalues) {
    }
        
    JSONArray::~JSONArray() {
    }        
    
    JSONValueVectorRef JSONArray::values() {
        return this->_values;
    }
    
    void JSONArray::appendValue(shared_ptr <JSONValue> value) {
        this->_values.push_back(value);
    }

    JSONType JSONArray::getJSONType() {
        return kJSONArray;
    }
    
    void JSONArray::apply(JSONValueApplierFunc func, void* context) {
        vector <shared_ptr <JSONValue> > values = this->values();
        size_t count = values.size();
        for (size_t i = 0 ; i < count ; i++) {
            values[i]->apply(func, context);
        }
    }

}

#endif
