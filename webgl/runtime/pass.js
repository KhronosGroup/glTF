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

require("dependencies/gl-matrix");
var Node = require("node").Node;
var Projection = require("projection").Projection;
var Camera = require("camera").Camera;
var Utilities = require("utilities").Utilities;

exports.LinkedListNode = Object.create(Object.prototype, {

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

exports.LinkedList = Object.create(Object.prototype, {

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

exports.Pass = Object.create(Object.prototype, {

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

    _states: { value: null, writable: true },

    states: {
        get: function() {
            return this._states;
        },
        set: function(value) {
            this._states = value;
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

    _parentPass: { value: null, writable: true },

    parentPass: {
        get: function() {
            return this._parentPass;
        },
        set: function(value) {
            this._parentPass = value;
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
                        self.inputWillChange.call(self, property, value);
                        self.inputs[privateName] = value;
                        self.inputDidChange.call(self, property);
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

    addPass: {
        value: function(pass) {
            pass.parentPass = this;
            this.passes[pass.id] = pass;
        }
    },

    _updateSubPassesWithNodeIfNeeded: {
        value: function(node, parent) {
            //in the dag configuration a node has always the same children but not necessarly the same parent
            //so keep track of the parent to handle different matrices entries

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

                    var meshesCount = 0;
                    if (node.meshes) {
                        meshesCount = node.meshes.length;
                    }

                    if (meshesCount === 0) {
                        return;
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
                                                this.addPass(technique.rootPass);
                                                pass = technique.rootPass;
                                            }
                                            var primitiveInfo =
                                            {   "primitive" : primitive ,
                                                "node" : node,
                                                "mesh" : mesh,
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
            this.passes = {};
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

    isOpaque:  {
        value: function() {
            return !this.states ? true : (!this.states.BLEND);
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
                    var primitivesWithParent = null;
                    var worldMatrix = null;
                    var normalMatrix = null;

                    if (parent) {
                        if (primitives) {
                            primitivesWithParent = primitives[parent.id];

                            worldMatrix = primitivesWithParent.worldMatrix;
                            normalMatrix = primitivesWithParent.normalMatrix;
                        }
                    }

                    if (!worldMatrix) {
                        worldMatrix = mat4.create();
                    }
                    
                    if (!normalMatrix) {
                        normalMatrix = mat3.create();
                    }
                    var modelview = mat4.create();
                    mat4.multiply(parentTransform, node.transform , worldMatrix);
                    //mat4.multiply(cameraMatrix, modelview, worldMatrix);
                    
                    if (primitives)
                        mat3.transpose(mat4.toInverseMat3(worldMatrix), normalMatrix);
                    
                    return worldMatrix;
                    
                }, true, ctx);
            }
            /*
            if (this.passes) {
                if (Object.keys(this.passes).length > 0) {
                    console.log("*** ROOT PASS:"+Object.keys(this.passes));
                }
            }
*/
//            console.log("render Pass:"+this.id+" opaque:"+this.isOpaque());


            engine.renderer.renderPass(this);

            var nonOpaquePasses = [];
            if (this.passes) {

                var passesKey = Object.keys(this.passes);

                passesKey.forEach( function(passKey) {
                    var pass = this.passes[passKey];
                    if (!pass.isOpaque()) {
                        nonOpaquePasses.push(pass);
                    } else {
                        pass.execute(engine);
                    }
                }, this);

                nonOpaquePasses.forEach(function(transparentPass) {
                    transparentPass.execute(engine);
                }, this);
            }
        }
    },

    hitTest: {
        value: function(position, viewport, options) {

            if (this.inputs.scene && this.inputs.viewPoint) {
                var results = [];
                var cameraMatrix = mat4.create();
                var viewerMat =  this.inputs.viewPoint.transform;
                var viewer = vec3.createFrom(viewerMat[12],viewerMat[13], viewerMat[14]);
                var self = this;
                mat4.inverse(viewerMat, cameraMatrix);
                var origin = vec3.create();
                var dest = vec3.create();
                var camera = this.inputs.viewPoint.cameras[0];
                var screenSpaceVec1 = [position[0], viewport[3] - position[1],  camera.projection.znear];
                var screenSpaceVec2 = [position[0], viewport[3] - position[1],  camera.projection.zfar];

                var projectionMatrix = camera.projection.matrix;
                vec3.unproject(screenSpaceVec1, cameraMatrix, projectionMatrix, viewport, origin);
                vec3.unproject(screenSpaceVec2, cameraMatrix, projectionMatrix, viewport, dest);

                var X = 0; 
                var Y = 1; 
                var Z = 2; 
                var direction = vec3.create();
                var originTr = vec3.create();
                var directionTr = vec3.create();
                var ctx = mat4.identity();                  
                this.inputs.scene.rootNode.apply( function(node, parent, parentTransform) {
                    var modelMatrix = mat4.create();
                    var modelViewMatrix = mat4.create();
                    mat4.multiply( parentTransform, node.transform, modelMatrix);
                    mat4.multiply( cameraMatrix, modelMatrix, modelViewMatrix);

                    if (node.boundingBox) {
                        var modelMatrixInv = mat4.create();
                        mat4.inverse(modelMatrix, modelMatrixInv);

                        mat4.multiplyVec3(modelMatrixInv, origin, originTr);
                        mat4.multiplyVec3(modelMatrixInv, dest, directionTr);
                        vec3.subtract(directionTr, originTr);
                        vec3.normalize(directionTr);

                        var bbox = node.boundingBox;
                        if (Utilities.intersectsBBOX(bbox, [originTr , directionTr])) {
                            var meshes = node.meshes;
                            meshes.forEach( function(mesh) {
                                var box = mesh.boundingBox;
                                if (box) {
                                    if (Utilities.intersectsBBOX(box, [originTr , directionTr])) {
                                        Utilities.rayIntersectsMesh([originTr , directionTr], mesh, modelViewMatrix, results, options);
                                    }
                                }
                            }, this);
                            if (results.length > 0) {
                                results.sort( function(a,b) {
                                    
                                    var dist = vec3.create();
                                    vec3.subtract(a.intersection, viewer, dist);
                                    var d1 = vec3.dot(dist,dist);
                                    vec3.subtract(b.intersection, viewer, dist);
                                    var d2 = vec3.dot(dist,dist);
                                    return d1 - d2 }
                                );
                            }
                        }
                    }                            
                    return modelMatrix;

                }, true, ctx);
            }
            return results;
        }

    },

});