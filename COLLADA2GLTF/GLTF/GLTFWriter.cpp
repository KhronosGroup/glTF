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
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"
#include "GLTFWriter.h"

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{    
    shared_ptr <JSONValue> serializeVec3(double x,double y, double z) {
        shared_ptr <JSONArray> vec3(new GLTF::JSONArray());
        
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(x)));
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(y)));
        vec3->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(z)));
        
        return vec3;
    }
    
    shared_ptr <JSONValue> serializeVec4(double x,double y, double z, double w) {
        shared_ptr <JSONArray> vec4(new GLTF::JSONArray());
        
        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(x)));
        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(y)));
        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(z)));

        vec4->appendValue(shared_ptr <GLTF::JSONNumber> (new GLTF::JSONNumber(w)));

        return vec4;
    }
        
}
