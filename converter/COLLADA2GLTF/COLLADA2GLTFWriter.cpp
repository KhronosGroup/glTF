// Copyright (c) 2012, Motorola Mobility, Inc.
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

// TODO: consider LOD
// TODO: normal generation when needed
// DESIGN: justification generation shader. geometry may need regeneration if lambert
// DESIGN: difference between COLLADA and JSON format. In JSON format it is possible to make an interleaved array not only made by float.
// reminder; to run recursively against all dae files: find . -name '*.dae' -exec dae2json {} \;

#include <algorithm>

#include "GLTF.h"
#include "COLLADA2GLTFWriter.h"
#include "COLLADAFWPolygons.h"
#include "shaders/commonProfileShaders.h"
#include "helpers/geometryHelpers.h"

namespace GLTF
{
    enum unmatrix_indices {
        U_SCALEX,
        U_SCALEY,
        U_SCALEZ,
        U_SHEARXY,
        U_SHEARXZ,
        U_SHEARYZ,
        U_ROTATEX,
        U_ROTATEY,
        U_ROTATEZ,
        U_ROTATEW,
        U_TRANSX,
        U_TRANSY,
        U_TRANSZ,
        
    };
        
    /* Originally from: http://tog.acm.org/resources/GraphicsGems/gemsii/unmatrix.c
     * Simplified version without Shear and Perspective decomposition
     *
     * unmatrix.c - given a 4x4 matrix, decompose it into standard operations.
     *
     * Author:	Spencer W. Thomas
     * 		University of Michigan
     */
    bool unmatrix(COLLADABU::Math::Matrix4 mat, double *tran)
    {
        int i, j;
        COLLADABU::Math::Matrix4 locmat;
        COLLADABU::Math::Matrix4 pmat, invpmat, tinvpmat;
        COLLADABU::Math::Vector3 row[3], pdum3;
        
        locmat = mat;
        
        /* Normalize the matrix. */
        for ( i=0; i<4;i++ )
            for ( j=0; j<4; j++ )
                locmat.setElement(i, j, locmat.getElement(i,j) / locmat[3][3]) ;
        /* pmat is used to solve for perspective, but it also provides
         * an easy way to test for singularity of the upper 3x3 component.
         */
        pmat = locmat;
        for ( i=0; i<3; i++ )
            pmat.setElement(i,3, 0);
        pmat.setElement(3,3, 1);
        
        if ( pmat.determinant() == 0.0 )
            return false;
        
        /* First, isolate perspective.  This is the messiest. */
        if ( locmat.getElement(0,3) != 0 ||
            locmat.getElement(1,3) != 0 ||
            locmat.getElement(2,3) != 0 ) {
            locmat.setElement(0, 3,     0);
            locmat.setElement(1, 3,     0);
            locmat.setElement(2, 3,     0);
            locmat.setElement(3, 3,     1);
        }
        
        for ( i=0; i<3; i++ ) {
            tran[U_TRANSX + i] = locmat[3][i];
            locmat.setElement(3,i,   0);
        }
        
        /* Now get scale and shear. */
        for ( i=0; i<3; i++ ) {
            row[i].x = locmat[i][0];
            row[i].y = locmat[i][1];
            row[i].z = locmat[i][2];
        }
        
        /* Compute X scale factor and normalize first row. */
        tran[U_SCALEX] = row[0].length();
        row[0].normalise();
        /* Compute XY shear factor and make 2nd row orthogonal to 1st. */
        //tran[U_SHEARXY] = row[0].Dot(row[1]);
        //(void)V3Combine(&row[1], &row[0], &row[1], 1.0, -tran[U_SHEARXY]);
        
        /* Now, compute Y scale and normalize 2nd row. */
        tran[U_SCALEY] = row[1].length();
        row[1].normalise();
        // tran[U_SHEARXY] /= tran[U_SCALEY];
        
        /* Compute XZ and YZ shears, orthogonalize 3rd row. */
        //tran[U_SHEARXZ] = V3Dot(&row[0], &row[2]);
        //(void)V3Combine(&row[2], &row[0], &row[2], 1.0, -tran[U_SHEARXZ]);
        //tran[U_SHEARYZ] = V3Dot(&row[1], &row[2]);
        //(void)V3Combine(&row[2], &row[1], &row[2], 1.0, -tran[U_SHEARYZ]);
        
        /* Next, get Z scale and normalize 3rd row. */
        tran[U_SCALEZ] = row[2].length();
        row[2].normalise();
        //tran[U_SHEARXZ] /= tran[U_SCALEZ];
        //tran[U_SHEARYZ] /= tran[U_SCALEZ];
        
        /* At this point, the matrix (in rows[]) is orthonormal.
         * Check for a coordinate system flip.  If the determinant
         * is -1, then negate the matrix and the scaling factors.
         */
        
        if ( row[0].dotProduct(row[1].crossProduct(row[2]) ) < 0 ) {
            for ( i = 0; i < 3; i++ ) {
                tran[U_SCALEX+i] *= -1;
                row[i].x *= -1;
                row[i].y *= -1;
                row[i].z *= -1;
            }
        }
        
        COLLADABU::Math::Matrix3 amat3( row[0][0], row[1][0], row[2][0],
                                       row[0][1], row[1][1], row[2][1],
                                       row[0][2], row[1][2], row[2][2]);
        COLLADABU::Math::Real angle;
        COLLADABU::Math::Vector3 axis;
        //COLLADABU::Math::Quaternion aquat = QuaternionFromMatrix(amat3);
        COLLADABU::Math::Quaternion aquat;
        aquat.fromRotationMatrix(amat3);
        
        aquat.toAngleAxis(angle, axis);
        tran[U_ROTATEX] = axis.x;
        tran[U_ROTATEY] = axis.y;
        tran[U_ROTATEZ] = axis.z;
        tran[U_ROTATEW] = angle;
        
        return true;
    }
    
    //converted to C++ from gl-matrix by Brandon Jones ( https://github.com/toji/gl-matrix )
    void buildLookAtMatrix(Lookat *lookat, COLLADABU::Math::Matrix4& matrix)
    {
        assert(lookat);
        
        const COLLADABU::Math::Vector3& eye = lookat->getEyePosition();
        const COLLADABU::Math::Vector3& center = lookat->getInterestPointPosition();
        const COLLADABU::Math::Vector3& up = lookat->getUpAxisDirection();
        
        if ((eye.x == center.x) && (eye.y == center.y) && (eye.z == center.z)) {
            matrix = COLLADABU::Math::Matrix4::IDENTITY;
            return;
        }
        
        COLLADABU::Math::Vector3 z = (eye - center);
        z.normalise(); // TODO: OpenCOLLADA typo should be normalize (with a z).
        COLLADABU::Math::Vector3 x = up.crossProduct(z);
        x.normalise();
        COLLADABU::Math::Vector3 y = z.crossProduct(x);
        y.normalise();
        
        matrix.setAllElements(x.x,
                              y.x,
                              z.x,
                              0,
                              x.y,
                              y.y,
                              z.y,
                              0,
                              x.z,
                              y.z,
                              z.z,
                              0,
                              -(x.x * eye.x + x.y  * eye.y + x.z * eye.z),
                              -(y.x * eye.x + y.y * eye.y + y.z * eye.z),
                              -(z.x * eye.x + z.y * eye.y + z.z * eye.z),
                              1);
        matrix = matrix.inverse();
        matrix = matrix.transpose();
    }
    
    BBOX::BBOX() {
        this->_min = COLLADABU::Math::Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
        this->_max = COLLADABU::Math::Vector3(DBL_MIN, DBL_MIN, DBL_MIN);
    }
    
    BBOX::BBOX(const COLLADABU::Math::Vector3 &min, const COLLADABU::Math::Vector3 &max) {
        this->_min = min;
        this->_max = max;
    }
    
    const COLLADABU::Math::Vector3&  BBOX::getMin3() {
        return this->_min;
    }
    
    const COLLADABU::Math::Vector3& BBOX::getMax3() {
        return this->_max;
    }
    
    void BBOX::merge(BBOX* bbox) {
        this->_min.makeFloor(bbox->getMin3());
        this->_max.makeCeil(bbox->getMax3());
    }
    
    void BBOX::transform(const COLLADABU::Math::Matrix4& mat4) {
        COLLADABU::Math::Vector3 min = COLLADABU::Math::Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
        COLLADABU::Math::Vector3 max = COLLADABU::Math::Vector3(DBL_MIN, DBL_MIN, DBL_MIN);
        
        COLLADABU::Math::Vector3 pt0 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_min.y, this->_min.z);
        COLLADABU::Math::Vector3 pt1 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_min.y, this->_min.z);
        COLLADABU::Math::Vector3 pt2 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_max.y, this->_min.z);
        COLLADABU::Math::Vector3 pt3 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_max.y, this->_min.z);
        COLLADABU::Math::Vector3 pt4 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_min.y, this->_max.z);
        COLLADABU::Math::Vector3 pt5 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_min.y, this->_max.z);
        COLLADABU::Math::Vector3 pt6 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_max.y, this->_max.z);
        COLLADABU::Math::Vector3 pt7 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_max.y, this->_max.z);
        
        min.makeFloor(pt0); max.makeCeil(pt0);
        min.makeFloor(pt1); max.makeCeil(pt1);
        min.makeFloor(pt2); max.makeCeil(pt2);
        min.makeFloor(pt3); max.makeCeil(pt3);
        min.makeFloor(pt4); max.makeCeil(pt4);
        min.makeFloor(pt5); max.makeCeil(pt5);
        min.makeFloor(pt6); max.makeCeil(pt6);
        min.makeFloor(pt7); max.makeCeil(pt7);
        
        this->_min = min;
        this->_max = max;
    }
    
    
    
    //---- Convert OpenCOLLADA Animation GLTFAnimation  -------------------------------------------
    
    shared_ptr <GLTFBufferView> __ConvertFloatOrDoubleArrayToGLTFBufferView(const FloatOrDoubleArray &floatOrDoubleArray) {
        unsigned char* sourceData = 0;
        size_t sourceSize = 0;
        
        switch (floatOrDoubleArray.getType()) {
            case MeshVertexData::DATA_TYPE_FLOAT: {
                const FloatArray* array = floatOrDoubleArray.getFloatValues();
                
                sourceData = (unsigned char*)array->getData();
                sourceSize = array->getCount() * sizeof(float);
            }
                break;
            case MeshVertexData::DATA_TYPE_DOUBLE: {
                const DoubleArray* array = floatOrDoubleArray.getDoubleValues();
                
                sourceData = (unsigned char*)array->getData();
                sourceSize = array->getCount() * sizeof(double);
            }
                break;
            default:
            case MeshVertexData::DATA_TYPE_UNKNOWN:
                //FIXME report error
                break;
        }
        unsigned char* copiedData = (unsigned char*)malloc(sourceSize);
        memcpy(copiedData, sourceData, sourceSize);
        
        shared_ptr <GLTF::GLTFBufferView> bufferView = createBufferViewWithAllocatedBuffer(copiedData, 0, sourceSize, true);
        
        return bufferView;
    }
    
    static void __DecomposeMatrix(COLLADABU::Math::Matrix4 &matrix, float *translation, float *rotation, float *scale) {
        COLLADABU::Math::Matrix4 tr = matrix.transpose();
        tr.setElement(0,3, 0);
        tr.setElement(1,3, 0);
        tr.setElement(2,3, 0);
        tr.setElement(3,3, 1);
        double tran[20];
        
        //MathGeoLib is failing to decompose here, use SPENCER decomposition instead
        if (!unmatrix(tr, tran)) {
            printf("WARNING: matrix can't be decomposed \n");
        }
        
        translation[0] = tran[U_TRANSX];
        translation[1] = tran[U_TRANSY];
        translation[2] = tran[U_TRANSZ];
        
        rotation[0] = tran[U_ROTATEX];
        rotation[1] = tran[U_ROTATEY];
        rotation[2] = tran[U_ROTATEZ];
        rotation[3] = tran[U_ROTATEW];
        
        scale[0] = tran[U_SCALEX];
        scale[1] = tran[U_SCALEY];
        scale[2] = tran[U_SCALEZ];
    }

    //  printf("rotation at:%d %f %f %f %f\n",  i,
    //  rotationData[0],rotationData[1],rotationData[2],rotationData[3]);
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
            
            __DecomposeMatrix(mat, translationData, rotationData, scaleData);            
                        
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

    static void __AddChannel(shared_ptr <GLTFAnimation> cvtAnimation,
                             const std::string &targetID,
                             const std::string &path) {
        std::string channelID = cvtAnimation->getID() + "_" + targetID + "_" +path + "_channel";
        shared_ptr<JSONObject> trChannel(new JSONObject());
        shared_ptr<JSONObject> trTarget(new JSONObject());
        
        trChannel->setString("sampler", cvtAnimation->getSamplerIDForName(path));
        trChannel->setValue("target", trTarget);
        trTarget->setString(targetID, path);
        cvtAnimation->channels()->setValue(channelID, trChannel);
    }
    
    static void __SetupAndWriteAnimationParameter(shared_ptr <GLTFAnimation> cvtAnimation,
                                          const string& parameterSID,
                                          const string& parameterType,
                                          shared_ptr <GLTFBufferView> bufferView,
                                          ofstream &animationsOutputStream) {
        //setup
        shared_ptr <GLTFAnimation::Parameter> parameter(new GLTFAnimation::Parameter(parameterSID));
        parameter->setType(parameterType);
        __SetupSamplerForParameter(cvtAnimation, parameter.get());
        
        cvtAnimation->parameters()->push_back(parameter);
        
        //write
        parameter->setByteOffset(static_cast<size_t>(animationsOutputStream.tellp()));
        animationsOutputStream.write((const char*)( bufferView->getBufferDataByApplyingOffset()),
                                     bufferView->getByteLength());
    }
    
    bool COLLADA2GLTFWriter::writeAnimation(shared_ptr <GLTFAnimation> cvtAnimation,
                                  const COLLADAFW::AnimationList::AnimationClass animationClass,
                                  AnimatedTargetsSharedPtr animatedTargets,
                                  ofstream &animationsOutputStream) {
        
        
        
        std::string samplerID;
        std::string name;
        shared_ptr<JSONObject> samplers = cvtAnimation->samplers();
        shared_ptr<JSONObject> channels = cvtAnimation->channels();
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
                        angles[i] = angles[i] * 0.0174532925;
                    }
                    
                    for (size_t animatedTargetIndex = 0 ; animatedTargetIndex < animatedTargets->size() ; animatedTargetIndex++) {
                        shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[animatedTargetIndex];
                        std::string targetID = animatedTarget->getString("target");
                        
                        if (this->_converterContext._uniqueIDToTrackedObject.count(targetID) != 0) {
                            shared_ptr<JSONObject> targetObject = this->_converterContext._uniqueIDToTrackedObject[targetID];
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
    
    static shared_ptr <GLTFAnimation> ConvertOpenCOLLADAAnimationToGLTFAnimation(const COLLADAFW::Animation* animation)
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
            const FloatOrDoubleArray &inputArray =  animationCurve->getInputValues();
            const FloatOrDoubleArray &outputArray =  animationCurve->getOutputValues();
            
            const String originalID = animationCurve->getOriginalId();
            
            shared_ptr <GLTFBufferView> inputBufferView = __ConvertFloatOrDoubleArrayToGLTFBufferView(inputArray);
            shared_ptr <GLTFBufferView> outputBufferView = __ConvertFloatOrDoubleArrayToGLTFBufferView(outputArray);
            
            //build up input parameter, typically TIME
            shared_ptr <GLTFAnimation::Parameter> inputParameter(new GLTFAnimation::Parameter("TIME"));
            
            inputParameter->setType("FLOAT");
            inputParameter->setBufferView(inputBufferView);
            inputParameter->setByteOffset(0);
            
            animationParameters->push_back(inputParameter);
            
            //build up output parameter
            shared_ptr <GLTFAnimation::Parameter> outputParameter(new GLTFAnimation::Parameter("OUTPUT"));
            
            outputParameter->setBufferView(outputBufferView);
            outputParameter->setByteOffset(0);
            
            animationParameters->push_back(outputParameter);
        }
        
        return cvtAnimation;
    }
    
    //---- Convert OpenCOLLADA mesh to mesh -------------------------------------------
    
    static unsigned int ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(const COLLADAFW::MeshVertexData &vertexData, GLTF::IndexSetToMeshAttributeHashmap &meshAttributes)
    {
        // The following are OpenCOLLADA fmk issues preventing doing a totally generic processing of sources
        //1. "set"(s) other than texCoord don't have valid input infos
        //2. not the original id in the source
        
        std::string name;
        size_t length, elementsCount;
        size_t stride = 0;
        size_t size = 0;
        size_t byteOffset = 0;
        size_t inputLength = 0;
        
        size_t setCount = vertexData.getNumInputInfos();
        bool unpatchedOpenCOLLADA = (setCount == 0); // reliable heuristic to know if the input have not been set
        
        if (unpatchedOpenCOLLADA)
            setCount = 1;
        
        for (size_t indexOfSet = 0 ; indexOfSet < setCount ; indexOfSet++) {
            
            if (!unpatchedOpenCOLLADA) {
                name = vertexData.getName(indexOfSet);
                size = vertexData.getStride(indexOfSet);
                inputLength = vertexData.getLength(indexOfSet);
            } else {
                // for unpatched version of OpenCOLLADA we need this work-around.
                name = GLTF::GLTFUtils::generateIDForType("buffer").c_str();
                size = 3; //only normal and positions should reach this code
                inputLength = vertexData.getLength(0);
            }
            
            //name is the id
            length = inputLength ? inputLength : vertexData.getValuesCount();
            elementsCount = length / size;
            unsigned char *sourceData = 0;
            size_t sourceSize = 0;
            
            GLTF::ComponentType componentType = GLTF::NOT_AN_ELEMENT_TYPE;
            switch (vertexData.getType()) {
                case MeshVertexData::DATA_TYPE_FLOAT: {
                    componentType = GLTF::FLOAT;
                    stride = sizeof(float) * size;
                    const FloatArray* array = vertexData.getFloatValues();
                    
                    sourceData = (unsigned char*)array->getData() + byteOffset;
                    
                    sourceSize = length * sizeof(float);
                    byteOffset += sourceSize; //Doh! - OpenCOLLADA store all sets contiguously in the same array
                }
                    break;
                case MeshVertexData::DATA_TYPE_DOUBLE: {
                    /*
                     sourceType = DOUBLE;
                     
                     const DoubleArray& array = vertexData.getDoubleValues()[indexOfSet];
                     const size_t count = array.getCount();
                     sourceData = (void*)array.getData();
                     sourceSize = count * sizeof(double);
                     */
                    // Warning if can't make "safe" conversion
                }
                    
                    break;
                default:
                case MeshVertexData::DATA_TYPE_UNKNOWN:
                    //FIXME report error
                    break;
            }
            
            // FIXME: the source could be shared, store / retrieve it here
            shared_ptr <GLTFBufferView> cvtBufferView = createBufferViewWithAllocatedBuffer(name, sourceData, 0, sourceSize, false);
            shared_ptr <GLTFMeshAttribute> cvtMeshAttribute(new GLTFMeshAttribute());
            
            cvtMeshAttribute->setBufferView(cvtBufferView);
            cvtMeshAttribute->setComponentsPerAttribute(size);
            cvtMeshAttribute->setByteStride(stride);
            cvtMeshAttribute->setComponentType(componentType);
            cvtMeshAttribute->setCount(elementsCount);
            
            meshAttributes[(unsigned int)indexOfSet] = cvtMeshAttribute;
        }
        
        return (unsigned int)setCount;
    }
    
    static void __AppendIndices(shared_ptr <GLTF::GLTFPrimitive> &primitive, IndicesVector &primitiveIndicesVector, shared_ptr <GLTF::GLTFIndices> &indices, GLTF::Semantic semantic, unsigned int indexOfSet)
    {
        primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>( new GLTF::JSONVertexAttribute(semantic,indexOfSet)));
        primitiveIndicesVector.push_back(indices);
    }
    
    static void __HandleIndexList(unsigned int idx,
                                  IndexList *indexList,
                                  Semantic semantic,
                                  bool shouldTriangulate,
                                  unsigned int count,
                                  unsigned int vcount,
                                  unsigned int *verticesCountArray,
                                  shared_ptr <GLTF::GLTFPrimitive> cvtPrimitive,
                                  IndicesVector &primitiveIndicesVector
                                  )
    {
        unsigned int triangulatedIndicesCount = 0;
        bool ownData = false;
        unsigned int *indices = indexList->getIndices().getData();
        
        if (shouldTriangulate) {
            indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
            count = triangulatedIndicesCount;
            ownData = true;
        }
        
        //Why is OpenCOLLADA doing this ? why adding an offset the indices ??
        //We need to offset it backward here.
        unsigned int initialIndex = indexList->getInitialIndex();
        if (initialIndex != 0) {
            unsigned int *bufferDestination = 0;
            if (!ownData) {
                bufferDestination = (unsigned int*)malloc(sizeof(unsigned int) * count);
                ownData = true;
            } else {
                bufferDestination = indices;
            }
            for (size_t idx = 0 ; idx < count ; idx++) {
                bufferDestination[idx] = indices[idx] - initialIndex;
            }
            indices = bufferDestination;
        }
        
        shared_ptr <GLTF::GLTFBufferView> uvBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), ownData);
        
        //FIXME: Looks like for texcoord indexSet begin at 1, this is out of the sync with the index used in ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes that begins at 0
        //for now forced to 0, to be fixed for multi texturing.
        
        //unsigned int idx = (unsigned int)indexList->getSetIndex();
        
        shared_ptr <GLTFIndices> jsonIndices(new GLTFIndices(uvBuffer, count));
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, jsonIndices, semantic, idx);
    }
    
    static shared_ptr <GLTF::GLTFPrimitive> ConvertOpenCOLLADAMeshPrimitive(
                                                                            COLLADAFW::MeshPrimitive *openCOLLADAMeshPrimitive,
                                                                            IndicesVector &primitiveIndicesVector)
    {
        shared_ptr <GLTF::GLTFPrimitive> cvtPrimitive(new GLTF::GLTFPrimitive());
        
        // We want to match OpenGL/ES mode , as WebGL spec points to OpenGL/ES spec...
        // "Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES are accepted."
        std::string type;
        bool shouldTriangulate = false;
        
        switch(openCOLLADAMeshPrimitive->getPrimitiveType()) {
                //these 2 requires transforms
            case MeshPrimitive::POLYLIST:
            case MeshPrimitive::POLYGONS:
                // FIXME: perform conversion, but until not done report error
                //these mode are supported by WebGL
                shouldTriangulate = true;
                //force triangles
                type = "TRIANGLES";
                break;
            case  MeshPrimitive::LINES:
                type = "LINES";
                break;
            case  MeshPrimitive::LINE_STRIPS:
                type = "LINE_STRIP";
                break;
                
            case  MeshPrimitive::TRIANGLES:
                type = "TRIANGLES";
                break;
                
            case  MeshPrimitive::TRIANGLE_FANS:
                type = "TRIANGLE_FANS";
                break;
                
            case  MeshPrimitive::TRIANGLE_STRIPS:
                type = "TRIANGLE_STRIPS";
                break;
                
            case  MeshPrimitive::POINTS:
                type = "POINTS";
                break;
            default:
                break;
        }
        
        cvtPrimitive->setMaterialObjectID((unsigned int)openCOLLADAMeshPrimitive->getMaterialId());
        cvtPrimitive->setType(type);
        
        //count of indices , it must be the same for all kind of indices
        size_t count = openCOLLADAMeshPrimitive->getPositionIndices().getCount();
        
        //vertex
        //IndexList &positionIndexList = openCOLLADAMeshPrimitive->getPositionIndices();
        unsigned int *indices = openCOLLADAMeshPrimitive->getPositionIndices().getData();
        unsigned int *verticesCountArray = 0;
        unsigned int vcount = 0;   //count of elements in the array containing the count of indices per polygon & polylist.
        
        if (shouldTriangulate) {
            unsigned int triangulatedIndicesCount = 0;
            //We have to upcast to polygon to retrieve the array of vertexCount
            //OpenCOLLADA use polylist as polygon.
            COLLADAFW::Polygons *polygon = (COLLADAFW::Polygons*)openCOLLADAMeshPrimitive;
            const COLLADAFW::Polygons::VertexCountArray& vertexCountArray = polygon->getGroupedVerticesVertexCountArray();
            vcount = (unsigned int)vertexCountArray.getCount();
            verticesCountArray = (unsigned int*)malloc(sizeof(unsigned int) * vcount);
            for (size_t i = 0; i < vcount; i++) {
                verticesCountArray[i] = polygon->getGroupedVerticesVertexCount(i);;
            }
            indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
            count = triangulatedIndicesCount;
        }
        
        shared_ptr <GLTFBufferView> positionBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), shouldTriangulate ? true : false);
        
        shared_ptr <GLTF::GLTFIndices> positionIndices(new GLTF::GLTFIndices(positionBuffer,count));
        
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, positionIndices, POSITION, 0);
        
        if (openCOLLADAMeshPrimitive->hasNormalIndices()) {
            unsigned int triangulatedIndicesCount = 0;
            indices = openCOLLADAMeshPrimitive->getNormalIndices().getData();
            if (shouldTriangulate) {
                indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
                count = triangulatedIndicesCount;
            }
            
            shared_ptr <GLTF::GLTFBufferView> normalBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), shouldTriangulate ? true : false);
            shared_ptr <GLTF::GLTFIndices> normalIndices(new GLTF::GLTFIndices(normalBuffer,
                                                                               count));
            __AppendIndices(cvtPrimitive, primitiveIndicesVector, normalIndices, NORMAL, 0);
        }
        
        if (openCOLLADAMeshPrimitive->hasColorIndices()) {
            IndexListArray& colorListArray = openCOLLADAMeshPrimitive->getColorIndicesArray();
            for (size_t i = 0 ; i < colorListArray.getCount() ; i++) {
                IndexList* indexList = openCOLLADAMeshPrimitive->getColorIndices(i);
                __HandleIndexList(i,
                                  indexList,
                                  GLTF::COLOR,
                                  shouldTriangulate,
                                  count,
                                  vcount,
                                  verticesCountArray,
                                  cvtPrimitive,
                                  primitiveIndicesVector);
            }
        }
        
        if (openCOLLADAMeshPrimitive->hasUVCoordIndices()) {
            IndexListArray& uvListArray = openCOLLADAMeshPrimitive->getUVCoordIndicesArray();
            for (size_t i = 0 ; i < uvListArray.getCount() ; i++) {
                IndexList* indexList = openCOLLADAMeshPrimitive->getUVCoordIndices(i);
                __HandleIndexList(i,
                                  indexList,
                                  GLTF::TEXCOORD,
                                  shouldTriangulate,
                                  count,
                                  vcount,
                                  verticesCountArray,
                                  cvtPrimitive,
                                  primitiveIndicesVector);
            }
        }
        
        if (verticesCountArray) {
            free(verticesCountArray);
        }
        
        return cvtPrimitive;
    }
    
    static void __InvertV(void *value,
                          GLTF::ComponentType type,
                          size_t componentsPerAttribute,
                          size_t index,
                          size_t vertexAttributeByteSize,
                          void *context) {
        char* bufferData = (char*)value;
        
        if (componentsPerAttribute > 1) {
            switch (type) {
                case GLTF::FLOAT: {
                    float* vector = (float*)bufferData;
                    vector[1] = (float) (1.0 - vector[1]);
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    void ConvertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh,
                                MeshVector &meshes)
    {
        shared_ptr <GLTF::GLTFMesh> cvtMesh(new GLTF::GLTFMesh());
        
        cvtMesh->setID(openCOLLADAMesh->getOriginalId());
        cvtMesh->setName(openCOLLADAMesh->getName());
        
        const MeshPrimitiveArray& primitives =  openCOLLADAMesh->getMeshPrimitives();
        size_t primitiveCount = primitives.getCount();
        
        std::vector< shared_ptr<IndicesVector> > allPrimitiveIndicesVectors;
        
        // get all primitives
        for (size_t i = 0 ; i < primitiveCount ; i++) {
            const MeshPrimitive::PrimitiveType primitiveType = primitives[i]->getPrimitiveType();
            if ((primitiveType != MeshPrimitive::TRIANGLES) &&
                (primitiveType != MeshPrimitive::TRIANGLE_STRIPS) &&
                (primitiveType != MeshPrimitive::POLYLIST) &&
                (primitiveType != MeshPrimitive::POLYGONS))
                continue;
            
            shared_ptr <GLTF::IndicesVector> primitiveIndicesVector(new GLTF::IndicesVector());
            allPrimitiveIndicesVectors.push_back(primitiveIndicesVector);
            
            shared_ptr <GLTF::GLTFPrimitive> primitive = ConvertOpenCOLLADAMeshPrimitive(primitives[i],*primitiveIndicesVector);
            cvtMesh->appendPrimitive(primitive);
            
            VertexAttributeVector vertexAttributes = primitive->getVertexAttributes();
            primitiveIndicesVector = allPrimitiveIndicesVectors[i];
            
            // once we got a primitive, keep track of its meshAttributes
            std::vector< shared_ptr<GLTF::GLTFIndices> > allIndices = *primitiveIndicesVector;
            for (size_t k = 0 ; k < allIndices.size() ; k++) {
                shared_ptr<GLTF::GLTFIndices> indices = allIndices[k];
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
                GLTF::IndexSetToMeshAttributeHashmap& meshAttributes = cvtMesh->getMeshAttributesForSemantic(semantic);
                
                switch (semantic) {
                    case GLTF::POSITION:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getPositions(), meshAttributes);
                        break;
                        
                    case GLTF::NORMAL:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getNormals(), meshAttributes);
                        break;
                        
                    case GLTF::TEXCOORD:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getUVCoords(), meshAttributes);
                        break;
                        
                    case GLTF::COLOR:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getColors(), meshAttributes);
                        break;
                        
                    default:
                        break;
                }
                
                cvtMesh->setMeshAttributesForSemantic(semantic, meshAttributes);
            }
        }
        
        //https://github.com/KhronosGroup/collada2json/issues/41
        //Goes through all texcoord and invert V
        GLTF::IndexSetToMeshAttributeHashmap& texcoordMeshAttributes = cvtMesh->getMeshAttributesForSemantic(GLTF::TEXCOORD);
        GLTF::IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
        
        //FIXME: consider turn this search into a method for mesh
        for (meshAttributeIterator = texcoordMeshAttributes.begin() ; meshAttributeIterator != texcoordMeshAttributes.end() ; meshAttributeIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <GLTF::GLTFMeshAttribute> meshAttribute = (*meshAttributeIterator).second;
            
            meshAttribute->apply(__InvertV, NULL);
        }
        
        if (cvtMesh->getPrimitives().size() > 0) {
            //After this point cvtMesh should be referenced anymore and will be deallocated
            shared_ptr <GLTF::GLTFMesh> unifiedMesh = CreateUnifiedIndexesMeshFromMesh(cvtMesh.get(), allPrimitiveIndicesVectors);
            if  (CreateMeshesWithMaximumIndicesCountFromMeshIfNeeded(unifiedMesh.get(), 65535, meshes) == false) {
                meshes.push_back(unifiedMesh);
            }
        }
    }
    
    //--------------------------------------------------------------------
	COLLADA2GLTFWriter::COLLADA2GLTFWriter( const GLTFConverterContext &converterArgs, PrettyWriter <FileStream> *jsonWriter ):
    _converterContext(converterArgs),
    _visualScene(0)
	{
        this->_writer.setWriter(jsonWriter);
	}
    
	//--------------------------------------------------------------------
	COLLADA2GLTFWriter::~COLLADA2GLTFWriter()
	{
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::reportError( const std::string& method, const std::string& message)
	{
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::write()
	{
        ifstream inputVertices;
        ifstream inputIndices;
        ifstream inputAnimations;
        ofstream verticesOutputStream;
        
        /*
         1. We output vertices and indices separatly in 2 different files
         2. Then output them in a single file
         */
        this->_converterContext.shaderIdToShaderString.clear();
        this->_converterContext._uniqueIDToMeshes.clear();
        COLLADABU::URI inputURI(this->_converterContext.inputFilePath.c_str());
        COLLADABU::URI outputURI(this->_converterContext.outputFilePath.c_str());
        
        std::string sharedVerticesBufferID = inputURI.getPathFileBase() + "vertices" + ".bin";
        std::string sharedIndicesBufferID = inputURI.getPathFileBase() + "indices" + ".bin";
        std::string sharedAnimationsBufferID = inputURI.getPathFileBase() + "animations" + ".bin";
        std::string sharedBufferID = inputURI.getPathFileBase() + ".bin";
        std::string outputVerticesFilePath = outputURI.getPathDir() + sharedVerticesBufferID;
        std::string outputIndicesFilePath = outputURI.getPathDir() + sharedIndicesBufferID;
        std::string outputAnimationsFilePath = outputURI.getPathDir() + sharedAnimationsBufferID;
        std::string outputFilePath = outputURI.getPathDir() + sharedBufferID;
        
        this->_verticesOutputStream.open (outputVerticesFilePath.c_str(), ios::out | ios::ate | ios::binary);
        this->_indicesOutputStream.open (outputIndicesFilePath.c_str(), ios::out | ios::ate | ios::binary);
        this->_animationsOutputStream.open (outputAnimationsFilePath.c_str(), ios::out | ios::ate | ios::binary);
        verticesOutputStream.open (outputFilePath.c_str(), ios::out | ios::ate | ios::binary);
        
        this->_converterContext.root = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
        this->_converterContext.root->setString("profile", "WebGL 1.0");
        this->_converterContext.root->setString("version", "0.3");
        this->_converterContext.root->setValue("nodes", shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject()));
        
        COLLADASaxFWL::Loader loader;
		COLLADAFW::Root root(&loader, this);
        
		if (!root.loadDocument( this->_converterContext.inputFilePath))
			return false;
        
        
        //reopen .bin files for vertices and indices
        size_t verticesLength = this->_verticesOutputStream.tellp();
        size_t indicesLength = this->_indicesOutputStream.tellp();
        size_t animationsLength = this->_animationsOutputStream.tellp();
        
        this->_verticesOutputStream.flush();
        this->_verticesOutputStream.close();
        this->_indicesOutputStream.flush();
        this->_indicesOutputStream.close();
        this->_animationsOutputStream.flush();
        this->_animationsOutputStream.close();
        
        inputVertices.open(outputVerticesFilePath.c_str(), ios::in | ios::binary);
        inputIndices.open(outputIndicesFilePath.c_str(), ios::in | ios::binary);
        inputAnimations.open(outputAnimationsFilePath.c_str(), ios::in | ios::binary);
        
        char* bufferIOStream = (char*)malloc(sizeof(char) * verticesLength);
        inputVertices.read(bufferIOStream, verticesLength);
        verticesOutputStream.write(bufferIOStream, verticesLength);
        free(bufferIOStream);
        bufferIOStream = (char*)malloc(sizeof(char) * indicesLength);
        inputIndices.read(bufferIOStream, indicesLength);
        verticesOutputStream.write(bufferIOStream, indicesLength);
        free(bufferIOStream);
        bufferIOStream = (char*)malloc(sizeof(char) * animationsLength);
        inputAnimations.read(bufferIOStream, animationsLength);
        verticesOutputStream.write(bufferIOStream, animationsLength);
        free(bufferIOStream);
        
        inputVertices.close();
        inputIndices.close();
        inputAnimations.close();
        
        remove(outputIndicesFilePath.c_str());
        remove(outputVerticesFilePath.c_str());
        remove(outputAnimationsFilePath.c_str());
        
        //---
        
        shared_ptr <GLTFBuffer> sharedBuffer(new GLTFBuffer(sharedBufferID, verticesLength + indicesLength + animationsLength));
        
        shared_ptr <GLTFBufferView> verticesBufferView(new GLTFBufferView(sharedBuffer, 0, verticesLength));
        shared_ptr <GLTFBufferView> indicesBufferView(new GLTFBufferView(sharedBuffer, verticesLength, indicesLength));
        shared_ptr <GLTFBufferView> animationsBufferView(new GLTFBufferView(sharedBuffer, verticesLength + indicesLength, animationsLength));
        
        // ----
        UniqueIDToMeshes::const_iterator UniqueIDToMeshesIterator;
        shared_ptr <GLTF::JSONObject> meshesObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("meshes", meshesObject);
        
        for (UniqueIDToMeshesIterator = this->_converterContext._uniqueIDToMeshes.begin() ; UniqueIDToMeshesIterator != this->_converterContext._uniqueIDToMeshes.end() ; UniqueIDToMeshesIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            MeshVectorSharedPtr meshes = (*UniqueIDToMeshesIterator).second;
            if (meshes) {
                for (size_t j = 0 ; j < meshes->size() ; j++) {
                    shared_ptr<GLTFMesh> mesh = (*meshes)[j];
                    if (mesh) {
                        /* some exporter bring meshes not used in the scene graph,
                         for the moment we have to remove these meshes because this involves the side effect of not having a material assigned. Which makes it incomplete.
                         */
                        bool shouldSkipMesh = false;
                        PrimitiveVector primitives = mesh->getPrimitives();
                        for (size_t k = 0 ; k < primitives.size() ; k++) {
                            shared_ptr <GLTF::GLTFPrimitive> primitive = primitives[k];
                            if (primitive->getMaterialID().length() == 0)
                                shouldSkipMesh  = true;
                        }
                        if (shouldSkipMesh)
                            continue;
                        
                        void *buffers[2];
                        buffers[0] = (void*)verticesBufferView.get();
                        buffers[1] = (void*)indicesBufferView.get();
                        
                        shared_ptr <GLTF::JSONObject> meshObject = serializeMesh(mesh.get(), (void*)buffers);
                        
                        meshesObject->setValue(mesh->getID(), meshObject);
                    }
                }
            }
        }
        
        // ----
        shared_ptr <GLTF::JSONObject> materialsObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("materials", materialsObject);
        
        UniqueIDToEffect::const_iterator UniqueIDToEffectIterator;
        
        for (UniqueIDToEffectIterator = this->_converterContext._uniqueIDToEffect.begin() ; UniqueIDToEffectIterator != this->_converterContext._uniqueIDToEffect.end() ; UniqueIDToEffectIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <GLTF::GLTFEffect> effect = (*UniqueIDToEffectIterator).second;
            if (effect->getTechniqueID() != "") {
                shared_ptr <GLTF::JSONObject> effectObject = serializeEffect(effect.get(), 0);
                //FIXME:HACK: effects are exported as materials
                materialsObject->setValue(effect->getID(), effectObject);
            }
        }
        
        // ----
        UniqueIDToAnimation::const_iterator UniqueIDToAnimationsIterator;
        shared_ptr <GLTF::JSONObject> animationsObject(new GLTF::JSONObject());
        
        this->_converterContext.root->setValue("animations", animationsObject);
        
        for (UniqueIDToAnimationsIterator = this->_converterContext._uniqueIDToAnimation.begin() ; UniqueIDToAnimationsIterator != this->_converterContext._uniqueIDToAnimation.end() ; UniqueIDToAnimationsIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            
            shared_ptr<GLTFAnimation> animation = (*UniqueIDToAnimationsIterator).second;
            std::vector <shared_ptr <GLTFAnimation::Parameter> > *parameters = animation->parameters();
            for (size_t i = 0 ; i < animation->parameters()->size() ; i++) {
                shared_ptr <GLTFAnimation::Parameter> parameter = (*parameters)[i];
                parameter->setBufferView(animationsBufferView);
            }
            
            if (animation->channels()->getAllKeys().size() > 0) {
                shared_ptr <JSONObject> animationObject = serializeAnimation(animation.get());
            
                animationsObject->setValue(animation->getID(), animationObject);
            }
        }
        
        shared_ptr <JSONObject> buffersObject(new JSONObject());
        shared_ptr <JSONObject> bufferObject = serializeBuffer(sharedBuffer.get(), 0);
        
        this->_converterContext.root->setValue("buffers", buffersObject);
        buffersObject->setValue(sharedBufferID, bufferObject);
        
        //FIXME: below is an acceptable short-cut since in this converter we will always create one buffer view for vertices and one for indices.
        //Fabrice: Other pipeline tools should be built on top of the format manipulate the buffers and end up with a buffer / bufferViews layout that matches the need of a given application for performance. For instance we might want to concatenate a set of geometry together that come from different file and call that a "level" for a game.
        shared_ptr <JSONObject> bufferViewsObject(new JSONObject());
        this->_converterContext.root->setValue("bufferViews", bufferViewsObject);
        
        shared_ptr <JSONObject> bufferViewIndicesObject = serializeBufferView(indicesBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewVerticesObject = serializeBufferView(verticesBufferView.get(), 0);
        shared_ptr <JSONObject> bufferViewAnimationsObject = serializeBufferView(animationsBufferView.get(), 0);
        bufferViewsObject->setValue(indicesBufferView->getID(), bufferViewIndicesObject);
        bufferViewsObject->setValue(verticesBufferView->getID(), bufferViewVerticesObject);
        bufferViewsObject->setValue(animationsBufferView->getID(), bufferViewAnimationsObject);
        bufferViewIndicesObject->setString("target", "ELEMENT_ARRAY_BUFFER");
        bufferViewVerticesObject->setString("target", "ARRAY_BUFFER");
        
        //---
        
        this->_converterContext.root->write(&this->_writer);
        
        bool sceneFlatteningEnabled = false;
        if (sceneFlatteningEnabled) {
            //second pass to actually output the JSON of the node themselves (and for all sub nodes)
            processSceneFlatteningInfo(&this->_sceneFlatteningInfo);
        }
        
        verticesOutputStream.flush();
        verticesOutputStream.close();
        
		return true;
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::cancel( const std::string& errorMessage )
	{
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::start()
	{
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::finish()
	{
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeGlobalAsset( const COLLADAFW::FileInfo* asset )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
    
    static void __GetFloatArrayFromMatrix(const COLLADABU::Math::Matrix4 &matrix, float *m) {
        shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());
        
        for (int i = 0 ; i < 4 ; i++)  {
            const COLLADABU::Math::Real * real = matrix[i];
            
            m[(i*4) + 0] = real[0];
            m[(i*4) + 1] = real[1];
            m[(i*4) + 2] = real[2];
            m[(i*4) + 3] = real[3];
        }
    }
    
    shared_ptr <GLTF::JSONArray> COLLADA2GLTFWriter::serializeMatrix4Array(const COLLADABU::Math::Matrix4 &matrix)
    {
        float m[16];
        shared_ptr <GLTF::JSONArray> array(new GLTF::JSONArray());
        COLLADABU::Math::Matrix4 transpose = matrix.transpose();
        
        __GetFloatArrayFromMatrix(transpose, m);
        
        for (int i = 0 ; i < 16; i++)  {
            array->appendValue(shared_ptr <GLTF::JSONValue> (new GLTF::JSONNumber(m[i])));
        }
        
        return array;
    }
    
    float COLLADA2GLTFWriter::getTransparency(const COLLADAFW::EffectCommon* effectCommon)
    {
        //super naive for now, also need to check sketchup work-around
        if (effectCommon->getOpacity().isTexture()) {
            return 1;
        }
        float transparency = static_cast<float>(effectCommon->getOpacity().getColor().getAlpha());
        
        return this->_converterContext.invertTransparency ? 1 - transparency : transparency;
    }
    
    float COLLADA2GLTFWriter::isOpaque(const COLLADAFW::EffectCommon* effectCommon)
    {
        return getTransparency(effectCommon)  >= 1;
    }
    
    void COLLADA2GLTFWriter::registerObjectWithUniqueUID(std::string objectUID, shared_ptr <JSONObject> obj, shared_ptr <JSONObject> objLib)
    {
        if (this->_converterContext._uniqueIDToTrackedObject.count(objectUID) == 0) {
            if (!objLib->contains(objectUID)) {
                objLib->setValue(objectUID, obj);
                this->_converterContext._uniqueIDToTrackedObject[objectUID] = obj;
            } else {
                printf("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", objectUID.c_str());
            }
        } else {
            printf("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", objectUID.c_str());
        }
    }

    bool COLLADA2GLTFWriter::writeNode( const COLLADAFW::Node* node,
                                       shared_ptr <GLTF::JSONObject> nodesObject,
                                       COLLADABU::Math::Matrix4 parentMatrix,
                                       SceneFlatteningInfo* sceneFlatteningInfo)
    {
        bool shouldExportTRS = false;
        const NodePointerArray& nodes = node->getChildNodes();
        const std::string& nodeUID = uniqueIdWithType("node", node->getUniqueId());
        COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
        
        shared_ptr <GLTF::JSONObject> nodeObject(new GLTF::JSONObject());
        nodeObject->setString("name",node->getName());
        
        bool nodeContainsLookAtTr = false;
        const InstanceCameraPointerArray& instanceCameras = node->getInstanceCameras();
        size_t camerasCount = instanceCameras.getCount();
        if (camerasCount > 0) {
            InstanceCamera* instanceCamera = instanceCameras[0];
            shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
            
            std::string cameraId = uniqueIdWithType("camera", instanceCamera->getInstanciatedObjectId());
            nodeObject->setString("camera", cameraId);
            
            //FIXME: just handle the first camera within a node now
            //for (size_t i = 0 ; i < camerasCount ; i++) {
            //}
            //Checks if we have a "look at" transformm because it is not handled by getTransformationMatrix when baking the matrix. (TODO: file a OpenCOLLADA issue for that).
            const TransformationPointerArray& transformations = node->getTransformations();
            size_t transformationsCount = transformations.getCount();
            for (size_t i = 0 ; i < transformationsCount ; i++) {
                const Transformation* tr = transformations[i];
                
                if (tr->getTransformationType() == Transformation::LOOKAT) {
                    Lookat* lookAt = (Lookat*)tr;
                    buildLookAtMatrix(lookAt, matrix);
                    nodeContainsLookAtTr = true;
                    break;
                }
            }
            
            if (nodeContainsLookAtTr && (transformationsCount > 1)) {
                //FIXME: handle warning/error
                printf("WARNING: node contains a look at transform combined with other transforms\n");
            }
        }
        
        if (!nodeContainsLookAtTr) {
            node->getTransformationMatrix(matrix);
        }
        
        float scale[3];
        float translation[3];
        float rotation[4];
        
        const TransformationPointerArray& transformations = node->getTransformations();
        size_t transformationsCount = transformations.getCount();
        for (size_t i = 0 ; i < transformationsCount ; i++) {
            const Transformation* tr = transformations[i];
            const UniqueId& animationListID = tr->getAnimationList();
            if (!animationListID.isValid())
                continue;
            shared_ptr<AnimatedTargets> animatedTargets(new AnimatedTargets());
            this->_converterContext._uniqueIDToAnimatedTargets[animationListID.getObjectId()] = animatedTargets;
            shared_ptr <JSONObject> animatedTarget(new JSONObject());
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::MATRIX)  {
                animatedTarget->setString("target", nodeUID);
                animatedTarget->setString("path", "MATRIX");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::TRANSLATE)  {
                animatedTarget->setString("target", nodeUID);
                animatedTarget->setString("path", "translation");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }

            
            if (tr->getTransformationType() == COLLADAFW::Transformation::ROTATE)  {
                animatedTarget->setString("target", nodeUID);
                animatedTarget->setString("path", "rotation");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
        }
        
        
        const COLLADABU::Math::Matrix4 worldMatrix = parentMatrix * matrix;
                
        if (shouldExportTRS) {
            __DecomposeMatrix(matrix, translation, rotation, scale);
            
            nodeObject->setValue("translation", serializeVec3(translation[0], translation[1], translation[2]));
            nodeObject->setValue("rotation", serializeVec4(rotation[0], rotation[1], rotation[2], rotation[3]));
            nodeObject->setValue("scale", serializeVec3(scale[0], scale[1], scale[2]));
            
        } else {
            nodeObject->setValue("matrix", this->serializeMatrix4Array(matrix));
        }
        
        
        // save mesh
		const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();
        
        unsigned int count = (unsigned int)instanceGeometries.getCount();
        if (count > 0) {
            shared_ptr <GLTF::JSONArray> meshesArray(new GLTF::JSONArray());
            nodeObject->setValue("meshes", meshesArray);
            
            for (unsigned int i = 0 ; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                
                unsigned int meshUID = (unsigned int)instanceGeometry->getInstanciatedObjectId().getObjectId();
                MeshVectorSharedPtr meshes = this->_converterContext._uniqueIDToMeshes[meshUID];
                
                if (meshes)
                {
                    for (size_t meshIndex = 0 ; meshIndex < meshes->size() ; meshIndex++) {
                        shared_ptr <GLTFMesh> mesh = (*meshes)[meshIndex];
                        
                        if (!mesh) {
                            continue;
                        }
                        
                        if (sceneFlatteningInfo) {
                            GLTF::IndexSetToMeshAttributeHashmap& semanticMap = mesh->getMeshAttributesForSemantic(GLTF::POSITION);
                            shared_ptr <GLTF::GLTFMeshAttribute> vertexMeshAttribute = semanticMap[0];
                            
                            BBOX vertexBBOX(COLLADABU::Math::Vector3(vertexMeshAttribute->getMin()),
                                            COLLADABU::Math::Vector3(vertexMeshAttribute->getMax()));
                            vertexBBOX.transform(worldMatrix);
                            
                            sceneFlatteningInfo->sceneBBOX.merge(&vertexBBOX);
                        }
                        
                        PrimitiveVector primitives = mesh->getPrimitives();
                        for (size_t j = 0 ; j < primitives.size() ; j++) {
                            shared_ptr <GLTF::GLTFPrimitive> primitive = primitives[j];
                            
                            //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
                            int materialBindingIndex = -1;
                            for (size_t k = 0; k < materialBindings.getCount() ; k++) {
//                                printf("materialID in binding:%d primitveObjectID:%d\n", materialBindings[k].getMaterialId(),
                                    //   primitive->getMaterialObjectID());
                                
                                if (materialBindings[k].getMaterialId() == primitive->getMaterialObjectID()) {
                                    materialBindingIndex = (unsigned int)k;
                                }
                            }
                            
                            std::map<std::string , std::string > texcoordBindings;
                            
                            if (materialBindingIndex != -1) {
                                unsigned int referencedMaterialID = (unsigned int)materialBindings[materialBindingIndex].getReferencedMaterial().getObjectId();
                                
                                /* will be needed to get semantic & set association to create the shader */
                                const TextureCoordinateBindingArray &textureCoordBindings = materialBindings[materialBindingIndex].getTextureCoordinateBindingArray();
                                
                                unsigned int effectID = this->_converterContext._materialUIDToEffectUID[referencedMaterialID];
                                std::string materialName = this->_converterContext._materialUIDToName[referencedMaterialID];
                                shared_ptr <GLTFEffect> effect = this->_converterContext._uniqueIDToEffect[effectID];
                                
                                // retrieve the semantic to be associated
                                size_t coordBindingsCount = textureCoordBindings.getCount();
                                if (coordBindingsCount > 0) {
                                    //some models come with a setIndex > 0, we do not handle this, we need to find what's the minimum index and substract it to ensure start at set=0
                                    size_t minimumIndex = textureCoordBindings[0].getSetIndex();
                                    for (size_t coordIdx = 1 ; coordIdx < coordBindingsCount ; coordIdx++) {
                                        if (textureCoordBindings[coordIdx].getSetIndex() < minimumIndex)
                                            minimumIndex = textureCoordBindings[coordIdx].getSetIndex();
                                    }
                                    
                                    for (size_t coordIdx = 0 ; coordIdx < coordBindingsCount ; coordIdx++) {
                                        std::string texcoord = textureCoordBindings[coordIdx].getSemantic();
                                        SemanticArrayPtr semanticArrayPtr = effect->getSemanticsForTexcoordName(texcoord);
                                        
                                        std::string shaderSemantic = "TEXCOORD_"+ GLTFUtils::toString(textureCoordBindings[coordIdx].getSetIndex() - minimumIndex);
                                        
                                        if (semanticArrayPtr) {
                                            for (size_t semanticIndex = 0 ; semanticIndex < semanticArrayPtr->size() ; semanticIndex++){
                                                std::string slot = (*semanticArrayPtr)[semanticIndex];
                                                texcoordBindings[slot] = shaderSemantic;
                                            }
                                        }
                                    }
                                }
                                
                                //generate shaders if needed
                                shared_ptr<JSONObject> technique = effect->getTechnique();
                                const std::string& techniqueID = getReferenceTechniqueID(technique, texcoordBindings, this->_converterContext);
                                
                                effect->setTechniqueID(techniqueID);
                                
                                effect->setName(materialName);
                                primitive->setMaterialID(effect->getID());
                            }
                            //printf(":::: %s\n",primitive->getMaterialID().c_str());

                        }


                        meshesArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(mesh->getID())));
                        if (sceneFlatteningInfo) {
                            shared_ptr <MeshFlatteningInfo> meshFlatteningInfo(new MeshFlatteningInfo(meshUID, parentMatrix));
                            sceneFlatteningInfo->allMeshes.push_back(meshFlatteningInfo);
                        }
                    }
                }
            }
        }
        
        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        nodeObject->setValue("children", childrenArray);
        
        count = (unsigned int)nodes.getCount();
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            std::string id = uniqueIdWithType("node", nodes[i]->getUniqueId());
            childrenArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(id)));
        }
        
        registerObjectWithUniqueUID(nodeUID, nodeObject, nodesObject);
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            this->writeNode(nodes[i], nodesObject, worldMatrix, sceneFlatteningInfo);
        }
        
        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = (unsigned int)instanceNodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++) {
            InstanceNode* instanceNode  = instanceNodes[i];
            
            std::string id = uniqueIdWithType("node", instanceNode->getInstanciatedObjectId());
            childrenArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(id)));
        }
        
        return true;
    }
    
    // Flattening   [UNFINISHED CODE]
    //conditions for flattening
    // -> same material
    // option to merge of not non-opaque geometry
    //  -> check per primitive that sources / semantic layout matches
    // [meshAttributes]
    // -> for all meshes
    //   -> collect all kind of semantic
    // -> for all meshes
    //   -> get all meshAttributes
    //   -> transforms & write vtx attributes
    bool COLLADA2GLTFWriter::processSceneFlatteningInfo(SceneFlatteningInfo* sceneFlatteningInfo)
    {
        /*
         MeshFlatteningInfoVector allMeshes = sceneFlatteningInfo->allMeshes;
         //First collect all kind of meshAttributes available
         size_t count = allMeshes.size();
         for (size_t i = 0 ; i < count ; i++) {
         shared_ptr <MeshFlatteningInfo> meshInfo = allMeshes[i];
         MeshVectorSharedPtr *meshes = this->_uniqueIDToMeshes[meshInfo->getUID()];
         // shared_ptr <MeshAttributeVector> meshAttributes = mesh->meshAttributes();
         }
         */
        return true;
    }
    
    bool COLLADA2GLTFWriter::writeVisualScene( const COLLADAFW::VisualScene* visualScene )
	{
        //FIXME: only one visual scene assumed/handled
        shared_ptr <GLTF::JSONObject> scenesObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> sceneObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> nodesObject = static_pointer_cast <GLTF::JSONObject> (this->_converterContext.root->getValue("nodes"));
        shared_ptr <GLTF::JSONObject> rootObject(new GLTF::JSONObject());
        
		const NodePointerArray& nodePointerArray = visualScene->getRootNodes();
        size_t nodeCount = nodePointerArray.getCount();
        
        this->_converterContext.root->setValue("scenes", scenesObject);
        this->_converterContext.root->setString("scene", "defaultScene");
        
        sceneObject->setString("node", "root");
        scenesObject->setValue("defaultScene", sceneObject); //FIXME: should use this id -> visualScene->getOriginalId()
        nodesObject->setValue("root", rootObject);
        
        //first pass to output children name of our root node
        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        
        for (size_t i = 0 ; i < nodeCount ; i++) {
            std::string id = uniqueIdWithType("node", nodePointerArray[i]->getUniqueId());
            
            shared_ptr <GLTF::JSONString> nodeIDValue(new GLTF::JSONString(id));
            childrenArray->appendValue(static_pointer_cast <GLTF::JSONValue> (nodeIDValue));
        }
        
        rootObject->setValue("children", childrenArray);
        
        for (size_t i = 0 ; i < nodeCount ; i++) {
            //FIXME: &this->_sceneFlatteningInfo
            this->writeNode(nodePointerArray[i], nodesObject, COLLADABU::Math::Matrix4::IDENTITY, NULL);
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeScene( const COLLADAFW::Scene* scene )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes )
	{
        const NodePointerArray& nodes = libraryNodes->getNodes();
        
        shared_ptr <GLTF::JSONObject> nodesObject = static_pointer_cast <GLTF::JSONObject> (this->_converterContext.root->getValue("nodes"));
        
        size_t count = nodes.getCount();
        for (size_t i = 0 ; i < count ; i++) {
            const COLLADAFW::Node *node = nodes[i];
            
            if (!this->writeNode(node,  nodesObject, COLLADABU::Math::Matrix4::IDENTITY, 0))
                return false;
        }
        
        return true;
	}
    
	//--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeGeometry( const COLLADAFW::Geometry* geometry )
	{
        switch (geometry->getType()) {
            case Geometry::GEO_TYPE_MESH:
            {
                const COLLADAFW::Mesh* mesh = (COLLADAFW::Mesh*)geometry;
                unsigned int meshID = (unsigned int)geometry->getUniqueId().getObjectId();
                MeshVectorSharedPtr meshes;
                
                if (this->_converterContext._uniqueIDToMeshes.count(meshID) == 0) {
                    meshes =  shared_ptr<MeshVector> (new MeshVector);
                    
                    ConvertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, (*meshes));
                    
                    if (meshes->size()) {
                        for (size_t i = 0 ; i < meshes->size() ; i++) {
                            if ((*meshes)[i]->getPrimitives().size() > 0) {
                                (*meshes)[i]->writeAllBuffers(this->_verticesOutputStream, this->_indicesOutputStream);
                            }
                        }
                    }
                    
                    this->_converterContext._uniqueIDToMeshes[meshID] = meshes;
                }
            }
                break;
            case Geometry::GEO_TYPE_SPLINE:
            case Geometry::GEO_TYPE_CONVEX_MESH:
                // FIXME: handle convertion to mesh
            case Geometry::GEO_TYPE_UNKNOWN:
                //FIXME: handle error
                return false;
        }
        
        return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeMaterial( const COLLADAFW::Material* material )
	{
        const UniqueId& effectUID = material->getInstantiatedEffect();
		unsigned int materialID = (unsigned int)material->getUniqueId().getObjectId();
        this->_converterContext._materialUIDToName[materialID] = material->getName();
        this->_converterContext._materialUIDToEffectUID[materialID] = (unsigned int)effectUID.getObjectId();
		return true;
	}
    
    
	//--------------------------------------------------------------------
    //FIXME: should be different depending on profiles. now assuming WebGL
	static std::string __GetGLWrapMode(COLLADAFW::Sampler::WrapMode wrapMode) {
        switch (wrapMode) {
            case COLLADAFW::Sampler::WRAP_MODE_UNSPECIFIED:
            case COLLADAFW::Sampler::WRAP_MODE_NONE:
            case COLLADAFW::Sampler::WRAP_MODE_WRAP:
                return "REPEAT";
            case COLLADAFW::Sampler::WRAP_MODE_MIRROR:
                return "MIRRORED_REPEAT";
            case COLLADAFW::Sampler::WRAP_MODE_CLAMP:
                return "CLAMP_TO_EDGE";
            default:
                break;
        }
        return "REPEAT";
    }
    
    static std::string __GetFilterMode(COLLADAFW::Sampler::SamplerFilter wrapMode) {
        switch (wrapMode) {
            case COLLADAFW::Sampler::SAMPLER_FILTER_UNSPECIFIED:
            case COLLADAFW::Sampler::SAMPLER_FILTER_NONE:
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR:
                return "LINEAR";
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST:
                return "NEAREST";
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST:
                return "NEAREST_MIPMAP_NEAREST";
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST:
                return "LINEAR_MIPMAP_NEAREST";
            case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR:
                return "NEAREST_MIPMAP_LINEAR";
            case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR:
                return "LINEAR_MIPMAP_LINEAR";
            default:
                break;
        }
        return "LINEAR";
    }
    
    void COLLADA2GLTFWriter::handleEffectSlot(const COLLADAFW::EffectCommon* commonProfile,
                                              std::string slotName,
                                              shared_ptr <GLTFEffect> cvtEffect)
    {
        shared_ptr <JSONObject> technique = cvtEffect->getTechnique();
        shared_ptr <JSONObject> parameters = technique->getObject("parameters");
        
        ColorOrTexture slot;
        
        if (slotName == "diffuse")
            slot = commonProfile->getDiffuse();
        else if (slotName == "ambient")
            slot = commonProfile->getAmbient();
        else if (slotName == "emission")
            slot = commonProfile->getEmission();
        else if (slotName == "specular")
            slot = commonProfile->getSpecular();
        else if (slotName == "reflective")
            slot = commonProfile->getReflective();
        else
            return;
        
        
        //retrieve the type, parameterName -> symbol -> type
        double red = 1, green = 1, blue = 1;
        if (slot.isColor()) {
            const Color& color = slot.getColor();
            if (slot.getType() != COLLADAFW::ColorOrTexture::UNSPECIFIED) {
                red = color.getRed();
                green = color.getGreen();
                blue = color.getBlue();
            }
            shared_ptr <JSONObject> slotObject(new JSONObject());
            slotObject->setValue("value", serializeVec3(red,green,blue));
            slotObject->setString("type", "FLOAT_VEC3");
            
            parameters->setValue(slotName, slotObject);
            
        } else if (slot.isTexture()) {
            
            const Texture&  texture = slot.getTexture();
            const SamplerPointerArray& samplers = commonProfile->getSamplerPointerArray();
            const Sampler* sampler = samplers[texture.getSamplerId()];
            const UniqueId& imageUID = sampler->getSourceImage();
            
            shared_ptr <JSONObject> sampler2D(new JSONObject());
            
            std::string texcoord = texture.getTexcoord();
            
            cvtEffect->addSemanticForTexcoordName(texcoord, slotName);
            
            //FIXME: this is assume SAMPLER_2D
            sampler2D->setString("wrapS", __GetGLWrapMode(sampler->getWrapS()));
            sampler2D->setString("wrapT", __GetGLWrapMode(sampler->getWrapT()));
            sampler2D->setString("minFilter", __GetFilterMode(sampler->getMinFilter()));
            sampler2D->setString("magFilter", __GetFilterMode(sampler->getMagFilter()));
            sampler2D->setString("image", uniqueIdWithType("image",imageUID));
            sampler2D->setString("type", "SAMPLER_2D");
            parameters->setValue(slotName, sampler2D);
        } else {
            // nothing to do, no texture or color
        }
    }
    
    bool COLLADA2GLTFWriter::writeEffect( const COLLADAFW::Effect* effect )
	{
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
        
        if (commonEffects.getCount() > 0) {
            std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS
            uniqueId += "material.";
#else
            uniqueId += "effect.";
#endif
            uniqueId += GLTF::GLTFUtils::toString(effect->getUniqueId().getObjectId());;
            
            shared_ptr <GLTFEffect> cvtEffect(new GLTFEffect(uniqueId));
            shared_ptr <JSONObject> technique(new JSONObject());
            shared_ptr <JSONObject> parameters(new JSONObject());
            
            cvtEffect->setTechnique(technique);
            technique->setValue("parameters", parameters);
            
            const COLLADAFW::EffectCommon* effectCommon = commonEffects[0];
            
            handleEffectSlot(effectCommon,"diffuse" , cvtEffect);
            handleEffectSlot(effectCommon,"ambient" , cvtEffect);
            handleEffectSlot(effectCommon,"emission" , cvtEffect);
            handleEffectSlot(effectCommon,"specular" , cvtEffect);
            handleEffectSlot(effectCommon,"reflective" , cvtEffect);
            
            if (!isOpaque(effectCommon)) {
                shared_ptr <JSONObject> transparency(new JSONObject());
                double transparencyValue = this->getTransparency(effectCommon);
                
                transparency->setDouble("value", transparencyValue);
                transparency->setString("type", "FLOAT");
                
                parameters->setValue("transparency", transparency);
            }
            
            //should check if has specular first and the lighting model (if not lambert)
            shared_ptr <JSONObject> shininessObject(new JSONObject());
            double shininess = effectCommon->getShininess().getFloatValue();
            shininessObject->setDouble("value", shininess);
            shininessObject->setString("type", "FLOAT");
            parameters->setValue("shininess", shininessObject);
            
            this->_converterContext._uniqueIDToEffect[(unsigned int)effect->getUniqueId().getObjectId()] = cvtEffect;
            
        }
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeCamera( const COLLADAFW::Camera* camera )
	{
        shared_ptr <GLTF::JSONObject> camerasObject = static_pointer_cast <GLTF::JSONObject> (this->_converterContext.root->getValue("cameras"));
        if (!camerasObject) {
            camerasObject = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
            this->_converterContext.root->setValue("cameras", camerasObject);
        }
        
        shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
        
        std::string id = uniqueIdWithType("camera", camera->getUniqueId());
        
        camerasObject->setValue(id, cameraObject);
        
        switch (camera->getCameraType()) {
            case Camera::UNDEFINED_CAMERATYPE:
                printf("WARNING: unknown camera type: using perspective\n");
                cameraObject->setString("cameraType", "perspective");
                break;
            case Camera::ORTHOGRAPHIC:
            {
                cameraObject->setString("projection", "orthographic");
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
            case Camera::PERSPECTIVE:
            {
                cameraObject->setString("projection", "perspective");
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
        }
        
        cameraObject->setDouble("znear", camera->getNearClippingPlane().getValue());
        cameraObject->setDouble("zfar", camera->getFarClippingPlane().getValue());
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeImage( const COLLADAFW::Image* openCOLLADAImage )
	{
        shared_ptr <GLTF::JSONObject> images = this->_converterContext.root->createObjectIfNeeded("images");
        shared_ptr <GLTF::JSONObject> image(new GLTF::JSONObject());
        
        images->setValue(uniqueIdWithType("image",openCOLLADAImage->getUniqueId()), image);
        /*
         bool relativePath = true;
         if (pathDir.size() > 0) {
         if ((pathDir[0] != '.') || (pathDir[0] == '/')) {
         relativePath = false;
         }
         }*/
        const COLLADABU::URI &imageURI = openCOLLADAImage->getImageURI();
        std::string relPathFile = imageURI.getPathFile();
        if (imageURI.getPathDir() != "./") {
            relPathFile = imageURI.getPathDir() + imageURI.getPathFile();
        } else {
            relPathFile = imageURI.getPathFile();
        }
        image->setString("path", relPathFile);
        
        this->_converterContext._imageIdToImagePath[uniqueIdWithType("image",openCOLLADAImage->getUniqueId()) ] = relPathFile;
        return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLight( const COLLADAFW::Light* light )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimation( const COLLADAFW::Animation* animation )
	{
        shared_ptr <GLTFAnimation> cvtAnimation = ConvertOpenCOLLADAAnimationToGLTFAnimation(animation);
        
        this->_converterContext._uniqueIDToAnimation[animation->getUniqueId().getObjectId()] = cvtAnimation;
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimationList( const COLLADAFW::AnimationList* animationList )
	{
        const COLLADAFW::AnimationList::AnimationBindings &animationBindings = animationList->getAnimationBindings();
        
        AnimatedTargetsSharedPtr animatedTargets = this->_converterContext._uniqueIDToAnimatedTargets[animationList->getUniqueId().getObjectId()];
        
        for (size_t i = 0 ; i < animationBindings.getCount() ; i++) {
            shared_ptr <GLTFAnimation> cvtAnimation = this->_converterContext._uniqueIDToAnimation[animationBindings[i].animation.getObjectId()];
            const COLLADAFW::AnimationList::AnimationClass animationClass = animationBindings[i].animationClass;
            if (!writeAnimation(cvtAnimation, animationClass, animatedTargets, this->_animationsOutputStream)) {
                this->_converterContext._uniqueIDToAnimation.erase(this->_converterContext._uniqueIDToAnimation.find(animationBindings[i].animation.getObjectId()));
            }
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeController( const COLLADAFW::Controller* Controller )
	{
		return true;
	}
    
} // namespace COLLADA2JSON
