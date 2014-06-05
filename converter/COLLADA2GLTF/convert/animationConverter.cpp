#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"

#include "animationConverter.h"
#include "meshConverter.h"
#include "../helpers/mathHelpers.h"

#include "GLTF-Open3DGC.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{    
    
#define ANIMATIONFLATTENER_FOR_PATH_AND_TARGETID(path, targetID) std::string targetUIDWithPath = path+targetID;\
    if (asset->_targetUIDWithPathToAnimationFlattener.count(targetUIDWithPath) > 0) {\
        animationFlattener = asset->_targetUIDWithPathToAnimationFlattener[targetUIDWithPath];\
    } else {\
        animationFlattener = cvtAnimation->animationFlattenerForTargetUID(targetID);\
        asset->_targetUIDWithPathToAnimationFlattener[targetUIDWithPath] = animationFlattener;\
    }
    
    /*
     The animation creation/write is in 2 steps.
     We first create the animation, but then, we need to set the channels for every targets, and targets ids are not available when animations are created
     */
    bool writeAnimation(shared_ptr <GLTFAnimation> cvtAnimation,
                        const COLLADAFW::AnimationList::AnimationClass animationClass,
                        AnimatedTargetsSharedPtr animatedTargets,
                        GLTF::GLTFAsset *asset) {
        
        std::string inputParameterName = "TIME";
        shared_ptr<JSONObject> samplers = cvtAnimation->samplers();
        shared_ptr<JSONArray> channels = cvtAnimation->channels();
        shared_ptr<GLTFBufferView> timeBufferView = cvtAnimation->getBufferViewForParameter(inputParameterName);
        
        shared_ptr<GLTFAnimationFlattener> animationFlattener;

        shared_ptr<GLTFBufferView> bufferView =  cvtAnimation->getBufferViewForParameter("OUTPUT");
        cvtAnimation->unregisterBufferView("OUTPUT");
        
        switch (animationClass) {
            case COLLADAFW::AnimationList::TIME:
            {
                //In Currrent OpenCOLLADA Implementation, this is never called, only cases mapping to OUTPUT are, so we handle INPUT (i.e time) when we enter this function.
            }
                break;
            case COLLADAFW::AnimationList::AXISANGLE: {
                    //the angles to radians necessary convertion is done within the animationFlattener
                    //but it might be better to make it before...
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString(kTarget);
                        if (asset->_uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
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
                return true;
            case COLLADAFW::AnimationList::MATRIX4X4: {
                    std::vector< shared_ptr <GLTFBufferView> > TRSBufferViews;
                    //FIXME: we assume float here, might be double
                    float* matrices = (float*)bufferView->getBufferDataByApplyingOffset();
                    float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        if (animatedTarget->getString("path") == "MATRIX") {
                            std::string targetID = animatedTarget->getString(kTarget);
                            if (asset->_uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
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
                } 
                return true;
            case COLLADAFW::AnimationList::POSITION_XYZ: {
                    //the angles to radians necessary convertion is done within the animationFlattener
                    //but it might be better to make it before...
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString(kTarget);
                        if (asset->_uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
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
                
return true;
            case COLLADAFW::AnimationList::ANGLE: {
                //the angles to radians necessary convertion is done within the animationFlattener
                //but it might be better to make it before...
                for (size_t animatedTargetIndex = 0; animatedTargetIndex < animatedTargets->size(); animatedTargetIndex++) {
                    shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                    std::string targetID = animatedTarget->getString(kTarget);
                    if (asset->_uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                        cvtAnimation->targets()->setValue(targetID, animatedTarget);

                        std::string path = animatedTarget->getString("path");
                        if (path == "rotation") {
                            std::string transformID = animatedTarget->getString("transformId");
                            ANIMATIONFLATTENER_FOR_PATH_AND_TARGETID(path, targetID);

                            float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                            float* rotations = (float*)bufferView->getBufferDataByApplyingOffset();
                            for (size_t k = 0; k < cvtAnimation->getCount(); k++) {
                                animationFlattener->insertValueAtTime(transformID, rotations[k], 3, timeValues[k]);
                            }
                        }
                    }
                }
            }
return true;
            case COLLADAFW::AnimationList::POSITION_X:
            case COLLADAFW::AnimationList::POSITION_Y:
            case COLLADAFW::AnimationList::POSITION_Z:
            {
                int index = animationClass - COLLADAFW::AnimationList::POSITION_X;
                for (size_t animatedTargetIndex = 0; animatedTargetIndex < animatedTargets->size(); animatedTargetIndex++) {
                    shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                    std::string targetID = animatedTarget->getString(kTarget);
                    if (asset->_uniqueIDToOpenCOLLADAObject.count(targetID) != 0) {
                        cvtAnimation->targets()->setValue(targetID, animatedTarget);
                        std::string path = animatedTarget->getString("path");
                        std::string transformID = animatedTarget->getString("transformId");

                        ANIMATIONFLATTENER_FOR_PATH_AND_TARGETID(path, targetID);

                        float* timeValues = (float*)timeBufferView->getBufferDataByApplyingOffset();
                        float* values = (float*)bufferView->getBufferDataByApplyingOffset();
                        for (size_t k = 0; k < cvtAnimation->getCount(); k++) {
                            animationFlattener->insertValueAtTime(transformID, values[k], index, timeValues[k]);
                        }
                    }
                }
                return true;
            }
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

    shared_ptr <GLTFAnimation> convertOpenCOLLADAAnimationToGLTFAnimation(const COLLADAFW::Animation* animation, GLTF::GLTFAsset *asset, double distanceScale)
    {
        shared_ptr <GLTFAnimation> cvtAnimation(new GLTFAnimation());
        if (animation->getAnimationType() == COLLADAFW::Animation::ANIMATION_CURVE) {
            shared_ptr <JSONObject> animationParameters = cvtAnimation->parameters();

            const COLLADAFW::AnimationCurve *animationCurve = (const COLLADAFW::AnimationCurve*)animation;

            //This needs to be fixed when re-working: https://github.com/KhronosGroup/glTF/issues/158
            //especially, this point: "by default the converter should replicate COLLADA animations layout (not yet done), but an option should allow to have one animation per target. (this is actually the case)."

            std::string animationID = uniqueIdWithType(kAnimation, animation->getUniqueId());

            cvtAnimation->setID(animationID);

            cvtAnimation->setCount(animationCurve->getKeyCount());

            /** Returns the input values of the animation. */
            const COLLADAFW::FloatOrDoubleArray &inputArray = animationCurve->getInputValues();
            const COLLADAFW::FloatOrDoubleArray &outputArray = animationCurve->getOutputValues();

            // Scales any distance values if needed
            if (distanceScale != 1.0)
            {
                bool bNeedsScale = false;
                const COLLADAFW::PhysicalDimensionArray& dimensions = animationCurve->getOutPhysicalDimensions();
                size_t numDimensions = animationCurve->getOutDimension();
                for (size_t dimIndex = 0; dimIndex < numDimensions; ++dimIndex)
                {
                    if (dimensions[dimIndex] == COLLADAFW::PHYSICAL_DIMENSION_LENGTH)
                    {
                        bNeedsScale = true;
                        break;
                    }
                }

                if (bNeedsScale)
                {
                    switch (outputArray.getType()) {
                    case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                        COLLADAFW::FloatArray* array = const_cast<COLLADAFW::FloatArray*>(outputArray.getFloatValues());
                        float* fArray = array->getData();
                        for (size_t index = 0; index < array->getCount(); ++index)
                        {
                            if (dimensions[index % numDimensions] == COLLADAFW::PHYSICAL_DIMENSION_LENGTH)
                            {
                                fArray[index] *= (float)distanceScale;
                            }
                        }
                    }
                        break;
                    case COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE: {
                        COLLADAFW::DoubleArray* array = const_cast<COLLADAFW::DoubleArray*>(outputArray.getDoubleValues());
                        double* dArray = array->getData();
                        for (size_t index = 0; index < array->getCount(); ++index)
                        {
                            if (dimensions[index % numDimensions] == COLLADAFW::PHYSICAL_DIMENSION_LENGTH)
                            {
                                dArray[index] *= distanceScale;
                            }
                        }
                    }
                        break;
                    default:
                    case COLLADAFW::MeshVertexData::DATA_TYPE_UNKNOWN:
                        //FIXME report error
                        break;
                    };
                }
            }
            
            const std::string originalID = animationCurve->getOriginalId();
            
            //shared_ptr <JSONObject> inputParameter(new JSONObject());
            shared_ptr <GLTFBufferView> inputBufferView = convertFloatOrDoubleArrayToGLTFBufferView(inputArray);
            shared_ptr <GLTFBufferView> outputBufferView = convertFloatOrDoubleArrayToGLTFBufferView(outputArray);
            
            cvtAnimation->registerBufferView("TIME", inputBufferView);
            cvtAnimation->registerBufferView("OUTPUT", outputBufferView);            
        }
        return cvtAnimation;
    }
}
