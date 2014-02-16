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

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF 
{
    void GLTFAccessor::_generateID()  {
        this->_ID = GLTFUtils::generateIDForType("accessor");
    }
    
    GLTFAccessor::GLTFAccessor(shared_ptr<GLTFProfile> profile, unsigned int glType):
    JSONObject(),
    _min(0),
    _max(0) {
        this->setUnsignedInt32(kType, glType);
        this->_componentsPerElement = profile->getComponentsCountForGLType(glType);
        this->_elementByteLength = profile->sizeOfGLType(glType);
        this->_componentType = profile->getComponentTypeForGLType(glType);
        
        this->setByteStride(0);
        this->setByteOffset(0);
        this->_generateID();
        this->setCount(0);
    }
        
    GLTFAccessor::GLTFAccessor(GLTFAccessor* accessor):
    JSONObject(),
    _bufferView(accessor->getBufferView()),
    _min(0),
    _max(0) {
        assert(accessor);
        this->setUnsignedInt32(kType, accessor->type());
        this->_componentsPerElement = accessor->componentsPerElement();
        this->_elementByteLength = accessor->elementByteLength();
        this->_componentType = accessor->componentType();
        this->setByteStride(accessor->getByteStride());
        this->setByteOffset(accessor->getByteOffset());
        this->_generateID();
        this->setCount(accessor->getCount());
    }
        
    GLTFAccessor::~GLTFAccessor() {
        if (this->_min) {
            delete [] this->_min;
        }
        if (this->_max) {
            delete [] this->_max;
        }
    }
        
    void GLTFAccessor::setBufferView(shared_ptr <GLTFBufferView> bufferView) {
        this->_bufferView = bufferView;
    }
        
    shared_ptr <GLTFBufferView> GLTFAccessor::getBufferView() {
        return this->_bufferView;
    }
    
    size_t GLTFAccessor::componentsPerElement() {
        return this->_componentsPerElement;
    }

    ComponentType GLTFAccessor::componentType() {
        return this->_componentType;
    }

    void GLTFAccessor::setByteStride(size_t byteStride) {
        this->setUnsignedInt32(kByteStride, byteStride);
    }
        
    size_t GLTFAccessor::getByteStride() {
        return this->getUnsignedInt32(kByteStride);
    }
            
    void GLTFAccessor::setByteOffset(size_t byteOffset) {
        this->setUnsignedInt32(kByteOffset, byteOffset);
    }
        
    size_t GLTFAccessor::getByteOffset() {
        return this->getUnsignedInt32(kByteOffset);
    }
        
    size_t GLTFAccessor::getCount() {
        return this->getUnsignedInt32(kCount);
    }
    
    void GLTFAccessor::setCount(size_t count) {
        this->setUnsignedInt32(kCount, count);
    }
 
    const std::string& GLTFAccessor::getID() {
        return this->_ID;
    }
        
    size_t GLTFAccessor::elementByteLength() {
        return this->_elementByteLength;
    }
    
    const double* GLTFAccessor::getMin() {
        return this->_min;
    }

    const double* GLTFAccessor::getMax() {
        return this->_max;
    }
    
    typedef struct {
        double *min, *max;
    } __MinMaxApplierInfo;
    
    static void __ComputeMinMax(void *value,
                               ComponentType type,
                               size_t componentsPerElement,
                               size_t index,
                               size_t vertexAttributeByteSize,
                               void *context) {
        __MinMaxApplierInfo *applierInfo = (__MinMaxApplierInfo*)context;
        char* bufferData = (char*)value;

        switch (type) {
            case GLTF::FLOAT: {
                float* vector = (float*)bufferData;
                for (size_t j = 0 ; j < componentsPerElement ; j++) {
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
    
    void GLTFAccessor::computeMinMax()  {
        size_t componentsPerElement = this->_componentsPerElement;

        //realloc
        if (this->_min) {
            delete [] this->_min;
        }
        if (this->_max) {
            delete [] this->_max;
        }
        this->_min = new double[componentsPerElement];
        this->_max = new double[componentsPerElement];

        __MinMaxApplierInfo minMaxApplierInfo;
        minMaxApplierInfo.min = this->_min;
        minMaxApplierInfo.max = this->_max;
        
        for (size_t i = 0 ; i < componentsPerElement ; i++) {
            this->_min[i] = DBL_MAX;
            this->_max[i] = -DBL_MAX;
        }
        
        apply(__ComputeMinMax, &minMaxApplierInfo);
    }
    
    void GLTFAccessor::apply(GLTFAccessorApplierFunc applierFunc, void* context) {
        size_t byteStride = this->getByteStride();
        size_t componentsPerElement = this->_componentsPerElement;
        size_t vertexAttributeByteSize = this->_elementByteLength;
        shared_ptr <GLTF::GLTFBufferView> bufferView = this->getBufferView();
        ComponentType type = this->_componentType;
        unsigned char* bufferData = (unsigned char*)bufferView->getBufferDataByApplyingOffset();

        size_t count = this->getCount();
        for (size_t i = 0 ; i < count ; i++) {
            (*applierFunc)(bufferData + (i * byteStride), type, componentsPerElement, i, vertexAttributeByteSize, context);
        }
    }
    
    bool GLTFAccessor::matchesLayout(GLTFAccessor* meshAttribute) {
        assert(meshAttribute);
        
        return ((meshAttribute->componentsPerElement() == this->componentsPerElement()) &&
                (meshAttribute->getByteStride() == this->getByteStride()) &&
                (meshAttribute->componentType() == this->componentType()) &&
                (meshAttribute->elementByteLength() == this->elementByteLength()));
    }
    
    unsigned int GLTFAccessor::type() {
        return this->getUnsignedInt32(kType);
    }

}
