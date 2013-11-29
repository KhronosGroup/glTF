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

#ifndef __GLTF_CONFIG__
#define __GLTF_CONFIG__

namespace GLTF
{
    class GLTFConfig {
    public:        
        GLTFConfig();
        bool initWithPath(const std::string& path);
        
        virtual ~GLTFConfig();
        shared_ptr <JSONObject> config();

        unsigned int unsignedInt32ForKeyPath(const std::string &keyPath, unsigned int defaultValue);
        int int32ForKeyPath(const std::string &keyPath, int defaultValue);
        double doubleForKeyPath(const std::string &keyPath, double defaultValue);
        bool boolForKeyPath(const std::string &keyPath, bool defaultValue = false);
        std::string stringForKeyPath(const std::string &keyPath, std::string defaultValue);
        
    private:
        void _setupDefaultConfigOptions(shared_ptr<JSONObject> optionsRoot);
    private:
        shared_ptr<JSONObject> _configObject;
    };
}


#endif