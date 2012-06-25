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
    void JSONAccessor::_generateID() 
    {
        this->_ID = JSONUtils::generateIDForType("accessor");
    }
    
    JSONAccessor::JSONAccessor() : 
    _min(0),
    _max(0)
    {
        this->setElementType(NOT_A_SOURCE_TYPE);
        this->setByteStride(0);
        this->setByteOffset(0);
        this->_generateID();
        this->setElementsPerVertexAttribute(0);
        this->setCount(0);
    }
        
    JSONAccessor::JSONAccessor(JSONAccessor* accessor): 
    _buffer(accessor->getBuffer()),
    _min(0),
    _max(0)
    {
        this->setElementType(accessor->getElementType());
        this->setByteStride(accessor->getByteStride());
        this->setByteOffset(accessor->getByteOffset());
        this->_generateID();
        this->setElementsPerVertexAttribute(accessor->getElementsPerVertexAttribute());
        this->setCount(accessor->getCount());
    }
        
    JSONAccessor::~JSONAccessor()
    {
        if (this->_min) {
            free(this->_min);
        }
        if (this->_max) {
            free(this->_max);
        }
    }
        
    void JSONAccessor::setBuffer(shared_ptr <JSONExport::JSONBuffer> buffer)
    {
        this->_buffer = buffer;
    }
        
    shared_ptr <JSONExport::JSONBuffer> JSONAccessor::getBuffer()
    {
        return this->_buffer;
    }
        
    void JSONAccessor::setElementsPerVertexAttribute(size_t elementsPerVertexAttribute)
    {
        this->_elementsPerVertexAttribute = elementsPerVertexAttribute;
    }
        
    size_t JSONAccessor::getElementsPerVertexAttribute()
    {
        return this->_elementsPerVertexAttribute;
    }
        
    void JSONAccessor::setByteStride(size_t byteStride)
    {
        this->_byteStride = byteStride;
    }
        
    size_t JSONAccessor::getByteStride()
    {
        return this->_byteStride;
    }
        
    void JSONAccessor::setElementType(SourceType elementType)
    {
        this->_elementType = elementType;
    }
        
    SourceType JSONAccessor::getElementType()
    {
        return this->_elementType;
    }
        
    void JSONAccessor::setByteOffset(size_t byteOffset)
    {
        this->_byteOffset = byteOffset;
    }
        
    size_t JSONAccessor::getByteOffset()
    {
        return this->_byteOffset;
    }
        
    size_t JSONAccessor::getCount()
    {
        return this->_count;
    }
    
    void JSONAccessor::setCount(size_t count)
    {
        this->_count = count;
    }
        
    const std::string& JSONAccessor::getID()
    {
        return this->_ID;
    }
        
    size_t JSONAccessor::getElementByteLength()
    {
        size_t elementsPerVertexAttribute = this->getElementsPerVertexAttribute();
        SourceType type = this->getElementType();
        switch (type) {
            case JSONExport::BYTE:
            case JSONExport::UNSIGNED_BYTE:
                return elementsPerVertexAttribute;
            case JSONExport::SHORT:
                return elementsPerVertexAttribute * sizeof(short);                    
            case JSONExport::UNSIGNED_SHORT:
                return elementsPerVertexAttribute * sizeof(unsigned short);                    
            case JSONExport::FIXED:
                return elementsPerVertexAttribute * sizeof(int);                    
            case JSONExport::FLOAT:
                return elementsPerVertexAttribute * sizeof(float);
            default: 
                // FIXME: report error or just log ?
                break;
        }
        
        // FIXME: report error or just log ?
        return 0;
    }
    
    void* JSONAccessor::getMin()
    {
        return this->_min;
    }

    void* JSONAccessor::getMax()
    {
        return this->_max;
    }

    void JSONAccessor::computeMinMax() 
    {
        if (this->_min) {
            free(this->_min);
        }
        if (this->_max) {
            free(this->_max);
        }
        
        this->_min = malloc(this->getElementByteLength());
        this->_max = malloc(this->getElementByteLength());

        size_t byteStride = this->getByteStride();
        size_t elementsPerVertexAttribute = this->getElementsPerVertexAttribute();
        
        SourceType type = this->getElementType();
        switch (type) {
            case JSONExport::FLOAT: {
                
                shared_ptr <JSONExport::JSONBuffer> buffer = this->getBuffer();
                char* bufferData = ((char*)  ((JSONDataBuffer*)buffer.get())->getData() + this->getByteOffset());
                float* vector = (float*)bufferData;
                
                float* min = (float*)this->_min;
                float* max = (float*)this->_max;
                
                for (size_t i = 0 ; i < elementsPerVertexAttribute ; i++) {
                    min[i] = max[i] = vector[i];
                }
                
                size_t count = this->getCount();
                for (size_t i = 0 ; i < count ; i++) {
                    vector = (float*)(bufferData + (i * byteStride));
                    
                    for (size_t j = 0 ; j < elementsPerVertexAttribute ; j++) {
                        if (vector[j] < min[j]) {
                            min[j] = vector[j];
                        }
                        if (vector[j] > max[j]) {
                            max[j] = vector[j];
                        }
                    }
                }
                
            }
                break;    
            default:
                break;
        }
    }
    
                    
}
