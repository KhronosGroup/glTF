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
    void JSONAccessor::_generateID() 
    {
        this->_ID = JSONUtils::generateIDForType("accessor");
    }
    
    JSONAccessor::JSONAccessor(): _min(0), _max(0) 
    {
        this->setElementType(NOT_AN_ELEMENT_TYPE);
        this->setByteStride(0);
        this->_byteOffset = 0;
        this->_generateID();
        this->setElementsPerVertexAttribute(0);
        this->setCount(0);
    }
        
    JSONAccessor::JSONAccessor(JSONAccessor* accessor): 
    _bufferView(accessor->getBufferView()), _min(0), _max(0)
    {
        assert(accessor);
        
        this->setElementType(accessor->getElementType());
        this->setByteStride(accessor->getByteStride());
        this->_byteOffset = accessor->getByteOffset();
        this->_generateID();
        this->setElementsPerVertexAttribute(accessor->getElementsPerVertexAttribute());
        this->setCount(accessor->getCount());
    }
        
    JSONAccessor::~JSONAccessor()
    {
        if (this->_min) {
            delete [] this->_min;
        }
        if (this->_max) {
            delete [] this->_max;
        }
    }
        
    void JSONAccessor::setBufferView(shared_ptr <JSONBufferView> bufferView)
    {
        this->_bufferView = bufferView;
    }
        
    shared_ptr <JSONBufferView> JSONAccessor::getBufferView()
    {
        return this->_bufferView;
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
        
    void JSONAccessor::setElementType(ElementType elementType)
    {
        this->_elementType = elementType;
    }
        
    ElementType JSONAccessor::getElementType()
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
        
    size_t JSONAccessor::getVertexAttributeByteLength()
    {
        size_t elementsPerVertexAttribute = this->getElementsPerVertexAttribute();
        ElementType type = this->getElementType();
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
    
    const double* JSONAccessor::getMin() {
        return this->_min;
    }

    const double* JSONAccessor::getMax() {
        return this->_max;
    }
    
    typedef struct {
        double *min, *max;
    } __MinMaxApplierInfo;
    
    static void __ComputeMinMax(void *value,
                               ElementType type,
                               size_t elementsPerVertexAttribute,
                               size_t index,
                               size_t vertexAttributeByteSize,
                               void *context) {
        __MinMaxApplierInfo *applierInfo = (__MinMaxApplierInfo*)context;
        char* bufferData = (char*)value;

        switch (type) {
            case JSONExport::FLOAT: {
                float* vector = (float*)bufferData;
                for (size_t j = 0 ; j < elementsPerVertexAttribute ; j++) {
                    float value = vector[j];
                    if (value < applierInfo->min[j]) {
                        applierInfo->min[j] = value;
                    }
                    if (value > applierInfo->max[j]) {
                        applierInfo->max[j] = value;
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    
    void JSONAccessor::computeMinMax() 
    {
        //size_t byteStride = this->getByteStride();
        size_t elementsPerVertexAttribute = this->getElementsPerVertexAttribute();

        //realloc
        if (this->_min) {
            delete [] this->_min;
        }
        if (this->_max) {
            delete [] this->_max;
        }
        this->_min = new double[elementsPerVertexAttribute];
        this->_max = new double[elementsPerVertexAttribute];

        __MinMaxApplierInfo minMaxApplierInfo;
        minMaxApplierInfo.min = this->_min;
        minMaxApplierInfo.max = this->_max;
        
        for (size_t i = 0 ; i < elementsPerVertexAttribute ; i++) {
            this->_min[i] = DBL_MAX;
            this->_max[i] = -DBL_MAX;
        }
        
        apply(__ComputeMinMax, &minMaxApplierInfo);
    }
    
    void JSONAccessor::apply(JSONAccessorApplierFunc applierFunc, void* context)
    {
        size_t byteStride = this->getByteStride();
        size_t elementsPerVertexAttribute = this->getElementsPerVertexAttribute();
        size_t vertexAttributeByteSize = this->getVertexAttributeByteLength();
        shared_ptr <JSONExport::JSONBufferView> bufferView = this->getBufferView();
        ElementType type = this->getElementType();
        unsigned char* bufferData = (unsigned char*)bufferView->getBufferDataByApplyingOffset();

        for (size_t i = 0 ; i < _count ; i++) {
            (*applierFunc)(bufferData + (i * byteStride), type, elementsPerVertexAttribute, i, vertexAttributeByteSize, context);
        }
    }
    
    bool JSONAccessor::matchesLayout(JSONAccessor* accessor)
    {
        assert(accessor);
        
        return ((accessor->getElementsPerVertexAttribute() == this->getElementsPerVertexAttribute()) &&
                (accessor->getByteStride() == this->getByteStride()) &&
                (accessor->getElementType() == this->getElementType()) &&
                (accessor->getVertexAttributeByteLength() == this->getVertexAttributeByteLength()));
    }
                    
}
