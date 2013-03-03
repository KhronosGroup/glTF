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
    //---- JSONPrimitiveRemapInfos -------------------------------------------------------------
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
        this->_originalCountAndIndexesSize = 0;
    }
    
    JSONPrimitive::~JSONPrimitive()
    {
        if (this->_originalCountAndIndexes) {
            free(this->_originalCountAndIndexes);
        }        
    }

    JSONExport::Semantic JSONPrimitive::getSemanticAtIndex(unsigned int index)
    {
        return this->_allIndicesInfos[index]->getSemantic();
    }
    
    unsigned int JSONPrimitive::getIndexOfSetAtIndex(unsigned int index)
    {
        return this->_allIndicesInfos[index]->getIndexOfSet();
    }

    unsigned int JSONPrimitive::getIndicesInfosCount()
    {
        return this->_allIndicesInfos.size();
    }
    
    PrimitiveIndicesInfosVector JSONPrimitive::getPrimitiveIndicesInfos()
    {
        return this->_allIndicesInfos;
    }
    
    void JSONPrimitive::appendPrimitiveIndicesInfos(shared_ptr <JSONPrimitiveIndicesInfos> primitiveIndicesInfos)
    {
        this->_allIndicesInfos.push_back(primitiveIndicesInfos);
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
    
    static AccessorsBufferInfos* createAccessorsBuffersInfos(AccessorVector allOriginalAccessors ,AccessorVector allRemappedAccessors, unsigned int*indicesInRemapping, unsigned int count)
    {
        AccessorsBufferInfos* allBufferInfos = (AccessorsBufferInfos*)malloc(count * sizeof(AccessorsBufferInfos));
        for (size_t accessorIndex = 0 ; accessorIndex < count; accessorIndex++) {
            AccessorsBufferInfos *bufferInfos = &allBufferInfos[accessorIndex];

            shared_ptr <JSONExport::JSONAccessor> remappedAccessor = allRemappedAccessors[indicesInRemapping[accessorIndex]];
            shared_ptr <JSONExport::JSONAccessor> originalAccessor = allOriginalAccessors[indicesInRemapping[accessorIndex]];

            if (originalAccessor->getElementByteLength() != remappedAccessor->getElementByteLength()) {
                // FIXME : report error
                free(allBufferInfos);
                return 0;
            }
            
            shared_ptr <JSONExport::JSONBuffer> remappedBuffer = remappedAccessor->getBuffer();
            shared_ptr <JSONExport::JSONBuffer> originalBuffer = originalAccessor->getBuffer();     
            
            bufferInfos->remappedBufferData = (unsigned char*)static_pointer_cast <JSONDataBuffer> (remappedBuffer)->getData() + remappedAccessor->getByteOffset();
            bufferInfos->remappedAccessorByteStride = remappedAccessor->getByteStride();

            bufferInfos->originalBufferData = (unsigned char*)static_pointer_cast <JSONDataBuffer> (originalBuffer)->getData() + originalAccessor->getByteOffset();
            bufferInfos->originalAccessorByteStride = originalAccessor->getByteStride();
        
            bufferInfos->elementByteLength = remappedAccessor->getElementByteLength();
        }
        
        return allBufferInfos;
    }
    
    bool JSONPrimitive::_remapVertexes(std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices,
                                       AccessorVector allOriginalAccessors,
                                       AccessorVector allRemappedAccessors,
                                       unsigned int* indicesInRemapping,
                                       shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos)
    {
        if (!this->_originalCountAndIndexes) {
            // FIXME: report error
            return false;
        }
        
        size_t indicesSize = allIndices.size();
        if (allOriginalAccessors.size() < indicesSize) {
            //TODO: assert & inconsistency check
        }
        
        unsigned int vertexAttributesCount = (unsigned int)indicesSize;
        
        //get the primitive infos to know where we need to "go" for remap 
        unsigned int count = primitiveRemapInfos->generatedIndicesCount();
        unsigned int* indices = primitiveRemapInfos->generatedIndices();

        AccessorsBufferInfos *allBufferInfos = createAccessorsBuffersInfos(allOriginalAccessors , allRemappedAccessors, indicesInRemapping, vertexAttributesCount);
        
        unsigned int* uniqueIndicesBuffer = (unsigned int*) (static_pointer_cast <JSONDataBuffer> (this->_uniqueIndices->getBuffer())->getData());
        
        
        for (unsigned int k = 0 ; k < count ; k++) { 
            unsigned int idx = indices[k];                
            unsigned int* remappedIndex = &this->_originalCountAndIndexes[idx * (allOriginalAccessors.size() + 1)];
            
            for (size_t accessorIndex = 0 ; accessorIndex < vertexAttributesCount  ; accessorIndex++) {
                AccessorsBufferInfos *bufferInfos = &allBufferInfos[accessorIndex];
                void *ptrDst = bufferInfos->remappedBufferData + (uniqueIndicesBuffer[idx] * bufferInfos->remappedAccessorByteStride);
                void *ptrSrc = (unsigned char*)bufferInfos->originalBufferData + (remappedIndex[1 /* skip count */ + indicesInRemapping[accessorIndex]] * bufferInfos->originalAccessorByteStride);
                //FIXME: optimize / secure this a bit, too many indirections without testing for invalid pointers
                /* copy the vertex attributes at the right offset and right indice (using the generated uniqueIndexes table */
                memcpy(ptrDst, ptrSrc , bufferInfos->elementByteLength);
            }
        }
        
        if (allBufferInfos)
            free(allBufferInfos);
        
        return true;        
    }
    
    shared_ptr<JSONExport::JSONPrimitiveRemapInfos> JSONPrimitive::buildUniqueIndexes(std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices,
        RemappedMeshIndexesHashmap& remappedMeshIndexesMap,
        unsigned int* indicesInRemapping,
        unsigned int startIndex,
        unsigned int accessorsCount,
        unsigned int &endIndex)
    {
        size_t allIndicesSize = allIndices.size();
        size_t vertexAttributeCount = allIndices[0]->getCount();

        unsigned int generatedIndicesCount = 0;
        unsigned int vertexAttributesCount = accessorsCount;
        size_t sizeOfRemappedIndex = (vertexAttributesCount + 1) * sizeof(unsigned int);
        
        this->_originalCountAndIndexes = (unsigned int*)calloc( (vertexAttributeCount * sizeOfRemappedIndex), sizeof(unsigned char));
        //this is useful for debugging.
        this->_originalCountAndIndexesSize = (vertexAttributeCount * sizeOfRemappedIndex);
        
        unsigned int *uniqueIndexes = (unsigned int*)calloc( vertexAttributeCount * sizeof(unsigned int), 1);
        unsigned int* generatedIndices = (unsigned int*) calloc (vertexAttributeCount * sizeof(unsigned int) , 1); //owned by PrimitiveRemapInfos
        unsigned int currentIndex = startIndex;
         
        for (size_t k = 0 ; k < vertexAttributeCount ; k++) {
            unsigned int* remappedIndex = &this->_originalCountAndIndexes[k * (vertexAttributesCount + 1)];
            
            remappedIndex[0] = vertexAttributesCount;
            for (unsigned int i = 0 ; i < allIndicesSize ; i++) {
                unsigned int idx = indicesInRemapping[i];                
                remappedIndex[1 + idx] = ((unsigned int*)(static_pointer_cast <JSONDataBuffer> (allIndices[i]->getBuffer())->getData()))[k];
            }
             
            unsigned int index = remappedMeshIndexesMap[remappedIndex];
            if (index == 0) {
                index = currentIndex++;
                generatedIndices[generatedIndicesCount++] = (unsigned int)k;
                remappedMeshIndexesMap[remappedIndex] = index;
            } 
            
            uniqueIndexes[k] = index - 1;
        }
        
        endIndex = currentIndex;
        shared_ptr <JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos(new JSONExport::JSONPrimitiveRemapInfos(generatedIndices, generatedIndicesCount));                        
        shared_ptr <JSONExport::JSONDataBuffer> indicesBuffer(new JSONExport::JSONDataBuffer(uniqueIndexes, vertexAttributeCount * sizeof(unsigned int), true));
        
        this->_uniqueIndices = shared_ptr <JSONExport::JSONIndices> (new JSONExport::JSONIndices(indicesBuffer, vertexAttributeCount, JSONExport::VERTEX, 0));
        
        return primitiveRemapInfos;
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
