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

using namespace std::tr1;
using namespace std;

namespace GLTF 
{        

    void GLTFIndices::_indicesCommonInit() 
    {
    }
    
    // this constructor is private and typically won't be called
    GLTFIndices::GLTFIndices() {}
    
    GLTFIndices::GLTFIndices(shared_ptr <GLTFBufferView> bufferView,
                             size_t count):
    _count(count),
    _byteOffset(0),
    _bufferView(bufferView)
    {
        this->_indicesCommonInit();
    }

    GLTFIndices::~GLTFIndices()
    {
    }
    
    size_t GLTFIndices::getCount()
    {
        return this->_count;
    }
                
    shared_ptr <GLTFBufferView>  const GLTFIndices::getBufferView()
    {
        return this->_bufferView;
    }
    
    void GLTFIndices::setBufferView(shared_ptr <GLTFBufferView> bufferView)
    {
        this->_bufferView = bufferView;
    }

    void GLTFIndices::setByteOffset(size_t byteOffset)
    {
        this->_byteOffset = byteOffset;
    }
    
    size_t GLTFIndices::getByteOffset()
    {
        return this->_byteOffset;
    }
            
}
