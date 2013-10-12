// Copyright (c) 2013, Fabrice Robinet.
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

using namespace std::tr1;
using namespace std;

namespace GLTF 
{        
    GLTFOutputStream::GLTFOutputStream() {
        
    }
    
    GLTFOutputStream::GLTFOutputStream(const std::string &folder, const std::string &file, const std::string &kind) {
        this->_id = file + kind;
        this->_filename = this->_id + ".bin";
        this->_outputPath = folder + this->_filename;        
        this->_stream.open(this->_outputPath.c_str(), ios::out | ios::ate | ios::binary);
    }
    
    const std::string& GLTFOutputStream::filename() {
        return this->_filename;
    }
    
    const std::string& GLTFOutputStream::id() {
        return this->_id;
    }
    
    const char* GLTFOutputStream::outputPathCStr() {
        return this->_outputPath.c_str();
    }

    size_t GLTFOutputStream::length() {
        return static_cast<size_t> (this->_stream.tellp());
    }
    
    void GLTFOutputStream::write(const char* buffer, size_t length) {
        this->_stream.write(buffer, length);
    }
    
    void GLTFOutputStream::write(shared_ptr<GLTFBuffer> buffer) {
        this->write((const char*)(buffer->getData()), buffer->getByteLength());
    }

    void GLTFOutputStream::write(shared_ptr<GLTFBufferView> bufferView) {
        this->write((const char*)( bufferView->getBufferDataByApplyingOffset()), bufferView->getByteLength());
    }
    
    GLTFOutputStream::~GLTFOutputStream() {
        this->_stream.flush();
        this->_stream.close();
    }
}
