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

define(["backend/engine", "backend/utilities", "dependencies/gl-matrix"], function(Engine, Utilities) {

    var View = Object.create(Object, {

        _sceneBBox: { value: null, writable: true },
    
        sceneBBox: {
            get: function() { 
                return this._sceneBBox; 
            },
            set: function(value) { 
                this._sceneBBox = value; 
            }
        },


        _canvas: { value: null, writable: true },
    
        canvas: {
            get: function() { 
                return this._canvas; 
            },
            set: function(value) { 
                this._canvas = value; 
            }
        },

        _delegate: { value: null, writable: true },
    
        delegate: {
            get: function() { 
                return this._delegate; 
            },
            set: function(value) { 
                this._delegate = value; 
            }
        },

        _engine: { value: null, writable: true },
    
        engine: {
            get: function() { 
                return this._engine; 
            },
            set: function(value) { 
                this._engine = value; 
            }
        },

        scene: {
            get: function() { 
                if (this.engine) {
                    if (this.engine.rootPass) {
                        return this.engine.rootPass.scene;
                    }
                }
                return null;
            },
            
            set: function(value) { 
                if (this.engine) {
                    if (this.engine.rootPass) {
                        //compute hierarchical bbox for the whole scene
                        //this will be removed from this place when node bounding box become is implemented as hierarchical
                    
                    
                        var ctx = mat4.identity();
                        var node = value.rootNode.children[0];
                        //node.transform = mat4.identity();
                        var sceneBBox = value.rootNode.boundingBox;
                        value.rootNode.apply( function(node, parentTransform) {
                            var modelMatrix = mat4.create();
                            mat4.multiply( parentTransform, node.transform, modelMatrix);
                
                            if (node.boundingBox) {
                                var bbox = Utilities.transformBBox(node.boundingBox, parentTransform);
                                if (sceneBBox) {
                                    sceneBBox = Utilities.mergeBBox(bbox, sceneBBox);
                                } else {
                                    sceneBBox = bbox;
                                }
                            }                            
                                    
                            return modelMatrix;
                        }, true, ctx);
                        this.sceneBBox = sceneBBox;
                        this.engine.rootPass.scene = value;
                    }
                }
           }
        },
        
        init: {
            value: function(canvasId) {
        
                window.requestAnimationFrame = ( function() {
                    return  window.webkitRequestAnimationFrame ||
                    window.mozRequestAnimationFrame ||
                    window.oRequestAnimationFrame ||
                    window.msRequestAnimationFrame ||
                    function(callback) { 
                        window.setTimeout( callback, 1000 / 60, new Date()); };
                } )();
        
                // first thing, get gl context
                this.canvas = document.getElementById(canvasId);
                var webGLContext = this.canvas.getContext("experimental-webgl", { antialias: true}) ||this.canvas.getContext("webgl", { antialias: true});

                var options = null;
                this.engine = Object.create(Engine);
                this.engine.init(webGLContext, options);
                        
                var self = this;
                requestAnimationFrame(function() {self.draw.call(self)});
            }
        }, 
    
        getWebGLContext: {
            value: function() {
                if (this.engine) {
                    return this.engine.renderer.webGLContext;
                }
                return null;
            }
        },

        draw: {
            value: function() {
                
                if (this.delegate)
                    this.delegate.willDraw(this);
            
                if (this.scene) {
                    var renderer = this.engine.renderer;
                    var webGLContext = renderer.webGLContext;
                    if (webGLContext) {
                        // FIXME: cache this
                        var width = parseInt(this.canvas.getAttribute("width"));
                        var height = parseInt(this.canvas.getAttribute("height"));
                
                        webGLContext.viewport(0, 0, width, height);

                        webGLContext.depthFunc(webGLContext.LESS);
                        webGLContext.enable(webGLContext.DEPTH_TEST);
                        webGLContext.enable(webGLContext.CULL_FACE);

                        //webGLContext.clearColor(0.2, 0, 0, 1);
                       // webGLContext.clear(webGLContext.DEPTH_BUFFER_BIT | webGLContext.COLOR_BUFFER_BIT);

                        this.engine.render();

                        webGLContext.flush();
                
                        var error = webGLContext.getError();
                        if (error != webGLContext.NO_ERROR) {
                            console.log("gl error"+webGLContext.getError());
                        }
                    }
                }
                // FIXME: should be based on animations object or user interation
                var self = this;
                requestAnimationFrame(function() {self.draw.call(self)});
                if (this.delegate)
                    this.delegate.didDraw(this);
            }
        }    
    });
    
        return View;
    }
);
