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

#ifndef __GLTF__
#define __GLTF__

#include "GLTF.h"

using namespace std;
using namespace std::tr1;

namespace GLTF 
{
    JSONString::JSONString() : JSONValue() {
    }

    JSONString::JSONString(const std::string& str) : JSONValue() {
        this->_str = str;
    }

    JSONString::~JSONString() {
    }        
        
    const char* JSONString::getCString() {
        return this->_str.c_str();
    }
    
    const std::string& JSONString::getString() {
        return this->_str;
    }
    
    JSONType JSONString::getJSONType() {
        return kJSONString;
    }
    
    bool JSONString::isEqualTo(JSONValue* value) {
        assert(value != nullptr);

        if (JSONValue::isEqualTo(value) == true)
            return true;
        
        JSONString *stringValue = (JSONString*)(value);
        
        return this->_str == stringValue->getString();
    }
    
}
#endif

