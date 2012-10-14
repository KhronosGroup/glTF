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
var Renderer = require("renderer").Renderer;

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

var Pass = exports.Pass = Object.create(Object.prototype, {

    //constants
    PROGRAM: { value: "program", writable: false },
    SCENE: { value: "scene", writable: false },

    _type: { value: null, writable:true},

    type: {
        get: function() {
            return this._type;
        }
    }

});

exports.ProgramPass = Object.create(Pass, {

    _attributes: { value: null, writable: true },
    _uniforms: { value: null, writable: true },
    _states: { value: null, writable: true },
    _program: { value: null, writable: true },

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


    _pathsInfosArray: { value: null, writable: true },

    _pathsInfos: { value: null, writable: true },

    _pathIDsForNodeID: { value: null, writable: true },

    _primitivesPerPass: { value: null, writable: true },

    _viewPoint: { value: null, writable: true },

    _scene: { value: null, writable: true },

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

    setupNodeAtPath: {
        value:function(node, pathID) {
            if (node.meshes) {
                node.meshes.forEach( function(mesh) {
                    if (mesh.primitives) {
                        //go through all primitives within all meshes
                        //TODO: cache all this
                        mesh.primitives.forEach( function (primitive) {
                            if (primitive.material) {
                                var technique = primitive.material.technique;
                                if (technique) {
                                    if (technique.rootPass) {
                                        var passUniqueID = technique.rootPass.id;
                                        var passWithPrimitives = this._primitivesPerPass[passUniqueID];
                                        if (!passWithPrimitives) {
                                            passWithPrimitives = this._primitivesPerPass[passUniqueID] = { 
                                                "pass" : technique.rootPass,
                                                "primitives" : [] 
                                            };
                                        }

                                        var pathInfo = this._pathsInfos[pathID];
                                        if (pathInfo) {
                                            var WORLD = Renderer.WORLD;
                                            var WORLDVIEW = Renderer.WORLDVIEW;
                                            var WORLDVIEWINVERSETRANSPOSE = Renderer.WORLDVIEWINVERSETRANSPOSE;

                                            var renderPrimitive = {};
                                            renderPrimitive["primitive"] = primitive;
                                            renderPrimitive[WORLD] = pathInfo[WORLD];
                                            renderPrimitive[WORLDVIEWINVERSETRANSPOSE] = pathInfo[WORLDVIEWINVERSETRANSPOSE];
                                            renderPrimitive[WORLDVIEW] = pathInfo[WORLDVIEW];

                                            passWithPrimitives.primitives.push(renderPrimitive);
                                        }
                                    }
                                }
                            }
                        }, this);
                    }
                }, this);
            }
        }
    },

    getPathIDsForNodeID: {
        value: function(nodeID) {
            return this._pathIDsForNodeID[nodeID];
        }
    },    

    addPathIDForNodeID: {
        value: function(nodeID, pathID) {
            var pathIDs = this.pathIDsForNodeID;
            if (!pathIDs) {
                pathIDs = [];
                this._pathIDsForNodeID[nodeID] = pathIDs;
            }

            pathIDs.push(pathID);
        }
    },

    sceneDidChange: {
        value: function() {
            //prepares all infos
            this._pathsInfos = {};
            this._pathIDsForNodeID = {};
            this._primitivesPerPass = {};
            this._pathsInfosArray = [];

            //TODO: expose list of nodes / cameras / light / material
            var nodes = {};

            //Assign a view point from available nodes with camera if none
            var self = this;
            var cameraNodes = [];
            var WORLD = Renderer.WORLD;
            var WORLDVIEW = Renderer.WORLDVIEW;
            var WORLDVIEWINVERSETRANSPOSE = Renderer.WORLDVIEWINVERSETRANSPOSE;

            var context = {};
            context["path"] = [];
            context[WORLD] = mat4.identity();
            var pathCount = 0;

            this.scene.rootNode.apply( function(node, parent, context) {
                var worldMatrix = mat4.create();

                mat4.multiply(context[WORLD], node.transform , worldMatrix);

                var path = context.path.concat([node.id]); 
                var pathID = path.join('-');

                nodes[node.id] = node;

                var pathInfos = {};
                pathInfos[WORLD] = worldMatrix;
                pathInfos[WORLDVIEWINVERSETRANSPOSE] = mat3.create();
                pathInfos[WORLDVIEW] = mat4.create();
                pathInfos["path"] = path;

                self.addPathIDForNodeID(node.id, pathID);
                self._pathsInfos[pathID] = pathInfos;
                self._pathsInfosArray[pathCount++] = pathInfos;
                self.setupNodeAtPath(node, pathID);

                if (node.cameras) {
                    if (node.cameras.length)
                        cameraNodes = cameraNodes.concat(node);
                }

                var newContext = {};
                newContext["path"] = path;
                newContext[WORLD] = worldMatrix;

                return newContext;
            } , true, context);

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

            if (!this.scene)
                return;

            //set projection matrix
            renderer.projectionMatrix = this.viewPoint.cameras[0].projection.matrix;

            //get view matrix
            var viewMatrix = mat4.create();
            mat4.inverse(this.viewPoint.transform, viewMatrix);

            //to be cached
            var count = this._pathsInfosArray.length;

            if (this.viewPoint.flipped) {
                var translationMatrix = mat4.translate(mat4.identity(), [0, 0, 0 ]);
                var scaleMatrix = mat4.scale(translationMatrix, [1, 1, -1]);
                mat4.multiply(viewMatrix, scaleMatrix, viewMatrix) ;
            }

            for (var i = 0 ; i < count ; i++) {
                var pathInfos = this._pathsInfosArray[i];
                var worldMatrix = pathInfos[renderer.WORLD];
                var worldViewMatrix = pathInfos[renderer.WORLDVIEW];
                var normalMatrix = pathInfos[renderer.WORLDVIEWINVERSETRANSPOSE];
                mat4.multiply(viewMatrix, worldMatrix, worldViewMatrix);
                mat4.toInverseMat3(worldViewMatrix, normalMatrix);
                mat3.transpose(normalMatrix);
            }

            var nonOpaquePassesWithPrimitives = [];
            var keys = Object.keys(this._primitivesPerPass);
            keys.forEach( function(key) {
                var passWithPrimitives = this._primitivesPerPass[key];
                var states = passWithPrimitives.pass.states;
                if (states.BLEND) {
                    nonOpaquePassesWithPrimitives.push(passWithPrimitives);
                } else {
                    renderer.renderPrimitivesWithPass(passWithPrimitives.primitives, passWithPrimitives.pass);
                }
            }, this);

            nonOpaquePassesWithPrimitives.forEach( function(passWithPrimitives) {
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
    },

    init: {
        value: function() {
            return this;
        }
    }

});

exports.ScenePass = Object.create(Pass, {

    _sceneRenderer: { value: null, writable: true },

    createSceneRendererIfNeeded: {
        value: function() {
            if (!this._sceneRenderer) {
                this._sceneRenderer = Object.create(SceneRenderer).init();
            }
        }
    },

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


