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
var global = window;
(function (root, factory) {
    if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like enviroments that support module.exports,
        // like Node.
      
        module.exports = factory(global);
        module.exports.Utilities = module.exports;
    } else if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
        define([], function () {
            return factory(root);
        });
    } else {
        // Browser globals
        factory(root);
    }
}(this, function (root) {
    
    if (typeof exports === 'object') {
        require("dependencies/gl-matrix");
    } 

    var Utilities = Object.create(Object.prototype, {

        vec3Min: {
            value: function(min, aVec) {
                if (aVec[0] < min[0]) {
                    min[0] = aVec[0];
                }
                if (aVec[1] < min[1]) {
                    min[1] = aVec[1];
                }
                if (aVec[2] < min[2]) {
                    min[2] = aVec[2];
                }
            }
        },

        vec3Max: {
            value: function(max, aVec) {
                if (aVec[0] > max[0]) {
                    max[0] = aVec[0];
                }
                if (aVec[1] > max[1]) {
                    max[1] = aVec[1];
                }
                if (aVec[2] > max[2]) {
                    max[2] = aVec[2];
                }
            }
        },

        mergeBBox: {
            value: function(boxa, boxb) {

                var minBoxA = boxa[0];
                var maxBoxA = boxa[1];
                var minBoxB = boxb[0];
                var maxBoxB = boxb[1];

                var min = [ (minBoxA[0] < minBoxB[0]) ? minBoxA[0] : minBoxB[0],
                            (minBoxA[1] < minBoxB[1]) ? minBoxA[1] : minBoxB[1],
                            (minBoxA[2] < minBoxB[2]) ? minBoxA[2] : minBoxB[2] ];

                var max = [ (maxBoxA[0] > maxBoxB[0]) ? maxBoxA[0] : maxBoxB[0],
                            (maxBoxA[1] > maxBoxB[1]) ? maxBoxA[1] : maxBoxB[1],
                            (maxBoxA[2] > maxBoxB[2]) ? maxBoxA[2] : maxBoxB[2] ];

                return [min, max];
            }
        },

        computeBBOXFromVec3Array: {
            value: function(vectors) {
                if (vectors.length === 0)
                    return null;

                var min = [vectors[0][0], vectors[0][1], vectors[0][2]];
                var max = [vectors[0][0], vectors[0][1], vectors[0][2]];

                for (var i = 0 ; i < 8 ; i++)
                {
                    if (vectors[i][0] < min[0])
                        min[0] = vectors[i][0];
                    if (vectors[i][1] < min[1])
                        min[1] = vectors[i][1];
                    if (vectors[i][2] < min[2])
                        min[2] = vectors[i][2];

                    if (vectors[i][0] > max[0])
                        max[0] = vectors[i][0];
                    if (vectors[i][1] > max[1])
                        max[1] = vectors[i][1];
                    if (vectors[i][2] > max[2])
                        max[2] = vectors[i][2];
                }
              return [min, max];
            }
        },

        transformBBox: {
            value: function(box, transform) {
                var min = box[0];
                var max = box[1];

                var X = 0;
                var Y = 1;
                var Z = 2;

                var pt0 = mat4.multiplyVec3(transform, vec3.createFrom(min[X], min[Y], min[Z]));
                var pt1 = mat4.multiplyVec3(transform, vec3.createFrom(max[X], min[Y], min[Z]));
                var pt2 = mat4.multiplyVec3(transform, vec3.createFrom(min[X], max[Y], min[Z]));
                var pt3 = mat4.multiplyVec3(transform, vec3.createFrom(max[X], max[Y], min[Z]));
                var pt4 = mat4.multiplyVec3(transform, vec3.createFrom(min[X], min[Y], max[Z]));
                var pt5 = mat4.multiplyVec3(transform, vec3.createFrom(max[X], min[Y], max[Z]));
                var pt6 = mat4.multiplyVec3(transform, vec3.createFrom(min[X], max[Y], max[Z]));
                var pt7 = mat4.multiplyVec3(transform, vec3.createFrom(max[X], max[Y], max[Z]));

                return this.computeBBOXFromVec3Array([pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7]);
            }
        },


            rayIntersectsMesh: {
                value: function(ray, mesh, worldMatrix, results, options) {
                    var verticesBuffer = mesh.verticesBuffer;
                    if (verticesBuffer === null) {
                        return false;
                    }
                    var hasResults = false;
                    var count = mesh.primitives.length;
                    var vert0 = vec3.create();
                    var vert1 = vec3.create();
                    var vert2 = vec3.create();
                    for (var i = 0 ; i < count ; i++) {
                        var primitive = mesh.primitives[i];
                        
                        if (options) {
                            var materials = options.materials;
                            if (materials) {
                                if (materials.indexOf(primitive.material["name"]) == -1) {
                                    continue;
                                } 
                            }
                        }

                        if ((primitive.material.inputs.transparency !== null) && (typeof primitive.material.inputs.transparency !== "undefined")) {
                            if (primitive.material.inputs.transparency < 1)
                                continue;
                        }
                        
                        var bbox = primitive.boundingBox;
                        if (!this.intersectsBBOX(bbox, ray))
                            continue;
                        return true;
                    }
                    return hasResults;
                }
            },

            
            /*
                An Efficient and Robust Ray–Box Intersection Algorithm  
                Amy Williams Steve Barrus R. Keith Morley Peter Shirley University of Utah
                Non optimized version
            */
            intersectsBBOX: {
                value: function(bbox, ray) {

                    var X = 0;
                    var Y = 1;
                    var Z = 2;
                    var tmin, tmax;
                    var tymin, tymax;
                    var tzmin, tzmax;
                    
                    var direction =  ray[1];
                    var origin = ray[0];

                    var min = bbox[0];
                    var max = bbox[1];

                    if (direction[X] >= 0) {
                        tmin = (min[X] - origin[X]) / direction[X];
                        tmax = (max[X] - origin[X]) / direction[X];
                    } else {
                        tmin = (max[X] - origin[X]) / direction[X];
                        tmax = (min[X] - origin[X]) / direction[X];
                    }

                    if (direction[Y] >= 0) {
                        tymin = (min[Y] - origin[Y]) / direction[Y];
                        tymax = (max[Y] - origin[Y]) / direction[Y];
                    } else {
                        tymin = (max[Y] - origin[Y]) / direction[Y];
                        tymax = (min[Y] - origin[Y]) / direction[Y];
                    }

                    if ( (tmin > tymax) || (tymin > tmax) ) {
                        return false;
                    }

                    if (tymin > tmin)
                        tmin = tymin;
                    if (tymax < tmax)
                        tmax = tymax;

                    if (direction[Z] >= 0) {
                        tzmin = (min[Z] - origin[Z]) / direction[Z];
                        tzmax = (max[Z] - origin[Z]) / direction[Z];
                    } else {
                        tzmin = (max[Z] - origin[Z]) / direction[Z];
                        tzmax = (min[Z] - origin[Z]) / direction[Z];
                    }

                    if ( (tmin > tzmax) || (tzmin > tmax) ) {
                        return false;
                    }
                    
                    if (tzmin > tmin)
                        tmin = tzmin;
                    if (tzmax < tmax)
                        tmax = tzmax;

                    return true;
                }
            }


    });

    if(root) {
        root.Utilities = Utilities;
    }

    return Utilities;

}));