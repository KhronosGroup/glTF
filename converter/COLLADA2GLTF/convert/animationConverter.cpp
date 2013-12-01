#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "../GLTFConverterContext.h"

#include "animationConverter.h"
#include "meshConverter.h"
#include "../helpers/mathHelpers.h"

#include "GLTF-Open3DGC.h"

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
                    rotationData[3] = (float)angle;
                    rotationData[0] = (float)axis2.x;
                    rotationData[1] = (float)axis2.y;
                    rotationData[2] = (float)axis2.z;
                    
                    key2.fromAngleAxis(rotationData[3], axis2);
                    
                    //FIXME: this needs to be refined, we ensure continuity here, but assume in clockwise order
                    cosHalfTheta = key1.dot(key2);
                    if (cosHalfTheta < 0) {
                        rotationData[3] += (float)(2. * 3.14159265359);
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
    
    
#define ANIMATIONFLATTENER_FOR_PATH_AND_TARGETID(path, targetID) std::string targetUIDWithPath = path+targetID;\
    if (converterContext._targetUIDWithPathToAnimationFlattener.count(targetUIDWithPath) > 0) {\
        animationFlattener = converterContext._targetUIDWithPathToAnimationFlattener[targetUIDWithPath];\
    } else {\
        animationFlattener = cvtAnimation->animationFlattenerForTargetUID(targetID);\
        converterContext._targetUIDWithPathToAnimationFlattener[targetUIDWithPath] = animationFlattener;\
    }
    
    /*
     The animation creation/write is in 2 steps.
     We first create the animation, but then, we need to set the channels for every targets, and targets ids are not available when animations are created
     */
    bool writeAnimation(shared_ptr <GLTFAnimation> cvtAnimation,
                        const COLLADAFW::AnimationList::AnimationClass animationClass,
                        AnimatedTargetsSharedPtr animatedTargets,
                        GLTF::GLTFConverterContext &converterContext) {
        
        shared_ptr<JSONObject> samplers = cvtAnimation->samplers();
        shared_ptr<JSONArray> channels = cvtAnimation->channels();
        GLTFAnimation::Parameter *timeParameter = cvtAnimation->getParameterNamed("TIME");
        shared_ptr<GLTFBufferView> timeBufferView = timeParameter->getBufferView();
        std::string name = "TIME";
        std::string samplerID = cvtAnimation->getSamplerIDForName(name);
                
        cvtAnimation->removeParameterNamed("TIME");
        
        setupAndWriteAnimationParameter(cvtAnimation,
                                             "TIME",
                                             "FLOAT",
                                             (unsigned char*)timeBufferView->getBufferDataByApplyingOffset(), timeBufferView->getByteLength(), true,
                                             converterContext);
        
        shared_ptr<GLTFAnimationFlattener> animationFlattener;

        //timeParameter->setByteOffset(outputStream->length());
        //outputStream->write(timeBufferView);
        //printf("time bufferLength: %d\n",(int)timeBufferView->getByteLength());
        
        switch (animationClass) {
            case COLLADAFW::AnimationList::TIME:
            {
                //In Currrent OpenCOLLADA Implementation, this is never called, only cases mapping to OUTPUT are, so we handle INPUT (i.e time) when we enter this function.
            }
                break;
            case COLLADAFW::AnimationList::AXISANGLE: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    //the angles to radians necessary convertion is done within the animationFlattener
                    //but it might be better to make it before...
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        if (converterContext._uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                            cvtAnimation->targets()->setValue(targetID, animatedTarget);
                            
                            std::string path = animatedTarget->getString("path");
                            if (path == "rotation") {
                                std::string transformID = animatedTarget->getString("transformId");
                                animationFlattener = cvtAnimation->animationFlattenerForTargetUID(targetID);
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* rotations = (float*)bufferView->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    size_t offset = k * 4;
                                    
                                    shared_ptr <COLLADAFW::Rotate> rotate(new COLLADAFW::Rotate(rotations[offset + 0],
                                                                                                rotations[offset + 1],
                                                                                                rotations[offset + 2],
                                                                                                rotations[offset + 3]));
                                    animationFlattener->insertTransformAtTime(transformID, rotate, timeValues[k]);
                                }
                            }
                        }
                    }
                }
                cvtAnimation->removeParameterNamed("OUTPUT");
            }
                return true;
            case COLLADAFW::AnimationList::MATRIX4X4: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    std::vector< shared_ptr <GLTFBufferView> > TRSBufferViews;
                    //FIXME: we assume float here, might be double
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    float* matrices = (float*)bufferView->getBufferDataByApplyingOffset();
                    float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        if (animatedTarget->getString("path") == "MATRIX") {
                            std::string targetID = animatedTarget->getString("target");
                            if (converterContext._uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                                cvtAnimation->targets()->setValue(targetID, animatedTarget);
                                
                                std::string transformID = animatedTarget->getString("transformId");
                                animationFlattener = cvtAnimation->animationFlattenerForTargetUID(targetID);
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    size_t offset = k * 16;
                                    float *m = matrices + offset;
                                    
                                    COLLADABU::Math::Matrix4 mat;
                                    mat.setAllElements(m[0], m[1], m[2], m[3],
                                                       m[4], m[5], m[6], m[7],
                                                       m[8], m[9], m[10], m[11],
                                                       m[12], m[13], m[14], m[15] );
                                    
                                    shared_ptr <COLLADAFW::Matrix> matTr(new COLLADAFW::Matrix(mat));
                                    animationFlattener->insertTransformAtTime(transformID, matTr, timeValues[k]);
                                }
                            }
                        }
                    }
                    cvtAnimation->removeParameterNamed("OUTPUT");

                } else {
                    //FIXME: report error
                    printf("WARNING: cannot find intermediate parameter named OUTPUT\n");
                }
            }
                return true;
            case COLLADAFW::AnimationList::POSITION_XYZ: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    //the angles to radians necessary convertion is done within the animationFlattener
                    //but it might be better to make it before...
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        if (converterContext._uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                            cvtAnimation->targets()->setValue(targetID, animatedTarget);
                            
                            std::string path = animatedTarget->getString("path");
                            if (path == "translation") {
                                std::string transformID = animatedTarget->getString("transformId");
                                animationFlattener = cvtAnimation->animationFlattenerForTargetUID(targetID);
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* translations = (float*)bufferView->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    size_t offset = k * 3;
                                    shared_ptr <COLLADAFW::Translate> translate(new COLLADAFW::Translate(translations[offset + 0],
                                                                                                         translations[offset + 1],
                                                                                                         translations[offset + 2]));
                                    animationFlattener->insertTransformAtTime(transformID, translate, timeValues[k]);
                                }
                            } else if (path == "scale") {
                                std::string transformID = animatedTarget->getString("transformId");
                                animationFlattener = cvtAnimation->animationFlattenerForTargetUID(targetID);
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* scales = (float*)bufferView->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    size_t offset = k * 3;
                                    shared_ptr <COLLADAFW::Scale> scale(new COLLADAFW::Scale(scales[offset + 0],
                                                                                             scales[offset + 1],
                                                                                             scales[offset + 2]));
                                    animationFlattener->insertTransformAtTime(transformID, scale, timeValues[k]);
                                }
                            }
                        }
                    }
                }
                cvtAnimation->removeParameterNamed("OUTPUT");
            }
                
                return true;
            case COLLADAFW::AnimationList::ANGLE: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    //the angles to radians necessary convertion is done within the animationFlattener
                    //but it might be better to make it before...
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        if (converterContext._uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                            cvtAnimation->targets()->setValue(targetID, animatedTarget);
                            
                            std::string path = animatedTarget->getString("path");
                            if (path == "rotation") {
                                std::string transformID = animatedTarget->getString("transformId");
                                ANIMATIONFLATTENER_FOR_PATH_AND_TARGETID(path, targetID);
                                
                                float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                                float* rotations = (float*)bufferView->getBufferDataByApplyingOffset();
                                for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                    animationFlattener->insertValueAtTime(transformID, rotations[k], 3, timeValues[k]);
                                }
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
                        if (converterContext._uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                            cvtAnimation->targets()->setValue(targetID, animatedTarget);
                            std::string path = animatedTarget->getString("path");
                            std::string transformID = animatedTarget->getString("transformId");
                            
                            ANIMATIONFLATTENER_FOR_PATH_AND_TARGETID(path, targetID);
                            
                            float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                            float* values = (float*)bufferView->getBufferDataByApplyingOffset();
                            for (size_t k = 0 ; k < cvtAnimation->getCount() ; k++) {
                                animationFlattener->insertValueAtTime(transformID, values[k], index, timeValues[k]);
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
                static bool printedOnce = false;
                if (!printedOnce) {
                    printf("WARNING: unhandled transform type\n");
                    printedOnce = true;
                }
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
            
            //This needs to be fixed when re-working: https://github.com/KhronosGroup/glTF/issues/158
            //especially, this point: "by default the converter should replicate COLLADA animations layout (not yet done), but an option should allow to have one animation per target. (this is actually the case)."

            std::string animationID = uniqueIdWithType("animation", animation->getUniqueId());
            
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
