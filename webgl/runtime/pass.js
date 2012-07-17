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

    //--

    var LinkedListNode = Object.create(Object, {

        _content: { value: null, writable:true},

        content: {
            get: function() {
                return this._content;
            },
            set: function(value) {
                this._content = value;
            }
        },

        _previous: { value: null, writable:true},

        previous: {
            get: function() {
                return this._previous;
            },
            set: function(value) {
                this._previous = value;
            }
        },

        _next: { value: null, writable:true},

        next: {
            get: function() {
                return this._next;
            },
            set: function(value) {
                this._next = value;
            }
        },

        init: {
            value: function(content) {
                this.content = content;
                this.previous = null;
                this.next = null;
            }
        },

        removeFromList: {
            value: function() {
                if (this.previous) {
                    this.previous.next = this.next;
                }
                if (this.next) {
                    this.next.previous = this.previous;
                }
                this.next = null;
                this.previous = null;
            }
        }

    });

    var LinkedList = Object.create(Object, {

        _tail: { value: null, writable:true},

        tail: {
            get: function() {
                return this._tail;
            },
            set: function(value) {
                this._tail = value;
            }
        },

        _head: { value: null, writable:true},

        head: {
            get: function() {
                return this._head;
            },
            set: function(value) {
                this._head = value;
            }
        },

        append: {
            value: function(node) {
                if (!this.head) {
                    this.head = node;
                }
                if (this.tail) {
                    node.previous = this.tail;
                    this.tail.next = node;
                } 
                this.tail = node;
            }
        },

        remove: {
            value: function(node) {
                var id = node.content.id;

                var isTail = false,isHead = false;
                if (this.tail === node) {
                    isTail = true;
                    this.tail = node.previous;
                }

                if (this.head === node) {
                    isHead = true;
                    this.head = node.next;
                }

                //node.removeFromList();
                /* consistency check 
                for (cnode = this.head ; cnode != null ; cnode = cnode.next) {
                    if (id === cnode.content.id) {
                        console.log("ERROR: inconsistency found isTail:"+isTail+" isHead:"+isHead);
                    }
                }
                */
            }
        }

    });

    //-- Pass ---

    var Pass = Object.create(Object, {

        _nodeIDToPrimitives: { value: null, writable: true},

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

        _program: { value: null, writable: true },
    
        program: {
            get: function() {
                return this._program;
            },
            set: function(value) {
                this._program = value;
            }
        },

        _passes: { value: null, writable: true },
    
        passes: {
            get: function() {
                return this._passes;
            },
            set: function(value) {
                this._passes = value;
            }
        },

        _primitives: { value: null, writable: true },
    
        primitives: {
            get: function() {
                return this._primitives;
            },
            set: function(value) {
                this._primitives = value;
            }
        },

        _getPasses: {
            value: function(list) {
                var node = Object.create(LinkedListNode);
                if (this.primitives) {
                    node.init(this);
                    list.append(node);
                }

                //no dependency solving in passes yet, just forward straitghly to all of them
                if (this.passes) {
                    var passesKey = Object.keys(this.passes);
                    passesKey.forEach( function(key) {
                        var pass = this.passes[key];
                        pass._getPasses(list);
                    }, this);
                }
            }
        },

        getPasses: {
            value: function() {
                var list = Object.create(LinkedList);
                this._getPasses(list);
                return list;
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

        _updateSubPassesWithNodeIfNeeded: {
            value: function(node, parent) {
                //in the dag configuration a node has always the same children but not necessarly the same parent
                //so keep track of the parent to handle different matrices entries 
                var meshesCount = 0;
                if (node.meshes) {
                    meshesCount = node.meshes.length;
                }

                if (meshesCount === 0) {
                    return;
                }

                var primitives = this._nodeIDToPrimitives[node.id];
                if (!primitives) {
                    this._nodeIDToPrimitives[node.id] = primitives = {};
                }

                if (parent) {
                    var primitivesWithParent = primitives[parent.id];
                    if (!primitivesWithParent) {
                        primitives[parent.id] = primitivesWithParent = 
                        {   "worldMatrix" : mat4.create(), 
                            "normalMatrix" : mat3.create(),
                            "primitivesInfo" : []
                        }

                        node.meshes.forEach( function(mesh) {
                            if (mesh.primitives) {
                                mesh.primitives.forEach( function (primitive) {
                                    if (primitive.material) {
                                        var technique = primitive.material.technique;
                                        if (technique) {
                                            if (technique.rootPass) {
                                                var pass = this.passes[technique.rootPass.id];

                                                if (!pass) {
                                                    this.passes[technique.rootPass.id] = pass = technique.rootPass;
                                                }
                                                var primitiveInfo = 
                                                {   "primitive" : primitive , 
                                                    "worldMatrix" : primitivesWithParent.worldMatrix, 
                                                    "normalMatrix" : primitivesWithParent.normalMatrix 
                                                };  

                                                pass.primitives.push(primitiveInfo);

                                                //now dispatch in passes
                                                primitivesWithParent.primitivesInfo.push(primitiveInfo);
                                            }
                                        }
                                    }
                                }, this);
                            }
                        }, this);
                    }
                }
            }    
        },
    
        sceneDidChange: {
            value: function() {
                //TODO: perform this only when the scene is about to be used and just set a dirty flag intread
                this._nodeIDToPrimitives = {};
                var self = this;
                // search for all cameras
                var cameraNodes = [];
                this.inputs.scene.rootNode.apply( function(node, parent) {
                    self._updateSubPassesWithNodeIfNeeded(node, parent);

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
                this.primitives = [];
                this.passes = {};
                this.inputs = {};
                this._prepareInputsProperties();
                return this;
            }
        },
    
        execute: {
            value: function(engine) {  

                if (this.inputs.scene) {
                    var cameraMatrix = mat4.create();
                    var self = this;
                    mat4.inverse(this.inputs.viewPoint.transform, cameraMatrix);
                    var ctx = cameraMatrix;
                    engine.renderer.projectionMatrix = this.inputs.viewPoint.cameras[0].projection.matrix;
                    this.inputs.scene.rootNode.apply( function(node, parent, parentTransform) {
                        var primitives = self._nodeIDToPrimitives[node.id];
                        if (primitives && parent) {
                            var primitivesWithParent = primitives[parent.id];
                            mat4.multiply(parentTransform, node.transform , primitivesWithParent.worldMatrix);
                            mat3.transpose(mat4.toInverseMat3(primitivesWithParent.worldMatrix), primitivesWithParent.normalMatrix);
                            return primitivesWithParent.worldMatrix;
                        } else {
                            var worldMatrix = mat4.create();
                            mat4.multiply(parentTransform, node.transform , worldMatrix);
                            return worldMatrix;
                        }

                    }, true, ctx);
                }

                engine.renderer.renderPass(this);
                if (this.passes) {
                    var passesKey = Object.keys(this.passes);
                    passesKey.forEach( function(passKey) {
                        this.passes[passKey].execute(engine);
                    }, this);
                }
            }
        }

    });
    
        return Pass;
    }
);
