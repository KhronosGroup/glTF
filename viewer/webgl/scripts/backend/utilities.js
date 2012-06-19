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

define(["dependencies/gl-matrix"], function() {

	var Utilities = Object.create(Object, {

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
        }
	});
	
		return Utilities;
	}
);
