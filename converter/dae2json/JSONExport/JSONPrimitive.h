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

#ifndef __JSON_PRIMITIVE_H__
#define __JSON_PRIMITIVE_H__

namespace JSONExport 
{
    class JSONPrimitiveIndicesInfos;
    class JSONMesh;
    
    
    //---- JSONPrimitiveIndicesInfos -------------------------------------------------------------
    class JSONPrimitiveIndicesInfos
    {
    public:
        JSONPrimitiveIndicesInfos(JSONExport::Semantic semantic, unsigned int indexOfSet):
        _semantic(semantic),
        _indexOfSet(indexOfSet) {
        }
        JSONExport::Semantic getSemantic() {
            return _semantic;
        }
        unsigned int const getIndexOfSet() {
            return _indexOfSet;
        }
    private:
        JSONExport::Semantic _semantic;
        unsigned int _indexOfSet;
    };
        
    class JSONPrimitive
    {
    public:
        // FIXME: make protected and mesh a friend
    public:
        JSONPrimitive();
        virtual ~JSONPrimitive();
        
        shared_ptr <JSONExport::JSONIndices> getUniqueIndices();
        
        std::string getType();
        void setType(std::string type);

        std::string getMaterialID();
        void setMaterialID(std::string materialID);
        
        unsigned int getMaterialObjectID();
        void setMaterialObjectID(unsigned int materialID);

        JSONExport::Semantic getSemanticAtIndex(unsigned int index);
        unsigned int getIndexOfSetAtIndex(unsigned int index);
        unsigned int getIndicesInfosCount();
        
        PrimitiveIndicesInfosVector getPrimitiveIndicesInfos();
        void appendPrimitiveIndicesInfos(shared_ptr <JSONPrimitiveIndicesInfos> primitiveIndicesInfos);
        
        shared_ptr <JSONExport::JSONIndices>  getIndices();
        void setIndices(shared_ptr <JSONExport::JSONIndices> indices);
        
    private:                
        std::string _type;
        std::string _materialID;
        unsigned int _materialObjectID;
        unsigned int* _originalCountAndIndexes;
        shared_ptr <JSONExport::JSONIndices> _uniqueIndices;
        unsigned long _originalCountAndIndexesSize;
        PrimitiveIndicesInfosVector _allIndicesInfos;
    };

}


#endif