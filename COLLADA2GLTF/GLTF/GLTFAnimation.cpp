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

#include "GLTFAsset.h"
#include "GLTFAnimation.h"
#include "meshConverter.h"
#include "GLTF-Open3DGC.h" //FIXME: setupAndWriteAnimationParameter has to move out from this header

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{
    //-- GLTFAnimation
    const std::string GLTFAnimation::AnimationType::ROTATE = "rotation";
    const std::string GLTFAnimation::AnimationType::SCALE = "scale";
    const std::string GLTFAnimation::AnimationType::TRANSLATE = "translation";
    const std::string GLTFAnimation::AnimationType::TIME = "TIME";

    const std::string GLTFAnimation::AnimationDataType::VEC3 = "FLOAT_VEC3";
    const std::string GLTFAnimation::AnimationDataType::VEC4 = "FLOAT_VEC4";
    const std::string GLTFAnimation::AnimationDataType::SCALAR = "FLOAT";

    const std::vector<string> GLTFAnimation::ANIMATION_TYPES = {
        GLTFAnimation::AnimationType::ROTATE,
        GLTFAnimation::AnimationType::SCALE,
        GLTFAnimation::AnimationType::TRANSLATE,
        GLTFAnimation::AnimationType::TIME
    };

    const std::map<std::string, std::string> GLTFAnimation::ANIMATION_DATA_TYPES = {
        { GLTFAnimation::AnimationType::ROTATE, GLTFAnimation::AnimationDataType::VEC4 },
        { GLTFAnimation::AnimationType::SCALE, GLTFAnimation::AnimationDataType::VEC3 },
        { GLTFAnimation::AnimationType::TRANSLATE, GLTFAnimation::AnimationDataType::VEC3 },
        { GLTFAnimation::AnimationType::TIME, GLTFAnimation::AnimationDataType::SCALAR }
    };

    const std::map<std::string, int> GLTFAnimation::ANIMATION_DATA_TYPE_SIZE = {
        { GLTFAnimation::AnimationDataType::VEC3, sizeof(float)*3 },
        { GLTFAnimation::AnimationDataType::VEC4, sizeof(float)*4 },
        { GLTFAnimation::AnimationDataType::SCALAR, sizeof(float) }
    };
   
    GLTFAnimation::GLTFAnimation() : JSONObject() {
        this->createObjectIfNeeded(kSamplers);
        this->createArrayIfNeeded(kChannels);
        this->createObjectIfNeeded(kParameters);
        
        this->_targets = shared_ptr<JSONObject> (new JSONObject());
    }

    GLTFAnimation::GLTFAnimation(const COLLADAFW::Animation* animation, GLTFAsset* asset) : GLTFAnimation() {
        if (animation->getAnimationType() == COLLADAFW::Animation::ANIMATION_CURVE) {
            shared_ptr <JSONObject> animationParameters = this->parameters();
            COLLADAFW::AnimationCurve *animationCurve = (COLLADAFW::AnimationCurve*)animation;

            std::string animationID = uniqueIdWithType(kAnimation, animation->getUniqueId());
            this->setID(animationID);

            // Get the input and output values of the animation
            COLLADAFW::FloatOrDoubleArray inputArray = animationCurve->getInputValues();
            COLLADAFW::FloatOrDoubleArray outputArray = animationCurve->getOutputValues();

            // Scale the data and copy it
            bool bNeedsScale = false;
            int numDimensions = animationCurve->getOutDimension();
            const COLLADAFW::PhysicalDimensionArray& dimensions = animationCurve->getOutPhysicalDimensions();
            if (asset->getDistanceScale() != 1.0) {
                for (int dimIndex = 0; dimIndex < numDimensions; ++dimIndex) {
                    if (dimensions[dimIndex] == COLLADAFW::PHYSICAL_DIMENSION_LENGTH) {
                        bNeedsScale = true;
                        break;
                    }
                }
            }

            switch (outputArray.getType()) {
            case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                COLLADAFW::FloatArray* inputFloats = const_cast<COLLADAFW::FloatArray*>(inputArray.getFloatValues());
                COLLADAFW::FloatArray* array = const_cast<COLLADAFW::FloatArray*>(outputArray.getFloatValues());          
                vector<double> values = vector<double>();
                float* fArray = array->getData();
                for (int index = 0; index < array->getCount(); index++) {
                    if (dimensions[index % numDimensions] == COLLADAFW::PHYSICAL_DIMENSION_LENGTH) {
                        if (bNeedsScale) {
                            fArray[index] *= (float)asset->getDistanceScale();
                        }
                    }
                    values.push_back(fArray[index]);
                    if (values.size() == numDimensions) {
                        this->addValuesAtKeyFrame(inputFloats->getData()[index / numDimensions], values);
                        values = vector<double>();
                    }
                }
                break;
            }
            case COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE: {
                COLLADAFW::FloatArray* inputFloats = const_cast<COLLADAFW::FloatArray*>(inputArray.getFloatValues());
                COLLADAFW::DoubleArray* array = const_cast<COLLADAFW::DoubleArray*>(outputArray.getDoubleValues());
                vector<double> values = vector<double>();
                double* dArray = array->getData();
                for (int index = 0; index < array->getCount(); index++) {
                    if (dimensions[index % numDimensions] == COLLADAFW::PHYSICAL_DIMENSION_LENGTH) {
                        if (bNeedsScale) {
                            dArray[index] *= asset->getDistanceScale();
                        }
                    }
                    values.push_back(dArray[index]);
                    if (values.size() == numDimensions) {
                        this->addValuesAtKeyFrame(inputFloats->getData()[index / numDimensions], values);
                        values = vector<double>();
                    }
                }
                break;
            }
            case COLLADAFW::MeshVertexData::DATA_TYPE_UNKNOWN: {
                break;
            }}
        }
    }

    GLTFAnimation::~GLTFAnimation() {}

    shared_ptr <JSONObject> GLTFAnimation::targets() {
        return this->_targets;
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
   

    void GLTFAnimation::setOriginalID(std::string originalID) {
        this->_originalID = originalID;
    }
    
    std::string GLTFAnimation::getOriginalID() {
        return this->_originalID;
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

    /**
     * Insert the keyframe and values into the value map and add the keyframe in order
     * with the others
     */
    void GLTFAnimation::addValuesAtKeyFrame(double frame, vector<double> values) {
        _outputValues[frame] = values;
        bool added = false;
        for (int i = 0; i < _keyFrames.size(); i++) {
            double keyFrame = _keyFrames[i];
            if (frame < keyFrame) {
                _keyFrames.insert(_keyFrames.begin() + i, frame);
                added = true;
                break;
            }
        }
        if (!added) {
            _keyFrames.push_back(frame);
        }
    }

    /**
     * Interpolate values for a keyframe and add them to the data
     */
    void GLTFAnimation::addInterpolatedKeyFrame(double frame) {
        if (_outputValues.count(frame)) {
            // We already have a value
            return;
        }
        else {
            int left = 0;
            int right = _keyFrames.size() - 1;
            bool changed = true;
            while(changed) {
                changed = false;
                if (left < _keyFrames.size() && _keyFrames[left + 1] < frame) {
                    left++;
                    changed = true;
                }
                if (right > 0 && _keyFrames[right - 1] > frame) {
                    right--;
                    changed = true;
                }
            }
            double start = _keyFrames[left];
            double end = _keyFrames[right];

            vector<double> startValues = _outputValues[start];
            vector<double> endValues = _outputValues[end];
            double range = end - start;
            double fraction = (frame - start) / range;

            vector<double> interpolatedValues = vector<double>();
            for (int i = 0; i < startValues.size(); i++) {
                double value = startValues[i] + (endValues[i] - startValues[i]) * fraction;
                interpolatedValues.push_back(value);
            }

            this->addValuesAtKeyFrame(frame, interpolatedValues);
        }
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

<<<<<<< HEAD
    void GLTFAnimation::writeAnimation(GLTFAsset* asset) {
        for (const std::string animationType : ANIMATION_TYPES) {
            std::shared_ptr<GLTFBufferView> bufferView = this->getBufferViewForParameter(animationType);
            if (bufferView != NULL) {
                unsigned char* data = (unsigned char*)bufferView->getBufferDataByApplyingOffset();
                std::string dataType = ANIMATION_DATA_TYPES.at(animationType);
                bool inputParameter = false;
                if (animationType != AnimationType::TIME) {
                    __AddChannel(this, _nodeId, animationType);
                }
                else {
                    inputParameter = true;
                }
                setupAndWriteAnimationParameter(this,
                    animationType,
                    dataType,
                    data,
                    this->getCount() * ANIMATION_DATA_TYPE_SIZE.at(dataType),
                    inputParameter,
                    asset);
            }
=======
    void GLTFAnimation::writeAnimationForTargetID(const std::string &targetID, GLTFAsset* asset) {
        shared_ptr <JSONObject> target =  this->targets()->getObject(targetID);
        shared_ptr<GLTFAnimationFlattener> animationFlattener = this->animationFlattenerForTargetUID(targetID);
        
        int count = 0;
        float* rotations = 0;
        float* positions = 0;
        float* scales = 0;
        
        animationFlattener->allocAndFillAffineTransformsBuffers(&positions, &rotations, &scales, count);
        
        if (animationFlattener->hasAnimatedScale()) {
            //Scale
            __AddChannel(this, targetID, "scale");
            setupAndWriteAnimationParameter(this,
                                            "scale",
                                            "FLOAT_VEC3",
                                            (unsigned char*)scales,
                                            count * sizeof(float) * 3, false,
                                            asset);
            free(scales);
        }
        
        if (animationFlattener->hasAnimatedTranslation()) {
            //Translation
            __AddChannel(this, targetID, "translation");
            setupAndWriteAnimationParameter(this,
                                            "translation",
                                            "FLOAT_VEC3",
                                            (unsigned char*)positions,
                                            count * sizeof(float) * 3, false,
                                            asset);
            free(positions);
        }
        
        if (animationFlattener->hasAnimatedRotation()) {
            //Rotation
            __AddChannel(this, targetID, "rotation");
            setupAndWriteAnimationParameter(this,
                                            "rotation",
                                            "FLOAT_VEC4",
                                            (unsigned char*)rotations,
                                            count * sizeof(float) * 4, false,
                                            asset);
            free(rotations);
>>>>>>> 899c59356a1a49e6edba131d46600ee69a10824f
        }
    }
    
    std::string GLTFAnimation::valueType() {
        return "animation";
    }
}
