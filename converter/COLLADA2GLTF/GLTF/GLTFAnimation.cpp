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
#include "../GLTFConverterContext.h"
#include "animationConverter.h"

using namespace rapidjson;

namespace GLTF 
{

    //-- GLTFAnimation
    
    GLTFAnimation::GLTFAnimation() {
        this->_samplers = shared_ptr<JSONObject> (new JSONObject());
        this->_channels = shared_ptr<JSONArray> (new JSONArray());
        this->_targets = shared_ptr<JSONObject> (new JSONObject());
        this->_parameters = shared_ptr<JSONObject> (new JSONObject());
    }

    GLTFAnimation::~GLTFAnimation() {}

    shared_ptr <JSONObject> GLTFAnimation::targets() {
        return this->_targets;
    }

    size_t GLTFAnimation::getCount() {
        return this->_count;
    }
    
    void GLTFAnimation::setCount(size_t count) {
        this->_count = count;
    }
    
    double GLTFAnimation::getDuration() {
        return this->_duration;
    }
    
    void GLTFAnimation::setDuration(double duration) {
        this->_duration = duration;
    }
 
    shared_ptr <JSONObject> GLTFAnimation::parameters() {
        return this->_parameters;
    }

    shared_ptr <JSONObject>  GLTFAnimation::getParameterNamed(std::string parameter) {
        return this->_parameters->getObject(parameter);
    }

    void GLTFAnimation::removeParameterNamed(std::string parameter) {
        this->_parameters->removeValue(parameter);
    }

    void GLTFAnimation::setID(std::string animationID) {
        this->_id = animationID;
    }
    
    std::string GLTFAnimation::getID() {
        return this->_id;
    }
    
    shared_ptr <JSONObject> GLTFAnimation::samplers() {
        return this->_samplers;
    }

    shared_ptr <JSONArray> GLTFAnimation::channels() {
        return this->_channels;
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

}
