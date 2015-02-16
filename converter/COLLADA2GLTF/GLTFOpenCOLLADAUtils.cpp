// Copyright (c) 2013, Fabrice Robinet
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
#include "GLTFOpenCOLLADA.h"
#include "GLTFOpenCOLLADAUtils.h"

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

void fillFloatPtrFromOpenCOLLADAMatrix4(const COLLADABU::Math::Matrix4 &matrix, float *m) {
    shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());
    
    COLLADABU::Math::Matrix4 transpose = matrix.transpose();
    
    for (int i = 0 ; i < 4 ; i++)  {
        const COLLADABU::Math::Real * real = transpose[i];
        
        m[(i*4) + 0] = (float)real[0];
        m[(i*4) + 1] = (float)real[1];
        m[(i*4) + 2] = (float)real[2];
        m[(i*4) + 3] = (float)real[3];
    }
}

shared_ptr <GLTF::JSONArray> serializeOpenCOLLADAMatrix4(const COLLADABU::Math::Matrix4 &matrix) {
    float m[16];
    shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());
    
    fillFloatPtrFromOpenCOLLADAMatrix4(matrix, m);
    
    for (int i = 0 ; i < 16; i++)  {
        array->appendValue(shared_ptr <GLTF::JSONValue> (new GLTF::JSONNumber(m[i])));
    }
    
    return array;
}

std::string opaqueModeToString(COLLADAFW::EffectCommon::OpaqueMode opaqueMode) {

    std::string opaqueStr;
    
    switch (opaqueMode) {
        case COLLADAFW::EffectCommon::OpaqueMode::A_ONE:
            opaqueStr = "A_ONE";
            break;
        case COLLADAFW::EffectCommon::OpaqueMode::A_ZERO:
            opaqueStr = "A_ZERO";
            break;
        case COLLADAFW::EffectCommon::OpaqueMode::RGB_ZERO:
            opaqueStr = "RGB_ZERO";
            break;
        case COLLADAFW::EffectCommon::OpaqueMode::RGB_ONE:
            opaqueStr = "RGB_ONE";
            break;
        default:
        case COLLADAFW::EffectCommon::OpaqueMode::UNSPECIFIED_OPAQUE:
            opaqueStr = "UNSPECIFIED_OPAQUE";
            break;
    }
    return opaqueStr;
}


