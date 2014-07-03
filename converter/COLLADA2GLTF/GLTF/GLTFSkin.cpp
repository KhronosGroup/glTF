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

#include "GLTF.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF 
{
    //--- Controller
    GLTFController::GLTFController() : JSONObject() {
        this->_extras = shared_ptr<JSONObject>(new JSONObject());
    }
    
    GLTFController::~GLTFController() {
        
    }

    shared_ptr <JSONObject> GLTFController::extras() {
        return this->_extras;
    }

    //--- Skin
    
    GLTFSkin::GLTFSkin() : GLTFController(), _jointsCount(0) {
        this->_id = GLTFUtils::generateIDForType(kSkin.c_str());
    }

    GLTFSkin::GLTFSkin(std::string id) : GLTFController(), _jointsCount(0) {
        this->_id = id;
    }

    
    GLTFSkin::~GLTFSkin() {
    }
    
    std::string GLTFSkin::getType() {
        return "skin";
    }
    
    shared_ptr<JSONArray> GLTFSkin::getBindShapeMatrix() {
        return this->getArray(kBindShapeMatrix);
    }
    
    void GLTFSkin::setBindShapeMatrix(shared_ptr<JSONArray>  bindShapeMatrix) {
        this->setValue(kBindShapeMatrix, bindShapeMatrix);
    }
    
    shared_ptr<JSONArray> GLTFSkin::getJointsIds() {
        return this->getArray(kJoints);
    }
    
    void GLTFSkin::setJointsIds(shared_ptr<JSONArray> jointIds) {
        this->setValue(kJoints, jointIds);
    }
    
    const std::string& GLTFSkin::getId() {
        return this->_id;
    }
    
    void GLTFSkin::setJoints(shared_ptr <GLTFAccessor> joints) {
        this->_joints = joints;
    }
    
    shared_ptr <GLTFAccessor> GLTFSkin::getJoints() {
        return this->_joints;
    }
    
    void GLTFSkin::setWeights(shared_ptr <GLTFAccessor> weights) {
        this->_weights = weights;
    }

    shared_ptr <GLTFAccessor> GLTFSkin::getWeights() {
        return this->_weights;
    }
    
    void GLTFSkin::setSourceUID(std::string uniqueId) {
        this->_sourceUID = uniqueId;
    }
    
    std::string GLTFSkin::getSourceUID() {
        return this->_sourceUID;
    }
    
    void GLTFSkin::setInverseBindMatrices(shared_ptr <GLTFBufferView> inverseBindMatrices) {
        this->_inverseBindMatrices = inverseBindMatrices;
    }
    
    shared_ptr <GLTFBufferView> GLTFSkin::getInverseBindMatrices() {
        return this->_inverseBindMatrices;
    }
    
    void GLTFSkin::setJointsCount(size_t count) {
        this->_jointsCount = count;
    }
    
    size_t GLTFSkin::getJointsCount() {
        return this->_jointsCount;
    }
    
};
