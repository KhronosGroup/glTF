/*
 */

#ifndef __GLTFANIMATIONCONVERTER_H__
#define __GLTFANIMATIONCONVERTER_H__

#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "mathHelpers.h"

namespace GLTF
{    
    //a few helper classes to help flattening animations
    
    typedef std::map<std::string , std::shared_ptr<COLLADAFW::Transformation> > IDToTransform;
    
    class GLTFTransformKey {
    public:
        
        GLTFTransformKey(double time, std::shared_ptr<COLLADAFW::Transformation> transform, std::string transformID) {
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
        void _updateTransformByReplacingValueAtIndex(std::shared_ptr<COLLADAFW::Transformation> transform, size_t index, float value)
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
        
        std::shared_ptr<COLLADAFW::Transformation> _cloneTransformByReplacingValueAtIndex(std::string transformID, size_t index, float value)
        {
            std::shared_ptr<COLLADAFW::Transformation> transform = _idToTransform[transformID];
            std::shared_ptr<COLLADAFW::Transformation> clonedTransform(transform->clone());
            
            _updateTransformByReplacingValueAtIndex(clonedTransform, index, value);
            
            return clonedTransform;
        }
        
    public:
        
        GLTFAnimationFlattener(COLLADAFW::Node *node) {
            this->_transformsOrder = std::shared_ptr <std::vector<std::string> > (new std::vector<std::string>);
            const COLLADAFW::TransformationPointerArray& transformations = node->getTransformations();
            size_t transformationsCount = transformations.getCount();
            int index = 0;
            
            this->_hasAnimatedScale = this->_hasAnimatedTranslation = this->_hasAnimatedRotation = false;
            
            this->_targetUID = node->getOriginalId();
            
            _idIndex = (int*)malloc(sizeof(int) * transformationsCount);
            for (size_t i = 0 ; i < transformationsCount ; i++) {
                const COLLADAFW::Transformation* tr = transformations[i];
                std::shared_ptr<COLLADAFW::Transformation> clonedTransform(tr->clone());
                const COLLADAFW::UniqueId& animationListID = tr->getAnimationList();
                if (animationListID.isValid()) {                    
                    _idIndex[i] = index++;
                    _idToTransform[animationListID.toAscii()] = clonedTransform;
                    this->_transformsOrder->push_back(animationListID.toAscii());
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
            
            float *previousAxisAngle = 0;
            float axisAngle[4];
            for (size_t i = 0 ; i < _transforms.size() ; i++) {
                std::shared_ptr<GLTFTransformKey> key = this->_transforms[i];

                if ((i > 0) && (rotations != 0)) {
                    previousAxisAngle = rotations + ((i-1) * 4);
                }
                
                getTransformationMatrixAtIndex(transformationMatrix, i);
                decomposeMatrix(transformationMatrix,
                                (translations != 0) ? translations + (i * 3) : 0,
                                (rotations != 0) ? axisAngle : 0,
                                (scales != 0) ? scales + (i * 3) : 0);
                
                if ((i > 0) && (rotations != 0)) {
                    //each quaternions have 2 possible representations from axis angle
                    //we want to pick-up the closest one to the last orientation
                    COLLADABU::Math::Vector3 axis(axisAngle[0], axisAngle[1], axisAngle[2]);
                    COLLADABU::Math::Quaternion key1;
                    COLLADABU::Math::Quaternion key2;
                    
                    key1.fromAngleAxis(axisAngle[3], axis);
                    bool skip = false;
                    if (0 == memcmp(axisAngle, previousAxisAngle, 4 * sizeof(float))) {
                        skip = true;
                    }
                    
                    key2.x = -key1.x;
                    key2.y = -key1.y;
                    key2.z = -key1.z;
                    key2.w = -key1.w;
                    
                    key1.normalise();
                    key2.normalise();
                    
                    COLLADABU::Math::Vector3 previousAxis(previousAxisAngle[0], previousAxisAngle[1], previousAxisAngle[2]);
                    COLLADABU::Math::Quaternion previousKey;
                    previousKey.fromAngleAxis(previousAxisAngle[3], previousAxis);
                    previousKey.normalise();

                    double angle1 = acos(previousKey.dot(key1));
                    double angle2 = acos(previousKey.dot(key2));

                    if (angle1 > COLLADABU::Math::HALF_PI)
                        angle1 -= COLLADABU::Math::HALF_PI;
                    if (angle2 > COLLADABU::Math::HALF_PI)
                        angle2 -= COLLADABU::Math::HALF_PI;
                  
                    COLLADABU::Math::Vector3 destAxis1;
                    COLLADABU::Math::Vector3 destAxis2;
                    COLLADABU::Math::Real destAngle1;
                    COLLADABU::Math::Real destAngle2;
                    key1.toAngleAxis ( destAngle1, destAxis1 );
                    key2.toAngleAxis ( destAngle2, destAxis2 );
                    
                    if ((skip == false) && (angle1 > angle2)) {
                        axisAngle[0] = (float)destAxis2[0];
						axisAngle[1] = (float)destAxis2[1];
						axisAngle[2] = (float)destAxis2[2];
						axisAngle[3] = (float)destAngle2;
                    } else {
						axisAngle[0] = (float)destAxis1[0];
						axisAngle[1] = (float)destAxis1[1];
						axisAngle[2] = (float)destAxis1[2];
						axisAngle[3] = (float)destAngle1;
                    }
                }
                
                if (rotations != 0)
                    memcpy(rotations + (i * 4), axisAngle, sizeof(float) * 4);

                                
            }
        }
        
        void transformWasInserted(std::shared_ptr<COLLADAFW::Transformation> tr) {
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
                    printf("warning: unhandled animation type\n");
                    break;
            }
        }
        
        //to be used for whole matrices and angle axis
        void insertTransformAtTime(std::string transformID, std::shared_ptr<COLLADAFW::Transformation> transformation, double time) {
            transformWasInserted(transformation);
            if (_transforms.size() == 0) {
                std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                _transforms.push_back(key);
            } else {
                for (size_t i = 0 ; i < _transforms.size() ; i++) {
                    std::shared_ptr<GLTFTransformKey> key = _transforms[i];
                    if (time == key->getTime()) {
                        if ( (*key->subTransforms()).count(transformID) > 0) {
                            printf("INCONSISTENCY ERROR: overlap\n");
                        }
                        (*key->subTransforms())[transformID] = transformation;
                        return;
                    } else if (time > key->getTime()) {
                        if (i + 1 == _transforms.size()) {
                            std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.push_back(key);
                            return;
                        } else if (time < _transforms[i+1]->getTime()) {
                            std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.insert(_transforms.begin() + i, key);
                            return;
                        }
                    } else {
                        std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                        _transforms.insert(_transforms.begin() + i, key);
                        return;
                    }
                }
            }
        }
        
        void insertValueAtTime(std::string transformID, float value, size_t index, double time) {
            std::shared_ptr <COLLADAFW::Transformation> transformation;
            if (_transforms.size() == 0) {
                transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                
                std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                _transforms.push_back(key);
                transformWasInserted(transformation);
                return;
            } else {
                for (size_t i = 0 ; i < _transforms.size() ; i++) {
                    std::shared_ptr<GLTFTransformKey> key = _transforms[i];
                    if (time == key->getTime()) {
                        if ( (*key->subTransforms()).count(transformID) > 0) {
                            transformation = (*key->subTransforms())[transformID];
                            this->_updateTransformByReplacingValueAtIndex(transformation, index, value);
                        } else {
                            transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                            (*key->subTransforms())[transformID] = transformation;
                        }
                        transformWasInserted(transformation);
                        return;
                    } else if (time > key->getTime()) {
                        if (i + 1 == _transforms.size()) {
                            transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                            
                            std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.push_back(key);
                            transformWasInserted(transformation);
                            return;
                        } else if (time < _transforms[i+1]->getTime()) {
                            transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                            
                            std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                            _transforms.insert(_transforms.begin() + i, key);
                            transformWasInserted(transformation);
                            return;
                        }
                    } else {
                        transformation = this->_cloneTransformByReplacingValueAtIndex(transformID,  index, value);
                        
                        std::shared_ptr <GLTFTransformKey> key(new GLTFTransformKey(time, transformation, transformID));
                        _transforms.insert(_transforms.begin() + i, key);
                        transformWasInserted(transformation);
                        return;
                    }
                }
            }
            
        }
        
        void setTransformsOrder(std::shared_ptr<std::vector<std::string> > transformsOrder) {
            this->_transformsOrder = transformsOrder;
        }
        
        std::shared_ptr<std::vector<std::string> > getTransformsOrder() {
            return this->_transformsOrder;
        }
        
        size_t getCount() { return this->_transforms.size(); };
        
#define _INTERPOLATE(I1, I2, STEP) (I1 + (STEP * (I2-I1)))
        
        //TODO: might be worth checking for equality for prev & transform to be interpolated here
        std::shared_ptr <COLLADAFW::Transformation> _interpolateTransforms(std::shared_ptr<COLLADAFW::Transformation> previousTransform, std::shared_ptr<COLLADAFW::Transformation> nextTransform, double ratio) {
            
            COLLADAFW::Transformation::TransformationType transformationType = previousTransform->getTransformationType();
            std::shared_ptr<COLLADAFW::Transformation> transform(previousTransform->clone());
            
            switch (transformationType) {
                case COLLADAFW::Transformation::ROTATE:
                {
                    COLLADAFW::Rotate* r1 = (COLLADAFW::Rotate*)previousTransform.get();
                    COLLADAFW::Rotate* r2 = (COLLADAFW::Rotate*)nextTransform.get();
                    
                    COLLADAFW::Rotate* r = (COLLADAFW::Rotate*)transform.get();
                    
                    r->setRotationAngle(_INTERPOLATE(r1->getRotationAngle(), r2->getRotationAngle(), ratio));
                    
                    COLLADABU::Math::Vector3& rAxis1 = r1->getRotationAxis();
                    COLLADABU::Math::Vector3& rAxis2 = r2->getRotationAxis();
                    
                    r->setRotationAxis(_INTERPOLATE(rAxis1.x, rAxis2.x, ratio),
                                       _INTERPOLATE(rAxis1.y, rAxis2.y, ratio),
                                       _INTERPOLATE(rAxis1.z, rAxis2.z, ratio));
                    break;
                }
                case COLLADAFW::Transformation::TRANSLATE:
                {
                    COLLADAFW::Translate* t1 = (COLLADAFW::Translate*)previousTransform.get();
                    COLLADAFW::Translate* t2 = (COLLADAFW::Translate*)nextTransform.get();
                    COLLADAFW::Translate* t = (COLLADAFW::Translate*)transform.get();
                    
                    COLLADABU::Math::Vector3& translation1 = t1->getTranslation();
                    COLLADABU::Math::Vector3& translation2 = t2->getTranslation();
                    
                    t->setTranslation(_INTERPOLATE(translation1.x, translation2.x, ratio),
                                      _INTERPOLATE(translation1.y, translation2.y, ratio),
                                      _INTERPOLATE(translation1.z, translation2.z, ratio));
                    
                    break;
                    
                }
                case COLLADAFW::Transformation::SCALE:
                {
                    COLLADAFW::Scale* s1 = (COLLADAFW::Scale*)previousTransform.get();
                    COLLADAFW::Scale* s2 = (COLLADAFW::Scale*)nextTransform.get();
                    
                    COLLADAFW::Scale* t = (COLLADAFW::Scale*)transform.get();
                    
                    COLLADABU::Math::Vector3& scale1 = s1->getScale();
                    COLLADABU::Math::Vector3& scale2 = s2->getScale();
                    
                    t->setScale(_INTERPOLATE(scale1.x, scale2.x, ratio),
                                _INTERPOLATE(scale1.y, scale2.y, ratio),
                                _INTERPOLATE(scale1.z, scale2.z, ratio));
                    
                    break;
                }
                default:
                {
                    static bool printOnce = false;
                    if (!printOnce) {
                        printf("WARNING: unhandled interpolation in animation flattener\n");
                        printOnce = true;
                    }
                }
                    break;
            }
            
            return transform;
        }
        
        void getTransformationMatrixAtIndex(COLLADABU::Math::Matrix4& transformationMatrix, size_t index) {
            transformationMatrix = COLLADABU::Math::Matrix4::IDENTITY;
            
            for ( size_t i = 0, count = this->_originalTransforms.size(); i < count; ++i ) {
                COLLADAFW::Transformation* transform;
                int idIndex = _idIndex[i];
                if (idIndex != -1) {
                    std::string transformID = this->_transformsOrder->at(idIndex);
                    std::shared_ptr <GLTFTransformKey> key = this->_transforms[index];
                    
                    if ((*key->subTransforms()).count(transformID) == 0) {
                        //so here we need to get a transform matching transformID for this key but it does not contain it,
                        //we need to figure it out by interpolating the previous/next key containing this transform (this involves a search.
                        
                        std::shared_ptr<COLLADAFW::Transformation> previousTransform;
                        std::shared_ptr<COLLADAFW::Transformation> nextTransform;
                        std::shared_ptr <GLTFTransformKey> previousKey;
                        std::shared_ptr <GLTFTransformKey> nextKey;
                        double t1 = 0, t2 = 0;
                        
                        bool found = false;
                        if (index > 0) {
                            int previousIndex = (int)index - 1;
                            do {
                                previousKey = this->_transforms[previousIndex--];
                                if ((*previousKey->subTransforms()).count(transformID) != 0) {
                                    previousTransform = (*previousKey->subTransforms())[transformID];
                                    t1 = previousKey->getTime();
                                    found = true;
                                    break;
                                }
                            } while (previousIndex >= 0);
                        }
                        
                        if (found == false) {
                            previousTransform = _idToTransform[transformID];
                            t1 = 0;
                        }
                        
                        found = false;
                        if (index + 1 < this->_transforms.size()) {
                            size_t nextIndex = index + 1;
                            do {
                                nextKey = this->_transforms[nextIndex++];
                                if ((*nextKey->subTransforms()).count(transformID) != 0) {
                                    nextTransform = (*nextKey->subTransforms())[transformID];
                                    t2 = nextKey->getTime();
                                    found = true;
                                    break;
                                }
                            } while (nextIndex < this->_transforms.size());
                        }
                        
                        if (found == false) {
                            std::shared_ptr <GLTFTransformKey> lastKey = this->_transforms[this->_transforms.size() - 1];
                            nextTransform = _idToTransform[transformID];
                            t2 = lastKey->getTime();
                        }
                        
                        if (previousTransform->getTransformationType() == nextTransform->getTransformationType()) {
                            double ratio = key->getTime() / (t2 - t1);
                            (*key->subTransforms())[transformID] = _interpolateTransforms(previousTransform, nextTransform, ratio);
                        } else {
                            printf("inconsistent state: cannot interpolate keys of different types\n");
                        }
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
        std::vector<std::shared_ptr<COLLADAFW::Transformation> > _originalTransforms;
        std::vector<std::shared_ptr<GLTFTransformKey> > _transforms;
        std::map<std::string , std::shared_ptr<COLLADAFW::Transformation> > _idToTransform;
        std::shared_ptr<std::vector<std::string> > _transformsOrder;
    };
    
    std::shared_ptr <GLTFAnimation> convertOpenCOLLADAAnimationToGLTFAnimation(const COLLADAFW::Animation* animation, GLTF::GLTFAsset *asset);
    bool writeAnimation(std::shared_ptr <GLTFAnimation> cvtAnimation,
                        const COLLADAFW::AnimationList::AnimationClass animationClass,
                        AnimatedTargetsSharedPtr animatedTargets,
                        GLTF::GLTFAsset *asset);
    
    //-------
}


#endif

