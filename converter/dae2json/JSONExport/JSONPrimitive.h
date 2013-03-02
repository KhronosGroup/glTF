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
    
    typedef unordered_map<unsigned int* ,unsigned int /* index of existing n-uplet of indices */, RemappedMeshIndexesHash, RemappedMeshIndexesEq> RemappedMeshIndexesHashmap;       
    typedef std::vector< shared_ptr<JSONExport::JSONAccessor> > AccessorVector;
    
    //FIXME: this could be just an intermediate anonymous(no id) JSONBuffer
    class JSONPrimitiveRemapInfos
    {
    public:
        JSONPrimitiveRemapInfos(unsigned int* generatedIndices, unsigned int generatedIndicesCount);
        virtual ~JSONPrimitiveRemapInfos();
        
        unsigned int generatedIndicesCount();
        unsigned int* generatedIndices();
        
    private:
        unsigned int _generatedIndicesCount;
        unsigned int* _generatedIndices;
    };
    
    class JSONPrimitive
    {
    public:
        // FIXME: make protected and mesh a friend
        bool _remapVertexes(std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices,
                            AccessorVector allOriginalAccessors ,
                            AccessorVector allRemappedAccessors,
                            unsigned int* indicesInRemapping,
                            shared_ptr<JSONPrimitiveRemapInfos> primitiveRemapInfos);
    public:
        JSONPrimitive();
        virtual ~JSONPrimitive();
        
        void appendIndices(shared_ptr <JSONExport::JSONIndices> indices);

        shared_ptr<JSONPrimitiveRemapInfos> buildUniqueIndexes(std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices,
            RemappedMeshIndexesHashmap& remappedMeshIndexesMap,
            unsigned int* indicesInRemapping,
            unsigned int startIndex,
            unsigned int accessorsCount,
            unsigned int &endIndex);
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
        
    private:                
        std::string _type;
        std::string _materialID;
        unsigned int _materialObjectID;
        unsigned int* _originalCountAndIndexes;
        shared_ptr <JSONExport::JSONIndices> _uniqueIndices;
        unsigned long _originalCountAndIndexesSize;
        std::vector <shared_ptr<JSONExport::JSONPrimitiveIndicesInfos> >_allIndicesInfos;
    };

}


#endif