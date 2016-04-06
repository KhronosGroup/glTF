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

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{
    JSONValue::JSONValue() {
    }

    JSONValue::~JSONValue() {
    }
    
//    static void __eval(JSONValue* value, void *context) {
  //      value->evaluate(context);
    //}

    void JSONValue::write(GLTFWriter* writer, void* context) {
        //value->apply(__eval, context);
        writer->write(this, context);
    }
    
    shared_ptr<JSONValue> JSONValue::valueForKeyPath(std::string keyPath) {
        std::size_t pos = keyPath.find(".");
        if (pos == std::string::npos) {
            if (this->getJSONType() == kJSONObject) {
                JSONObject *currentObject = (JSONObject*)this;
                if (currentObject->contains(keyPath)) {
                    return currentObject->getObject(keyPath);
                }
            }
        } else {
            //here we expect an object
            if (this->getJSONType() == kJSONObject) {
                //we have an object, so we can continue parsing
                //First, we need to get the substring after the dot
                std::string currentPath = keyPath.substr(0,pos);
                JSONObject *currentObject = (JSONObject*)this;
                if (currentObject->contains(currentPath)) {
                    shared_ptr<JSONObject> nextObject = currentObject->getObject(currentPath);
                    pos += 1; //skip the dot
                    std::string nextPath = keyPath.substr(pos);
                    return nextObject->valueForKeyPath(nextPath);
                }
            }
        }
        return shared_ptr<JSONValue> ((JSONValue*)0);
    }
    
    void JSONValue::evaluate(void*) {
    }

    void JSONValue::apply(JSONValueApplierFunc func, void* context) {
        (*func)(this, context);
    }
    
    void JSONValue::apply(JSONValueApplier* applier, void* context) {
        applier->apply(this, context);
    }
    
    bool JSONValue::isEqualTo(JSONValue *value) {
        if (value->getJSONType() != this->getJSONType())
            return false;
        if (value == this)
            return true;
        
        return false;
    }
}