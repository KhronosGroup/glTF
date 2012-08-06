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

require("dependencies/gl-matrix");
var GLSLProgram = require("glsl-program").GLSLProgram;
var ResourceManager = require("c2j/helpers/resource-manager").ResourceManager;

exports.Renderer = Object.create(Object, {

    shininess: { value: 200, writable: true },

    light: { value: [0, 0, -1], writable: true },

    specularColor: { value: [1, 1, 1], writable: true },

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
    
        _lambertProgram: { value: null, writable: true },

        _resourceManager: { value: null, writable: true },
    
        _webGLContext: { value : null, writable: true },
    
        _projectionMatrix: { value : null, writable: true },

        projectionMatrix: {
            get: function() {
                return this._projectionMatrix;
            },
            set: function(value) {
                this._projectionMatrix = value;
            }
        },

        debugProgram: {
            get: function() {            
            
                if (!this._debugProgram) {
                    this._debugProgram = Object.create(GLSLProgram);
                    
                    var debugVS = "precision highp float;" +
                                        "attribute vec3 vert;"  +
                                        "uniform mat4 u_mvMatrix; " +
                                        "uniform mat4 u_projMatrix; " +
                                        "void main(void) { " +
                                        "gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0); }" 
                
                    var debugFS = "precision highp float;" +
                    " void main(void) { " +
                    " gl_FragColor = vec4(1.,0.,0.,1.); }";
                
                    this._debugProgram.initWithShaders( { "x-shader/x-vertex" : debugVS , "x-shader/x-fragment" : debugFS } );
                    if (!this._debugProgram.build(this.webGLContext)) {
                        console.log(this._debugProgram.errorLogs);                     
                    } 

                }
                        
                return this._debugProgram;
            }
        },
    
        lambertProgram: {
            get: function() {            
            
                if (!this._lambertProgram) {
                    this._lambertProgram = Object.create(GLSLProgram);
                    
                    var lambertVS = "precision highp float;" +
                                        "attribute vec3 vert;"  +
                                        "attribute vec3 normal; " +
                                        "varying vec3 v_normal; " +
                                        "uniform mat4 u_mvMatrix; " +
                                        "uniform mat3 u_normalMatrix; " +
                                        "uniform mat4 u_projMatrix; " +
                                        "void main(void) { " +
                                        "v_normal = normalize(u_normalMatrix * normal); " + 
                                        "gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0); }" 
                
                    var lambertFS = "precision highp float;" +
                    " uniform vec3 color;" +
                    " varying vec3 v_normal;" +
                    " void main(void) { " +
                    " vec3 normal = normalize(v_normal); " +
                    " float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);" +
                    " gl_FragColor = vec4(color.xyz *lambert, 1.); }";

                    this._lambertProgram.initWithShaders( { "x-shader/x-vertex" : lambertVS , "x-shader/x-fragment" : lambertFS } );
                    if (!this._lambertProgram.build(this.webGLContext)) {
                        console.log(this._lambertProgram.errorLogs);                     
                    } 

                }
                        
                return this._lambertProgram;
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
                webGLContext:  {
                    value: null, writable: true
                },

                handleError: function(errorCode, info) {
                    // FIXME: report error
                    console.log("ERROR:vertexAttributeBufferDelegate:"+errorCode+" :"+info);
                },
        
                //should be called only once
                convert: function (resource, ctx) {
                    var gl = this.webGLContext;                
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
                webGLContext:  {
                    value: null, writable: true
                },

                handleError: function(errorCode, info) {
                    console.log("ERROR:vertexAttributeBufferDelegate:"+errorCode+" :"+info);
                },
        
                convert: function (resource, ctx) {
                    var gl = this.webGLContext;                
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

        textureDelegate: {
            value: {
                handleError: function(errorCode, info) {
                    // FIXME: report error
                    console.log("ERROR:textureDelegate:"+errorCode+" :"+info);
                },
        
                //should be called only once
                convert: function (resource, ctx) {
                    return resource;
                },
        
                resourceAvailable: function (glResource, ctx) {
                }
            }
        },
        
        _lastMaxEnabledArray: { value: 0, writable: true },
    
        _blend: { value: false },

        setState: {
            value: function(stateID, flag, force) {
                var gl = this.webGLContext;
                switch (stateID) {
                    case gl.BLEND:
                        if ((this._blend !== flag) || force) {
                            if (flag) {
                                gl.depthMask(false);
                                gl.enable(gl.BLEND);
                                gl.blendFunc (gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
                            } else {
                                gl.depthMask(true);
                                gl.disable(gl.BLEND);                               
                            }
                            this._blend = flag;
                        }
                    break; 

                    default:
                    break;
                }
            }
        },

        resetStates : {
            value: function() {
                var gl = this.webGLContext;
                if (gl && (this._lastMaxEnabledArray !== -1)) {
                    for (var i = 0 ; i < this._lastMaxEnabledArray ; i++) {
                        gl.disableVertexAttribArray(i);
                    }
                }
                this._lastMaxEnabledArray = -1;
                this.bindedProgram = null;
                this.setState(this.BLEND, false, true);
            }
        },
    
        renderPrimitive: {
            value: function(primitiveDescription) {
                var renderVertices = false;
                var worldMatrix = primitiveDescription.worldMatrix;
                var projectionMatrix = this.projectionMatrix;
                var primitive = primitiveDescription.primitive;
                var newMaxEnabledArray = -1;
                var gl = this.webGLContext;
                var program =  renderVertices ? this.debugProgram : this.bindedProgram;

                //FIXME: remove that association
                var materialSemantic = { "VERTEX" : "vert" , "NORMAL" : "normal", "TEXCOORD" : "texcoord" };

                //FIXME: should got through inputs without hardcoded symbols
                if (program.getLocationForSymbol("u_projMatrix")) {
                    program.setValueForSymbol("u_projMatrix",projectionMatrix);
                }
            
                if (program.getLocationForSymbol("u_normalMatrix")) {
                     program.setValueForSymbol("u_normalMatrix",primitiveDescription.normalMatrix);
                }
            
                if (program.getLocationForSymbol("u_mvMatrix")) {
                    program.setValueForSymbol("u_mvMatrix",worldMatrix);
                }

                var shininess = this.shininess;
                if (program.getLocationForSymbol("u_shininess")) {
                    shininess = primitive.material.inputs.shininess;
                    if ((typeof shininess === "undefined") || (shininess === null)) {
                        shininess = this.shininess;
                    }
                    program.setValueForSymbol("u_shininess",shininess);
                }

                var light = this.light;
                if (program.getLocationForSymbol("u_light")) {
                    light = primitive.material.inputs.light;
                    if ((typeof light === "undefined") || (light === null)) {
                        light = this.light;
                    }
                    program.setValueForSymbol("u_light",light);
                }

                var specularColor = this.specularColor;
                if (program.getLocationForSymbol("u_specularColor")) {
                    specularColor = primitive.material.inputs.specularColor;
                    if ((typeof specularColor === "undefined") || (specularColor === null)) {
                        specularColor = this.specularColor;
                    }
                    program.setValueForSymbol("u_specularColor",specularColor);
                } 

                var defaultReflectionIntensity = 0.3;
                var reflectionIntensity = defaultReflectionIntensity;
                if (program.getLocationForSymbol("u_reflectionIntensity")) {
                    reflectionIntensity = primitive.material.inputs.reflectionIntensity;
                    if ((typeof reflectionIntensity === "undefined") || (reflectionIntensity === null)) {
                        reflectionIntensity = defaultReflectionIntensity;
                    }
                    program.setValueForSymbol("u_reflectionIntensity",reflectionIntensity);
                }
                var defaultTransparency = 1.0;
                var transparency = defaultTransparency;
                if (program.getLocationForSymbol("u_transparency")) {
                    transparency = primitive.material.inputs.transparency;
                    if ((typeof transparency === "undefined") || (transparency === null)) {
                        transparency = defaultTransparency;
                    }
                    program.setValueForSymbol("u_transparency",transparency);
                }

                if (program.getLocationForSymbol("u_diffuseColor")) {
                    var color = primitive.material.inputs.diffuseColor;
                    program.setValueForSymbol("u_diffuseColor",color);
                }

                var currentTexture = 0;
                if (program.getLocationForSymbol("u_diffuseTexture")) {
                    var image = primitive.material.inputs.diffuseTexture;
                    var texture = this.resourceManager.getResource(image, this.textureDelegate, this.webGLContext);
                    if (texture) {
                        gl.activeTexture(gl.TEXTURE0 + currentTexture);
                        gl.bindTexture(gl.TEXTURE_2D, texture);
                        var samplerLocation = program.getLocationForSymbol("u_diffuseTexture");
                        if (typeof samplerLocation !== "undefined") {
                            program.setValueForSymbol("u_diffuseTexture", currentTexture);
                            currentTexture++;
                        }
                    }
                }

                if (program.getLocationForSymbol("u_reflectionTexture")) {
                    var image = primitive.material.inputs.reflectionTexture;
                    var texture = this.resourceManager.getResource(image, this.textureDelegate, this.webGLContext);
                    if (texture) {
                        gl.activeTexture(gl.TEXTURE0 + currentTexture);
                        gl.bindTexture(gl.TEXTURE_2D, texture);
                        var samplerLocation = program.getLocationForSymbol("u_reflectionTexture");
                        if (typeof samplerLocation !== "undefined") {
                            program.setValueForSymbol("u_reflectionTexture", currentTexture);
                            currentTexture++;
                        }
                    }
                }

                program.commit(gl);
            
                var availableCount = 0;
                //Bind Attribute
                primitive.vertexAttributes.forEach( function(vertexAttribute) {

                    var accessor = vertexAttribute.accessor;
                    var symbol = materialSemantic[vertexAttribute.semantic];
                    if (symbol) {
                        //FIXME: do not assume continuous data, this will break with interleaved arrays (should not use byteStride here).
                        this.vertexAttributeBufferDelegate.webGLContext = this.webGLContext;
                        var glResource = this.resourceManager.getResource(accessor, this.vertexAttributeBufferDelegate,  { "semantic": vertexAttribute.semantic, "primitive": primitive});
                        // this call will bind the resource when available
                        if (glResource) {
                            gl.bindBuffer(gl.ARRAY_BUFFER, glResource);

                            var attributeLocation = program.getLocationForSymbol(symbol);
                            if (typeof attributeLocation !== "undefined") {
                                
                                if (attributeLocation > newMaxEnabledArray) {
                                    newMaxEnabledArray = attributeLocation;
                                }

                                //Just enable what was not enabled before...
                                //FIXME: find root cause why it is needed to disable this optimization as it works well 99% of the time
                                //if (this._lastMaxEnabledArray < attributeLocation) {
                                    gl.enableVertexAttribArray(attributeLocation);
                                //} 
                                gl.vertexAttribPointer(attributeLocation, accessor.elementsPerValue, gl.FLOAT, false, accessor.byteStride, 0);
                                if ( renderVertices && (vertexAttribute.semantic == "VERTEX")) {
                                    gl.drawArrays(gl.POINTS, 0, accessor.count);
                                }
                            }

                            availableCount++;
                        } else {
                            this._lastMaxEnabledArray = -1;
                        }

                    }                
                }, this);

                var available = (availableCount === primitive.vertexAttributes.length);
                if (!renderVertices)  { 
                    //Just disable what is not required here…
                    if (available) {
                        for (var i = (newMaxEnabledArray + 1); i < this._lastMaxEnabledArray ; i++) {
                            gl.disableVertexAttribArray(i);
                        }
                    }
              
                    var glIndices = null;
                    //FIXME should not assume 2 bytes per indices (WebGL supports one byte too…)
                    this.indicesDelegate.webGLContext = this.webGLContext;
                    glIndices = this.resourceManager.getResource(primitive.indices, this.indicesDelegate, primitive);              
                    if (glIndices && available) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, glIndices);
                        gl.drawElements(gl.TRIANGLES, primitive.indices.length, gl.UNSIGNED_SHORT, 0);                            
                    }
                }
                this._lastMaxEnabledArray = newMaxEnabledArray;
                return available;
            }
        },

        programDelegate: {
            value: {
                handleError: function(errorCode, info) {
                    // FIXME: report error
                    console.log("ERROR:programDelegate:"+errorCode+" :"+info);
                },
        
                //should be called only once
                convert: function (resource, ctx) {
                    var gl = ctx;
                    var glslProgram = Object.create(GLSLProgram);
                    glslProgram.initWithShaders( resource );
                    if (!glslProgram.build(ctx)) {
                        console.log(resource)
                        console.log(glslProgram.errorLogs);                     
                    } 

                    return glslProgram;
                },
        
                resourceAvailable: function (glResource, ctx) {
                }
            }
        },

        renderPass: {
            value: function(pass) {
                var primitives = pass.primitives;
                var count = primitives.length;
                var gl = this.webGLContext;
                if (pass.program) {
                    var glProgram = this.resourceManager.getResource(pass.program, this.programDelegate, gl);
                    if (glProgram) {
                        var blending = false;
                        if (pass.states) {  
                            if (pass.states.BLEND) {
                                blending = true;
                            } 
                        } 
                        this.setState(gl.BLEND, blending);

                        this.bindedProgram = glProgram;
                        for (var i = 0 ; i < count ; i++) {
                            this.renderPrimitive(primitives[i]);
                        }
                    }
                }
            }
        }

});