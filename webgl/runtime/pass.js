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

    /*
                "defaultPass": {
                    "attributes": {
                        "normal": {
                            "semantic": "NORMAL"
                        },
                        "vert": {
                            "semantic": "VERTEX"
                        }
                    },
                    "states": {
                        "BLEND": false
                    },
                    "uniforms": {
                        "u_diffuseColor": "diffuseColor",
                        "u_mvMatrix": "WORLDVIEW",
                        "u_normalMatrix": "WORLDVIEWINVERSETRANSPOSE",
                        "u_projMatrix": "PROJECTION"
                    },
                    "x-shader/x-fragment": "lambert0Fs",
                    "x-shader/x-vertex": "lambert0Vs"
                }
    */

//-- Pass ---

var Pass = exports.Pass = Object.create(Object.prototype, {

    //constants
    PROGRAM: { value: "program", writable: false },
    SCENE: { value: "scene", writable: false },

    _type: { value: null, writable:true},

    type: {
        get: function() {
            return this._type;
        }
    },

    //_nodeIDToPrimitives: { value: null, writable: true},

    /*
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
    */

    /*
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

    */

    /*
    isOpaque:  {
        value: function() {
            return !this.states ? true : (!this.states.BLEND);
        }
    },
    */


    /*
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

    },*/

});

            /*
            if (this.passes) {
                if (Object.keys(this.passes).length > 0) {
                    console.log("*** ROOT PASS:"+Object.keys(this.passes));
                }
            }
            */
            //console.log("render Pass:"+this.id+" opaque:"+this.isOpaque());

            /*
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
            }*/

exports.ProgramPass = Object.create(Pass, {

    _attributes: { value: null, writable: true },
    _uniforms: { value: null, writable: true },
    _states: { value: null, writable: true },
    _program: { value: null, writable: true },

    attributes: {
        get: function() {
            return this._attributes;
        },
        set: function(value) {
            this._attributes = value;
        }
    },

    uniforms: {
        get: function() {
            return this._uniforms;
        },
        set: function(value) {
            this._uniforms = value;
        }
    },

    states: {
        get: function() {
            return this._states;
        },
        set: function(value) {
            this._states = value;
        }
    },

    program: {
        get: function() {
            return this._program;
        },
        set: function(value) {
            this._program = value;
        }
    },

    init: {
        value: function() {
            this.attributes = {};
            this.uniforms = {};
            this.states = {};
            this._type = Pass.ProgramPass;
            return this;
        }
    }

});

var SceneRenderer = exports.SceneRenderer = Object.create(Object.prototype, {

    _viewPoint: { value: null, writable: true },

    viewPoint: {
        get: function() {
            return this._viewPoint;
        },
        set: function(value) {
            if (this._viewPoint != value) {
                this._viewPoint = value;
            }
        }
    },

    _scene: { value: null, writable: true },

    sceneDidChange: {
        value: function() {    
            //Assign a view point from available nodes with camera if none
            var self = this;
            var cameraNodes = [];
            this.scene.rootNode.apply( function(node, parent) {
                if (node.cameras) {
                    if (node.cameras.length)
                        cameraNodes = cameraNodes.concat(node);
                }
                return null;
            } , true, null);

            // arbitry set first coming camera as the view point
            if (cameraNodes.length) {
                this.viewPoint = cameraNodes[0];
            } else {
                //TODO: make that a default projection method
                var projection = Object.create(Projection);
                projection.initWithDescription( {   "projection":"perspective", 
                                                    "yfov":45, 
                                                    "aspectRatio":1, 
                                                    "znear":0.1, 
                                                    "zfar":100});

                //create camera
                var camera = Object.create(Camera).init();
                camera.projection = projection;

                //create node to hold the camera
                var cameraNode = Object.create(Node).init();
                cameraNode.id = "__default_camera";
                cameraNode.cameras.push(camera);
                this.scene.rootNode.children.push(cameraNode);
                this.viewPoint = cameraNode;
            }
        }
    },

    render: {
        value: function(renderer) {

            var primitivesPerPass = {};

            if (this.scene) {
                var self = this;
                var ctx = mat4.create();
                mat4.inverse(this.viewPoint.transform, ctx);
                renderer.projectionMatrix = this.viewPoint.cameras[0].projection.matrix;
                
                this.scene.rootNode.apply( function(node, parent, parentTransform) {
                    var worldMatrix = mat4.create();

                    mat4.multiply(parentTransform, node.transform , worldMatrix);
                    
                    node.meshes.forEach( function(mesh) {
                        if (mesh.primitives) {
                            //compute normal matrix
                            var normalMatrix = mat3.create();
                            mat3.transpose(mat4.toInverseMat3(worldMatrix), normalMatrix);

                            //go through all primitives within all meshes
                            //TODO: cache all this
                            mesh.primitives.forEach( function (primitive) {
                                if (primitive.material) {
                                    var technique = primitive.material.technique;
                                    if (technique) {
                                        if (technique.rootPass) {
                                            var passUniqueID = technique.rootPass.id;
                                            var passWithPrimitives = primitivesPerPass[passUniqueID];
                                            if (!passWithPrimitives) {
                                                passWithPrimitives = primitivesPerPass[passUniqueID] = { 
                                                    "pass" : technique.rootPass,
                                                    "primitives" : [] 
                                                };
                                            }

                                            passWithPrimitives.primitives.push({   
                                                "primitive" : primitive ,
                                                "worldMatrix" : worldMatrix,
                                                "normalMatrix" : normalMatrix
                                            });
                                        }
                                    }
                                }
                            }, this);
                        }
                    }, this);

                    return worldMatrix;
                    
                }, true, ctx);
            }

            var keys = Object.keys(primitivesPerPass);
            keys.forEach( function(key) {
                var passWithPrimitives = primitivesPerPass[key];

                renderer.renderPrimitivesWithPass(passWithPrimitives.primitives, passWithPrimitives.pass);
            }, this);
        }
    },

    scene: {
        get: function() {
            return this._scene;
        },
        set: function(value) {
            if (this._scene != value) {
                this._scene = value;
                this.sceneDidChange();
            }
        }
    }

});

exports.ScenePass = Object.create(Pass, {

    createSceneRendererIfNeeded: {
        value: function() {
            if (!this._sceneRenderer) {
                this._sceneRenderer = Object.create(SceneRenderer);
            }
        }
    },

    _sceneRenderer: { value: null, writable: true },

    sceneRenderer: {
        get: function() {
            this.createSceneRendererIfNeeded();
            return this._sceneRenderer;
        },
        set: function(value) {
            this.createSceneRendererIfNeeded();
            if (this._sceneRenderer != value) {
                this._sceneRenderer = value;
            }
        }
    },

    viewPoint: {
        get: function() {
            return this.sceneRenderer ? this.sceneRenderer.viewPoint : null;
        }
    },

    scene: {
        get: function() {
            return this.sceneRenderer.scene;
        },
        set: function(value) {
            this.sceneRenderer.scene = value;
        }
    },

    execute: {
        value: function(renderer) {
            this.sceneRenderer.render(renderer);
        }
    },

    init: {
        value: function() {
            this._type = Pass.ScenePass;
        }
    }

});
