// Copyright (c) Fabrice Robinet
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

#ifndef __MATH_HELPERS__
#define __MATH_HELPERS__

namespace GLTF
{
    //FIXME: add these methods to OpenCOLLADA if needed.
    void decomposeMatrix(COLLADABU::Math::Matrix4 &matrix, float *translation, float *rotation, float *scale);
    void buildLookAtMatrix(COLLADAFW::Lookat *lookat, COLLADABU::Math::Matrix4& matrix);

    class BBOX
    {
    public:
        BBOX();
        BBOX(const COLLADABU::Math::Vector3 &min, const COLLADABU::Math::Vector3 &max);
        
        void merge(BBOX* bbox);
        const COLLADABU::Math::Vector3& getMin3();
        const COLLADABU::Math::Vector3& getMax3();
        
        void transform(const COLLADABU::Math::Matrix4& mat4);
    private:
        COLLADABU::Math::Vector3 _min;
        COLLADABU::Math::Vector3 _max;
    };

}

#endif