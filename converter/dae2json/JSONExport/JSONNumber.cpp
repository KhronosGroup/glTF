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

#include "JSONExport.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace JSONExport 
{    
    JSONNumber::JSONNumber(unsigned int value):
    JSONValue(JSONExport::NUMBER),
    _type(JSONNumberType::UNSIGNED_INT32)        
    {
        this->_value = malloc(sizeof(unsigned int));
        memcpy(this->_value, &value, sizeof(unsigned int));
    }
    
    JSONNumber::JSONNumber(int value):
    JSONValue(JSONExport::NUMBER),
    _type(JSONNumberType::INT32)        
    {
        this->_value = malloc(sizeof(int));
        memcpy(this->_value, &value, sizeof(int));
    }
    
    JSONNumber::JSONNumber(double value):
    JSONValue(JSONExport::NUMBER),
    _type(JSONNumberType::DOUBLE)        
    {
        this->_value = malloc(sizeof(double));
        memcpy(this->_value, &value, sizeof(double));
    }

    JSONNumber::JSONNumber(bool value):
    JSONValue(JSONExport::NUMBER),
    _type(JSONNumberType::BOOL)        
    {
        this->_value = malloc(sizeof(bool));
        memcpy(this->_value, &value, sizeof(bool));
    }
    
    JSONNumber::~JSONNumber() 
    {
        free(this->_value);
    }
    
    void JSONNumber::write(JSONWriter* writer, void* context)
    {
        writer->writeNumber(this, context);
    }
    
    unsigned int JSONNumber::getUnsignedInt32() {
        unsigned int value = 0;
        if (this->_type == JSONNumberType::UNSIGNED_INT32)
            memcpy(&value, this->_value, sizeof(unsigned int));
        return value;
    }        
    
    int JSONNumber::getInt32() {
        int value = 0;
        if (this->_type == JSONNumberType::INT32)
            memcpy(&value, this->_value, sizeof(int));
        return value;
    }        
    
    double JSONNumber::getDouble() {
        double value = 0;
        if (this->_type == JSONNumberType::DOUBLE)
            memcpy(&value, this->_value, sizeof(double));
        return value;
    }      

    bool JSONNumber::getBool() {
        bool value = 0;
        if (this->_type == JSONNumberType::BOOL)
            memcpy(&value, this->_value, sizeof(bool));
        return value;
    }      

    JSONNumber::JSONNumberType JSONNumber::getType()
    {
        return this->_type;
    }

        
}
