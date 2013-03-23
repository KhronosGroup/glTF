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

namespace GLTF 
{    
    GLTFBuffer::GLTFBuffer() {}     //private

    GLTFBuffer::~GLTFBuffer() {
        if (this->_ownData && this->_data)
            free(this->_data);
    }    

    //FIXME:This one just should be removed, but some code depends on it.
    GLTFBuffer::GLTFBuffer(std::string ID, size_t byteLength):
    _ID(ID),
    _byteLength(byteLength),
    _data(0),
    _ownData(false)
    {
    }
    
    GLTFBuffer::GLTFBuffer(void *data, size_t byteLength, bool ownData):
    _byteLength(byteLength),
    _data((unsigned char*)data),
    _ownData(ownData)
    {
        this->_ID = GLTFUtils::generateIDForType("buffer");
    }
    
    GLTFBuffer::GLTFBuffer(std::string ID,void *data, size_t byteLength, bool ownData):
    _ID(ID),
    _byteLength(byteLength),
    _data((unsigned char*)data),
    _ownData(ownData)
    {
    }

    size_t const GLTFBuffer::getByteLength()
    {
        return this->_byteLength;
    }
        
    std::string const GLTFBuffer::getID()
    {
        return this->_ID;
    }
    
    const void* const GLTFBuffer::getData()
    {
        return this->_data;
    }
    
    //--- GLTFBufferView    
    
    GLTFBufferView::~GLTFBufferView()
    {
    }
    
    GLTFBufferView::GLTFBufferView(shared_ptr <GLTF::GLTFBuffer> buffer, size_t byteOffset, size_t byteLength)
    {
        this->_ID = GLTFUtils::generateIDForType("bufferView");
        this->_buffer = buffer;
        this->_byteLength = byteLength;
        this->_byteOffset = byteOffset;
    }
    
    GLTFBufferView::GLTFBufferView(std::string ID, shared_ptr <GLTF::GLTFBuffer> buffer, size_t byteOffset, size_t byteLength)
    {
        this->_buffer = buffer;
        this->_byteLength = byteLength;
        this->_byteOffset = byteOffset;
    }
        
    GLTFBufferView::GLTFBufferView()
    {
        this->_ID = GLTFUtils::generateIDForType("bufferView");
        this->_byteLength = 0;
        this->_byteOffset = 0;
    }
    
    void const GLTFBufferView::setByteLength(size_t byteLength)
    {
        this->_byteLength = byteLength;
    }
    
    size_t const GLTFBufferView::getByteLength()
    {
        return this->_byteLength;
    }
    
    void const GLTFBufferView::setByteOffset(size_t byteOffset)
    {
        this->_byteOffset = byteOffset;
    }
    
    size_t const GLTFBufferView::getByteOffset()
    {
        return this->_byteOffset;
    }

    std::string const GLTFBufferView::getID()
    {
        return this->_ID;
    }
    
    shared_ptr <GLTFBuffer> GLTFBufferView::getBuffer()
    {
        return this->_buffer;
    }
    
    void* GLTFBufferView::getBufferDataByApplyingOffset()
    {
        unsigned char *data = (unsigned char*)this->_buffer->getData();
        
        return (void*)(data + this->getByteOffset());
    }

    shared_ptr <GLTFBufferView>  createBufferViewWithAllocatedBuffer(std::string ID, void *data, size_t byteOffset, size_t byteLength, bool ownData)
    {
        shared_ptr<GLTFBuffer> buffer(new GLTFBuffer(data, byteLength, ownData));
        shared_ptr<GLTFBufferView> bufferView(new GLTFBufferView(ID, buffer, byteOffset, byteLength));

        return bufferView;
    }
    
    shared_ptr <GLTFBufferView>  createBufferViewWithAllocatedBuffer(void *data, size_t byteOffset, size_t byteLength, bool ownData)
    {
        shared_ptr<GLTFBuffer> buffer(new GLTFBuffer(data, byteLength, ownData));
        shared_ptr<GLTFBufferView> bufferView(new GLTFBufferView(buffer, byteOffset, byteLength));
        
        return bufferView;
    }
    
}
