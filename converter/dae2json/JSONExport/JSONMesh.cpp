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
using namespace std::tr1;
using namespace std;

namespace JSONExport
{
    static std::string keyWithSemanticAndSet(JSONExport::Semantic semantic, unsigned int indexSet)
    {
        std::string semanticIndexSetKey = "";
        semanticIndexSetKey += "semantic";
        semanticIndexSetKey += JSONUtils::toString(semantic);
        semanticIndexSetKey += ":indexSet";
        semanticIndexSetKey += JSONUtils::toString(indexSet);
        
        return semanticIndexSetKey;
    }

    JSONMesh* CreateUnifiedIndexesMeshFromMesh(JSONMesh *sourceMesh, std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector)
    {
        AccessorVector originalAccessors;
        AccessorVector remappedAccessors;
        JSONMesh *targetMesh = new JSONMesh();
        
        PrimitiveVector sourcePrimitives = sourceMesh->getPrimitives();
        PrimitiveVector targetPrimitives = targetMesh->getPrimitives();
        
        size_t startIndex = 1; // begin at 1 because the hashtable will return 0 when the element is not present
        unsigned endIndex = 0;
        size_t primitiveCount = sourcePrimitives.size();
        unsigned int maxVertexAttributes = 0;
        
        if (primitiveCount == 0) {
            // FIXME: report error
            return false;
        }
        
        //in originalAccessors we'll get the flattened list of all the accessors as a vector.
        //fill semanticAndSetToIndex with key: (semantic, indexSet) value: index in originalAccessors vector.
        vector <JSONExport::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = sourceMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> selectedAccessor = (*accessorIterator).second;
                unsigned int indexSet = (*accessorIterator).first;
                JSONExport::Semantic semantic = allSemantics[i];
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int size = (unsigned int)originalAccessors.size();
                semanticAndSetToIndex[semanticIndexSetKey] = size;
                
                originalAccessors.push_back(selectedAccessor);
            }
        }
        
        maxVertexAttributes = (unsigned int)originalAccessors.size();
        
        vector <shared_ptr<JSONExport::JSONPrimitiveRemapInfos> > allPrimitiveRemapInfos;
        
        //build a array that maps the accessors that the indices points to with the index of the indice.
        JSONExport::RemappedMeshIndexesHashmap remappedMeshIndexesMap;
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)malloc(sizeof(unsigned int) * allIndices->size());
            
            for (unsigned int k = 0 ; k < allIndices->size() ; k++) {
                JSONExport::Semantic semantic = (*allIndices)[k]->getSemantic();
                unsigned int indexSet = (*allIndices)[k]->getIndexOfSet();
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos = targetPrimitives[i]->buildUniqueIndexes(*allIndices, remappedMeshIndexesMap, indicesInRemapping, startIndex, maxVertexAttributes, endIndex);
            
            free(indicesInRemapping);
            
            if (primitiveRemapInfos.get()) {
                startIndex = endIndex;
                allPrimitiveRemapInfos.push_back(primitiveRemapInfos);
            } else {
                // FIXME: report error
                return false;
            }
        }
        
        // we are using WebGL for rendering, this involve OpenGL/ES where only float are supported.
        // now we got not only the uniqueIndexes but also the number of different indexes, i.e the number of vertex attributes count
        // we can allocate the buffer to hold vertex attributes
        unsigned int vertexCount = endIndex - 1;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = sourceMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap& destinationIndexSetToAccessor = targetMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            
            //FIXME: consider turn this search into a method for mesh
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> selectedAccessor = (*accessorIterator).second;
                
                size_t sourceSize = vertexCount * selectedAccessor->getElementByteLength();
                void* sourceData = malloc(sourceSize);
                
                shared_ptr <JSONExport::JSONBuffer> referenceBuffer = selectedAccessor->getBuffer();
                shared_ptr <JSONExport::JSONDataBuffer> remappedBuffer(new JSONExport::JSONDataBuffer(referenceBuffer->getID(), sourceData, sourceSize, true));
                shared_ptr <JSONExport::JSONAccessor> remappedAccessor(new JSONExport::JSONAccessor(selectedAccessor.get()));
                remappedAccessor->setBuffer(remappedBuffer);
                remappedAccessor->setCount(vertexCount);
                
                destinationIndexSetToAccessor[(*accessorIterator).first] = remappedAccessor;
                
                remappedAccessors.push_back(remappedAccessor);
            }
        }
        
        /*
         if (_allOriginalAccessors.size() != allIndices.size()) {
         // FIXME: report error
         return false;
         }
         */
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)calloc(sizeof(unsigned int) * (*allIndices).size(), 1);
            
            for (unsigned int k = 0 ; k < (*allIndices).size() ; k++) {
                JSONExport::Semantic semantic = (*allIndices)[k]->getSemantic();
                unsigned int indexSet = (*allIndices)[k]->getIndexOfSet();
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            bool status = targetPrimitives[i]->_remapVertexes((*allIndices),
                                                         originalAccessors ,
                                                         remappedAccessors,
                                                         indicesInRemapping,
                                                         allPrimitiveRemapInfos[i]);
            free(indicesInRemapping);
            
            if (!status) {
                // FIXME: report error
                return false;
            }
            
        }
        
        if (endIndex > 65535) {
            //The mesh should be split but we do not handle this feature yet
            printf("WARNING: unsupported (yet) feature - mesh has more than 65535 vertex, splitting has to be done for GL/ES \n");
            delete targetMesh;
            return NULL; //for now return false as splitting is not implemented
        } 
        
        return targetMesh;
        
    
    }
    
    JSONMesh::JSONMesh()
    {
    }
    
    JSONMesh::~JSONMesh()
    {
    }

    vector <shared_ptr< JSONExport::JSONAccessor> > JSONMesh::remappedAccessors()
    {
        return _allRemappedAccessors;
    }
    
    bool JSONMesh::appendPrimitive(shared_ptr <JSONExport::JSONPrimitive> primitive)
    {
        this->_primitives.push_back(primitive);
        return true;
    }
    
    void JSONMesh::setAccessorsForSemantic(JSONExport::Semantic semantic, IndexSetToAccessorHashmap& indexSetToAccessorHashmap)
    {
        this->_semanticToAccessors[semantic] = indexSetToAccessorHashmap;
    }
    
    IndexSetToAccessorHashmap& JSONMesh::getAccessorsForSemantic(Semantic semantic)
    {
        return this->_semanticToAccessors[semantic];
    }
    
    vector <JSONExport::Semantic> JSONMesh::allSemantics()
    {
        vector <JSONExport::Semantic> allSemantics;
        
        SemanticToAccessorHashmap::const_iterator accessorIterator;
        
        //FIXME: consider turn this search into a method for mesh
        for (accessorIterator = this->_semanticToAccessors.begin() ; accessorIterator != this->_semanticToAccessors.end() ; accessorIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            allSemantics.push_back((*accessorIterator).first);
        }
        
        return allSemantics;
    }
    
    std::string JSONMesh::getID()
    {
        return _ID;
    }
    
    void JSONMesh::setID(std::string ID)
    {
        this->_ID = ID;
    }
    
    std::string JSONMesh::getName()
    {        
        return _name;
    }

    void JSONMesh::setName(std::string name)
    {
        this->_name = name;
    }
    
    bool JSONMesh::buildUniqueIndexes(std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector)
    {
        unsigned int startIndex = 1; // begin at 1 because the hashtable will return 0 when the element is not present
        unsigned int endIndex = 0;
        unsigned int primitiveCount = (unsigned int)_primitives.size();
        unsigned int maxVertexAttributes = 0;
        
        if (primitiveCount == 0) {
            // FIXME: report error
            return false;
        }
        
        //in _allOriginalAccessors we'll get the flattened list of all the accessors as a vector.
        //fill semanticAndSetToIndex with key: (semantic, indexSet) value: index in _allOriginalAccessors vector.
        _allOriginalAccessors.clear();
        vector <JSONExport::Semantic> allSemantics = this->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;

        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = this->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> selectedAccessor = (*accessorIterator).second;
                unsigned int indexSet = (*accessorIterator).first;
                JSONExport::Semantic semantic = allSemantics[i];
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);   
                unsigned int size = (unsigned int)_allOriginalAccessors.size();
                semanticAndSetToIndex[semanticIndexSetKey] = size;   
                
                _allOriginalAccessors.push_back(selectedAccessor);
            }
        }
        
        maxVertexAttributes = (unsigned int)_allOriginalAccessors.size();
        
        vector <shared_ptr<JSONExport::JSONPrimitiveRemapInfos> > allPrimitiveRemapInfos;
        
        //build a array that maps the accessors that the indices points to with the index of the indice.
        
        JSONExport::RemappedMeshIndexesHashmap remappedMeshIndexesMap;
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)malloc(sizeof(unsigned int) * allIndices->size());
            
            for (unsigned int k = 0 ; k < allIndices->size() ; k++) {
                JSONExport::Semantic semantic = (*allIndices)[k]->getSemantic();
                unsigned int indexSet = (*allIndices)[k]->getIndexOfSet();
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos = this->_primitives[i]->buildUniqueIndexes(*allIndices, remappedMeshIndexesMap, indicesInRemapping, startIndex, maxVertexAttributes, endIndex);
            
            free(indicesInRemapping);
            
            if (primitiveRemapInfos.get()) {
                startIndex = endIndex;
                allPrimitiveRemapInfos.push_back(primitiveRemapInfos);
            } else {
                // FIXME: report error
                return false;
            }
        }
        
        // we are using WebGL for rendering, this involve OpenGL/ES where only float are supported.
        // now we got not only the uniqueIndexes but also the number of different indexes, i.e the number of vertex attributes count
        // we can allocate the buffer to hold vertex attributes
        unsigned int vertexCount = endIndex - 1;

        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = this->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;

            //FIXME: consider turn this search into a method for mesh
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> selectedAccessor = (*accessorIterator).second;
                
                size_t sourceSize = vertexCount * selectedAccessor->getElementByteLength();
                void* sourceData = malloc(sourceSize);
                
                shared_ptr <JSONExport::JSONBuffer> referenceBuffer = selectedAccessor->getBuffer();
                shared_ptr <JSONExport::JSONDataBuffer> remappedBuffer(new JSONExport::JSONDataBuffer(referenceBuffer->getID(), sourceData, sourceSize, true));                
                shared_ptr <JSONExport::JSONAccessor> remappedAccessor(new JSONExport::JSONAccessor(selectedAccessor.get()));
                remappedAccessor->setBuffer(remappedBuffer);
                remappedAccessor->setCount(vertexCount);
                
                indexSetToAccessor[(*accessorIterator).first] = remappedAccessor;
                
                _allRemappedAccessors.push_back(remappedAccessor);                            
            }
        }
        
        /*
        if (_allOriginalAccessors.size() != allIndices.size()) {
            // FIXME: report error
            return false;
        }
        */
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)calloc(sizeof(unsigned int) * (*allIndices).size(), 1);

            for (unsigned int k = 0 ; k < (*allIndices).size() ; k++) {
                JSONExport::Semantic semantic = (*allIndices)[k]->getSemantic();
                unsigned int indexSet = (*allIndices)[k]->getIndexOfSet();
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            bool status = _primitives[i]->_remapVertexes((*allIndices),
                                                         this->_allOriginalAccessors ,
                                                         this->_allRemappedAccessors,
                                                         indicesInRemapping,
                                                         allPrimitiveRemapInfos[i]);
            free(indicesInRemapping);

            if (!status) {
                // FIXME: report error
                return false;
            }
            
        } 
        
        if (endIndex > 65535) {
            //The mesh should be split but we do not handle this feature yet
            printf("WARNING: mesh has more than 65535 vertex, splitting has to be done for GL/ES \n");
            return false; //for now return false as splitting is not implemented
        } 
        
        return true;
    }
    
    PrimitiveVector JSONMesh::getPrimitives()
    {
        return this->_primitives;
    }
        
    bool const JSONMesh::writeAllBuffers(std::ofstream& fileOutputStream) 
    {
        typedef map<std::string , shared_ptr<JSONExport::JSONBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        
        vector <shared_ptr< JSONExport::JSONAccessor> > allAccessors = this->remappedAccessors();
        
        shared_ptr <JSONBuffer> dummyBuffer(new JSONBuffer(0));
        
        PrimitiveVector primitives = this->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {            
            shared_ptr<JSONExport::JSONPrimitive> primitive = primitives[i];            
            shared_ptr <JSONExport::JSONIndices> uniqueIndices = primitive->getUniqueIndices();
            
            /*
             Convert the indices to unsigned short and write the blob 
             */
            
            unsigned int indicesCount = (unsigned int)uniqueIndices->getCount();
            unsigned int* uniqueIndicesBuffer = (unsigned int*) static_pointer_cast <JSONDataBuffer> (uniqueIndices->getBuffer())->getData();
            if (indicesCount <= 0) {
                // FIXME: report error
            } else {
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                unsigned short* ushortIndices = (unsigned short*)calloc(indicesLength, 1);
                for (unsigned int idx = 0 ; idx < indicesCount ; idx++) {
                    ushortIndices[idx] = (unsigned short)uniqueIndicesBuffer[idx];
                }
                    
                uniqueIndices->setByteOffset(static_cast<size_t>(fileOutputStream.tellp()));
                fileOutputStream.write((const char*)ushortIndices, indicesLength);
                
                //now that we wrote to the stream we can release the buffer.
                uniqueIndices->setBuffer(dummyBuffer);
                
                free(ushortIndices);
            }
        }
        
        for (unsigned int j = 0 ; j < allAccessors.size() ; j++) {
            shared_ptr <JSONExport::JSONAccessor> accessor = allAccessors[j];
            shared_ptr <JSONExport::JSONBuffer> buffer = accessor->getBuffer();
            
            if (!buffer.get()) {
                // FIXME: report error
                return false;
            }
                        
            if (!IDToBuffer[buffer->getID().c_str()].get()) {
                // FIXME: this should be internal to accessor when a Data buffer is set
                // for this, add a type to buffers , and check this type in setBuffer , then call computeMinMax
                accessor->computeMinMax();
                
                accessor->setByteOffset(static_cast<size_t>(fileOutputStream.tellp()));
                fileOutputStream.write((const char*)(static_pointer_cast <JSONDataBuffer> (buffer)->getData()), buffer->getByteSize());

                //now that we wrote to the stream we can release the buffer.
                accessor->setBuffer(dummyBuffer);
                
                IDToBuffer[buffer->getID()] = buffer;
            } 
        }
                
        return true;
    }

}
