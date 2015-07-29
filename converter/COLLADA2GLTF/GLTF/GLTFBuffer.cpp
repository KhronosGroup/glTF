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
#include "GLTFAsset.h"

using namespace rapidjson;
using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{    
    GLTFBuffer::GLTFBuffer() : JSONObject() {}     //private

    GLTFBuffer::~GLTFBuffer() {
        if (this->_ownData && this->_data)
            free(this->_data);
    }    

    //FIXME:This one just should be removed, but some code depends on it.
    GLTFBuffer::GLTFBuffer(std::string ID, size_t byteLength): JSONObject(),
    _ID(ID),
    _data(0),
    _ownData(false) {
		this->setUnsignedInt32(kByteLength, (unsigned int)byteLength);
    }
    
    GLTFBuffer::GLTFBuffer(void *data, size_t byteLength, bool ownData): JSONObject(),
    _data((unsigned char*)data),
    _ownData(ownData) {
		this->setUnsignedInt32(kByteLength, (unsigned int)byteLength);
        this->_ID = GLTFUtils::generateIDForType(kBuffer.c_str());
    }
    
    GLTFBuffer::GLTFBuffer(std::string ID,void *data, size_t byteLength, bool ownData): JSONObject(),
    _ID(ID),
    _data((unsigned char*)data),
    _ownData(ownData) {
		this->setUnsignedInt32(kByteLength, (unsigned int)byteLength);
    }

    void GLTFBuffer::evaluate(void* context) {
        //FIXME:this would be better somewhere else..
        GLTFAsset* asset = (GLTFAsset*)context;
        
        this->setString(kURI, COLLADABU::URI::uriEncode(asset->resourceOuputPathForPath(this->getString(kURI))));
    }
    
    size_t GLTFBuffer::getByteLength() {
        return this->getUnsignedInt32(kByteLength);
    }
        
    std::string const GLTFBuffer::getID() {
        return this->_ID;
    }
    
    const void* GLTFBuffer::getData() {
        return this->_data;
    }
    
    std::string GLTFBuffer::valueType() {
        return "buffer";
    }

    //--- GLTFBufferView
    
    GLTFBufferView::~GLTFBufferView() {
    }
    
    void GLTFBufferView::_setBuffer(shared_ptr <GLTFBuffer> buffer) {
        this->_buffer = buffer;
        this->setString(kBuffer, buffer->getID());
    }
    
    GLTFBufferView::GLTFBufferView(shared_ptr <GLTF::GLTFBuffer> buffer, size_t byteOffset, size_t byteLength) : JSONObject() {
        this->_ID = GLTFUtils::generateIDForType(kBufferView.c_str());
        
        this->_setBuffer(buffer);
        this->setByteLength(byteLength);
        this->setByteOffset(byteOffset);
    }
    
    GLTFBufferView::GLTFBufferView(std::string ID, shared_ptr <GLTF::GLTFBuffer> buffer, size_t byteOffset, size_t byteLength)
        : JSONObject(), _ID(ID) {
        this->_setBuffer(buffer);
        this->setByteLength(byteLength);
        this->setByteOffset(byteOffset);
    }
        
    GLTFBufferView::GLTFBufferView() {
        this->_ID = GLTFUtils::generateIDForType(kBufferView.c_str());
        this->setByteLength(0);
        this->setByteOffset(0);
    }
    
    void GLTFBufferView::setByteLength(size_t byteLength) {
		this->setUnsignedInt32(kByteLength, (unsigned int)byteLength);
    }
    
    size_t GLTFBufferView::getByteLength() {
        return this->getUnsignedInt32(kByteLength);
    }
    
    void GLTFBufferView::setByteOffset(size_t byteOffset) {
		this->setUnsignedInt32(kByteOffset, (unsigned int)byteOffset);
    }
    
    size_t GLTFBufferView::getByteOffset() {
        return this->getUnsignedInt32(kByteOffset);
    }

    std::string const GLTFBufferView::getID() {
        return this->_ID;
    }
    
    shared_ptr <GLTFBuffer> GLTFBufferView::getBuffer() {
        return this->_buffer;
    }
    
    void* GLTFBufferView::getBufferDataByApplyingOffset() {
        unsigned char *data = (unsigned char*)this->_buffer->getData();
        return (void*)(data + this->getByteOffset());
    }

    shared_ptr <GLTFBufferView>  createBufferViewWithAllocatedBuffer(std::string ID, void *data, size_t byteOffset, size_t byteLength, bool ownData) {
        shared_ptr<GLTFBuffer> buffer(new GLTFBuffer(data, byteLength, ownData));
        shared_ptr<GLTFBufferView> bufferView(new GLTFBufferView(ID, buffer, byteOffset, byteLength));

        return bufferView;
    }
    
    shared_ptr <GLTFBufferView>  createBufferViewWithAllocatedBuffer(void *data, size_t byteOffset, size_t byteLength, bool ownData) {
        shared_ptr<GLTFBuffer> buffer(new GLTFBuffer(data, byteLength, ownData));
        shared_ptr<GLTFBufferView> bufferView(new GLTFBufferView(buffer, byteOffset, byteLength));
        
        return bufferView;
    }
    
    std::string GLTFBufferView::valueType() {
        return "bufferView";
    }

}
