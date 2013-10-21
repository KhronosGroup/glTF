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

#ifndef __JSON_EXPORT_TYPES_AND_CONSTANTS_H__
#define __JSON_EXPORT_TYPES_AND_CONSTANTS_H__

#define EXPORT_MATERIALS_AS_EFFECTS 1

#ifdef _LIBCPP_VERSION
namespace std{
    namespace tr1 = std;
}
#endif

using namespace std::tr1;

namespace GLTF 
{
    class JSONObject;
    class GLTFIndices;
    class GLTFPrimitive;
    class GLTFMeshAttribute;
    class GLTFMesh;
    class GLTFEffect;
    class JSONVertexAttribute;
    class GLTFAnimationFlattener;
    //-- Args & Options
    typedef std::vector <shared_ptr<GLTF::GLTFIndices> > IndicesVector;
    typedef std::vector <shared_ptr<GLTF::GLTFPrimitive> > PrimitiveVector;
    typedef std::vector <shared_ptr<GLTF::GLTFMeshAttribute> > MeshAttributeVector;
    typedef std::vector <shared_ptr<GLTF::JSONVertexAttribute> > VertexAttributeVector;
    
    typedef shared_ptr<GLTFMesh> GLTFMeshSharedPtr;
    typedef std::vector <shared_ptr<GLTF::GLTFMesh> > MeshVector;

    typedef std::map<std::string , shared_ptr <GLTFAnimationFlattener> > AnimationFlattenerForTargetUID;
    typedef shared_ptr<AnimationFlattenerForTargetUID> AnimationFlattenerForTargetUIDSharedPtr;
    typedef std::map<std::string , AnimationFlattenerForTargetUIDSharedPtr > FlattenerMapsForAnimationID;

    std::string generateIDForType( const char* typeCStr, const char* suffix = 0);
    
    typedef enum {
        POSITION = 1,
        NORMAL = 2,
        TEXCOORD = 3,
        COLOR = 4,
        WEIGHT = 5,
        JOINT = 6
    } Semantic;    

    typedef enum {
        NOT_A_JSON_TYPE = 0,
        NUMBER = 1,
        OBJECT = 2,
        ARRAY = 3,
        STRING = 4,
        
        ACCESSOR = 5,
        BUFFER = 6,
        EFFECT = 7,
        INDICES = 8,
        MESH = 9,
        PRIMITIVE = 10
    } JSONValueType;
    
    typedef enum {
        NOT_AN_ELEMENT_TYPE = 0,
        BYTE = 1,
        UNSIGNED_BYTE = 2,
        SHORT = 3,
        UNSIGNED_SHORT = 4,
        FIXED = 5,
        FLOAT = 6,
        INT = 7,
        UNSIGNED_INT = 8
    } ComponentType;
    
};

#endif
