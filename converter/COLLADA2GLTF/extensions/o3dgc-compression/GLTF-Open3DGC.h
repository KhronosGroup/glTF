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

#ifndef __GLTF_Open3DGC__
#define __GLTF_Open3DGC__

namespace GLTF
{
    bool canEncodeOpen3DGCMesh(shared_ptr <GLTFMesh> mesh);
    
    void encodeOpen3DGCMesh(shared_ptr <GLTFMesh> mesh,
                            shared_ptr<JSONObject> floatAttributeIndexMapping,
                            GLTFConverterContext& converterContext);
    
    void encodeDynamicVector(float *buffer, const std::string &path, size_t componentsCount, size_t count, GLTFConverterContext& converterContext);
    
    void setupAndWriteAnimationParameter(shared_ptr <GLTFAnimation> cvtAnimation,
                                         const std::string& parameterSID,
                                         const std::string& parameterType,
                                         unsigned char* buffer, size_t length, bool isInputParameter,
                                         GLTFConverterContext &converterContext);
}

#endif
