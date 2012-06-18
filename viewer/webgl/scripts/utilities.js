define(function() {

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
