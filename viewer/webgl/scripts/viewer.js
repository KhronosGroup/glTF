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

define( ["backend/utilities", "backend/node", "backend/camera", "backend/view", "backend/glsl-program", "backend/reader", "dependencies/gl-matrix"],
    function(Utilities, Node, Camera, View, GLSLProgram, Reader) {
        var Viewer = Object.create(Object, {
            run: {
                value: function(scenePath) {
                
                    var glView = Object.create(View);
                    glView.init("gl-canvas");    

                    var readerDelegate = {};
                    readerDelegate.readCompleted = function (key, value, userInfo) {
                        console.log("key:"+key+" value:"+value.length);
                        if (key === "entries") {
                            var scene = value[0];
                            glView.scene = scene;
                        } 
                    };
    
                    var angle = 0;    
                    var viewDelegate = {};
                    viewDelegate.willDraw = function(view) {
                        if (!view.scene)
                            return;
                    
                        /* ------------------------------------------------------------------------------------------------------------
                            draw gradient 
                         ------------------------------------------------------------------------------------------------------------ */        
                        view.engine.renderer.bindedProgram = null;

                        var orthoMatrix = mat4.ortho(-1, 1, 1.0, -1, 0, 1000);
                        var gl = view.getWebGLContext();
            
                        gl.disable(gl.DEPTH_TEST);
                        gl.disable(gl.CULL_FACE);
            
                        if (!this._gradientProgram) {
                            this._gradientProgram = Object.create(GLSLProgram);
                
                            var vertexShader =  "precision highp float;" +
                                                "attribute vec3 vert;"  +
                                                "attribute vec3 color;"  +
                                                 "uniform mat4 u_projMatrix; " +
                                                "varying vec3 v_color;"  +
                                                "void main(void) { " +
                                                "v_color = color;" +
                                                "gl_Position = u_projMatrix * vec4(vert,1.0); }" 
                
                            var fragmentShader =    "precision highp float;" +
                                                    "varying vec3 v_color;"  +
                                                    " void main(void) { " +
                                                    " gl_FragColor = vec4(v_color, 1.); }";
                
                            this._gradientProgram.initWithShaders( { "x-shader/x-vertex" : vertexShader , "x-shader/x-fragment" : fragmentShader } );
                            if (!this._gradientProgram.build(gl))
                                console.log(this._gradientProgram.errorLogs);
                       }
            
                       if (!this.vertexBuffer) {
                            /*
                                2/3----5
                                | \   |   
                                |  \  |
                                |   \ |
                                0----1/4
                            */
                            var vertices = [
                                -1.0,-1, 0.0,       1.0, 1.0, 1.0,
                                1.0,-1, 0.0,        1.0, 1.0, 1.0,
                                -1.0, 1.0, 0.0,     0.3, 0.4, 0.5,

                                -1.0, 1.0, 0.0,     0.3, 0.4, 0.5,
                                1.0,-1, 0.0,        1.0, 1.0, 1,
                                1.0, 1.0, 0.0,      0.3, .4, 0.5
                           ];

                            // Init the buffer
                            this.vertexBuffer = gl.createBuffer();
                            gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
                            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
                        }

                        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);

                        var attributeLocation = this._gradientProgram.getLocationForSymbol("vert");
                        if (typeof attributeLocation !== "undefined") {
                            gl.enableVertexAttribArray(attributeLocation);
                            gl.vertexAttribPointer(attributeLocation, 3, gl.FLOAT, false, 24, 0);
                        }
                        attributeLocation = this._gradientProgram.getLocationForSymbol("color");
                        if (typeof attributeLocation !== "undefined") {
                            gl.enableVertexAttribArray(attributeLocation);
                            gl.vertexAttribPointer(attributeLocation, 3, gl.FLOAT, false, 24, 12);
                        }
        
                        view.engine.renderer.bindedProgram = this._gradientProgram;
        
                        var projectionMatrixLocation = this._gradientProgram.getLocationForSymbol("u_projMatrix");
                        if (projectionMatrixLocation) {
                            this._gradientProgram.setValueForSymbol("u_projMatrix",orthoMatrix);
                        }
        
                        this._gradientProgram.commit(gl);
                        gl.drawArrays(gl.TRIANGLES, 0, 6);
                        gl.disableVertexAttribArray(attributeLocation);
                    };

                    viewDelegate.didDraw = function(view) {
                    };
    
                glView.delegate = viewDelegate;
    
                var reader = Object.create(Reader);
                reader.initWithPath(scenePath);
                reader.readEntries(["defaultScene"], readerDelegate, null);    
            }
        }
    });  
        return Viewer;
    }
);
