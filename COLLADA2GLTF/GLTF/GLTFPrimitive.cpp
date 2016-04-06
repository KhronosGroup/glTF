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

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{    
    //---- GLTFPrimitiveRemapInfos ------------------------------------------------------------------------------------------

    GLTFPrimitive::GLTFPrimitive()
    {
    }
    
    GLTFPrimitive::~GLTFPrimitive()
    {
    }
    
    GLTFPrimitive::GLTFPrimitive(const GLTFPrimitive &primitive) : JSONObject()
    {
        this->_allVertexAttributes = primitive._allVertexAttributes;
        GLTFPrimitive *pr = const_cast<GLTFPrimitive*>(&primitive);
        this->setMode(pr->getMode());
        this->setMaterialID(pr->getMaterialID());
        this->_materialObjectID = primitive._materialObjectID;
        this->_uniqueIndices = primitive._uniqueIndices;
        
    }
    
    shared_ptr<GLTFPrimitive> GLTFPrimitive::clone()
    {
        shared_ptr<GLTFPrimitive> primitive(new GLTFPrimitive(*this));
        //FIXME: this should be done in GLTFPrimitive, but it's not possible at the moment, due to const signature that needs to be widely updated..
        primitive->setIndices(this->getIndices());
        
        return primitive;
    }


    GLTF::Semantic GLTFPrimitive::getSemanticAtIndex(unsigned int index)
    {
        return this->_allVertexAttributes[index]->getSemantic();
    }
    
    unsigned int GLTFPrimitive::getIndexOfSetAtIndex(unsigned int index)
    {
		return (unsigned int)this->_allVertexAttributes[index]->getIndexOfSet();
    }

    size_t GLTFPrimitive::getVertexAttributesCount()
    {
        return this->_allVertexAttributes.size();
    }
    
    VertexAttributeVector GLTFPrimitive::getVertexAttributes()
    {
        return this->_allVertexAttributes;
    }
    
    void GLTFPrimitive::appendVertexAttribute(shared_ptr <JSONVertexAttribute> VertexAttribute)
    {
        this->_allVertexAttributes.push_back(VertexAttribute);
    }    
        
    unsigned int GLTFPrimitive::getMode() {
        return this->getUnsignedInt32(kMode);
    }
    
    void GLTFPrimitive::setMode(unsigned int type) {
        this->setUnsignedInt32(kMode, type);
    }
    
    std::string GLTFPrimitive::getMaterialID() {
        return this->getString(kMaterial);
    }
    
    void GLTFPrimitive::setMaterialID(std::string materialID) {
        this->setString(kMaterial, materialID);
    }
    
    unsigned int GLTFPrimitive::getMaterialObjectID() {
        return this->_materialObjectID;
    }
    
    void GLTFPrimitive::setMaterialObjectID(unsigned int materialID) {
        this->_materialObjectID = materialID;
    }
    
    shared_ptr <GLTF::GLTFAccessor>  GLTFPrimitive::getIndices() {
        return this->_uniqueIndices;
    }
    
    void GLTFPrimitive::setIndices(shared_ptr <GLTF::GLTFAccessor> indices) {
        this->setString(kIndices, indices->getID());
        this->_uniqueIndices = indices;
    }
    
    std::string GLTFPrimitive::valueType() {
        return "primitive";
    }

};
