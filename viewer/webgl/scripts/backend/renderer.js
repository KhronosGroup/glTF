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

define(["backend/glsl-program", "backend/resource-manager", "dependencies/gl-matrix"], function(GLSLProgram, ResourceManager) {

	var Renderer = Object.create(Object, {

    	//temporary state for testing
    	_bindedProgram: { value: null, writable: true },
    
    	bindedProgram: {
        	get: function() {
            	return this._bindedProgram;
        	},
        	set: function(value) {
            	if ((this._bindedProgram !== value) && this._webGLContext) {
                	this._bindedProgram = value;
                	if (this._bindedProgram) {
                    	this._bindedProgram.use(this._webGLContext, false);
                	} 
            	}
        	}
    	},
    
    	_debugProgram: { value: null, writable: true },
    
    	_resourceManager: { value: null, writable: true },
    
    	_webGLContext: { value : null, writable: true },
    
    	debugProgram: {
        	get: function() {            
            
            	if (!this._debugProgram) {
                	this._debugProgram = Object.create(GLSLProgram);
                
	                var vertexShader = "precision highp float;" +
    	                                "attribute vec3 vert;"  +
        	                            "attribute vec3 normal; " +
            	                        "varying vec3 v_normal; " +
                	                    "uniform mat4 u_mvMatrix; " +
                    	                //"uniform mat3 u_normalMatrix; " +
                        	            "uniform mat4 u_projMatrix; " +
                            	        "void main(void) { " +
                                	   	//"v_normal = normalize(u_normalMatrix * normal); " + 
                                    	"v_normal = normalize(mat3(u_mvMatrix) * normal); " + 
                                   		//"v_normal = vec3(1.);" +
                                    	"gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0); }" 
                
                	var fragmentShader = "precision highp float;" +
                	" uniform vec3 color;" +
                	" varying vec3 v_normal;" +
                	" void main(void) { " +
                	" vec3 normal = normalize(v_normal); " +
                	" float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);" +
                	" gl_FragColor = vec4(color.xyz * lambert, 1.); }";

                	var vertexShader1 = "precision highp float;" +
                    	                "attribute vec3 vert;"  +
                        	            "attribute vec3 normal; " +
                            	        "varying vec3 v_normal; " +
                                	    "uniform mat4 u_mvMatrix; " +
                                    	"uniform mat3 u_normalMatrix; " +
                                    	"uniform mat4 u_projMatrix; " +
                                    	"void main(void) { " +
                                    	"v_normal = normalize(u_normalMatrix * normal); " + 
                                    	"gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0); }" 
                
	                var fragmentShader1 = "precision highp float;" +
    	            " uniform vec3 color;" +
        	        " varying vec3 v_normal;" +
            	    " void main(void) { " +
                	" vec3 normal = normalize(v_normal); " +
               	 	" float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);" +
                	" gl_FragColor = vec4(color.xyz * lambert, 1.); }";
                
                	this._debugProgram.initWithShaders( { "x-shader/x-vertex" : vertexShader , "x-shader/x-fragment" : fragmentShader } );
                	if (!this._debugProgram.build(this.webGLContext)) {
                    	console.log(this._debugProgram.errorLogs);
                   		this._debugProgram = Object.create(GLSLProgram);
                    	this._debugProgram.initWithShaders( { "x-shader/x-vertex" : vertexShader1 , "x-shader/x-fragment" : fragmentShader1 } );
                    	if (!this._debugProgram.build(this.webGLContext)) {
                        	console.log(this._debugProgram.errorLogs);
                    	}
                     
                	} 
            	}
                        
	            return this._debugProgram;
    	    }
    	},
    
    	webGLContext: {
        	get: function() {
            	return this._webGLContext;
        	},
        	set: function(value) {
            	this._webGLContext = value;
        	}
    	},

    	resourceManager: {
        	get: function() {
            	if (!this._resourceManager) {
            		//FIXME: this should be in init
                	this._resourceManager = Object.create(ResourceManager);
                	this._resourceManager.init();
            	}
            
            	return this._resourceManager;
        	}
    	},
    
    	indicesDelegate: {
        	value: {
            	handleError: function(errorCode, info) {
                	// FIXME: report error
                	console.log("ERROR:vertexAttributeBufferDelegate:"+errorCode+" :"+info);
            	},
        
	            //should be called only once
    	        convert: function (resource, ctx) {
        	        var gl = ctx;                
            	    var previousBuffer = gl.getParameter(gl.ELEMENT_ARRAY_BUFFER_BINDING);

                	var glResource =  gl.createBuffer();                
                	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, glResource);
                
	                gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, resource, gl.STATIC_DRAW);                
    	            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, previousBuffer);
                                
        	        return glResource;
            	},
        
            	resourceAvailable: function (glResource, ctx) {
            	}
        	}
    	},
    
    	vertexAttributeBufferDelegate: {
        	value: {
            	handleError: function(errorCode, info) {
                	// FIXME: report error
                	console.log("ERROR:vertexAttributeBufferDelegate:"+errorCode+" :"+info);
            	},
        
            	//should be called only once
            	convert: function (resource, ctx) {
                	var gl = ctx;                

	                var previousBuffer = gl.getParameter(gl.ARRAY_BUFFER_BINDING);
	
    	            var glResource =  gl.createBuffer();                
        	        gl.bindBuffer(gl.ARRAY_BUFFER, glResource);
            	    gl.bufferData(gl.ARRAY_BUFFER, resource, gl.STATIC_DRAW);                

                	gl.bindBuffer(gl.ARRAY_BUFFER, previousBuffer);
                	return glResource;
            	},
        
            	resourceAvailable: function (glResource, ctx) {
            	}
        	}
    	},
    
	    //Debug test
    	isMat4Equals: {
        	value: function(matA, matB) {            
            	if (matA === matB)
                	return true;
            	return mat4.equal(matA,matB);
        	}
    	},

	    isMat3Equals: {
    	    value: function(matA, matB) {
        
        	    if (matA === matB)
            	    return true;
            	return mat3.equal(matA,matB);
	        }
    	},
    
    	isVec3Equals: {
        	value: function(vecA, vecB) {
        
            	if (vecA === vecB)
                	return true;

	            if ((vecA[0] === vecB[0]) && (vecA[1] === vecB[1]) && (vecA[2] === vecB[2]))
    	            return true;

	            return false;
    	    }
    	},
    
    
    	_lastMaxEnabledArray: { value: 0, writable: true },
    
    	resetStates : {
        	value: function() {
            	this._lastMaxEnabledArray = -1;
            	this.bindedProgram = null;
        	}
    	},
    
	    renderPrimitive: {
    	    value: function(primitiveDescription) {
        
        	    var worldMatrix = primitiveDescription.worldMatrix;
           	 	var projectionMatrix = primitiveDescription.projectionMatrix;
            	var primitive = primitiveDescription.primitive;
            	var newMaxEnabledArray = -1;
            	var gl = this.webGLContext;
            	var program = this.debugProgram;
            	var materialSemantic = { "VERTEX" : "vert" , "NORMAL" : "normal" };

	            //var mvpMatrix = mat4.create();
    	        //mat4.multiply(projectionMatrix, worldMatrix, mvpMatrix);
    	    	/*
	            if (program.getLocationForSymbol("u_mvpMatrix")) {
                var currentMVPMatrix = program.getValueForSymbol("u_mvpMatrix");
                if (currentMVPMatrix) {
                    if (!this.isMat4Equals(currentMVPMatrix, mvpMatrix)) {
                        program.setValueForSymbol("u_mvpMatrix",mvpMatrix);
                    }  
                } else {                
                    program.setValueForSymbol("u_mvpMatrix",mvpMatrix);
                }
           		 }
				*/
	            if (program.getLocationForSymbol("u_projMatrix")) {
    	            var currentProjectionMatrix = program.getValueForSymbol("u_projMatrix");
        	        if (currentProjectionMatrix) {
            	        if (!this.isMat4Equals(currentProjectionMatrix, projectionMatrix)) {
                    	    program.setValueForSymbol("u_projMatrix",projectionMatrix);
                	    }  
                	} else {                
                    	program.setValueForSymbol("u_projMatrix",projectionMatrix);
                	}
            	}
            
	            if (program.getLocationForSymbol("u_normalMatrix")) {
    	            var currentNormalMatrix = program.getValueForSymbol("u_normalMatrix");
        	        if (currentNormalMatrix) {
            	        if (!this.isMat3Equals(currentNormalMatrix, primitiveDescription.normalMatrix)) {
                	        program.setValueForSymbol("u_normalMatrix",primitiveDescription.normalMatrix);
                    	} 
                    } else {                
    	                program.setValueForSymbol("u_normalMatrix",primitiveDescription.normalMatrix);
        	        }
            	}
            
            	if (program.getLocationForSymbol("u_mvMatrix")) {
                	var currentWorldMatrix = program.getValueForSymbol("u_mvMatrix");
                	if (currentWorldMatrix) {
                    	if (!this.isMat4Equals(currentWorldMatrix, worldMatrix)) {
                        	program.setValueForSymbol("u_mvMatrix",worldMatrix);
                    	}                     
	                } else {                
                    	program.setValueForSymbol("u_mvMatrix",worldMatrix);
                	}
            	}
            
            	
            
	            if (program.getLocationForSymbol("color")) {
    	            var color = primitive.material.color;
        	        var step = primitive.step * primitive.step;
            	    var oneMinusPrimitiveStep = 1 - (1 * step);
                	var colorStep = [((oneMinusPrimitiveStep) + (step * color[0])), 
                    	            ((oneMinusPrimitiveStep) + (step * color[1])),
                        	        ((oneMinusPrimitiveStep) + (step * color[2]))];

	                var currentColor = program.getValueForSymbol("color");
    	            if (currentColor) {
        	            if (currentColor !== colorStep) {
            	            if (!this.isVec3Equals(currentColor,colorStep))
                	            program.setValueForSymbol("color",colorStep);
                    	}
                	} else {
                    	program.setValueForSymbol("color",colorStep);
                	}
            	}

            	this.bindedProgram = program;               
            	program.commit(gl);
            
	            var available = true;
    	        //Bind Attribute
        	    primitive.vertexAttributes.forEach( function(vertexAttribute) {
                	var accessor = vertexAttribute.accessor;
                	var symbol = materialSemantic[vertexAttribute.semantic];
                	if (symbol) {
                    	//FIXME: do not assume continuous data, this will break with interleaved arrays (should not use byteStride here).
                    	var range = [accessor.byteOffset ? accessor.byteOffset : 0 , (accessor.byteStride * accessor.count) + accessor.byteOffset];    
                    	var glResource = this.resourceManager.getWebGLResource(accessor.id, accessor.buffer, range, this.vertexAttributeBufferDelegate, this.webGLContext);
                    	// this call will bind the resource when available
                    	if (glResource) {
                       		gl.bindBuffer(gl.ARRAY_BUFFER, glResource);
                    	} else {
                        	 available = false;
                    	}
                    
                    	if (available) {
                        	var attributeLocation = this.debugProgram.getLocationForSymbol(symbol);
                        	if (typeof attributeLocation !== "undefined") {
                        
                            	if (attributeLocation > newMaxEnabledArray)
                                	newMaxEnabledArray = attributeLocation;
                        
                            	//Just enable what was not enabled before...
                            	if (this._lastMaxEnabledArray < attributeLocation) {
                                	gl.enableVertexAttribArray(attributeLocation);
                            	} 
	                            gl.vertexAttribPointer(attributeLocation, accessor.elementsPerValue, gl.FLOAT, false, accessor.byteStride, 0);
    	                    }
        	            }
            	    }                
                
            	}, this);
            	//Just disable what is not required here…
            	for (var i = (newMaxEnabledArray + 1); i < this._lastMaxEnabledArray ; i++)
                	gl.disableVertexAttribArray(i);
            	this._lastMaxEnabledArray = newMaxEnabledArray;
            
	            if (available)  { 
    	            if (primitive.step < 1.0)
        	            primitive.step += 0.05;
              
             	   var glIndices = null;
                	//FIXME should not assume 2 bytes per indices (WebGL supports one too…)
                	var range = [primitive.indices.byteOffset, primitive.indices.byteOffset + ( primitive.indices.length * Uint16Array.BYTES_PER_ELEMENT)];
                	glIndices = this.resourceManager.getWebGLResource(primitive.indices.id, primitive.indices.buffer, range, this.indicesDelegate, this.webGLContext);              
                	if (glIndices) {
                    	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, glIndices);
                    	gl.drawElements(gl.TRIANGLES, primitive.indices.length, gl.UNSIGNED_SHORT, 0);                            
                	}
            	}
        	}
    	}
	});
	
		return Renderer;
	}
);
