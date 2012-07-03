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

#ifndef __JSON_EXPORT_TYPES_AND_CONSTANTS_H__
#define __JSON_EXPORT_TYPES_AND_CONSTANTS_H__

#define EXPORT_MATERIALS_AS_EFFECTS 1

// FIXME: check if this is required for osx only, looks like the standard on other platform would be just std::
using namespace std::tr1;

namespace JSONExport 
{
    std::string generateIDForType( const char* typeCStr, const char* suffix = 0);
    
    // FIXME: put better comment here
    //RemappedMeshIndexes[0] = count ;  RemappedMeshIndexes[1+] original indexes (used for hash code)
    struct RemappedMeshIndexesHash {
        inline size_t operator()(unsigned int* remappedMeshIndexes) const
        {
            size_t hash = 0;
            size_t count = (size_t)remappedMeshIndexes[0];
            
            for (int i = 0 ; i < count ; i++) {
                hash += (size_t)remappedMeshIndexes[i + 1 /* skip count */];
            }
            
            return hash;
        }
    };
    
    struct RemappedMeshIndexesEq {
        inline bool operator()(unsigned int* k1, unsigned int* k2) const {
            
            size_t count = (size_t)k1[0];
            
            if (count != (size_t)k2[0])
                return false;
            
            for (int i = 0 ; i < count ; i++) {
                if (k1[i + 1] != k2[i + 1])
                    return false;
            }
            
            return true;
        }
    };
        
    enum Semantic {
        VERTEX = 1,
        NORMAL = 2,
        TEXCOORD = 3,
        COLOR = 4
    };    
/*
    enum JSONNumberType {
        UNSIGNED_INT,
        INT,
        DOUBLE
    };
  */  
    enum JSONValueType {
        NOT_A_JSON_TYPE = 0,
        NUMBER = 1,
        OBJECT = 2,
        ARRAY = 3,
        STRING = 4,
        
        ACCESSOR = 5,
        BUFFER = 6,
        EFFECT = 7,
        INDICES = 8,
        MESH = 9,
        PRIMITIVE = 10
    };
    
    enum ElementType {
        NOT_AN_ELEMENT_TYPE = 0,
        BYTE = 1,
        UNSIGNED_BYTE = 2,
        SHORT = 3,
        UNSIGNED_SHORT = 4,
        FIXED = 5,
        FLOAT = 6
    };
    
};

#endif
