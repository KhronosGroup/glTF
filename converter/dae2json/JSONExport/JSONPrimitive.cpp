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

#include "JSONExport.h"

using namespace rapidjson;

namespace JSONExport 
{
    //---- JSONPrimitiveRemapInfos ------------------------------------------------------------------------------------------
    JSONPrimitiveRemapInfos::JSONPrimitiveRemapInfos(unsigned int* generatedIndices, unsigned int generatedIndicesCount):
    _generatedIndicesCount(generatedIndicesCount),
    _generatedIndices(generatedIndices)
    {
    }
    
    JSONPrimitiveRemapInfos::~JSONPrimitiveRemapInfos()
    {
        if (this->_generatedIndices)
            free(this->_generatedIndices);
    }
    
    unsigned int JSONPrimitiveRemapInfos::generatedIndicesCount()
    {
        return _generatedIndicesCount;
    }
    
    unsigned int* JSONPrimitiveRemapInfos::generatedIndices()
    {
        return _generatedIndices;
    }
    
    //---- JSONPrimitiveRemapInfos ------------------------------------------------------------------------------------------

    JSONPrimitive::JSONPrimitive()
    {
        this->_originalCountAndIndexes = 0;
    }
    
    JSONPrimitive::~JSONPrimitive()
    {
        if (this->_originalCountAndIndexes) {
            free(this->_originalCountAndIndexes);
        }        
    }
    
    typedef struct {
        unsigned char* remappedBufferData;
        //size_t remappedAccessorByteOffset;
        size_t remappedAccessorByteStride;
        
        unsigned char* originalBufferData;
        //size_t originalAccessorByteOffset;
        size_t originalAccessorByteStride;
        
        size_t elementByteLength;
        
    } AccessorsBufferInfos;
    
    static AccessorsBufferInfos* createAccessorsBuffersInfos(AccessorVector allOriginalAccessors ,AccessorVector allRemappedAccessors)
    {
        size_t count = allOriginalAccessors.size();
        AccessorsBufferInfos* allBufferInfos = (AccessorsBufferInfos*)malloc(count * sizeof(AccessorsBufferInfos));
        for (size_t accessorIndex = 0 ; accessorIndex < count; accessorIndex++) {
            AccessorsBufferInfos *bufferInfos = &allBufferInfos[accessorIndex];

            shared_ptr <JSONExport::JSONAccessor> remappedAccessor = allRemappedAccessors[accessorIndex];
            shared_ptr <JSONExport::JSONAccessor> originalAccessor = allOriginalAccessors[accessorIndex];

            if (originalAccessor->getElementByteLength() != remappedAccessor->getElementByteLength()) {
                // FIXME : report error
                free(allBufferInfos);
                return 0;
            }
            
            shared_ptr <JSONExport::JSONBuffer> remappedBuffer = remappedAccessor->getBuffer();
            shared_ptr <JSONExport::JSONBuffer> originalBuffer = originalAccessor->getBuffer();     
            
            bufferInfos->remappedBufferData = (unsigned char*)static_pointer_cast <JSONDataBuffer> (remappedBuffer)->getData() + remappedAccessor->getByteOffset();
            //bufferInfos->remappedAccessorByteOffset = remappedAccessor->getByteOffset();
            bufferInfos->remappedAccessorByteStride = remappedAccessor->getByteStride();

            bufferInfos->originalBufferData = (unsigned char*)static_pointer_cast <JSONDataBuffer> (originalBuffer)->getData() + originalAccessor->getByteOffset();
            //bufferInfos->originalAccessorByteOffset = originalAccessor->getByteOffset();
            bufferInfos->originalAccessorByteStride = originalAccessor->getByteStride();
        
            bufferInfos->elementByteLength = remappedAccessor->getElementByteLength();
        }
        
        return allBufferInfos;
    }
    
    bool JSONPrimitive::_remapVertexes(AccessorVector allOriginalAccessors ,AccessorVector allRemappedAccessors, shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos)
    {
        if (!this->_originalCountAndIndexes) {
            // FIXME: report error
            return false;
        }
        
        //we reserve vertexCount * slices of [_allIndices.size() + 1] (for count and remappedIndex)
        unsigned int vertexAttributesCount = (unsigned int)this->_allIndices.size();
        
        //get the primitive infos to know where we need to "go" for remap 
        unsigned int count = primitiveRemapInfos->generatedIndicesCount();
        unsigned int* indices = primitiveRemapInfos->generatedIndices();

        AccessorsBufferInfos *allBufferInfos = createAccessorsBuffersInfos(allOriginalAccessors , allRemappedAccessors);
        
        unsigned int* uniqueIndicesBuffer = (unsigned int*) (static_pointer_cast <JSONDataBuffer> (this->_uniqueIndices->getBuffer())->getData());
        
        for (unsigned int k = 0 ; k < count ; k++) { 
            unsigned int idx = indices[k];                
            unsigned int* remappedIndex = &this->_originalCountAndIndexes[idx * (vertexAttributesCount + 1)];
            
            for (size_t accessorIndex = 0 ; accessorIndex < allOriginalAccessors.size(); accessorIndex++) {
                AccessorsBufferInfos *bufferInfos = &allBufferInfos[accessorIndex];
                                
                //uniqueIndexes
                memcpy(/* copy the vertex attributes at the right offset and right indice (using the generated uniqueIndexes table */
                       
                       //FIXME: optimize / secure this a bit, too many indirections without testing for invalid pointers
                       bufferInfos->remappedBufferData /*+ bufferInfos->remappedAccessorByteOffset */+ (uniqueIndicesBuffer[idx] * bufferInfos->remappedAccessorByteStride), 
                        (unsigned char*)bufferInfos->originalBufferData /*+ bufferInfos->originalAccessorByteOffset */+ (remappedIndex[1 /* skip count */ + accessorIndex] * bufferInfos->originalAccessorByteStride),
                       bufferInfos->elementByteLength);
            }
        }
        
        if (allBufferInfos)
            free(allBufferInfos);
        
        return true;        
    }
    
    shared_ptr<JSONExport::JSONPrimitiveRemapInfos> JSONPrimitive::buildUniqueIndexes(RemappedMeshIndexesHashmap& remappedMeshIndexesMap, unsigned int* indicesInRemapping, unsigned int startIndex, unsigned int &endIndex)
    {
        //get indices[0] presumably the VERTEX
        size_t indicesCount = this->indicesCount();
        
        //we reserve vertexCount * slices of [_allIndices.size() + 1] (for count)
        unsigned int generatedIndicesCount = 0;
        unsigned int vertexAttributesCount = (unsigned int)_allIndices.size();
        size_t sizeOfRemappedIndex = (vertexAttributesCount + 1) * sizeof(unsigned int);
        this->_originalCountAndIndexes = (unsigned int*)calloc( (indicesCount * sizeOfRemappedIndex), sizeof(unsigned char));
        
        unsigned int *uniqueIndexes = (unsigned int*)malloc( indicesCount * sizeof(unsigned int));
        unsigned int* generatedIndices = (unsigned int*) malloc (indicesCount * sizeof(unsigned int)); //owned by PrimitiveRemapInfos
        unsigned int currentIndex = startIndex;
        
        for (size_t k = 0 ; k < indicesCount ; k++) {
            unsigned int* remappedIndex = &this->_originalCountAndIndexes[k * (vertexAttributesCount + 1)];
            
            remappedIndex[0] = vertexAttributesCount;
            for (unsigned int i = 0 ; i < vertexAttributesCount ; i++) {
                
                remappedIndex[1 + indicesInRemapping[i]] = ((unsigned int*)(static_pointer_cast <JSONDataBuffer> (_allIndices[i]->getBuffer())->getData()))[k];
            }
            
            unsigned int index = remappedMeshIndexesMap[remappedIndex];
            if (index == 0) {
                index = currentIndex++;
                generatedIndices[generatedIndicesCount++] = k;
                remappedMeshIndexesMap[remappedIndex] = index;
            } 
            
            uniqueIndexes[k] = index - 1;
        }
        
        endIndex = currentIndex;
        shared_ptr <JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos(new JSONExport::JSONPrimitiveRemapInfos(generatedIndices, generatedIndicesCount));                        
        shared_ptr <JSONExport::JSONDataBuffer> indicesBuffer(new JSONExport::JSONDataBuffer(uniqueIndexes, indicesCount * sizeof(unsigned int), true));
        
        this->_uniqueIndices = shared_ptr <JSONExport::JSONIndices> (new JSONExport::JSONIndices(indicesBuffer, indicesCount, JSONExport::VERTEX, 0));
        
        return primitiveRemapInfos;
    }
    
    void JSONPrimitive::appendIndices(shared_ptr <JSONExport::JSONIndices> indices)
    {
        this->_allIndices.push_back(indices);
    }
    
    std::vector< shared_ptr<JSONExport::JSONIndices> > JSONPrimitive::allIndices()
    {
        return this->_allIndices;
    }
    
    size_t JSONPrimitive::indicesCount()
    {
        //JSONExport::Semantic semantic = this->_allIndices[0]->getSemantic();
        //TODO: assert semantic == VERTEX
        return this->_allIndices[0].get()->getCount();
    }
    
    shared_ptr <JSONExport::JSONIndices> JSONPrimitive::getUniqueIndices()
    {
        return this->_uniqueIndices;
    }
    
    std::string JSONPrimitive::getType()
    {
        return _type;
    }
    
    void JSONPrimitive::setType(std::string type)
    {
        this->_type = type;
    }
    
    std::string JSONPrimitive::getMaterialID()
    {
        return this->_materialID;
    }
    
    void JSONPrimitive::setMaterialID(std::string materialID)
    {
        this->_materialID = materialID;
    }
    
    unsigned int JSONPrimitive::getMaterialObjectID()
    {
        return this->_materialObjectID;
    }
    
    void JSONPrimitive::setMaterialObjectID(unsigned int materialID)
    {
        this->_materialObjectID = materialID;
    }

};
