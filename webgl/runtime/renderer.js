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
        module.exports.Renderer = module.exports;
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
    var GLSLProgram , ResourceManager;
    if (typeof exports === 'object') {
        require("dependencies/gl-matrix");
        GLSLProgram = require("glsl-program").GLSLProgram;
        ResourceManager = require("c2j/helpers/resource-manager").ResourceManager;
    } else {
        GLSLProgram = global.GLSLProgram;
        ResourceManager = global.ResourceManager;
    }


    var Renderer = Object.create(Object, {

         WORLD: { value: "WORLD", writable: false},
         VIEW: { value: "VIEW", writable: false},
         PROJECTION: { value: "PROJECTION", writable: false},
         WORLDVIEW: { value: "WORLDVIEW", writable: false},
         VIEWPROJECTION: { value: "VIEWPROJECTION", writable: false},
         WORLDVIEWPROJECTION: { value: "WORLDVIEWPROJECTION", writable: false},
         WORLDINVERSE: { value: "WORLDINVERSE", writable: false},
         VIEWINVERSE: { value: "VIEWINVERSE", writable: false},
         PROJECTIONINVERSE: { value: "PROJECTIONINVERSE", writable: false},
         WORLDVIEWINVERSE: { value: "WORLDVIEWINVERSE", writable: false},
         VIEWPROJECTIONINVERSE: { value: "VIEWPROJECTIONINVERSE", writable: false},
         WORLDVIEWPROJECTIONINVERSE: { value: "WORLDVIEWPROJECTIONINVERSE", writable: false},
         WORLDTRANSPOSE: { value: "WORLDTRANSPOSE", writable: false},
         VIEWTRANSPOSE: { value: "VIEWTRANSPOSE", writable: false},
         PROJECTIONTRANSPOSE: { value: "PROJECTIONTRANSPOSE", writable: false},
         WORLDVIEWTRANSPOSE: { value: "WORLDVIEWTRANSPOSE", writable: false},
         VIEWPROJECTIONTRANSPOSE: { value: "VIEWPROJECTIONTRANSPOSE", writable: false},
         WORLDVIEWPROJECTIONTRANSPOSE: { value: "WORLDVIEWPROJECTIONTRANSPOSE", writable: false},
         WORLDINVERSETRANSPOSE: { value: "WORLDINVERSETRANSPOSE", writable: false},
         VIEWINVERSETRANSPOSE: { value: "VIEWINVERSETRANSPOSE", writable: false},
         PROJECTIONINVERSETRANSPOSE: { value: "PROJECTIONINVERSETRANSPOSE", writable: false},
         WORLDVIEWINVERSETRANSPOSE: { value: "WORLDVIEWINVERSETRANSPOSE", writable: false},
         VIEWPROJECTIONINVERSETRANSPOSE: { value: "VIEWPROJECTIONINVERSETRANSPOSE", writable: false},
         WORLDVIEWPROJECTIONINVERSETRANSPOSE: { value: "WORLDVIEWPROJECTIONINVERSETRANSPOSE", writable: false},

        //private accessors
        _bindedProgram: { value: null, writable: true },

        _debugProgram: { value: null, writable: true },
        
        _lambertProgram: { value: null, writable: true },

        _resourceManager: { value: null, writable: true },
        
        _webGLContext: { value : null, writable: true },

        _projectionMatrix: { value : null, writable: true },

        //default values
        shininess: { value: 200, writable: true },

        light: { value: [0, 0, -1], writable: true },

        specularColor: { value: [1, 1, 1], writable: true },

        initWithWebGLContext: {
            value: function(value) {
                this.webGLContext = value;
                return this;
            }
        },

        init: {
            value: function() {
                return this;
            }
        },

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

        projectionMatrix: {
            get: function() {
                return this._projectionMatrix;
            },
            set: function(value) {
                this._projectionMatrix = value;
            }
        },

        //FIXME:needs to be updated to reflect latest changes
        debugProgram: {
            get: function() {            
                if (!this._debugProgram) {
                    this._debugProgram = Object.create(GLSLProgram);
                    var debugVS =   "precision highp float;" +
                                    "attribute vec3 vert;"  +
                                    "uniform mat4 u_mvMatrix; " +
                                    "uniform mat4 u_projMatrix; " +
                                    "void main(void) { " +
                                    "gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0); }" 
                    
                    var debugFS =   "precision highp float;" +
                                    "void main(void) { " +
                                    "gl_FragColor = vec4(1.,0.,0.,1.); }";
                    
                    this._debugProgram.initWithShaders( { "x-shader/x-vertex" : debugVS , "x-shader/x-fragment" : debugFS } );
                    if (!this._debugProgram.build(this.webGLContext)) {
                        console.log(this._debugProgram.errorLogs);                     
                    } 

                }
                            
                return this._debugProgram;
            }
        },

        //FIXME:needs to be updated to reflect latest changes
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
                    debugger;

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
                    debugger;

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
                webGLContext:  {
                    value: null, writable: true
                },

                getGLFilter: function(filter) {
                    var gl = this.webGLContext;
                    var glFilter = gl.LINEAR;
                    if (filter === "LINEAR") {
                        glFilter = gl.LINEAR;
                    } else if (filter === "NEAREST") {
                        glFilter = gl.NEAREST;
                    } else if (filter === "NEAREST_MIPMAP_NEAREST") {
                        glFilter = gl.NEAREST_MIPMAP_NEAREST;
                    } else if (filter === "LINEAR_MIPMAP_NEAREST") {
                        glFilter = gl.LINEAR_MIPMAP_NEAREST;
                    } else if (filter === "NEAREST_MIPMAP_LINEAR") {
                        glFilter = gl.NEAREST_MIPMAP_LINEAR;
                    } else if (filter === "LINEAR_MIPMAP_LINEAR") {
                        glFilter = gl.LINEAR_MIPMAP_LINEAR;
                    }

                    return glFilter;
                },

                getGLWrapMode: function(wrapMode) {
                    var gl = this.webGLContext;
                    var glWrapMode = gl.REPEAT;
                    if (wrapMode === "REPEAT") {
                        glWrapMode = gl.REPEAT;
                    } else if (wrapMode === "CLAMP_TO_EDGE") {
                        glWrapMode = gl.CLAMP_TO_EDGE;
                    } else if (wrapMode === "MIRROR_REPEAT") {
                        glWrapMode = gl.MIRROR_REPEAT;
                    }
                    return glWrapMode;
                },

                handleError: function(errorCode, info) {
                    // FIXME: report error
                    console.log("ERROR:textureDelegate:"+errorCode+" :"+info);
                },

                //nextHighestPowerOfTwo && isPowerOfTwo borrowed from http://www.khronos.org/webgl/wiki/WebGL_and_OpenGL_Differences
                nextHighestPowerOfTwo: function (x) {
                    --x;
                    for (var i = 1; i < 32; i <<= 1) {
                    x = x | x >> i;
                    }
                    return x + 1;
                },

                isPowerOfTwo: function(x) {
                    return (x & (x - 1)) == 0;
                },

                 //should be called only once
                convert: function (resource, ctx) {
                    var image = ctx;
                    var gl = this.webGLContext;
                    var canvas = document.createElement("canvas");
                    
                    //TODO: add compressed textures

                    //set default values
                    var minFilter = this.getGLFilter(resource.description.minFilter);
                    var maxFilter = this.getGLFilter(resource.description.maxFilter);
                    var wrapS = this.getGLWrapMode(resource.description.wrapS);
                    var wrapT = this.getGLWrapMode(resource.description.wrapT);

                    if ((wrapS === gl.REPEAT) || (wrapT === gl.REPEAT)) {
                        var width = parseInt(image.width);
                        var height = parseInt(image.height);

                        if (!this.isPowerOfTwo(width)) {
                            width = this.nextHighestPowerOfTwo(width);
                        }
                        if (!this.isPowerOfTwo(height)) {
                            height = this.nextHighestPowerOfTwo(height);
                        }
                        canvas.width = width;
                        canvas.height = height;

                    } else {
                        canvas.width = image.width;
                        canvas.height = image.height;
                    }

                    var graphicsContext = canvas.getContext("2d");
                    graphicsContext.drawImage(image, 0, 0, parseInt(canvas.width), parseInt(canvas.height));
                    canvas.id = image.id;
                    image = canvas;
                            
                    var texture = gl.createTexture(); 
                    gl.bindTexture(gl.TEXTURE_2D, texture);  
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, wrapS);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, wrapT);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, minFilter);  
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, maxFilter);  
                    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);  
                    if ((maxFilter === gl.NEAREST_MIPMAP_NEAREST) ||
                        (maxFilter === gl.LINEAR_MIPMAP_NEAREST) ||
                        (maxFilter === gl.NEAREST_MIPMAP_LINEAR) ||
                        (maxFilter === gl.LINEAR_MIPMAP_LINEAR))
                    {
                        gl.generateMipmap(gl.TEXTURE_2D);
                    }
                
                    gl.bindTexture(gl.TEXTURE_2D, null);  

                    return texture;
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
                //var worldMatrix = primitiveDescription.worldViewMatrix;
                //var projectionMatrix = this.projectionMatrix;
                  var primitive = primitiveDescription.primitive;
                var newMaxEnabledArray = -1;
                var gl = this.webGLContext;
                var program =  this.bindedProgram;
                var pass = primitive.material.technique.rootPass;
                var i;
                var currentTexture = 0;

                //----- bind uniforms -----
                var uniforms = pass.program.description.uniforms;
                var parameters = primitive.material.parameters;
                for (i = 0; i < uniforms.length ; i++) {
                    var uniform = uniforms[i];
                    var symbol = uniform.symbol;
                    if (uniform.semantic) {
                        if (uniform.semantic == this.PROJECTION) {
                            value = this.projectionMatrix;
                        } else 
                            value = primitiveDescription[uniform.semantic];
                    } else {
                        value = parameters[uniform.parameter];
                    }

                    var uniformIsSampler2D = program.getTypeForSymbol(symbol) === gl.SAMPLER_2D;
                    if (uniformIsSampler2D) {
                        var image = value;
                        this.textureDelegate.webGLContext = this.webGLContext;
                        var texture = this.resourceManager.getResource(image, this.textureDelegate, this.webGLContext);
                        if (texture) {
                            gl.activeTexture(gl.TEXTURE0 + currentTexture);
                            gl.bindTexture(gl.TEXTURE_2D, texture);
                            var samplerLocation = program.getLocationForSymbol(symbol);
                            if (typeof samplerLocation !== "undefined") {
                                program.setValueForSymbol(symbol, currentTexture);
                                currentTexture++;
                            }
                        }
                    } else {
                        program.setValueForSymbol(symbol, value);
                    }
                }

                program.commit(gl);
                
                var availableCount = 0;


                //----- bind attributes -----
                var attributes = pass.program.description.attributes;

                for (i = 0 ; i < attributes.length ; i++) {
                    var attribute = attributes[i];
                        
                    var symbol = attribute.symbol;
                    var semantic = attribute.semantic;
                    var accessor = primitive.semantics[semantic];

                    this.vertexAttributeBufferDelegate.webGLContext = this.webGLContext;
                    var glResource = this.resourceManager.getResource(  accessor, 
                                                                        this.vertexAttributeBufferDelegate,  { 
                                                                            "semantic": semantic, 
                                                                            "primitive": primitive
                                                                        });
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
                            gl.vertexAttribPointer(attributeLocation, accessor.componentsPerAttribute, gl.FLOAT, false, accessor.byteStride, 0);
                            if ( renderVertices && (semantic == "VERTEX")) {
                                gl.drawArrays(gl.POINTS, 0, accessor.count);
                            }
                        }
                        availableCount++;
                    } else {
                        this._lastMaxEnabledArray = -1;
                    }
                }                

                //-----


                var available = availableCount === primitive.attributesCount;
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
                        //if (!primitiveDescription.mesh.loaded) {
                         //    primitiveDescription.mesh.loadedPrimitivesCount++;
                            //}
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, glIndices);
                        gl.drawElements(gl.TRIANGLES, primitive.indices.count, gl.UNSIGNED_SHORT, 0);
                    }
                }
                this._lastMaxEnabledArray = newMaxEnabledArray;
                return available;
            }
        },

        programDelegate: {
            value: {
                handleError: function(errorCode, info) {
                    debugger;
                    // FIXME: report
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

        renderPrimitivesWithPass: {
            value: function(primitives, pass) {
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


    if(root) {
        root.Renderer = Renderer;
    }

    return Renderer;

}));