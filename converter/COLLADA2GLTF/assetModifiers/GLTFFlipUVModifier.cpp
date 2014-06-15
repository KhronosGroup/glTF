// Copyright (c) 2013, Fabrice Robinet.
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
#include "GLTFFlipUVModifier.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{
    GLTFFlipUVModifier::GLTFFlipUVModifier() {
        
    }
    
    GLTFFlipUVModifier::~GLTFFlipUVModifier() {
        
    }

    bool GLTFFlipUVModifier::init() {
        return true;
    };
    
    static void __InvertV(void *value,
                          const std::string &componentType,
                          const std::string &type,
                          size_t componentsPerElement,
                          size_t index,
                          size_t vertexAttributeByteSize,
                          void *context) {
        char* bufferData = (char*)value;
        
        if ((componentType == "FLOAT") && (componentsPerElement > 1)) {
            float* vector = (float*)bufferData;
            vector[1] = (float) (1.0 - vector[1]);
        }         
    }
    
    void GLTFFlipUVModifier::modify(shared_ptr<JSONObject> glTFAsset) {
        if (glTFAsset->contains(kMeshes) == false)
            return;
        
        shared_ptr<JSONObject> meshes = glTFAsset->getObject(kMeshes);
        std::vector <std::string> meshesUIDs = meshes->getAllKeys();
        
        for (size_t i = 0 ; i < meshesUIDs.size() ; i++) {
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(meshes->getObject(meshesUIDs[i]));
            if (mesh->hasSemantic(GLTF::TEXCOORD)) {
                //https://github.com/KhronosGroup/collada2json/issues/41
                
                size_t attributesCount = mesh->getMeshAttributesCountForSemantic(GLTF::TEXCOORD);
                for (size_t k = 0 ; k < attributesCount ; k++) {
                    shared_ptr <GLTF::GLTFAccessor> meshAttribute = mesh->getMeshAttribute(GLTF::TEXCOORD, k);
                    meshAttribute->applyOnAccessor(__InvertV, NULL);
                }
            }
        }
    }
    
    void GLTFFlipUVModifier::cleanup() {
    }
    
}

