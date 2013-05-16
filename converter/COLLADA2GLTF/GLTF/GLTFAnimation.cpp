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

using namespace rapidjson;

namespace GLTF 
{
    
    //-- GLTFAnimation::Parameter
    
    GLTFAnimation::Parameter::Parameter() {
        this->_id = GLTFUtils::generateIDForType("parameter");
    }

    GLTFAnimation::Parameter::Parameter(std::string id) {
        this->_id = id;
    }
    
    GLTFAnimation::Parameter::~Parameter() {
        
    }
    
    void GLTFAnimation::Parameter::setBufferView(shared_ptr <GLTFBufferView> bufferView) {
        this->_bufferView = bufferView;
    }
    
    shared_ptr <GLTFBufferView>  GLTFAnimation::Parameter::getBufferView() {
        return this->_bufferView;
    }
    
    void GLTFAnimation::Parameter::setType(std::string componentType) {
        this->_type = componentType;
    }
    
    std::string GLTFAnimation::Parameter::getType() {
        return this->_type;
    }
    
    size_t GLTFAnimation::Parameter::getByteOffset() {
        return this->_byteOffset;
    }
    
    void GLTFAnimation::Parameter::setByteOffset(size_t byteOffset) {
        this->_byteOffset = byteOffset;
    }
    
    void GLTFAnimation::Parameter::setID(std::string ID) {
        this->_id = ID;
    }
    
    std::string GLTFAnimation::Parameter::getID() {
        return this->_id;
    }

    //-- GLTFAnimation
    
    GLTFAnimation::GLTFAnimation() {
        this->_samplers = shared_ptr<JSONObject> (new JSONObject());
        this->_channels = shared_ptr<JSONObject> (new JSONObject());
    }

    GLTFAnimation::~GLTFAnimation() {}
    
    size_t GLTFAnimation::getCount() {
        return this->_count;
    }
    
    void GLTFAnimation::setCount(size_t count) {
        this->_count = count;
    }
    
    size_t GLTFAnimation::getDuration() {
        return this->_duration;
    }
    
    void GLTFAnimation::setDuration(size_t duration) {
        this->_duration = duration;
    }
 
    std::vector <shared_ptr <GLTFAnimation::Parameter> >* GLTFAnimation::parameters() {
        return &this->_parameters;
    }

    //TODO: consider having a map to avoid this loop, but likely, we won't typically have many parameters
    GLTFAnimation::Parameter* GLTFAnimation::getParameterNamed(std::string parameter) {
        GLTFAnimation::Parameter *parameterOutput = 0;
        for (size_t i = 0 ; i < this->_parameters.size() ; i++) {
            if (this->_parameters[i]->getID() == parameter) {
                return this->_parameters[i].get();
            }
        }
        
        return parameterOutput;
    }

    void GLTFAnimation::removeParameterNamed(std::string parameter) {
        for (size_t i = 0 ; i < this->_parameters.size() ; i++) {
            if (this->_parameters[i]->getID() == parameter) {
                this->_parameters.erase(this->_parameters.begin() + i);
                break;
            }
        }
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

    shared_ptr <JSONObject> GLTFAnimation::channels() {
        return this->_channels;
    }

    std::string GLTFAnimation::getSamplerIDForName(std::string name) {
        return this->_id + "_" + name + "_sampler";
    }

    
}
