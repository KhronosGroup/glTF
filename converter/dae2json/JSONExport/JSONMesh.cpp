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
    
    bool JSONMesh::buildUniqueIndexes()
    {
        unsigned int startIndex = 1; // begin at 1 because the hashtable will return 0 when the element is not present
        unsigned int endIndex = 0;
        unsigned int primitiveCount = (unsigned int)_primitives.size();
        unsigned int maxVertexAttributes = 0;
        
        if (primitiveCount == 0) {
            // FIXME: report error
            return false;
        }
            
        vector <shared_ptr<JSONExport::JSONPrimitiveRemapInfos> > allPrimitiveRemapInfos;
        
        if (primitiveCount > 0) {
            JSONExport::RemappedMeshIndexesHashmap remappedMeshIndexesMap;
            
            for (unsigned int i = 0 ; i < primitiveCount ; i++) {
                unsigned int indicesCount = (unsigned int)this->_primitives[i]->allIndices().size();
                if (indicesCount > maxVertexAttributes) {
                    maxVertexAttributes = indicesCount; 
                }
            }
            
            for (unsigned int i = 0 ; i < primitiveCount ; i++) {
                shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos = this->_primitives[i]->buildUniqueIndexes(remappedMeshIndexesMap, maxVertexAttributes, startIndex, endIndex);
                if (primitiveRemapInfos.get()) {
                    startIndex = endIndex;
                    allPrimitiveRemapInfos.push_back(primitiveRemapInfos);               
                } else {
                    // FIXME: report error
                    return false;
                }
            }
        } else {
            
            return false;
        }
        
        // we are using WebGL for rendering, this involve OpenGL/ES where only float are supported.
        // now we got not only the uniqueIndexes but also the number of different indexes, i.e the number of vertex attributes count
        // we can allocate the buffer to hold vertex attributes
        unsigned int vertexCount = endIndex - 1;
        
        //For each indices:
        shared_ptr <JSONExport::JSONPrimitive> primitive = this->_primitives[0];
        vector <shared_ptr<JSONExport::JSONIndices> > allIndices = primitive->allIndices();
        //unsigned int allIndicesSize = allIndices.size();
        
        for (int i = 0 ; i < allIndices.size() ; i++) {
            shared_ptr <JSONExport::JSONIndices> indices = allIndices[i];
            shared_ptr <JSONExport::JSONAccessor> selectedAccessor;
            
            //1. get name and semantic.
            JSONExport::Semantic semantic = indices->getSemantic();
            std::string name = indices->getAccessorID();
            
            //2. get accessors for semantic
            IndexSetToAccessorHashmap& indexSetToAccessor = this->getAccessorsForSemantic(semantic);
            
            //3. if accessors.size() > 1 select using name
            if (name.size() > 0) {
                IndexSetToAccessorHashmap::const_iterator accessorIterator;
                
                //FIXME: consider turn this search into a method for mesh
                for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                    //(*it).first;             // the key value (of type Key)
                    //(*it).second;            // the mapped value (of type T)
                    shared_ptr <JSONExport::JSONAccessor> accessor = (*accessorIterator).second;
                    
                    if (accessor->getBuffer()->getID() == name) {
                        selectedAccessor = accessor;
                    }
                }
            } else {
                selectedAccessor = indexSetToAccessor[0];
            }
            
            if (selectedAccessor.get() == 0) {
                // FIXME: report error
                return false;
            }
            
            size_t sourceSize = vertexCount * selectedAccessor->getElementByteLength();
            void* sourceData = malloc(sourceSize);
            
            shared_ptr <JSONExport::JSONBuffer> referenceBuffer = selectedAccessor->getBuffer();
            shared_ptr <JSONExport::JSONDataBuffer> remappedBuffer(new JSONExport::JSONDataBuffer(referenceBuffer->getID(), sourceData, sourceSize, true));                
            shared_ptr <JSONExport::JSONAccessor> remappedAccessor(new JSONExport::JSONAccessor(selectedAccessor.get()));
            remappedAccessor->setBuffer(remappedBuffer);
            remappedAccessor->setCount(vertexCount);
            
            indexSetToAccessor[indices->getIndexOfSet()] = remappedAccessor;
            
            _allOriginalAccessors.push_back(selectedAccessor);
            _allRemappedAccessors.push_back(remappedAccessor);
        }
        
        if (_allOriginalAccessors.size() != allIndices.size()) {
            // FIXME: report error
            return false;
        }
        
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            if (!_primitives[i]->_remapVertexes(this->_allOriginalAccessors , this->_allRemappedAccessors, allPrimitiveRemapInfos[i])) {
                // FIXME: report error
                return false;
            }
        }
        
        if (endIndex > 65535) {
            //The should be split but we do not handle this feature yet
            printf("WARNING: mesh has more than 65535 vertex, splitting has to be done for GL/ES \n");
            return false; //for now return false as splitting is not implemented
        } 
        
        return true;
    }
    
    std::vector< shared_ptr<JSONExport::JSONPrimitive> > JSONMesh::getPrimitives()
    {
        return this->_primitives;
    }
        
    bool const JSONMesh::writeAllBuffers(std::ofstream& fileOutputStream) 
    {
        typedef map<std::string , shared_ptr<JSONExport::JSONBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        
        vector <shared_ptr< JSONExport::JSONAccessor> > allAccessors = this->remappedAccessors();
        
        shared_ptr <JSONBuffer> dummyBuffer(new JSONBuffer(0));
        
        std::vector< shared_ptr<JSONExport::JSONPrimitive> > primitives = this->getPrimitives();
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
                unsigned short* ushortIndices = (unsigned short*)malloc(indicesLength);
                
                for (unsigned int idx = 0 ; idx < indicesCount ; idx++) {
                    ushortIndices[idx] = (unsigned short)uniqueIndicesBuffer[idx];
                }
                    
                uniqueIndices->setByteOffset(fileOutputStream.tellp());
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
                // for this, add a type to buffers , and check this type in setBuffer , then call compuateMinMax
                accessor->computeMinMax();
                
                accessor->setByteOffset(fileOutputStream.tellp());
                fileOutputStream.write((const char*)(static_pointer_cast <JSONDataBuffer> (buffer)->getData()), buffer->getByteSize());

                //now that we wrote to the stream we can release the buffer.
                accessor->setBuffer(dummyBuffer);
                
                IDToBuffer[buffer->getID()] = buffer;
            } 
        }
                
        return true;
    }

}
