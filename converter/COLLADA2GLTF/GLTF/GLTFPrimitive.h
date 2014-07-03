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

namespace GLTF 
{
    class JSONVertexAttribute
    {
    public:
        JSONVertexAttribute(GLTF::Semantic semantic, size_t indexOfSet):
        _semantic(semantic),
        _indexOfSet(indexOfSet) {
        }
        GLTF::Semantic getSemantic() {
            return _semantic;
        }
        size_t getIndexOfSet() {
            return _indexOfSet;
        }
    private:
        GLTF::Semantic _semantic;
        size_t _indexOfSet;
    };
        
    class GLTFPrimitive : public JSONObject
    {
    public:
        GLTFPrimitive();
        GLTFPrimitive(const GLTFPrimitive& primitive);
        virtual ~GLTFPrimitive();
                
        unsigned int getPrimitive();
        void setPrimitive(unsigned int type);

        std::string getMaterialID();
        void setMaterialID(std::string materialID);
        
        //TODO: These 2 methods should be out of GLTFPrimitive.
        //A map like: primitive [->] objectID, could handle that within COLLADA2GLTFWriter.cpp
        unsigned int getMaterialObjectID();
        void setMaterialObjectID(unsigned int materialID);

        GLTF::Semantic getSemanticAtIndex(unsigned int index);
        unsigned int getIndexOfSetAtIndex(unsigned int index);
        size_t getVertexAttributesCount();
        
        VertexAttributeVector getVertexAttributes();
        void appendVertexAttribute(std::shared_ptr <JSONVertexAttribute> VertexAttribute);
        
        std::shared_ptr <GLTF::GLTFAccessor>  getIndices();
        void setIndices(std::shared_ptr <GLTF::GLTFAccessor> indices);
        
        std::shared_ptr<GLTFPrimitive> clone();

    private:
        unsigned int _materialObjectID;
        std::shared_ptr <GLTF::GLTFAccessor> _uniqueIndices;
        VertexAttributeVector _allVertexAttributes;
    };

}


#endif