#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "../GLTFConverterContext.h"

#include "animationConverter.h"
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
        Convert an OpenCOLLADA's FloatOrDoubleArray type to a GLTFBufferView
        Note: the resulting GLTFBufferView is not typed, it's the call responsability to keep track of the type if needed.
     */
    static shared_ptr <GLTFBufferView> __ConvertFloatOrDoubleArrayToGLTFBufferView(const COLLADAFW::FloatOrDoubleArray &floatOrDoubleArray) {
        unsigned char* sourceData = 0;
        size_t sourceSize = 0;
        
        switch (floatOrDoubleArray.getType()) {
            case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                const COLLADAFW::FloatArray* array = floatOrDoubleArray.getFloatValues();
                
                sourceData = (unsigned char*)array->getData();
                sourceSize = array->getCount() * sizeof(float);
            }
                break;
            case COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE: {
                const COLLADAFW::DoubleArray* array = floatOrDoubleArray.getDoubleValues();
                
                sourceData = (unsigned char*)array->getData();
                sourceSize = array->getCount() * sizeof(double);
            }
                break;
            default:
            case COLLADAFW::MeshVertexData::DATA_TYPE_UNKNOWN:
                //FIXME report error
                break;
        }
        unsigned char* copiedData = (unsigned char*)malloc(sourceSize);
        memcpy(copiedData, sourceData, sourceSize);
        
        shared_ptr <GLTF::GLTFBufferView> bufferView = createBufferViewWithAllocatedBuffer(copiedData, 0, sourceSize, true);
        
        return bufferView;
    }
    
    /*
        Since GLTF does not have the same granularity as COLLADA, we need in some situations to duplicate a few datas.
        For instance a target path like position.x is not supported by glTF, just the 3 components (x,y,z) can be animated.
        This function create a buffer that will allow to handle a target path supported by GLTF.
     */
    static shared_ptr<GLTFBufferView> __CreateBufferViewByReplicatingArrayAndReplacingValueAtIndex(shared_ptr<GLTFBufferView> bufferView,
                                                                                                   shared_ptr<JSONArray> array,
                                                                                                   size_t index,
                                                                                                   std::string type,
                                                                                                   size_t keyCount) {
        
        char *destinationBuffer = 0;
        char *sourceBuffer = (char*)bufferView->getBufferDataByApplyingOffset();
        size_t elementSize = 0;
        size_t offset = 0;
        //TODO handle other types
        if (type == "FLOAT") {
            elementSize = sizeof(float);
            offset = (elementSize * array->values().size());
        }
        
        size_t destinationBufferLength = offset * keyCount;
        
        if (elementSize != 0) {
            //FIXME: should not assume FLOAT here
            size_t count = array->values().size();
            float *values = (float*)malloc(elementSize * count);
            for (size_t i = 0 ; i < count ; i++) {
                shared_ptr <JSONNumber> nb = static_pointer_cast<JSONNumber>(array->values()[i]);
                values[i] = (float)nb->getDouble();
            }
            
            destinationBuffer = (char*)malloc(destinationBufferLength);
            for (size_t i = 0 ; i < keyCount ; i++) {
                memcpy(destinationBuffer + (offset * i), values, offset);
                memcpy(destinationBuffer + (offset * i) + (index * elementSize) ,
                       sourceBuffer + (i * elementSize) ,
                       elementSize);
            }
            
            free(values);
            
        } else {
            //TODO:..
            printf("WARNING attempt to use __CreateBufferViewByReplicatingArrayAndReplacingValueAtIndex without using floats\n");
        }
        
        return  createBufferViewWithAllocatedBuffer(destinationBuffer, 0, destinationBufferLength, true);
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
        Handles Parameter creation / addition / write
     */
    static void __SetupAndWriteAnimationParameter(shared_ptr <GLTFAnimation> cvtAnimation,
                                                  const std::string& parameterSID,
                                                  const std::string& parameterType,
                                                  shared_ptr <GLTFBufferView> bufferView,
                                                  std::ofstream &animationsOutputStream) {
        //setup
        shared_ptr <GLTFAnimation::Parameter> parameter(new GLTFAnimation::Parameter(parameterSID));
        parameter->setCount(cvtAnimation->getCount());
        parameter->setType(parameterType);
        __SetupSamplerForParameter(cvtAnimation, parameter.get());
        
        cvtAnimation->parameters()->push_back(parameter);
        
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
        
        
        
        std::string samplerID;
        std::string name;
        shared_ptr<JSONObject> samplers = cvtAnimation->samplers();
        shared_ptr<JSONArray> channels = cvtAnimation->channels();
        size_t keyCount = cvtAnimation->getCount();
        GLTFAnimation::Parameter *timeParameter = cvtAnimation->getParameterNamed("TIME");
        if (timeParameter) {
            shared_ptr<GLTFBufferView> timeBufferView = timeParameter->getBufferView();
            std::string name = "TIME";
            std::string samplerID = cvtAnimation->getSamplerIDForName(name);
            
            timeParameter->setByteOffset(static_cast<size_t>(animationsOutputStream.tellp()));
            animationsOutputStream.write((const char*)( timeBufferView->getBufferDataByApplyingOffset()),
                                         timeBufferView->getByteLength());
            
            //printf("time bufferLength: %d\n",(int)timeBufferView->getByteLength());
        }
        
        switch (animationClass) {
            case COLLADAFW::AnimationList::TIME:
            {
                //In Currrent COLLADA Implementation, this is never called, only cases mapping to OUTPUT are, so we handle INPUT when we enter this function.
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
                break;
            case COLLADAFW::AnimationList::ANGLE: {
                GLTFAnimation::Parameter *parameter = cvtAnimation->getParameterNamed("OUTPUT");
                if (parameter) {
                    shared_ptr<GLTFBufferView> bufferView = parameter->getBufferView();
                    //Convert angles to radians
                    float *angles = (float*)bufferView->getBufferDataByApplyingOffset();
                    for (size_t i = 0 ; i < keyCount ; i++) {
                        angles[i] = angles[i] * 0.0174532925; //to radians.
                    }
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        
                        if (converterContext._uniqueIDToTrackedObject.count(targetID) != 0) {
                            shared_ptr<JSONObject> targetObject = converterContext._uniqueIDToTrackedObject[targetID];
                            std::string path = animatedTarget->getString("path");
                            if (path == "rotation") {
                                shared_ptr<JSONArray> rotationArray = static_pointer_cast <JSONArray>(targetObject->getValue(path));
                                shared_ptr<GLTFBufferView> adjustedBuffer = __CreateBufferViewByReplicatingArrayAndReplacingValueAtIndex(bufferView, rotationArray, 3, "FLOAT", cvtAnimation->getCount());
                                
                                __SetupAndWriteAnimationParameter(cvtAnimation,
                                                                  "rotation",
                                                                  "FLOAT_VEC4",
                                                                  adjustedBuffer,
                                                                  animationsOutputStream);
                                
                                __AddChannel(cvtAnimation, targetID, path);
                            }
                        }
                    }
                }
                cvtAnimation->removeParameterNamed("OUTPUT");
            }
                return true;
                break;
            case COLLADAFW::AnimationList::POSITION_X:
            case COLLADAFW::AnimationList::POSITION_Y:
            case COLLADAFW::AnimationList::POSITION_Z:
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
            
            shared_ptr <GLTFBufferView> inputBufferView = __ConvertFloatOrDoubleArrayToGLTFBufferView(inputArray);
            shared_ptr <GLTFBufferView> outputBufferView = __ConvertFloatOrDoubleArrayToGLTFBufferView(outputArray);
            
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
