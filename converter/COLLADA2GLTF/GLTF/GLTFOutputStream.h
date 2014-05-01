// Copyright (c) 2012, Fabrice Robinet.
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

#ifndef __GLTF_OUTPUT_STREAM__
#define __GLTF_OUTPUT_STREAM__

namespace GLTF
{
    class GLTFOutputStream {
    private:
        GLTFOutputStream();
    public:
        GLTFOutputStream(const std::string &folder, const std::string &file, const std::string &kind);
        
        size_t length();
        void write(std::shared_ptr<GLTFBuffer> buffer);
        void write(std::shared_ptr<GLTFBufferView> bufferView);
        void write(const char* buffer, size_t length);

        const std::string& filename();
        const std::string& id();

        const char* outputPathCStr();
        
        void close();
        
        virtual ~GLTFOutputStream();
        
    private:
        std::ofstream _stream;
        std::string _outputPath;
        std::string _filename;
        std::string _id;
        bool _opened;
    };
}


#endif
