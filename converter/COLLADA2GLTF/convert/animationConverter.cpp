#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "../GLTFConverterContext.h"

#include "animationConverter.h"
#include "meshConverter.h"
#include "../helpers/mathHelpers.h"

namespace GLTF
{
    static void __DecomposeMatrices(float *matrices, size_t count,
                                    std::vector< shared_ptr <GLTFBufferView> > &TRSBufferViews) {
        
        size_t translationBufferSize = sizeof(float) * 3 * count;
        size_t rotationBufferSize = sizeof(float) * 4 * count;
        size_t scaleBufferSize = sizeof(float) * 3 * count;
        
        float *translationData = (float*)malloc(translationBufferSize);
        float *rotationData = (float*)malloc(rotationBufferSize);
        float *scaleData = (float*)malloc(scaleBufferSize);
        
        shared_ptr <GLTF::GLTFBufferView> translationBufferView = createBufferViewWithAllocatedBuffer(translationData, 0, translationBufferSize, true);
        shared_ptr <GLTF::GLTFBufferView> rotationBufferView = createBufferViewWithAllocatedBuffer(rotationData, 0, rotationBufferSize, true);
        shared_ptr <GLTF::GLTFBufferView> scaleBufferView = createBufferViewWithAllocatedBuffer(scaleData, 0, scaleBufferSize, true);
        
        float *previousRotation = 0;
        
        for (size_t i = 0 ; i < count ; i++) {
            
            float *m = matrices;
            COLLADABU::Math::Matrix4 mat;
            mat.setAllElements(m[0], m[1], m[2], m[3],
                               m[4], m[5], m[6], m[7],
                               m[8], m[9], m[10], m[11],
                               m[12], m[13], m[14], m[15] );
            
            decomposeMatrix(mat, translationData, rotationData, scaleData);
            
            //make sure we export the short path from orientations
            if (0 != previousRotation) {
                COLLADABU::Math::Vector3 axis1(previousRotation[0], previousRotation[1], previousRotation[2]);
                COLLADABU::Math::Vector3 axis2(rotationData[0], rotationData[1], rotationData[2]);
                
                COLLADABU::Math::Quaternion key1;
                COLLADABU::Math::Quaternion key2;
                
                key1.fromAngleAxis(previousRotation[3], axis1);
                key2.fromAngleAxis(rotationData[3], axis2);
                
                COLLADABU::Math::Real cosHalfTheta = key1.dot(key2);
                if (cosHalfTheta < 0) {
                    key2.x = -key2.x;
                    key2.y = -key2.y;
                    key2.z = -key2.z;
                    key2.w = -key2.w;
                    
                    COLLADABU::Math::Real angle;
                    key2.toAngleAxis(angle, axis2);
                    rotationData[3] = angle;
                    rotationData[0] = axis2.x;
                    rotationData[1] = axis2.y;
                    rotationData[2] = axis2.z;
                    
                    key2.fromAngleAxis(rotationData[3], axis2);
                    
                    //FIXME: this needs to be refined, we ensure continuity here, but assume in clockwise order
                    cosHalfTheta = key1.dot(key2);
                    if (cosHalfTheta < 0) {
                        rotationData[3] += (2 * 3.14159265359);
                        key2.fromAngleAxis(rotationData[3], axis2);
                    }
                }
                
            }
            
            previousRotation = rotationData;
            translationData += 3;
            rotationData += 4;
            scaleData += 3;
            matrices += 16;
        }
        /*
         rotationData = (float*)rotationBufferView->getBufferDataByApplyingOffset();
         for (size_t i = 0 ; i < count ; i++) {
         printf("rotation at:%d %f %f %f %f\n",  i,
         rotationData[0],rotationData[1],rotationData[2],rotationData[3]);
         
         rotationData += 4;
         }
         */
        TRSBufferViews.push_back(translationBufferView);
        TRSBufferViews.push_back(rotationBufferView);
        TRSBufferViews.push_back(scaleBufferView);
    }
    
    static std::string __SetupSamplerForParameter(shared_ptr <GLTFAnimation> cvtAnimation,
                                                  GLTFAnimation::Parameter *parameter) {
        shared_ptr<JSONObject> sampler(new JSONObject());
        std::string name = parameter->getID();
        std::string samplerID = cvtAnimation->getSamplerIDForName(name);
        sampler->setString("input", "TIME");           //FIXME:harcoded for now
        sampler->setString("interpolation", "LINEAR"); //FIXME:harcoded for now
        sampler->setString("output", name);
        cvtAnimation->samplers()->setValue(samplerID, sampler);
        
        return samplerID;
    }
    
    
    /*
     Handles Channel creation + additions
     */
    static void __AddChannel(shared_ptr <GLTFAnimation> cvtAnimation,
                             const std::string &targetID,
                             const std::string &path) {
        shared_ptr<JSONObject> trChannel(new JSONObject());
        shared_ptr<JSONObject> trTarget(new JSONObject());
        
        trChannel->setString("sampler", cvtAnimation->getSamplerIDForName(path));
        trChannel->setValue("target", trTarget);
        trTarget->setString("id", targetID);
        trTarget->setString("path", path);
        cvtAnimation->channels()->appendValue(trChannel);
    }
    
    /*
     Handles Parameter creation / addition
     */
    static shared_ptr <GLTFAnimation::Parameter> __SetupAnimationParameter(shared_ptr <GLTFAnimation> cvtAnimation,
                                                  const std::string& parameterSID,
                                                  const std::string& parameterType) {
        //setup
        shared_ptr <GLTFAnimation::Parameter> parameter(new GLTFAnimation::Parameter(parameterSID));
        parameter->setCount(cvtAnimation->getCount());
        parameter->setType(parameterType);
        __SetupSamplerForParameter(cvtAnimation, parameter.get());
        
        cvtAnimation->parameters()->push_back(parameter);
        
        return parameter;
    }
    
    /*
     Handles Parameter creation / addition / write
     */
    static void __SetupAndWriteAnimationParameter(shared_ptr <GLTFAnimation> cvtAnimation,
                                                  const std::string& parameterSID,
                                                  const std::string& parameterType,
                                                  shared_ptr <GLTFBufferView> bufferView,
                                                  std::ofstream &animationsOutputStream) {
        //setup
        shared_ptr <GLTFAnimation::Parameter> parameter = __SetupAnimationParameter(cvtAnimation, parameterSID, parameterType);
        
        //write
        parameter->setByteOffset(static_cast<size_t>(animationsOutputStream.tellp()));
        animationsOutputStream.write((const char*)( bufferView->getBufferDataByApplyingOffset()),
                                     bufferView->getByteLength());
    }
    
    /*
     The animation creation/write is in 2 steps.
     We first create the animation, but then, we need to set the channels for every targets, and targets ids are not available when animations are created
     */
    
    
    bool writeAnimation(shared_ptr <GLTFAnimation> cvtAnimation,
                        const COLLADAFW::AnimationList::AnimationClass animationClass,
                        AnimatedTargetsSharedPtr animatedTargets,
                        std::ofstream &animationsOutputStream,
                        GLTF::GLTFConverterContext &converterContext) {
        
        shared_ptr<JSONObject> samplers = cvtAnimation->samplers();
        shared_ptr<JSONArray> channels = cvtAnimation->channels();
        GLTFAnimation::Parameter *timeParameter = cvtAnimation->getParameterNamed("TIME");
        shared_ptr<GLTFBufferView> timeBufferView = timeParameter->getBufferView();
        std::string name = "TIME";
        std::string samplerID = cvtAnimation->getSamplerIDForName(name);
        
        timeParameter->setByteOffset(static_cast<size_t>(animationsOutputStream.tellp()));
        animationsOutputStream.write((const char*)( timeBufferView->getBufferDataByApplyingOffset()),
                                     timeBufferView->getByteLength());
        
            //printf("time bufferLength: %d\n",(int)timeBufferView->getByteLength());
        
        switch (animationClass) {
            case COLLADAFW::AnimationList::TIME:
            {
                //In Currrent COLLADA Implementation, this is never called, only cases mapping to OUTPUT are, so we handle INPUT (i.e time) when we enter this function.
            }
                break;
            case COLLADAFW::AnimationList::AXISANGLE:
                break;
            case COLLADAFW::AnimationList::MATRIX4X4: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    std::vector< shared_ptr <GLTFBufferView> > TRSBufferViews;
                    //FIXME: we assume float here, might be double
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    float* matrices = (float*)bufferView->getBufferDataByApplyingOffset();
                    __DecomposeMatrices(matrices, cvtAnimation->getCount(), TRSBufferViews);
                    cvtAnimation->removeParameterNamed("OUTPUT");
                    
                    //Translation
                    __SetupAndWriteAnimationParameter(cvtAnimation,
                                                      "translation",
                                                      "FLOAT_VEC3",
                                                      TRSBufferViews[0],
                                                      animationsOutputStream);
                    
                    //Rotation
                    __SetupAndWriteAnimationParameter(cvtAnimation,
                                                      "rotation",
                                                      "FLOAT_VEC4",
                                                      TRSBufferViews[1],
                                                      animationsOutputStream);
                    
                    //Scale
                    __SetupAndWriteAnimationParameter(cvtAnimation,
                                                      "scale",
                                                      "FLOAT_VEC3",
                                                      TRSBufferViews[2],
                                                      animationsOutputStream);
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        if (animatedTarget->getString("path") == "MATRIX") {
                            std::string targetID = animatedTarget->getString("target");
                            __AddChannel(cvtAnimation, targetID, "translation");
                            __AddChannel(cvtAnimation, targetID, "rotation");
                            __AddChannel(cvtAnimation, targetID, "scale");
                        }
                    }
                    
                } else {
                    //FIXME: report error
                    printf("WARNING: cannot find intermediate parameter named OUTPUT\n");
                }
            }
                return true;
                break;
            case COLLADAFW::AnimationList::POSITION_XYZ: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    
                    __SetupAndWriteAnimationParameter(cvtAnimation,
                                                      "translation",
                                                      "FLOAT_VEC3",
                                                      bufferView,
                                                      animationsOutputStream);
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        
                        if (animatedTarget->getString("path") == "translation") {
                            std::string targetID = animatedTarget->getString("target");
                            __AddChannel(cvtAnimation, targetID, "translation");
                        }
                    }
                    cvtAnimation->removeParameterNamed("OUTPUT");
                }
            }
                
                return true;
            case COLLADAFW::AnimationList::ANGLE: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    //Convert angles to radians
                    //float *angles = (float*)bufferView->getBufferDataByApplyingOffset();
                    //for (size_t i = 0 ; i < keyCount ; i++) {
                    //    angles[i] = angles[i] * 0.0174532925; //to radians.
                    //}
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        if (converterContext._uniqueIDToTrackedObject.count(targetID) != 0) {
                            cvtAnimation->targets()->setValue(targetID, animatedTarget);
                            
                            shared_ptr<JSONObject> targetObject = converterContext._uniqueIDToTrackedObject[targetID];
                            std::string path = animatedTarget->getString("path");
                            if (path == "rotation") {
                                std::string transformID = animatedTarget->getString("transformId");
                                shared_ptr<GLTFAnimationFlattener> animationFlattener = converterContext._uniqueIDToAnimationFlattener[targetID];
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* rotations = (float*)bufferView->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    animationFlattener->insertValueAtTime(transformID, rotations[k], 3, timeValues[k]);
                                }
                                
                                /*
                                shared_ptr<JSONArray> rotationArray = static_pointer_cast <JSONArray>(targetObject->getValue(path));
                                shared_ptr<GLTFBufferView> adjustedBuffer = __CreateBufferViewByReplicatingArrayAndReplacingValueAtIndex(bufferView, rotationArray, 3, "FLOAT", cvtAnimation->getCount());
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* rotations = (float*)adjustedBuffer->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    size_t offset = k * 4;
                                    shared_ptr <COLLADAFW::Rotate> rotate(new COLLADAFW::Rotate(rotations[offset + 0],
                                                             rotations[offset + 1],
                                                             rotations[offset + 2],
                                                             rotations[offset + 3]));
                                    animationFlattener->insertTransformAtTime(transformID, rotate, timeValues[k]);
                                }
                                */
                                
                                /*
                                __SetupAndWriteAnimationParameter(cvtAnimation,
                                                                  "rotation",
                                                                  "FLOAT_VEC4",
                                                                  adjustedBuffer,
                                                                  animationsOutputStream);
                                
                                __AddChannel(cvtAnimation, targetID, path);
                                 */
                            }
                        }
                    }
                }
                cvtAnimation->removeParameterNamed("OUTPUT");
            }
                return true;
            case COLLADAFW::AnimationList::POSITION_X:
            case COLLADAFW::AnimationList::POSITION_Y:
            case COLLADAFW::AnimationList::POSITION_Z:
            {
                int index = animationClass - COLLADAFW::AnimationList::POSITION_X;
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        
                        if (converterContext._uniqueIDToTrackedObject.count(targetID) != 0) {
                            cvtAnimation->targets()->setValue(targetID, animatedTarget);
                            shared_ptr<JSONObject> targetObject = converterContext._uniqueIDToTrackedObject[targetID];
                            std::string path = animatedTarget->getString("path");
                            if (path == "translation") {
                                std::string transformID = animatedTarget->getString("transformId");
                                shared_ptr<GLTFAnimationFlattener> animationFlattener = converterContext._uniqueIDToAnimationFlattener[targetID];
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* translations = (float*)bufferView->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    animationFlattener->insertValueAtTime(transformID, translations[k], index, timeValues[k]);
                                }

                                /*
                                shared_ptr<JSONArray> translationArray = static_pointer_cast <JSONArray>(targetObject->getValue(path));
                                shared_ptr<GLTFBufferView> adjustedBuffer = __CreateBufferViewByReplicatingArrayAndReplacingValueAtIndex(bufferView, translationArray, index, "FLOAT", cvtAnimation->getCount());
                                
                                std::string transformID = animatedTarget->getString("transformId");
                                
                                shared_ptr<GLTFAnimationFlattener> animationFlattener = converterContext._uniqueIDToAnimationFlattener[targetID];
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* translations = (float*)adjustedBuffer->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    size_t offset = k * 3;
                                    shared_ptr <COLLADAFW::Translate> translate(new COLLADAFW::Translate(translations[offset + 0],
                                                                                                translations[offset + 1],
                                                                                                translations[offset + 2]));
                                    animationFlattener->insertTransformAtTime(transformID, translate, timeValues[k]);
                                }
                                */
                                
                                /*
                                __SetupAndWriteAnimationParameter(cvtAnimation,
                                                                  path,
                                                                  "FLOAT_VEC3",
                                                                  adjustedBuffer,
                                                                  animationsOutputStream);
                                
                                __AddChannel(cvtAnimation, targetID, path);
                                 */
                            }
                        }
                    }
                }
                cvtAnimation->removeParameterNamed("OUTPUT");
            }
                return true;
                
                
            case COLLADAFW::AnimationList::COLOR_RGB:
            case COLLADAFW::AnimationList::COLOR_RGBA:
            case COLLADAFW::AnimationList::COLOR_R:
            case COLLADAFW::AnimationList::COLOR_G:
            case COLLADAFW::AnimationList::COLOR_B:
            case COLLADAFW::AnimationList::COLOR_A:
            case COLLADAFW::AnimationList::ARRAY_ELEMENT_1D:
            case COLLADAFW::AnimationList::ARRAY_ELEMENT_2D:
            case COLLADAFW::AnimationList::FLOAT:
            default:
                break;
        }
        
        return false;
    }
    
    shared_ptr <GLTFAnimation> convertOpenCOLLADAAnimationToGLTFAnimation(const COLLADAFW::Animation* animation)
    {
        shared_ptr <GLTFAnimation> cvtAnimation(new GLTFAnimation());
        if (animation->getAnimationType() == COLLADAFW::Animation::ANIMATION_CURVE) {
            std::vector <shared_ptr <GLTFAnimation::Parameter> >* animationParameters = cvtAnimation->parameters();
            
            const COLLADAFW::AnimationCurve* animationCurve = (const COLLADAFW::AnimationCurve*)animation;
            
            std::string animationID = animation->getOriginalId();
            if (animationID.size() == 0) {
                animationID = uniqueIdWithType("animation", animation->getUniqueId());
            }
            cvtAnimation->setID(animationID);
            
            cvtAnimation->setCount(animationCurve->getKeyCount());
            
            /** Returns the input values of the animation. */
            const COLLADAFW::FloatOrDoubleArray &inputArray =  animationCurve->getInputValues();
            const COLLADAFW::FloatOrDoubleArray &outputArray =  animationCurve->getOutputValues();
            
            const std::string originalID = animationCurve->getOriginalId();
            
            shared_ptr <GLTFBufferView> inputBufferView = convertFloatOrDoubleArrayToGLTFBufferView(inputArray);
            shared_ptr <GLTFBufferView> outputBufferView = convertFloatOrDoubleArrayToGLTFBufferView(outputArray);
            
            //build up input parameter, typically TIME
            shared_ptr <GLTFAnimation::Parameter> inputParameter(new GLTFAnimation::Parameter("TIME"));
            
            inputParameter->setType("FLOAT");
            inputParameter->setBufferView(inputBufferView);
            inputParameter->setByteOffset(0);
            inputParameter->setCount(cvtAnimation->getCount());
            
            animationParameters->push_back(inputParameter);
            
            //build up output parameter
            shared_ptr <GLTFAnimation::Parameter> outputParameter(new GLTFAnimation::Parameter("OUTPUT"));
            
            outputParameter->setBufferView(outputBufferView);
            outputParameter->setByteOffset(0);
            
            animationParameters->push_back(outputParameter);
        }
        
        return cvtAnimation;
    }
    
    
}
