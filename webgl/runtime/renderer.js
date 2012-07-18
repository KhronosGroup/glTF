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

define(["runtime/glsl-program", "helpers/resource-manager", "dependencies/gl-matrix", "dependencies/webgl-texture-utils"], function(GLSLProgram, ResourceManager) {

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
    
        setState: {
            value: function(stateID, flag) {
                switch (stateID) {
                    case this.webGLContext.BLEND:
                        if (flag) {
                            this.webGLContext.enable(this.webGLContext)
                        } else {
                            this.webGLContext.disable(this.webGLContext)
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
                this.setState(this.BLEND, true);
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
                
                var transparency = 1;
                if (program.getLocationForSymbol("u_transparency")) {
                    transparency = primitive.material.inputs.transparency;
                    if (typeof transparency === "undefined") {
                        transparency = 1;
                    }
                    program.setValueForSymbol("u_transparency",transparency);
                }

                if (program.getLocationForSymbol("u_diffuseColor")) {
                    var color = primitive.material.inputs.diffuseColor;
                    var step = primitive.step * primitive.step;
                    var oneMinusPrimitiveStep = 1 - (1 * step);
                    var colorStep = [((oneMinusPrimitiveStep) + (step * color[0])), 
                                    ((oneMinusPrimitiveStep) + (step * color[1])),
                                    ((oneMinusPrimitiveStep) + (step * color[2]))];

                    program.setValueForSymbol("u_diffuseColor",colorStep);
                }

                if (program.getLocationForSymbol("u_diffuseTexture")) {
                    var image = primitive.material.inputs.diffuseTexture;
                    var texture = this.resourceManager.getResource(image, this.textureDelegate, this.webGLContext);
                    if (texture) {
                        gl.activeTexture(gl.TEXTURE0);
                        gl.bindTexture(gl.TEXTURE_2D, texture);
                        var samplerLocation = program.getValueForSymbol("u_diffuseTexture");
                        if (typeof samplerLocation === "undefined") {
                            program.setValueForSymbol("u_diffuseTexture", 0);
                        }
                    }
                }
                program.commit(gl);
            
                var available = true;
                //Bind Attribute
                primitive.vertexAttributes.forEach( function(vertexAttribute) {

                    var accessor = vertexAttribute.accessor;
                    var symbol = materialSemantic[vertexAttribute.semantic];
                    if (symbol) {
                        //FIXME: do not assume continuous data, this will break with interleaved arrays (should not use byteStride here).
                        var glResource = this.resourceManager.getResource(accessor, this.vertexAttributeBufferDelegate, this.webGLContext);
                        // this call will bind the resource when available
                        if (glResource) {
                            gl.bindBuffer(gl.ARRAY_BUFFER, glResource);
                        } else {
                             available = false;
                        }

                        if (available) {
                            var attributeLocation = program.getLocationForSymbol(symbol);
                            if (typeof attributeLocation !== "undefined") {
                                
                                if (attributeLocation > newMaxEnabledArray) {
                                    newMaxEnabledArray = attributeLocation;
                                }

                                //Just enable what was not enabled before...
                                if (this._lastMaxEnabledArray < attributeLocation) {
                                    gl.enableVertexAttribArray(attributeLocation);
                                } 
                                gl.vertexAttribPointer(attributeLocation, accessor.elementsPerValue, gl.FLOAT, false, accessor.byteStride, 0);
                                if ( renderVertices && (vertexAttribute.semantic == "VERTEX")) {
                                   gl.drawArrays(gl.POINTS, 0, accessor.count);
                                }
                            }
                        }
                    }                
                
                }, this);
                if (!renderVertices)  { 
                    //Just disable what is not required here…
                    if (available) {
                        for (var i = (newMaxEnabledArray + 1); i < this._lastMaxEnabledArray ; i++) {
                            gl.disableVertexAttribArray(i);
                        }
                        if (primitive.step < 1.0)
                            primitive.step += 0.05;
                    }
              
                    var glIndices = null;
                    //FIXME should not assume 2 bytes per indices (WebGL supports one byte too…)
                    glIndices = this.resourceManager.getResource(primitive.indices, this.indicesDelegate, this.webGLContext);              
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

                        if (pass.states) {
                            if (pass.states.BLEND) {
                                gl.enable(gl.BLEND);
                                gl.blendFunc (gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
                            } else {
                                gl.disable(gl.BLEND);
                            }
                        }

                        this.bindedProgram = glProgram;
                        for (var i = 0 ; i < count ; i++) {
                            this.renderPrimitive(primitives[i]);
                        }
                    }
                }
            }
        }

    });
    
        return Renderer;
    }
);
