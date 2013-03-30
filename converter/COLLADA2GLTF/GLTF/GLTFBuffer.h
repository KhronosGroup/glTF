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

#ifndef __JSON_BUFFER_H__
#define __JSON_BUFFER_H__

/* 
    A GLTFBuffer holds raw data to be exported as an ArrayBuffer i.e http://www.khronos.org/registry/typedarray/specs/latest/
    GLTFBuffer can take ownership of the data passed, in that case when a GLTFBuffer instance then the data is freed.
 */

namespace GLTF 
{
    class GLTFBuffer {
    private:
        GLTFBuffer();
    public:
        GLTFBuffer(std::string ID, size_t byteLength);
        GLTFBuffer(std::string ID, void *data, size_t byteLength, bool ownData);
        GLTFBuffer(void *data, size_t byteLength, bool ownData);
        
        virtual ~GLTFBuffer();
        
        size_t getByteLength();
        std::string const getID();
        const void* getData();
        
    private:
        std::string  _ID;
        size_t _byteLength;
        unsigned char* _data;
        bool _ownData;
    };
    
    class GLTFBufferView {
    public:
        GLTFBufferView();
        GLTFBufferView(shared_ptr <GLTF::GLTFBuffer> buffer, size_t byteOffset, size_t byteLength);
        GLTFBufferView(std::string ID, shared_ptr <GLTF::GLTFBuffer> buffer, size_t byteOffset, size_t byteLength);
        
        virtual ~GLTFBufferView();
        
        void setByteLength(size_t byteLength);
        size_t getByteLength();
        
        void setByteOffset(size_t byteOffset);
        size_t getByteOffset();
        
        shared_ptr <GLTFBuffer> getBuffer();

        std::string const getID();
    
        void* getBufferDataByApplyingOffset();

    private:
        std::string  _ID;
        size_t _byteLength;
        size_t _byteOffset;
        shared_ptr<GLTFBuffer> _buffer;
    };
    
    //conveniences functions
    shared_ptr <GLTFBufferView>  createBufferViewWithAllocatedBuffer(std::string ID, void *data, size_t byteOffset, size_t byteLength, bool ownData);
    shared_ptr <GLTFBufferView>  createBufferViewWithAllocatedBuffer(void *data, size_t byteOffset, size_t byteLength, bool ownData);

}

#endif