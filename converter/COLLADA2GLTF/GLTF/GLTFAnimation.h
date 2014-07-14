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
    class GLTFAnimationFlattener;
    class GLTFAsset;
        
    class COLLADA2GLTF_EXPORT GLTFAnimation : public JSONObject {
    public:
        GLTFAnimation();
        virtual ~GLTFAnimation();
        
        size_t getCount();
        void setCount(size_t count);
        
        void setID(std::string animationID);
        std::string getID();
        
        void setOriginalID(std::string originalID);
        std::string getOriginalID();
        
        std::shared_ptr <JSONObject> getParameterNamed(std::string parameter);
        void removeParameterNamed(std::string parameter);
        
        std::shared_ptr <JSONObject> samplers();
        std::shared_ptr <JSONArray> channels();
        std::shared_ptr <JSONObject> targets();
        std::shared_ptr <JSONObject> parameters();

        std::string getSamplerIDForName(std::string name);
        
        void registerAnimationFlatteners(AnimationFlattenerForTargetUIDSharedPtr);
        
        std::shared_ptr<GLTFAnimationFlattener> animationFlattenerForTargetUID(std::string);

        void registerBufferView(std::string parameterName, std::shared_ptr <GLTFBufferView>);
        void unregisterBufferView(std::string parameterName);

        std::shared_ptr<GLTFBufferView> getBufferViewForParameter(std::string);
        
        void writeAnimationForTargetID(const std::string &targetID, GLTFAsset* asset);
        
    private:
        std::string _id;
        std::string _originalID;
        std::shared_ptr <JSONObject> _targets;
        AnimationFlattenerForTargetUIDSharedPtr _animationFlattenerForTargetUID;
        std::map<std::string , std::shared_ptr<GLTFBufferView> > _bufferViews;
    };
    
}

#endif