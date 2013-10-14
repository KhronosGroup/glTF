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

#ifndef __GLTF_ANIMATION_H__
#define __GLTF_ANIMATION_H__

namespace GLTF 
{
    class GLTFAnimation {
    public:
        
        class Parameter {
        public:
            Parameter(std::string ID);
            virtual ~Parameter();
            
            void setBufferView(shared_ptr <GLTFBufferView> bufferView);
            shared_ptr <GLTFBufferView>  getBufferView();

            void setType(std::string type);
            std::string getType();
            
            void setID(std::string ID);
            std::string getID();
            
            size_t getCount();
            void setCount(size_t count);

            size_t getByteOffset();
            void setByteOffset(size_t byteOffset);
            
            shared_ptr <JSONObject> extensions();
        private:
            Parameter();
        private:
            std::string _id;
            std::string _type;
            size_t _byteOffset;
            size_t _count;
            shared_ptr <GLTFBufferView> _bufferView;
            shared_ptr <JSONObject> _extensions;
        };
        
        GLTFAnimation();
        virtual ~GLTFAnimation();
        
        size_t getCount();
        void setCount(size_t count);
        
        double getDuration();
        void setDuration(double duration);
        
        void setID(std::string animationID);
        std::string getID();
        
        std::vector <shared_ptr <Parameter> >* parameters();
        GLTFAnimation::Parameter* getParameterNamed(std::string parameter);
        void removeParameterNamed(std::string parameter);
        int indexOfParameterNamed(std::string parameter);
        
        shared_ptr <JSONObject> samplers();
        shared_ptr <JSONArray> channels();
        shared_ptr <JSONObject> targets();

        std::string getSamplerIDForName(std::string name);

    private:
        size_t _count;
        double _duration;
        std::vector <shared_ptr <Parameter> > _parameters;
        std::string _id;
        shared_ptr <JSONArray> _channels;
        shared_ptr <JSONObject> _samplers;
        shared_ptr <JSONObject> _targets;
    };
    
}

#endif