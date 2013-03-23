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
#include "../helpers/geometryHelpers.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF
{
    GLTFMesh::GLTFMesh()
    {
    }
    
    GLTFMesh::~GLTFMesh()
    {
    }

    GLTFMesh::GLTFMesh(const GLTFMesh &mesh)
    {
        this->_primitives = mesh._primitives;
        this->_semanticToAccessors = mesh._semanticToAccessors;
        this->_ID = mesh._ID;
        this->_name = mesh._name;
    }
    
    shared_ptr <AccessorVector> GLTFMesh::accessors()
    {
        shared_ptr <AccessorVector> accessors(new AccessorVector());
        vector <GLTF::Semantic> allSemantics = this->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = this->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <GLTF::GLTFAccessor> selectedAccessor = (*accessorIterator).second;
                unsigned int indexSet = (*accessorIterator).first;
                GLTF::Semantic semantic = allSemantics[i];
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
                unsigned int size = (unsigned int)accessors->size();
                semanticAndSetToIndex[semanticIndexSetKey] = size;
                
                accessors->push_back(selectedAccessor);
            }
        }
        return accessors;
    }
    
    bool GLTFMesh::appendPrimitive(shared_ptr <GLTF::GLTFPrimitive> primitive)
    {
        this->_primitives.push_back(primitive);
        return true;
    }
    
    void GLTFMesh::setAccessorsForSemantic(GLTF::Semantic semantic, IndexSetToAccessorHashmap& indexSetToAccessorHashmap)
    {
        this->_semanticToAccessors[semantic] = indexSetToAccessorHashmap;
    }
    
    IndexSetToAccessorHashmap& GLTFMesh::getAccessorsForSemantic(Semantic semantic)
    {
        return this->_semanticToAccessors[semantic];
    }
    
    vector <GLTF::Semantic> GLTFMesh::allSemantics()
    {
        vector <GLTF::Semantic> allSemantics;
        
        SemanticToAccessorHashmap::const_iterator accessorIterator;
        
        //FIXME: consider turn this search into a method for mesh
        for (accessorIterator = this->_semanticToAccessors.begin() ; accessorIterator != this->_semanticToAccessors.end() ; accessorIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            allSemantics.push_back((*accessorIterator).first);
        }
        
        return allSemantics;
    }
    
    std::string GLTFMesh::getID()
    {
        return _ID;
    }
    
    void GLTFMesh::setID(std::string ID)
    {
        this->_ID = ID;
    }
    
    std::string GLTFMesh::getName()
    {        
        return _name;
    }

    void GLTFMesh::setName(std::string name)
    {
        this->_name = name;
    }
    
    PrimitiveVector const GLTFMesh::getPrimitives()
    {
        return this->_primitives;
    }
        
    bool const GLTFMesh::writeAllBuffers(std::ofstream& verticesOutputStream, std::ofstream& indicesOutputStream)
    {
        typedef map<std::string , shared_ptr<GLTF::GLTFBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        
        shared_ptr <AccessorVector> allAccessors = this->accessors();
        
        shared_ptr <GLTFBufferView> dummyBuffer(new GLTFBufferView());
        
        PrimitiveVector primitives = this->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {            
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];            
            shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
            
            /*
                Convert the indices to unsigned short and write the blob 
             */
            unsigned int indicesCount = (unsigned int)uniqueIndices->getCount();
            
            shared_ptr <GLTFBufferView> indicesBufferView = uniqueIndices->getBufferView();
            unsigned char* uniqueIndicesBufferPtr = (unsigned char*)indicesBufferView->getBuffer()->getData();
            uniqueIndicesBufferPtr += indicesBufferView->getByteOffset();
            
            unsigned int* uniqueIndicesBuffer = (unsigned int*) uniqueIndicesBufferPtr;
            if (indicesCount <= 0) {
                // FIXME: report error
            } else {
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                unsigned short* ushortIndices = (unsigned short*)calloc(indicesLength, 1);
                for (unsigned int idx = 0 ; idx < indicesCount ; idx++) {
                    ushortIndices[idx] = (unsigned short)uniqueIndicesBuffer[idx];
                }
                    
                uniqueIndices->setByteOffset(static_cast<size_t>(indicesOutputStream.tellp()));
                indicesOutputStream.write((const char*)ushortIndices, indicesLength);
                
                //now that we wrote to the stream we can release the buffer.
                uniqueIndices->setBufferView(dummyBuffer);
                
                free(ushortIndices);
            }
        }
        
        for (unsigned int j = 0 ; j < allAccessors->size() ; j++) {
            shared_ptr <GLTFAccessor> accessor = (*allAccessors)[j];
            shared_ptr <GLTFBufferView> bufferView = accessor->getBufferView();
            shared_ptr <GLTFBuffer> buffer = bufferView->getBuffer();
            
            if (!bufferView.get()) {
                // FIXME: report error
                return false;
            }
                        
            if (!IDToBuffer[bufferView->getBuffer()->getID().c_str()].get()) {
                // FIXME: this should be internal to accessor when a Data buffer is set
                // for this, add a type to buffers , and check this type in setBuffer , then call computeMinMax
                accessor->computeMinMax();
                
                accessor->setByteOffset(static_cast<size_t>(verticesOutputStream.tellp()));
                verticesOutputStream.write((const char*)(static_pointer_cast <GLTFBuffer> (buffer)->getData()), buffer->getByteLength());

                //now that we wrote to the stream we can release the buffer.
                accessor->setBufferView(dummyBuffer);
                
                IDToBuffer[bufferView->getBuffer()->getID().c_str()] = buffer;
            } 
        }
                
        return true;
    }

}
