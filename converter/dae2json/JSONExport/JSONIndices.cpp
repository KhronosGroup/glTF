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

    void JSONIndices::_indicesCommonInit() 
    {
    }
    
    // this constructor is private and typically won't be called
    JSONIndices::JSONIndices() {}
    
    JSONIndices::JSONIndices(shared_ptr <JSONExport::JSONBuffer> buffer, size_t count, JSONExport::Semantic semantic, unsigned int indexOfSet):
    _accessorID(""),
    _count(count),
    _semantic(semantic),
    _indexOfSet(indexOfSet),
    _buffer(buffer),
    _byteOffset(0)
    {
        this->_indicesCommonInit();
    }

    JSONIndices::JSONIndices(std::string accessorID, shared_ptr <JSONExport::JSONBuffer> buffer, size_t count, JSONExport::Semantic semantic, unsigned int indexOfSet):
    _accessorID(""),
    _count(count),
    _semantic(semantic),
    _indexOfSet(indexOfSet),
    _buffer(buffer),
    _byteOffset(0)
    {
        this->_indicesCommonInit();
    }

    JSONIndices::~JSONIndices()
    {
    }
    
    size_t const JSONIndices::getCount()
    {
        return this->_count;
    }
    
    JSONExport::Semantic const JSONIndices::getSemantic()
    {
        return this->_semantic;
    }
    
    unsigned int const JSONIndices::getIndexOfSet()
    {
        return this->_indexOfSet;
    }
        
    const std::string const JSONIndices::getAccessorID()
    {
        return this->_accessorID;
    }
    
    const shared_ptr <JSONExport::JSONBuffer>  const JSONIndices::getBuffer()
    {
        return this->_buffer;
    }
    
    const void JSONIndices::setBuffer(shared_ptr <JSONBuffer> buffer)
    {
        this->_buffer = buffer;
    }

    void JSONIndices::setByteOffset(size_t byteOffset)
    {
        this->_byteOffset = byteOffset;
    }
    
    size_t JSONIndices::getByteOffset()
    {
        return this->_byteOffset;
    }
            
}
