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

#include "GLTF.h"
#include "GLTFAsset.h"
#include "animationConverter.h"
#include "GLTF-Open3DGC.h" //FIXME: setupAndWriteAnimationParameter has to move out from this header

using namespace rapidjson;
using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{
    //-- GLTFAnimation
    
    GLTFAnimation::GLTFAnimation() : JSONObject() {
        this->createObjectIfNeeded(kSamplers);
        this->createArrayIfNeeded(kChannels);
        this->createObjectIfNeeded(kParameters);
        
        this->_targets = shared_ptr<JSONObject> (new JSONObject());
    }

    GLTFAnimation::~GLTFAnimation() {}

    shared_ptr <JSONObject> GLTFAnimation::targets() {
        return this->_targets;
    }

    size_t GLTFAnimation::getCount() {
        return this->getUnsignedInt32(kCount);
    }
    
    void GLTFAnimation::setCount(size_t count) {
		this->setUnsignedInt32(kCount, (unsigned int)count);
    }
     
    shared_ptr <JSONObject> GLTFAnimation::parameters() {
        return this->getObject(kParameters);
    }

    shared_ptr <JSONObject>  GLTFAnimation::getParameterNamed(std::string parameter) {
        return this->parameters()->getObject(parameter);
    }

    void GLTFAnimation::removeParameterNamed(std::string parameter) {
        this->parameters()->removeValue(parameter);
    }

    void GLTFAnimation::setID(std::string animationID) {
        this->_id = animationID;
    }
    
    std::string GLTFAnimation::getID() {
        return this->_id;
    }
    
    shared_ptr <JSONObject> GLTFAnimation::samplers() {
        return this->getObject(kSamplers);
    }

    shared_ptr <JSONArray> GLTFAnimation::channels() {
        return this->getArray(kChannels);
    }

    std::string GLTFAnimation::getSamplerIDForName(std::string name) {
        return this->_id + "_" + name + "_sampler";
    }
    
    shared_ptr<GLTFAnimationFlattener> GLTFAnimation::animationFlattenerForTargetUID(std::string targetUID) {
        
        return (*this->_animationFlattenerForTargetUID)[targetUID];
    }

    void GLTFAnimation::setOriginalID(std::string originalID) {
        this->_originalID = originalID;
    }
    
    std::string GLTFAnimation::getOriginalID() {
        return this->_originalID;
    }
    
    void GLTFAnimation::registerAnimationFlatteners(AnimationFlattenerForTargetUIDSharedPtr animationFlattenerMaps) {
        this->_animationFlattenerForTargetUID = animationFlattenerMaps;
    }

    void GLTFAnimation::registerBufferView(std::string parameterName, shared_ptr <GLTFBufferView> bufferView) {
        this->_bufferViews[parameterName] = bufferView;
    }

    void GLTFAnimation::unregisterBufferView(std::string parameterName) {
        this->_bufferViews.erase(parameterName);
    }

    shared_ptr<GLTFBufferView> GLTFAnimation::getBufferViewForParameter(std::string parameterName) {
        return this->_bufferViews[parameterName];
    }

    /*
     Handles Channel creation + additions
     */
    static void __AddChannel(GLTFAnimation* cvtAnimation,
                             const std::string &targetID,
                             const std::string &path) {
        shared_ptr<JSONObject> trChannel(new JSONObject());
        shared_ptr<JSONObject> trTarget(new JSONObject());
        
        trChannel->setString("sampler", cvtAnimation->getSamplerIDForName(path));
        trChannel->setValue(kTarget, trTarget);
        trTarget->setString("id", targetID);
        trTarget->setString("path", path);
        cvtAnimation->channels()->appendValue(trChannel);
    }

    void GLTFAnimation::writeAnimationForTargetID(const std::string &targetID, GLTFAsset* asset) {
        shared_ptr <JSONObject> target =  this->targets()->getObject(targetID);
        shared_ptr<GLTFAnimationFlattener> animationFlattener = this->animationFlattenerForTargetUID(targetID);
        
        size_t count = 0;
        float* rotations = 0;
        float* positions = 0;
        float* scales = 0;
        
        animationFlattener->allocAndFillAffineTransformsBuffers(&positions, &rotations, &scales, count);
        
        if (animationFlattener->hasAnimatedScale()) {
            //Scale
            setupAndWriteAnimationParameter(this,
                                            "scale",
                                            "FLOAT_VEC3",
                                            (unsigned char*)scales,
                                            count * sizeof(float) * 3, false,
                                            asset);
            __AddChannel(this, targetID, "scale");
            free(scales);
        }
        
        if (animationFlattener->hasAnimatedTranslation()) {
            //Translation
            setupAndWriteAnimationParameter(this,
                                            "translation",
                                            "FLOAT_VEC3",
                                            (unsigned char*)positions,
                                            count * sizeof(float) * 3, false,
                                            asset);
            __AddChannel(this, targetID, "translation");
            free(positions);
        }
        
        if (animationFlattener->hasAnimatedRotation()) {
            //Rotation
            setupAndWriteAnimationParameter(this,
                                            "rotation",
                                            "FLOAT_VEC4",
                                            (unsigned char*)rotations,
                                            count * sizeof(float) * 4, false,
                                            asset);
            __AddChannel(this, targetID, "rotation");
            free(rotations);
        }
    }
    
    std::string GLTFAnimation::valueType() {
        return "animation";
    }

}
