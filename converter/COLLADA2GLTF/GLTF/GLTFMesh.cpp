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

//--- X3DGC
#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
//#include "opengc_Vector.h"
#include "ogcSC3DMCEncodeParams.h"
#include "ogcIndexedFaceSet.h"
#include "ogcSC3DMCEncoder.h"
#include "ogcSC3DMCDecoder.h"
using namespace ogc;
//--- X3DGC


#include "GLTF.h"
#include "../helpers/geometryHelpers.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF
{
    GLTFMesh::GLTFMesh() {
        this->_remapTableForPositions = 0;
    }
    
    GLTFMesh::~GLTFMesh() {
        if (this->_remapTableForPositions)
            free(this->_remapTableForPositions);
    }

    GLTFMesh::GLTFMesh(const GLTFMesh &mesh) {
        this->_remapTableForPositions = 0;
        this->_primitives = mesh._primitives;
        this->_semanticToMeshAttributes = mesh._semanticToMeshAttributes;
        this->_ID = mesh._ID;
        this->_name = mesh._name;
        this->_extensions = shared_ptr<JSONObject>(new JSONObject());
    }
    
    shared_ptr <MeshAttributeVector> GLTFMesh::meshAttributes() {
        shared_ptr <MeshAttributeVector> meshAttributes(new MeshAttributeVector());
        vector <GLTF::Semantic> allSemantics = this->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = this->getMeshAttributesForSemantic(allSemantics[i]);
            IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
            for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <GLTF::GLTFMeshAttribute> selectedMeshAttribute = (*meshAttributeIterator).second;
                unsigned int indexSet = (*meshAttributeIterator).first;
                GLTF::Semantic semantic = allSemantics[i];
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int size = (unsigned int)meshAttributes->size();
                semanticAndSetToIndex[semanticIndexSetKey] = size;
                
                meshAttributes->push_back(selectedMeshAttribute);
            }
        }
        return meshAttributes;
    }
    
    bool GLTFMesh::appendPrimitive(shared_ptr <GLTF::GLTFPrimitive> primitive) {
        this->_primitives.push_back(primitive);
        return true;
    }
    
    void GLTFMesh::setMeshAttributesForSemantic(GLTF::Semantic semantic, IndexSetToMeshAttributeHashmap& indexSetToMeshAttributeHashmap) {
        this->_semanticToMeshAttributes[semantic] = indexSetToMeshAttributeHashmap;
    }
    
    IndexSetToMeshAttributeHashmap& GLTFMesh::getMeshAttributesForSemantic(Semantic semantic) {
        return this->_semanticToMeshAttributes[semantic];
    }
    
    vector <GLTF::Semantic> GLTFMesh::allSemantics() {
        vector <GLTF::Semantic> allSemantics;
        
        SemanticToMeshAttributeHashmap::const_iterator meshAttributeIterator;
        
        //FIXME: consider turn this search into a method for mesh
        for (meshAttributeIterator = this->_semanticToMeshAttributes.begin() ; meshAttributeIterator != this->_semanticToMeshAttributes.end() ; meshAttributeIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            allSemantics.push_back((*meshAttributeIterator).first);
        }
        
        return allSemantics;
    }
    
    std::string GLTFMesh::getID() {
        return _ID;
    }
    
    void GLTFMesh::setID(std::string ID) {
        this->_ID = ID;
    }
    
    std::string GLTFMesh::getName() {
        return _name;
    }

    void GLTFMesh::setName(std::string name) {
        this->_name = name;
    }
    
    PrimitiveVector const GLTFMesh::getPrimitives() {
        return this->_primitives;
    }
    
    //TODO: allocate extras lazily
    shared_ptr<JSONObject> GLTFMesh::getExtensions() {
        return _extensions;
    }
    
    bool GLTFMesh::writeAllBuffers(std::ofstream& verticesOutputStream, std::ofstream& indicesOutputStream, std::ofstream& compressedDataOutputStream) {

        if (this->getExtensions()->contains("won-compression")) {
            shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(this->getExtensions()->getValue("won-compression"));

            shared_ptr<GLTFBuffer> buffer = this->getCompressedBuffer();
            
            shared_ptr<JSONObject> compressedData(new JSONObject());
            compressedData->setUnsignedInt32("count", buffer->getByteLength());
            compressedData->setString("type", "UNSIGNED_BYTE");
            compressedData->setUnsignedInt32("byteOffset", static_cast<size_t>(compressedDataOutputStream.tellp()));
            compressionObject->setValue("compressedData", compressedData);
            compressedDataOutputStream.write((const char*)buffer->getData(), buffer->getByteLength());
            
            //bufferView will be set when the mesh is serialized
            return true;
        }
        
    void GLTFMesh::setRemapTableForPositions(unsigned int* remapTableForPositions) {
        this->_remapTableForPositions = remapTableForPositions;
    }
    
    unsigned int* GLTFMesh::getRemapTableForPositions() {
        return this->_remapTableForPositions;
    }
    
    bool GLTFMesh::writeAllBuffers(std::ofstream& verticesOutputStream, std::ofstream& indicesOutputStream)
    {
        SC3DMCEncodeParams params;
        IndexedFaceSet ifs;

        bool shouldOGCompressMesh = true;
        if (shouldOGCompressMesh) {
            int qcoord    = 12;
            int qtexCoord = 10;
            int qnormal   = 10;
            params.SetCoordQuantBits(qcoord);
            params.SetNormalQuantBits(qnormal);
            params.SetTexCoordQuantBits(qtexCoord);
        }
        
        typedef map<std::string , shared_ptr<GLTF::GLTFBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        
        shared_ptr <MeshAttributeVector> allMeshAttributes = this->meshAttributes();
        shared_ptr <GLTFBufferView> dummyBuffer(new GLTFBufferView());
        int vertexCount;
        unsigned int indicesCount;
        PrimitiveVector primitives = this->getPrimitives();
        unsigned bufferStart = indicesOutputStream.tellp();
        
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {            
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];            
            shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
            /*
                Convert the indices to unsigned short and write the blob 
             */
            indicesCount = (unsigned int)uniqueIndices->getCount();
            
            if (shouldOGCompressMesh)
                ifs.SetNCoordIndex(indicesCount / 3);
            
            shared_ptr <GLTFBufferView> indicesBufferView = uniqueIndices->getBufferView();
            unsigned char* uniqueIndicesBufferPtr = (unsigned char*)indicesBufferView->getBuffer()->getData();
            uniqueIndicesBufferPtr += indicesBufferView->getByteOffset();
            
            unsigned int* uniqueIndicesBuffer = (unsigned int*) uniqueIndicesBufferPtr;
            if (indicesCount <= 0) {
                // FIXME: report error
            } else {
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                unsigned short* ushortIndices = (unsigned short*)calloc(indicesLength, 1);
                long *longIndices = 0;
                
                if (shouldOGCompressMesh)
                    longIndices = (long*)calloc(sizeof(long) * indicesCount, 1);

                for (unsigned int idx = 0 ; idx < indicesCount ; idx++) {
                    ushortIndices[idx] = (unsigned short)uniqueIndicesBuffer[idx];
                    if (shouldOGCompressMesh)
                        longIndices[idx] = (long)uniqueIndicesBuffer[idx];
                }
                
                if (shouldOGCompressMesh) {
                    ifs.SetCoordIndex((long * const ) longIndices);
                }
                
                if (!shouldOGCompressMesh) {
                    uniqueIndices->setByteOffset(static_cast<size_t>(indicesOutputStream.tellp()));
                    indicesOutputStream.write((const char*)ushortIndices, indicesLength);
                } else {
                    uniqueIndices->setByteOffset(bufferStart);
                    bufferStart += indicesLength; //we simulate how will be the uncompressed data here, so this is the length in short *on purpose*
                }
                
                //now that we wrote to the stream we can release the buffer.
                uniqueIndices->setBufferView(dummyBuffer);
                
                free(ushortIndices);
            }
        }
        
        int attributeCount = 0;
        for (unsigned int j = 0 ; j < allMeshAttributes->size() ; j++) {
            shared_ptr <GLTFMeshAttribute> meshAttribute = (*allMeshAttributes)[j];
            shared_ptr <GLTFBufferView> bufferView = meshAttribute->getBufferView();
            shared_ptr <GLTFBuffer> buffer = bufferView->getBuffer();
            
            if (!bufferView.get()) {
                // FIXME: report error
                return false;
            }
                        
            if (!IDToBuffer[bufferView->getBuffer()->getID()].get()) {
                // FIXME: this should be internal to meshAttribute when a Data buffer is set
                // for this, add a type to buffers , and check this type in setBuffer , then call computeMinMax
                meshAttribute->computeMinMax();
                
                vertexCount = meshAttribute->getCount();
                attributeCount++;
                
                if (shouldOGCompressMesh) {
                    if (j == 0) {
                        ifs.SetNCoord(vertexCount);
                        ifs.SetCoord((Real * const)buffer->getData());
                    }
                    if (j == 1) {
                        ifs.SetNNormal(vertexCount);
                        ifs.SetNormal((Real * const)buffer->getData());
                    }
                    if (j == 2) {
                        ifs.SetNTexCoord(vertexCount);
                        ifs.SetTexCoord((Real * const)buffer->getData());
                    }
                    meshAttribute->setByteOffset(bufferStart);
                    bufferStart += buffer->getByteLength();
                } else {
                    meshAttribute->setByteOffset(static_cast<size_t>(verticesOutputStream.tellp()));
                    verticesOutputStream.write((const char*)(buffer->getData()), buffer->getByteLength());
                }

                //now that we wrote to the stream we can release the buffer.
                meshAttribute->setBufferView(dummyBuffer);
                IDToBuffer[bufferView->getBuffer()->getID()] = buffer;
            } 
        }
        
        if (shouldOGCompressMesh) {
            ifs.ComputeMinMax(OGC_SC3DMC_MAX_ALL_DIMS);
            BinaryStream bstream(vertexCount * 8);
            SC3DMCEncoder encoder;
            encoder.Encode(params, ifs, bstream);
            
            //meshAttribute->setByteOffset(static_cast<size_t>(verticesOutputStream.tellp()));
            //verticesOutputStream.write((const char*)(buffer->getData()), buffer->getByteLength());
        }
        
        return true;
    }

    void GLTFMesh::setCompressedBuffer(shared_ptr<GLTFBuffer> compressedBuffer) {
        this->_compressedBuffer = compressedBuffer;
    }
    
    shared_ptr<GLTFBuffer> GLTFMesh::getCompressedBuffer() {
        return this->_compressedBuffer;
    }

}
