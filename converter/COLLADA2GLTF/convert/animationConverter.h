/*
*/

#ifndef __GLTFANIMATIONCONVERTER_H__
#define __GLTFANIMATIONCONVERTER_H__

#include "mathHelpers.h"

namespace GLTF
{
    shared_ptr <GLTFAnimation> convertOpenCOLLADAAnimationToGLTFAnimation(const COLLADAFW::Animation* animation);
    bool writeAnimation(shared_ptr <GLTFAnimation> cvtAnimation,
                        const COLLADAFW::AnimationList::AnimationClass animationClass,
                        AnimatedTargetsSharedPtr animatedTargets,
                        std::ofstream &animationsOutputStream,
                        GLTF::GLTFConverterContext &converterContext);
    
    //a few helper classes to help flattening animations
    
    typedef std::map<std::string , shared_ptr<COLLADAFW::Transformation> > IDToTransform;
    
    class GLTFTransformKey {
    public:
        
        GLTFTransformKey(double time, shared_ptr<COLLADAFW::Transformation> transform, std::string transformID) {
            this->_subTransforms[transformID] = transform;
            this->_time = time;
        }
        
        double getTime() { return this->_time; }
        
        IDToTransform* subTransforms() {
            return &this->_subTransforms;
        }
        /*
         bool canEvaluateTransform() {
         for (size_t i = 0 ; i < this->_transformsOrder->size(); i++) {
         std::string transformID = this->_transformsOrder->at(i);
         if (this->_subTransforms.count(transformID) == 0) {
         return false;
         }
         }
         
         return true;
         }
         */
        
    private:
        IDToTransform _subTransforms;
        double _time;
    };
    
    class GLTFAnimationFlattener {
    private:
        void _updateTransformByReplacingValueAtIndex(shared_ptr<COLLADAFW::Transformation> transform, size_t index, float value)
        {
            switch (transform->getTransformationType()) {
                case COLLADAFW::Transformation::ROTATE:
                {
                    COLLADAFW::Rotate* rotate = (COLLADAFW::Rotate*)transform.get();
                    COLLADABU::Math::Vector3& rotationAxis = rotate->getRotationAxis();
                    switch (index) {
                        case 0:
                            rotate->setRotationAxis(value, rotationAxis.y, rotationAxis.z);
                            break;
                        case 1:
                            rotate->setRotationAxis(rotationAxis.x, value, rotationAxis.z);
                            break;
                        case 2:
                            rotate->setRotationAxis(rotationAxis.x, rotationAxis.y, value);
                            break;
                        case 3:
                            rotate->setRotationAngle(value);
                            break;
                        default:
                            break;
                            
                    }
                    
                    break;
                }
                case COLLADAFW::Transformation::TRANSLATE:
                {
                    COLLADAFW::Translate* translate = (COLLADAFW::Translate*)transform.get();
                    COLLADABU::Math::Vector3& translation = translate->getTranslation();
                    switch (index) {
                        case 0:
                            translate->setTranslation(value, translation.y, translation.z);
                            break;
                        case 1:
                            translate->setTranslation(translation.x, value, translation.z);
                            break;
                        case 2:
                            translate->setTranslation(translation.x, translation.y, value);
                            break;
                        default:
                            break;
                    }
                }
                case COLLADAFW::Transformation::SCALE:
                {
                    COLLADAFW::Scale* scale = (COLLADAFW::Scale*)transform.get();
                    COLLADABU::Math::Vector3& sc = scale->getScale();
                    switch (index) {
                        case 0:
                            scale->setScale(value, sc.y, sc.z);
                            break;
                        case 1:
                            scale->setScale(sc.x, value, sc.z);
                            break;
                        case 2:
                            scale->setScale(sc.x, sc.y, value);
                            break;
                        default:
                            break;
                    }
                }
                default:
                    break;
            }
        }
        
        shared_ptr<COLLADAFW::Transformation> _cloneTransformByReplacingValueAtIndex(std::string transformID, size_t index, float value)
        {
            shared_ptr<COLLADAFW::Transformation> transform = _idToTransform[transformID];
            shared_ptr<COLLADAFW::Transformation> clonedTransform(transform->clone());
            
            _updateTransformByReplacingValueAtIndex(clonedTransform, index, value);
            
            return clonedTransform;
        }

    public:
        
        GLTFAnimationFlattener(COLLADAFW::Node *node) {
            this->_transformsOrder = shared_ptr <std::vector<std::string> > (new std::vector<std::string>);
            const COLLADAFW::TransformationPointerArray& transformations = node->getTransformations();
            size_t transformationsCount = transformations.getCount();
            int index = 0;
            
            this->_hasAnimatedScale = this->_hasAnimatedTranslation = this->_hasAnimatedRotation = false;

            this->_targetUID = node->getOriginalId();
            
            //printf("for node:%s\n",node->getOriginalId().c_str());
            
            _idIndex = (int*)malloc(sizeof(int) * transformationsCount);
            for (size_t i = 0 ; i < transformationsCount ; i++) {
                const COLLADAFW::Transformation* tr = transformations[i];
                shared_ptr<COLLADAFW::Transformation> clonedTransform(tr->clone());
                const COLLADAFW::UniqueId& animationListID = tr->getAnimationList();
                if (animationListID.isValid()) {
                    switch (tr->getTransformationType()) {
                        case COLLADAFW::Transformation::MATRIX:
                            this->_hasAnimatedScale = this->_hasAnimatedTranslation = this->_hasAnimatedRotation = true;
                            break;
                        case COLLADAFW::Transformation::TRANSLATE:
                            this->_hasAnimatedTranslation = true;
                            break;
                        case COLLADAFW::Transformation::ROTATE:
                            this->_hasAnimatedRotation = true;
                            break;
                        case COLLADAFW::Transformation::SCALE:
                            this->_hasAnimatedScale = true;
                            break;
                        default:
                            printf("warning: unhandled animation type");
                            break;
                    }
                    
                    _idIndex[i] = index++;
                    _idToTransform[animationListID.toAscii()] = clonedTransform;
                    this->_transformsOrder->push_back(animationListID.toAscii());
                    //printf("tr:%s\n",animationListID.toAscii().c_str());

                } else {
                    _idIndex[i] = -1;
                }
                _originalTransforms.push_back(clonedTransform);
            }
        }
        
        virtual ~GLTFAnimationFlattener() {
            free(this->_idIndex);
        }
        
        bool hasAnimatedScale() {
            return this->_hasAnimatedScale;
        }

        bool hasAnimatedTranslation() {
            return this->_hasAnimatedTranslation;
        }

        bool hasAnimatedRotation() {
            return this->_hasAnimatedRotation;
        }

        void allocAndFillAffineTransformsBuffers(float **translationsPtr, float **rotationsPtr, float **scalePtr, size_t &count) {
            
            COLLADABU::Math::Matrix4 transformationMatrix;
            float *translations = 0;
            float *rotations = 0;
            float *scales = 0;
            
            count = _transforms.size();
            
            if (this->_hasAnimatedTranslation && translationsPtr) {
                *translationsPtr = (float*)malloc(sizeof(float) * count * 3);
                translations = *translationsPtr;
            }
            
            if (this->_hasAnimatedRotation && rotationsPtr) {
                *rotationsPtr = (float*)malloc(sizeof(float) * count * 4);
                rotations = *rotationsPtr;
            }

            if (this->_hasAnimatedScale && scalePtr) {
                *scalePtr = (float*)malloc(sizeof(float) * count * 3);
                scales = *scalePtr;
            }
            
            for (size_t i = 0 ; i < _transforms.size() ; i++) {
                shared_ptr<GLTFTransformKey> key = this->_transforms[i];
                
                getTransformationMatrixAtIndex(transformationMatrix, i);
                decomposeMatrix(transformationMatrix,   translations ? translations + (i * 3) : 0,
                                                        rotations ? rotations + (i * 4) : 0,
                                                        scales ? scales + (i * 3) : 0);
            }
        }
        
        //to be used for whole matrices and angle axis
        void insertTransformAtTime(std::string transformID, shared_ptr<COLLADAFW::Transformation> transformation, double time) {

            if (_transforms.size() == 0) {
                shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                _transforms.push_back(key);
            } else {
                for (size_t i = 0 ; i < _transforms.size() ; i++) {
                    shared_ptr<GLTFTransformKey> key = _transforms[i];
                    if (time == key->getTime()) {
                        if ( (*key->subTransforms()).count(transformID) > 0) {
                            printf("INCONSISTENCY ERROR: overlap\n");
                        }
                        (*key->subTransforms())[transformID] = transformation;
                        return;
                    } else if (time > key->getTime()) {
                        if (i + 1 == _transforms.size()) {
                            shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.push_back(key);
                            return;
                        } else if (time < _transforms[i+1]->getTime()) {
                            shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.insert(_transforms.begin() + i, key);
                            return;
                        }
                    }
                }
            }
        }
        
        void insertValueAtTime(std::string transformID, float value, size_t index, double time) {
            if (_transforms.size() == 0) {
                
                shared_ptr <COLLADAFW::Transformation> transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                
                shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                _transforms.push_back(key);
            } else {
                for (size_t i = 0 ; i < _transforms.size() ; i++) {
                    shared_ptr<GLTFTransformKey> key = _transforms[i];
                    if (time == key->getTime()) {
                        if ( (*key->subTransforms()).count(transformID) > 0) {
                            shared_ptr <COLLADAFW::Transformation> transformation = (*key->subTransforms())[transformID];
                            this->_updateTransformByReplacingValueAtIndex(transformation, index, value);
                        } else {
                            shared_ptr <COLLADAFW::Transformation> transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                            (*key->subTransforms())[transformID] = transformation;
                        }
                        return;
                    } else if (time > key->getTime()) {
                        if (i + 1 == _transforms.size()) {
                            shared_ptr <COLLADAFW::Transformation> transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                            
                            shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.push_back(key);
                            return;
                        } else if (time < _transforms[i+1]->getTime()) {
                            shared_ptr <COLLADAFW::Transformation> transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);

                            shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.insert(_transforms.begin() + i, key);
                            return;
                        }
                    }
                }
            }
        }

        void setTransformsOrder(shared_ptr<std::vector<std::string> > transformsOrder) {
            this->_transformsOrder = transformsOrder;
        }
        
        shared_ptr<std::vector<std::string> > getTransformsOrder() {
            return this->_transformsOrder;
        }
        
        size_t getCount() { return this->_transforms.size(); };
        
        void getTransformationMatrixAtIndex(COLLADABU::Math::Matrix4& transformationMatrix, size_t index) {
            transformationMatrix = COLLADABU::Math::Matrix4::IDENTITY;
            
            for ( size_t i = 0, count = this->_originalTransforms.size(); i < count; ++i ) {
                COLLADAFW::Transformation* transform;
                int idIndex = _idIndex[i];
                if (idIndex != -1) {
                    std::string transformID = this->_transformsOrder->at(idIndex);
                    shared_ptr <GLTFTransformKey> key = this->_transforms[index];
                    
                    if ((*key->subTransforms()).count(transformID) == 0) {
                        std::string nodeUID = _targetUID;
                        printf("warning: missing key\n");
                    }
                    
                    transform = (*key->subTransforms())[transformID].get();
                    if (!transform) {
                        printf("warning:can't find id\n");

                    }
                } else {
                    transform = this->_originalTransforms[i].get();
                }
                
                switch ( transform->getTransformationType() ) {
                    case COLLADAFW::Transformation::ROTATE:
                    {
                        COLLADAFW::Rotate* rotate = (COLLADAFW::Rotate*)transform;
                        COLLADABU::Math::Vector3 axis = rotate->getRotationAxis();
                        axis.normalise();
                        double angle = rotate->getRotationAngle();
                        transformationMatrix = transformationMatrix * COLLADABU::Math::Matrix4(COLLADABU::Math::Quaternion(COLLADABU::Math::Utils::degToRad(angle), axis));
                        break;
                    }
                    case COLLADAFW::Transformation::TRANSLATE:
                    {
                        COLLADAFW::Translate* translate = (COLLADAFW::Translate*)transform;
                        const COLLADABU::Math::Vector3& translation = translate->getTranslation();
                        COLLADABU::Math::Matrix4 translationMatrix;
                        translationMatrix.makeTrans(translation);
                        transformationMatrix = transformationMatrix * translationMatrix;
                        break;
                    }
                    case COLLADAFW::Transformation::SCALE:
                    {
                        COLLADAFW::Scale* scale = (COLLADAFW::Scale*)transform;
                        const COLLADABU::Math::Vector3& scaleVector = scale->getScale();
                        COLLADABU::Math::Matrix4 scaleMatrix;
                        scaleMatrix.makeScale(scaleVector);
                        transformationMatrix = transformationMatrix * scaleMatrix;
                        break;
                    }
                    case COLLADAFW::Transformation::MATRIX:
                    {
                        COLLADAFW::Matrix* matrix = (COLLADAFW::Matrix*)transform;
                        transformationMatrix = transformationMatrix * matrix->getMatrix();
                        break;
                    }
                    case COLLADAFW::Transformation::LOOKAT:
                        break;
                    case COLLADAFW::Transformation::SKEW:
                        break;
                }
                
            }
        }
        
    private:
        bool _hasAnimatedScale, _hasAnimatedTranslation, _hasAnimatedRotation;
        std::string _targetUID;
        int *_idIndex;
        std::vector<shared_ptr<COLLADAFW::Transformation> > _originalTransforms;
        std::vector<shared_ptr<GLTFTransformKey> > _transforms;
        std::map<std::string , shared_ptr<COLLADAFW::Transformation> > _idToTransform;
        shared_ptr<std::vector<std::string> > _transformsOrder;
    };
    
    
    //-------
}


#endif

