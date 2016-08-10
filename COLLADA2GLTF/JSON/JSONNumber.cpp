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
#include <cstring>

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{    
    JSONNumber::JSONNumber(unsigned int value):
    _type(UNSIGNED_INT32) {
        this->_value = malloc(sizeof(unsigned int));
        memcpy(this->_value, &value, sizeof(unsigned int));
    }
    
    JSONNumber::JSONNumber(int value):
    _type(INT32) {
        this->_value = malloc(sizeof(int));
        memcpy(this->_value, &value, sizeof(int));
    }
    
    JSONNumber::JSONNumber(double value):
    _type(DOUBLE)        
    {
        this->_value = malloc(sizeof(double));
        memcpy(this->_value, &value, sizeof(double));
    }

    JSONNumber::JSONNumber(bool value):
    _type(BOOL)        
    {
        this->_value = malloc(sizeof(bool));
        memcpy(this->_value, &value, sizeof(bool));
    }
    
    JSONNumber::~JSONNumber() 
    {
        free(this->_value);
    }
        
    unsigned int JSONNumber::getUnsignedInt32() {
        unsigned int value = 0;
        if (this->_type == UNSIGNED_INT32) {
            memcpy(&value, this->_value, sizeof(unsigned int));
            return value;
        }
        
        switch (this->_type) {
            case INT32:
                value  = (unsigned int)this->getInt32();
                break;
            case DOUBLE:
                value  = (unsigned int)this->getDouble();
                break;
            case BOOL:
                value  = (unsigned int)this->getBool();
                break;
            default:
                break;
        }
        
        return value;
    }        
    
    int JSONNumber::getInt32() {
        int value = 0;
        if (this->_type == INT32) {
            memcpy(&value, this->_value, sizeof(int));
            return value;
        }

        switch (this->_type) {
            case UNSIGNED_INT32:
                value  = (int)this->getUnsignedInt32();
                break;
            case DOUBLE:
                value  = (int)this->getDouble();
                break;
            case BOOL:
                value  = (int)this->getBool();
                break;
            default:
                break;
        }
        
        return value;
    }        
    
    double JSONNumber::getDouble() {
        double value = 0;
        if (this->_type == DOUBLE) {
            memcpy(&value, this->_value, sizeof(double));
            return value;
        }

        switch (this->_type) {
            case UNSIGNED_INT32:
                value  = (double)this->getUnsignedInt32();
                break;
            case INT32:
                value  = (double)this->getInt32();
                break;
            case BOOL:
                value  = (double)this->getBool();
                break;
            default:
                break;
        }

        return value;
    }

    bool JSONNumber::getBool() {
        bool value = 0;
        
        if (this->_type == BOOL) {
            memcpy(&value, this->_value, sizeof(bool));
            return value;
        }
        
        switch (this->_type) {
            case UNSIGNED_INT32:
				value = (this->getUnsignedInt32() != 0);
                break;
            case INT32:
				value = (this->getInt32() != 0);
                break;
            case DOUBLE:
                value  = this->getBool();
                break;
            default:
                break;
        }
        return value;
    }

    JSONNumber::JSONNumberType JSONNumber::getNumberType()
    {
        return this->_type;
    }
    
    JSONType JSONNumber::getJSONType() {
        return kJSONNumber;
    }
    
    std::string JSONNumber::valueType() {
        return "number";
    }
    
    bool JSONNumber::isEqualTo(JSONValue* value) {
        assert(value != nullptr);
        
        if (JSONValue::isEqualTo(value) == true)
            return true;
        
        JSONNumber *numberValue = (JSONNumber*)(value);
        switch(this->_type) {
            case UNSIGNED_INT32:
                return this->getUnsignedInt32() == numberValue->getUnsignedInt32();
            case INT32:
                return this->getInt32() == numberValue->getInt32();
            case DOUBLE:
                return this->getDouble() == numberValue->getDouble();
            case BOOL:
                return this->getBool() == numberValue->getBool();
            default:
                break;
        }
        
        return false;
    }

}
