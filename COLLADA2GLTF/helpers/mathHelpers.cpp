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

#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "mathHelpers.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

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
    static bool unmatrix(COLLADABU::Math::Matrix4 mat, double *tran)
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
        
        COLLADABU::Math::Matrix3 amat3(row[0][0], row[1][0], row[2][0],
                                       row[0][1], row[1][1], row[2][1],
                                       row[0][2], row[1][2], row[2][2]);
        COLLADABU::Math::Quaternion aquat;
        aquat.fromRotationMatrix(amat3);
        aquat.normalise();

        tran[U_ROTATEX] = aquat.x;
        tran[U_ROTATEY] = aquat.y;
        tran[U_ROTATEZ] = aquat.z;
        tran[U_ROTATEW] = aquat.w;
        
        return true;
    }
    
    void decomposeMatrix(COLLADABU::Math::Matrix4 &matrix, float *translation, float *rotation, float *scale) {
        COLLADABU::Math::Matrix4 tr = matrix.transpose();
        tr.setElement(0,3, 0);
        tr.setElement(1,3, 0);
        tr.setElement(2,3, 0);
        tr.setElement(3,3, 1);
        double tran[20];
        memset(tran, 0, sizeof(tran));
        
        // unmatrix can fail if the uniform scale is 0
        // This just makes sure we don't print the warning if
        //  we are trying to extract only scale because it's
        //  probably this valid case we are running into.
        if (!unmatrix(tr, tran) && (translation || rotation)) {
            printf("WARNING: matrix can't be decomposed \n");
        }
        
        if (translation) {
            translation[0] = (float)tran[U_TRANSX];
            translation[1] = (float)tran[U_TRANSY];
            translation[2] = (float)tran[U_TRANSZ];
        }
        
        if (rotation) {
            rotation[0] = (float)tran[U_ROTATEX];
            rotation[1] = (float)tran[U_ROTATEY];
            rotation[2] = (float)tran[U_ROTATEZ];
            rotation[3] = (float)tran[U_ROTATEW];
        }
        
        if (scale) {
            scale[0] = (float)tran[U_SCALEX];
            scale[1] = (float)tran[U_SCALEY];
            scale[2] = (float)tran[U_SCALEZ];
        }
    }
    
    //converted to C++ from gl-matrix by Brandon Jones ( https://github.com/toji/gl-matrix )
    void buildLookAtMatrix(COLLADAFW::Lookat *lookat, COLLADABU::Math::Matrix4& matrix)
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
    

}