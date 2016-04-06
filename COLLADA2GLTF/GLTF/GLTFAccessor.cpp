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
    void GLTFAccessor::_generateID()  {
        this->_ID = GLTFUtils::generateIDForType("accessor");
    }
    
    GLTFAccessor::GLTFAccessor(shared_ptr<GLTFProfile> profile, const std::string& componentType, const std::string& type):
    JSONObject(),
    _bufferView(nullptr),
    _minMaxDirty(true)
    {        
        this->setString(kType, type);
        unsigned int glType = profile->getGLTypeForComponentTypeAndType(componentType, type);

        this->_componentType = componentType;
        this->_componentsPerElement = profile->getComponentsCountForGLType(glType);
        this->_elementByteLength = profile->sizeOfGLType(glType);
        this->setUnsignedInt32(kComponentType, profile->getGLComponentTypeForGLType(glType));
        this->setByteStride(0);
        this->setByteOffset(0);
        this->_generateID();
        this->setCount(0);
    }
        
    GLTFAccessor::GLTFAccessor(GLTFAccessor* accessor):
    JSONObject(),
    _bufferView(accessor->getBufferView()),
    _minMaxDirty(true)
   {
        assert(accessor);
        this->setString(kType, accessor->type());
        this->_componentsPerElement = accessor->componentsPerElement();
        this->_elementByteLength = accessor->elementByteLength();
        this->setUnsignedInt32(kComponentType, accessor->GLComponentType());
        this->_componentType = accessor->componentType();
        this->setByteStride(accessor->getByteStride());
        this->setByteOffset(accessor->getByteOffset());
        this->_generateID();
        this->setCount(accessor->getCount());
    }
        
    GLTFAccessor::~GLTFAccessor() {
    }
        
    void GLTFAccessor::setBufferView(shared_ptr <GLTFBufferView> bufferView) {
        this->_bufferView = bufferView;
        this->_minMaxDirty = true;
        this->setString(kBufferView, bufferView->getID());
    }
        
    shared_ptr <GLTFBufferView> GLTFAccessor::getBufferView() {
        return this->_bufferView;
    }
    
    size_t GLTFAccessor::componentsPerElement() {
        return this->_componentsPerElement;
    }

    std::string GLTFAccessor::componentType() {
        return this->_componentType;
    }
    
    unsigned int GLTFAccessor::GLComponentType() {
        return this->getUnsignedInt32(kComponentType);
    }

    void GLTFAccessor::setByteStride(size_t byteStride) {
		this->setUnsignedInt32(kByteStride, (unsigned int)byteStride);
    }
        
    size_t GLTFAccessor::getByteStride() {
        return this->getUnsignedInt32(kByteStride);
    }
            
    void GLTFAccessor::setByteOffset(size_t byteOffset) {
		this->setUnsignedInt32(kByteOffset, (unsigned int)byteOffset);
    }
        
    size_t GLTFAccessor::getByteOffset() {
        return this->getUnsignedInt32(kByteOffset);
    }
        
    size_t GLTFAccessor::getCount() {
        return this->getUnsignedInt32(kCount);
    }
    
    void GLTFAccessor::setCount(size_t count) {
		this->setUnsignedInt32(kCount, (unsigned int)count);
    }
 
    const std::string& GLTFAccessor::getID() {
        return this->_ID;
    }
        
    size_t GLTFAccessor::elementByteLength() {
        return this->_elementByteLength;
    }
    
    shared_ptr<JSONArray> GLTFAccessor::min() {
        this->_computeMinMaxIfNeeded();        
        return this->createArrayIfNeeded("min");
    }

    shared_ptr<JSONArray> GLTFAccessor::max() {
        this->_computeMinMaxIfNeeded();
        return this->createArrayIfNeeded("max");
    }
    
    typedef struct {
        double *min, *max;
    } __MinMaxApplierInfo;
    
    static void __ComputeMinMax(void *value,
                                const std::string& componentType /* componentType */,
                                const std::string& type,
                                size_t componentsPerElement,
                                size_t index,
                                size_t vertexAttributeByteSize,
                                void *context) {
        __MinMaxApplierInfo *applierInfo = (__MinMaxApplierInfo*)context;
        char* bufferData = (char*)value;
        
        if (componentType == "FLOAT") {
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
    }
    
    void GLTFAccessor::_computeMinMaxIfNeeded()  {
        if (this->_minMaxDirty == true) {
            double min[32];
            double max[32];

            size_t componentsPerElement = this->_componentsPerElement;
            shared_ptr<JSONArray> minObject = this->createArrayIfNeeded("min");
            shared_ptr<JSONArray> maxObject = this->createArrayIfNeeded("max");
            
            __MinMaxApplierInfo minMaxApplierInfo;
            minMaxApplierInfo.min = min;
            minMaxApplierInfo.max = max;
            for (size_t i = 0 ; i < componentsPerElement ; i++) {
                min[i] = DBL_MAX;
                max[i] = -DBL_MAX;
            }
            
            applyOnAccessor(__ComputeMinMax, &minMaxApplierInfo);
            
            for (size_t i = 0 ; i < this->_componentsPerElement ; i++) {
                minObject->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(min[i])));
                maxObject->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(max[i])));
            }
            this->_minMaxDirty = false;
        }
    }
    
    void GLTFAccessor::exposeMinMax() {
        this->_computeMinMaxIfNeeded();
    }

    void GLTFAccessor::applyOnAccessor(GLTFAccessorApplierFunc applierFunc, void* context) {
        size_t byteStride = this->getByteStride();
        size_t componentsPerElement = this->_componentsPerElement;
        size_t vertexAttributeByteSize = this->_elementByteLength;
        shared_ptr <GLTF::GLTFBufferView> bufferView = this->getBufferView();
        unsigned char* bufferData = (unsigned char*)bufferView->getBufferDataByApplyingOffset();

        size_t count = this->getCount();
        for (size_t i = 0 ; i < count ; i++) {
            (*applierFunc)(bufferData + (i * byteStride), this->_componentType, this->type(), componentsPerElement, i, vertexAttributeByteSize, context);
        }
    }
    
    bool GLTFAccessor::matchesLayout(GLTFAccessor* meshAttribute) {
        assert(meshAttribute);
        
        return ((meshAttribute->componentsPerElement() == this->componentsPerElement()) &&
                (meshAttribute->getByteStride() == this->getByteStride()) &&
                (meshAttribute->componentType() == this->componentType()) &&
                (meshAttribute->elementByteLength() == this->elementByteLength()));
    }
    
    std::string GLTFAccessor::type() {
        return this->getString(kType);
    }

    std::string GLTFAccessor::valueType() {
        return "accessor";
    }

}
