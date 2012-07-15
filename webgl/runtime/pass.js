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

/* 
    a pass has the following 3 key elements
        -> inputs  []  -> scene,viewpoint
        -> program
        -> outputs

        handleDidChange/handleWillChange inputs

        inputs -> program -> ouput (default to framebuffer)

        -> delegate , to give control to another client object
*/

define(["runtime/node", "runtime/projection", "runtime/resource-description", "runtime/camera", "runtime/glsl-program", "dependencies/gl-matrix"], 
    function(Node, Projection, ResourceDescription, Camera, GLSLProgram, glMatrix) {

    var Pass = Object.create(Object, {

        //TODO: outputs (i.e render target)
        _inputs: { value: null, writable: true },
    
        inputs: {
            get: function() {
                return this._inputs;
            },
            set: function(value) {
                this._inputs = value;
            }
        },

        inputWillChange: {
            value: function(name, newValue) {
            }
        },

        inputDidChange: {
            value: function(name) {
                if (name === "scene") {
                    this.sceneDidChange();
                }
            }
        },

        _addInputPropertyIfNeeded: {
            value: function(property) {
                var privateName = "_" + property;
                var self = this;
                if (this.inputs.hasOwnProperty(property) === false) {
                
                    Object.defineProperty(this.inputs, privateName, { writable:true , value: null });

                    Object.defineProperty(this.inputs, property, {
                        get: function() { return self.inputs[privateName]; },
                        set: function(value) {  
                            self.inputWillChange.call(self, property, value)                                            
                            self.inputs[privateName] = value;
                            self.inputDidChange.call(self, property)                
                        }
                    }); 
                }
            }
        },

        _prepareInputsProperties: {
            value: function() {
                var properties = ["scene", "viewPoint"];

                properties.forEach( function(property) {
                    this._addInputPropertyIfNeeded(property);
                }, this);
            }
        },
    
        sceneDidChange: {
            value: function() {
                // search for all cameras
                var cameraNodes = [];
                this.inputs.scene.rootNode.apply( function(node) {
                    if (node.cameras) {
                        if (node.cameras.length)
                            cameraNodes = cameraNodes.concat(node);
                    }
                    return null;
                } , true, null);

                // arbitry set first coming camera as the view point
                if (cameraNodes.length) {
                    this.inputs.viewPoint = cameraNodes[0];
                } else {
                    var projection = Object.create(Projection);
                    projection.initWithDescription( { "projection":"perspective", "yfov":45, "aspectRatio":1, "znear":0.1, "zfar":100});

                    var camera = Object.create(Camera);
                    camera.init();
                    camera.projection = projection;
    
                    // node
                    var cameraNode = Object.create(Node);
                    cameraNode.init();
                    cameraNode.id = "__default_camera";
                    cameraNode.cameras.push(camera);
                    if (cameraNode)
                        this.inputs.scene.rootNode.children.push(cameraNode);
                    this.inputs.viewPoint = cameraNode;
                }
            }
        },
        
        init: {
            value: function() {
                this.inputs = {};
                this._prepareInputsProperties();
                return this;
            }
        },
    
        execute: {
            value: function(engine) {  

                if (!this.inputs.viewPoint)
                    return;

                var projectionMatrix = this.inputs.viewPoint.cameras[0].projection.matrix;
                var cameraMatrix = mat4.create();
                var self = this;
                mat4.inverse(this.inputs.viewPoint.transform, cameraMatrix);
                var ctx = cameraMatrix;

                this.inputs.scene.rootNode.apply( function(node, parentTransform) {
                    //FIXME: ouch , remove these create !
                    var modelViewMatrix = mat4.create();

                    mat4.multiply(parentTransform, node.transform , modelViewMatrix);

                    //TODO: cache this 
                    var normalMatrix = mat3.transpose(mat4.toInverseMat3(modelViewMatrix));
                    if (node.meshes) {                    
                        node.meshes.forEach( function(mesh) {
                    
                            if (mesh.primitives) {
                                mesh.primitives.forEach( function (primitive) {                            

                                //TODO: create a pool for all those anonymous objects
                                engine.commandQueue.enqueuePrimitive({  "primitive" : primitive , 
                                                                        "worldMatrix" : modelViewMatrix, 
                                                                        "normalMatrix" : normalMatrix, 
                                                                        "projectionMatrix" : projectionMatrix });
                                }, this);
                            }
                        }, this);
                    }
                            
                    return modelViewMatrix;
                
                }, true, ctx);
         
            }
        }

    });
    
        return Pass;
    }
);
