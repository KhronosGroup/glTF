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
    GLTFMesh::GLTFMesh() {
        this->_remapTableForPositions = 0;
        this->_extensions = shared_ptr<JSONObject>(new JSONObject());
        this->_ID = GLTFUtils::generateIDForType("mesh");
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
                shared_ptr <GLTF::GLTFAccessor> selectedMeshAttribute = (*meshAttributeIterator).second;
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
    
    bool GLTFMesh::hasSemantic(Semantic semantic) {
        return this->_semanticToMeshAttributes.count(semantic) > 0;
    }
    
    IndexSetToMeshAttributeHashmap& GLTFMesh::getMeshAttributesForSemantic(Semantic semantic) {
        return this->_semanticToMeshAttributes[semantic];
    }
    
    size_t GLTFMesh::getMeshAttributesCountForSemantic(Semantic semantic) {
        return this->_semanticToMeshAttributes[semantic].size();
    }
    
    shared_ptr<GLTFAccessor> GLTFMesh::getMeshAttribute(Semantic semantic, size_t indexOfSet) {
        IndexSetToMeshAttributeHashmap& hasmap = this->_semanticToMeshAttributes[semantic];
        return hasmap[indexOfSet];
    }

    void GLTFMesh::setMeshAttribute(Semantic semantic, size_t indexOfSet, shared_ptr<GLTFAccessor> meshAttribute) {
        IndexSetToMeshAttributeHashmap& hasmap = this->_semanticToMeshAttributes[semantic];
        hasmap[indexOfSet] = meshAttribute;
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
    
    void GLTFMesh::setRemapTableForPositions(unsigned int* remapTableForPositions) {
        this->_remapTableForPositions = remapTableForPositions;
    }
    
    unsigned int* GLTFMesh::getRemapTableForPositions() {
        return this->_remapTableForPositions;
    }
    
#ifdef USE_WEBGLLOADER
    void GLTFMesh::setCompressedBuffer(shared_ptr<GLTFBuffer> compressedBuffer) {
        this->_compressedBuffer = compressedBuffer;
    }
    
    shared_ptr<GLTFBuffer> GLTFMesh::getCompressedBuffer() {
        return this->_compressedBuffer;
    }
#endif
}
